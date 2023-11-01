#ifndef SSEDATADEF_H
#define SSEDATADEF_H

#include <stdint.h>
#include <string>

#define SSE_LEN_SECURITY_ID         12  // securityID char len
#define SSE_LEN_PHASE_CODE          8   // tradingPhaseCode char len
#define SSE_LEN_INST_STAT           8   // instrumentStatus char len
#define SSE_LEN_BSFLAG              4

#ifdef __linux__
  #define PACKED __attribute__((packed))
#else
  #define PACKED
  #pragma pack(push,1)
#endif

// 消息类型定义
enum SSEMDMsgType
{
    MT_SSEMarketSS              = 0x0C82, // 竞价行情快照(UA3202)
    MT_SSETransaction           = 0x0C81, // 竞价逐笔成交(UA3201)
    MT_SSEIndexSS               = 0x0C29, // 指数行情快照(UA3113)
    MT_SSEOrder                 = 0x16A9, // 竞价逐笔委托(UA5801)
    MT_SSEXtsMarketData         = 0x0EDA, // 债券行情快照数据(UA3802)
    MT_SSEXtsTick               = 0x0F3D  // 债券逐笔委托及成交数据(UA3901)
};

// 通用头
typedef struct SSEHead
{
    uint16_t    msgType;    // 消息类型
    uint16_t    msgBodyLen; // 包数据大小(不包含通用头和通用尾)
    uint64_t    seqNum;     // 序号，有分包则SeqNum不变，否则递增
    uint16_t    subSeq;     // 子序号，最高位1：有后续分包，最高位0：无后续分包
    char        reserve[2]; // 预留位
}PACKED SSEHead_t;

// 通用尾
typedef struct SSETail
{
    uint32_t    checkSum;   // 校验码
}PACKED SSETail_t;

// 价量对
typedef struct SSEPriceUnit
{
    uint32_t    price;      // 价格(元)*1000，无效则填充全0
    int64_t     volume;     // 数量*1000 (股票：股; 权证：份; 债券：手)，无效则填充全0
}PACKED SSEPriceUnit_t;


// 竞价行情快照0x0C82 (UA3202, tag10142 = 6)
#define SSE_CNT_MARKET_SS_QUEUE 10
typedef struct SSEMarketSS
{
    SSEHead     msgHead;        // msgType=0x0C82, msgBodyLen=368

    int32_t     dataTimeStamp;  // 行情时间(秒), 143025 表示 14:30:25
    char        securityID[SSE_LEN_SECURITY_ID];    // 证券代码 + ".SH", 多余字节补'\0'
    uint32_t    preClosePx;     // 昨收盘价(元)*1000，无效则填充全0
    uint32_t    openPx;         // 开盘价(元)*1000，无效则填充全0
    uint32_t    highPx;         // 最高价(元)*1000，无效则填充全0
    uint32_t    lowPx;          // 最低价(元)*1000，无效则填充全0
    uint32_t    lastPx;         // 最近价(元)*1000，无效则填充全0
    uint32_t    closePx;        // 今日收盘价(元)*1000，无效则填充全0

    /**
     * @brief instrumentStatus
     * 当前品种交易状态, 多余字节补'\0'
     * START    启动
     * OCALL    开市集合竞价
     * TRADE    连续自动撮合
     * SUSP     停牌
     * CCALL    收盘集合竞价
     * CLOSE    闭市,自动计算闭市价格
     * ENDTR    交易结束
     **/
    char        instrumentStatus[SSE_LEN_INST_STAT];

    /**
     * @brief tradingPhaseCode
     * 当前产品状态, 多余字节补'\0'
     * 该字段为8字节字符串，左起每位表示特定的含义，无定义则填空格。
     * 第0字节: 'S'表示启动（开市前）时段，
     *         'C'表示开盘集合竞价时段，
     *         'T'表示连续交易时段，
     *         'E'表示闭市时段，
     *         'P'表示产品停牌，
     *         'M'表示可恢复交易的熔断时段（盘中集合竞价），
     *         'N'表示不可恢复交易的熔断时段（暂停交易至闭市），
     *         'U'表示收盘集合竞价时段。
     * 第1字节: '0'表示此产品不可正常交易，
     *         '1'表示此产品可正常交易，无意义填空格。
     * 第2字节: '0'表示未上市，
     *         '1'表示已上市。
     * 第3字节: '0'表示此产品在当前时段不接受订单申报，
     *         '1' 表示此产品在当前时段可接受订单申报。无意义填空格。
     */
    char        tradingPhaseCode[SSE_LEN_PHASE_CODE];

    int32_t     numTrades;      // 成交笔数，无效则填充全0
    int64_t     totalVolTrade;  // 成交总量*1000 (股票：股; 权证：份; 债券：手)，无效则填充全0
    int64_t     totalValTrade;  // 成交总金额(元)*100,000，无效则填充全0

    int64_t     totalBuyVol;    // 委托买入总量*1000 (股票：股; 权证：份; 债券：手)，无效则填充全0
    uint32_t    avgBuyPx;       // 加权平均委买价格(元)*1000，无效则填充全0
    uint32_t    altAvgBuyPx;    // 债券加权平均委买价格(元)*1000，无效则填充全0

    int64_t     totalSelVol;    // 委托卖出总量*1000 (股票：股; 权证：份; 债券：手)，无效则填充全0
    uint32_t    avgSelPx;       // 加权平均委卖价格(元)*1000，无效则填充全0
    uint32_t    altAvgSelPx;    // 债券加权平均委卖价格(元)*1000，无效则填充全0

    int32_t     yieldToMaturity;// 债券到期收益率*10,000，无效则填充全0

    int32_t     totalBidNum;    // 买入总笔数，无效则填充全0
    int32_t     totalOfrNum;    // 卖出总笔数，无效则填充全0
    int32_t     numBidOrders;   // 买方委托价位数，无效则填充全0
    int32_t     numOfrOrders;   // 卖方委托价位数，无效则填充全0

    SSEPriceUnit    buyUnit[SSE_CNT_MARKET_SS_QUEUE];   // 买价(元)*1000/买量*1000
    SSEPriceUnit    selUnit[SSE_CNT_MARKET_SS_QUEUE];   // 卖价(元)*1000/卖量*1000

    SSETail     msgTail;
}PACKED SSEMarketSS_t;


// 竞价逐笔成交0x0C81 (UA3201, tag10142 = 7)
typedef struct SSETransaction
{
    SSEHead     msgHead;        // msgType=0x0C81, msgBodyLen=64

    int32_t     tradeIdx;       // 成交序号
    int32_t     tradeChnl;      // 成交通道
    char        securityID[SSE_LEN_SECURITY_ID];    // 证券代码 + ".SH", 多余字节补'\0'
    int32_t     tradingTime;    // 成交时间(百分之一秒), 14302506 表示 14:30:25.06
    uint32_t    tradePx;        // 成交价格(元)*1000
    int64_t     tradeQty;       // 成交数量*1000(股票：股；债券：手)
    int64_t     tradeMoney;     // 成交金额(元)*100,000
    int64_t     bidSeqNum;      // 买方订单号
    int64_t     ofrSeqNum;      // 卖方订单号

    /**
     * @brief tradeBSFlag
     * 内外盘标志, 多余字节补'\0'
     * "B" – 外盘,主动买
     * "S" – 内盘,主动卖
     * "N" – 未知
     **/
    char        tradeBSFlag[SSE_LEN_BSFLAG];

    int64_t     bizIndex;       // 业务序号,与竞价逐笔委托消息合并后的连续编号,从1开始,按Channel连续

    SSETail     msgTail;
}PACKED SSETransaction_t;


// 指数行情快照0x0C29 (UA3113, tag10142 = 6)
typedef struct SSEIndexSS
{
    SSEHead     msgHead;        // msgType=0x0C29, msgBodyLen=60

    int32_t     dataTimeStamp;  // 行情时间(秒), 143025 表示 14:30:25
    char        securityID[SSE_LEN_SECURITY_ID];    // 证券代码 + ".SH", 多余字节补'\0'
    uint32_t    preCloseIdx;    // 昨收盘指数*100,000，无效则填充全0
    uint32_t    openIdx;        // 开盘指数*100,000，无效则填充全0
    int64_t     turnOver;       // 参与计算相应指数的成交金额(元)*10，无效则填充全0
    uint32_t    highIdx;        // 最高指数*100,000，无效则填充全0
    uint32_t    lowIdx;         // 最低指数*100,000，无效则填充全0
    uint32_t    lastIdx;        // 最近指数*100,000，无效则填充全0
    int32_t     tradingTime;    // 成交时间(百分之一秒), 14302506 表示 14:30:25.06，无效则填充全0
    int64_t     totalVolTrade;  // 参与计算相应指数的交易数量*100,000(手)，无效则填充全0
    uint32_t    closeIdx;       // 今日收盘指数*100,000，无效则填充全0

    SSETail     msgTail;
}PACKED SSEIndexSS_t;


// 竞价逐笔委托0x16A9 (UA5801, tag10142 = 58)
#define SSE_LEN_ORDERTYPE      4
typedef struct SSEOrder
{
    SSEHead     msgHead;        // msgType=0x16A9, msgBodyLen=60

    int32_t     orderIndex;     // 委托序号 从1开始,按Channel连续
    int32_t     channel;        // 通道
    char        securityID[SSE_LEN_SECURITY_ID];    // 证券代码 + ".SH", 多余字节补'\0'
    int32_t     orderTime;      // 委托时间(百分之一秒) 14302506 表示 14:30:25.06

    /**
     * @brief orderType
     * 订单类型, 多余字节补'\0'
     * "A" – 委托订单(增加)
     * "D" – 委托订单(删除)
     **/
    char        orderType[SSE_LEN_ORDERTYPE];

    int64_t     orderNO;        // 原始订单号
    uint32_t    orderPrice;     // 委托价格(元)*1000
    uint64_t    balance;        // 剩余委托量*1000 (股票:股; 债券:手)

    /**
     * @brief orderBSFlag
     * 内外盘标志, 多余字节补'\0'
     * "B" – 买单
     * "S" – 卖单
     **/
    char        orderBSFlag[SSE_LEN_BSFLAG];

    int64_t     bizIndex;       // 业务序列号 与成交统一编号,从1开始,按Channel连续

    SSETail     msgTail;
}PACKED SSEOrder_t;


// 债券行情快照数据 (UA3802, tag10142 = 38)
#define SSE_CNT_XTS_MARKET_QUEUE     10
typedef struct SSEXtsMarketData
{
    SSEHead     msgHead;        // msgType=0x0EDA, msgBodyLen=348

    int32_t     dataTimeStamp;  // 最新订单时间(毫秒), 143025002 表示 14:30:25.002
    char        securityID[SSE_LEN_SECURITY_ID];    // 证券代码 + ".SH", 多余字节补'\0'
    uint32_t     preClosePx;    // 昨收盘(元)*1000，无效则填充全0
    uint32_t     openPx;        // 开盘价(元)*1000，无效则填充全0
    uint32_t     highPx;        // 最高价(元)*1000，无效则填充全0
    uint32_t     lowPx;         // 最低价(元)*1000，无效则填充全0
    uint32_t     lastPx;        // 最近价(元)*1000，无效则填充全0
    uint32_t     closePx;       // 今日收盘价(元)*1000，无效则填充全0

    /**
     * @brief instrumentStatus
     * 当前品种交易状态, 多余字节补'\0'
     * "START"    启动
     * "OCALL"    开市集合竞价
     * "TRADE"    连续自动撮合
     * "SUSP"     停牌
     * "CCALL"    收盘集合竞价
     * "CLOSE"    闭市,自动计算闭市价格
     * "ENDTR"    交易结束
     **/
    char        instrumentStatus[SSE_LEN_INST_STAT];

    int32_t     numTrades;      // 成交笔数，无效则填充全0
    int64_t     totalVolTrade;  // 成交总量(手)*1000，无效则填充全0
    int64_t     totalValTrade;  // 成交总金额(元)*100,000，无效则填充全0

    int64_t     totalBidQty;    // 委托买入总量(手)*1000，无效则填充全0
    uint32_t    altAvgBidPx;    // 债券加权平均委买价格(元)*1000，无效则填充全0

    int64_t     totalOfrQty;    // 委托卖出总量(手)*1000，无效则填充全0
    uint32_t    altAvgOfrPx;    // 债券加权平均委卖价格(元)*1000，无效则填充全0

    int32_t     totalBidNum;    // 买入总笔数，无效则填充全0
    int32_t     totalOfrNum;    // 卖出总笔数，无效则填充全0
    int32_t     numBidOrders;   // 买方委托价位数，无效则填充全0
    int32_t     numOfrOrders;   // 卖方委托价位数，无效则填充全0

    SSEPriceUnit    buyUnit[SSE_CNT_XTS_MARKET_QUEUE];   // 买价(元)*1000/买量(手)*1000
    SSEPriceUnit    selUnit[SSE_CNT_XTS_MARKET_QUEUE];   // 卖价(元)*1000/卖量(手)*1000

    SSETail     msgTail;
}PACKED SSEXtsMarketData_t;

// 债券逐笔委托及成交数据 (UA3901, tag10142 = 39)
#define SSE_LEN_XTS_TICKTYPE      4
#define SSE_LEN_XTS_BSFLAG        8
typedef struct SSEXtsTick
{
    SSEHead     msgHead;        // msgType=0x0F3D, msgBodyLen=72

    int32_t     tickIndex;      // 逐笔序号, 从1开始,按Channel连续
    int32_t     channel;        // 通道
    char        securityID[SSE_LEN_SECURITY_ID];    // 债券代码 + ".SH", 多余字节补'\0'
    int32_t     tickTime;       // 订单或成交时间(毫秒) 143025006 表示 14:30:25.006

    /**
     * @brief tickType
     * 类型, 多余字节补'\0'
     * "A" - 新增委托订单
     * "D" - 删除委托订单
     * "S" - 产品状态订单
     * "T" - 成交
     **/
    char        tickType[SSE_LEN_XTS_TICKTYPE];

    int64_t     buyOrderNO;     // 买方订单. 若为产品状态订单或卖方订单，则无意义，填充全0
    int64_t     selOrderNO;     // 卖方订单. 若为产品状态订单或买方订单，则无意义，填充全0

    uint32_t    price;          // 价格(元)*1000. 若为产品状态订单、删除订单，则无意义，填充全0
    int64_t     qty;            // 数量(手)*1000. 若为产品状态订单，则无意义，填充全0
    int64_t     tradeMoney;     // 成交金额(元)*100,000, 仅适用于tickType=T，无效则填充全0

    /**
     * @brief tickBSFlag
     * BSFlag, 多余字节补'\0'
     * 若为新增或删除委托订单(tickType=A或D)：
     *   "B" – 买单
     *   "S" – 卖单
     * 若为产品状态订单(tickType=S)：
     *   "ADD"   – 产品未上市
     *   "START" – 启动
     *   "OCALL" – 开市集合竞价
     *   "TRADE" – 连续自动撮合
     *   "SUSP"  – 停牌
     *   "CLOSE" – 闭市
     *   "ENDTR" – 交易结束
     * 若为成交(tickType=T)：
     *   "B" – 外盘，主动买
     *   "S" – 内盘，主动卖
     *   "N" – 未知
     **/
    char        tickBSFlag[SSE_LEN_XTS_BSFLAG];

    SSETail     msgTail;
}PACKED SSEXtsTick_t;


// 数据回调接口
// 回调函数应只使用msg指针访问数据或拷贝数据，不能 delete msg 指针、修改 msg 数据，
// 回调函数结束不能保存msg指针
class SSEMsgHandler
{
public:
    virtual ~SSEMsgHandler() {}

    // 竞价行情快照(0x0C82)
    virtual void onSSEMarketSS(SSEMarketSS *msg) = 0;

    // 竞价逐笔成交(0x0C81)
    virtual void (SSETransaction *msg) = 0;

    // 指数行情快照(0x0C29)
    virtual void onSSEIndexSS(SSEIndexSS *msg) = 0;

    // 竞价逐笔委托(0x16A9)
    virtual void onSSEOrder(SSEOrder *msg) = 0;

    // 债券行情快照数据(0x0EDA)
    virtual void onSSEXtsMarketData(SSEXtsMarketData *msg) = 0;

    // 债券逐笔委托及成交数据(0x0F3D)
    virtual void onSSEXtsTick(SSEXtsTick *msg) = 0;
};


// 初始化参数
typedef struct SSERecvParam
{
    // 组播网卡名,不能为空
    // linux下为网卡名，例如：eth0
    // windows下为网卡mac，例如：6C-4B-9F-3E-E5-F9
    std::string interface;

    // 组播IP,不能为空
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
    SSEMsgHandler *callbackHandler;
} SSERecvParam_t;

#ifndef __linux__
  #pragma pack(pop)
#endif

#endif // SSEDATADEF_H
