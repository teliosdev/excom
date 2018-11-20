use crate::errors::Error;
use excom_proto::{Error as ProtoError, ErrorKind as ProtoErrorKind, ExcomCodec, Message};
use futures::sync::mpsc::{unbounded, UnboundedSender};
use futures::sync::oneshot::Sender;
use std::collections::HashMap;
use std::sync::{Arc, Mutex};
use tokio::codec::{FramedRead, FramedWrite};
use tokio::io::{ReadHalf, WriteHalf};
use tokio::net::TcpStream;
use tokio::prelude::*;
use tokio_tls::TlsStream;

pub struct Drive<Stream, Sink>(
    Box<dyn Future<Item = (Stream, Sink), Error = Error> + Send + 'static>,
);

impl<Stream, Sink> Future for Drive<Stream, Sink> {
    type Item = (Stream, Sink);
    type Error = Error;

    fn poll(&mut self) -> Result<Async<(Stream, Sink)>, Error> {
        self.0.poll()
    }
}

pub(super) fn drive(
    client: TlsStream<TcpStream>,
) -> (
    UnboundedSender<Message>,
    Arc<Mutex<HashMap<usize, Sender<Message>>>>,
    Drive<impl Stream, impl Sink>,
) {
    let (read, write) = client.split();
    let reader = FramedRead::new(read, ExcomCodec::new());
    let writer = FramedWrite::new(write, ExcomCodec::new());
    let wait = Arc::new(Mutex::new(HashMap::new()));
    let (send, wf) = drive_write(writer);
    (
        send,
        wait.clone(),
        Drive(Box::new(
            drive_read(reader, wait.clone()).join(wf).map(|(_, v)| v),
        )),
    )
}

fn drive_read(
    reader: FramedRead<ReadHalf<TlsStream<TcpStream>>, ExcomCodec>,
    wait: Arc<Mutex<HashMap<usize, Sender<Message>>>>,
) -> impl Future<Item = (), Error = Error> {
    reader
        .map_err(Error::from)
        .for_each(move |message| match wait.lock() {
            Ok(mut wait) => {
                let _ = wait.remove(&message.id).map(|value| {
                    value.send(message).map_err(|_| {
                        error!("receiver was dropped!");
                        ()
                    })
                });
                Ok(())
            }
            _ => {
                error!("lock acquisition failed!");
                Ok(())
            }
        })
}

fn drive_write(
    write: FramedWrite<WriteHalf<TlsStream<TcpStream>>, ExcomCodec>,
) -> (
    UnboundedSender<Message>,
    impl Future<Item = (impl Stream, impl Sink), Error = Error> + Send + 'static,
) {
    let (send, recv) = unbounded();
    let future = recv
        .map_err(|_| ProtoError::from(ProtoErrorKind::ProtocolError))
        .map_err(Error::from)
        .forward(write);

    (send, future)
}
