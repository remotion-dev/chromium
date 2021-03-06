// Copyright 2021 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#import "ios/chrome/browser/infobars/overlays/browser_agent/interaction_handlers/test/mock_reading_list_infobar_interaction_handler.h"

#if !defined(__has_feature) || !__has_feature(objc_arc)
#error "This file requires ARC support."
#endif

MockReadingListInfobarModalInteractionHandler::
    MockReadingListInfobarModalInteractionHandler(Browser* browser)
    : ReadingListInfobarModalInteractionHandler(browser) {}

MockReadingListInfobarModalInteractionHandler::
    ~MockReadingListInfobarModalInteractionHandler() = default;
