
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

#define cn_app_server "49.4.85.232"
#define cn_app_port "5683"

short int Lux;

/**
 * @addtogroup DecodeMessageId
 * @{
 * 
 */
#define cn_app_SET_FAN 0x0 /*!< 风扇控制指令 */ 
#define cn_app_SET_LIGHT 0x1 /*!< 电灯控制指令 */
#define cn_app_SET_CURTAIN 0x2 /*!< 窗帘控制指令 */
#define cn_app_SET_AC 0x3 /*!< 空调控制指令 */
#define cn_app_report_room_one_message 0x4 /*!< 房间一的环境信息 */
#define cn_app_report_room_one_status 0x5 /*!< 房间一的用电器状态信息 */

/**
 * @}
 * 
 */

#pragma pack(1)

typedef struct
{
    int8u messageId; /*!< 根据messageId来区分用电器类型 */
    int8u roomNo;
    int8u deviceNo;
    int8u deviceSwitch;
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

typedef struct
{
    int8u messageId; /*!< 根据messageId来区分房间号 */
    int8u peopleFlowrate;
    int8u temp;
    int8u humi;
    int8u noise;
} RoomMessage;

typedef struct
{
    int8u messageId;
    int8u fanNum;
    variant fanStatus[6];
    int8u lightNum;
    variant lightStatus[6];
    int8u curtainNum;
    variant curtainStatus[6];
    int8u acNum;
    variant acStatus[6];
} RoomStatus;

// handle all the Big endian and Little endian problem
#pragma pack()
typedef unsigned short uint16;
typedef unsigned long uint32;

#define Swap16(A) ((((uint16)(A)&0xff00) >> 8) | (((uint16)(A)&0x00ff) << 8))
#define Swap32(A) \
    ((((uint32)(A)&0xff000000) >> 24) | (((uint32)(A)&0x00ff0000) >> 8) | (((uint32)(A)&0x0000ff00) << 8) | (((uint32)(A)&0x000000ff) << 24))

// union {
//     char c[4];
//     unsigned long mylong;
// } endian_test = {{'l', '?', '?', 'b'}};

// #define ENDIANNESS ((char)endian_test.mylong)

// uint16 htons(uint16 hs)
// {
//     return (ENDIANNESS == 'l') ? Swap16(hs) : hs;
// }

// uint32 htonl(uint32 hl) { return (ENDIANNESS == 'l') ? Swap32(hl) : hl; }

// uint16 ntohs(uint16 ns) { return (ENDIANNESS == 'l') ? Swap16(ns) : ns; }

// uint32 ntohl(uint32 nl) { return (ENDIANNESS == 'l') ? Swap32(nl) : nl; }

// if your command is very fast,please use a queue here
#define cn_app_rcv_buf_len 128
uint8_t s_rcv_buffer[cn_app_rcv_buf_len];
uint32_t s_rcv_datalen;
semp_t s_rcv_sync;

// use this function to push all the message to the buffer
int32_t app_msg_deal(uint8_t *msg, int32_t len)
{
    int32_t ret = -1;
    memset(s_rcv_buffer, 0, sizeof(s_rcv_buffer));
    if (len <= cn_app_rcv_buf_len)
    {
        memcpy(s_rcv_buffer, msg, len);
        s_rcv_datalen = len;
        semp_post(s_rcv_sync);
        ret = 0;
    }
    return ret;
}

int32_t app_cmd_task_entry()
{
    int32_t ret = -1;

    while (1)
    {
        sensor_controll_handler();
        ret = 0;
        task_sleepms(300);
    }
    return ret;
}

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

int app_init()
{
    semp_create(&s_rcv_sync, 1, 0);
    task_create("app_report", (fnTaskEntry)app_report_task_entry, 0x1000, NULL, NULL, 2);
    task_create("app_command", (fnTaskEntry)app_cmd_task_entry, 0x1000, NULL, NULL, 3);
    return 0;
}