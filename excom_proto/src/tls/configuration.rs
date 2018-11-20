use crate::errors::{Error, ErrorKind, ResultExt};
use std::fs::File;
use std::io::Read;
use std::path::PathBuf;

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct Configuration {
    pub file: PathBuf,
    pub pass: String,
}

impl Configuration {
    pub fn identity(&self) -> Result<native_tls::Identity, Error> {
        let mut file = File::open(&self.file)?;
        let mut content = vec![];
        file.read_to_end(&mut content)?;

        native_tls::Identity::from_pkcs12(&content, &self.pass).chain_err(|| ErrorKind::TlsError)
    }
}
