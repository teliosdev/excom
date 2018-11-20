use error_chain::*;

error_chain!{
    errors {
        SerializationError {}
        DeserializationError {}
        ProtocolError {}
        TlsError {}
        InvalidPacketError {}
    }

    foreign_links {
        Io(::std::io::Error);
    }
}
