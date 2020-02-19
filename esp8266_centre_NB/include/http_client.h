/*
 * http_client.h
 */

#ifndef _HTTP_CLIENT_H_
#define _HTTP_CLIENT_H_

#include "ets_sys.h"
#include "os_type.h"

extern void http_client_init(u8* ip, u16 port);
extern void http_client_connect(void);

extern struct espconn *server;
void send2server (uint8 *pdata, uint8 len);

#endif /* _HTTP_CLIENT_H_ */
