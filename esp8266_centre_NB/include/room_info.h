#ifndef __ROOM_INFO_H__
#define __ROOM_INFO_H__

#include "driver/uart.h"
#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"

#define CURTAIN_IP  201
#define LIGHT_IP    202
#define FAN_IP   	203
#define NODE_IP   	204
//..another esp8266 model

#define DEVICE_CURTAIN 0x02
#define DEVICE_LIGHT   0x01
#define DEVICE_FAN     0x00
#define DEVICE_NODE    0x04
//...another device

#define DEVICE_ON   "T"
#define DEVICE_OFF  "C"

extern uint8 *command_list[4];
extern uint8 room_data_buf[4];

extern uint8 room;
extern uint8 people;
extern uint8 temp;
extern uint8 humi;
extern uint8 noise;

#endif
