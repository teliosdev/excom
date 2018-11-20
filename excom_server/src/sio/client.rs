use crate::errors::{Error, ErrorKind, ResultExt};
use excom_proto::Message;
use futures::sync::mpsc::UnboundedSender;
use std::cmp::Ordering;
use std::hash::{Hash, Hasher};
use std::sync::Arc;

#[derive(Debug, Clone)]
pub struct Client(Arc<UnboundedSender<Message>>, usize);

impl Client {
    pub fn new(sender: Arc<UnboundedSender<Message>>, id: usize) -> Client {
        Client(sender, id)
    }

    pub fn sender(&self) -> &UnboundedSender<Message> {
        self.0.as_ref()
    }
    pub fn id(&self) -> usize {
        self.1
    }

    pub fn send(&self, message: Message) -> Result<(), Error> {
        self.sender()
            .unbounded_send(message)
            .chain_err(|| ErrorKind::SendFailedError)
    }
}

impl PartialEq for Client {
    fn eq(&self, other: &Self) -> bool {
        self.1 == other.1
    }
}

impl Eq for Client {}

impl PartialOrd for Client {
    fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
        self.1.partial_cmp(&other.1)
    }
}

impl Ord for Client {
    fn cmp(&self, other: &Self) -> Ordering {
        self.1.cmp(&other.1)
    }
}

impl Hash for Client {
    fn hash<H: Hasher>(&self, hasher: &mut H) {
        self.1.hash(hasher)
    }
}
