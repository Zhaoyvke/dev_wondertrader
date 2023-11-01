/*************************************************
�ļ�: UtilMutex.h
����: ʵ���ٽ���������,�̼߳以����

*************************************************/
#pragma once
#include "UtilCommon.h"

#ifdef Windows_Version
#include <atlstr.h>
// �ٽ���������
#define UTIL_MUTEX CRITICAL_SECTION
#endif

/*************************************************
��: UtilMutex
����: �ٽ������࣬�����������̽���ʹ�ã�
     ע������ onCreateMutex
     ������   onLockMutex
     ������   onUnLockMutex
     ע������ onDestroyMutex

     ����ʹ��UtilMutexAgent���мӡ���������
*************************************************/
class UtilMutex
{
public:
    // ������UtilMutex
    // ���ܣ����캯��
    // ���룺
    // �����
    // ����ֵ��
    UtilMutex();

    // ������~UtilMutex
    // ���ܣ���������
    // ���룺
    // �����
    // ����ֵ��
    virtual ~UtilMutex();

    // ������onCreateMutex
    // ���ܣ�ע����
    // ���룺
    // �����
    // ����ֵ��bool 
    bool onCreateMutex();

    // ������onLockMutex
    // ���ܣ�����
    // ���룺
    // �����
    // ����ֵ��bool 
    bool onLockMutex();

    // ������onUnLockMutex
    // ���ܣ�����
    // ���룺
    // �����
    // ����ֵ��bool 
    bool onUnLockMutex();

    // ������onDestroyMutex
    // ���ܣ�ע����
    // ���룺
    // �����
    // ����ֵ��bool 
    bool onDestroyMutex();

protected:
    UtilMutex(const UtilMutex&);

private:
    UTIL_MUTEX           m_Mutex;
    bool                 m_bCreate;
};

/*************************************************
��: UtilMutexAgent
����: �������࣬�������м����ͽ�������
      ֻ֧�ֶ�������˳�������������������֧�ֲ��в�����
      ������Ϊ�ֲ�֮��ļӡ���������

*************************************************/
class  UtilMutexAgent
{
public:
    // ������UtilMutexAgent
    // ���ܣ����캯��
    // ���룺rMutex �������� 
    // �����
    // ����ֵ�� 
    UtilMutexAgent(UtilMutex& rMutex);

    // ������~UtilMutexAgen
    // ���ܣ���������
    // ���룺
    // �����
    // ����ֵ�� 
    ~UtilMutexAgent();

    // ������onLockMutex
    // ���ܣ�����
    // ���룺
    // �����
    // ����ֵ��bool 
    bool onLockMutex();

    // ������onUnLockMutex
    // ���ܣ�����
    // ���룺
    // �����
    // ����ֵ��bool 
    bool onUnLockMutex();

protected:
    UtilMutexAgent(const UtilMutexAgent&);

private:
    UtilMutex&        m_rMutex;       // ������
    bool            m_bLock;        // �Ƿ��Ѿ������ı�־
};

