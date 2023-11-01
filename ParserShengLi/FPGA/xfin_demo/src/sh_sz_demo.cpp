#include "szbmdreceiver.h"
#include "ssemdreceiver.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// 上交所行情回调接口
class SSEHandler : public SSEMsgHandler
{
public:
    SSEHandler() : m_msgCnt(0) {}

    ~SSEHandler()
    {
        printf("~SSEHandler() 1\n");
    }

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
        printf("  onSSETransaction: msgCnt: %d, seqNum: %lld, securityID: %s, tradeChnl: %d, tradingTime: %d\n", m_msgCnt, msg->msgHead.seqNum, msg->securityID, msg->tradeChnl, msg->tradingTime);
    }

    // 指数行情快照(0x0C29)
    virtual void onSSEIndexSS(SSEIndexSS *msg)
    {
        ++m_msgCnt;
        printf("      onSSEIndexSS: msgCnt: %d, seqNum: %lld, securityID: %s, dataTimeStamp: %d\n", m_msgCnt, msg->msgHead.seqNum, msg->securityID, msg->dataTimeStamp);
    }

    // 竞价逐笔委托(0x16A9)
    virtual void onSSEOrder(SSEOrder *msg)
    {
        ++m_msgCnt;
        printf("        onSSEOrder: msgCnt: %d, seqNum: %lld, securityID: %s, channel: %d, orderTime: %d\n", m_msgCnt, msg->msgHead.seqNum, msg->securityID, msg->channel, msg->orderTime);
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

// 深交所行情回调接口
class SZBHandler : public SZBMsgHandler
{
public:
    SZBHandler() : m_msgCnt(0) {}

    ~SZBHandler()
    {
        printf("~SZBHandler() 1\n");
    }

    // FinMsgHandler interface
public:
    // 普通快照股票(0x0001)(交易所300111)
    virtual void onSZBStockSS(SZBStockSS *msg) override
    {
        ++m_msgCnt;
        printf("    onSZBStockSS: msgCnt: %d, seqNum: %lld, securityID: %s, channelNo: %d\n", m_msgCnt, msg->msgHead.seqNum, msg->securityID, msg->channelNo);
    }

    // 合成快照股票(0x0002)(交易所300191、300192)
    virtual void onSZBStockSSProc(SZBStockSSRebuild *msg) override
    {
        ++m_msgCnt;
        printf("onSZBStockSSProc: msgCnt: %d, seqNum: %lld, securityID: %s, channelNo: %d\n", m_msgCnt, msg->msgHead.seqNum, msg->securityID, msg->channelNo);
    }

    // 股票逐笔成交(0x0004)(交易所300191)
    virtual void onSZBStockTick(SZBStockTick *msg) override
    {
        ++m_msgCnt;
        printf("  onSZBStockTick: msgCnt: %d, seqNum: %lld, securityID: %s, channelNo: %d, orderID: %d\n", m_msgCnt, msg->msgHead.seqNum, msg->securityID, msg->channelNo, msg->orderID);
    }

    // 股票逐笔委托(0x0005)(交易所300192)
    virtual void onSZBStockOrder(SZBStockOrder *msg) override
    {
        ++m_msgCnt;
        printf(" onSZBStockOrder: msgCnt: %d, seqNum: %lld, securityID: %s, channelNo: %d, orderID: %d\n", m_msgCnt, msg->msgHead.seqNum, msg->securityID, msg->channelNo, msg->orderID);
    }

    // 指数通用协议(0x0007)(交易所309011)
    virtual void onSZBIndex(SZBIndex *msg) override
    {
        ++m_msgCnt;
        printf("      onSZBIndex: msgCnt: %d, seqNum: %lld, securityID: %s, channelNo: %d\n", m_msgCnt, msg->msgHead.seqNum, msg->securityID, msg->channelNo);
    }

    // 港股通协议(0x0008)(交易所306311)
    virtual void onSZBStockHKSS(SZBStockHKSS *msg) override
    {
        ++m_msgCnt;
        printf("  onSZBStockHKSS: msgCnt: %d, seqNum: %lld, securityID: %s, channelNo: %d\n", m_msgCnt, msg->msgHead.seqNum, msg->securityID, msg->channelNo);
    }

    // 普通快照债券(0x1001)(交易所300111)
    virtual void onSZBBondSS(SZBBondSS *msg) override
    {
        ++m_msgCnt;
        printf("     onSZBBondSS: msgCnt: %d, seqNum: %lld, securityID: %s, channelNo: %d\n", m_msgCnt, msg->msgHead.seqNum, msg->securityID, msg->channelNo);
    }

    // 普通快照基金(0x2001)(交易所300111)
    virtual void onSZBFundSS(SZBFundSS *msg) override
    {
        ++m_msgCnt;
        printf("     onSZBFundSS: msgCnt: %d, seqNum: %lld, securityID: %s, channelNo: %d\n", m_msgCnt, msg->msgHead.seqNum, msg->securityID, msg->channelNo);
    }

private:
    int m_msgCnt;
};




int main(int argc, char *argv[])
{
    SSEHandler sh_handler;
    SZBHandler sz_handler;

#if 0
    SSERecvParam sh_param;
    sh_param.interface = "eno1";
    sh_param.groupAddr = "239.255.43.22";
    sh_param.groupPort = 45455;
    sh_param.stackType = 1;
    sh_param.recvCpuIndex = 1;
    sh_param.callbackHandler = &sh_handler;

    SZBRecvParam sz_param;
    sz_param.interface = "eno1";
    sz_param.groupAddr = "239.255.43.21";
    sz_param.groupPort = 45454;
    sz_param.stackType = 1;
    sz_param.recvCpuIndex = 2;
    sz_param.callbackHandler = &sz_handler;
#endif

    // 上交所行情
    SSEMDReceiver sh_receiver;
    if(!sh_receiver.init("../etc/config-sdk.conf", &sh_handler))
    {
        printf("SSEMDReceiver init failed\n");
        return 0;
    }

    if(!sh_receiver.start())
    {
        printf("SSEMDReceiver start failed\n");
        return 0;
    }

    printf("SSEMDReceiver start success\n");

    // 深交所行情
    SZBMDReceiver sz_receiver;

    if(!sz_receiver.init("../etc/config-sdk.conf", &sz_handler))
    {
        printf("SZBMDReceiver init failed\n");
        return 0;
    }

    if(!sz_receiver.start())
    {
        printf("SZBMDReceiver start failed\n");
        return 0;
    }

    printf("SZBMDReceiver start success\n");

    while (1) {
        sleep(1);
    }

    printf("quit\n");

    return 0;
}


