/*
 * Copyright (c) 2025 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esl_service.h"
#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>
#include <string.h>

LOG_MODULE_REGISTER(esl_service, CONFIG_APP_LOG_LEVEL);

/* ESL service data */
static uint8_t esl_status = ESL_STATUS_IDLE;
static struct esl_display_info display_info = {
	.width = 200,
	.height = 200,
	.color_depth = 1, /* 1-bit (black/white) */
	.display_type = 0x01, /* e-ink */
};

/* Image transfer buffer */
#define ESL_IMAGE_BUFFER_SIZE 1024
static uint8_t image_buffer[ESL_IMAGE_BUFFER_SIZE];
static size_t image_offset = 0;
static size_t image_total_size = 0;

/* Connection reference for notifications */
static struct bt_conn *notification_conn = NULL;

/* Forward declarations */
static ssize_t esl_control_point_write(struct bt_conn *conn,
					const struct bt_gatt_attr *attr,
					const void *buf, uint16_t len,
					uint16_t offset, uint8_t flags);

static ssize_t esl_image_transfer_write(struct bt_conn *conn,
					const struct bt_gatt_attr *attr,
					const void *buf, uint16_t len,
					uint16_t offset, uint8_t flags);

static ssize_t esl_display_info_read(struct bt_conn *conn,
				      const struct bt_gatt_attr *attr,
				      void *buf, uint16_t len, uint16_t offset);

static ssize_t esl_status_read(struct bt_conn *conn,
			       const struct bt_gatt_attr *attr,
			       void *buf, uint16_t len, uint16_t offset);

static void esl_status_ccc_changed(const struct bt_gatt_attr *attr, uint16_t value);

/* ESL Service Declaration */
BT_GATT_SERVICE_DEFINE(esl_service,
	BT_GATT_PRIMARY_SERVICE(ESL_SERVICE_UUID),

	/* ESL Control Point Characteristic */
	BT_GATT_CHARACTERISTIC(ESL_CONTROL_POINT_UUID,
			       BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP,
			       BT_GATT_PERM_WRITE,
			       NULL, esl_control_point_write, NULL),

	/* ESL Image Transfer Characteristic */
	BT_GATT_CHARACTERISTIC(ESL_IMAGE_TRANSFER_UUID,
			       BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP,
			       BT_GATT_PERM_WRITE,
			       NULL, esl_image_transfer_write, NULL),

	/* ESL Display Information Characteristic */
	BT_GATT_CHARACTERISTIC(ESL_DISPLAY_INFO_UUID,
			       BT_GATT_CHRC_READ,
			       BT_GATT_PERM_READ,
			       esl_display_info_read, NULL, NULL),

	/* ESL Status Characteristic */
	BT_GATT_CHARACTERISTIC(ESL_STATUS_UUID,
			       BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
			       BT_GATT_PERM_READ,
			       esl_status_read, NULL, NULL),
	BT_GATT_CCC(esl_status_ccc_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
);

/* ESL Control Point write handler */
static ssize_t esl_control_point_write(struct bt_conn *conn,
					const struct bt_gatt_attr *attr,
					const void *buf, uint16_t len,
					uint16_t offset, uint8_t flags)
{
	const uint8_t *data = buf;
	
	if (len < 1) {
		LOG_WRN("Invalid control point command length: %d", len);
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
	}

	uint8_t cmd = data[0];
	LOG_INF("ESL Control Point command: 0x%02x", cmd);

	switch (cmd) {
	case ESL_CMD_UPDATE_DISPLAY:
		LOG_INF("Update display command received");
		esl_status = ESL_STATUS_UPDATING;
		esl_service_notify_status(esl_status);
		/* Simulate display update */
		k_sleep(K_MSEC(100));
		esl_status = ESL_STATUS_IDLE;
		esl_service_notify_status(esl_status);
		break;

	case ESL_CMD_CLEAR_DISPLAY:
		LOG_INF("Clear display command received");
		esl_status = ESL_STATUS_UPDATING;
		esl_service_notify_status(esl_status);
		/* Clear image buffer */
		memset(image_buffer, 0, sizeof(image_buffer));
		image_offset = 0;
		image_total_size = 0;
		esl_status = ESL_STATUS_IDLE;
		esl_service_notify_status(esl_status);
		break;

	case ESL_CMD_SLEEP:
		LOG_INF("Sleep command received");
		esl_status = ESL_STATUS_SLEEPING;
		esl_service_notify_status(esl_status);
		break;

	case ESL_CMD_WAKE:
		LOG_INF("Wake command received");
		esl_status = ESL_STATUS_IDLE;
		esl_service_notify_status(esl_status);
		break;

	case ESL_CMD_GET_STATUS:
		LOG_INF("Get status command received");
		esl_service_notify_status(esl_status);
		break;

	default:
		LOG_WRN("Unknown control point command: 0x%02x", cmd);
		return BT_GATT_ERR(BT_ATT_ERR_VALUE_NOT_ALLOWED);
	}

	return len;
}

/* ESL Image Transfer write handler */
static ssize_t esl_image_transfer_write(struct bt_conn *conn,
					const struct bt_gatt_attr *attr,
					const void *buf, uint16_t len,
					uint16_t offset, uint8_t flags)
{
	const uint8_t *data = buf;
	
	LOG_INF("Image transfer: %d bytes at offset %d", len, offset);

	/* Handle image data transfer */
	if (offset == 0) {
		/* First packet - could contain size information */
		if (len >= 4) {
			/* Assume first 4 bytes contain total image size */
			image_total_size = (data[0] << 24) | (data[1] << 16) | 
					   (data[2] << 8) | data[3];
			LOG_INF("Image total size: %zu bytes", image_total_size);
			
			/* Copy remaining data */
			if (len > 4) {
				size_t copy_len = MIN(len - 4, ESL_IMAGE_BUFFER_SIZE);
				memcpy(image_buffer, data + 4, copy_len);
				image_offset = copy_len;
			}
		}
	} else {
		/* Subsequent packets */
		size_t copy_len = MIN(len, ESL_IMAGE_BUFFER_SIZE - image_offset);
		if (copy_len > 0) {
			memcpy(image_buffer + image_offset, data, copy_len);
			image_offset += copy_len;
		}
	}

	LOG_INF("Image buffer: %zu/%zu bytes", image_offset, image_total_size);

	/* Check if transfer is complete */
	if (image_total_size > 0 && image_offset >= image_total_size) {
		LOG_INF("Image transfer complete");
		/* Here you would trigger actual display update */
	}

	return len;
}

/* ESL Display Information read handler */
static ssize_t esl_display_info_read(struct bt_conn *conn,
				      const struct bt_gatt_attr *attr,
				      void *buf, uint16_t len, uint16_t offset)
{
	LOG_INF("Display info read requested");
	return bt_gatt_attr_read(conn, attr, buf, len, offset, 
				 &display_info, sizeof(display_info));
}

/* ESL Status read handler */
static ssize_t esl_status_read(struct bt_conn *conn,
			       const struct bt_gatt_attr *attr,
			       void *buf, uint16_t len, uint16_t offset)
{
	LOG_INF("Status read: 0x%02x", esl_status);
	return bt_gatt_attr_read(conn, attr, buf, len, offset, 
				 &esl_status, sizeof(esl_status));
}

/* ESL Status CCC changed handler */
static void esl_status_ccc_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
	bool notifications_enabled = (value == BT_GATT_CCC_NOTIFY);
	LOG_INF("Status notifications %s", notifications_enabled ? "enabled" : "disabled");
}

/* Public API functions */

int esl_service_init(void)
{
	LOG_INF("ESL Service initialized");
	return 0;
}

int esl_service_notify_status(uint8_t status)
{
	int err;
	
	if (notification_conn) {
		err = bt_gatt_notify(notification_conn, &esl_service.attrs[7], 
				     &status, sizeof(status));
		if (err) {
			LOG_WRN("Failed to send status notification: %d", err);
			return err;
		}
		LOG_INF("Status notification sent: 0x%02x", status);
	}
	
	esl_status = status;
	return 0;
}

uint8_t esl_service_get_status(void)
{
	return esl_status;
}

int esl_service_set_display_info(const struct esl_display_info *info)
{
	if (!info) {
		return -EINVAL;
	}
	
	memcpy(&display_info, info, sizeof(display_info));
	LOG_INF("Display info updated: %dx%d, depth: %d, type: %d",
		info->width, info->height, info->color_depth, info->display_type);
	
	return 0;
}

/* Bluetooth connection callbacks */
static void connected(struct bt_conn *conn, uint8_t err)
{
	if (err) {
		LOG_ERR("Connection failed (err 0x%02x)", err);
		return;
	}

	notification_conn = bt_conn_ref(conn);
	LOG_INF("ESL Service: Connected");
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
	LOG_INF("ESL Service: Disconnected (reason 0x%02x)", reason);
	
	if (notification_conn) {
		bt_conn_unref(notification_conn);
		notification_conn = NULL;
	}
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
	.connected = connected,
	.disconnected = disconnected,
};