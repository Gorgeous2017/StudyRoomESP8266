#ifndef __ROOM_INFO_H__
#define __ROOM_INFO_H__


#include "driver/uart.h"
#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"

#include "user_json.h"
#include "driver/hal_temp_hum.h"


void setCommand(char *json);

char* get_room_json(void);

void doCommand(int device, int command);


#define ROOM_NO    201
#define DEVICE_1   0x01
#define DEVICE_2   0x02
#define DEVICE_3   0x03
#define DEVICE_4   0x04
//..another device


#define DEVICE_LIGHT_1 0x01
#define DEVICE_LIGHT_2 0x02
//....

extern uint8 room;
extern uint8 people;
extern uint8 temp;
extern uint8 humi;
extern uint8 noise;




#endif
