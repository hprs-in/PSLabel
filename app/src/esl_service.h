/*
 * Copyright (c) 2025 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ESL_SERVICE_H_
#define ESL_SERVICE_H_

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/uuid.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Electronic Shelf Label (ESL) Service UUID
 * As defined in Bluetooth ESL specification
 */
#define ESL_SERVICE_UUID_VAL \
	BT_UUID_128_ENCODE(0x0000181D, 0x0000, 0x1000, 0x8000, 0x00805f9b34fb)

/**
 * @brief ESL Control Point Characteristic UUID
 */
#define ESL_CONTROL_POINT_UUID_VAL \
	BT_UUID_128_ENCODE(0x00002BE1, 0x0000, 0x1000, 0x8000, 0x00805f9b34fb)

/**
 * @brief ESL Image Transfer Characteristic UUID
 */
#define ESL_IMAGE_TRANSFER_UUID_VAL \
	BT_UUID_128_ENCODE(0x00002BE2, 0x0000, 0x1000, 0x8000, 0x00805f9b34fb)

/**
 * @brief ESL Display Information Characteristic UUID
 */
#define ESL_DISPLAY_INFO_UUID_VAL \
	BT_UUID_128_ENCODE(0x00002BE3, 0x0000, 0x1000, 0x8000, 0x00805f9b34fb)

/**
 * @brief ESL Status Characteristic UUID
 */
#define ESL_STATUS_UUID_VAL \
	BT_UUID_128_ENCODE(0x00002BE4, 0x0000, 0x1000, 0x8000, 0x00805f9b34fb)

/* Define UUIDs */
#define ESL_SERVICE_UUID          BT_UUID_DECLARE_128(ESL_SERVICE_UUID_VAL)
#define ESL_CONTROL_POINT_UUID    BT_UUID_DECLARE_128(ESL_CONTROL_POINT_UUID_VAL)
#define ESL_IMAGE_TRANSFER_UUID   BT_UUID_DECLARE_128(ESL_IMAGE_TRANSFER_UUID_VAL)
#define ESL_DISPLAY_INFO_UUID     BT_UUID_DECLARE_128(ESL_DISPLAY_INFO_UUID_VAL)
#define ESL_STATUS_UUID           BT_UUID_DECLARE_128(ESL_STATUS_UUID_VAL)

/**
 * @brief ESL Control Point Commands
 */
enum esl_control_cmd {
	ESL_CMD_UPDATE_DISPLAY = 0x01,
	ESL_CMD_CLEAR_DISPLAY = 0x02,
	ESL_CMD_SLEEP = 0x03,
	ESL_CMD_WAKE = 0x04,
	ESL_CMD_GET_STATUS = 0x05,
};

/**
 * @brief ESL Status Values
 */
enum esl_status {
	ESL_STATUS_IDLE = 0x00,
	ESL_STATUS_UPDATING = 0x01,
	ESL_STATUS_SLEEPING = 0x02,
	ESL_STATUS_ERROR = 0xFF,
};

/**
 * @brief ESL Display Information Structure
 */
struct esl_display_info {
	uint16_t width;
	uint16_t height;
	uint8_t color_depth;
	uint8_t display_type;
} __packed;

/**
 * @brief Initialize the ESL service
 * 
 * @return 0 on success, negative errno code on failure
 */
int esl_service_init(void);

/**
 * @brief Send notification to ESL Status characteristic
 * 
 * @param status The status value to send
 * @return 0 on success, negative errno code on failure
 */
int esl_service_notify_status(uint8_t status);

/**
 * @brief Get current ESL status
 * 
 * @return Current ESL status
 */
uint8_t esl_service_get_status(void);

/**
 * @brief Set ESL display information
 * 
 * @param info Pointer to display information structure
 * @return 0 on success, negative errno code on failure
 */
int esl_service_set_display_info(const struct esl_display_info *info);

#ifdef __cplusplus
}
#endif

#endif /* ESL_SERVICE_H_ */