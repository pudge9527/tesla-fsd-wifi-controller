#!/usr/bin/env python3
"""Serve the embedded ESP32 UI locally with mock API responses.

This keeps the web UI single-sourced in include/web_ui.h while making it easy
to preview layout and interaction changes without reflashing an ESP32 board.
"""

from __future__ import annotations

import argparse
import json
import math
import re
import threading
import time
from dataclasses import dataclass
from http import HTTPStatus
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
from typing import Any
from urllib.parse import parse_qs, urlparse


ROOT = Path(__file__).resolve().parent.parent
WEB_UI_HEADER = ROOT / "include" / "web_ui.h"
MAX_UPSTREAM_NETWORKS = 10
SPEED_OFFSET_BUCKET_COUNT = 10
MAX_OTA_URL_LEN = 256
GITHUB_REPO = "pudge9527/tesla-fsd-wifi-controller"
GITHUB_ASSET_NAME = "fsd-controller.bin"


def extract_embedded_html() -> str:
    content = WEB_UI_HEADER.read_text(encoding="utf-8")
    match = re.search(
        r'const char INDEX_HTML\[\] PROGMEM = R"rawliteral\(\r?\n(.*?)\r?\n\)rawliteral";',
        content,
        re.DOTALL,
    )
    if not match:
        raise RuntimeError(f"Failed to locate INDEX_HTML in {WEB_UI_HEADER}")
    return match.group(1)


def first_param(params: dict[str, list[str]], key: str, default: str = "") -> str:
    values = params.get(key)
    return values[0] if values else default


def to_bool(value: str) -> bool:
    return value not in {"", "0", "false", "False", "off"}


def clamp(value: int, min_value: int, max_value: int) -> int:
    return max(min_value, min(max_value, value))


def count_dns_rules(rules: str) -> int:
    return len([part for part in re.split(r"[\s,;]+", rules.strip()) if part])


def get_speed_offset_bucket_index(road_limit: int | None) -> int:
    if road_limit is None or road_limit < 0:
        return -1
    if road_limit < 40:
        return 0
    if road_limit < 50:
        return 1
    if road_limit < 60:
        return 2
    if road_limit < 70:
        return 3
    if road_limit < 80:
        return 4
    if road_limit < 90:
        return 5
    if road_limit < 100:
        return 6
    if road_limit < 110:
        return 7
    if road_limit < 120:
        return 8
    return 9


def get_upstream_signal_text(rssi: int | None) -> str:
    if rssi is None:
        return ""
    if rssi >= -55:
        return "优秀"
    if rssi >= -67:
        return "良好"
    if rssi >= -75:
        return "一般"
    return "较弱"


@dataclass
class SavedNetwork:
    ssid: str
    password: str = ""

    @property
    def has_password(self) -> bool:
        return bool(self.password)


class PreviewState:
    def __init__(self) -> None:
        self.seed_demo_state()

    def seed_demo_state(self) -> None:
        self.started_at = time.monotonic()
        self.rx_count = 142_360
        self.modified_count = 11_842
        self.error_count = 3

        self.can_ok = True
        self.fsd_triggered = True
        self.fsd_enable = True
        self.hw_mode = 1
        self.speed_profile = 1
        self.profile_mode_auto = False
        self.speed_offset_enable = True
        self.speed_offset_buckets = [0, 0, 5, 5, 8, 10, 12, 15, 18, 20]
        self.isa_chime = False
        self.emergency_detection = True
        self.china_mode = False

        self.road_speed_limit = 80
        self.vision_speed_limit = 80
        self.road_speed_limit_last_update = time.monotonic()

        self.ap_ssid = "FSD-Controller"
        self.ap_password = "12345678"
        self.ap_ip = "9.9.9.9"
        self.ap_clients = 1
        self.wifi_channel = 6

        self.upstream_enable = True
        self.saved_networks: list[SavedNetwork] = [
            SavedNetwork("Pudge-iPhone", "12345678"),
            SavedNetwork("Office-2.4G", "officepass"),
            SavedNetwork("Xiaomi-13-Hotspot", "xiaomi123"),
        ]
        self.scan_results = [
            {"ssid": "Pudge-iPhone", "rssi": -53, "secure": True},
            {"ssid": "Office-2.4G", "rssi": -66, "secure": True},
            {"ssid": "Xiaomi-13-Hotspot", "rssi": -61, "secure": True},
            {"ssid": "Tesla-Guest", "rssi": -72, "secure": False},
            {"ssid": "Cafe-WiFi", "rssi": -78, "secure": True},
        ]

        self.dns_whitelist_enable = True
        self.dns_allowlist = (
            "baidu.com\n"
            "tesla.com\n"
            "navinfo.com\n"
            "map.qq.com\n"
            "apple.com"
        )
        self.dns_blocklist = "ads.example.com\ntracking.example.net"
        self.dns_blocked_requests = [
            {"domain": "telemetry.tesla.cn", "count": 12, "seconds_ago": 42},
            {"domain": "map-updates.example.com", "count": 4, "seconds_ago": 160},
            {"domain": "doubleclick.net", "count": 18, "seconds_ago": 19},
            {"domain": "connectivitycheck.gstatic.com", "count": 9, "seconds_ago": 25},
            {"domain": "graph.facebook.com", "count": 6, "seconds_ago": 31},
            {"domain": "metrics.icloud.com", "count": 7, "seconds_ago": 55},
            {"domain": "api.amplitude.com", "count": 14, "seconds_ago": 68},
            {"domain": "sdk.split.io", "count": 5, "seconds_ago": 74},
            {"domain": "gateway.icloud.com", "count": 11, "seconds_ago": 93},
            {"domain": "ads.example.com", "count": 22, "seconds_ago": 108},
            {"domain": "tracking.example.net", "count": 16, "seconds_ago": 126},
            {"domain": "mobile.events.data.microsoft.com", "count": 8, "seconds_ago": 154},
            {"domain": "www.google-analytics.com", "count": 10, "seconds_ago": 183},
            {"domain": "api.mapbox.com", "count": 3, "seconds_ago": 207},
            {"domain": "logs.netflix.com", "count": 13, "seconds_ago": 236},
            {"domain": "browser.pipe.aria.microsoft.com", "count": 17, "seconds_ago": 264},
        ]
        self.firmware_version = "0.0.0-dev"
        self.github_latest_version = "v1.0.1"
        self.ota_url = self.github_latest_download_url()

    def github_latest_download_url(self) -> str:
        return f"https://github.com/{GITHUB_REPO}/releases/latest/download/{GITHUB_ASSET_NAME}"

    def github_release_asset_url(self, version: str) -> str:
        return f"https://github.com/{GITHUB_REPO}/releases/download/{version}/{GITHUB_ASSET_NAME}"

    def uptime_seconds(self) -> int:
        return int(time.monotonic() - self.started_at)

    def thermal_snapshot(self) -> tuple[float, float, str, bool]:
        uptime = self.uptime_seconds()
        current = 46.2 + 1.4 * math.sin(uptime / 11.0)
        average = 45.5 + 0.8 * math.sin(uptime / 19.0)
        return round(current, 1), round(average, 1), "正常", False

    def is_reserved_upstream_ssid(self, ssid: str) -> bool:
        return ssid == self.ap_ssid

    def connected_network(self) -> SavedNetwork | None:
        if not self.upstream_enable or not self.saved_networks:
            return None
        return self.saved_networks[0]

    def connected_rssi(self) -> int | None:
        network = self.connected_network()
        if network is None:
            return None
        uptime = self.uptime_seconds()
        base = next(
            (item["rssi"] for item in self.scan_results if item["ssid"] == network.ssid),
            -61,
        )
        return int(round(base + 2 * math.sin(uptime / 7.0)))

    def build_saved_networks(self) -> list[dict[str, Any]]:
        connected = self.connected_network()
        connected_ssid = connected.ssid if connected else ""
        active_ssid = connected_ssid or (self.saved_networks[0].ssid if self.saved_networks else "")
        return [
            {
                "ssid": network.ssid,
                "hasPass": network.has_password,
                "connected": network.ssid == connected_ssid,
                "active": network.ssid == active_ssid,
            }
            for network in self.saved_networks
        ]

    def build_scan_results(self) -> list[dict[str, Any]]:
        saved_ssids = {network.ssid for network in self.saved_networks}
        return [
            {
                "ssid": item["ssid"],
                "rssi": item["rssi"],
                "secure": item["secure"],
                "saved": item["ssid"] in saved_ssids,
            }
            for item in self.scan_results
        ]

    def active_speed_offset_pct(self) -> int | None:
        if self.hw_mode != 1 or not self.speed_offset_enable:
            return None
        bucket_index = get_speed_offset_bucket_index(self.road_speed_limit)
        if bucket_index < 0:
            return None
        return self.speed_offset_buckets[bucket_index]

    def build_status(self) -> dict[str, Any]:
        self.rx_count += 6
        if self.fsd_enable and self.fsd_triggered and self.can_ok:
            self.modified_count += 2

        uptime = self.uptime_seconds()
        chip_temp_c, chip_temp_avg_c, thermal_status, thermal_protect = self.thermal_snapshot()
        connected = self.connected_network()
        upstream_connected = connected is not None
        upstream_rssi = self.connected_rssi()
        upstream_signal = get_upstream_signal_text(upstream_rssi)
        active_offset = self.active_speed_offset_pct()
        effective_speed_limit = None
        if self.road_speed_limit is not None and active_offset is not None:
            effective_speed_limit = round(
                self.road_speed_limit * (1.0 + (active_offset / 100.0)),
                1,
            )

        if not self.upstream_enable:
            upstream_status = "未启用"
        elif not self.saved_networks:
            upstream_status = "未配置热点"
        elif thermal_protect:
            upstream_status = "过热保护中"
        elif upstream_connected:
            upstream_status = "已连接"
        else:
            upstream_status = "未连接"

        nat_enabled = upstream_connected
        nat_status = "已启用" if nat_enabled else "未连接上游"

        dns_blocked_requests = [
            {
                "domain": item["domain"],
                "count": item["count"],
                "lastBlockedAt": max(0, uptime - item["seconds_ago"]),
            }
            for item in self.dns_blocked_requests
        ]

        return {
            "rx": self.rx_count,
            "modified": self.modified_count,
            "errors": self.error_count,
            "uptime": uptime,
            "chipTempC": chip_temp_c,
            "chipTempAvgC": chip_temp_avg_c,
            "roadSpeedLimit": self.road_speed_limit,
            "visionSpeedLimit": self.vision_speed_limit,
            "roadSpeedLimitAgeMs": int((time.monotonic() - self.road_speed_limit_last_update) * 1000),
            "activeSpeedOffsetPct": active_offset,
            "effectiveSpeedLimit": effective_speed_limit,
            "thermalStatus": thermal_status,
            "thermalProtect": thermal_protect,
            "canOK": self.can_ok,
            "fsdTriggered": self.fsd_triggered,
            "fsdEnable": int(self.fsd_enable),
            "hwMode": self.hw_mode,
            "speedProfile": self.speed_profile,
            "profileMode": int(self.profile_mode_auto),
            "speedOffsetEnable": int(self.speed_offset_enable),
            "speedOffsetBuckets": self.speed_offset_buckets,
            "isaChime": int(self.isa_chime),
            "emergencyDet": int(self.emergency_detection),
            "chinaMode": int(self.china_mode),
            "upstreamEnable": int(self.upstream_enable),
            "upstreamConfigured": int(bool(self.saved_networks)),
            "upstreamConnected": upstream_connected,
            "upstreamRSSI": upstream_rssi,
            "wifiChannel": self.wifi_channel,
            "apClients": self.ap_clients,
            "apPassword": self.ap_password,
            "upstreamSSID": connected.ssid if connected else (self.saved_networks[0].ssid if self.saved_networks else ""),
            "connectedUpstreamSSID": connected.ssid if connected else "",
            "upstreamSavedCount": len(self.saved_networks),
            "upstreamNetworks": self.build_saved_networks(),
            "upstreamStatus": upstream_status,
            "upstreamSignal": upstream_signal,
            "upstreamIP": "192.168.31.23" if upstream_connected else "",
            "apSSID": self.ap_ssid,
            "apIP": self.ap_ip,
            "fwVersion": self.firmware_version,
            "otaUrl": self.ota_url,
            "githubRepo": GITHUB_REPO,
            "githubAssetName": GITHUB_ASSET_NAME,
            "githubLatestDownloadUrl": self.github_latest_download_url(),
            "dnsWhitelistEnable": int(self.dns_whitelist_enable),
            "dnsWhitelistCount": count_dns_rules(self.dns_allowlist),
            "dnsBlacklistCount": count_dns_rules(self.dns_blocklist),
            "dnsAllowlist": self.dns_allowlist,
            "dnsBlocklist": self.dns_blocklist,
            "dnsBlockedCount": sum(item["count"] for item in self.dns_blocked_requests),
            "dnsBlockedRecentCount": len(self.dns_blocked_requests),
            "dnsBlockedRequests": dns_blocked_requests,
            "natEnabled": int(nat_enabled),
            "natStatus": nat_status,
        }

    def update_from_set(self, params: dict[str, list[str]]) -> tuple[int, str]:
        if "fsdEnable" in params:
            self.fsd_enable = to_bool(first_param(params, "fsdEnable"))
        if "hwMode" in params:
            self.hw_mode = clamp(int(first_param(params, "hwMode", str(self.hw_mode))), 0, 2)
        if "speedProfile" in params:
            self.speed_profile = clamp(
                int(first_param(params, "speedProfile", str(self.speed_profile))),
                0,
                4,
            )
        if "profileMode" in params:
            self.profile_mode_auto = to_bool(first_param(params, "profileMode"))
        if "speedOffsetEnable" in params:
            self.speed_offset_enable = to_bool(first_param(params, "speedOffsetEnable"))

        for index in range(SPEED_OFFSET_BUCKET_COUNT):
            key = f"speedOffsetPct{index}"
            if key in params:
                value = clamp(int(first_param(params, key, "0")), 0, 50)
                self.speed_offset_buckets[index] = value

        if "isaChime" in params:
            self.isa_chime = to_bool(first_param(params, "isaChime"))
        if "emergencyDet" in params:
            self.emergency_detection = to_bool(first_param(params, "emergencyDet"))
        if "chinaMode" in params:
            self.china_mode = to_bool(first_param(params, "chinaMode"))

        if "apSSID" in params or "apPass" in params:
            ap_ssid = first_param(params, "apSSID", self.ap_ssid).strip()
            ap_password = first_param(params, "apPass", self.ap_password)

            if not ap_ssid or len(ap_ssid) > 32:
                return HTTPStatus.BAD_REQUEST, "热点名称长度必须为 1-32 个字符"
            if len(ap_password) < 8 or len(ap_password) > 63:
                return HTTPStatus.BAD_REQUEST, "热点密码长度必须为 8-63 个字符"

            self.ap_ssid = ap_ssid
            self.ap_password = ap_password

        if "upstreamEnable" in params:
            self.upstream_enable = to_bool(first_param(params, "upstreamEnable"))
        if "dnsWhitelistEnable" in params:
            self.dns_whitelist_enable = to_bool(first_param(params, "dnsWhitelistEnable"))
        if "dnsAllowlist" in params:
            self.dns_allowlist = first_param(params, "dnsAllowlist").strip()
        if "dnsBlocklist" in params:
            self.dns_blocklist = first_param(params, "dnsBlocklist").strip()
        if "otaUrl" in params:
            ota_url = first_param(params, "otaUrl").strip()
            if ota_url and (len(ota_url) > MAX_OTA_URL_LEN or not ota_url.startswith(("http://", "https://"))):
                return HTTPStatus.BAD_REQUEST, "在线固件地址必须以 http:// 或 https:// 开头"
            self.ota_url = ota_url
        return HTTPStatus.OK, "OK"

    def add_upstream(self, params: dict[str, list[str]]) -> tuple[int, str]:
        if "ssid" not in params:
            return HTTPStatus.BAD_REQUEST, "缺少热点名称"

        ssid = first_param(params, "ssid").strip()
        password = first_param(params, "pass")

        if not ssid:
            return HTTPStatus.BAD_REQUEST, "热点名称不能为空"
        if len(ssid) > 32 or len(password) > 63:
            return HTTPStatus.BAD_REQUEST, "热点名称或密码长度不合法"
        if self.is_reserved_upstream_ssid(ssid):
            return HTTPStatus.BAD_REQUEST, "不能保存本机发射的热点"

        existing = next((network for network in self.saved_networks if network.ssid == ssid), None)
        if existing is None and len(self.saved_networks) >= MAX_UPSTREAM_NETWORKS:
            return HTTPStatus.BAD_REQUEST, "已达到可保存热点上限"

        if existing is None:
            self.saved_networks.append(SavedNetwork(ssid, password))
        elif "pass" in params:
            existing.password = password

        return HTTPStatus.OK, "OK"

    def delete_upstream(self, params: dict[str, list[str]]) -> tuple[int, str]:
        if "ssid" not in params:
            return HTTPStatus.BAD_REQUEST, "缺少热点名称"

        ssid = first_param(params, "ssid").strip()
        remaining = [network for network in self.saved_networks if network.ssid != ssid]
        if len(remaining) == len(self.saved_networks):
            return HTTPStatus.NOT_FOUND, "热点不存在"

        self.saved_networks = remaining
        return HTTPStatus.OK, "OK"

    def clear_blocked_dns(self) -> tuple[int, str]:
        self.dns_blocked_requests = []
        return HTTPStatus.OK, "OK"

    def restart(self) -> None:
        self.started_at = time.monotonic()
        self.rx_count = 0
        self.modified_count = 0
        self.error_count = 0

    def online_ota(self, params: dict[str, list[str]]) -> tuple[int, str]:
        ota_url = first_param(params, "url").strip()
        if not ota_url:
            return HTTPStatus.BAD_REQUEST, "在线固件地址不能为空"
        if len(ota_url) > MAX_OTA_URL_LEN or not ota_url.startswith(("http://", "https://")):
            return HTTPStatus.BAD_REQUEST, "在线固件地址必须以 http:// 或 https:// 开头"

        self.ota_url = ota_url
        if ota_url in {self.github_latest_download_url(), self.github_release_asset_url(self.github_latest_version)}:
            self.firmware_version = self.github_latest_version
        else:
            self.firmware_version = "preview-" + time.strftime("%m%d-%H%M%S")
        self.restart()
        return HTTPStatus.OK, "OK"

    def github_latest(self) -> dict[str, Any]:
        latest_download_url = self.github_latest_download_url()
        return {
            "repo": GITHUB_REPO,
            "assetName": GITHUB_ASSET_NAME,
            "currentVersion": self.firmware_version,
            "latestVersion": self.github_latest_version,
            "assetUrl": self.github_release_asset_url(self.github_latest_version),
            "latestDownloadUrl": latest_download_url,
            "updateAvailable": self.firmware_version != self.github_latest_version,
        }


STATE = PreviewState()
STATE_LOCK = threading.Lock()


class PreviewRequestHandler(BaseHTTPRequestHandler):
    server_version = "ESP32Preview/1.0"

    def do_GET(self) -> None:  # noqa: N802
        parsed = urlparse(self.path)
        params = parse_qs(parsed.query, keep_blank_values=True)

        if parsed.path in {"/", "/index.html"}:
            try:
                html = extract_embedded_html()
            except RuntimeError as exc:
                self.send_text(HTTPStatus.INTERNAL_SERVER_ERROR, str(exc))
                return
            self.send_bytes(HTTPStatus.OK, html.encode("utf-8"), "text/html; charset=utf-8")
            return

        if parsed.path == "/favicon.ico":
            self.send_bytes(HTTPStatus.NO_CONTENT, b"", "image/x-icon")
            return

        if parsed.path == "/api/status":
            with STATE_LOCK:
                payload = STATE.build_status()
            self.send_json(HTTPStatus.OK, payload)
            return

        if parsed.path == "/api/set":
            with STATE_LOCK:
                status, message = STATE.update_from_set(params)
            self.send_text(status, message)
            return

        if parsed.path == "/api/upstream/scan":
            with STATE_LOCK:
                payload = {"results": STATE.build_scan_results()}
            self.send_json(HTTPStatus.OK, payload)
            return

        if parsed.path == "/api/upstream/add":
            with STATE_LOCK:
                status, message = STATE.add_upstream(params)
            self.send_text(status, message)
            return

        if parsed.path == "/api/upstream/delete":
            with STATE_LOCK:
                status, message = STATE.delete_upstream(params)
            self.send_text(status, message)
            return

        if parsed.path == "/api/dns/blocked/clear":
            with STATE_LOCK:
                status, message = STATE.clear_blocked_dns()
            self.send_text(status, message)
            return

        if parsed.path == "/api/ota/github/latest":
            with STATE_LOCK:
                payload = STATE.github_latest()
            self.send_json(HTTPStatus.OK, payload)
            return

        self.send_text(HTTPStatus.NOT_FOUND, "Not Found")

    def do_POST(self) -> None:  # noqa: N802
        parsed = urlparse(self.path)
        length = int(self.headers.get("Content-Length", "0"))
        body = b""
        params: dict[str, list[str]] = {}
        if length > 0:
            body = self.rfile.read(length)

        if parsed.path == "/api/ota/online" and body:
            params = parse_qs(body.decode("utf-8"), keep_blank_values=True)

        if parsed.path == "/api/ota":
            self.send_text(HTTPStatus.OK, "OK")
            return

        if parsed.path == "/api/ota/online":
            with STATE_LOCK:
                status, message = STATE.online_ota(params)
            self.send_text(status, message)
            return

        if parsed.path == "/api/restart":
            with STATE_LOCK:
                STATE.restart()
            self.send_text(HTTPStatus.OK, "OK")
            return

        self.send_text(HTTPStatus.NOT_FOUND, "Not Found")

    def log_message(self, format: str, *args: Any) -> None:
        print(
            f"[{self.log_date_time_string()}] "
            f"{self.address_string()} {format % args}"
        )

    def send_json(self, status: int, payload: dict[str, Any]) -> None:
        data = json.dumps(payload, ensure_ascii=False).encode("utf-8")
        self.send_bytes(status, data, "application/json; charset=utf-8")

    def send_text(self, status: int, message: str) -> None:
        self.send_bytes(status, message.encode("utf-8"), "text/plain; charset=utf-8")

    def send_bytes(self, status: int, body: bytes, content_type: str) -> None:
        self.send_response(status)
        self.send_header("Content-Type", content_type)
        self.send_header("Content-Length", str(len(body)))
        self.send_header("Cache-Control", "no-store")
        self.end_headers()
        if body:
            self.wfile.write(body)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Preview the embedded ESP32 web UI locally with mock APIs.",
    )
    parser.add_argument("--host", default="127.0.0.1", help="Host interface to bind.")
    parser.add_argument("--port", type=int, default=8000, help="Port to listen on.")
    return parser.parse_args()


def main() -> None:
    args = parse_args()
    server = ThreadingHTTPServer((args.host, args.port), PreviewRequestHandler)
    print(f"Serving preview for {WEB_UI_HEADER} at http://{args.host}:{args.port}")
    print("Refresh the browser after editing include/web_ui.h to see the latest UI.")
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        print("\nStopping preview server...")
    finally:
        server.server_close()


if __name__ == "__main__":
    main()
