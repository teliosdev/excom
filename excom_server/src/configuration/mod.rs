use serde_derive::*;

mod server;

pub use self::server::ServerConfiguration;

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct Configuration {
    pub server: ServerConfiguration,
}
