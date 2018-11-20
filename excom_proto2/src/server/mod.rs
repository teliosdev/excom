use crate::error::*;
use futures::sync::mpsc::UnboundedSender;
use tokio::executor::{DefaultExecutor, Executor};
use tokio::net::TcpStream;
use tokio::prelude::*;
use tokio_tls::TlsStream;

mod build;
mod handle;
mod service;

pub use self::build::ServerBuilder;
pub use self::handle::Handle;
pub use self::service::Service;

#[derive(Debug)]
pub(crate) enum Act {
    Connect(TlsStream<TcpStream>),
    Stop,
}

pub struct Server<S>
where
    S: Service,
{
    service: S,
    stream: Box<dyn Stream<Item = Act, Error = Error> + Send + Sync + 'static>,
    insert: UnboundedSender<Act>,
}

impl<S> Server<S>
where
    S: Service,
{
    pub(crate) fn new<T>(service: S, stream: T, insert: UnboundedSender<Act>) -> Server<S>
    where
        T: Stream<Item = Act, Error = Error> + Send + Sync + 'static,
    {
        Server {
            service,
            stream: Box::new(stream),
            insert,
        }
    }

    pub fn builder(service: S) -> ServerBuilder<S> {
        ServerBuilder::from_service(service)
    }

    pub fn drive(self) -> Result<Handle>
    where
        S: 'static,
    {
        self.drive_onto(DefaultExecutor::current())
    }

    pub fn drive_onto(self, mut exec: impl Executor) -> Result<Handle>
    where
        S: 'static,
    {
        let handle = Handle::new(self.insert);
        exec.spawn(Box::new(drive_stream(self.service, self.stream)))
            .map(|_| handle.clone())
            .map_err(|_| ErrorKind::DriveServerError.into())
    }
}

fn drive_stream<S: Service>(
    service: S,
    stream: impl Stream<Item = Act, Error = Error>,
) -> impl Future<Item = (), Error = ()> {
    stream.for_each(|_incoming| Ok(())).map_err(|_| ())
}
