use error_chain::*;

error_chain!{
    errors {
        InvalidAddressError {}
        InvalidStateError {}
        SyncError {}
    }

    links {
        Proto(::excom_proto::Error, ::excom_proto::ErrorKind);
    }
}
