/**
 * Copyright (c) 2022 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

// cyw43_arch_init etc.
#include <pico/cyw43_arch.h>

// uint32_t
#include <stdint.h>

const uint32_t blink_pattern_retry_ms = 250;
const uint32_t blink_pattern_halt_ms = 500;
const uint32_t blink_pattern_success_ms = 1000;

const uint32_t wifi_timeout_ms = 10000;

#ifdef WIFI_CREDENTIALS_FILE
#include WIFI_CREDENTIALS_FILE
#endif

void blink(uint32_t ms, uint32_t iterations)
{
    while (iterations--)
    {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        sleep_ms(ms);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        sleep_ms(ms);
    }
}

void halt()
{
    while (true) {
        blink(blink_pattern_halt_ms, 1);
    }
}

void retry()
{
    blink(blink_pattern_retry_ms, 2);
}

static unsigned heartbeat = 0;

int main() {
    /* Initialize wifi driver (and LED GPIO) */
    if (cyw43_arch_init_with_country(CYW43_COUNTRY_SWEDEN) != 0) {
        return -1;
    }

    /* Tell the world we're firing up by solid light */
    /* TODO: RGB color theme */
    /* TODO: Blink pattern in separate thread/core? */
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

    /* Enable STAtion mode */
    cyw43_arch_enable_sta_mode();

    /* Connect to wifi */
    int code;
    while (code = cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PW, CYW43_AUTH_WPA2_AES_PSK, wifi_timeout_ms) != 0)
    {
        blink(blink_pattern_retry_ms, abs(code));
    }

    /* Success */

    /* Setup lwip udp */
    ip_addr_t server_ip;
    IP4_ADDR(&server_ip, 192, 168, 1, 109); // as seen in router network map, 2023-02-16
    u16_t port = 44444; // unregistered IANA port
    struct udp_pcb* my_udp = udp_new_ip_type(IPADDR_TYPE_V4);
    struct pbuf* udp_buffer = NULL;
    udp_buffer = pbuf_alloc(PBUF_TRANSPORT, sizeof(heartbeat), PBUF_RAM);

    while(true)
    {
        blink(blink_pattern_success_ms, 1);
        heartbeat++;
        memcpy(udp_buffer->payload, &heartbeat, sizeof(heartbeat));
        err_t status = udp_sendto(my_udp, udp_buffer, &server_ip, port);
        if (status)
        {
            halt();
        }
    }
}
