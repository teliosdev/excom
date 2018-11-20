#[macro_use]
extern crate log;

use excom_proto::Packet;
use tokio::prelude::*;

pub mod client;
pub mod errors;

pub use self::client::Client;

fn main() {
    env_logger::init();
    info!("beginning connection...");
    let fut = Client::new("example.com".into(), "127.0.0.1:4209".into(), true)
        .and_then(|(client, driver)| {
            let driver = tokio::spawn(
                driver
                    .map_err(|err| {
                        error!("driver returned err: {:?}", err);
                        ()
                    })
                    .map(|_| ()),
            );

            info!("client connected! sending ping...");
            let reqresp = client
                .call(Packet::Ping)
                .and_then(|resp| {
                    warn!("received response from server: {:?}", resp);
                    Ok(())
                })
                .or_else(move |err| {
                    error!("error while managing request: {:?}", err);
                    Ok(())
                });

            reqresp
                .then(|_: Result<(), ()>| driver.into_future())
                .map_err(|_| unreachable!())
                .map(move |_| client)
        })
        .map_err(|err| {
            error!("error: {:?}", err);
            ()
        });

    tokio::run(fut.map(|_| ()))
}
