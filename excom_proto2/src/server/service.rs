use serde::de::DeserializeOwned;
use serde::Serialize;
use std::sync::Arc;
use tokio::prelude::*;

pub trait Service {
    type Error: Serialize + DeserializeOwned;

    // fn handle_call(&self, call: Call) -> Box<Future<Item = Response, Error = Self::Error>>;
}

// #[derive(Debug, Copy, Clone, PartialEq, Eq, Hash)]
// pub struct NullService;

// impl<S: Service> Service for Arc<S> {
//     type Error = S::Error;

//     fn handle_call(&self, call: Call) -> Box<dyn Future<Item = Response, Error = Self::Error>> {
//         self.as_ref().handle_call(call)
//     }
// }

// impl<S: Service> Service for NullService {
//     type Error = ();

//     fn handle_call(&self, call: Call) -> Box<dyn Future<Item = Response, Error = Self::Error>> {
//         Box::new(Err(()).into())
//     }
// }
