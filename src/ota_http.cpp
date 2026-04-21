#include <Arduino.h>
#include <Update.h>
#include <WiFi.h>
#include <esp_crt_bundle.h>
#include <esp_err.h>
#include <esp_http_client.h>

#include "ota_http.h"

namespace {

String getGitHubLatestReleaseApiUrl(const String& repo) {
    return "https://api.github.com/repos/" + repo + "/releases/latest";
}

String getGitHubLatestDownloadUrl(const String& repo, const String& assetName) {
    return "https://github.com/" + repo + "/releases/latest/download/" + assetName;
}

bool extractJsonStringField(const String& json, const char* key, size_t searchFrom, String& value, size_t* valueEndOut = nullptr) {
    String pattern = "\"" + String(key) + "\":";
    int keyPos = json.indexOf(pattern, static_cast<int>(searchFrom));
    if (keyPos < 0) return false;

    int quoteStart = json.indexOf('"', keyPos + pattern.length());
    if (quoteStart < 0) return false;

    value = "";
    bool escaped = false;
    for (int i = quoteStart + 1; i < json.length(); ++i) {
        char c = json[i];
        if (escaped) {
            switch (c) {
                case '"':
                case '\\':
                case '/':
                    value += c;
                    break;
                case 'n':
                    value += '\n';
                    break;
                case 'r':
                    value += '\r';
                    break;
                case 't':
                    value += '\t';
                    break;
                default:
                    value += c;
                    break;
            }
            escaped = false;
            continue;
        }
        if (c == '\\') {
            escaped = true;
            continue;
        }
        if (c == '"') {
            if (valueEndOut) *valueEndOut = static_cast<size_t>(i);
            return true;
        }
        value += c;
    }

    return false;
}

bool findGitHubAssetDownloadUrl(const String& json, const String& assetName, String& assetUrl) {
    size_t searchFrom = 0;
    while (true) {
        String currentName;
        size_t nameEnd = 0;
        if (!extractJsonStringField(json, "name", searchFrom, currentName, &nameEnd)) return false;

        int nextNamePos = json.indexOf("\"name\":", static_cast<int>(nameEnd + 1));
        int downloadPos = json.indexOf("\"browser_download_url\":", static_cast<int>(nameEnd + 1));
        if (downloadPos >= 0 && (nextNamePos < 0 || downloadPos < nextNamePos)) {
            String currentUrl;
            if (extractJsonStringField(json, "browser_download_url", nameEnd + 1, currentUrl)) {
                if (currentName == assetName) {
                    assetUrl = currentUrl;
                    return true;
                }
                if (assetUrl.isEmpty() && currentUrl.endsWith(".bin")) {
                    assetUrl = currentUrl;
                }
            }
        }

        if (nextNamePos < 0) break;
        searchFrom = static_cast<size_t>(nextNamePos);
    }

    return !assetUrl.isEmpty();
}

struct HttpStringResponse {
    String payload;
    bool reserveAttempted = false;
    bool outOfMemory = false;
};

struct OnlineOTAHttpContext {
    bool updateStarted = false;
    bool updateFailed = false;
    size_t expectedLength = 0;
    size_t written = 0;
    String error;
};

esp_err_t collectHttpResponseEvent(esp_http_client_event_t* event) {
    if (event == nullptr || event->user_data == nullptr) return ESP_OK;
    if (event->event_id != HTTP_EVENT_ON_DATA || event->data == nullptr || event->data_len <= 0) return ESP_OK;
    if (esp_http_client_get_status_code(event->client) != HttpStatus_Ok) return ESP_OK;

    auto* response = static_cast<HttpStringResponse*>(event->user_data);
    if (!response->reserveAttempted) {
        int contentLength = esp_http_client_get_content_length(event->client);
        if (contentLength > 0) {
            response->outOfMemory = !response->payload.reserve(static_cast<unsigned>(contentLength));
        }
        response->reserveAttempted = true;
    }
    if (response->outOfMemory) return ESP_OK;

    if (!response->payload.concat(static_cast<const char*>(event->data), static_cast<unsigned>(event->data_len))) {
        response->outOfMemory = true;
    }
    return ESP_OK;
}

esp_err_t writeOnlineOTAEvent(esp_http_client_event_t* event) {
    if (event == nullptr || event->user_data == nullptr) return ESP_OK;
    if (event->event_id != HTTP_EVENT_ON_DATA || event->data == nullptr || event->data_len <= 0) return ESP_OK;
    if (esp_http_client_get_status_code(event->client) != HttpStatus_Ok) return ESP_OK;

    auto* ota = static_cast<OnlineOTAHttpContext*>(event->user_data);
    if (ota->updateFailed) return ESP_OK;

    if (!ota->updateStarted) {
        int contentLength = esp_http_client_get_content_length(event->client);
        if (contentLength > 0) ota->expectedLength = static_cast<size_t>(contentLength);

        size_t updateSize = ota->expectedLength > 0 ? ota->expectedLength : UPDATE_SIZE_UNKNOWN;
        if (!Update.begin(updateSize)) {
            Update.printError(Serial);
            ota->updateFailed = true;
            ota->error = "无法开始写入固件";
            return ESP_OK;
        }
        ota->updateStarted = true;
    }

    size_t written = Update.write(reinterpret_cast<uint8_t*>(event->data), static_cast<size_t>(event->data_len));
    if (written != static_cast<size_t>(event->data_len)) {
        Update.printError(Serial);
        ota->updateFailed = true;
        ota->error = "固件写入失败";
        return ESP_OK;
    }
    ota->written += written;
    return ESP_OK;
}

}  // namespace

bool fetchLatestGitHubRelease(const String& repo, const String& assetName,
                              String& latestVersion, String& assetUrl,
                              String& latestDownloadUrl, String& error) {
    latestVersion = "";
    assetUrl = "";
    latestDownloadUrl = getGitHubLatestDownloadUrl(repo, assetName);

    if (WiFi.status() != WL_CONNECTED) {
        error = "请先连接上游热点后再检查 GitHub 更新";
        return false;
    }

    String apiUrl = getGitHubLatestReleaseApiUrl(repo);
    HttpStringResponse response;
    esp_http_client_config_t config = {};
    config.url = apiUrl.c_str();
    config.user_agent = "tesla-fsd-wifi-controller";
    config.timeout_ms = 20000;
    config.event_handler = collectHttpResponseEvent;
    config.user_data = &response;
    config.crt_bundle_attach = esp_crt_bundle_attach;

    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (client == nullptr) {
        error = "无法创建 GitHub Release HTTP 客户端";
        return false;
    }

    esp_http_client_set_header(client, "Accept", "application/vnd.github+json");
    esp_err_t err = esp_http_client_perform(client);
    int httpCode = esp_http_client_get_status_code(client);
    esp_http_client_cleanup(client);

    if (err != ESP_OK) {
        error = "GitHub Release 查询失败，" + String(esp_err_to_name(err));
        return false;
    }
    if (httpCode != HttpStatus_Ok) {
        error = "GitHub Release 查询失败，HTTP " + String(httpCode);
        return false;
    }
    if (response.outOfMemory) {
        error = "GitHub Release 响应过大";
        return false;
    }

    if (!extractJsonStringField(response.payload, "tag_name", 0, latestVersion) || latestVersion.isEmpty()) {
        error = "GitHub Release 未返回版本号";
        return false;
    }

    if (!findGitHubAssetDownloadUrl(response.payload, assetName, assetUrl) || assetUrl.isEmpty()) {
        error = "GitHub Release 未找到固件附件 " + assetName;
        return false;
    }

    return true;
}

bool performOnlineOTA(const String& url, String& error) {
    if (WiFi.status() != WL_CONNECTED) {
        error = "请先连接上游热点后再在线更新";
        return false;
    }

    OnlineOTAHttpContext ota;
    esp_http_client_config_t config = {};
    config.url = url.c_str();
    config.user_agent = "tesla-fsd-wifi-controller";
    config.timeout_ms = 20000;
    config.event_handler = writeOnlineOTAEvent;
    config.user_data = &ota;
    if (url.startsWith("https://")) config.crt_bundle_attach = esp_crt_bundle_attach;

    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (client == nullptr) {
        error = "无法创建在线更新 HTTP 客户端";
        return false;
    }

    esp_err_t err = esp_http_client_perform(client);
    int httpCode = esp_http_client_get_status_code(client);
    esp_http_client_cleanup(client);

    if (err != ESP_OK) {
        if (ota.updateStarted) Update.end(false);
        error = ota.error.isEmpty() ? "在线更新连接失败，" + String(esp_err_to_name(err)) : ota.error;
        return false;
    }
    if (httpCode != HttpStatus_Ok) {
        if (ota.updateStarted) Update.end(false);
        error = "固件下载失败，HTTP " + String(httpCode);
        return false;
    }
    if (ota.updateFailed) {
        if (ota.updateStarted) Update.end(false);
        error = ota.error.isEmpty() ? "固件写入失败" : ota.error;
        return false;
    }
    if (!ota.updateStarted) {
        error = "固件响应为空";
        return false;
    }
    if (ota.expectedLength > 0 && ota.written != ota.expectedLength) {
        Update.end(false);
        error = "固件下载不完整";
        return false;
    }
    if (!Update.end(true)) {
        Update.printError(Serial);
        error = "固件写入失败";
        return false;
    }
    if (!Update.isFinished()) {
        error = "固件写入未完成";
        return false;
    }
    return true;
}
