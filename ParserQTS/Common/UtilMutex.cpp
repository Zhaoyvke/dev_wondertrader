/*************************************************
文件: UtilMutex.cpp
功能: 实现临界区锁操作

*************************************************/
#include "UtilMutex.h"
/*************************************************
函数: UtilMutex::UtilMutex
功能:  构造函数
*************************************************/
UtilMutex::UtilMutex()
{
    m_bCreate = false;
}

/*************************************************
函数: UtilMutex::~UtilMutex
功能:  析构函数,释放锁资源
输入: 
输出: 
返回值:
*************************************************/
UtilMutex::~UtilMutex()
{
    onDestroyMutex();
}

/*************************************************
函数: UtilMutex::onCreateMutex
功能:  注册锁资源
输入: 
输出: 
返回值:bool
*************************************************/
bool
UtilMutex::onCreateMutex()
{
    if (false != m_bCreate)
    {
        return false;
    }

#ifdef Windows_Version
    ::InitializeCriticalSection(&m_Mutex);
#else
    int iRet = pthread_mutex_init(&m_Mutex, NULL);
    if (0 != iRet)
    {
        return false;
    }
#endif

    m_bCreate = true;
    return true;
}

/*************************************************
函数: UtilMutex::onLockMutex
功能:  加锁
输入: 
输出: 
返回值:bool
*************************************************/
bool UtilMutex::onLockMutex()
{
    if (false == m_bCreate)
    {
        return false;
    }

#ifdef Windows_Version
    ::EnterCriticalSection(&m_Mutex);
#else
    int iRet = pthread_mutex_lock(&m_Mutex);
    if (0 != iRet)
    {
        return false;
    }
#endif
    
    return true;
}

/*************************************************
函数: UtilMutex::onUnLockMutex
功能:  解锁
输入: 
输出: 
返回值:bool
*************************************************/
bool UtilMutex::onUnLockMutex()
{
    if (false == m_bCreate)
    {
        return false;
    }

#ifdef Windows_Version
    ::LeaveCriticalSection(&m_Mutex);
#else
    int iRet = pthread_mutex_unlock(&m_Mutex);
    if (0 != iRet)
    {
        return false;
    }
#endif
    
    return true;
}

/*************************************************
函数: UtilMutex::onDestroyMutex
功能:  注销锁资源
输入: 
输出: 
返回值:bool
*************************************************/
bool UtilMutex::onDestroyMutex()
{
    if (false == m_bCreate)
    {
        return true;
    }

#ifdef Windows_Version
    ::DeleteCriticalSection(&m_Mutex);
#else
    int iRet = pthread_mutex_destroy(&m_Mutex);
    if (0 != iRet)
    {
        return false;
    }
#endif
    m_bCreate = false;
    return true;
}

/*************************************************
函数: UtilMutexAgent::UtilMutexAgent
功能:  构造函数
输入: 
输出: 
返回值:
*************************************************/
UtilMutexAgent::UtilMutexAgent(UtilMutex& rMutex):m_rMutex(rMutex)
{
    m_bLock = false;
}

/*************************************************
函数: UtilMutexAgent::UtilMutexAgent
功能:  析构函数
输入: 
输出: 
返回值:
*************************************************/
UtilMutexAgent::~UtilMutexAgent()
{
    onUnLockMutex();
}

/*************************************************
函数: UtilMutexAgent::onLockMutex
功能:  加锁操作，并记录已加锁标志,返回操作结果
输入: 
输出: 
返回值:bool
*************************************************/
bool UtilMutexAgent::onLockMutex()
{
    if (false != m_bLock)
    {
        return false;
    }

    bool bRet = m_rMutex.onLockMutex();
    if (false != bRet)
    {
        m_bLock = true;
    }
    return bRet;
}

/*************************************************
函数: UtilMutexAgent::onUnLockMutex
功能:  解锁操作，并清除已加锁标志,返回操作结果
输入: 
输出: 
返回值:bool
*************************************************/
bool 
UtilMutexAgent::onUnLockMutex()
{
    if (false == m_bLock)
    {
        return true;
    }

    bool bRet = m_rMutex.onUnLockMutex();
    if (false != bRet)
    {
        m_bLock = false;
    }
    return bRet;
}


