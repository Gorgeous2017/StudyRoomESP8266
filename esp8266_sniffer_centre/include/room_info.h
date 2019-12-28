#ifndef __ROOM_INFO_H__
#define __ROOM_INFO_H__

#include "driver/uart.h"
#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"


// 各个房间节点的静态IP
#define ROOM_ONE_IP    101
#define ROOM_TWO_IP    102
#define ROOM_THREE_IP  103
#define ROOM_FOUR_IP   104

//ocean connect 定义的messageId
#define ROOM_ONE_NO    0x01
#define ROOM_TWO_NO    0x02
#define ROOM_THREE_NO  0x03
#define ROOM_FOUR_NO   0x04

#define DEVICE_1   0x01
#define DEVICE_2   0x02
#define DEVICE_3   0x03
#define DEVICE_4   0x04
//..another device


#define DEVICE_LIGHT_1 0x01
#define DEVICE_LIGHT_2 0x02
//....



extern int device;
extern int command;

extern uint8 room;
extern uint8 people;
extern uint8 temp;
extern uint8 humi;
extern uint8 noise;




#endif
