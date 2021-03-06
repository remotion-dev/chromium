// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "third_party/blink/renderer/core/css/container_query.h"
#include "third_party/blink/renderer/core/css/media_query_exp.h"
#include "third_party/blink/renderer/core/style/computed_style_constants.h"

namespace blink {

ContainerSelector::ContainerSelector(AtomicString name,
                                     const MediaQueryExpNode& query)
    : name_(std::move(name)), feature_flags_(query.CollectFeatureFlags()) {}

unsigned ContainerSelector::Type(WritingMode writing_mode) const {
  unsigned type = kContainerTypeNone;

  if (feature_flags_ & MediaQueryExpNode::kFeatureInlineSize)
    type |= kContainerTypeInlineSize;
  if (feature_flags_ & MediaQueryExpNode::kFeatureBlockSize)
    type |= kContainerTypeBlockSize;
  if (feature_flags_ & MediaQueryExpNode::kFeatureWidth) {
    type |= (IsHorizontalWritingMode(writing_mode) ? kContainerTypeInlineSize
                                                   : kContainerTypeBlockSize);
  }
  if (feature_flags_ & MediaQueryExpNode::kFeatureHeight) {
    type |= (IsHorizontalWritingMode(writing_mode) ? kContainerTypeBlockSize
                                                   : kContainerTypeInlineSize);
  }

  return type;
}

ContainerQuery::ContainerQuery(ContainerSelector selector,
                               std::unique_ptr<MediaQueryExpNode> query)
    : selector_(std::move(selector)), query_(std::move(query)) {}

ContainerQuery::ContainerQuery(const ContainerQuery& other)
    : selector_(other.selector_), query_(other.query_->Copy()) {}

String ContainerQuery::ToString() const {
  return query_->Serialize();
}

ContainerQuery* ContainerQuery::CopyWithParent(
    const ContainerQuery* parent) const {
  ContainerQuery* copy = MakeGarbageCollected<ContainerQuery>(*this);
  copy->parent_ = parent;
  return copy;
}

}  // namespace blink
