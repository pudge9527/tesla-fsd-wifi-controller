# Tesla FSD WIFI Controller — ESP32 / ESP32-C3 / ESP32-S3 / ESP32-C5 / ESP32-C6 Web 版

基于 [Tesla FSD Controller](https://github.com/JelloEa/tesla-fsd-controller) 修改。教程请参考Tesla FSD Controller项目。


---

## ⚠️ 重要声明（请先读完）

1. 代码基于AI codex编写，不保证100%没问题，请自行测试
2. 板子同时接收热点又发射热点，可能会导致发热，需自行斟酌
3. 本项目需要外接 CAN 收发器，不能只接开发板本体直接上车

---

## 新增功能
- wifi热点转发（手机共享热点给esp32 --> esp32共享热点给车机）
- dns白名单、黑名单
- 分区间限速偏移
- 允许车机直接访问-浏览器输入9.9.9.9
- 支持页面直接修改wifi名称密码
- 支持在线固件更新

## 支持板子
- `ESP32`
- `ESP32-C3`
- `ESP32-S3`
- `ESP32-C5`
- `ESP32-C6`

## Release 固件说明
- `*-ota.bin`
  升级包，只包含应用镜像，适用于网页上传 OTA / 在线更新
- `*-full.bin`
  完整包，包含 bootloader + partition table + app，适用于空板首刷 / 救砖

## 下载时请同时对应好
- 板子型号
- `ota` 还是 `full`

## 自动更新说明
- 网页里的“检查更新 / 一键更新”走的是 `*-ota.bin`
- `*-full.bin` 只用于首刷、串口刷写、救砖，不参与在线自动更新

## 默认硬件配置
当前代码里的默认板级配置如下：

| 板子 | 固件环境 | 默认 TWAI TX | 默认 TWAI RX | 默认板载 LED |
| --- | --- | --- | --- | --- |
| ESP32 DevKit | `esp32` | `GPIO5` | `GPIO4` | `GPIO2` |
| ESP32-C3-DevKitM-1 | `esp32c3` | `GPIO5` | `GPIO4` | `GPIO8` |
| ESP32-S3-DevKitC-1 | `esp32s3` | `GPIO5` | `GPIO4` | `GPIO48` |
| ESP32-C5-WIFI6-KIT-N16R4 | `esp32c5` | `GPIO5` | `GPIO4` | `GPIO27` |
| ESP32-C6-DevKitC-1 | `esp32c6` | `GPIO5` | `GPIO4` | `GPIO8` |

如果你的实际接线不同，可以直接改 [platformio.ini](./platformio.ini) 里的 `build_flags`。
如果你的 `ESP32-S3-DevKitC-1` 是较新的 `v1.1` 修订板，板载 RGB LED 可能在 `GPIO38`，这时请把 `PIN_LED` 从 `48` 改成 `38`。

## 源码编译
编译老 ESP32：

```bash
pio run -e esp32
```

编译 ESP32-C5：

```bash
pio run -e esp32c5
```

编译 ESP32-C3：

```bash
pio run -e esp32c3
```

编译 ESP32-S3：

```bash
pio run -e esp32s3
```

编译 ESP32-C6：

```bash
pio run -e esp32c6
```

编译产物分别在：

- `.pio/build/esp32/fsd-controller-esp32.bin`
- `.pio/build/esp32c3/fsd-controller-esp32c3.bin`
- `.pio/build/esp32s3/fsd-controller-esp32s3.bin`
- `.pio/build/esp32c5/fsd-controller-esp32c5.bin`
- `.pio/build/esp32c6/fsd-controller-esp32c6.bin`

如果要在本地额外生成 `ota/full` 两种包，可以执行：

```bash
python3 tools/package_firmware.py \
  --build-dir .pio/build/esp32c5 \
  --app-bin fsd-controller-esp32c5.bin \
  --ota-name fsd-controller-esp32c5-ota.bin \
  --full-name fsd-controller-esp32c5-full.bin \
  --flash-size 16MB \
  --output-dir dist/esp32c5
```

## 烧录
烧录老 ESP32：

```bash
pio run -e esp32 -t upload
```

烧录 ESP32-C5：

```bash
pio run -e esp32c5 -t upload
```

烧录 ESP32-C3：

```bash
pio run -e esp32c3 -t upload
```

烧录 ESP32-S3：

```bash
pio run -e esp32s3 -t upload
```

烧录 ESP32-C6：

```bash
pio run -e esp32c6 -t upload
```

如果有多个串口设备，可以手动指定：

```bash
pio run -e esp32c5 -t upload --upload-port <你的串口>
```

## 第一次使用
1. 手机连接板子发射的热点
2. 进入FSD控制台（9.9.9.9）
3. 设置白名单（不知道设置什么可以先只设置baidu.com）
4. 开启白名单
5. 手机忽略板子发射的热点
6. 车机连接板子发射的热点
7. 车机浏览器进入FSD控制台（9.9.9.9）
8. 开启手机热点
9. 车机FSD控制台搜索并连接手机热点

## 提示
- 板子一般只支持 2.4G Wi-Fi，iPhone 共享热点可能需要打开兼容性模式
- `ESP32-C3 / ESP32-C6` 这里先按通用开发板做了默认引脚适配，板载 LED 如果不亮，优先检查你买到的板子修订版和引脚图
- `ESP32-S3` 这边默认按通用 `DevKitC-1` 配置处理，如果你的板载 LED 不亮，优先检查是不是 `GPIO38 / GPIO48` 差异
- `ESP32-C5` 是单核芯片，因此固件是单独适配并单独打包的，不能直接刷老 `ESP32` 的 bin
