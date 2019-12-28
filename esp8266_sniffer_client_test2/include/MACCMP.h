#ifndef __MACCMP_H__
#define __MACCMP_H__

#include "driver/uart.h"
#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"

#define MACarraylength 60
#define MAClength      10
#define MAXStatus      570

// 初始化一维数组
void initArray(char* array , int arrayLength); 

/**
 * 将一个成员加入目标数组，并与目标数组的各个成员对比，如果不重复则加入否则丢弃
 * 对比完后重复/新加入的成员的状态恢复初始，其他的成员状态减一
 * insertArray  要加入的数组
 * targetmember 要加入的成员
*/
void add(char insertArray[MACarraylength][MAClength] , char* targetmember);

/**
 *  数组每个非零成员值减一
 *  array       数组
 *  arraylength 数组长度
 */
void reduceProgressively(int* Statusarray , int arrayLength);

/**
 * 恢复某个成员的状态值
 */
void recoverStatus(int* Statusarray , int mamberNum);

/**
 * 获取状态数组内活着的成员数量
 */
uint8 get_memberNum(int* Statusarray , int arrayLength);

// 状态数组  -->   =0 ->死亡   >0  ->正常
extern int Status[MACarraylength];
// MAC数组
extern char MAC[MACarraylength][MAClength];

#endif
