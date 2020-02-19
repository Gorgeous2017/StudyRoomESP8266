/**
 * ******************************************************************************
 * @file nb_bc35.c
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

#include "nb_bc35.h"

os_timer_t nb_response_timer;

void ICACHE_FLASH_ATTR
NB_ResponseTimerCb(void *arg) {

	// uint8 *response_msg = (uint8 *)arg;

	// //if (0 != os_strcmp(uart_buff, response_msg)) {
	// if (0 != os_strstr(uart_buff, response_msg)) {

	// 	ESP_DEBUG("OK! response message parse!");

	// 	os_timer_disarm(&nb_response_timer);
	// 	os_memset(uart_buff,0,sizeof(uart_buff))
	// 	NB_Init();

	// 	return;

	// } else {

	// 	ESP_DEBUG("ERROR! response message parse failed");

	// 	return;
	// }
}


/**
 * @brief 上报数据到云端
 * 
 * @par AT Command
 * 	AT+NMGS=数据长度,内容
 * 
 * 	例如：
 * 
 * 	发送0、1两个数据：AT+NMGS=02,0001
 * 
 * 	发送9、10、11、16、17、32、33七个数据：AT+NMGS=07,090A0B10112021
 * 
 * @note 上报数据的格式需要与IoT平台的“编解码插件”的解码顺序一致，不然上报的数据无法正确解析
 * 
 * @param[in] data_str 需要上报的十六进制数据字符串
 */
void NB_ReportData(uint8 *data_str) {
	
	uint8 report_buf[128];

	/* 因为数据字符串是十六进制的，两个字符表示一个十六进制数，故数据长度为os_strlen(data_str)/2 */
	os_sprintf(report_buf, AT_NB_REPORT_PREFIX "%02d,%s" , os_strlen(data_str)/2, data_str );

	NB_SendCmd(report_buf, os_strlen(report_buf), NULL);

}



uint8 response_flag = 0; /*!< 响应消息接收标志 */
uint8 response_msg[128]; /*!< 期望接收到的响应消息 */

/**
 * @brief 向NB模组发送指令
 * 
 * @details 
 * 	1. 将指令通过串口发送给NB模组
 *  2. 将响应消息复制到响应消息数组 response_msg[]
 *  3. 置位响应消息接收标志 response_flag。此标志位用于给串口消息处理函数 NB_RxMsgHandler
 * 里的分支作判断，从而决定是否将接受的消息与响应消息作匹配。因为串口接收到的消息可能是云端
 * 下发的指令，所以这里面加了这个标志位。
 * 
 * @param[in] cmd 要发送的指令字符串
 * @param[in] cmd_len 指令字符串长度
 * @param[in] res_msg 期望接收到的响应消息，参见NB模组的AT指令参考手册
 * 
 * @todo 增加对查询指令返回的查询结果信息的处理。将采用匹配消息头和存储消息体的方法。
 * 
 */
void NB_SendCmd(uint8 *cmd, uint8 cmd_len, uint8 *res_msg) {

	uint8 at_cmd[128];
	//os_memset(at_cmd,0,128);

	os_printf("MCU >>>>>> NB : %s\n", cmd);

	/* 这里在命令前面加上 AT_CMD_BEGIN 是为了清除掉NB模块中可能存在的消息缓存 */
	/* 若是在调用该函数前NB模块中缓存了一些“意料之外”的消息，这时发送的指令会连同前面的消息
	 一同被NB模块解析，结果必然出错。所以在每次发送消息前加上了 AT_CMD_BEGIN */
	os_sprintf(at_cmd, AT_CMD_BEGIN "%s" AT_LINE_END, cmd);
	uart1_tx_buffer(at_cmd, cmd_len+2);

	/* 拷贝响应消息到响应消息数组并置位响应标志位，等待串口串接收中断调用 */

	if (res_msg != NULL) {	
		os_memcpy(response_msg, res_msg, sizeof(res_msg));
		os_printf("response_msg is %s\n", response_msg);
		response_flag = 1;
	}

	// os_timer_disarm(&nb_response_timer);
	// os_timer_setfn(&nb_response_timer, (os_timer_func_t *) NB_ResponseTimerCb, (void *)response_msg);
	// os_timer_arm(&nb_response_timer, response_time_ms, 1);

}

/**
 * @brief NB消息处理函数
 * 
 * @details 
 * 	NB消息有以下几种类型：
 * 	- 动作指令的执行情况。如“OK”、“ERROR”
 * 	- 查询指令的查询结果
 *  - 云端下发的控制命令。以“+NNMI:”为前缀
 * 
 * 	当NB发送消息来的时候，我们先判断在接收到这条消息之前有没有发送过指令，即判断响应标志位
 *  response_flag是否被置位，再决定是否去匹配响应消息。
 * 
 * @param[in] nb_msg 串口接收到的NB消息
 */
void NB_RxMsgHandler(uint8 *nb_msg ) {

	if (response_flag != 0 ) { /* 等待响应匹配 */

		ESP_DEBUG("response_flag = 1");
		
		if (0 != os_strstr(nb_msg, response_msg)) {

			ESP_DEBUG("OK! response message parse!");

			os_memset(response_msg, 0, sizeof(response_msg));
			response_flag = 0;

			NB_Init();

			return;

		} else {

			ESP_DEBUG("ERROR! response message parse failed");

			return;
		}

	} else {

		ESP_DEBUG("response_flag = 0");

		/* 匹配下发消息前缀 */

		/* 命令下发 */

	}

}

void NB_Init(void) { 

	static uint8 cmd_order = 1;

	ESP_DEBUG("cmd_order = %d", cmd_order);

	switch (cmd_order) {

		case 0:
			NB_SendCmd("", 1, "OK");
			cmd_order = 1;
			break;
		case 1:
			NB_SendCmd("AT", os_strlen("AT"), "OK");
			cmd_order = 2;
			break;
		case 2:
			NB_SendCmd(AT_NB_OPEN_RF, os_strlen(AT_NB_OPEN_RF), "OK");
			cmd_order = 3;
			break;
		case 3:
			NB_SendCmd(AT_NB_CLOSE_PSM, os_strlen(AT_NB_CLOSE_PSM), "OK");
			cmd_order = 4;
			break;
		case 4:
			NB_SendCmd(AT_NB_CLOSE_EDRX, os_strlen(AT_NB_CLOSE_EDRX), "OK");
			cmd_order = 5;
			break;
		case 5:
			NB_SendCmd(AT_NB_CGATT_ATTACH, os_strlen(AT_NB_CGATT_ATTACH), "OK");
			cmd_order = 10;
			break;
		case 6:
			NB_SendCmd(AT_NB_OPEN_RF, os_strlen(AT_NB_OPEN_RF), "OK");
			NB_SendCmd("AT", os_strlen("AT"), "OK");
			cmd_order = 6;
			break;
		default:
			break;
	}

}

/**
 * @brief 将十六进制码流转换为字符串形式
 * 
 * @param[in] bufin 输入的十六进制码流
 * @param[in] len 十六进制码流的长度
 * @param[out] bufout 输出的十六进制字符串
 * @return int 
 * 
 * @par 示例
 * bufin[0] = 0x01, bufin[1] = 0x02, bufin[2] = 0x03, len = 3, 则bufout = "010203"
 * bufin[0] = 0x01, bufin[1] = 0x05, bufin[2] = 0x06, bufin[3] = 0x07, len = 4, 则bufout = "01050607"
 * 
 */
int str_to_hex(const char *bufin, int len, char *bufout)
{
	int i = 0;
	if (NULL == bufin || len <= 0 || NULL == bufout)
	{
		return -1;
	}
	for(i = 0; i < len; i++)
	{
		os_sprintf(bufout+i*2, "%02X", bufin[i]);
	}
	return 0;
}

/**
 * @brief 将字符串形式的十六进制码流转为 unsigned char 类型
 * 
 * @param[in] source  从串口接收到的字符串形式的十六进制码流
 * @param[out] dest 转换好的十六进制码流
 * @param[in] sourceLen 十六进制码流的长度
 * 
 * @par 示例
 * source = "010203", sourceLen = 3, 则dest[0]=0x01, dest[1]=0x02, dest[2]=0x03
 * source = "01050607", sourceLen = 4, 则dest[0]=0x01, dest[1]=0x05, dest[2]=0x06, dest[3]=0x07
 */
void HexStrToStr(const unsigned char *source, unsigned char *dest, int sourceLen)
{
	short i;
	unsigned char highByte, lowByte;
	for (i = 0; i < sourceLen; i += 2)
	{
		/* 将HEX的字母统一转换大小写，方便后面的字母与数字转换 */
		highByte = toupper(source[i]);
		lowByte  = toupper(source[i + 1]);
		
		/* 转换ASCII字符为所代表的十六进制数 */
		if (highByte > 0x39) {
			highByte -= 0x37;
		} else {
			highByte -= 0x30;
		}
		if (lowByte > 0x39) {
			lowByte -= 0x37;
		} else {
			lowByte -= 0x30;
		}
		
		/* 高低位合并 */
		/* 因为十六进制数的HexStr形式是由两位ASCII码来表示的，所以在 */
		/* 将HexStr转为实际的十六进制码流时需要合并高低位 */
		dest[i / 2] = (highByte << 4) | lowByte;
	}
	return ;
}

// int32_t nb_reboot(void)
// {
//    // memset(sockinfo, 0, MAX_SOCK_NUM * sizeof(struct _socket_info_t));
// 	return at.cmd((int8_t*)AT_NB_reboot, strlen(AT_NB_reboot), "OK", NULL,NULL);
// }

// /**
//  * @brief 查询射频是否开启
//  * 
//  * @return int32_t 
//  */
// int32_t nb_hw_detect(void)//"AT+CFUN?\r"
// {
// 	return at.cmd((int8_t*)AT_NB_hw_detect, strlen(AT_NB_hw_detect), "+CFUN:1", NULL,NULL); /* +CFUN:1：射频已经开启 */
// }

// /**
//  * @brief 检测接收信号的强度 RSSI
//  * 
//  * @return int32_t 
//  */
// int32_t nb_check_csq(void)
// {
// 	char *cmd = "AT+CSQ\r";
// 	return at.cmd((int8_t*)cmd, strlen(cmd), "+CSQ:", NULL,NULL);
// }

// /**
//  * @brief 设置 CDP（持续数据保护） 服务器，即华为IoT平台
//  * 
//  * @param[in] host  主机地址
//  * @param[in] port  端口号
//  * @return int32_t 
//  */
// int32_t nb_set_cdpserver(char* host, char* port)
// {
// 	char *cmd = "AT+NCDP="; /*!< 设置 CDP 服务器地址指令 */
// 	char *cmd2 = "AT+NCDP?"; /*!< 查询已设置的 CDP 服务器地址指令 */
// 	char *cmdNNMI = "AT+NNMI=1\r"; /*!< 设置终端出现问题时自动上报指令 */
// 	char *cmdCMEE = "AT+CMEE=1\r"; /*!< 设置平台发送消息过来时可以自动上报指令 */
// 	//char *cmdCGP = "AT+CGPADDR";
// 	char tmpbuf[128] = {0};
// 	int ret = -1;
// 	char ipaddr[100] = {0};

// 	/* 输入的主机地址和端口号有误时提前返回 */
// 	if(strlen(host) > 70 || strlen(port) > 20 || host==NULL || port == NULL)
// 	{
// 		ret = at.cmd((int8_t*)cmdNNMI, strlen(cmdNNMI), "OK", NULL,NULL);
// 		ret = at.cmd((int8_t*)cmdCMEE, strlen(cmdCMEE), "OK", NULL,NULL);
// 		return ret;
// 	}

// 	/* 字符串拼接，tmpbuf = AT+NCDP=host,port */
// 	snprintf(ipaddr, sizeof(ipaddr) - 1, "%s,%s\r", host, port);
// 	snprintf(tmpbuf, sizeof(tmpbuf) - 1, "%s%s%c", cmd, ipaddr, '\r');

// 	ret = at.cmd((int8_t*)tmpbuf, strlen(tmpbuf), "OK", NULL,NULL);
// 	if(ret < 0)
// 	{
// 		return ret;
// 	}
// 	/* 查询设置后的地址是否一致 */
// 	ret = at.cmd((int8_t*)cmd2, strlen(cmd2), ipaddr, NULL,NULL);
// 	//LOS_TaskDelay(1000);
// 	ret = at.cmd((int8_t*)cmdNNMI, strlen(cmdNNMI), "OK", NULL,NULL);
// 	//at.cmd((int8_t*)cmdCMEE, strlen(cmdCMEE), "OK", NULL, NULL);
// 	//ret = at.cmd((int8_t*)cmdCGP, strlen(cmdCGP), NULL, NULL);
// 	return ret;
// }

// int32_t nb_send_psk(char* pskid, char* psk)
// {
// 	char* cmds = "AT+QSECSWT";//AT+QSECSWT=1,100    OK
// 	char* cmdp = "AT+QSETPSK";//AT+QSETPSK=86775942,E6F4C799   OK
// 	sprintf(wbuf, "%s=%d,%d\r", cmds, 1, 100);//min
// 	at.cmd((int8_t*)wbuf, strlen(wbuf), "OK", NULL,NULL);
// 	snprintf(wbuf, AT_DATA_LEN, "%s=%s,%s\r", cmdp, pskid, psk);
// 	return at.cmd((int8_t*)wbuf, strlen(wbuf), "OK", NULL,NULL);
// }

// /**
//  * @brief 发送数据至平台
//  * 
//  * @param[in] buf  需要发送的数据
//  * @param[in] len  数据长度
//  * @return int32_t 
//  */
// int32_t nb_send_payload(const char* buf, int len)
// {
// 	char *cmd1 = "AT+NMGS="; /*!< 发送数据指令 */
// 	char *cmd2 = "AT+NQMGS\r";
// 	int ret;
// 	char* str = NULL;
// 	int curcnt = 0;
// 	int rbuflen;
// 	static int sndcnt = 0;

// 	/* 数据异常处理 */
// 	if(buf == NULL || len > AT_MAX_PAYLOADLEN)
// 	{
// 		AT_LOG("payload too long");
// 		return -1;
// 	}

// 	memset(tmpbuf, 0, AT_DATA_LEN);
// 	memset(wbuf, 0, AT_DATA_LEN);
// 	str_to_hex(buf, len, tmpbuf); /* 将buf的值转为十六进制存入tmpbuf中 */
// 	memset(rbuf, 0, AT_DATA_LEN);
// 	/* 格式化字符串 AT+NMGS=len,tmpbuf  */
// 	snprintf(wbuf, AT_DATA_LEN,"%s%d,%s%c",cmd1,(int)len,tmpbuf,'\r');
// 	ret = at.cmd((int8_t*)wbuf, strlen(wbuf), "OK", NULL,NULL);
// 	if(ret < 0)
// 		return -1;

// 	/* ？？验证已发送的数据？ */
// 	ret = at.cmd((int8_t*)cmd2, strlen(cmd2), "SENT=", rbuf,&rbuflen);
// 	if(ret < 0)
// 		return -1;
// 	str = strstr(rbuf,"SENT=");
// 	if(str == NULL)
// 		return -1;
// 	sscanf(str,"SENT=%d,%s",&curcnt,wbuf);
// 	if(curcnt == sndcnt)
// 		return -1;
// 	sndcnt = curcnt;
// 	return ret;
// }

// /**
//  * @brief 查询已经设置的IP地址
//  * 
//  * @return int 
//  */
// int nb_query_ip(void)
// {
// 	char *cmd = "AT+CGPADDR\r";
// 	return at.cmd((int8_t*)cmd, strlen(cmd), "+CGPADDR:0,", NULL,NULL);
// }

// /**
//  * @brief 查询网络状态
//  * 
//  * @return int32_t 
//  */
// int32_t nb_get_netstat(void)
// {
// 	char *cmd = "AT+CGATT?\r";
// 	return at.cmd((int8_t*)cmd, strlen(cmd), "CGATT:1", NULL,NULL);
// }

// static int32_t nb_cmd_with_2_suffix(const int8_t *cmd, int  len,
// 						const char* suffix_ok, const char* suffix_err,  char *resp_buf, uint32_t* resp_len)
// {

// 	const char *suffix[2] = {0};
// 	at_cmd_info_s cmd_info = {0};

// 	suffix[0] = suffix_ok;
// 	suffix[1] = suffix_err;

// 	cmd_info.suffix = suffix;
// 	cmd_info.suffix_num = array_size(suffix);

// 	cmd_info.resp_buf = resp_buf;
// 	cmd_info.resp_len = resp_len;

// 	if (at.cmd_multi_suffix(cmd, len, &cmd_info) != AT_OK)
// 	{
// 		return AT_FAILED;
// 	}

// 	if (cmd_info.match_idx != 0)
// 	{
// 		AT_LOG("cmd_info.match_idx %d", cmd_info.match_idx);
// 		return AT_FAILED;
// 	}

// 	return AT_OK;
// }


// int32_t nb_bind(const int8_t * host, const int8_t *port, int32_t proto)
// {
// 	int ret = 0;
// 	int portnum;

// 	(void)host;
// 	(void)proto;
// 	portnum = chartoint((char*)port);

// 	if (nb_create_sock_link(portnum, &ret) != AT_OK)
// 	{
// 		return AT_FAILED;
// 	}

// 	sockinfo[ret].localport = *(unsigned short*)portnum;

// 	return AT_OK;
// }

// int32_t nb_connect(const int8_t * host, const int8_t *port, int32_t proto)
// {
// 	int ret = 0;
// 	static uint16_t localport = NB_STAT_LOCALPORT;
// 	const int COAP_SEVER_PORT = 5683;

// 	if (nb_create_sock_link(localport, &ret) != AT_OK)
// 	{
// 		return AT_FAILED;
// 	}

// 	localport++;
// 	if (localport == COAP_SEVER_PORT || localport == (COAP_SEVER_PORT + 1))
// 	{
// 		localport = 5685;
// 	}

// 	strncpy(sockinfo[ret].remoteip, (const char *)host, sizeof(sockinfo[ret].remoteip));
// 	sockinfo[ret].remoteip[sizeof(sockinfo[ret].remoteip) - 1] = '\0';
// 	sockinfo[ret].remoteport = chartoint((char*)port);

// 	AT_LOG("ret:%d remoteip:%s port:%d",ret,sockinfo[ret].remoteip,sockinfo[ret].remoteport);

// 	return ret;

// }


// int32_t nb_sendto(int32_t id , const uint8_t  *buf, uint32_t len, char* ipaddr, int port)
// {
// 	char *cmd = "AT+NSOST=";
// 	int data_len = len/2;
// 	int cmd_len;

// 	if(buf == NULL || data_len > AT_MAX_PAYLOADLEN || id >= MAX_SOCK_NUM)
// 	{
// 		AT_LOG("invalid args");
// 		return -1;
// 	}

// 	AT_LOG("id:%d remoteip:%s port:%d",(int)sockinfo[id].socket, ipaddr, port);
// 	memset(wbuf, 0, AT_DATA_LEN);
// 	memset(tmpbuf, 0, AT_DATA_LEN);
// 	str_to_hex((const char *)buf, len, tmpbuf);

// 	cmd_len = snprintf(wbuf, AT_DATA_LEN, "%s%d,%s,%d,%d,%s\r",cmd,(int)sockinfo[id].socket,
// 		ipaddr, port, (int)len, tmpbuf);


// 	if (nb_cmd_with_2_suffix((int8_t*)wbuf, cmd_len, "OK", "ERROR",
// 				NULL, NULL) != AT_OK)
// 	{
// 		return AT_FAILED;
// 	}

// 	return len;
// }


// int32_t nb_send(int32_t id , const uint8_t *buf, uint32_t len)
// {
// 	if (id >= MAX_SOCK_NUM)
// 	{
// 		AT_LOG("invalid args");
// 		return AT_FAILED;
// 	}
// 	return nb_sendto(id , buf, len, sockinfo[id].remoteip,(int)sockinfo[id].remoteport);
// }


// int32_t nb_recv(int32_t id , uint8_t  *buf, uint32_t len)
// {
// 	return nb_recv_timeout(id, buf, len,NULL,NULL, LOS_WAIT_FOREVER);
// }

// int32_t nb_recvfrom(int32_t id , uint8_t  *buf, uint32_t len,char* ipaddr,int* port)
// {
// 	return nb_recv_timeout(id, buf, len, ipaddr,port,LOS_WAIT_FOREVER);
// }

// int32_t nb_recv_timeout(int32_t id , uint8_t  *buf, uint32_t len,char* ipaddr,int* port, int32_t timeout)
// {
// 	int rlen = 0;
// 	int ret;
// 	QUEUE_BUFF	qbuf;
// 	UINT32 qlen = sizeof(QUEUE_BUFF);

// 	if (id  >= MAX_SOCK_NUM)
// 	{
// 		AT_LOG("link id %ld invalid", id);
// 		return AT_FAILED;
// 	}


// 	ret = LOS_QueueReadCopy(at.linkid[id].qid, &qbuf, &qlen, timeout);
// 	//AT_LOG("wwww LOS_QueueReadCopy data,qid %d, len %ld, ret %d", at.linkid[id].qid, qbuf.len, ret);
// 	if (ret != LOS_OK)
// 	{
// 		return AT_TIMEOUT;
// 	}


// 	if (('\0' == sockinfo[id].remoteip[0])
// 		|| (0 == sockinfo[id].remoteport))
// 	{
// 		AT_LOG("update ip and port for link %ld", id);
// 		strncpy(sockinfo[id].remoteip, qbuf.ipaddr, sizeof(sockinfo[id].remoteip));
// 		sockinfo[id].remoteip[sizeof(sockinfo[id].remoteip) - 1] = '\0';
// 		sockinfo[id].remoteport = qbuf.port;
// 	}

// 	if(ipaddr != NULL)
// 	{
// 		memcpy(ipaddr,qbuf.ipaddr,strlen(qbuf.ipaddr));
// 		*port = qbuf.port;
// 	}

// 	rlen = MIN(qbuf.len, len);

// 	//AT_LOG("recv data, %d", rlen);

// 	if (rlen){
// 		memcpy(buf, qbuf.addr, rlen);
// 		at_free(qbuf.addr);
// 	}
// 	return rlen;

// }


// int32_t nb_close(int32_t id)
// {
// 	int ret;

// 	if ((id  >= MAX_SOCK_NUM)
// 		|| (!sockinfo[id].used_flag))
// 	{
// 		AT_LOG("link id %ld invalid", id);
// 		return AT_FAILED;
// 	}

// 	nb_close_sock(sockinfo[id].socket);

// 	do
// 	{
// 		QUEUE_BUFF	qbuf = {0};
// 		UINT32 qlen = sizeof(QUEUE_BUFF);
// 		ret = LOS_QueueReadCopy(at.linkid[id].qid, &qbuf, &qlen, 0);
// 		if (ret == LOS_OK && qbuf.addr != NULL)
// 		{
// 			at_free(qbuf.addr);
// 		}
// 	}while(ret == LOS_OK);
// 	ret = LOS_QueueDelete(at.linkid[id].qid);
// 	if (ret != LOS_OK)
// 	{
// 		AT_LOG("LOS_QueueDelete failed, ret is %d!,qid %d", ret, at.linkid[id].qid);
// 	}
// 	(void)memset(&sockinfo[id], 0, sizeof(sockinfo[id]));

// 	return AT_OK;
// }

// int32_t nb_recv_cb(int32_t id)
// {
// 	return AT_FAILED;
// }

// static int32_t nb_init(void)
// {
// 	at_config at_user_conf = {
// 		.name = AT_MODU_NAME,
// 		.usart_port = AT_USART_PORT,
// 		.buardrate = AT_BUARDRATE,
// 		.linkid_num = AT_MAX_LINK_NUM,
// 		.user_buf_len = MAX_AT_USERDATA_LEN,
// 		.cmd_begin = AT_CMD_BEGIN,
// 		.line_end = AT_LINE_END,
// 		.mux_mode = 1, //support multi connection mode
// 		.timeout = AT_CMD_TIMEOUT,   //  ms
// 	};
	
// 	at_set_config(&at_user_conf);
// 	memset(&sockinfo, 0, sizeof(sockinfo));
// 	memset(&g_data_ind_info, 0, sizeof(g_data_ind_info));
// 	at_reg_step_callback(&at, nb_step);

// 	return AT_OK;
// }

// int32_t nb_deinit(void)
// {

// 	for (int i = 0; i < MAX_SOCK_NUM; ++i)
// 	{
// 		if (sockinfo[i].used_flag)
// 		{
// 			nb_close(i);
// 		}
// 	}
// 	return nb_reboot();
// }

// at_adaptor_api bc95_interface =
// {
// 	.init = nb_init,

// 	.bind = nb_bind,

// 	.connect = nb_connect,
// 	.send = nb_send,
// 	.sendto = nb_sendto,

// 	.recv_timeout = nb_recv_timeout,
// 	.recv = nb_recv,
// 	.recvfrom = nb_recvfrom,

// 	.close = nb_close,
// 	.recv_cb = nb_recv_cb,

// 	.deinit = nb_deinit,
// };

// void nb_reattach(void)
// {
// 	(void)nb_cmd_with_2_suffix((int8_t*)CGATT, strlen(CGATT), "OK", "ERROR", NULL, NULL);
// 	 (void)nb_cmd_with_2_suffix((int8_t*)CGATT_DEATTACH, strlen(CGATT_DEATTACH), "OK", "ERROR", NULL, NULL);
// 	 LOS_TaskDelay(1000);
// 	 (void)nb_cmd_with_2_suffix((int8_t*)CGATT_ATTACH, strlen(CGATT_ATTACH), "OK", "ERROR", NULL, NULL);
// }

// static int nb_cmd_rcv_data(int sockid, int readleft);


// static int32_t nb_handle_sock_data(const int8_t *data, uint32_t len)
// {
// 	(void) len;
// 	char *curr = (char *)data;

// 	if (strstr((char *) data, "ERROR") != NULL)
// 	{
// 		return AT_OK;
// 	}

// 	do
// 	{

// 		int readleft;
// 		int sockid;

// 		char *next = strstr(curr, "\r\n");

// 		if (next == curr)
// 		{
// 			curr += 2;
// 		}

// 		if (next != NULL)
// 		{
// 			next += 2;
// 		}

// 		if (nb_decompose_str(curr, &readleft, &sockid) == AT_OK)
// 		{
// 		/*
// 			if (readleft != 0)
// 			{
// 				nb_cmd_rcv_data(sockid, readleft);
// 			}*/
// 			return AT_OK;
// 		}
// 		curr = next;
// 	}while(curr);

// 	return AT_FAILED;
// }


// static int nb_cmd_rcv_data(int sockid, int readleft)
// {
// 	int cmdlen;
// 	char cmdbuf[40];
// 	const char* cmd = "AT+NSORF=";
// 	const uint32_t timeout = 10;

// 	cmdlen = snprintf(cmdbuf, sizeof(cmdbuf), "%s%d,%d\r", cmd, sockid, readleft);
// 	return at_cmd_in_callback((int8_t*)cmdbuf, cmdlen, nb_handle_sock_data, timeout);
// }

// static int32_t nb_handle_data_ind(const char *buf)
// {
// 	int32_t sockid;
// 	int32_t data_len;
// 	const char *p1, *p2;
// 	int link_idx;

// 	p2 = strstr(buf, AT_DATAF_PREFIX);
// 	if (NULL == p2)
// 	{
// 		return AT_FAILED;
// 	}
// 	p2+=strlen(AT_DATAF_PREFIX);

// 	p1 = strstr(p2, ",");
// 	if (p1 == NULL)
// 	{
// 		return AT_FAILED;
// 	}
// 	sockid = chartoint(p2);
// 	data_len = chartoint(p1 + 1);
// 	link_idx = nb_sock_to_idx(sockid);
// 	if (link_idx >= MAX_SOCK_NUM)
// 	{
// 		AT_LOG("invalid sock id %ld", sockid);


// 		return AT_OK;
// 	}

// 	if (nb_cmd_rcv_data(sockid, data_len) != AT_OK)
// 	{
// 		g_data_ind_info.data_len = (uint32_t)data_len;
// 		g_data_ind_info.link_idx = link_idx;
// 		g_data_ind_info.valid_flag = true;
// 	}
// 	else
// 	{
// 		g_data_ind_info.valid_flag = false;
// 	}

// 	return AT_OK;

// }


// int32_t nb_cmd_match(const char *buf, char* featurestr,int len)
// {
// 	if (buf == NULL)
// 	{
// 		return AT_FAILED;
// 	}

// 	nb_handle_data_ind(buf);

// 	return AT_FAILED;
// }

// void nb_step(void)
// {
// 	if ((!g_data_ind_info.valid_flag)
// 		|| (!sockinfo[g_data_ind_info.link_idx].used_flag))
// 	{
// 		return;
// 	}
// 	if (nb_cmd_rcv_data(sockinfo[g_data_ind_info.link_idx].socket, g_data_ind_info.data_len) == AT_OK)
// 	{
// 		g_data_ind_info.valid_flag = false;
// 	}
// }

