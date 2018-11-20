use super::sio::{Act, ActFuture, Client};
use excom_proto::{Message, Packet};
use std::collections::HashMap;
use std::sync::RwLock;
use tokio::prelude::*;

pub struct Direct {
    clients: RwLock<HashMap<usize, Client>>,
}

impl Direct {
    pub fn new() -> Direct {
        Direct {
            clients: RwLock::new(HashMap::new()),
        }
    }
}

impl Act for Direct {
    fn connect(&self, client: Client) -> ActFuture {
        warn!("client connected!");
        Box::new(
            self.clients
                .write()
                .map(|mut result| result.insert(client.id(), client))
                .map(|_| ())
                .map_err(|_| ())
                .into_future(),
        )
    }

    fn disconnect(&self, client: Client) -> ActFuture {
        warn!("client disconnected!");
        Box::new(
            self.clients
                .write()
                .map(|mut result| result.remove(&client.id()))
                .map(|_| ())
                .map_err(|_| ())
                .into_future(),
        )
    }

    fn message(&self, client: Client, message: Message) -> ActFuture {
        warn!("client sent a message!");
        match message.content {
            Packet::Ping => Box::new(
                client
                    .send(message.reply(Packet::Pong))
                    .map_err(|_| ())
                    .into_future(),
            ),

            _ => Box::new(Err(()).into_future()),
        }
    }
}
