/*************************************************
�ļ�: UtilMutex.cpp
����: ʵ���ٽ���������

*************************************************/
#include "UtilMutex.h"
/*************************************************
����: UtilMutex::UtilMutex
����:  ���캯��
*************************************************/
UtilMutex::UtilMutex()
{
    m_bCreate = false;
}

/*************************************************
����: UtilMutex::~UtilMutex
����:  ��������,�ͷ�����Դ
����: 
���: 
����ֵ:
*************************************************/
UtilMutex::~UtilMutex()
{
    onDestroyMutex();
}

/*************************************************
����: UtilMutex::onCreateMutex
����:  ע������Դ
����: 
���: 
����ֵ:bool
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
����: UtilMutex::onLockMutex
����:  ����
����: 
���: 
����ֵ:bool
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
����: UtilMutex::onUnLockMutex
����:  ����
����: 
���: 
����ֵ:bool
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
����: UtilMutex::onDestroyMutex
����:  ע������Դ
����: 
���: 
����ֵ:bool
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
����: UtilMutexAgent::UtilMutexAgent
����:  ���캯��
����: 
���: 
����ֵ:
*************************************************/
UtilMutexAgent::UtilMutexAgent(UtilMutex& rMutex):m_rMutex(rMutex)
{
    m_bLock = false;
}

/*************************************************
����: UtilMutexAgent::UtilMutexAgent
����:  ��������
����: 
���: 
����ֵ:
*************************************************/
UtilMutexAgent::~UtilMutexAgent()
{
    onUnLockMutex();
}

/*************************************************
����: UtilMutexAgent::onLockMutex
����:  ��������������¼�Ѽ�����־,���ز������
����: 
���: 
����ֵ:bool
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
����: UtilMutexAgent::onUnLockMutex
����:  ����������������Ѽ�����־,���ز������
����: 
���: 
����ֵ:bool
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


