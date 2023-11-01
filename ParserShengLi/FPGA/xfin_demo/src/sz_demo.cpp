#include "szbmdreceiver.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


// 深交所行情回调接口
class MsgHandler : public SZBMsgHandler
{
public:
    MsgHandler() : m_msgCnt(0) {}

    // FinMsgHandler interface
public:
    // 普通快照股票(0x0001)(交易所300111)
    virtual void onSZBStockSS(SZBStockSS *msg) override
    {
        ++m_msgCnt;
        printf("    onSZBStockSS: msgCnt: %d, seqNum: %lld, securityID: %s, channelNo: %d\n", m_msgCnt, msg->msgHead.seqNum, msg->securityID, msg->channelNo);
        printf(" buyUnit[0].vol = %d  %d %d %d %d ", msg->buyUnit[0].volume, msg->buyUnit[1].volume, msg->buyUnit[2].volume, msg->buyUnit[3].volume, msg->buyUnit[4].volume);
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
        printf("  onSZBStockTick: msgCnt: %d, seqNum: %lld, securityID: %s, channelNo: %d, orderID: %d, bidSeqNum: %d, offerSeqNum:%d ,execType :%c .\n", m_msgCnt, msg->msgHead.seqNum, msg->securityID, msg->channelNo, msg->orderID, msg->bidSeqNum, msg->offerSeqNum,msg->execType);
    }

    // 股票逐笔委托(0x0005)(交易所300192)
    virtual void onSZBStockOrder(SZBStockOrder *msg) override
    {
        ++m_msgCnt;
        printf(" onSZBStockOrder: msgCnt: %d, seqNum: %lld, securityID: %s, channelNo: %d, orderID: %d orderSide:%c \n", m_msgCnt, msg->msgHead.seqNum, msg->securityID, msg->channelNo, msg->orderID, msg->orderSide);
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
    MsgHandler handler;

#if 0
    SZBRecvParam param;
    param.interface = "eno1";
    param.groupAddr = "239.255.43.21";
    param.groupPort = 45454;
    param.stackType = 1;
    param.recvCpuIndex = 0;
    param.callbackHandler = &handler;
#endif

    SZBMDReceiver receiver;

    if(!receiver.init("../etc/config-sdk.conf", &handler))
    {
        printf("SZBMDReceiver init failed\n");
        return 0;
    }

    if(!receiver.start())
    {
        printf("SZBMDReceiver start failed\n");
        return 0;
    }

    printf("SZBMDReceiver start success\n");

    while (1) {
        sleep(1);
    }

    receiver.stop();

    printf("quit\n");

    return 0;
}


