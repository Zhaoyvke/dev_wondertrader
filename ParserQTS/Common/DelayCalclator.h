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
    int iLocalRecvTime; //本地接收到的时间
    int iVssTime;       //我司系统接收到的时间
    int iExchangeTime;  //交易所时间
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
    int m_iWriteCount;         //单个文件的当前记录的条数

    std::vector<CalcPara> m_vctCalclateBuffer;   // 计算队列
    std::vector<CalcPara> m_vctStorageBuffer;    // 缓存队列
    UtilMutex     m_lock;
    std::fstream  m_fstream;
    std::fstream  m_Intervalfstream;

    bool m_stop;
    std::unique_ptr<tthread::thread>	m_DelayThread;  ///< 执行任务线程


};

