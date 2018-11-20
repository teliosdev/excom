use excom_proto::tls::Configuration;
use serde_derive::*;
use std::net::{IpAddr, Ipv6Addr, SocketAddr};

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ServerConfiguration {
    #[serde(default = "default_address")]
    pub address: SocketAddr,
    pub tls: Configuration,
}

fn default_address() -> SocketAddr {
    SocketAddr::new(IpAddr::V6(Ipv6Addr::UNSPECIFIED), 4209)
}
