/**
 * Copyright (c) 2022 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

const uint32_t blink_pattern_retry_ms = 100;
const uint32_t blink_pattern_halt_ms = 250;
const uint32_t blink_pattern_success_ms = 1000;

const uint32_t wifi_timeout_ms = 1000;

#ifdef WIFI_CREDENTIALS_FILE
#include WIFI_CREDENTIALS_FILE
#endif

void blink(uint32_t ms)
{
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
    sleep_ms(ms);
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    sleep_ms(ms);
}

void halt()
{
    while (true) {
        blink(blink_pattern_halt_ms);
    }
}

void retry()
{
    blink(blink_pattern_retry_ms);
    blink(blink_pattern_retry_ms);
}

int main() {
    /* Initialize wifi driver (and LED GPIO) */
    if (cyw43_arch_init() != 0) {
        return -1;
    }

    /* Tell the world we're firing up by solid light */
    /* TODO: RGB color theme */
    /* TODO: Blink pattern in separate thread/core? */
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

    /* Connect to wifi */
    while (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PW, CYW43_AUTH_WPA2_AES_PSK, wifi_timeout_ms) != 0)
    {
        retry();
    }

    /* Message success */
    while(true)
    {
        blink(blink_pattern_success_ms);
    }
}
