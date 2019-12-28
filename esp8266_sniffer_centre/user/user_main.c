/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2016 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"

#include "driver/uart.h"

/******************************************************************************
 * FunctionName : user_rf_cal_sector_set
 * Description  : SDK just reversed 4 sectors, used for rf init data and paramters.
 *                We add this function to force users to set rf cal sector, since
 *                we don't know which sector is free in user's application.
 *                sector map for last several sectors : ABCCC
 *                A : rf cal
 *                B : rf init data
 *                C : sdk parameters
 * Parameters   : none
 * Returns      : rf cal sector
*******************************************************************************/
uint32 ICACHE_FLASH_ATTR
user_rf_cal_sector_set(void)
{
    enum flash_size_map size_map = system_get_flash_size_map();
    uint32 rf_cal_sec = 0;

    switch (size_map) {
        case FLASH_SIZE_4M_MAP_256_256:
            rf_cal_sec = 128 - 5;
            break;

        case FLASH_SIZE_8M_MAP_512_512:
            rf_cal_sec = 256 - 5;
            break;

        case FLASH_SIZE_16M_MAP_512_512:
        case FLASH_SIZE_16M_MAP_1024_1024:
            rf_cal_sec = 512 - 5;
            break;

        case FLASH_SIZE_32M_MAP_512_512:
        case FLASH_SIZE_32M_MAP_1024_1024:
            rf_cal_sec = 1024 - 5;
            break;

        default:
            rf_cal_sec = 0;
            break;
    }

    return rf_cal_sec;
}

void ICACHE_FLASH_ATTR
user_rf_pre_init(void)
{
}

void ICACHE_FLASH_ATTR
system_init_done(void)
{
	/****  设置dhcp  ****/
	wifi_softap_dhcps_stop();
	struct dhcps_lease dhcp_lease;
	const char* start_ip = "192.168.4.10";
	const char* end_ip = "192.168.4.30";
	dhcp_lease.start_ip.addr = ipaddr_addr(start_ip);
	dhcp_lease.end_ip.addr = ipaddr_addr(end_ip);
	wifi_softap_set_dhcps_lease(&dhcp_lease);
	wifi_softap_dhcps_start();

	tcp_server_init(8080);
}

//#define __DEBUG__


/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR
user_init(void)
{
	uart_init(BIT_RATE_115200, BIT_RATE_115200);

#ifdef __DEBUG__
	system_uart_de_swap();
	UART_SetPrintPort(0);
#else
	UART_SetPrintPort(1); // os_printf转为UART1输出
	system_uart_swap(); // UART0转换，将TX转换为GPIO15(D8)，RX转换为GPIO13(D7)
#endif


	//----- wifi test -----------
	struct softap_config ap;
	wifi_set_opmode(SOFTAP_MODE);
	os_memset(ap.ssid, 0, 32);
	os_memset(ap.password, 0, 64);
	os_memcpy(ap.ssid, "Centre",6);
	os_memcpy(ap.password, "12345678", 8);
	ap.authmode = AUTH_WPA_PSK;
	ap.ssid_len = 0;
	ap.max_connection = 4;
	wifi_softap_set_config(&ap);

    // Continue to 'sniffer_system_init_done'
    system_init_done_cb(system_init_done);


}
