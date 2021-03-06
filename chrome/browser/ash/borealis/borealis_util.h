// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_ASH_BOREALIS_BOREALIS_UTIL_H_
#define CHROME_BROWSER_ASH_BOREALIS_BOREALIS_UTIL_H_

#include <string>

#include "base/callback_forward.h"
#include "base/strings/string_piece.h"
#include "chromeos/dbus/dlcservice/dlcservice_client.h"
#include "third_party/abseil-cpp/absl/types/optional.h"
#include "ui/views/widget/widget.h"

class Profile;

namespace borealis {

// This is used by the Borealis installer app.
// Generated by crx_file::id_util::GenerateId("org.chromium.borealis");
extern const char kInstallerAppId[];
// This is the id of the main application which borealis runs.
extern const char kClientAppId[];
// App IDs prefixed with this are unidentified and should be largely ignored.
// These are "anonymous" apps, created because a window couldn't be associated
// with any known app. When these appear in Borealis it's usually caused by
// the main app; but we can't rely on that, so just ignore them.
extern const char kIgnoredAppIdPrefix[];
// This is used to install the Borealis DLC component.
extern const char kBorealisDlcName[];
// The regex used for extracting the Borealis app ID of an application.
extern const char kBorealisAppIdRegex[];
// Base64-encoded allowed x-scheme for Borealis apps.
extern const char kAllowedScheme[];
// Base64-encoded allow list of URLs that can be handled by Borealis apps.
extern const base::StringPiece kURLAllowlist[];
// Error string to replace Proton version info in the event that a GameID
// parsed with /usr/bin/get_proton_version.py in the Borealis VM does not
// match the GameID expected based on extraction with kBorealisAppIdRegex.
extern const char kProtonVersionGameMismatch[];
// Query parameter key for device information in the borealis feedback
// form.
extern const char kDeviceInformationKey[];

// TODO(b/218403711): remove these when insert_coin is deprecated. We only have
// insert_coin in the short-term until installer UX is finalized.
extern const char kInsertCoinSuccessMessage[];
extern const char kInsertCoinRejectMessage[];

struct ProtonVersionInfo {
  std::string proton = "Not applicable";
  std::string slr = "Not applicable";
};

// Shows the Borealis installer (borealis_installer_view).
void ShowBorealisInstallerView(Profile* profile);

// Returns a Borealis app ID parsed from |exec|, or nullopt on failure.
// TODO(b/173547790): This should probably be moved when we've decided
// the details of how/where it will be used.
absl::optional<int> GetBorealisAppId(std::string exec);

// Returns the Borealis app ID of the |window|, or nullopt on failure.
absl::optional<int> GetBorealisAppId(const aura::Window* window);

// Shows the splash screen (borealis_splash_screen_view).
void ShowBorealisSplashScreenView(Profile* profile);
// Closes the splash screen (borealis_splash_screen_view).
void CloseBorealisSplashScreenView();

// Creates a URL for a feedback form with prefilled app/device info, or an
// invalid URL if we don't want to collect feedback for the given |app_id|. Will
// invoke |url_callback| when the url is ready.
void FeedbackFormUrl(Profile* const profile,
                     const std::string& app_id,
                     const std::string& window_title,
                     base::OnceCallback<void(GURL)> url_callback);

// Checks that a given URL has the allowed scheme and that its contents starts
// with one of the URLs in the allowlist.
bool IsExternalURLAllowed(const GURL& url);

// Executes /usr/bin/get_proton_version.py in the borealis VM, which outputs
// the Proton version information of any recent Proton game session.
bool GetProtonVersionInfo(const std::string& owner_id, std::string* output);

// Parses the output returned by GetProtonVersionInfo.
ProtonVersionInfo ParseProtonVersionInfo(absl::optional<int> game_id,
                                         const std::string& output);

// Used in the splash screen to get the dlc path as dlcservice could not be
// imported directly.
void GetDlcPath(base::OnceCallback<void(const std::string& path)> callback);

}  // namespace borealis

#endif  // CHROME_BROWSER_ASH_BOREALIS_BOREALIS_UTIL_H_
