// Copyright 2021 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/**
 * @fileoverview Navigation listener to report hash change.
 */

goog.provide('__crWeb.navigationListeners');

/** Beginning of anonymous object */
(function() {

window.addEventListener('hashchange', function(evt) {
  __gCrWeb.common.sendWebKitMessage(
      'NavigationEventMessage',
      {'command': 'hashchange', 'frame_id': __gCrWeb.message.getFrameId()});
});
}());  // End of anonymous object
