#include "DelayCalclator.h"
#include <time.h>
#include "public.h"

#define  DATA_MAX_COUNT      500000

///UTILTime转成当天毫秒数
inline int UTILTimeToMilliSecond(UTIL_Time &stTime)
{
    return stTime.iHour * 3600000 + stTime.iMin * 60000 + stTime.iSec * 1000 + stTime.iMillisec;
}

///HHMMSSMMM格式时间转成当天毫秒数
inline int IntegerTimeToMilliSecond(int t)
{
    return t / 10000000 * 3600000 + t % 10000000 / 100000 * 60000 + t % 100000 / 1000 * 1000 + t % 1000;
}

DelayCalclator &DelayCalclator::Instance()
{
    static DelayCalclator calc;
    return calc;
}

DelayCalclator::DelayCalclator(void)
    :m_lock()
    ,m_stop(false)
{
    m_lock.onCreateMutex();
    m_iWriteCount = 0;
    m_DelayThread.reset(new tthread::thread(&DelayThreadProc, this));
}

DelayCalclator::~DelayCalclator(void)
{
    m_lock.onDestroyMutex();

    m_stop = true;
    if ( m_DelayThread.get() == 0 )
    {
        return ;
    }
    m_DelayThread->join();    

    if (m_fstream.is_open())
    {
        m_fstream.close();
        m_fstream.clear();
    }

    if (m_Intervalfstream.is_open())
    {
        m_Intervalfstream.close();
        m_Intervalfstream.clear();
    }
}

void DelayCalclator::DelayThreadProc(void* arg)
{
    DelayCalclator* pDelayCalclator = (DelayCalclator*)arg;
    pDelayCalclator->DelayProcess();
}

void DelayCalclator::DelayProcess(void)
{
    int iCount = 0;
    int iMaxDelay = 0;
    int iMinDelay = 0;
    int iAvgDelay = 0;

    time_t stPrevTime;
    time(&stPrevTime);
    while (!m_stop)
    {
        time_t stCurrTime;
        time(&stCurrTime);
        if (stCurrTime-stPrevTime >= 5)
        {//间隔5秒统计一下
            IntervalCalclateDelayTime(iCount, iMaxDelay, iMinDelay, iAvgDelay);
            stPrevTime = stCurrTime;
        }
        else
        {
            if (stCurrTime-stPrevTime<0)
            {
                stPrevTime = stCurrTime;
            }
        }
        Sleep(100);
    }
}

void DelayCalclator::Push(CalcPara &calcPara)
{
    m_lock.onLockMutex();
    m_vctStorageBuffer.push_back(calcPara);
    m_lock.onUnLockMutex();
}




void DelayCalclator::IntervalCalclateDelayTime(int  &iCount, int  &iMaxDelay, int  &iMinDelay, int  &iAvgDelay)
{
    m_lock.onLockMutex();
    m_vctStorageBuffer.swap(m_vctCalclateBuffer);
    m_lock.onUnLockMutex();        
    
    int delay = 0;
    int dataCount = 0;
    int maxDelay = -1;
    int minDelay = -1;
    long long totalDelay = 0;

    for (auto it = m_vctCalclateBuffer.begin(); it != m_vctCalclateBuffer.end(); ++it)
    {
        //使用本地接收到的时间 减去 交易所的数据生成时间
        delay = IntegerTimeToMilliSecond(it->iLocalRecvTime) - IntegerTimeToMilliSecond(it->iExchangeTime);

        if (minDelay < 0 || delay < minDelay)
        {//计算最小值
            minDelay = delay;
        }
        if (maxDelay < 0 || delay > maxDelay)
        {//计算最大值
            maxDelay = delay;
        }

        totalDelay += delay;
        dataCount++;

        if (m_iWriteCount >= DATA_MAX_COUNT || m_iWriteCount == 0)
        {
            m_iWriteCount = 0;
            if (m_fstream.is_open())
            {
                m_fstream.close();
                m_fstream.clear();
            }
            char szFileName[MAX_PATH] = {0};
            UTIL_Time stTime;
            PUTIL_GetLocalTime(&stTime);

            char szFilePath[MAX_PATH + 1]={0};
            GetModulePath(szFilePath, sizeof(szFilePath));
            sprintf(szFileName ,"%s/%4d%02d%02d_%02d%02d%02d.csv", szFilePath, stTime.iYear, stTime.iMon, stTime.iDay, stTime.iHour, stTime.iMin, stTime.iSec);
            m_fstream.open(szFileName,std::ios::out|std::ios::app);           
        }

        if (m_fstream.is_open())
        {
            char szOutPut[1024] = {0};
            sprintf(szOutPut,"LocalRecvTime,%09d, VssTime, %09d, ExchangeTime, %09d, DelayTime, %d\n", it->iLocalRecvTime, it->iVssTime, it->iExchangeTime, delay);
            int nSize = (int)strlen(szOutPut);
            m_fstream.write(szOutPut, nSize);
            m_iWriteCount++;            
        }
    }
    m_vctCalclateBuffer.clear();
    if (m_fstream.is_open())
    {
        m_fstream.flush();
    }

    iCount      = dataCount;
    iMaxDelay   = maxDelay;
    iMinDelay   = minDelay;

    if (dataCount > 0)
    {
        iAvgDelay = (int)(totalDelay / dataCount);
    }
    else
    {
        iAvgDelay = -1;
    }

    if (!m_Intervalfstream.is_open())
    {
        char szResultFileName[MAX_PATH] = {0};
        UTIL_Time stTime;
        PUTIL_GetLocalTime(&stTime);
        char szFilePath[MAX_PATH + 1]={0};
        GetModulePath(szFilePath, sizeof(szFilePath));
        sprintf_s(szResultFileName ,MAX_PATH, "%s/Interval_%4d%02d%02d_%02d%02d%02d.csv", szFilePath, stTime.iYear, stTime.iMon, stTime.iDay, stTime.iHour, stTime.iMin, stTime.iSec);
        m_Intervalfstream.open(szResultFileName,std::ios::out|std::ios::app);
    }
    if (m_Intervalfstream.is_open())
    {
        char szResultOutPut[1024] = {0};
        sprintf_s(szResultOutPut,1024,"Count,%d,Avg,%lld,Min,%d,Max,%d\n", dataCount, totalDelay / (dataCount==0?1:dataCount), minDelay, maxDelay);
        int nResultSize = (int)strlen(szResultOutPut);
        m_Intervalfstream.write(szResultOutPut, nResultSize);
        m_Intervalfstream.flush();
    }
}

