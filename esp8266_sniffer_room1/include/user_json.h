/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2016 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef __USER_JSON_H__
#define __USER_JSON_H__

#include "json/jsonparse.h"
#include "json/jsontree.h"

#define jsonSize   2*1024

void json_parse(struct jsontree_context *json, char *ptrJSONMessage);

void json_ws_send(struct jsontree_value *tree, const char *path, char *pbuf);

int json_putchar(int c);

struct jsontree_value *find_json_path(struct jsontree_context *json, const char *path);
//-------------------------------------------
int jsonTree_set(struct jsontree_context *js_ctx, struct jsonparse_state *parser);

//int jsonObject_set(struct jsonparse_state *parser);

int jsonArray_set(struct jsontree_context *js_ctx, struct jsonparse_state *parser);



#endif