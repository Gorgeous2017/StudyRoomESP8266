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
#include "nb_bc35.h"

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
	| 空调用电器状态 | 窗帘用电器状态 | 电灯用电器状态 |  风扇用电器状态 |
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
 * 	- 更新单个房间的单个用电器的状态
 *  - 由各个房间的client节点发送到本节点的tcp server上，msg_string来自于tcp_server_recv()
 * 
 * @param[in] msg_string 信息指令串
 * 
 * @see room_status
 */
void StudyRoom_UpdataDevData(void *stDataReport, uint8 *msg_string) {

	uint8 hexstr[128];

	/* 位运算 先将目标用电器位置零，再将指令赋值给目标用电器位 */
	*(room_status + (msg_string[1] - 1)) &= ~(0x01 << (8 * msg_string[2] + msg_string[3]));
	*(room_status + (msg_string[1] - 1)) |= ((uint32)msg_string[4] << (8 * msg_string[2] + msg_string[3]));

}

/**
 * @brief 更新本地数据
 * @details 
 * 	- 更新单个房间的环境信息
 *  - 由各个房间的client节点发送到本节点的tcp server上，msg_string来自于tcp_server_recv()
 * 
 * @param[in] msg_string 信息指令串
 * @see room_message
 */
void StudyRoom_UpdataEnvData(void *stDataReport, uint8 *msg_string) {

	uint8 hexstr[128];

	/* msg_string[1] 房间编号，取值范围[1..4] */
	os_memcpy((room_message + msg_string[1] - 1), msg_string + 2, 4 );

}

/**
 * @brief 将一个房间所有用电器的状态转成 Hex 字符串
 * 
 * @note 数据格式与IoT平台的编解码插件保持一致
 * 
 * @param[in] room_no 需要转换的房间编号
 * @return uint8* 转换后的字符串
 * 
 * @see room_status
 */
uint8 * StudyRoom_GetDevHex(void *stDataReport, uint8 room_no) {

	uint8 i,j;
	bool one_status; /* 一个用电器的状态, 只用一个数据位表示开关即可 */
	static uint8 out_hexstr[128]; /* 转换后的字符串，必须指定成static类型才能return */
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

	return out_hexstr;	
}

// void StudyRoom_ReportData(DecodeMessageId mid, uint8 room_no, uint8 * (*get_hex)()) {

// 	NB_ReportData(mid, get_hex(room_no));

// }

void StudyRoom_ReportData(void *stDataReport, uint8 room_no, uint8 * (*get_hex)()) {

	//NB_ReportData(/* mid offset + room no */, get_hex(room_no));

}

DataReportStruct stEnvDataReport = {
	.msg_flag = 0xFF,
	.messageId = MID_report_room_one_env,
	.App_UpdataData = StudyRoom_UpdataEnvData,
	.App_GetHexData = StudyRoom_GetDevHex
};

DataReportStruct stDevDataReport = {
	.msg_flag = 0xFE,
	.messageId = MID_report_room_one_dev, /* 改成mid offset */
	.App_UpdataData = StudyRoom_UpdataDevData,
	.App_GetHexData = StudyRoom_GetDevHex
};