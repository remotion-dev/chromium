// Copyright 2022 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.ui.base;

import org.junit.Assert;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.robolectric.RobolectricTestRunner;

import org.chromium.url.GURL;
import org.chromium.url.JUnitTestGURLs;

/** Unit test for {@link org.chromium.ui.base.DropDataAndroid}. */
@RunWith(RobolectricTestRunner.class)
public class DropDataAndroidUnitTest {
    @Test
    public void testPlainText() {
        final String text = "text";
        final DropDataAndroid data = DropDataAndroid.create(text, null, null, null);

        assertDragData(data, /*isPlainText=*/true, /*hasLink=*/false, /*hasImage=*/false);
        Assert.assertEquals("Text does not match.", text, data.text);
    }

    @Test
    public void testLink() {
        final String text = "text";
        final GURL gurl = JUnitTestGURLs.getGURL(JUnitTestGURLs.EXAMPLE_URL);
        final DropDataAndroid data = DropDataAndroid.create(text, gurl, null, null);

        assertDragData(data, /*isPlainText=*/false, /*hasLink=*/true, /*hasImage=*/false);
        Assert.assertEquals("Link does not match.", gurl, data.gurl);
    }

    @Test
    public void testImage() {
        final byte[] img = new byte[] {1, 2};
        final String imageExtension = "webp";
        final DropDataAndroid data = DropDataAndroid.create("", null, img, imageExtension);

        assertDragData(data, /*isPlainText=*/false, /*hasLink=*/false, /*hasImage=*/true);
        Assert.assertEquals("Image content does not match.", img, data.imageContent);
        Assert.assertEquals(
                "Image extension does not match.", imageExtension, data.imageContentExtension);
    }

    @Test
    public void testImageLink() {
        final GURL gurl = JUnitTestGURLs.getGURL(JUnitTestGURLs.EXAMPLE_URL);
        final byte[] img = new byte[] {1, 2};
        final String imageExtension = "webp";
        final DropDataAndroid data = DropDataAndroid.create("", gurl, img, imageExtension);

        assertDragData(data, /*isPlainText=*/false, /*hasLink=*/true, /*hasImage=*/true);
        Assert.assertEquals("Link does not match.", gurl, data.gurl);
        Assert.assertEquals("Image content does not match.", img, data.imageContent);
        Assert.assertEquals(
                "Image extension does not match.", imageExtension, data.imageContentExtension);
    }

    private void assertDragData(
            DropDataAndroid data, boolean isPlainText, boolean hasLink, boolean hasImage) {
        Assert.assertEquals("#isPlainText check failed.", isPlainText, data.isPlainText());
        Assert.assertEquals("#hasLink check failed.", hasLink, data.hasLink());
        Assert.assertEquals("#hasImage check failed.", hasImage, data.hasImage());
    }
}
