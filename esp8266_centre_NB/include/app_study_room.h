/**
 * ******************************************************************************
 * @file app_study_room.h
 * @author Gump
 * @version V0.1
 * @date 2020-01-29
 * @brief 
 * 
 * @copyright Copyright (c) <2020> <HANSHAN NORMAL UNIVERSITY(CHAOZHOU)> All rights Reserved.
 *   Media Recognition and Intelligent System Team
 * 
 * ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_STUDY_ROOM_H__
#define __APP_STUDY_ROOM_H__ 

/* Includes ------------------------------------------------------------------*/
// #include <stddef.h>
// #include <stdint.h>
#include "ets_sys.h"
#include "os_type.h"

#include "osapi.h" /* 需要用到os_sprintf */
#include "driver/uart.h" /* 需要用到ESP_DEBUG */

/* Exported typedef -----------------------------------------------------------*/
/**
 * @brief 用电器类型枚举体
 * 
 */
typedef enum 
{
	DEVICE_FAN = 0x00,	   /*!< 风扇用电器 */
	DEVICE_LIGHT = 0x01,   /*!< 电灯用电器 */
	DEVICE_CUTRAIN = 0x02, /*!< 窗帘用电器 */
	DEVICE_AC = 0x03	   /*!< 空调用电器 */
} Device;

/**
 * @brief 云端下发的用电器控制信息
 * 
 */
typedef struct
{
	uint8 deviceType; 	/*!< 根据messageId来区分用电器类型 */
	uint8 roomNo; 		/*!< 要控制的用电器所在房间的编号 */
	uint8 deviceNo;		/*!< 要控制的用电器的编号 */
	uint8 deviceSwitch; /*!< 用电器的目标状态 */
} DeviceController;

/**
 * @brief 室内环境信息结构体
 * 
 */
typedef struct
{
	uint8 messageId;		/*!< 根据messageId来区分房间号 */
	uint8 peopleFlowrate; 	/*!< 人群密度 */
	uint8 temp;				/*!< 环境温度 */
	uint8 humi;				/*!< 环境湿度 */
	uint8 noise;			/*!< 环境噪音 */
} RoomMessage;

/**
 * @brief 室内用电器状态信息结构体
 * 
 */
typedef struct
{
	uint8 messageId;		/*!< 用电器状态信息标识符 */
	uint8 fanNum;			/*!< 风扇数量 */
	uint8 fanStatus[6];	 	/*!< 风扇状态信息 */
	uint8 lightNum;		  	/*!< 电灯数量 */
	uint8 lightStatus[6];   /*!< 电灯状态信息 */
	uint8 curtainNum;		/*!< 窗帘数量 */
	uint8 curtainStatus[6]; /*!< 窗帘状态信息 */
	uint8 acNum;			/*!< 空调数量 */
	uint8 acStatus[6];	  	/*!< 空调状态信息 */
} RoomStatus;

/* Exported constants --------------------------------------------------------*/
#define cn_app_bands "5,8,20"
#define cn_app_plmn "46011";
#define cn_app_apn "1,\"IP\",\"HUAWEI.COM\""

/* 信息来源：华为IoT平台-应用-对接信息-设备接入信息 */
#define HUAWEI_IOT_SERVER "49.4.85.232" /*!< 华为IoT平台服务器地址 */
#define HUAWEI_IOT_PORT "5683" /*!< LWM2M(CoAP) 无DTLS形式接入平台的端口 */

/**
 * @addtogroup DecodeMessageId
 * @brief 华为IoT平台|编解码插件所用的唯一messageId，用以区分不同的平台消息
 * @{
 * 
 */
#define MID_SET_FAN 0x0					/*!< 风扇控制指令 */
#define MID_SET_LIGHT 0x1				/*!< 电灯控制指令 */
#define MID_SET_CURTAIN 0x2				/*!< 窗帘控制指令 */
#define MID_SET_AC 0x3					/*!< 空调控制指令 */
#define MID_report_room_one_message 0x4 /*!< 房间一的环境信息 */
#define MID_report_room_one_status 0x5  /*!< 房间一的用电器状态信息 */

/**
 * @}
 * 
 */


/* Exported function -----------------------------------------------*/
void StudyRoom_UpdataData(uint8 *msg_string);
void StudyRoom_StatusToHex(uint8 room_no, uint8 *out_hexstr );


#endif /* __APP_STUDY_ROOM_H__ */ 
/********************************** END OF FILE *******************************/

typedef unsigned char int8u;
typedef char int8s;
typedef unsigned short int16u;
typedef short int16s;
typedef unsigned char int24u;
typedef char int24s;
typedef int int32s;
typedef char string;
typedef char array;
typedef char varstring;
typedef char variant;

short int Lux;

// typedef struct
// {
//     int8u messageId;
//     int8u room;
//     int8u fanNo;
//     int8u fanGear;
// } tag_app_SET_FAN;

// typedef struct
// {
//     int8u messageId;
//     int8u room;
//     int8u acNo;
//     int8u acTemp;
// } tag_app_SET_AC;

#define Swap16(A) ((((uint16)(A)&0xff00) >> 8) | (((uint16)(A)&0x00ff) << 8))
#define Swap32(A) \
	((((uint32)(A)&0xff000000) >> 24) | (((uint32)(A)&0x00ff0000) >> 8) | (((uint32)(A)&0x0000ff00) << 8) | (((uint32)(A)&0x000000ff) << 24))

