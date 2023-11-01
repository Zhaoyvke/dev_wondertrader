#ifndef SZBDATADEF_H
#define SZBDATADEF_H

#include <stdint.h>
#include <string>

#define SZB_LEN_SECURITY_ID         12  // securityID char len
#define SZB_LEN_SECURITY_ID_SRC     4   // securityIDSource char len
#define SZB_LEN_PHASE_CODE          4   // tradingPhaseCode char len

#ifdef __linux__
  #define PACKED __attribute__((packed))
#else
  #define PACKED
  #pragma pack(push,1)
#endif

// 消息类型定义
enum SZBMDMsgType
{
    MT_SZBStockSS               = 0x0001, // 股票普通快照
    MT_SZBStockSSRebuild        = 0x0002, // 股票合成快照
    MT_SZBStockTick             = 0x0004, // 逐笔成交
    MT_SZBStockOrder            = 0x0005, // 逐笔委托
    MT_SZBIndex                 = 0x0007, // 指数快照
    MT_SZBStockHKSS             = 0x0008, // 港股通快照
    MT_SZBBondSS                = 0x1001, // 债券快照
    MT_SZBFundSS                = 0x2001  // 基金快照
};

// 通用头
typedef struct SZBHead
{
    uint16_t    msgType;    // 消息类型
    uint16_t    msgBodyLen; // 包数据大小(不包含通用头和通用尾)
    uint64_t    seqNum;     // 序号，有分包则SeqNum不变，否则递增
    uint16_t    subSeq;     // 子序号，最高位1：有后续分包，最高位0：无后续分包
    char        reserve[2]; // 预留位
}PACKED SZBHead_t;

// 通用尾
typedef struct SZBTail
{
    uint32_t    checkSum;   // 校验码
}PACKED SZBTail_t;

// 价量对
typedef struct SZBPriceUnit
{
    int32_t     price;      // 价格(元)*10000，无效则填充全0
    int32_t     volume;     // 数量，无效则填充全0
}PACKED SZBPriceUnit_t;


// 普通快照股票(0x0001)(交易所300111)
#define SZB_CNT_STOCKSS_QUEUE 10
typedef struct SZBStockSS
{
    SZBHead_t       msgHead;                // msgType=0x0001, msgBodyLen=274

    char            securityID[SZB_LEN_SECURITY_ID];    // 证券代码 + ".SZ", 多余字节补'\0'
    int64_t         origTime;               // 交易时间，YYYYMMDDHHMMSSsss

    /**
     * @brief tradingPhaseCode
     * 产品所处的交易阶段代码, 多余字节补'\0'
     * 第0字节：
     * 'S'=启动（开市前）
     * 'O'=开盘集合竞价
     * 'T'=连续竞价
     * 'B'=休市
     * 'C'=收盘集合竞价
     * 'E'=已闭市
     * 'H'=临时停牌
     * 'A'=盘后交易
     * 'V'=波动性中断
     * 第1字节：
     * '0'=正常状态
     * '1'=全天停牌
     */
    char            tradingPhaseCode[SZB_LEN_PHASE_CODE];

    /**
     * @brief securityIDSource
     * 证券代码源, 多余字节补'\0'
     * "102" = 深圳证券交易所
     * "103" = 香港交易所
     */
    char            securityIDSource[SZB_LEN_SECURITY_ID_SRC];  // 证券代码源
    uint16_t        channelNo;              // 频道
    int32_t         seqID;                  // 无效值，填充0
    int32_t         preClosePx;             // 昨收盘*10000
    int32_t         numTrades;              // 成交笔数
    int64_t         totalVolumeTrade;       // 成交总量*100
    int64_t         totalValueTrade;        // 成交额(元)*10000
    int32_t         openPx;                 // 开盘价(元)*10000，无效则填充全0
    int32_t         highPx;                 // 最高价(元)*10000，无效则填充全0
    int32_t         lowPx;                  // 最低价(元)*10000，无效则填充全0
    int32_t         lastPx;                 // 最近价(元)*10000，无效则填充全0
    int32_t         upperLimit;             // 涨停价(元)*10000，无效则填充全0
    int32_t         lowerLimit;             // 跌停价(元)*10000，无效则填充全0
    int32_t         diffPx1;                // 升跌1*10000，无效则填充全0
    int32_t         diffPx2;                // 升跌2*10000，无效则填充全0
    int64_t         totalBuyVol;            // 总买量*100，无效则填充全0
    int64_t         totalSellVol;           // 总卖量*100，无效则填充全0
    int32_t         avgBuyPx;               // 平均委买价(元)*10000，无效则填充全0
    int32_t         avgSellPx;              // 平均委卖价(元)*10000，无效则填充全0

    SZBPriceUnit_t  buyUnit[SZB_CNT_STOCKSS_QUEUE]; // 买价(元)*10000/买量
    SZBPriceUnit_t  selUnit[SZB_CNT_STOCKSS_QUEUE]; // 卖价(元)*10000/卖量

    SZBTail_t       msgTail;
}PACKED SZBStockSS_t;


// 合成快照股票(0x0002)(交易所300191、300192)
#define SZB_CNT_STOCKSS_PROC_QUEUE 10
typedef struct SZBStockSSRebuild
{
    SZBHead_t       msgHead;                // msgType=0x0002, msgBodyLen=274

    char            securityID[SZB_LEN_SECURITY_ID];    // 证券代码 + ".SZ", 多余字节补'\0'
    int64_t         origTime;               // 交易时间，YYYYMMDDHHMMSSsss

    /**
     * @brief tradingPhaseCode
     * 产品所处的交易阶段代码, 多余字节补'\0'
     * 第0字节：
     * 'S'=启动（开市前）
     * 'O'=开盘集合竞价
     * 'T'=连续竞价
     * 'B'=休市
     * 'C'=收盘集合竞价
     * 'E'=已闭市
     * 'H'=临时停牌
     * 'A'=盘后交易
     * 'V'=波动性中断
     * 第1字节：
     * '0'=正常状态
     * '1'=全天停牌
     */
    char            tradingPhaseCode[SZB_LEN_PHASE_CODE];
    
    /**
     * @brief securityIDSource
     * 证券代码源, 多余字节补'\0'
     * "102" = 深圳证券交易所
     * "103" = 香港交易所
     */
    char            securityIDSource[SZB_LEN_SECURITY_ID_SRC];
    
    uint16_t        channelNo;              // 频道
    int32_t         seqID;                  //
    int32_t         preClosePx;             // 昨收盘*10000，无效则填充全0
    int32_t         numTrades;              // 成交笔数
    int64_t         totalVolumeTrade;       // 成交总量*100
    int64_t         totalValueTrade;        // 成交额(元)*10000
    int32_t         openPx;                 // 开盘价(元)*10000，无效则填充全0
    int32_t         highPx;                 // 最高价(元)*10000，无效则填充全0
    int32_t         lowPx;                  // 最低价(元)*10000，无效则填充全0
    int32_t         lastPx;                 // 最近价(元)*10000，无效则填充全0
    int32_t         upperLimit;             // 涨停价(元)*10000，无效则填充全0
    int32_t         lowerLimit;             // 跌停价(元)*10000，无效则填充全0
    int32_t         diffPx1;                // 升跌1*10000，无效则填充全0
    int32_t         diffPx2;                // 升跌2*10000，无效则填充全0
    int64_t         totalBuyVol;            // 总买量*100
    int64_t         totalSelVol;            // 总卖量*100
    int32_t         avgBuyPx;               // 平均委买价(元)*10000，无效则填充全0
    int32_t         avgSellPx;              // 平均委卖价(元)*10000，无效则填充全0

    SZBPriceUnit_t  buyUnit[SZB_CNT_STOCKSS_PROC_QUEUE];    // 买价(元)*10000/买量
    SZBPriceUnit_t  selUnit[SZB_CNT_STOCKSS_PROC_QUEUE];    // 卖价(元)*10000/卖量

    SZBTail_t       msgTail;
}PACKED SZBStockSSProc_t;


// 逐笔成交(0x0004)(交易所300191)
typedef struct SZBStockTick
{
    SZBHead_t       msgHead;                // msgType=0x0004, msgBodyLen=47

    char            securityID[SZB_LEN_SECURITY_ID];    // 证券代码 + ".SZ", 多余字节补'\0'

    /**
     * @brief securityIDSource
     * 证券代码源, 多余字节补'\0'
     * "102" = 深圳证券交易所
     * "103" = 香港交易所
     */
    char            securityIDSource[SZB_LEN_SECURITY_ID_SRC];

    uint16_t        channelNo;              // 频道
    int32_t         orderID;                // 委托号
    int32_t         bidSeqNum;              // 买委托索引
    int32_t         offerSeqNum;            // 卖委托索引
    int32_t         lastPx;                 // 成交价格*10000
    int32_t         lastQty;                // 成交数量*100

    /**
     * @brief execType
     * 成交类别
     * '4'=撤销
     * 'F'=成交
     */
    char            execType;

    int64_t         transactTime;           // 交易时间，YYYYMMDDHHMMSSsss

    SZBTail_t       msgTail;
}PACKED SZBStockTick_t;


// 逐笔委托(0x0005)(交易所300192)
typedef struct SZBStockOrder
{
    SZBHead_t       msgHead;                // msgType=0x0005, msgBodyLen=40

    char            securityID[SZB_LEN_SECURITY_ID];    // 证券代码 + ".SZ", 多余字节补'\0'

    /**
     * @brief securityIDSource
     * 证券代码源, 多余字节补'\0'
     * "102" = 深圳证券交易所
     * "103" = 香港交易所
     */
    char            securityIDSource[SZB_LEN_SECURITY_ID_SRC];
    
    uint16_t        channelNo;              // 频道
    int32_t         orderID;                // 委托号
    uint32_t        orderPx;                // 委托价格(元)*10000
    int32_t         orderVol;               // 委托数量*100

    /**
     * @brief orderSide
     * 买卖方向
     * '1'=买
     * '2'=卖
     * 'G'=借入
     * 'F'=出借
     */
    char            orderSide;

    int64_t         transactTime;           // 委托时间，YYYYMMDDHHMMSSsss

    /**
     * @brief orderType
     * 订单类别
     * '1'=市价
     * '2'=限价
     * 'U'=本方最优
     */
    char            orderType;

    SZBTail_t       msgTail;
}PACKED SZBStockOrder_t;


// 指数快照(0x0007)(交易所309011)
typedef struct SZBIndex
{
    SZBHead_t       msgHead;                // msgType=0x0007, msgBodyLen=82

    char            securityID[SZB_LEN_SECURITY_ID];    // 证券代码 + ".SZ", 多余字节补'\0'
    int64_t         origTime;               // 交易时间，YYYYMMDDHHMMSSsss

    /**
     * @brief tradingPhaseCode
     * 产品所处的交易阶段代码, 多余字节补'\0'
     * 第0字节：
     * 'S'=启动（开市前）
     * 'O'=开盘集合竞价
     * 'T'=连续竞价
     * 'B'=休市
     * 'C'=收盘集合竞价
     * 'E'=已闭市
     * 'H'=临时停牌
     * 'A'=盘后交易
     * 'V'=波动性中断
     * 第1字节：
     * '0'=正常状态
     * '1'=全天停牌
     */
    char            tradingPhaseCode[SZB_LEN_PHASE_CODE];
    
    /**
     * @brief securityIDSource
     * 证券代码源, 多余字节补'\0'
     * "102" = 深圳证券交易所
     * "103" = 香港交易所
     */
    char            securityIDSource[SZB_LEN_SECURITY_ID_SRC];

    uint16_t        channelNo;              // 频道
    int32_t         preClosePx;             // 昨收指数*10000，无效则填充全0
    int32_t         numTrades;              // 成交笔数
    int64_t         totalVolumeTrade;       // 成交总量*100
    int64_t         totalValueTrade;        // 成交额*10000
    int32_t         openPx;                 // 开盘指数*10000，无效则填充全0
    int32_t         highPx;                 // 最高指数*10000，无效则填充全0
    int32_t         lowPx;                  // 最低指数*10000，无效则填充全0
    int32_t         lastPx;                 // 最新指数*10000，无效则填充全0
    int32_t         closePx;                // 收盘指数*10000，无效则填充全0
    int32_t         closePx2;               // 收盘指数2*10000，无效则填充全0
    int32_t         closePx3;               // 收盘指数3*10000，无效则填充全0

    SZBTail_t       msgTail;
}PACKED SZBIndex_t;


// 港股通快照(0x0008)(交易所306311)
#define SZB_CNT_STOCKHKSS_QUEUE 10
typedef struct SZBStockHKSS
{
    SZBHead_t       msgHead;                // msgType=0x0008, msgBodyLen=254

    char            securityID[SZB_LEN_SECURITY_ID];    // 证券代码 + ".SZ", 多余字节补'\0'
    int64_t         origTime;               // 交易时间，YYYYMMDDHHMMSSsss

    /**
     * @brief tradingPhaseCode
     * 产品所处的交易阶段代码, 多余字节补'\0'
     * 第0字节：
     * 'S'=启动（开市前）
     * 'O'=开盘集合竞价
     * 'T'=连续竞价
     * 'B'=休市
     * 'C'=收盘集合竞价
     * 'E'=已闭市
     * 'H'=临时停牌
     * 'A'=盘后交易
     * 'V'=波动性中断
     * 第1字节：
     * '0'=正常状态
     * '1'=全天停牌
     */
    char            tradingPhaseCode[SZB_LEN_PHASE_CODE];

    /**
     * @brief securityIDSource
     * 证券代码源, 多余字节补'\0'
     * "102" = 深圳证券交易所
     * "103" = 香港交易所
     */
    char            securityIDSource[SZB_LEN_SECURITY_ID_SRC];
    
    uint16_t        channelNo;              // 频道
    int32_t         preClosePx;             // 昨收盘*10000
    int32_t         numTrades;              // 成交笔数
    int64_t         totalVolumeTrade;       // 成交总量*100
    int64_t         totalValueTrade;        // 成交额(元)*10000
    int32_t         openPx;                 // 无效值，填充0
    int32_t         highPx;                 // 最高价(元)*10000，无效则填充全0
    int32_t         lowPx;                  // 最低价(元)*10000，无效则填充全0
    int32_t         lastPx;                 // 最近价(元)*10000，无效则填充全0

    SZBPriceUnit_t  buyUnit[SZB_CNT_STOCKHKSS_QUEUE];   // 买价(元)*10000/买量
    SZBPriceUnit_t  selUnit[SZB_CNT_STOCKHKSS_QUEUE];   // 卖价(元)*10000/卖量

    int32_t         norminalPx;             // 按盘价(元)*10000，无效则填充全0
    int32_t         referencePx;            // 参考价(元)*10000，无效则填充全0
    int64_t         complexEventStartTime;  // 冷静期开始，YYYYMMDDHHmmSSssss，无效则填充全0
    int64_t         complexEventEndTime;    // 冷静期结束，YYYYMMDDHHmmSSssss，无效则填充全0

    SZBTail_t       msgTail;
}PACKED SZBStockHKSS_t;


// 债券快照(0x1001)(交易所300111)
#define SZB_CNT_BONDSS_QUEUE 10
typedef struct SZBBondSS
{
    SZBHead_t       msgHead;                // msgType=0x1001, msgBodyLen=262

    char            securityID[SZB_LEN_SECURITY_ID];    // 证券代码 + ".SZ", 多余字节补'\0'
    int64_t         origTime;               // 交易时间，YYYYMMDDHHMMSSsss

    /**
     * @brief tradingPhaseCode
     * 产品所处的交易阶段代码, 多余字节补'\0'
     * 第0字节：
     * 'S'=启动（开市前）
     * 'O'=开盘集合竞价
     * 'T'=连续竞价
     * 'B'=休市
     * 'C'=收盘集合竞价
     * 'E'=已闭市
     * 'H'=临时停牌
     * 'A'=盘后交易
     * 'V'=波动性中断
     * 第1字节：
     * '0'=正常状态
     * '1'=全天停牌
     */
    char            tradingPhaseCode[SZB_LEN_PHASE_CODE];

    /**
     * @brief securityIDSource
     * 证券代码源, 多余字节补'\0'
     * "102" = 深圳证券交易所
     * "103" = 香港交易所
     */
    char            securityIDSource[SZB_LEN_SECURITY_ID_SRC];
    
    uint16_t        channelNo;              // 频道
    int32_t         seqID;                  // 无效值，填充0
    int32_t         preClosePx;             // 昨收盘*10000
    int32_t         numTrades;              // 成交笔数
    int64_t         totalVolumeTrade;       // 成交总量*100
    int64_t         totalValueTrade;        // 成交额(元)*10000
    int32_t         openYield;              // 开盘收益率*10000，无效则填充全0
    int32_t         highYield;              // 最高收益率*10000，无效则填充全0
    int32_t         lowYield;               // 最低收益率*10000，无效则填充全0
    int32_t         lastYield;              // 最近收益率*10000，无效则填充全0

    SZBPriceUnit_t  buyUnit[SZB_CNT_BONDSS_QUEUE];  // 买价(元)*10000/买量
    SZBPriceUnit_t  selUnit[SZB_CNT_BONDSS_QUEUE];  // 卖价(元)*10000/卖量

    int32_t         yieldToMaturity;        // 债券到期收益率*10000，无效则填充全0
    int32_t         weightedAvgPx;          // 加权平均价(元)*10000，无效则填充全0
    int32_t         weightedAvgPxBP;        // 加权平均价涨跌(元)*10000，无效则填充全0
    int32_t         preWeightedAvgPx;       // 昨收加权平均价(元)*10000，无效则填充全0
    int32_t         preCloseYield;          // 昨收益率*10000，无效则填充全0
    int32_t         preWeightedAvgYield;    // 昨加权平均收益率*10000，无效则填充全0
    int32_t         weightedAvgYield;       // 加权平均收益率*10000，无效则填充全0

    SZBTail_t       msgTail;
}PACKED SZBBondSS_t;


// 基金快照(0x2001)(交易所300111)
#define SZB_CNT_FUNDSS_QUEUE 10
typedef struct SZBFundSS
{
    SZBHead_t       msgHead;                // msgType=0x2001, msgBodyLen=306

    char            securityID[SZB_LEN_SECURITY_ID];    // 证券代码 + ".SZ", 多余字节补'\0'
    int64_t         origTime;               // 交易时间，YYYYMMDDHHMMSSsss

    /**
     * @brief tradingPhaseCode
     * 产品所处的交易阶段代码, 多余字节补'\0'
     * 第0字节：
     * 'S'=启动（开市前）
     * 'O'=开盘集合竞价
     * 'T'=连续竞价
     * 'B'=休市
     * 'C'=收盘集合竞价
     * 'E'=已闭市
     * 'H'=临时停牌
     * 'A'=盘后交易
     * 'V'=波动性中断
     * 第1字节：
     * '0'=正常状态
     * '1'=全天停牌
     */
    char            tradingPhaseCode[SZB_LEN_PHASE_CODE];
    
    /**
     * @brief securityIDSource
     * 证券代码源, 多余字节补'\0'
     * "102" = 深圳证券交易所
     * "103" = 香港交易所
     */
    char            securityIDSource[SZB_LEN_SECURITY_ID_SRC];
    
    uint16_t        channelNo;              // 频道
    int32_t         seqID;                  // 无效值，填充0
    int32_t         preClosePx;             // 昨收盘(元)*10000
    int32_t         numTrades;              // 成交笔数
    int64_t         totalVolumeTrade;       // 成交总量*100
    int64_t         totalValueTrade;        // 成交额(元)*10000
    int32_t         openPx;                 // 开盘价(元)*10000，无效则填充全0
    int32_t         highPx;                 // 最高价(元)*10000，无效则填充全0
    int32_t         lowPx;                  // 最低价(元)*10000，无效则填充全0
    int32_t         lastPx;                 // 最近价(元)*10000，无效则填充全0
    int32_t         diffPx1;                // 升跌1*10000，无效则填充全0
    int32_t         diffPx2;                // 升跌2*10000，无效则填充全0

    SZBPriceUnit_t  buyUnit[SZB_CNT_FUNDSS_QUEUE];  // 买价(元)*10000/买量
    SZBPriceUnit_t  selUnit[SZB_CNT_FUNDSS_QUEUE];  // 卖价(元)*10000/卖量

    int64_t         IOPV;                   // IPOV*10000，无效则填充全0
    int64_t         preIOPV;                // 昨IOPV*10000，无效则填充全0
    int64_t         purchaseNum;            // 申购笔数，无效则填充全0
    int64_t         purchaseQty;            // 申购数量，无效则填充全0
    int64_t         purchaseAmt;            // 申购金额(元)*10000，无效则填充全0
    int64_t         redemptionNum;          // 赎回笔数，无效则填充全0
    int64_t         redemptionQty;          // 赎回数量，无效则填充全0
    int64_t         redemptionAmt;          // 赎回金额(元)*10000，无效则填充全0

    SZBTail_t       msgTail;
}PACKED SZBFundSS_t;


// 数据回调接口
// 回调函数应只使用msg指针访问数据或拷贝数据，不能 delete msg 指针、修改 msg 数据，
// 回调结束不能保存msg指针
class SZBMsgHandler
{
public:
    virtual ~SZBMsgHandler() {}

    // 股票普通快照(0x0001)
    virtual void onSZBStockSS(SZBStockSS *msg) = 0;

    // 股票合成快照(0x0002)
    virtual void onSZBStockSSProc(SZBStockSSRebuild *msg) = 0;

    // 逐笔成交(0x0004)
    virtual void onSZBStockTick(SZBStockTick *msg) = 0;

    // 逐笔委托(0x0005)
    virtual void onSZBStockOrder(SZBStockOrder *msg) = 0;

    // 指数快照(0x0007)
    virtual void onSZBIndex(SZBIndex *msg) = 0;

    // 港股通快照(0x0008)
    virtual void onSZBStockHKSS(SZBStockHKSS *msg) = 0;

    // 债券快照(0x1001)
    virtual void onSZBBondSS(SZBBondSS *msg) = 0;

    // 基金快照(0x2001)
    virtual void onSZBFundSS(SZBFundSS *msg) = 0;
};


// 初始化参数
typedef struct SZBRecvParam
{
    // 组播网卡名,不能为空
    std::string interface;

    // 组播IP,不能为空
    // linux下为网卡名，例如：eth0
    // windows下为网卡mac，例如：6C-4B-9F-3E-E5-F9
    std::string groupAddr;

    // 组播端口
    uint16_t groupPort;

    // 设置协议栈，默认使用内核协议栈。
    // 如果设置的协议栈不支持，则会自动使用内核协议栈。
    // 1：内核协议栈, 2：EF_VI(需Solarflare网卡)
    int stackType;

    // 绑定CPU核心编号
    // 如果设为小于0的值，或者超出CPU核心最大编号的值，则不绑定
    int recvCpuIndex;

    // 回调接口,不能为NULL
    SZBMsgHandler *callbackHandler;

    // 验证授权信息,可留空不填
    std::string verifyServer;
    std::string verifyName;
    std::string verifyPwd;
} SZBRecvParam_t;

#ifndef __linux__
  #pragma pack(pop)
#endif

#endif // SZBDATADEF_H
