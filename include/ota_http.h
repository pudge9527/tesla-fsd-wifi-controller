#pragma once

#include <Arduino.h>

bool fetchLatestGitHubRelease(const String& repo, const String& assetName,
                              String& latestVersion, String& assetUrl,
                              String& latestDownloadUrl, String& error);
bool performOnlineOTA(const String& url, String& error);
