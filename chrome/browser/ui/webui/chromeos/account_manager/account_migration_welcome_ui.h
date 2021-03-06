// Copyright 2019 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_WEBUI_CHROMEOS_ACCOUNT_MANAGER_ACCOUNT_MIGRATION_WELCOME_UI_H_
#define CHROME_BROWSER_UI_WEBUI_CHROMEOS_ACCOUNT_MANAGER_ACCOUNT_MIGRATION_WELCOME_UI_H_

#include "base/memory/weak_ptr.h"
#include "content/public/browser/web_ui_message_handler.h"
#include "ui/web_dialogs/web_dialog_ui.h"

namespace chromeos {

// For chrome:://account-migration-welcome
class AccountMigrationWelcomeUI : public ui::WebDialogUI {
 public:
  explicit AccountMigrationWelcomeUI(content::WebUI* web_ui);

  AccountMigrationWelcomeUI(const AccountMigrationWelcomeUI&) = delete;
  AccountMigrationWelcomeUI& operator=(const AccountMigrationWelcomeUI&) =
      delete;

  ~AccountMigrationWelcomeUI() override;

 private:
  base::WeakPtrFactory<AccountMigrationWelcomeUI> weak_factory_{this};
};

}  // namespace chromeos

#endif  // CHROME_BROWSER_UI_WEBUI_CHROMEOS_ACCOUNT_MANAGER_ACCOUNT_MIGRATION_WELCOME_UI_H_
