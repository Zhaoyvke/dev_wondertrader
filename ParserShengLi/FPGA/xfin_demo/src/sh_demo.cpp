#include "ssemdreceiver.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


// 上交所行情回调接口
class MsgHandler : public SSEMsgHandler
{
public:
    MsgHandler() : m_msgCnt(0) {}

    // FinMsgHandler interface
public:
    // 竞价行情快照(0x0C82)
    virtual void onSSEMarketSS(SSEMarketSS *msg)
    {
        ++m_msgCnt;
        printf("     onSSEMarketSS: msgCnt: %d, seqNum: %lld, securityID: %s, dataTimeStamp: %d\n", m_msgCnt, msg->msgHead.seqNum, msg->securityID, msg->dataTimeStamp);
    }

    // 竞价逐笔成交(0x0C81)
    virtual void onSSETransaction(SSETransaction *msg)
    {
        ++m_msgCnt;
        printf("  onSSETransaction: msgCnt: %d, seqNum: %lld, securityID: %s, tradeChnl: %d, tradingTime: %d , tradeIdx: %d , bidSeqNum :%ld , ofrSeqNum: %ld  tradeChnl:  %d .\n", m_msgCnt, msg->msgHead.seqNum, msg->securityID, msg->tradeChnl, msg->tradingTime, msg->tradeIdx, msg->bidSeqNum, msg->ofrSeqNum, msg->tradeChnl);
    }

    // 指数行情快照(0x0C29)
    virtual void onSSEIndexSS(SSEIndexSS *msg)
    {
        ++m_msgCnt;
        printf("      onSSEIndexSS: msgCnt: %d, seqNum: %lld, securityID: %s, dataTimeStamp: %d\n", m_msgCnt, msg->msgHead.seqNum, msg->securityID, msg->dataTimeStamp);
    }

    // 竞价逐笔委托 (0x16A9)
    virtual void onSSEOrder(SSEOrder *msg)
    {
        ++m_msgCnt;
        printf("        onSSEOrder: msgCnt: %d, seqNum: %lld, securityID: %s, channel: %d, orderTime: %d, channel: %d .\n", m_msgCnt, msg->msgHead.seqNum, msg->securityID, msg->channel, msg->orderTime, msg->channel);
    }

    // 债券行情快照数据(0x0EDA)
    virtual void onSSEXtsMarketData(SSEXtsMarketData *msg)
    {
        ++m_msgCnt;
        printf("onSSEXtsMarketData: msgCnt: %d, seqNum: %lld, securityID: %s, dataTimeStamp: %d\n", m_msgCnt, msg->msgHead.seqNum, msg->securityID, msg->dataTimeStamp);
    }

    // 债券逐笔委托及成交数据(0x0F3D)
    virtual void onSSEXtsTick(SSEXtsTick *msg)
    {
        ++m_msgCnt;
        printf("      onSSEXtsTick: msgCnt: %d, seqNum: %lld, securityID: %s, channel: %d, tickTime: %d\n", m_msgCnt, msg->msgHead.seqNum, msg->securityID, msg->channel, msg->tickTime);
    }

private:
    int m_msgCnt;
};



int main(int argc, char *argv[])
{
    MsgHandler handler;

#if 0
    SSERecvParam param;
    param.interface = "eno1";
    param.groupAddr = "239.255.43.21";
    param.groupPort = 45454;
    param.stackType = 1;
    param.recvCpuIndex = 0;
    param.callbackHandler = &handler;
#endif

    SSEMDReceiver receiver;

    if(!receiver.init("../etc/config-sdk.conf", &handler))
    {
        printf("SSEMDReceiver init failed\n");
        return 0;
    }

    if(!receiver.start())
    {
        printf("SSEMDReceiver start failed\n");
        return 0;
    }

    printf("SSEMDReceiver start success\n");

    while (1) {
        sleep(1);
    }


    printf("quit\n");

    return 0;
}


