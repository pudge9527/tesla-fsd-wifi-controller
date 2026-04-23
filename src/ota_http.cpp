#include <Arduino.h>
#include <Update.h>
#include <WiFi.h>
#include <cstdlib>
#include <strings.h>
#include <esp_crt_bundle.h>
#include <esp_err.h>
#include <esp_http_client.h>
#include <esp_https_ota.h>

#include "ota_http.h"

namespace {

String getGitHubLatestReleaseApiUrl(const String& repo) {
    return "https://api.github.com/repos/" + repo + "/releases/latest";
}

String getGitHubLatestDownloadUrl(const String& repo, const String& assetName) {
    return "https://github.com/" + repo + "/releases/latest/download/" + assetName;
}

bool extractJsonUnsignedField(const String& json, const char* key, size_t searchFrom, size_t searchTo, size_t& value) {
    String pattern = "\"" + String(key) + "\":";
    int keyPos = json.indexOf(pattern, static_cast<int>(searchFrom));
    if (keyPos < 0) return false;
    if (searchTo > searchFrom && static_cast<size_t>(keyPos) >= searchTo) return false;

    size_t valueStart = static_cast<size_t>(keyPos) + pattern.length();
    while (valueStart < json.length() && (json[valueStart] == ' ' || json[valueStart] == '\n' || json[valueStart] == '\r' || json[valueStart] == '\t')) {
        ++valueStart;
    }

    size_t valueEnd = valueStart;
    while (valueEnd < json.length() && json[valueEnd] >= '0' && json[valueEnd] <= '9') {
        ++valueEnd;
    }
    if (valueEnd == valueStart) return false;
    if (searchTo > searchFrom && valueEnd > searchTo) return false;

    value = static_cast<size_t>(strtoull(json.substring(valueStart, valueEnd).c_str(), nullptr, 10));
    return true;
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

bool findGitHubAssetDownloadUrl(const String& json, const String& assetName, String& assetUrl, size_t& assetSizeBytes) {
    assetSizeBytes = 0;
    size_t searchFrom = 0;
    while (true) {
        String currentName;
        size_t nameEnd = 0;
        if (!extractJsonStringField(json, "name", searchFrom, currentName, &nameEnd)) return false;

        int nextNamePos = json.indexOf("\"name\":", static_cast<int>(nameEnd + 1));
        size_t sectionEnd = nextNamePos >= 0 ? static_cast<size_t>(nextNamePos) : static_cast<size_t>(json.length());
        int downloadPos = json.indexOf("\"browser_download_url\":", static_cast<int>(nameEnd + 1));
        String currentUrl;
        if (downloadPos >= 0 && (nextNamePos < 0 || downloadPos < nextNamePos)) {
            if (extractJsonStringField(json, "browser_download_url", nameEnd + 1, currentUrl)) {
                if (assetUrl.isEmpty() && currentUrl.endsWith(".bin")) assetUrl = currentUrl;
            }
        }
        size_t currentSize = 0;
        extractJsonUnsignedField(json, "size", nameEnd + 1, sectionEnd, currentSize);

        if (currentName == assetName) {
            if (!currentUrl.isEmpty()) assetUrl = currentUrl;
            assetSizeBytes = currentSize;
            return !assetUrl.isEmpty();
        }
        if (assetSizeBytes == 0 && currentSize > 0 && !assetUrl.isEmpty()) {
            assetSizeBytes = currentSize;
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

// 记录在线 OTA 的重定向和关键响应头，方便排查下载链路。
esp_err_t logOnlineOTAHttpEvent(esp_http_client_event_t* event) {
    if (event == nullptr) return ESP_OK;

    if (event->event_id == HTTP_EVENT_REDIRECT) {
        Serial.printf("Online OTA redirect: HTTP %d\n", esp_http_client_get_status_code(event->client));
    } else if (event->event_id == HTTP_EVENT_ON_HEADER && event->header_key != nullptr && event->header_value != nullptr) {
        if (strcasecmp(event->header_key, "Content-Type") == 0 || strcasecmp(event->header_key, "Content-Length") == 0) {
            Serial.printf("Online OTA header: %s=%s\n", event->header_key, event->header_value);
        }
    }
    return ESP_OK;
}

// 初始化在线 OTA 请求头。
esp_err_t initOnlineOTAHttpClient(esp_http_client_handle_t client) {
    if (client == nullptr) return ESP_ERR_INVALID_ARG;
    esp_http_client_set_header(client, "Accept", "application/octet-stream");
    return ESP_OK;
}

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

}  // namespace

bool fetchLatestGitHubRelease(const String& repo, const String& assetName,
                              String& latestVersion, String& assetUrl,
                              String& latestDownloadUrl, size_t& assetSizeBytes,
                              String& error) {
    latestVersion = "";
    assetUrl = "";
    assetSizeBytes = 0;
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

    if (!findGitHubAssetDownloadUrl(response.payload, assetName, assetUrl, assetSizeBytes) || assetUrl.isEmpty()) {
        error = "GitHub Release 未找到固件附件 " + assetName;
        return false;
    }

    return true;
}

bool performOnlineOTA(const String& url, String& error, OnlineOTAProgressCallback progressCallback) {
    if (WiFi.status() != WL_CONNECTED) {
        error = "请先连接上游热点后再在线更新";
        return false;
    }

    Serial.printf("Online OTA start: %s\n", url.c_str());
    esp_http_client_config_t config = {};
    config.url = url.c_str();
    config.user_agent = "tesla-fsd-wifi-controller";
    config.timeout_ms = 20000;
    config.max_redirection_count = 8;
    config.buffer_size = 8192;
    config.buffer_size_tx = 1024;
    config.keep_alive_enable = true;
    config.event_handler = logOnlineOTAHttpEvent;
    if (url.startsWith("https://")) config.crt_bundle_attach = esp_crt_bundle_attach;

    esp_https_ota_config_t otaConfig = {};
    otaConfig.http_config = &config;
    otaConfig.http_client_init_cb = initOnlineOTAHttpClient;

    esp_https_ota_handle_t otaHandle = nullptr;
    esp_err_t err = esp_https_ota_begin(&otaConfig, &otaHandle);
    if (err != ESP_OK) {
        error = "在线更新连接失败，" + String(esp_err_to_name(err));
        Serial.printf("Online OTA begin failed: %s\n", esp_err_to_name(err));
        return false;
    }

    int nextProgressPercent = 25;
    int totalSize = esp_https_ota_get_image_size(otaHandle);
    if (totalSize > 0) {
        Serial.printf("Online OTA image size: %d bytes\n", totalSize);
    } else {
        Serial.println("Online OTA image size: unknown");
    }
    if (progressCallback) progressCallback(0, totalSize > 0 ? static_cast<size_t>(totalSize) : 0);

    while ((err = esp_https_ota_perform(otaHandle)) == ESP_ERR_HTTPS_OTA_IN_PROGRESS) {
        int written = esp_https_ota_get_image_len_read(otaHandle);
        if (progressCallback) {
            progressCallback(written > 0 ? static_cast<size_t>(written) : 0,
                             totalSize > 0 ? static_cast<size_t>(totalSize) : 0);
        }
        if (totalSize <= 0 || nextProgressPercent > 100) continue;

        if (written <= 0) continue;

        int progress = written * 100 / totalSize;
        while (progress >= nextProgressPercent && nextProgressPercent <= 100) {
            Serial.printf("Online OTA progress: %d%% (%d/%d bytes)\n",
                          nextProgressPercent, written, totalSize);
            nextProgressPercent += 25;
        }
    }

    if (err != ESP_OK) {
        int httpCode = esp_https_ota_get_status_code(otaHandle);
        esp_https_ota_abort(otaHandle);
        if (httpCode > 0) {
            error = "固件下载失败，HTTP " + String(httpCode);
        } else {
            error = "在线更新失败，" + String(esp_err_to_name(err));
        }
        Serial.printf("Online OTA perform failed: %s, http=%d\n", esp_err_to_name(err), httpCode);
        return false;
    }
    if (!esp_https_ota_is_complete_data_received(otaHandle)) {
        esp_https_ota_abort(otaHandle);
        error = "固件下载不完整";
        Serial.println("Online OTA failed: complete image not received");
        return false;
    }
    err = esp_https_ota_finish(otaHandle);
    if (err != ESP_OK) {
        error = "固件写入失败，" + String(esp_err_to_name(err));
        Serial.printf("Online OTA finish failed: %s\n", esp_err_to_name(err));
        return false;
    }

    Serial.println("Online OTA success");
    if (progressCallback) {
        size_t totalBytes = totalSize > 0 ? static_cast<size_t>(totalSize) : 0;
        progressCallback(totalBytes, totalBytes);
    }
    return true;
}
