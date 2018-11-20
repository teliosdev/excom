use error_chain::*;

error_chain!{
    errors {
        InitServerError(error: &'static str) {
            display("an error occurred while building a server: {}", error)
        }

        AcceptTlsError {}
        ListenError {}
        DriveServerError {}

        ControlServerError(error: &'static str) {
            display("could not perform control operation: {}", error)
        }
    }

    // foreign_links {
    //     Io(::std::io::Error);
    // }
}
