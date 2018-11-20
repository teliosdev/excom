use crate::errors::{Error, ErrorKind, ResultExt};
use excom_proto::{tls, Error as ProtoError, ErrorKind as ProtoErrorKind, Message, Packet};
use futures::sync::mpsc::UnboundedSender;
use futures::sync::oneshot::{channel, Sender};
use std::collections::HashMap;
use std::sync::atomic::{AtomicUsize, Ordering};
use std::sync::{Arc, Mutex};
use tokio::net::TcpStream;
use tokio::prelude::*;
use tokio_tls::TlsStream;

mod drive;
mod resolv;

use self::drive::drive;
pub use self::drive::Drive;

pub struct Client(
    UnboundedSender<Message>,
    pub(super) Arc<Mutex<HashMap<usize, Sender<Message>>>>,
    AtomicUsize,
);

impl Client {
    pub fn new(
        domain: String,
        addr: String,
        allow_invalid: bool,
    ) -> impl Future<Item = (Client, Drive<impl Stream, impl Sink>), Error = Error> {
        connect(domain, addr, allow_invalid).map(|conn| {
            let (send, wait, drive) = drive(conn);
            (Client(send, wait, AtomicUsize::new(0)), drive)
        })
    }

    pub fn send(&self, message: Message) -> impl Future<Item = Message, Error = Error> {
        let mut wait = self.1.lock().unwrap();
        let (send, recv) = channel();
        info!("pushing send, recv pair into waitlist...");
        wait.insert(message.id, send).map(|v| {
            error!("previous value in waitlist detected: {:?}", v);
        });
        info!("waitlist: {:?}", *wait);
        let l = self.1.clone();
        self.0
            .unbounded_send(message)
            .chain_err(|| ErrorKind::SyncError)
            .into_future()
            .and_then(move |_| {
                info!("recv waitlist: {:?}", *l.lock().unwrap());
                recv.map_err(|e| Error::with_chain(e, ErrorKind::SyncError))
            })
    }

    pub fn call(&self, packet: Packet) -> impl Future<Item = Message, Error = Error> {
        let id = self.2.fetch_add(1, Ordering::SeqCst);
        let message = Message::new(id, packet);
        self.send(message)
    }
}

fn connect(
    domain: String,
    addr: String,
    allow_invalid: bool,
) -> impl Future<Item = TlsStream<TcpStream>, Error = Error> {
    tls::connector(allow_invalid)
        .map_err(Error::from)
        .into_future()
        .and_then(move |conn| {
            resolv::resolve(&addr)
                .ok_or_else(|| Error::from(ErrorKind::InvalidAddressError))
                .into_future()
                .and_then(|addr| {
                    TcpStream::connect(&addr)
                        .map_err(ProtoError::from)
                        .map_err(Error::from)
                        .and_then(move |s| {
                            conn.connect(&domain, s)
                                .map_err(|e| Error::with_chain(e, ProtoErrorKind::TlsError))
                        })
                })
        })
}
