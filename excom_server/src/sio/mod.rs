use crate::configuration::ServerConfiguration;
use crate::errors::{Error, ErrorKind, ResultExt};
use excom_proto;
use excom_proto::ExcomCodec;
use futures::sync::mpsc;
use std::sync::atomic::{AtomicUsize, Ordering};
use std::sync::Arc;
use std::sync::Mutex;
use tokio::codec::{FramedRead, FramedWrite};
use tokio::net::tcp::*;
use tokio::prelude::*;
use tokio_tls::TlsStream;

mod act;
mod client;

pub use self::act::{Act, ActFuture};
pub use self::client::Client;

pub fn stream(
    config: &ServerConfiguration,
) -> Result<impl Stream<Item = TlsStream<TcpStream>, Error = Error>, Error> {
    let listener = TcpListener::bind(&config.address).chain_err(|| ErrorKind::IoError)?;
    let acceptor = excom_proto::tls::acceptor(&config.tls)?;

    // We'll set up a stream here.  First, we set up a tcplistener on
    // the given address.  This should pass any new connections here,
    // through incoming.
    Ok(listener
        .incoming()
        // If we get any errors while doing so, we'll treat it properly.
        .map_err(|e| Error::with_chain(e, ErrorKind::IoError))
        .and_then(move |incoming| {
            // Then, for every incoming connection, we'll ask the TLS
            // provider to handle setting up the TLS connection.  Once
            // that's done, it'll move onto the next stage, whatever
            // that may be.  If there's an error, we'll note it.
            acceptor
                .accept(incoming)
                .map_err(|e| Error::with_chain(e, ErrorKind::TlsError))
        }))
}

pub struct TcpServer<S, A>
where
    S: Stream<Item = TlsStream<TcpStream>, Error = Error> + Send + Sync + 'static,
    A: Act + 'static,
{
    stream: S,
    act: A,
    count: AtomicUsize,
}

impl<S, A> TcpServer<S, A>
where
    S: Stream<Item = TlsStream<TcpStream>, Error = Error> + Send + Sync + 'static,
    A: Act + 'static,
{
    pub fn new(stream: S, act: A) -> TcpServer<S, A> {
        TcpServer {
            stream,
            act,
            count: AtomicUsize::new(0),
        }
    }

    pub fn anchor(self) -> impl Future<Item = (), Error = ()> {
        // first things first: put our actor into an Arc, because we're
        // going to be sharing this across threads.  We know our actor
        // is thread-safe because it implements Send+Sync+'static.
        let act = Arc::new(self.act);
        // We also need to take ownership of the counter, because we're
        // about to take ownership of the stream, and can't move self
        // into the closure.
        let count = self.count;

        self.stream
            .map_err(|err| {
                error!("received error from tls acceptor: {:?}", err);
                ()
            })
            .for_each(move |incoming| {
                info!("accepting incoming connection...");
                // now, we iterate over each incoming stream.  At this point,
                // we have properly encapsulated it in a TLS stream, and so
                // we don't need to do anything special here.
                // let act = act.clone();
                // We'll split the stream into read, write pairs, in order
                // to be able to read from and write to the stream separately...
                let (read, write) = incoming.split();
                // reader.
                let reader = FramedRead::new(read, ExcomCodec::new());
                // writer.
                let writer = FramedWrite::new(write, ExcomCodec::new());
                // increment the id counter for the current client.  They
                // get a new ID.
                let id = count.fetch_add(1, Ordering::SeqCst);
                // next, we create a send buffer for the client.  Each
                // message that has to be sent will be placed in this buffer,
                // and...
                let (send, recv) = mpsc::unbounded();
                let forward = recv
                    // every message we recieve in the buffer, we'll send
                    // off to be written to the socket.  This creates a
                    // future that completes when the receiver is done.
                    // Idealistically, this would only happen once all the
                    // references to the sender in this set are done.
                    .forward(writer.sink_map_err(|err| {
                        error!("recieved error from client socket writer: {:?}", err);
                        ()
                    }))
                    .map(|data| Mutex::new(data))
                    .shared();
                // We'll need this for the closure later.  We'll have to
                // shut down the forwarding system somehow.
                let fcl = forward.clone();

                // create a new client object with this sender.
                let client = Client::new(Arc::new(send), id);
                // we need to clone both of these for the message closure,
                // since it *has* to take ownership.  The closure cannot
                // guarentee that any references it takes will live as long
                // as the closure, so this has to be done for both closures.
                let mact = (client.clone(), act.clone());
                let dact = (client.clone(), act.clone());

                // Notify the actor we've got a connection.
                act.connect(client.clone());
                let future = reader
                    // If we get an error from the reader, we'll map it to
                    // a protocol error.  This bypasses the for_each, and
                    // goes straight to the or_else, and by extension, to
                    // the disconnect.
                    .map_err(|err| {
                        error!("recieved error from client socket reader: {:?}", err);
                        ()
                    })
                    // Then, for every message we recieve, notify the actor
                    // that we've recieved a message.
                    .for_each(move |message| mact.1.message(mact.0.clone(), message))
                    // If the actor returns an error, or we receive an error
                    // from the protocol line, we'll need to handle it.
                    // TODO: Maybe we should log it here.
                    .or_else(|err| {
                        error!("received error from message handler: {:?}", err);
                        Ok(())
                    })
                    // Then, notify the actor of a disconnect.
                    .and_then(move |_| dact.1.disconnect(dact.0.clone()))
                    .map_err(|err| {
                        error!("received error from disconnect handler: {:?}", err);
                        ()
                    })
                    // Next, we'll join the future into ours, as we need to
                    // close the read/write socket.
                    .and_then(move |_| fcl.map_err(|_| ()))
                    .and_then(|si| {
                        // We'll close the channel, to prevent any more
                        // sending to it.
                        si.lock().unwrap().0.close();
                        // Then, we'll continually try to shut down the
                        // socket using this poll.  It's a bit messy, but...
                        future::poll_fn(move || {
                            // First, we lock the mutex.  We have a mutex
                            // in place because we can't guarentee that we
                            // have exclusive access to this.
                            si.lock()
                                // If there's an error, ignore it.  There's
                                // nothing we can do, and it's better if we
                                // do this.
                                .unwrap()
                                // Grab the MapErr instance inside the
                                // mutex. We have a MapErr because we called
                                // MappErr on the FrameWrite in constructing
                                // the future earlier.
                                .1
                                // Next, we'll grab the FramedWrite from
                                // inside the MapErr...
                                .get_mut()
                                // Then the WriteHalf inside of that.
                                .get_mut()
                                // We'll shut that down, or at least try to,
                                .shutdown()
                                // And ignore any errors in the process.
                                .map_err(|_| ())
                        })
                    });

                // Spawn the forwarding into a separate thread, and join it
                // with the message handling in another separate thread.
                // The future will only complete once both are completed.
                //
                // This is a problem, because an error could force a
                // disconnect, but the client may keep the line open.  We
                // have to find a way to force a connection close on
                // disconnect.
                tokio::spawn(forward.clone().then(|_| Ok(())))
                    .into_future()
                    .join(tokio::spawn(future))
                    .map(|_| ())
            })
    }
}
