/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 * Copyright (C) 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011 Apple Inc.
 * All rights reserved.
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_CSS_RESOLVER_MATCH_REQUEST_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_CSS_RESOLVER_MATCH_REQUEST_H_

#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/css/css_style_sheet.h"
#include "third_party/blink/renderer/core/css/rule_set.h"

namespace blink {

class ContainerNode;

// Encapsulates the context for matching against a single style sheet by
// ElementRuleCollector. Carries the RuleSet, scope (a ContainerNode) and
// CSSStyleSheet.
class CORE_EXPORT MatchRequest {
  STACK_ALLOCATED();

 public:
  MatchRequest(RuleSet* rule_set,
               const ContainerNode* scope = nullptr,
               const CSSStyleSheet* css_sheet = nullptr,
               unsigned style_sheet_index = 0,
               Element* vtt_originating_element = nullptr)
      : rule_set(rule_set),
        scope(scope),
        style_sheet(css_sheet),
        style_sheet_index(style_sheet_index),
        vtt_originating_element(vtt_originating_element) {
    // Now that we're about to read from the RuleSet, we're done adding more
    // rules to the set and we should make sure it's compacted.
    rule_set->CompactRulesIfNeeded();
  }

  const RuleSet* rule_set;
  const ContainerNode* scope;
  const CSSStyleSheet* style_sheet;
  const unsigned style_sheet_index;
  // For WebVTT STYLE blocks, this is set to the featureless-like Element
  // described by the spec:
  // https://w3c.github.io/webvtt/#obtaining-css-boxes
  Element* vtt_originating_element;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_CSS_RESOLVER_MATCH_REQUEST_H_
