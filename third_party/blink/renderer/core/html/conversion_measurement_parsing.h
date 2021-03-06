// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_HTML_CONVERSION_MEASUREMENT_PARSING_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_HTML_CONVERSION_MEASUREMENT_PARSING_H_

#include "third_party/abseil-cpp/absl/types/optional.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"

namespace blink {

class ExecutionContext;
class HTMLAnchorElement;

struct WebImpression;

// Dummy struct to pass un-parsed Attribution Reporting window features into the
// parsing utilities below.
struct ImpressionFeatures {
  String impression_data;
  String conversion_destination;
  String reporting_origin;
  String expiry;
  String priority;
};

// Returns the WebImpression struct with all data declared by impression
// related attributes on |element|. If the impression attributes do not contain
// allowed values, absl::nullopt is returned.
absl::optional<WebImpression> GetImpressionForAnchor(
    HTMLAnchorElement* element);

// Same as GetImpressionForAnchor(), but gets an impression specified by the
// features string associated with a window.open call.
absl::optional<WebImpression> GetImpressionFromWindowFeatures(
    ExecutionContext* execution_context,
    const ImpressionFeatures& features);

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_HTML_CONVERSION_MEASUREMENT_PARSING_H_
