#ifndef __ROOM_INFO_H__
#define __ROOM_INFO_H__

#include "driver/uart.h"
#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"

/*
 * CENTRE_or_SERVER: 该节点是SERVER还是CENTRE
 * 0：CENTRE
 * 1：SERVER
 */
#define CENTRE_or_SERVER    1

// 各个房间节点的静态IP
#define ROOM_ONE_IP    101
#define ROOM_TWO_IP    102
#define ROOM_THREE_IP  103
#define ROOM_FOUR_IP   104

// 约定的房间编号
#define ROOM_ONE_NO    0x01
#define ROOM_TWO_NO    0x02
#define ROOM_THREE_NO  0x03
#define ROOM_FOUR_NO   0x04

// 各个用电器节点的静态IP
#define CURTAIN_IP  201
#define LIGHT_IP    202
#define FAN_IP   	203
#define NODE_IP   	204
//..another esp8266 model

//约定的用电器类型编码
#define DEVICE_CURTAIN 0x02
#define DEVICE_LIGHT   0x01
#define DEVICE_FAN     0x00
#define DEVICE_NODE    0x04
//...another device

//用电器控制命令
#define DEVICE_ON   "T"
#define DEVICE_OFF  "C"

extern uint8 *command_list[4]; //用于储存四个房间的用电器控制命令
extern uint8 room_data_buf[4]; //某个房间的环境信息

extern uint8 room;
extern uint8 people;
extern uint8 temp;
extern uint8 humi;
extern uint8 noise;

#endif
