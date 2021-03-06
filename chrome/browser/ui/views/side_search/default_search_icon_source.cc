// Copyright 2022 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/ui/views/side_search/default_search_icon_source.h"

#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/search_engines/template_url_service_factory.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/browser/ui/search/omnibox_utils.h"
#include "components/omnibox/browser/omnibox_edit_model.h"
#include "components/omnibox/browser/omnibox_view.h"
#include "ui/base/models/image_model.h"
#include "ui/gfx/geometry/insets.h"
#include "ui/gfx/image/canvas_image_source.h"

DefaultSearchIconSource::DefaultSearchIconSource(
    Browser* browser,
    IconChangedSubscription icon_changed_subscription)
    : browser_(browser),
      icon_changed_subscription_(std::move(icon_changed_subscription)) {
  // `template_url_service` may be null in tests.
  if (auto* template_url_service =
          TemplateURLServiceFactory::GetForProfile(browser->profile())) {
    template_url_service_observation_.Observe(template_url_service);
  }
}

DefaultSearchIconSource::~DefaultSearchIconSource() = default;

void DefaultSearchIconSource::OnTemplateURLServiceChanged() {
  const auto* default_template_url =
      TemplateURLServiceFactory::GetForProfile(browser_->profile())
          ->GetDefaultSearchProvider();

  // If there is currently no default search engine set, but there was one set
  // previously, reset `default_template_url_id_` and notify the client.
  if (!default_template_url &&
      default_template_url_id_ != kInvalidTemplateURLID) {
    default_template_url_id_ = kInvalidTemplateURLID;
    icon_changed_subscription_.Run();
  }

  // Update the favicon only if the current default search provider has changed.
  if (!default_template_url ||
      default_template_url->id() == default_template_url_id_)
    return;

  default_template_url_id_ = default_template_url->id();
  icon_changed_subscription_.Run();
}

void DefaultSearchIconSource::OnTemplateURLServiceShuttingDown() {
  template_url_service_observation_.Reset();
}

ui::ImageModel DefaultSearchIconSource::GetSizedIconImage(int size) const {
  content::WebContents* active_contents =
      browser_->tab_strip_model()->GetActiveWebContents();
  if (!active_contents)
    return ui::ImageModel();

  // Attempt to synchronously get the current default search engine's favicon.
  auto* omnibox_view = search::GetOmniboxView(active_contents);
  DCHECK(omnibox_view);
  gfx::Image icon =
      omnibox_view->model()->client()->GetFaviconForDefaultSearchProvider(
          base::BindRepeating(&DefaultSearchIconSource::OnIconFetched,
                              weak_ptr_factory_.GetWeakPtr()));
  if (icon.IsEmpty())
    return ui::ImageModel();

  // FaviconCache guarantee favicons will be of size gfx::kFaviconSize (16x16)
  // so add extra padding around them to align them vertically with the other
  // vector icons.
  DCHECK_GE(size, icon.Height());
  DCHECK_GE(size, icon.Width());
  gfx::Insets padding_border((size - icon.Height()) / 2,
                             (size - icon.Width()) / 2);

  return padding_border.IsEmpty()
             ? ui::ImageModel::FromImage(icon)
             : ui::ImageModel::FromImageSkia(
                   gfx::CanvasImageSource::CreatePadded(*icon.ToImageSkia(),
                                                        padding_border));
}

void DefaultSearchIconSource::OnIconFetched(const gfx::Image& icon) {
  // The favicon requested in the call to GetFaviconForDefaultSearchProvider()
  // will now have been cached by ChromeOmniboxClient's FaviconCache and
  // subsequent calls asking for the favicon will now return synchronously.
  // Notify clients so they can attempt to fetch the latest icon.
  icon_changed_subscription_.Run();
}
