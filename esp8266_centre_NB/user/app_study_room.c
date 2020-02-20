/**
 * ******************************************************************************
 * @file app_study_room.c
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

/* Includes ------------------------------------------------------------------*/
#include "app_study_room.h"
#include "nb_b35.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* 在后续的迭代中改为动态更改设备数量 */
/* 目前所有用电器都固定为 6 个 */
#define DEVICE_QAUNTITY 6

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/**
 * @brief 四个房间的环境信息
 * 
 * @details 
 * 	ROOM_NO 取值范围为 [1, 4], 对应四个房间
 * 
 * 	- room_message[ROOM_NO - 1] = ROOM_NO对应房间的环境信息
 * 
 * @note
 * 	由RoomMessage中的messageId成员来区分不同房间的上报数据，与华为IoT平台中产品的编解
 * 码插件保持一致
 * 
 * @par 信息指令串
 * 	各个子节点传输信息到主节点的通讯信息格式
 * 
 * 	室内环境信息指令串的信息标志位为 0xFF
	@verbatim
 	------------------------------------------------------------------------------------
 	|        |                          BODY                                 |         |
 	| HEADER | ------------------------------------------------------------- | TRAILER |
 	|        | ROOM_NO | ENV_CROWD_DENSITY | ENV_TEMP | ENV_HUMI | ENV_NOISE |         |
 	------------------------------------------------------------------------------------
 	|  0xFF  | 1,2,3,4 |      [0,100]      |  [0,50]  |  [0,100] | [30,130]  |   0xFF  |
	|        |         |         %         |    ℃    |    %RH   |    db     |         |
 	------------------------------------------------------------------------------------
 	@endverbatim
 * 
 * @see RoomMessage
 * 
 */
RoomMessage room_message[4] = {{0x11}, {0x12}, {0x13}, {0x14}}; /* 赋值 */

/**
 * @brief 四个房间的用电器状态信息
 * 
 * @details 
 * 	ROOM_NO 取值范围为 [1, 4], 对应四个房间
 * 
 * 	- room_status[ROOM_NO - 1][7..0] = 风扇用电器状态
 * 	- room_status[ROOM_NO - 1][15..8] = 电灯用电器状态
 * 	- room_status[ROOM_NO - 1][23..16] = 窗帘用电器状态
 * 	- room_status[ROOM_NO - 1][31..24] = 空调用电器状态
 * 
 * 每一个数据位表示一个用电器的状态，0为关，1为开
 * 
 * 数据位与用电器编号相对应。目前代码最大支持每类用电器包含 8 个用电器
 * 
 * @par 数组数据分布
	@verbatim
	-----------------------------------------------------------------
	|                    room_status[ROOM_NO - 1]                   |
	|-------------------------------------------------------------- |
	|     31..24    |     23..16    |     15..8     |      7..0     |
	-----------------------------------------------------------------
	| 空调用电器状态 | 窗帘用电器状态 | 电灯用电器状态 | 风扇用电器状态 |
	- ---------------------------------------------------------------
	@endverbatim
 * 
 *  数据示例：
 *
 *  room_status[ROOM_NO - 1][7..0] = 0x1B
	@verbatim
	-------------------------------------------------------------------------------------
	|                           room_status[ROOM_NO - 1][7..0]                          |
	|---------------------------------------------------------------------------------- |
	|    7    |    6     |    5    |    4     |    3    |    2     |    1    |    0     |
	-------------------------------------------------------------------------------------
	|    0    |    0     |    0    |    1     |    1    |    0     |    1    |    1     |
	-------------------------------------------------------------------------------------
	| 风扇八关 | 风扇七关 | 风扇六关 | 风扇五开 | 风扇四开 | 风扇三关 | 风扇二开 | 风扇一开 |
	-------------------------------------------------------------------------------------
	@endverbatim
 * 	
 * @par 信息指令串
 * 	各个子节点传输信息到主节点的通讯信息格式
 * 
 * 	用电器状态信息指令串的信息标志位为 0xFE
	@verbatim
 	------------------------------------------------------------------------
 	|        |                              BODY                 |         |
 	| HEADER | ------------------------------------------------- | TRAILER |
 	|        | ROOM_NO | DEVICE_TYPE | DEVICE_NO | DEVICE_STATUS |         |
 	------------------------------------------------------------------------
 	| 0xFE   | 1,2,3,4 |   0,1,2,3   |  1,2,3,4  |     0,1       |   0xFE  |
 	------------------------------------------------------------------------
 	@endverbatim
 * 
 * @todo 增加用电器数量数组，并于用电器状态一起封装成结构体
 * 
 */
uint32 room_status[4];

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
 * @brief 更新本地数据
 * @details 
 * 	- 更新单个房间的环境信息
 * 	- 更新单个房间的单个用电器的状态
 *  - 单次调用时，环境信息和用电器状态信息不同时更新
 *  - 由各个房间的client节点发送到本节点的tcp server上，msg_string来自于tcp_server_recv()
 * 
 * @param[in] msg_string 信息指令串
 * @arg @c 0xFF 作为标志位：室内环境信息
 * @arg @c 0xFE 作为标志位：用电器状态信息
 * 
 * @see room_status
 * @see room_message
 */
void StudyRoom_UpdataData(uint8 *msg_string) {

	uint8 hexstr[128];

	if (msg_string[0] == 0xFF)
	{
		/* msg_string[1] 房间编号，取值范围[1..4] */
		os_memcpy((room_message + msg_string[1] - 1), msg_string + 2, 4 );

	} 
	else if (msg_string[0] == 0xFE)
	{
		// StudyRoom_StatusToHex(msg_string[1], hexstr);
		// ESP_DEBUG("status str now is: %s, num is %d", hexstr, room_status[0]);

		/* 位运算 先将目标用电器位置零，再将指令赋值给目标用电器位 */
		*(room_status + (msg_string[1] - 1)) &= ~(0x01 << (8 * msg_string[2] + msg_string[3]));
		*(room_status + (msg_string[1] - 1)) |= ((uint32)msg_string[4] << (8 * msg_string[2] + msg_string[3]));

		// StudyRoom_StatusToHex(msg_string[1], hexstr);
		// ESP_DEBUG("status str now is: %s, num is %d", hexstr, room_status[0]);
	}
	
}

/**
 * @brief 将一个房间所有用电器的状态转成 Hex 字符串
 * 
 * @note 数据格式与IoT平台的编解码插件保持一致
 * 
 * @param[in] room_no 需要转换的房间编号
 * @param[out] out_hexstr 转换后的字符串
 * 
 * @see room_status
 */
void StudyRoom_StatusToHex(uint8 room_no, uint8 *out_hexstr ) {

	uint8 i,j;
	bool one_status; /* 一个用电器的状态, 只用一个数据位表示开关即可 */
	uint8 *current_pos = out_hexstr; /* 指向当前 out_hexstr 位置的指针 */

	for (i = DEVICE_FAN; i <= DEVICE_AC; i++) {

		/* 在每类用电器前面加上该类用电器的数量 */
		os_sprintf(current_pos, "%02X", DEVICE_QAUNTITY);
		current_pos += 2;

		for(j = 0; j < DEVICE_QAUNTITY; j++, current_pos += 2 ) {

			/* 先定位某个房间某一类用电器的状态，再定位具体的单个用电器状态 */
			/* 可以结合用电器状态的“数组数据分布”来理解，参阅 room_status[] 的注释 */
			one_status = (((*(room_status + room_no - 1) >> (8 * i)) & 0x000000ff) >> j) & 0x01;
			os_sprintf(current_pos, "%02X", one_status); 
		}

		ESP_DEBUG("out_hexstr = %s", out_hexstr);

	}
	
}

// int sensor_controll_handler()
// {
// // Define data report struct, all members come from devicemodel file


// 	// if (semp_pend(s_rcv_sync, 0x7fffffff))
// 	// {
// 	// 	uint8_t msgid = s_rcv_buffer[0];
// 	// 	switch (msgid)
// 	// 	{

// 	// 	default:
// 	// 		break;
// 	// 	}
// 	// }
// }

// int report_data_handler()
// {
// 	DeviceController SET_LIGHT;
// 	DeviceController SET_CURTAIN;
// 	DeviceController SET_FAN;
// 	DeviceController SET_AC;
// 	RoomMessage report_room_one_message;
// 	RoomStatus report_room_one_status;

// /********** code area for report data to IoT cloud  **********/

// /********** code area end  **********/

// // virtual sensor data example
// 	report_room_one_message.messageId = MID_report_room_one_message;
// 	report_room_one_message.peopleFlowrate = 0x1;
// 	report_room_one_message.temp = 0x1;
// 	report_room_one_message.humi = 0x1;
// 	report_room_one_message.noise = 0x1;
// 	oc_lwm2m_report((uint8_t *)&report_room_one_message, sizeof(report_room_one_message), 1000);

// 	report_room_one_status.messageId = MID_report_room_one_status;
// 	report_room_one_status.fanNum = 0x1;
// 	report_room_one_status.fanStatus[0] = 9;
// 	report_room_one_status.lightNum = 0x1;
// 	report_room_one_status.lightStatus[0] = 9;
// 	report_room_one_status.curtainNum = 0x1;
// 	report_room_one_status.curtainStatus[0] = 9;
// 	report_room_one_status.acNum = 0x1;
// 	report_room_one_status.acStatus[0] = 9;
// 	oc_lwm2m_report((uint8_t *)&report_room_one_status, sizeof(report_room_one_status), 1000);

// }
