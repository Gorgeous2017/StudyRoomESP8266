/*
 * sniffer.c
 */

#include "driver/uart.h"
#include "user_config.h"
#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"
#include "mem.h"
#include "user_interface.h"
#include "sniffer.h"

#include "sniffer.h"
#include "http_client.h"
#include "MACCMP.h"

#if HOP_JUMP_ENABLE
static volatile os_timer_t channelHop_timer;
#endif

uint8_t temp_mac[6] = {0xc4, 0x6a, 0xb7, 0x9f, 0xcc, 0x34};

#if HOP_JUMP_ENABLE
void ICACHE_FLASH_ATTR
channelHop(void *arg)
{
    // 1 - 13 channel hopping
    uint8 new_channel = wifi_get_channel() % 12 + 1;
    //os_printf("** hop to %d **\t    Client MAC\t\t    AP MAC\r\n", new_channel);
    wifi_set_channel(new_channel);
}
#endif

char mac_buf[5]={0x00,0x00,0x00,0x00};

/* Listens communication between AP and client */
static void ICACHE_FLASH_ATTR
promisc_cb(uint8_t *buf, uint16_t len)
{

    if (len == 12){
        struct RxControl *sniffer = (struct RxControl*) buf;
    } else if (len == 128) {
        struct sniffer_buf2 *sniffer = (struct sniffer_buf2*) buf;
    } else {
        struct sniffer_buf *sniffer = (struct sniffer_buf*) buf;

        int i=0;
        // Check MACs

        // 如果MAC地址和上一次一样就返回
		if(0==os_memcmp(temp_mac, &sniffer->buf[4], 6)){
			return;
		}

		// 缓存上次的MAC，避免重复打印
		for (i=0; i<6; i++){
			temp_mac[i] = sniffer->buf[i+4];
		}

#if SNIFFER_TEST

		os_sprintf(mac_buf,"%02X%02X%02X",*((sniffer->buf)+7),*((sniffer->buf)+8),*((sniffer->buf)+9));
		add(MAC, mac_buf);

#endif
	}
}


void ICACHE_FLASH_ATTR
sniffer_init(void)
{
    // Promiscuous works only with station mode
    wifi_set_opmode(STATION_MODE);

#if HOP_JUMP_ENABLE
    os_timer_disarm(&channelHop_timer);
    os_timer_setfn(&channelHop_timer, (os_timer_func_t *) channelHop, NULL);
    os_timer_arm(&channelHop_timer, CHANNEL_HOP_INTERVAL, 1);
#endif

}

void ICACHE_FLASH_ATTR
sniffer_init_in_system_init_done(void)
{
    // Set up promiscuous callback
    wifi_set_channel(1);
    wifi_promiscuous_enable(0);
    wifi_set_promiscuous_rx_cb(promisc_cb);
    wifi_promiscuous_enable(1);
}
