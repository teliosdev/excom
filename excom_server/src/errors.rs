use error_chain::*;

error_chain!{
    errors {
        IoError {}
        TlsError {}
        SendFailedError {}
        SyncError {}
    }

    links {
        Proto(::excom_proto::Error, ::excom_proto::ErrorKind);
    }
}
