#include "room_info.h"

uint8 room;
uint8 people = 0;
uint8 temp = 0;
uint8 humi = 0;
uint8 noise = 0;

uint8 room_data_buf[4] = {0};

void get_noise(void){
	uint16 adc = system_adc_read();
	uint16 adc2 = (adc / (1024 / 3.3))*10;
	noise =( 100 * (adc2 - 4 ) /16 )+ 30;  //电压模拟值转分贝
	//os_printf("noise is %d db\r\n", noise);
}

void get_humi_temp(void){
    dh11Read(&temp,&humi);
    //os_printf("Temp: %d'C Humi: %d%\n",temp,humi);
}

void get_room_data(void){
	get_noise();
	get_humi_temp();
}

void get_room_buf(void){
	get_room_data();

	room_data_buf[0] = 0xFF;   // 消息头
	room_data_buf[1] = people; // 人群密度
	room_data_buf[2] = temp;   // 温度
	room_data_buf[3] = humi;   // 湿度
	room_data_buf[4] = noise;  // 噪音
	room_data_buf[5] = 0xFF;   // 消息尾

	os_printf("people = %d\t",room_data_buf[1]);
	os_printf("temp = %d\t",room_data_buf[2]);
	os_printf("humi = %d\t",room_data_buf[3]);
	os_printf("noise = %d\t",room_data_buf[4]);

}
