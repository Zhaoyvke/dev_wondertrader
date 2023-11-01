#pragma once
#include <memory>
#include <vector>
#include <fstream>
#include "../Common/public.h"
#include "UtilMutex.h"
#include "../Common/TinyThread.h"
#include <functional>

struct CalcPara
{
    int iLocalRecvTime; //���ؽ��յ���ʱ��
    int iVssTime;       //��˾ϵͳ���յ���ʱ��
    int iExchangeTime;  //������ʱ��
};

class DelayCalclator
{
public:
    static DelayCalclator &Instance();

    virtual ~DelayCalclator(void);    

    void Push(CalcPara &calcPara);

    void IntervalCalclateDelayTime(int& iCount, int& iMaxDelay, int& iMinDelay, int& iAvgDelay);

private:
    DelayCalclator(void);

    static void DelayThreadProc(void* arg);
    void DelayProcess(void);


private:
    int m_iWriteCount;         //�����ļ��ĵ�ǰ��¼������

    std::vector<CalcPara> m_vctCalclateBuffer;   // �������
    std::vector<CalcPara> m_vctStorageBuffer;    // �������
    UtilMutex     m_lock;
    std::fstream  m_fstream;
    std::fstream  m_Intervalfstream;

    bool m_stop;
    std::unique_ptr<tthread::thread>	m_DelayThread;  ///< ִ�������߳�


};

