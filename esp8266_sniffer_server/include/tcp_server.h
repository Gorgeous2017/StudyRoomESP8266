/*
 * tcp_server.h
 *
 *  Created on: 2017��7��3��
 *      Author: Administrator
 */

#ifndef _TCP_SERVER_H_
#define _TCP_SERVER_H_

#include "c_types.h"

/* TCP Server ��ʼ�� */
extern void tcp_server_init(u32 port);
void send2client(uint8 clientID, char *pdata);

#endif /* _TCP_SERVER_H_ */

