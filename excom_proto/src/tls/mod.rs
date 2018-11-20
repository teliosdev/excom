use crate::errors::{Error, ErrorKind, ResultExt};
use native_tls;
use tokio_tls;

mod configuration;

pub use self::configuration::*;

pub fn acceptor(config: &Configuration) -> Result<tokio_tls::TlsAcceptor, Error> {
    let acceptor = native_tls::TlsAcceptor::builder(config.identity()?)
        .min_protocol_version(Some(native_tls::Protocol::Tlsv11))
        .max_protocol_version(Some(native_tls::Protocol::Tlsv12))
        .build()
        .chain_err(|| ErrorKind::TlsError)?;

    Ok(tokio_tls::TlsAcceptor::from(acceptor))
}

pub fn connector(allow_invalid: bool) -> Result<tokio_tls::TlsConnector, Error> {
    let mut builder = native_tls::TlsConnector::builder();
    builder
        .min_protocol_version(Some(native_tls::Protocol::Tlsv11))
        .max_protocol_version(Some(native_tls::Protocol::Tlsv12));

    if allow_invalid {
        builder
            .danger_accept_invalid_certs(true)
            .danger_accept_invalid_hostnames(true);
    }

    let connector = builder.build().chain_err(|| ErrorKind::TlsError)?;
    Ok(tokio_tls::TlsConnector::from(connector))
}
