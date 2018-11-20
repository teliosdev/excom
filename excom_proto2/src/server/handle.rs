use super::Act;
use crate::error::*;
use futures::sync::mpsc::UnboundedSender;

#[derive(Debug, Clone)]
pub struct Handle(UnboundedSender<Act>);

impl Handle {
    pub(crate) fn new(send: UnboundedSender<Act>) -> Handle {
        Handle(send)
    }

    pub fn stop(&self) -> Result<()> {
        self.0
            .unbounded_send(Act::Stop)
            .chain_err(|| ErrorKind::ControlServerError("could not send stop signal"))
    }
}
