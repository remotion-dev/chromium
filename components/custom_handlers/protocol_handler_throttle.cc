// Copyright 2022 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/custom_handlers/protocol_handler_throttle.h"

#include "components/custom_handlers/protocol_handler_registry.h"
#include "services/network/public/cpp/resource_request.h"

namespace custom_handlers {

ProtocolHandlerThrottle::ProtocolHandlerThrottle(
    const custom_handlers::ProtocolHandlerRegistry& protocol_handler_registry)
    : protocol_handler_registry_(&protocol_handler_registry) {}

void ProtocolHandlerThrottle::WillStartRequest(
    network::ResourceRequest* request,
    bool* defer) {
  // Don't translate the urn: scheme URL while loading the resource from the
  // specified web bundle.
  // TODO(https://crbug.com/1257045): Remove this when we drop urn: scheme
  // support in WebBundles.
  if (request->web_bundle_token_params &&
      request->url.SchemeIs(url::kUrnScheme)) {
    return;
  }
  TranslateUrl(request->url);
}

void ProtocolHandlerThrottle::WillRedirectRequest(
    net::RedirectInfo* redirect_info,
    const network::mojom::URLResponseHead& response_head,
    bool* defer,
    std::vector<std::string>* to_be_removed_headers,
    net::HttpRequestHeaders* modified_headers,
    net::HttpRequestHeaders* modified_cors_exempt_headers) {
  TranslateUrl(redirect_info->new_url);
}

void ProtocolHandlerThrottle::TranslateUrl(GURL& url) {
  // TODO(jfernandez): We should use scheme_piece instead, which would imply
  // adadpting the ProtocolHandlerRegistry code to use StringPiece.
  if (!protocol_handler_registry_->IsHandledProtocol(url.scheme()))
    return;
  GURL translated_url = protocol_handler_registry_->Translate(url);
  if (!translated_url.is_empty())
    url = translated_url;
}

}  // namespace custom_handlers
