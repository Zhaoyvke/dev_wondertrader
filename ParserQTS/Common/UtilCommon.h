/*************************************************
文件: UtilCommon.h
功能: util 公用类型定义

*************************************************/
#pragma once

#ifdef _WIN32  //Windows
#define Windows_Version
#endif

#define EOS '\0' // 字符串结束符

#define FILE_MAX_PATH  260 // 文件路径最大长度

#ifdef Windows_Version  //Windows

#else //Linux

#include <pthread.h> 

#include <sys/types.h> 
#include <sys/stat.h> 
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <semaphore.h>

#include <netinet/in.h>
#include <arpa/inet.h> 

#define MAX_PATH  256

// 临界区互斥锁
#define UTIL_MUTEX pthread_mutex_t

#define sprintf_s(sBuf, nLen, format, ...) snprintf(sBuf, nLen, format, ##__VA_ARGS__)
#define _vsnprintf_s(sBuf, nLen, format, arglist) vsnprintf(sBuf, nLen, format, arglist)

#ifndef _ASSERT
#define _ASSERT(p) {if (0 == (p)){ printf("File: %s Line: %d Error!\n", __FILE__, __LINE__);}}
#endif

#define Sleep(time) usleep((time) * 1000)

#define InterlockedIncrement(pDest) __sync_fetch_and_add((pDest), 1)
#define InterlockedDecrement(pDest) __sync_fetch_and_sub((pDest), 1)
#define InterlockedExchange(pDest, newValue) __sync_lock_test_and_set((pDest), (newValue))
#define InterlockedCompareExchange(pDest, newValue, oldValue) __sync_val_compare_and_swap((pDest), (oldValue), (newValue))

#endif

//typedef struct
//{
//    int iSec;     /* seconds after the minute - [0,59] */
//    int iMin;     /* minutes after the hour - [0,59] */
//    int iHour;    /* hours since midnight - [0,23] */
//
//    int iDay;    /* day of the month - [1,31] */
//    int iMon;     /* months since January - [1,12] */
//    int iYear;    /* year*/
//
//    int iWday;    /* days since Sunday - [0,6] */
//
//    int iMillisec;  /* Milliseconds after the second - [0,999] */
//}UTIL_Time, *UTIL_TimePtr;

#ifndef __cplusplus // C 语言环境没有bool
#define bool int
#define false 0
#define true 1
#endif