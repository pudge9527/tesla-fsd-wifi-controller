#pragma once

#include <Arduino.h>

typedef void (*OnlineOTAProgressCallback)(size_t writtenBytes, size_t totalBytes);

bool fetchLatestGitHubRelease(const String& repo, const String& assetName,
                              String& latestVersion, String& assetUrl,
                              String& latestDownloadUrl, size_t& assetSizeBytes,
                              String& error);
bool performOnlineOTA(const String& url, String& error,
                      OnlineOTAProgressCallback progressCallback = nullptr);
