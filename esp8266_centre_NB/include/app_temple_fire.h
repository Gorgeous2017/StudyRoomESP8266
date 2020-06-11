/**
 * ******************************************************************************
 * @file app_temple_fire.h
 * @author Gump
 * @version V0.1
 * @date 2020-05-23
 * @brief 
 * 
 * @copyright Copyright (c) <2020> <HANSHAN NORMAL UNIVERSITY(CHAOZHOU)> All rights Reserved.
 *   Media Recognition and Intelligent System Team
 * 
 * ******************************************************************************
 */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_TEMPLE_FIRE_H__
#define __APP_TEMPLE_FIRE_H__ 

/* Includes ------------------------------------------------------------------*/
// #include <stddef.h>
// #include <stdint.h>
#include "ets_sys.h"
#include "os_type.h"

#include "osapi.h" /* 需要用到os_sprintf */
#include "driver/uart.h" /* 需要用到ESP_DEBUG */

/* Exported typedef -----------------------------------------------------------*/

/*! @brief 编解码插件messageId枚举体 */
typedef enum
{
	MID_report_node_one_env    = 0x11,  /*!< 节点一的环境信息 */
	MID_report_node_two_env    = 0x12,  /*!< 节点二的环境信息 */
	MID_report_node_three_env  = 0x13,  /*!< 节点三的环境信息 */
	MID_report_node_four_env   = 0x14,  /*!< 节点四的环境信息 */
} DecodeMessageId;

typedef enum 
{
	MID_report_room_env_offset = 0x10,
	MID_report_room_dev_offset = 0x20	
} MessageIdOffset;

/*! @brief 庙宇环境信息结构体  */
typedef struct
{
	uint8 messageId;			/*!< 根据messageId来区分节点位置 */
	uint8 smoke;				/*!< 环境烟雾浓度 */
	uint8 temp;					/*!< 环境温度 */
	uint8 humi;					/*!< 环境湿度 */
	uint16 heatImage[80][80]; 	/*!< 热力图像 */
} TempleEnvMessage;

/*! @brief 数据上报结构体 */
typedef struct 
{
    uint8 msg_flag;  												/*!< 消息标志位，与用户所定义的Wi-Fi传输信息指令串一致 */
    DecodeMessageId messageId; 										/*!< 要上报消息的messageId */
    void (*App_UpdataData)(void *stDataReport, uint8 *msg_string); 	/*!< 更新本地数据 */
    uint8 * (*App_GetHexData)(void *stDataReport, uint8 room_no); 	/*!< 生成十六进制字符串格式的上报数据 */
} DataReportStruct;

/* Exported constants --------------------------------------------------------*/
#define cn_app_bands "5,8,20"
#define cn_app_plmn "46011";
#define cn_app_apn "1,\"IP\",\"HUAWEI.COM\""

/* 信息来源：华为IoT平台-应用-对接信息-设备接入信息 */
#define HUAWEI_IOT_SERVER "49.4.85.232" /*!< 华为IoT平台服务器地址 */
#define HUAWEI_IOT_PORT "5683" /*!< LWM2M(CoAP) 无DTLS形式接入平台的端口 */


/* Exported function -----------------------------------------------*/
void StudyRoom_UpdataDevData(void *stDataReport, uint8 *msg_string);
void StudyRoom_UpdataEnvData(void *stDataReport, uint8 *msg_string);
uint8 * StudyRoom_GetDevHex(void *stDataReport, uint8 room_no);

#endif /* __APP_TEMPLE_FIRE_H__ */ 
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

