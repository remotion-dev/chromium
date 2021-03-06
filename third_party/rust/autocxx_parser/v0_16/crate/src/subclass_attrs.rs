// Copyright 2021 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

use proc_macro2::Ident;
use quote::ToTokens;
use syn::Result as ParseResult;
use syn::{
    parse::{Parse, ParseStream},
    token::Comma,
};

#[derive(Default)]
pub struct SubclassAttrs {
    pub self_owned: bool,
    pub superclass: Option<String>,
}

impl Parse for SubclassAttrs {
    fn parse(input: ParseStream) -> ParseResult<Self> {
        let mut me = Self::default();
        let mut id = input.parse::<Option<Ident>>()?;
        while id.is_some() {
            match id {
                Some(id) if id == "self_owned" => me.self_owned = true,
                Some(id) if id == "superclass" => {
                    let args;
                    syn::parenthesized!(args in input);
                    let superclass: syn::LitStr = args.parse()?;
                    if me.superclass.is_some() {
                        return Err(syn::Error::new_spanned(
                            id.into_token_stream(),
                            "Expected single superclass specification",
                        ));
                    }
                    me.superclass = Some(superclass.value());
                }
                Some(id) => {
                    return Err(syn::Error::new_spanned(
                        id.into_token_stream(),
                        "Expected self_owned or superclass",
                    ))
                }
                None => {}
            };
            let comma = input.parse::<Option<Comma>>()?;
            if comma.is_none() {
                break;
            }
            id = input.parse::<Option<Ident>>()?;
        }
        Ok(me)
    }
}
