
/*
 * Name:app.c
 *
 *  Created on: 2019.5.27
 *  Author: IoT Studio
 *  Interface: sensor_controll_handler      sensor controll command from IoT cloud
 *  Interface: report_data_handler   report data to IoT cloud command
 */
#include "app.h"

int sensor_controll_handler()
{
// Define data report struct, all members come from devicemodel file
#if 1

#endif

    if (semp_pend(s_rcv_sync, 0x7fffffff))
    {
        uint8_t msgid = s_rcv_buffer[0];
        switch (msgid)
        {
#if 1
        default:
            break;
#endif
        }
    }
}

int report_data_handler()
{
    DeviceController SET_LIGHT;
    DeviceController SET_CURTAIN;
    DeviceController SET_FAN;
    DeviceController SET_AC;
    RoomMessage report_room_one_message;
    tag_app_report_room_one_status report_room_one_status;

/********** code area for report data to IoT cloud  **********/

/********** code area end  **********/

// virtual sensor data example
    SET_LIGHT.messageId = cn_app_SET_LIGHT;
    SET_LIGHT.room = 0x1;
    SET_LIGHT.lightNo = 0x1;
    SET_LIGHT.lightSwitch = 0x1;
    oc_lwm2m_report((uint8_t *)&SET_LIGHT, sizeof(SET_LIGHT), 1000);

    SET_CURTAIN.messageId = cn_app_SET_CURTAIN;
    SET_CURTAIN.room = 0x1;
    SET_CURTAIN.curtainNO = 0x1;
    SET_CURTAIN.curtainSwitch = 0x1;
    oc_lwm2m_report((uint8_t *)&SET_CURTAIN, sizeof(SET_CURTAIN), 1000);

    SET_FAN.messageId = cn_app_SET_FAN;
    SET_FAN.room = 0x1;
    SET_FAN.fanNo = 0x1;
    SET_FAN.fanGear = 0x1;
    oc_lwm2m_report((uint8_t *)&SET_FAN, sizeof(SET_FAN), 1000);

    SET_AC.messageId = cn_app_SET_AC;
    SET_AC.room = 0x1;
    SET_AC.acNo = 0x1;
    SET_AC.acTemp = 0x1;
    oc_lwm2m_report((uint8_t *)&SET_AC, sizeof(SET_AC), 1000);

    report_room_one_message.messageId = cn_app_report_room_one_message;
    report_room_one_message.peopleFlowrate = 0x1;
    report_room_one_message.temp = 0x1;
    report_room_one_message.humi = 0x1;
    report_room_one_message.noise = 0x1;
    oc_lwm2m_report((uint8_t *)&report_room_one_message, sizeof(report_room_one_message), 1000);

    report_room_one_status.messageId = cn_app_report_room_one_status;
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