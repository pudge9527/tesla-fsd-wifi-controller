#pragma once

#include "can_driver.h"

#include "soc/soc_caps.h"

#if defined(SOC_TWAI_SUPPORT_FD)

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

#include <esp_twai.h>
#include <esp_twai_onchip.h>

#ifndef TWAI_TX_PIN
#define TWAI_TX_PIN GPIO_NUM_5
#endif
#ifndef TWAI_RX_PIN
#define TWAI_RX_PIN GPIO_NUM_4
#endif

struct TWAIDriver : public CanDriver {
    gpio_num_t txPin, rxPin;
    twai_node_handle_t nodeHandle = nullptr;
    QueueHandle_t rxQueue = nullptr;
    static constexpr bool kSupportsISR = false;

    TWAIDriver(gpio_num_t tx = (gpio_num_t)TWAI_TX_PIN, gpio_num_t rx = (gpio_num_t)TWAI_RX_PIN)
        : txPin(tx), rxPin(rx) {}

    ~TWAIDriver() override {
        if (nodeHandle) {
            twai_node_disable(nodeHandle);
            twai_node_delete(nodeHandle);
        }
        if (rxQueue) {
            vQueueDelete(rxQueue);
        }
    }

    static bool IRAM_ATTR onRxDone(twai_node_handle_t handle, const twai_rx_done_event_data_t*, void* userCtx) {
        auto* self = static_cast<TWAIDriver*>(userCtx);
        if (!self || !self->rxQueue) return false;

        CanFrame frame = {};
        twai_frame_t rxFrame = {
            .header = {},
            .buffer = frame.data,
            .buffer_len = sizeof(frame.data),
        };
        if (twai_node_receive_from_isr(handle, &rxFrame) != ESP_OK) return false;

        frame.id = rxFrame.header.id;
        frame.dlc = static_cast<uint8_t>(rxFrame.header.dlc > 8 ? 8 : rxFrame.header.dlc);

        BaseType_t taskWoken = pdFALSE;
        xQueueSendFromISR(self->rxQueue, &frame, &taskWoken);
        return taskWoken == pdTRUE;
    }

    bool init() override {
        rxQueue = xQueueCreate(32, sizeof(CanFrame));
        if (!rxQueue) return false;

        twai_onchip_node_config_t nodeConfig = {
            .io_cfg = {
                .tx = txPin,
                .rx = rxPin,
                .quanta_clk_out = GPIO_NUM_NC,
                .bus_off_indicator = GPIO_NUM_NC,
            },
            .bit_timing = {
                .bitrate = 500000,
            },
            .tx_queue_depth = 16,
        };
        if (twai_new_node_onchip(&nodeConfig, &nodeHandle) != ESP_OK) return false;

        const twai_event_callbacks_t callbacks = {
            .on_rx_done = &TWAIDriver::onRxDone,
        };
        if (twai_node_register_event_callbacks(nodeHandle, &callbacks, this) != ESP_OK) return false;

        twai_mask_filter_config_t stdAcceptAll = {};
        stdAcceptAll.id = 0;
        stdAcceptAll.mask = 0;
        stdAcceptAll.is_ext = false;
        stdAcceptAll.no_fd = true;
        if (twai_node_config_mask_filter(nodeHandle, 0, &stdAcceptAll) != ESP_OK) return false;

        return twai_node_enable(nodeHandle) == ESP_OK;
    }

    bool send(const CanFrame& frame) override {
        if (!nodeHandle) return false;
        uint8_t buffer[8] = {};
        memcpy(buffer, frame.data, sizeof(buffer));
        twai_frame_t txFrame = {
            .header = {
                .id = frame.id,
                .dlc = frame.dlc,
            },
            .buffer = buffer,
            .buffer_len = frame.dlc,
        };
        return twai_node_transmit(nodeHandle, &txFrame, 5) == ESP_OK;
    }

    bool read(CanFrame& frame) override {
        if (!rxQueue) return false;
        return xQueueReceive(rxQueue, &frame, 0) == pdTRUE;
    }

    void setFilters(const uint32_t*, uint8_t) override {
        // C5 filter count/layout differs from classic ESP32; keep software filtering for now.
    }
};

#else

#include <driver/twai.h>

#ifndef TWAI_TX_PIN
#define TWAI_TX_PIN GPIO_NUM_5
#endif
#ifndef TWAI_RX_PIN
#define TWAI_RX_PIN GPIO_NUM_4
#endif

struct TWAIDriver : public CanDriver {
    gpio_num_t txPin, rxPin;
    static constexpr bool kSupportsISR = false;

    TWAIDriver(gpio_num_t tx = (gpio_num_t)TWAI_TX_PIN, gpio_num_t rx = (gpio_num_t)TWAI_RX_PIN)
        : txPin(tx), rxPin(rx) {}

    bool init() override {
        twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(txPin, rxPin, TWAI_MODE_NORMAL);
        g_config.rx_queue_len = 32;
        g_config.tx_queue_len = 16;
        twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
        twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
        if (twai_driver_install(&g_config, &t_config, &f_config) != ESP_OK) return false;
        return twai_start() == ESP_OK;
    }

    bool send(const CanFrame& frame) override {
        twai_message_t msg = {};
        msg.identifier = frame.id;
        msg.data_length_code = frame.dlc;
        memcpy(msg.data, frame.data, 8);
        return twai_transmit(&msg, pdMS_TO_TICKS(5)) == ESP_OK;
    }

    bool read(CanFrame& frame) override {
        twai_message_t msg;
        if (twai_receive(&msg, 0) != ESP_OK) return false;
        frame.id = msg.identifier;
        frame.dlc = msg.data_length_code;
        memcpy(frame.data, msg.data, 8);
        return true;
    }

    void setFilters(const uint32_t*, uint8_t) override {
        // TWAI hardware filter is limited; we filter in software
    }
};

#endif
