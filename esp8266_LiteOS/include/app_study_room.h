
#include <stddef.h>
#include <stdint.h>

#include <at.h>
#include <boudica150_oc.h>
#include <los_dev.h>
#include <oc_lwm2m_agent.h>
#include <osport.h>

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

#define cn_app_bands "5,8,20"
#define cn_app_plmn "46011";
#define cn_app_apn "1,\"IP\",\"HUAWEI.COM\""

#define HUAWEI_IOT_SERVER "49.4.85.232"
#define HUAWEI_IOT_PORT "5683"

short int Lux;

/**
 * @addtogroup DecodeMessageId
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

/**
 * @brief 云端下发的用电器控制信息
 * 
 */
typedef struct
{
	uint8_t deviceType; /*!< 根据messageId来区分用电器类型 */
	uint8_t roomNo;
	uint8_t deviceNo;
	uint8_t deviceSwitch;
} DeviceController;

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

/**
 * @brief 室内环境信息结构体
 * 
 */
typedef struct
{
	uint8_t messageId;		/*!< 根据messageId来区分房间号 */
	uint8_t peopleFlowrate; /*!< 人群密度 */
	uint8_t temp;			/*!< 环境温度 */
	uint8_t humi;			/*!< 环境湿度 */
	uint8_t noise;			/*!< 环境噪音 */
} RoomMessage;

enum Device
{
	DEVICE_FAN = 0x00,	 /*!< 风扇用电器 */
	DEVICE_LIGHT = 0x01,   /*!< 电灯用电器 */
	DEVICE_CUTRAIN = 0x02, /*!< 窗帘用电器 */
	DEVICE_AC = 0x03	   /*!< 空调用电器 */
};

/**
 * @brief 室内用电器状态信息结构体
 * 
 */
typedef struct
{
	uint8_t messageId;		  /*!< 用电器状态信息标识符 */
	uint8_t fanNum;			  /*!< 风扇数量 */
	variant fanStatus[6];	 /*!< 风扇状态信息 */
	uint8_t lightNum;		  /*!< 电灯数量 */
	variant lightStatus[6];   /*!< 电灯状态信息 */
	uint8_t curtainNum;		  /*!< 窗帘数量 */
	variant curtainStatus[6]; /*!< 窗帘状态信息 */
	uint8_t acNum;			  /*!< 空调数量 */
	variant acStatus[6];	  /*!< 空调状态信息 */
} RoomStatus;

// handle all the Big endian and Little endian problem
typedef unsigned short uint16;
typedef unsigned long uint32;

#define Swap16(A) ((((uint16)(A)&0xff00) >> 8) | (((uint16)(A)&0x00ff) << 8))
#define Swap32(A) \
	((((uint32)(A)&0xff000000) >> 24) | (((uint32)(A)&0x00ff0000) >> 8) | (((uint32)(A)&0x0000ff00) << 8) | (((uint32)(A)&0x000000ff) << 24))

int app_report_task_entry()
{
	int ret = -1;
	tag_oc_config_param oc_param;

	// System frame work initilized here, which means you could use the driver framwork, good luck~!
	los_driv_init();

	// AT interface initialize: UART 2
	extern bool_t uart_at_init(s32_t baudrate);
	uart_at_init(9600);

	// Do the AT module initialize: AT FRAMEWORK
	los_at_init("atdev");

	// START APPLICATION INITIALIZE HERE
	oc_lwm2m_agent_init();
	boudica150_init(NULL, NULL, cn_app_bands);
	oc_param.server = cn_app_server;
	oc_param.port = cn_app_port;

	// success, so we could receive and send now
	if (0 == oc_lwm2m_config(&oc_param))
	{
		// install a dealer for the led message received
		oc_lwm2m_install_msgdealer(app_msg_deal, NULL, 0);

		while (1)
		{
			report_data_handler();
			ret = 0;
			task_sleepms(10 * 1000);
		}
	}
	return ret;
}
