#include "room_info.h"


uint8 room;
uint8 people = 5;
uint8 temp = 0;
uint8 humi = 0;
uint8 noise = 0;



void get_noise(void){
	uint16 adc = system_adc_read();
	//os_printf("adc1 is %d db\r",adc);
	uint16 adc2 = (adc / (1024 / 3.3))*10;
	//os_printf("adc2 is %d db\r",adc2);
	noise =( 100 * (adc2 - 4 ) /16 )+ 30;  //电压模拟值转分贝
	//os_printf("noise is %d db\r\n", noise);

}

void get_humi_temp(void){
    dh11Read(&temp,&humi);
    //os_printf("Temp: %d'C Humi: %d%\n",temp,humi);

}

void get_people(void){

	// sniffer
	//os_printf("sniffer people \n");

}



void get_room_data(void){

	get_noise();
	get_humi_temp();
	get_people();

}

int ICACHE_FLASH_ATTR
command_set(struct jsontree_context *js_ctx, struct jsonparse_state *parser) {
	//--------
	int device;
	int command;

	//--------
	int type;

	while ((type = jsonparse_next(parser)) != 0) {
		//濡傛灉鏄疜EY绫诲瀷
		if (type == JSON_TYPE_PAIR_NAME) {

			if (jsonparse_strcmp_value(parser, "dev") == 0) {
				jsonparse_next(parser);	//杩斿洖鐨勬槸鍐掑彿瀛楃
				type = jsonparse_next(parser);	//杩斿洖鐨勬槸鍙屽紩鍙峰瓧绗�

				//濡傛灉Value鏄瓧绗︿覆绫诲瀷锛屽垯璇诲彇鏁版嵁鍒癰uffer
				if (JSON_TYPE_NUMBER == type) {	//#define JSON_TYPE_STRING '"'
					device = jsonparse_get_value_as_int(parser);
					os_printf("device Value = %s\n", device);
				}

			} else if (jsonparse_strcmp_value(parser, "comm") == 0) {
				jsonparse_next(parser);
				type = jsonparse_next(parser);

				//濡傛灉Value鏄暟鍊肩被鍨�
				if (JSON_TYPE_NUMBER == type) {	//#define JSON_TYPE_NUMBER '0'
					command = jsonparse_get_value_as_int(parser);
					os_printf("command Value = %d\n", command);
				}

			} else
				os_printf("No parse key name. \n");
		}

	}

	doCommand(device, command); // 鎵ц鎺у埗鍛戒护锛屾帶鍒剁户鐢靛櫒鐨勫紑鍏�

	return 0;
}

void ICACHE_FLASH_ATTR
doCommand(int device, int command) {

	switch (device) {

	case DEVICE_LIGHT_1:
		// do something
		// gpio_output_set(BIT12, 0, BIT12, 0): //璁剧疆GPIO12杈撳嚭楂樼數骞�
		// gpio_output_set(0, BIT12, BIT12, 0): //璁剧疆GPIO12杈撳嚭浣庣數骞�
		// gpio_output_set(0, 0, 0, BIT12):  //璁剧疆GPIO12涓鸿緭鍏�
		break;
	case DEVICE_LIGHT_2:
		//do somthing
		break;
		//	case ...
	default:
		os_printf("device %d no found.\n", device);
	}

}

int ICACHE_FLASH_ATTR
classroom_get(struct jsontree_context *js_ctx) {

	const char *path = jsontree_path_name(js_ctx, js_ctx->depth - 1);

	if (os_strncmp(path, "room", 4) == 0) {
		jsontree_write_int(js_ctx, room);
	} else if (os_strncmp(path, "people", 6) == 0) {
		jsontree_write_int(js_ctx, people);
	} else if (os_strncmp(path, "temp", 4) == 0) {
		jsontree_write_int(js_ctx, temp);
	} else if (os_strncmp(path, "humi", 3) == 0) {
		jsontree_write_int(js_ctx, humi);
	} else if (os_strncmp(path, "noise", 5) == 0) {
		jsontree_write_int(js_ctx, noise);
	}

	return 0;
}

/*  json 鏁扮粍娴嬭瘯
 LOCAL struct jsontree_callback jsonArrayCallback =
 JSONTREE_CALLBACK(jsonArray_get, NULL);

 JSONTREE_OBJECT(jsonArrayData,
 JSONTREE_PAIR("mac", &jsonArrayCallback),
 JSONTREE_PAIR("rssi", &jsonArrayCallback));
 JSONTREE_ARRAY(jsonArray,
 JSONTREE_PAIR_ARRAY(&jsonArrayData),
 JSONTREE_PAIR_ARRAY(&jsonArrayData),
 JSONTREE_PAIR_ARRAY(&jsonArrayData));
 */

LOCAL struct jsontree_callback cr_callback =
JSONTREE_CALLBACK(classroom_get, command_set);

JSONTREE_OBJECT(get_status, JSONTREE_PAIR("room", &cr_callback),
		JSONTREE_PAIR("people", &cr_callback),
		JSONTREE_PAIR("temp", &cr_callback),
		JSONTREE_PAIR("humi", &cr_callback),
		JSONTREE_PAIR("noise", &cr_callback));
JSONTREE_OBJECT(get_room, JSONTREE_PAIR("status", &get_status));

char* ICACHE_FLASH_ATTR
get_room_json(void) {
	get_room_data();
	static char room_buf[64];
	os_memset(room_buf, 0, 64);		//脟氓驴脮
	json_ws_send((struct jsontree_value *) &get_room, "status", room_buf);
	return room_buf;
}

void ICACHE_FLASH_ATTR
setCommand(char *json) {
	struct jsontree_context js;

	jsontree_setup(&js, (struct jsontree_value *) &get_room, json_putchar);
	json_parse(&js, json);
}
