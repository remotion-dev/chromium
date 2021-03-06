// Copyright 2021 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/enterprise/reporting/report_scheduler_android.h"

#include "chrome/browser/browser_process.h"
#include "components/prefs/pref_service.h"

namespace enterprise_reporting {

ReportSchedulerAndroid::ReportSchedulerAndroid()
    : ReportSchedulerAndroid(g_browser_process->local_state()) {}
ReportSchedulerAndroid::ReportSchedulerAndroid(raw_ptr<PrefService> prefs)
    : prefs_(prefs) {}

ReportSchedulerAndroid::~ReportSchedulerAndroid() = default;

PrefService* ReportSchedulerAndroid::GetPrefService() {
  return prefs_;
}

void ReportSchedulerAndroid::StartWatchingUpdatesIfNeeded(
    base::Time last_upload,
    base::TimeDelta upload_interval) {
  // No-op because in-app auto-update is not supported on Android.
}

void ReportSchedulerAndroid::StopWatchingUpdates() {
  // No-op because in-app auto-update is not supported on Android.
}

void ReportSchedulerAndroid::OnBrowserVersionUploaded() {
  // No-op because in-app auto-update is not supported on Android.
}

void ReportSchedulerAndroid::StartWatchingExtensionRequestIfNeeded() {
  // No-op because extensions are not supported on Android.
}

void ReportSchedulerAndroid::StopWatchingExtensionRequest() {
  // No-op because extensions are not supported on Android.
}

void ReportSchedulerAndroid::OnExtensionRequestUploaded() {
  // No-op because extensions are not supported on Android.
}

}  // namespace enterprise_reporting
