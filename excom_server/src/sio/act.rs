use super::Client;
use excom_proto::Message;
use tokio::prelude::*;

pub trait Act: Send + Sync {
    fn connect(&self, client: Client) -> ActFuture;
    fn disconnect(&self, client: Client) -> ActFuture;
    fn message(&self, client: Client, message: Message) -> ActFuture;
}

pub type ActFuture = Box<dyn Future<Item = (), Error = ()> + Send + Sync + 'static>;
