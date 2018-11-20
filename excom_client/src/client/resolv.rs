use std::net::SocketAddr;

pub fn resolve(addr: &str) -> Option<SocketAddr> {
    addr.parse::<SocketAddr>().ok()
}
