/*************************************************
文件: UtilMutex.h
功能: 实现临界区锁操作,线程间互斥锁

*************************************************/
#pragma once
#include "UtilCommon.h"

#ifdef Windows_Version
#include <atlstr.h>
// 临界区互斥锁
#define UTIL_MUTEX CRITICAL_SECTION
#endif

/*************************************************
类: UtilMutex
功能: 临界区锁类，根据下面流程进行使用：
     注册锁： onCreateMutex
     加锁：   onLockMutex
     解锁：   onUnLockMutex
     注销锁： onDestroyMutex

     建议使用UtilMutexAgent进行加、解锁操作
*************************************************/
class UtilMutex
{
public:
    // 函数：UtilMutex
    // 功能：构造函数
    // 输入：
    // 输出：
    // 返回值：
    UtilMutex();

    // 函数：~UtilMutex
    // 功能：析构函数
    // 输入：
    // 输出：
    // 返回值：
    virtual ~UtilMutex();

    // 函数：onCreateMutex
    // 功能：注册锁
    // 输入：
    // 输出：
    // 返回值：bool 
    bool onCreateMutex();

    // 函数：onLockMutex
    // 功能：加锁
    // 输入：
    // 输出：
    // 返回值：bool 
    bool onLockMutex();

    // 函数：onUnLockMutex
    // 功能：解锁
    // 输入：
    // 输出：
    // 返回值：bool 
    bool onUnLockMutex();

    // 函数：onDestroyMutex
    // 功能：注销锁
    // 输入：
    // 输出：
    // 返回值：bool 
    bool onDestroyMutex();

protected:
    UtilMutex(const UtilMutex&);

private:
    UTIL_MUTEX           m_Mutex;
    bool                 m_bCreate;
};

/*************************************************
类: UtilMutexAgent
功能: 锁代理类，对锁进行加锁和解锁操作
      只支持对锁进行顺序加锁、解锁操作，不支持并行操作，
      建议作为局部之间的加、解锁代理

*************************************************/
class  UtilMutexAgent
{
public:
    // 函数：UtilMutexAgent
    // 功能：构造函数
    // 输入：rMutex 锁的引用 
    // 输出：
    // 返回值： 
    UtilMutexAgent(UtilMutex& rMutex);

    // 函数：~UtilMutexAgen
    // 功能：析构函数
    // 输入：
    // 输出：
    // 返回值： 
    ~UtilMutexAgent();

    // 函数：onLockMutex
    // 功能：加锁
    // 输入：
    // 输出：
    // 返回值：bool 
    bool onLockMutex();

    // 函数：onUnLockMutex
    // 功能：解锁
    // 输入：
    // 输出：
    // 返回值：bool 
    bool onUnLockMutex();

protected:
    UtilMutexAgent(const UtilMutexAgent&);

private:
    UtilMutex&        m_rMutex;       // 锁引用
    bool            m_bLock;        // 是否已经加锁的标志
};

