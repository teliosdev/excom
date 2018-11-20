use crate::packet::Packet;

#[derive(Debug, Clone, PartialEq, Eq, Hash, Serialize, Deserialize)]
pub struct Message {
    pub id: usize,
    pub content: Packet,
}

impl Message {
    pub fn new(id: usize, content: Packet) -> Message {
        Message { id, content }
    }

    pub fn reply(&self, content: Packet) -> Message {
        Message {
            id: self.id,
            content,
        }
    }
}
