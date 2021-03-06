// Copyright 2022 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.ui.base;

import android.text.TextUtils;

import androidx.annotation.VisibleForTesting;

import org.chromium.base.annotations.CalledByNative;
import org.chromium.base.annotations.JNINamespace;
import org.chromium.url.GURL;

/**
 * Bare minimal wrapper class of native content::DropData.
 */
@JNINamespace("ui")
public class DropDataAndroid {
    public final String text;
    public final GURL gurl;
    public final byte[] imageContent;
    public final String imageContentExtension;

    /** Not generated from java */
    private DropDataAndroid(
            String text, GURL gurl, byte[] imageContent, String imageContentExtension) {
        this.text = text;
        this.gurl = gurl;
        this.imageContent = imageContent;
        this.imageContentExtension = imageContentExtension;
    }

    @VisibleForTesting
    @CalledByNative
    static DropDataAndroid create(
            String text, GURL gurl, byte[] imageContent, String imageContentExtension) {
        return new DropDataAndroid(text, gurl, imageContent, imageContentExtension);
    }

    /** Return whether this data presents a plain of text. */
    public boolean isPlainText() {
        return GURL.isEmptyOrInvalid(gurl) && !TextUtils.isEmpty(text);
    }

    /** Return whether this data presents a link. */
    public boolean hasLink() {
        return !GURL.isEmptyOrInvalid(gurl);
    }

    /** Return whether this data presents an image. */
    public boolean hasImage() {
        return imageContent != null && !TextUtils.isEmpty(imageContentExtension);
    }
}
