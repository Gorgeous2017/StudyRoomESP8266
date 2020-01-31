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
#include "app_study_room.h"

int sensor_controll_handler()
{
// Define data report struct, all members come from devicemodel file


	// if (semp_pend(s_rcv_sync, 0x7fffffff))
	// {
	// 	uint8_t msgid = s_rcv_buffer[0];
	// 	switch (msgid)
	// 	{

	// 	default:
	// 		break;
	// 	}
	// }
}

int report_data_handler()
{
	DeviceController SET_LIGHT;
	DeviceController SET_CURTAIN;
	DeviceController SET_FAN;
	DeviceController SET_AC;
	RoomMessage report_room_one_message;
	RoomStatus report_room_one_status;

/********** code area for report data to IoT cloud  **********/

/********** code area end  **********/

// virtual sensor data example
	report_room_one_message.messageId = MID_report_room_one_message;
	report_room_one_message.peopleFlowrate = 0x1;
	report_room_one_message.temp = 0x1;
	report_room_one_message.humi = 0x1;
	report_room_one_message.noise = 0x1;
	oc_lwm2m_report((uint8_t *)&report_room_one_message, sizeof(report_room_one_message), 1000);

	report_room_one_status.messageId = MID_report_room_one_status;
	report_room_one_status.fanNum = 0x1;
	report_room_one_status.fanStatus[0] = 9;
	report_room_one_status.lightNum = 0x1;
	report_room_one_status.lightStatus[0] = 9;
	report_room_one_status.curtainNum = 0x1;
	report_room_one_status.curtainStatus[0] = 9;
	report_room_one_status.acNum = 0x1;
	report_room_one_status.acStatus[0] = 9;
	oc_lwm2m_report((uint8_t *)&report_room_one_status, sizeof(report_room_one_status), 1000);

}