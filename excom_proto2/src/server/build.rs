use super::service::Service;
use super::{Act, Server};
use crate::error::*;
use native_tls::{Identity, Protocol, TlsAcceptor};
use std::net::SocketAddr;
use tokio::net::TcpListener;
use tokio::prelude::*;
use tokio_tls::TlsAcceptor as TlsAsyncAcceptor;

pub struct ServerBuilder<S: Service> {
    service: S,
    addr: Option<SocketAddr>,
    ttl: Option<u32>,
    minver: Protocol,
    maxver: Protocol,
    identity: Option<Identity>,
}

impl<S: Service> ServerBuilder<S> {
    /// Create a new builder.
    pub fn from_service(service: S) -> ServerBuilder<S> {
        ServerBuilder {
            service,
            addr: None,
            ttl: None,
            minver: Protocol::Tlsv11,
            maxver: Protocol::Tlsv12,
            identity: None,
        }
    }

    /// Pushes addresses to listen to when the server starts.  All
    /// connections from all addresses will result in the same handling;
    /// this just allows us to listen on two different sockets at once.
    ///
    /// # Example
    /// ```rust
    /// # fn main() -> Result<(), Box<Error>> {
    /// let build = ServerBuilder::new(());
    /// build.listen("localhost:3000".parse().unwrap())?;
    /// # }
    pub fn listen(&mut self, addr: SocketAddr) -> &mut Self {
        self.addr = Some(addr);
        self
    }

    /// Sets the TTL for the TCP socket.  Most often, this won't be
    /// needed, but can be used to help with realtime applications.
    ///
    /// # Example
    /// ```rust
    /// # fn main() -> Result<(), Box<Error>> {
    /// let build = ServerBuilder::new(());
    /// build.set_ttl(500);
    /// # }
    pub fn set_ttl(&mut self, ttl: u32) -> &mut Self {
        self.ttl = Some(ttl);
        self
    }

    /// Sets the minimum protocol version allowed for TLS.  This should
    /// at _least_ be TLSv11, as anything less may be insecure.  The
    /// ideal value would be TLSv12.  This defaults to TLSv11.
    pub fn tls_minimum_protocol_version(&mut self, ver: Protocol) -> &mut Self {
        self.minver = ver;
        self
    }

    /// Sets the maximum protocol version allowed for TLS.  This should
    /// be as high as possible, which at the time of writing is TLSv12.
    /// This defaults to TLSv12.
    pub fn tls_maximum_protocol_version(&mut self, ver: Protocol) -> &mut Self {
        self.maxver = ver;
        self
    }

    /// Sets the TLS identity.  This should be the certificate the
    /// server will use to identify itself to the client.
    pub fn tls_identity(&mut self, identity: Identity) -> &mut Self {
        self.identity = Some(identity);
        self
    }

    pub fn build(self) -> Result<Server<S>> {
        let identity: Identity = self
            .identity
            .ok_or_else(|| ErrorKind::InitServerError("missing identity information"))?;
        let acceptor: TlsAsyncAcceptor = TlsAcceptor::builder(identity)
            .min_protocol_version(Some(self.minver))
            .max_protocol_version(Some(self.maxver))
            .build()
            .chain_err(|| ErrorKind::InitServerError("tls acceptor invalid"))?
            .into();
        let addr = self
            .addr
            .ok_or_else(|| ErrorKind::InitServerError("no address to bind to"))?;
        let listener = TcpListener::bind(&addr)
            .chain_err(|| ErrorKind::InitServerError("could not bind to address"))?;

        match self.ttl {
            Some(ttl) => listener
                .set_ttl(ttl)
                .chain_err(|| ErrorKind::InitServerError("invalid ttl"))?,
            None => {}
        }

        let (send, recv) = ::futures::sync::mpsc::unbounded::<Act>();

        let stream = listener
            .incoming()
            .map_err(|e| Error::with_chain(e, ErrorKind::ListenError))
            .and_then(move |incoming| {
                acceptor
                    .accept(incoming)
                    .map_err(|e| Error::with_chain(e, ErrorKind::AcceptTlsError))
            })
            .map(Act::Connect)
            .select(recv.map_err(|_| unreachable!()));

        Ok(Server::new(self.service, stream, send))
    }
}
