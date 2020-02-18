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

/* TCP server receive handler, define in app_study_room.c */
extren void StudyRoom_UpdataData(uint8 *msg_string);
extren void StudyRoom_StatusToHex(uint8 room_no, uint8 *out_hexstr);

struct espconn *client[4];

/*
 * 鍑芥暟锛歵cp_server_sent_cb
 * 鍙傛暟锛歷oid *arg - 杩炴帴缁撴瀯浣�
 * 杩斿洖锛歷oid
 * 璇存槑锛歍CP Server鍙戦�佸洖璋�
 */
static void ICACHE_FLASH_ATTR
tcp_server_sent_cb(void *arg) {
	// TODO:
}

/*
 * 鍑芥暟锛歵cp_server_discon_cb
 * 鍙傛暟锛歷oid *arg - 杩炴帴缁撴瀯浣�
 * 杩斿洖锛歷oid
 * 璇存槑锛歍CP Server鏂紑鍥炶皟
 */
static void ICACHE_FLASH_ATTR
tcp_server_discon_cb(void *arg) {
	// TODO:
}

/*
 * 鍑芥暟锛歵cp_server_recv
 * 鍙傛暟锛歷oid *arg - 杩炴帴缁撴瀯浣�
 * 	   char *pdata - 鎺ユ敹鏁版嵁棣栧湴鍧�
 * 	   unsigned short len - 鎺ユ敹鏁版嵁闀垮害
 * 杩斿洖锛歷oid
 * 璇存槑锛歍CP Server鎺ユ敹鍥炶皟
 */
static void ICACHE_FLASH_ATTR
tcp_server_recv(void *arg, char *pdata, unsigned short len) {

	struct espconn *pesp_conn = arg;
	uint8 nb_buff[128];


	// TODO:
	os_printf("pdata = %s \n", pdata);
	uart1_tx_buffer((uint8 *)pdata,len);

	if (pdata[0] ==  0xFE)
	{
		StudyRoom_UpdataData(pdata);
		StudyRoom_StatusToHex(pdata[1], nb_buff);
	}
	
}

/*
 * 鍑芥暟锛歵cp_server_listen
 * 鍙傛暟锛歷oid *arg - 杩炴帴缁撴瀯浣�
 * 杩斿洖锛歷oid
 * 璇存槑锛歍CP Server鐩戝惉
 */
static void ICACHE_FLASH_ATTR
tcp_server_listen(void *arg) {
	struct espconn *pesp_conn = arg;

	uint16 ip = pesp_conn->proto.tcp->remote_ip[3];
	os_printf("ip is %d\n",ip);
	switch(ip){
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
	}

	espconn_regist_disconcb(pesp_conn, tcp_server_discon_cb);
	espconn_regist_recvcb(pesp_conn, tcp_server_recv);
	espconn_regist_sentcb(pesp_conn, tcp_server_sent_cb);
	//espconn_regist_reconcb(pesp_conn, client_recon_cb);

}

void send2room (uint8 roomNO, char *pdata) {
	struct espconn *room;
	os_printf("room no is %d\n",roomNO);
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
 * 鍑芥暟锛歵cp_server_init
 * 鍙傛暟锛歷oid *arg - 杩炴帴缁撴瀯浣�
 * 杩斿洖锛歷oid
 * 璇存槑锛歍CP Server鍒濆鍖�
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
	espconn_regist_time(&s_tcp_server, 0, 0);			// TCP server 瓒呮椂鏃堕棿

	//os_printf("tcp_server_init\r\n");
}
