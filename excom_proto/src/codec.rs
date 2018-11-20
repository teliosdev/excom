use bytes::BytesMut;
use crate::errors::{Error, ErrorKind, ResultExt};
use crate::message::Message;
use serde_cbor as cbor;
use tokio::codec::length_delimited::{Builder as LengthDelimitedBuilder, LengthDelimitedCodec};
use tokio::codec::*;

#[derive(Debug)]
pub struct ExcomCodec(LengthDelimitedCodec);

impl ExcomCodec {
    pub fn new() -> ExcomCodec {
        ExcomCodec(
            LengthDelimitedBuilder::new()
                .length_field_length(8)
                .new_codec(),
        )
    }
}

impl Decoder for ExcomCodec {
    type Item = Message;
    type Error = Error;

    fn decode(&mut self, src: &mut BytesMut) -> Result<Option<Message>, Error> {
        match self.0.decode(src) {
            Ok(Some(data)) => cbor::de::from_slice(&data[..])
                .chain_err(|| ErrorKind::DeserializationError)
                .map(Some),
            Ok(None) => Ok(None),
            Err(e) => Err(Error::with_chain(e, ErrorKind::ProtocolError)),
        }
    }
}

impl Encoder for ExcomCodec {
    type Item = Message;
    type Error = Error;

    fn encode(&mut self, item: Self::Item, dst: &mut BytesMut) -> Result<(), Self::Error> {
        let encoded =
            cbor::ser::to_vec_packed(&item).chain_err(|| ErrorKind::SerializationError)?;
        self.0
            .encode(encoded.into(), dst)
            .chain_err(|| ErrorKind::ProtocolError)
    }
}
