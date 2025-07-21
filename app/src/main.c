/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gap.h>

#include <app_version.h>
#include "esl_service.h"

LOG_MODULE_REGISTER(main, CONFIG_APP_LOG_LEVEL);

/* Advertisement data */
static const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
	BT_DATA_BYTES(BT_DATA_UUID128_ALL, ESL_SERVICE_UUID_VAL),
	BT_DATA(BT_DATA_NAME_COMPLETE, CONFIG_BT_DEVICE_NAME, sizeof(CONFIG_BT_DEVICE_NAME) - 1),
};

/* Scan response data */
static const struct bt_data sd[] = {
	BT_DATA(BT_DATA_NAME_COMPLETE, CONFIG_BT_DEVICE_NAME, sizeof(CONFIG_BT_DEVICE_NAME) - 1),
};

static void bt_ready(int err)
{
	if (err) {
		LOG_ERR("Bluetooth init failed (err %d)", err);
		return;
	}

	LOG_INF("Bluetooth initialized");

	/* Initialize ESL service */
	err = esl_service_init();
	if (err) {
		LOG_ERR("ESL service init failed (err %d)", err);
		return;
	}

	/* Start advertising */
	err = bt_le_adv_start(BT_LE_ADV_CONN_NAME, ad, ARRAY_SIZE(ad), 
			      sd, ARRAY_SIZE(sd));
	if (err) {
		LOG_ERR("Advertising failed to start (err %d)", err);
		return;
	}

	LOG_INF("PSLabel ESL Service advertising started");
}

int main(void)
{
	int err;

	LOG_INF("PSLabel ESL Application %s", APP_VERSION_STRING);

	/* Initialize Bluetooth */
	err = bt_enable(bt_ready);
	if (err) {
		LOG_ERR("Bluetooth init failed (err %d)", err);
		return 0;
	}

	/* Set default display information */
	struct esl_display_info info = {
		.width = 200,
		.height = 200,
		.color_depth = 1,
		.display_type = 0x01, /* e-ink */
	};
	esl_service_set_display_info(&info);

	LOG_INF("ESL Service ready - waiting for BLE connections");

	while (1) {
		/* Main application loop */
		k_sleep(K_SECONDS(1));
		
		/* Periodically log status for debugging */
		static int counter = 0;
		if (++counter % 30 == 0) {
			LOG_INF("ESL Status: 0x%02x", esl_service_get_status());
		}
	}

	return 0;
}
