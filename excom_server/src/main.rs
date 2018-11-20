#[macro_use]
extern crate log;
use std::error::Error as StdError;
use std::fs::File;
use std::io::Read;
use std::result::Result as StdResult;
use toml;

pub mod configuration;
pub mod direct;
mod errors;
pub mod sio;

use self::configuration::Configuration;
pub use self::errors::*;

fn main() -> StdResult<(), Box<dyn StdError>> {
    env_logger::init();
    info!("loaading configuration...");
    let mut file = File::open("config.toml")?;
    let mut content = vec![];
    file.read_to_end(&mut content)?;
    let config: Configuration = toml::from_slice(&content)?;

    info!("setting up processes...");
    let direct = self::direct::Direct::new();
    let stream = self::sio::stream(&config.server)?;
    let server = self::sio::TcpServer::new(stream, direct);

    info!("beginning event loop...");
    tokio::run(server.anchor());

    Ok(())
}
