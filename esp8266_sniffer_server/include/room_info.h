#ifndef __ROOM_INFO_H__
#define __ROOM_INFO_H__

#include "driver/uart.h"
#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"

/*
 * CENTRE_or_SERVER: �ýڵ���SERVER����CENTRE
 * 0��CENTRE
 * 1��SERVER
 */
#define CENTRE_or_SERVER    1

// ��������ڵ�ľ�̬IP
#define ROOM_ONE_IP    101
#define ROOM_TWO_IP    102
#define ROOM_THREE_IP  103
#define ROOM_FOUR_IP   104

// Լ���ķ�����
#define ROOM_ONE_NO    0x01
#define ROOM_TWO_NO    0x02
#define ROOM_THREE_NO  0x03
#define ROOM_FOUR_NO   0x04

// �����õ����ڵ�ľ�̬IP
#define CURTAIN_IP  201
#define LIGHT_IP    202
#define FAN_IP   	203
#define NODE_IP   	204
//..another esp8266 model

//Լ�����õ������ͱ���
#define DEVICE_CURTAIN 0x02
#define DEVICE_LIGHT   0x01
#define DEVICE_FAN     0x00
#define DEVICE_NODE    0x04
//...another device

//�õ�����������
#define DEVICE_ON   "T"
#define DEVICE_OFF  "C"

extern uint8 *command_list[4]; //���ڴ����ĸ�������õ�����������
extern uint8 room_data_buf[4]; //ĳ������Ļ�����Ϣ

extern uint8 room;
extern uint8 people;
extern uint8 temp;
extern uint8 humi;
extern uint8 noise;

#endif
