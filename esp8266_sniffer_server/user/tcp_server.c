/*
 * tcp_server.c
 *
 *  Created on: 2017骞�7鏈�3鏃�
 *      Author: Administrator
 */

#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"
#include "mem.h"
#include "espconn.h"

#include "tcp_server.h"
#include "room_info.h"

struct espconn *client[4];
uint8 *command_list[4] = {"C","C","C","C"}; //用电器的初始状态

/*
 * 函数：tcp_server_sent_cb
 * 参数：void *arg - 连接结构体
 * 返回：void
 * 说明：TCP Server发送回调
 */
static void ICACHE_FLASH_ATTR
tcp_server_sent_cb(void *arg) {
	// TODO:
}

/*
 * 函数：tcp_server_discon_cb
 * 参数：void *arg - 连接结构体
 * 返回：void
 * 说明：TCP Server断开回调
 */
static void ICACHE_FLASH_ATTR
tcp_server_discon_cb(void *arg) {
	// TODO:
}

/*
 * 函数：tcp_server_recv
 * 参数：void *arg - 连接结构体
 * 	   char *pdata - 接收数据首地址
 * 	   unsigned short len - 接收数据长度
 * 返回：void
 * 说明：TCP Server接收回调
 */
static void ICACHE_FLASH_ATTR
tcp_server_recv(void *arg, char *pdata, unsigned short len) {
	struct espconn *pesp_conn = arg;

	// TODO:
	os_printf("pdata = %s \n", pdata);
	uart1_tx_buffer((uint8 *)pdata,len);

}

/*
 * 函数：tcp_server_listen
 * 参数：void *arg - 连接结构体
 * 返回：void
 * 说明：TCP Server监听
 */
static void ICACHE_FLASH_ATTR
tcp_server_listen(void *arg) {
	struct espconn *pesp_conn = arg;

	uint16 ip = pesp_conn->proto.tcp->remote_ip[3];
	//os_printf("ip = %d\n",ip);

	switch(ip){

#if CENTRE_or_SERVER
	 case CURTAIN_IP:
		 client[DEVICE_CURTAIN] = pesp_conn;
		 espconn_send(pesp_conn, command_list[DEVICE_CURTAIN], 1);
		 break;
	 case LIGHT_IP:
		 client[DEVICE_LIGHT] = pesp_conn;
		 espconn_send(pesp_conn, command_list[DEVICE_LIGHT], 1);
		 break;
	 case FAN_IP:
		 client[DEVICE_FAN] = pesp_conn;
		 espconn_send(pesp_conn, command_list[DEVICE_FAN], 1);
		 break;
	 case NODE_IP:
		 client[DEVICE_NODE] = pesp_conn;
		 break;
#else
	 case ROOM_ONE_IP:
		 client[ROOM_ONE_NO] = pesp_conn;
		 break;
	 case ROOM_TWO_IP:
		 client[ROOM_TWO_NO] = pesp_conn;
		 break;
	 case ROOM_THREE_IP:
		 client[ROOM_THREE_NO] = pesp_conn;
		 break;
	 case ROOM_FOUR_IP:
		 client[ROOM_FOUR_NO] = pesp_conn;
		 break;
#endif
	}

	espconn_regist_disconcb(pesp_conn, tcp_server_discon_cb);
	espconn_regist_recvcb(pesp_conn, tcp_server_recv);
	espconn_regist_sentcb(pesp_conn, tcp_server_sent_cb);
	//espconn_regist_reconcb(pesp_conn, client_recon_cb);
	//os_printf("tcp connect,id = %d\n",client1->proto.tcp->remote_ip[3]);
}

void send2device (uint8 deviceID, char *pdata) {

	struct espconn *device;
	os_printf("deviceID is %d\n",deviceID);
	switch(deviceID){
		 case DEVICE_CURTAIN:
			 device = client[DEVICE_CURTAIN];
			 break;
		 case DEVICE_LIGHT:
			 device = client[DEVICE_LIGHT];
			 break;
		 case DEVICE_FAN:
			 device = client[DEVICE_FAN];
			 break;
	}

	os_printf("pdata is: %s, lenth is %d \n",pdata,os_strlen(pdata));
	espconn_send(device, pdata, os_strlen(pdata));
}

void send2room (uint8 roomNO, char *pdata) {
	struct espconn *room;
	//os_printf("room no is %d\n",roomNO);
	switch(roomNO){
		 case ROOM_ONE_NO:
			 room = client[ROOM_ONE_NO];
			 break;
		 case ROOM_TWO_NO:
			 room = client[ROOM_TWO_NO];
			 break;
		 case ROOM_THREE_NO:
			 room = client[ROOM_THREE_NO];
			 break;
		 case ROOM_FOUR_NO:
			 room = client[ROOM_FOUR_NO];
			 break;
	}

	espconn_send(room, pdata, 5);
}

/*
 * 函数：tcp_server_init
 * 参数：void *arg - 连接结构体
 * 返回：void
 * 说明：TCP Server初始化
 */
void ICACHE_FLASH_ATTR
tcp_server_init(uint32 port) {
	static struct espconn s_tcp_server;
	static esp_tcp s_esptcp;

	s_tcp_server.type = ESPCONN_TCP;
	s_tcp_server.state = ESPCONN_NONE;
	s_tcp_server.proto.tcp = &s_esptcp;
	s_tcp_server.proto.tcp->local_port = port;
	espconn_regist_connectcb(&s_tcp_server, tcp_server_listen);

	espconn_accept(&s_tcp_server);
	espconn_regist_time(&s_tcp_server, 0, 0);			// TCP server 超时时间

	//os_printf("tcp_server_init\r\n");
}
