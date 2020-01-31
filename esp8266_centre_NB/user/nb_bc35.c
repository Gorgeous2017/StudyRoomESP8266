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
#include <ctype.h>
#if defined(WITH_AT_FRAMEWORK)
#include "at_device/bc95.h"
#include "at_hal.h"

extern at_task at;
at_adaptor_api bc95_interface;
extern char rbuf[AT_DATA_LEN];
extern char wbuf[AT_DATA_LEN];

typedef struct
{
	uint32_t data_len;
	int link_idx;
	bool valid_flag;
}nb_data_ind_info_s;

char tmpbuf[AT_DATA_LEN]={0}; //transform to hex

socket_info sockinfo[MAX_SOCK_NUM];
static nb_data_ind_info_s g_data_ind_info;

#if defined ( __CC_ARM ) || defined ( __ICCARM__ )
static char *strnstr(const char *s1, const char *s2, size_t len)
{
	size_t l2;

	l2 = strlen(s2);
	if (!l2)
		return (char *)s1;
	while (len >= l2) {
		len--;
		if (!memcmp(s1, s2, l2))
			return (char *)s1;
		s1++;
	}
	return NULL;
}
#endif

/**
 * @brief 将十六进制码流转换为字符串形式
 * 
 * @param bufin 输入的十六进制码流
 * @param len 十六进制码流的长度
 * @param bufout 输出的十六进制字符串
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
		sprintf(bufout+i*2, "%02X", bufin[i]);
	}
	return 0;
}

/**
 * @brief 将字符串形式的十六进制码流转为 unsigned char 类型
 * 
 * @param source [IN] 从串口接收到的字符串形式的十六进制码流
 * @param dest [OUT] 转换好的十六进制码流
 * @param sourceLen 十六进制码流的长度
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

int32_t nb_reboot(void)
{
   // memset(sockinfo, 0, MAX_SOCK_NUM * sizeof(struct _socket_info_t));
	return at.cmd((int8_t*)AT_NB_reboot, strlen(AT_NB_reboot), "OK", NULL,NULL);
}

/**
 * @brief 查询射频是否开启
 * 
 * @return int32_t 
 */
int32_t nb_hw_detect(void)//"AT+CFUN?\r"
{
	return at.cmd((int8_t*)AT_NB_hw_detect, strlen(AT_NB_hw_detect), "+CFUN:1", NULL,NULL); /* +CFUN:1：射频已经开启 */
}

/**
 * @brief 检测接收信号的强度 RSSI
 * 
 * @return int32_t 
 */
int32_t nb_check_csq(void)
{
	char *cmd = "AT+CSQ\r";
	return at.cmd((int8_t*)cmd, strlen(cmd), "+CSQ:", NULL,NULL);
}

/**
 * @brief 设置 CDP（持续数据保护） 服务器，即华为IoT平台
 * 
 * @param host [IN] 主机地址
 * @param port [IN] 端口号
 * @return int32_t 
 */
int32_t nb_set_cdpserver(char* host, char* port)
{
	char *cmd = "AT+NCDP="; /*!< 设置 CDP 服务器地址指令 */
	char *cmd2 = "AT+NCDP?"; /*!< 查询已设置的 CDP 服务器地址指令 */
	char *cmdNNMI = "AT+NNMI=1\r"; /*!< 设置终端出现问题时自动上报指令 */
	char *cmdCMEE = "AT+CMEE=1\r"; /*!< 设置平台发送消息过来时可以自动上报指令 */
	//char *cmdCGP = "AT+CGPADDR";
	char tmpbuf[128] = {0};
	int ret = -1;
	char ipaddr[100] = {0};

	/* 输入的主机地址和端口号有误时提前返回 */
	if(strlen(host) > 70 || strlen(port) > 20 || host==NULL || port == NULL)
	{
		ret = at.cmd((int8_t*)cmdNNMI, strlen(cmdNNMI), "OK", NULL,NULL);
		ret = at.cmd((int8_t*)cmdCMEE, strlen(cmdCMEE), "OK", NULL,NULL);
		return ret;
	}

	/* 字符串拼接，tmpbuf = AT+NCDP=host,port */
	snprintf(ipaddr, sizeof(ipaddr) - 1, "%s,%s\r", host, port);
	snprintf(tmpbuf, sizeof(tmpbuf) - 1, "%s%s%c", cmd, ipaddr, '\r');

	ret = at.cmd((int8_t*)tmpbuf, strlen(tmpbuf), "OK", NULL,NULL);
	if(ret < 0)
	{
		return ret;
	}
	/* 查询设置后的地址是否一致 */
	ret = at.cmd((int8_t*)cmd2, strlen(cmd2), ipaddr, NULL,NULL);
	//LOS_TaskDelay(1000);
	ret = at.cmd((int8_t*)cmdNNMI, strlen(cmdNNMI), "OK", NULL,NULL);
	//at.cmd((int8_t*)cmdCMEE, strlen(cmdCMEE), "OK", NULL, NULL);
	//ret = at.cmd((int8_t*)cmdCGP, strlen(cmdCGP), NULL, NULL);
	return ret;
}

int32_t nb_send_psk(char* pskid, char* psk)
{
	char* cmds = "AT+QSECSWT";//AT+QSECSWT=1,100    OK
	char* cmdp = "AT+QSETPSK";//AT+QSETPSK=86775942,E6F4C799   OK
	sprintf(wbuf, "%s=%d,%d\r", cmds, 1, 100);//min
	at.cmd((int8_t*)wbuf, strlen(wbuf), "OK", NULL,NULL);
	snprintf(wbuf, AT_DATA_LEN, "%s=%s,%s\r", cmdp, pskid, psk);
	return at.cmd((int8_t*)wbuf, strlen(wbuf), "OK", NULL,NULL);
}

/**
 * @brief 发送数据至平台
 * 
 * @param buf [IN] 需要发送的数据
 * @param len [IN] 数据长度
 * @return int32_t 
 */
int32_t nb_send_payload(const char* buf, int len)
{
	char *cmd1 = "AT+NMGS="; /*!< 发送数据指令 */
	char *cmd2 = "AT+NQMGS\r";
	int ret;
	char* str = NULL;
	int curcnt = 0;
	int rbuflen;
	static int sndcnt = 0;

	/* 数据异常处理 */
	if(buf == NULL || len > AT_MAX_PAYLOADLEN)
	{
		AT_LOG("payload too long");
		return -1;
	}

	memset(tmpbuf, 0, AT_DATA_LEN);
	memset(wbuf, 0, AT_DATA_LEN);
	str_to_hex(buf, len, tmpbuf); /* 将buf的值转为十六进制存入tmpbuf中 */
	memset(rbuf, 0, AT_DATA_LEN);
	/* 格式化字符串 AT+NMGS=len,tmpbuf  */
	snprintf(wbuf, AT_DATA_LEN,"%s%d,%s%c",cmd1,(int)len,tmpbuf,'\r');
	ret = at.cmd((int8_t*)wbuf, strlen(wbuf), "OK", NULL,NULL);
	if(ret < 0)
		return -1;

	/* ？？验证已发送的数据？ */
	ret = at.cmd((int8_t*)cmd2, strlen(cmd2), "SENT=", rbuf,&rbuflen);
	if(ret < 0)
		return -1;
	str = strstr(rbuf,"SENT=");
	if(str == NULL)
		return -1;
	sscanf(str,"SENT=%d,%s",&curcnt,wbuf);
	if(curcnt == sndcnt)
		return -1;
	sndcnt = curcnt;
	return ret;
}

/**
 * @brief 查询已经设置的IP地址
 * 
 * @return int 
 */
int nb_query_ip(void)
{
	char *cmd = "AT+CGPADDR\r";
	return at.cmd((int8_t*)cmd, strlen(cmd), "+CGPADDR:0,", NULL,NULL);
}

/**
 * @brief 查询网络状态
 * 
 * @return int32_t 
 */
int32_t nb_get_netstat(void)
{
	char *cmd = "AT+CGATT?\r";
	return at.cmd((int8_t*)cmd, strlen(cmd), "CGATT:1", NULL,NULL);
}

static int32_t nb_cmd_with_2_suffix(const int8_t *cmd, int  len,
						const char* suffix_ok, const char* suffix_err,  char *resp_buf, uint32_t* resp_len)
{

	const char *suffix[2] = {0};
	at_cmd_info_s cmd_info = {0};

	suffix[0] = suffix_ok;
	suffix[1] = suffix_err;

	cmd_info.suffix = suffix;
	cmd_info.suffix_num = array_size(suffix);

	cmd_info.resp_buf = resp_buf;
	cmd_info.resp_len = resp_len;

	if (at.cmd_multi_suffix(cmd, len, &cmd_info) != AT_OK)
	{
		return AT_FAILED;
	}

	if (cmd_info.match_idx != 0)
	{
		AT_LOG("cmd_info.match_idx %d", cmd_info.match_idx);
		return AT_FAILED;
	}

	return AT_OK;
}


int32_t nb_bind(const int8_t * host, const int8_t *port, int32_t proto)
{
	int ret = 0;
	int portnum;

	(void)host;
	(void)proto;
	portnum = chartoint((char*)port);

	if (nb_create_sock_link(portnum, &ret) != AT_OK)
	{
		return AT_FAILED;
	}

	sockinfo[ret].localport = *(unsigned short*)portnum;

	return AT_OK;
}

int32_t nb_connect(const int8_t * host, const int8_t *port, int32_t proto)
{
	int ret = 0;
	static uint16_t localport = NB_STAT_LOCALPORT;
	const int COAP_SEVER_PORT = 5683;

	if (nb_create_sock_link(localport, &ret) != AT_OK)
	{
		return AT_FAILED;
	}

	localport++;
	if (localport == COAP_SEVER_PORT || localport == (COAP_SEVER_PORT + 1))
	{
		localport = 5685;
	}

	strncpy(sockinfo[ret].remoteip, (const char *)host, sizeof(sockinfo[ret].remoteip));
	sockinfo[ret].remoteip[sizeof(sockinfo[ret].remoteip) - 1] = '\0';
	sockinfo[ret].remoteport = chartoint((char*)port);

	AT_LOG("ret:%d remoteip:%s port:%d",ret,sockinfo[ret].remoteip,sockinfo[ret].remoteport);

	return ret;

}


int32_t nb_sendto(int32_t id , const uint8_t  *buf, uint32_t len, char* ipaddr, int port)
{
	char *cmd = "AT+NSOST=";
	int data_len = len/2;
	int cmd_len;

	if(buf == NULL || data_len > AT_MAX_PAYLOADLEN || id >= MAX_SOCK_NUM)
	{
		AT_LOG("invalid args");
		return -1;
	}

	AT_LOG("id:%d remoteip:%s port:%d",(int)sockinfo[id].socket, ipaddr, port);
	memset(wbuf, 0, AT_DATA_LEN);
	memset(tmpbuf, 0, AT_DATA_LEN);
	str_to_hex((const char *)buf, len, tmpbuf);

	cmd_len = snprintf(wbuf, AT_DATA_LEN, "%s%d,%s,%d,%d,%s\r",cmd,(int)sockinfo[id].socket,
		ipaddr, port, (int)len, tmpbuf);


	if (nb_cmd_with_2_suffix((int8_t*)wbuf, cmd_len, "OK", "ERROR",
				NULL, NULL) != AT_OK)
	{
		return AT_FAILED;
	}

	return len;
}


int32_t nb_send(int32_t id , const uint8_t *buf, uint32_t len)
{
	if (id >= MAX_SOCK_NUM)
	{
		AT_LOG("invalid args");
		return AT_FAILED;
	}
	return nb_sendto(id , buf, len, sockinfo[id].remoteip,(int)sockinfo[id].remoteport);
}

os_timer_t nb_response_timer;

void ICACHE_FLASH_ATTR
nb_response_timer_cb(void *arg) {

	uint8 *response_msg = (uint8 *)arg;

	if (0 != os_strcmp(uart_buff, response_msg)) {

	}


}

void NB_SendCmd(uint8 *cmd, uint8 cmd_len, uint8 *response_msg, uint32 response_time_ms ) {

	uart1_tx_buffer(cmd, cmd_len);

	os_timer_disarm(&nb_response_timer);
	os_timer_setfn(&nb_response_timer, (os_timer_func_t *) nb_response_timer_cb, (void *)response_msg);
	os_timer_arm(&nb_response_timer, response_time_ms, 1);

}

void NB_Init(void) { 

	static uint8 cmd_order = 0;

	switch (cmd_order) {

		case 0:
			NB_SendCmd(AT_NB_OPEN_RF, os_strlen(AT_NB_OPEN_RF), "OK",100);
			cmd_order = 1;
			break;
		case 1:
			NB_SendCmd(AT_NB_CLOSE_PSM, os_strlen(AT_NB_CLOSE_PSM), "OK",100);
			cmd_order = 2;
			break;
		case 2:
			NB_SendCmd(AT_NB_CLOSE_EDRX, os_strlen(AT_NB_CLOSE_EDRX), "OK",100);
			cmd_order = 3;
			break;
		case 3:
			NB_SendCmd(AT_NB_CGATT_ATTACH, os_strlen(AT_NB_CGATT_ATTACH), "OK",100);
			//cmd_order = 4;
			break;
		case 4:
			NB_SendCmd(AT_NB_OPEN_RF, os_strlen(AT_NB_OPEN_RF), "OK",100);
			cmd_order = 5;
			break;
		case 5:
			
			break;
		case 6:
			
			break;
		default:
			break;
	}


}


int32_t nb_recv(int32_t id , uint8_t  *buf, uint32_t len)
{
	return nb_recv_timeout(id, buf, len,NULL,NULL, LOS_WAIT_FOREVER);
}

int32_t nb_recvfrom(int32_t id , uint8_t  *buf, uint32_t len,char* ipaddr,int* port)
{
	return nb_recv_timeout(id, buf, len, ipaddr,port,LOS_WAIT_FOREVER);
}

int32_t nb_recv_timeout(int32_t id , uint8_t  *buf, uint32_t len,char* ipaddr,int* port, int32_t timeout)
{
	int rlen = 0;
	int ret;
	QUEUE_BUFF	qbuf;
	UINT32 qlen = sizeof(QUEUE_BUFF);

	if (id  >= MAX_SOCK_NUM)
	{
		AT_LOG("link id %ld invalid", id);
		return AT_FAILED;
	}


	ret = LOS_QueueReadCopy(at.linkid[id].qid, &qbuf, &qlen, timeout);
	//AT_LOG("wwww LOS_QueueReadCopy data,qid %d, len %ld, ret %d", at.linkid[id].qid, qbuf.len, ret);
	if (ret != LOS_OK)
	{
		return AT_TIMEOUT;
	}


	if (('\0' == sockinfo[id].remoteip[0])
		|| (0 == sockinfo[id].remoteport))
	{
		AT_LOG("update ip and port for link %ld", id);
		strncpy(sockinfo[id].remoteip, qbuf.ipaddr, sizeof(sockinfo[id].remoteip));
		sockinfo[id].remoteip[sizeof(sockinfo[id].remoteip) - 1] = '\0';
		sockinfo[id].remoteport = qbuf.port;
	}

	if(ipaddr != NULL)
	{
		memcpy(ipaddr,qbuf.ipaddr,strlen(qbuf.ipaddr));
		*port = qbuf.port;
	}

	rlen = MIN(qbuf.len, len);

	//AT_LOG("recv data, %d", rlen);

	if (rlen){
		memcpy(buf, qbuf.addr, rlen);
		at_free(qbuf.addr);
	}
	return rlen;

}


int32_t nb_close(int32_t id)
{
	int ret;

	if ((id  >= MAX_SOCK_NUM)
		|| (!sockinfo[id].used_flag))
	{
		AT_LOG("link id %ld invalid", id);
		return AT_FAILED;
	}

	nb_close_sock(sockinfo[id].socket);

	do
	{
		QUEUE_BUFF	qbuf = {0};
		UINT32 qlen = sizeof(QUEUE_BUFF);
		ret = LOS_QueueReadCopy(at.linkid[id].qid, &qbuf, &qlen, 0);
		if (ret == LOS_OK && qbuf.addr != NULL)
		{
			at_free(qbuf.addr);
		}
	}while(ret == LOS_OK);
	ret = LOS_QueueDelete(at.linkid[id].qid);
	if (ret != LOS_OK)
	{
		AT_LOG("LOS_QueueDelete failed, ret is %d!,qid %d", ret, at.linkid[id].qid);
	}
	(void)memset(&sockinfo[id], 0, sizeof(sockinfo[id]));

	return AT_OK;
}

int32_t nb_recv_cb(int32_t id)
{
	return AT_FAILED;
}

static int32_t nb_init(void)
{
	at_config at_user_conf = {
		.name = AT_MODU_NAME,
		.usart_port = AT_USART_PORT,
		.buardrate = AT_BUARDRATE,
		.linkid_num = AT_MAX_LINK_NUM,
		.user_buf_len = MAX_AT_USERDATA_LEN,
		.cmd_begin = AT_CMD_BEGIN,
		.line_end = AT_LINE_END,
		.mux_mode = 1, //support multi connection mode
		.timeout = AT_CMD_TIMEOUT,   //  ms
	};
	
	at_set_config(&at_user_conf);
	memset(&sockinfo, 0, sizeof(sockinfo));
	memset(&g_data_ind_info, 0, sizeof(g_data_ind_info));
	at_reg_step_callback(&at, nb_step);

	return AT_OK;
}

int32_t nb_deinit(void)
{

	for (int i = 0; i < MAX_SOCK_NUM; ++i)
	{
		if (sockinfo[i].used_flag)
		{
			nb_close(i);
		}
	}
	return nb_reboot();
}

at_adaptor_api bc95_interface =
{
	.init = nb_init,

	.bind = nb_bind,

	.connect = nb_connect,
	.send = nb_send,
	.sendto = nb_sendto,

	.recv_timeout = nb_recv_timeout,
	.recv = nb_recv,
	.recvfrom = nb_recvfrom,

	.close = nb_close,
	.recv_cb = nb_recv_cb,

	.deinit = nb_deinit,
};

void nb_reattach(void)
{
	(void)nb_cmd_with_2_suffix((int8_t*)CGATT, strlen(CGATT), "OK", "ERROR", NULL, NULL);
	 (void)nb_cmd_with_2_suffix((int8_t*)CGATT_DEATTACH, strlen(CGATT_DEATTACH), "OK", "ERROR", NULL, NULL);
	 LOS_TaskDelay(1000);
	 (void)nb_cmd_with_2_suffix((int8_t*)CGATT_ATTACH, strlen(CGATT_ATTACH), "OK", "ERROR", NULL, NULL);
}

static int nb_cmd_rcv_data(int sockid, int readleft);


static int32_t nb_handle_sock_data(const int8_t *data, uint32_t len)
{
	(void) len;
	char *curr = (char *)data;

	if (strstr((char *) data, "ERROR") != NULL)
	{
		return AT_OK;
	}

	do
	{

		int readleft;
		int sockid;

		char *next = strstr(curr, "\r\n");

		if (next == curr)
		{
			curr += 2;
		}

		if (next != NULL)
		{
			next += 2;
		}

		if (nb_decompose_str(curr, &readleft, &sockid) == AT_OK)
		{
		/*
			if (readleft != 0)
			{
				nb_cmd_rcv_data(sockid, readleft);
			}*/
			return AT_OK;
		}
		curr = next;
	}while(curr);

	return AT_FAILED;
}


static int nb_cmd_rcv_data(int sockid, int readleft)
{
	int cmdlen;
	char cmdbuf[40];
	const char* cmd = "AT+NSORF=";
	const uint32_t timeout = 10;

	cmdlen = snprintf(cmdbuf, sizeof(cmdbuf), "%s%d,%d\r", cmd, sockid, readleft);
	return at_cmd_in_callback((int8_t*)cmdbuf, cmdlen, nb_handle_sock_data, timeout);
}

static int32_t nb_handle_data_ind(const char *buf)
{
	int32_t sockid;
	int32_t data_len;
	const char *p1, *p2;
	int link_idx;

	p2 = strstr(buf, AT_DATAF_PREFIX);
	if (NULL == p2)
	{
		return AT_FAILED;
	}
	p2+=strlen(AT_DATAF_PREFIX);

	p1 = strstr(p2, ",");
	if (p1 == NULL)
	{
		return AT_FAILED;
	}
	sockid = chartoint(p2);
	data_len = chartoint(p1 + 1);
	link_idx = nb_sock_to_idx(sockid);
	if (link_idx >= MAX_SOCK_NUM)
	{
		AT_LOG("invalid sock id %ld", sockid);


		return AT_OK;
	}

	if (nb_cmd_rcv_data(sockid, data_len) != AT_OK)
	{
		g_data_ind_info.data_len = (uint32_t)data_len;
		g_data_ind_info.link_idx = link_idx;
		g_data_ind_info.valid_flag = true;
	}
	else
	{
		g_data_ind_info.valid_flag = false;
	}

	return AT_OK;

}


int32_t nb_cmd_match(const char *buf, char* featurestr,int len)
{
	if (buf == NULL)
	{
		return AT_FAILED;
	}

	nb_handle_data_ind(buf);

	return AT_FAILED;
}

void nb_step(void)
{
	if ((!g_data_ind_info.valid_flag)
		|| (!sockinfo[g_data_ind_info.link_idx].used_flag))
	{
		return;
	}
	if (nb_cmd_rcv_data(sockinfo[g_data_ind_info.link_idx].socket, g_data_ind_info.data_len) == AT_OK)
	{
		g_data_ind_info.valid_flag = false;
	}
}

#endif
