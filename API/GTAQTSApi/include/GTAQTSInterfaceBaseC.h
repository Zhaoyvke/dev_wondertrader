//////////////////////////////////////////////////////////////////////////////
/// @file       GTAQTSInterfaceBaseC.h 
/// @brief      QTS行情接口C语言接口，按结构体返回数据，自动重连
/// @copyright  Copyright (C), 2008-2023, CSMAR Information Tech. Co., Ltd.
/// @version    3.22
/// @date       2022.12.08
//////////////////////////////////////////////////////////////////////////////
#ifndef GTA_QTS_INTERFACE_BASE_C_H_
#define GTA_QTS_INTERFACE_BASE_C_H_
#include "QTSDataType.h"
#include "QTSStruct.h"
#include "QTSDataFieldDefine.h"
extern "C"
{
/// 实时数据、连接状态回调函数，按结构体返回数据
/// 行情系统服务接口，连接服务器进行认证，订阅数据，查询快照.
/// 登陆状态返回
/// @param  errCode  -- 失败原因(用户过期、重复登陆)用户需对这两个进行处理
typedef void (*OnBaseLoginState)( RetCode errCode);
void GTA_API_EXPORT _CDECL SetOnBaseLoginState(OnBaseLoginState cbOnBaseLoginState);

/// 连接状态返回，连接成功/失败
/// @param  msgType      -- 消息类型
/// @param  errCode     -- 失败原因，成功时返回0
typedef void (*OnBaseConnectionState)(MsgType msgType, RetCode errCode);
void GTA_API_EXPORT _CDECL SetOnBaseConnectionState(OnBaseConnectionState cbOnBaseConnectionState);

/// 延时数据返回
/// @param  DelayTime      -- 延时数据
typedef void (*OnBaseDelayTime)(const DelayTimeInfo* pDelayTime);
void GTA_API_EXPORT _CDECL SetOnBaseDelayTime(OnBaseDelayTime cbOnBaseDelayTime);

/// 上交所L1静态数据订阅数据实时回调接口
/// @param  pRealSSEL1Static     -- 实时数据
typedef void (*OnBaseSubscribe_SSEL1_Static)(const SSEL1_Static* pRealSSEL1Static);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SSEL1_Static(OnBaseSubscribe_SSEL1_Static cbOnBaseSubscribe_SSEL1_Static);

/// 上交所L1实时行情订阅数据实时回调接口
/// @param  pRealSSEL1Quotation  -- 实时数据
typedef void (*OnBaseSubscribe_SSEL1_Quotation)(const SSEL1_Quotation* pRealSSEL1Quotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SSEL1_Quotation(OnBaseSubscribe_SSEL1_Quotation cbOnBaseSubscribe_SSEL1_Quotation);

/// 上交所指数通数据实时回调接口
/// @param  pRealSSEIndexPress  -- 实时数据
typedef void (*OnBaseSubscribe_SSE_IndexPress)(const SSE_IndexPress* pRealSSEIndexPress);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SSE_IndexPress(OnBaseSubscribe_SSE_IndexPress cbOnBaseSubscribe_SSE_IndexPress);

/// 上交所L2静态数据订阅数据实时回调接口
/// @param  pRealSSEL2Static     -- 实时数据
typedef void (*OnBaseSubscribe_SSEL2_Static)(const SSEL2_Static* pRealSSEL2Static);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SSEL2_Static(OnBaseSubscribe_SSEL2_Static cbOnBaseSubscribe_SSEL2_Static);

/// 上交所L2实时行情快照订阅数据实时回调接口
/// @param  pRealSSEL2Quotation  -- 实时数据
typedef void (*OnBaseSubscribe_SSEL2_Quotation)(const SSEL2_Quotation* pRealSSEL2Quotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SSEL2_Quotation(OnBaseSubscribe_SSEL2_Quotation cbOnBaseSubscribe_SSEL2_Quotation);

/// 上交所L2指数行情订阅数据实时回调接口
/// @param  pRealSSEL2Index      -- 实时数据
typedef void (*OnBaseSubscribe_SSEL2_Index)(const SSEL2_Index* pRealSSEL2Index);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SSEL2_Index(OnBaseSubscribe_SSEL2_Index cbOnBaseSubscribe_SSEL2_Index);

/// 上交所L2逐笔成交订阅数据实时回调接口
/// @param  pRealSSEL2Transaction    -- 实时数据
typedef void (*OnBaseSubscribe_SSEL2_Transaction)(const SSEL2_Transaction* pRealSSEL2Transaction);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SSEL2_Transaction(OnBaseSubscribe_SSEL2_Transaction cbOnBaseSubscribe_SSEL2_Transaction);

/// 上交所L2虚拟集合竞价订阅数据实时回调接口
/// @param  pRealSSEL2Auction    -- 实时数据
typedef void (*OnBaseSubscribe_SSEL2_Auction)(const SSEL2_Auction* pRealSSEL2Auction);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SSEL2_Auction(OnBaseSubscribe_SSEL2_Auction cbOnBaseSubscribe_SSEL2_Auction);

/// 上交所L2市场总览订阅数据实时回调接口
/// @param  pRealSSEL2Overview   -- 实时数据
typedef void (*OnBaseSubscribe_SSEL2_Overview)(const SSEL2_Overview* pRealSSEL2Overview);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SSEL2_Overview(OnBaseSubscribe_SSEL2_Overview cbOnBaseSubscribe_SSEL2_Overview);

/// 上交所个股期权静态数据订阅数据实时回调接口
/// @param  pRealSSEIOL1Static   -- 实时数据
typedef void (*OnBaseSubscribe_SSEIOL1_Static)(const SSEIOL1_Static* pRealSSEIOL1Static);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SSEIOL1_Static(OnBaseSubscribe_SSEIOL1_Static cbOnBaseSubscribe_SSEIOL1_Static);

/// 上交所个股期权实时行情订阅数据实时回调接口
/// @param  pRealSSEIOL1Quotation    -- 实时数据
typedef void (*OnBaseSubscribe_SSEIOL1_Quotation)(const SSEIOL1_Quotation* pRealSSEIOL1Quotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SSEIOL1_Quotation(OnBaseSubscribe_SSEIOL1_Quotation cbOnBaseSubscribe_SSEIOL1_Quotation);

/// 上交所个股期权L1组合策略订阅数据实时回调接口
/// @param  pRealSSEIOL1Strategy    -- 实时数据
typedef void (*OnBaseSubscribe_SSEIOL1_Strategy)(const SSEIOL1_Strategy* pRealSSEIOL1Strategy);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SSEIOL1_Strategy(OnBaseSubscribe_SSEIOL1_Strategy cbOnBaseSubscribe_SSEIOL1_Strategy);

/// 深交所L1静态数据订阅数据实时回调接口
/// @param  pRealSZSEL1Static    -- 实时数据
typedef void (*OnBaseSubscribe_SZSEL1_Static)(const SZSEL1_Static* pRealSZSEL1Static);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEL1_Static(OnBaseSubscribe_SZSEL1_Static cbOnBaseSubscribe_SZSEL1_Static);

/// 深交所L1实时行情订阅数据实时回调接口
/// @param  pRealSZSEL1Quotation -- 实时数据
typedef void (*OnBaseSubscribe_SZSEL1_Quotation)(const SZSEL1_Quotation* pRealSZSEL1Quotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEL1_Quotation(OnBaseSubscribe_SZSEL1_Quotation cbOnBaseSubscribe_SZSEL1_Quotation);

/// 深交所L1公告回调接口
/// @param  pRealSZSEL1Bullet -- 实时数据
typedef void (*OnBaseSubscribe_SZSEL1_Bulletin)(const SZSEL1_Bulletin* pRealSZSEL1Bullet);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEL1_Bulletin(OnBaseSubscribe_SZSEL1_Bulletin cbOnBaseSubscribe_SZSEL1_Bulletin);

/// 深交所个股期权L1静态数据订阅数据实时回调接口
/// @param  pRealSZSEIOL1Static    -- 实时数据
typedef void (*OnBaseSubscribe_SZSEIOL1_Static)(const SZSEIOL1_Static* pRealSZSEIOL1Static);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEIOL1_Static(OnBaseSubscribe_SZSEIOL1_Static cbOnBaseSubscribe_SZSEIOL1_Static);

/// 深交所个股期权L1实时行情订阅数据实时回调接口
/// @param  pRealSZSEIOL1Quotation    -- 实时数据
typedef void (*OnBaseSubscribe_SZSEIOL1_Quotation)(const SZSEIOL1_Quotation* pRealSZSEIOL1Quotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEIOL1_Quotation(OnBaseSubscribe_SZSEIOL1_Quotation cbOnBaseSubscribe_SZSEIOL1_Quotation);

/// 深交所个股期权L1证券状态订阅数据实时回调接口
/// @param  pRealSZSEIOL1Status    -- 实时数据
typedef void (*OnBaseSubscribe_SZSEIOL1_Status)(const SZSEIOL1_Status* pRealSZSEIOL1Status);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEIOL1_Status(OnBaseSubscribe_SZSEIOL1_Status cbOnBaseSubscribe_SZSEIOL1_Status);

/// 深交所个股期权L1组合策略订阅数据实时回调接口
/// @param  pRealSZSEIOL1Strategy    -- 实时数据
typedef void (*OnBaseSubscribe_SZSEIOL1_Strategy)(const SZSEIOL1_Strategy* pRealSZSEIOL1Strategy);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEIOL1_Strategy(OnBaseSubscribe_SZSEIOL1_Strategy cbOnBaseSubscribe_SZSEIOL1_Strategy);


/// 深交所L2静态数据订阅数据实时回调接口
/// @param  pRealSZSEL2Static    -- 实时数据
typedef void (*OnBaseSubscribe_SZSEL2_Static)(const SZSEL2_Static* pRealSZSEL2Static);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEL2_Static(OnBaseSubscribe_SZSEL2_Static cbOnBaseSubscribe_SZSEL2_Static);

/// 深交所L2实时行情订阅数据实时回调接口
/// @param  pRealSZSEL2Quotation -- 实时数据
typedef void (*OnBaseSubscribe_SZSEL2_Quotation)(const SZSEL2_Quotation* pRealSZSEL2Quotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEL2_Quotation(OnBaseSubscribe_SZSEL2_Quotation cbOnBaseSubscribe_SZSEL2_Quotation);

/// 深交所L2证券状态订阅数据实时回调接口
/// @param  pRealSZSEL2Status    -- 实时数据
typedef void (*OnBaseSubscribe_SZSEL2_Status)(const SZSEL2_Status* pRealSZSEL2Status);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEL2_Status(OnBaseSubscribe_SZSEL2_Status cbOnBaseSubscribe_SZSEL2_Status);

/// 深交所L2指数行情订阅数据实时回调接口
/// @param  pRealSZSEL2Index     -- 实时数据
typedef void (*OnBaseSubscribe_SZSEL2_Index)(const SZSEL2_Index* pRealSZSEL2Index);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEL2_Index(OnBaseSubscribe_SZSEL2_Index cbOnBaseSubscribe_SZSEL2_Index);

/// 深交所L2逐笔委托订阅数据实时回调接口
/// @param  pRealSZSEL2Order     -- 实时数据
typedef void (*OnBaseSubscribe_SZSEL2_Order)(const SZSEL2_Order* pRealSZSEL2Order);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEL2_Order(OnBaseSubscribe_SZSEL2_Order cbOnBaseSubscribe_SZSEL2_Order);

/// 深交所L2逐笔成交订阅数据实时回调接口
/// @param  pRealSZSEL2Transaction   -- 实时数据
typedef void (*OnBaseSubscribe_SZSEL2_Transaction)(const SZSEL2_Transaction* pRealSZSEL2Transaction);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEL2_Transaction(OnBaseSubscribe_SZSEL2_Transaction cbOnBaseSubscribe_SZSEL2_Transaction);

/// 深交所L2逐笔订阅数据实时回调接口
/// @param  pRealSZSEL2Tick   -- 实时数据
typedef void (*OnBaseSubscribe_SZSEL2_Tick)(const SZSEL2_Tick* pRealSZSEL2Tick);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEL2_Tick(OnBaseSubscribe_SZSEL2_Tick cbOnBaseSubscribe_SZSEL2_Tick);

/// 中金所静态数据订阅数据实时回调接口
/// @param  pRealCFFEXL2Static   -- 实时数据
typedef void (*OnBaseSubscribe_CFFEXL2_Static)(const CFFEXL2_Static* pRealCFFEXL2Static);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_CFFEXL2_Static(OnBaseSubscribe_CFFEXL2_Static cbOnBaseSubscribe_CFFEXL2_Static);

/// 中金所L2实时行情订阅数据实时回调接口
/// @param  pRealCFFEXL2Quotation    -- 实时数据
typedef void (*OnBaseSubscribe_CFFEXL2_Quotation)(const CFFEXL2_Quotation* pRealCFFEXL2Quotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_CFFEXL2_Quotation(OnBaseSubscribe_CFFEXL2_Quotation cbOnBaseSubscribe_CFFEXL2_Quotation);

/// 郑商所静态数据订阅数据实时回调接口
/// @param  pRealCZCEL1Static    -- 实时数据
typedef void (*OnBaseSubscribe_CZCEL1_Static)(const CZCEL1_Static* pRealCZCEL1Static);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_CZCEL1_Static(OnBaseSubscribe_CZCEL1_Static cbOnBaseSubscribe_CZCEL1_Static);

/// 郑商所实时行情订阅数据实时回调接口
/// @param  pRealCZCEL1Quotation -- 实时数据
typedef void (*OnBaseSubscribe_CZCEL1_Quotation)(const CZCEL1_Quotation* pRealCZCEL1Quotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_CZCEL1_Quotation(OnBaseSubscribe_CZCEL1_Quotation cbOnBaseSubscribe_CZCEL1_Quotation);

/// 郑商所套利行情订阅数据实时回调接口
/// @param  pRealCZCEL1ArbiQuotation -- 实时数据
typedef void (*OnBaseSubscribe_CZCEL1_ArbiQuotation)(const CZCEL1_ArbiQuotation* pRealCZCEL1ArbiQuotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_CZCEL1_ArbiQuotation(OnBaseSubscribe_CZCEL1_ArbiQuotation cbOnBaseSubscribe_CZCEL1_ArbiQuotation);

///郑商所L1市场状态订阅数据实时回调接口
/// @param  pRealCZCEL1MktStatus -- 实时数据
typedef void (*OnBaseSubscribe_CZCEL1_MktStatus)(const CZCEL1_MktStatus* pRealCZCEL1MktStatus);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_CZCEL1_MktStatus(OnBaseSubscribe_CZCEL1_MktStatus cbOnBaseSubscribe_CZCEL1_MktStatus);

/// 郑商所L1品种状态订阅数据实时回调接口
/// @param  pRealCZCEL1Status -- 实时数据
typedef void (*OnBaseSubscribe_CZCEL1_Status)(const CZCEL1_Status* pRealCZCEL1Status);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_CZCEL1_Status(OnBaseSubscribe_CZCEL1_Status cbOnBaseSubscribe_CZCEL1_Status);

/// 易盛指数订阅数据实时回调接口
/// @param  pRealCZCEL1Quotation -- 实时数据
typedef void (*OnBaseSubscribe_ESUNNY_Index)(const ESUNNY_Index* pRealESUNNY_Index);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_ESUNNY_Index(OnBaseSubscribe_ESUNNY_Index cbOnBaseSubscribe_ESUNNY_Index);

/// 大商所L1静态数据订阅数据实时回调接口
/// @param  pRealDCEL1Static     -- 实时数据
typedef void (*OnBaseSubscribe_DCEL1_Static)(const DCEL1_Static* pRealDCEL1Static);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_DCEL1_Static(OnBaseSubscribe_DCEL1_Static cbOnBaseSubscribe_DCEL1_Static);

/// 大商所L1市场状态订阅数据实时回调接口
/// @param  pRealDCEL1MktStatus     -- 实时数据
typedef void (*OnBaseSubscribe_DCEL1_MktStatus)(const DCEL1_MktStatus* pRealDCEL1MktStatus);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_DCEL1_MktStatus(OnBaseSubscribe_DCEL1_MktStatus cbOnBaseSubscribe_DCEL1_MktStatus);

/// 大商所L1交易状态订阅数据实时回调接口
/// @param  pRealDCEL1Status     -- 实时数据
typedef void (*OnBaseSubscribe_DCEL1_Status)(const DCEL1_Status* pRealDCEL1Status);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_DCEL1_Status(OnBaseSubscribe_DCEL1_Status cbOnBaseSubscribe_DCEL1_Status);

/// 大商所L1期权参数订阅数据实时回调接口
/// @param  pRealDCEL1OptionParameter     -- 实时数据
typedef void (*OnBaseSubscribe_DCEL1_OptionParameter)(const DCEL1_OptionParameter* pRealDCEL1OptionParameter);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_DCEL1_OptionParameter(OnBaseSubscribe_DCEL1_OptionParameter cbOnBaseSubscribe_DCEL1_OptionParameter);

/// 大商所L1实时行情订阅数据实时回调接口
/// @param  pRealDCEL1Quotation  -- 实时数据
typedef void (*OnBaseSubscribe_DCEL1_Quotation)(const DCEL1_Quotation* pRealDCEL1Quotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_DCEL1_Quotation(OnBaseSubscribe_DCEL1_Quotation cbOnBaseSubscribe_DCEL1_Quotation);

/// 大商所L1套利行情订阅数据实时回调接口
/// @param  pRealDCEL1ArbiQuotation  -- 实时数据
typedef void (*OnBaseSubscribe_DCEL1_ArbiQuotation)(const DCEL1_ArbiQuotation* pRealDCEL1ArbiQuotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_DCEL1_ArbiQuotation(OnBaseSubscribe_DCEL1_ArbiQuotation cbOnBaseSubscribe_DCEL1_ArbiQuotation);

/// 大商所L2静态数据订阅数据实时回调接口
/// @param  pRealDCEL2Static     -- 实时数据
typedef void (*OnBaseSubscribe_DCEL2_Static)(const DCEL2_Static* pRealDCEL2Static);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_DCEL2_Static(OnBaseSubscribe_DCEL2_Static cbOnBaseSubscribe_DCEL2_Static);

/// 大商所L2实时行情订阅数据实时回调接口
/// @param  pRealDCEL2Quotation  -- 实时数据
typedef void (*OnBaseSubscribe_DCEL2_Quotation)(const DCEL2_Quotation* pRealDCEL2Quotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_DCEL2_Quotation(OnBaseSubscribe_DCEL2_Quotation cbOnBaseSubscribe_DCEL2_Quotation);

/// 大商所L2套利行情订阅数据实时回调接口
/// @param  pRealDCEL2ArbiQuotation  -- 实时数据
typedef void (*OnBaseSubscribe_DCEL2_ArbiQuotation)(const DCEL2_ArbiQuotation* pRealDCEL2ArbiQuotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_DCEL2_ArbiQuotation(OnBaseSubscribe_DCEL2_ArbiQuotation cbOnBaseSubscribe_DCEL2_ArbiQuotation);

/// 大商所L2实时结算价订阅数据实时回调接口
/// @param  pRealDCEL2RealTimePrice  -- 实时数据
typedef void (*OnBaseSubscribe_DCEL2_RealTimePrice)(const DCEL2_RealTimePrice* pRealDCEL2RealTimePrice);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_DCEL2_RealTimePrice(OnBaseSubscribe_DCEL2_RealTimePrice cbOnBaseSubscribe_DCEL2_RealTimePrice);

/// 大商所L2委托统计行情订阅数据实时回调接口
/// @param  pRealDCEL2OrderStatistic -- 实时数据
typedef void (*OnBaseSubscribe_DCEL2_OrderStatistic)(const DCEL2_OrderStatistic* pRealDCEL2OrderStatistic);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_DCEL2_OrderStatistic(OnBaseSubscribe_DCEL2_OrderStatistic cbOnBaseSubscribe_DCEL2_OrderStatistic);

/// 大商所L2分价成交量行情订阅数据实时回调接口
/// @param  pRealDCEL2MarchPriceQty  -- 实时数据
typedef void (*OnBaseSubscribe_DCEL2_MarchPriceQty)(const DCEL2_MarchPriceQty* pRealDCEL2MarchPriceQty);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_DCEL2_MarchPriceQty(OnBaseSubscribe_DCEL2_MarchPriceQty cbOnBaseSubscribe_DCEL2_MarchPriceQty);

/// 大商所L2市场状态订阅数据实时回调接口
/// @param  pRealDCEL2MktStatus     -- 实时数据
typedef void (*OnBaseSubscribe_DCEL2_MktStatus)(const DCEL2_MktStatus* pRealDCEL2MktStatus);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_DCEL2_MktStatus(OnBaseSubscribe_DCEL2_MktStatus cbOnBaseSubscribe_DCEL2_MktStatus);

/// 大商所L2交易状态订阅数据实时回调接口
/// @param  pRealDCEL2Status     -- 实时数据
typedef void (*OnBaseSubscribe_DCEL2_Status)(const DCEL2_Status* pRealDCEL2Status);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_DCEL2_Status(OnBaseSubscribe_DCEL2_Status cbOnBaseSubscribe_DCEL2_Status);

/// 大商所L2期权参数订阅数据实时回调接口
/// @param  pRealDCEL2OptionParameter     -- 实时数据
typedef void (*OnBaseSubscribe_DCEL2_OptionParameter)(const DCEL2_OptionParameter* pRealDCEL2OptionParameter);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_DCEL2_OptionParameter(OnBaseSubscribe_DCEL2_OptionParameter cbOnBaseSubscribe_DCEL2_OptionParameter);

/// 大商所L2五档深度行情订阅数据实时回调接口
/// @param  pRealDCEL2LevelQuotation     -- 实时数据
typedef void (*OnBaseSubscribe_DCEL2_LevelQuotation)(const DCEL2_LevelQuotation* pRealDCEL2LevelQuotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_DCEL2_LevelQuotation(OnBaseSubscribe_DCEL2_LevelQuotation cbOnBaseSubscribe_DCEL2_LevelQuotation);

/// 大商所L2委托队列行情数据实时回调接口
/// @param  pRealDCEL2OrderQueue     -- 实时数据
typedef void (*OnBaseSubscribe_DCEL2_OrderQueue)(const DCEL2_OrderQueue* pRealDCEL2OrderQueue);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_DCEL2_OrderQueue(OnBaseSubscribe_DCEL2_OrderQueue cbOnBaseSubscribe_DCEL2_OrderQueue);

/// 上期所静态数据订阅数据实时回调接口
/// @param  pRealSHFEL1Static    -- 实时数据
typedef void (*OnBaseSubscribe_SHFEL1_Static)(const SHFEL1_Static* pRealSHFEL1Static);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SHFEL1_Static(OnBaseSubscribe_SHFEL1_Static cbOnBaseSubscribe_SHFEL1_Static);

/// 上期所实时行情订阅数据实时回调接口
/// @param  pRealSHFEL1Quotation -- 实时数据
typedef void (*OnBaseSubscribe_SHFEL1_Quotation)(const SHFEL1_Quotation* pRealSHFEL1Quotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SHFEL1_Quotation(OnBaseSubscribe_SHFEL1_Quotation cbOnBaseSubscribe_SHFEL1_Quotation);

/// 上期能源静态数据订阅数据实时回调接口
/// @param  pRealINEL1Static    -- 实时数据
typedef void (*OnBaseSubscribe_INEL1_Static)(const INEL1_Static* pRealINEL1Static);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_INEL1_Static(OnBaseSubscribe_INEL1_Static cbOnBaseSubscribe_INEL1_Static);

/// 上期能源实时行情订阅数据实时回调接口
/// @param  pRealINEL1Quotation -- 实时数据
typedef void (*OnBaseSubscribe_INEL1_Quotation)(const INEL1_Quotation* pRealINEL1Quotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_INEL1_Quotation(OnBaseSubscribe_INEL1_Quotation cbOnBaseSubscribe_INEL1_Quotation);

//上期二代
/// 上期所L2静态数据订阅数据实时回调接口
/// @param  pRealSHFEL2Static    -- 实时数据
typedef void (*OnBaseSubscribe_SHFEL2_Static)(const SHFEL2_Static* pRealSHFEL2Static);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SHFEL2_Static(OnBaseSubscribe_SHFEL2_Static cbOnBaseSubscribe_SHFEL2_Static);

/// 上期所L2实时行情订阅数据实时回调接口
/// @param  pRealSHFEL2Quotation -- 实时数据
typedef void (*OnBaseSubscribe_SHFEL2_Quotation)(const SHFEL2_Quotation* pRealSHFEL2Quotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SHFEL2_Quotation(OnBaseSubscribe_SHFEL2_Quotation cbOnBaseSubscribe_SHFEL2_Quotation);

/// 上期能源L2静态数据订阅数据实时回调接口
/// @param  pRealINEL2Static    -- 实时数据
typedef void (*OnBaseSubscribe_INEL2_Static)(const INEL2_Static* pRealINEL2Static);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_INEL2_Static(OnBaseSubscribe_INEL2_Static cbOnBaseSubscribe_INEL2_Static);

/// 上期能源L2实时行情订阅数据实时回调接口
/// @param  pRealINEL2Quotation -- 实时数据
typedef void (*OnBaseSubscribe_INEL2_Quotation)(const INEL2_Quotation* pRealINEL2Quotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_INEL2_Quotation(OnBaseSubscribe_INEL2_Quotation cbOnBaseSubscribe_INEL2_Quotation);


/// 港交所实时行情订阅数据实时回调接口
/// @param  pRealHKEXL2Quotation -- 实时数据
typedef void (*OnBaseSubscribe_HKEXL2_Quotation)(const HKEXL2_Quotation* pRealHKEXL2Quotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_HKEXL2_Quotation(OnBaseSubscribe_HKEXL2_Quotation cbOnBaseSubscribe_HKEXL2_Quotation);

/// 港交所静态数据订阅数据实时回调接口
/// @param  pRealHKEXL2Static    -- 实时数据
typedef void (*OnBaseSubscribe_HKEXL2_Static)(const HKEXL2_Static* pRealHKEXL2Static);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_HKEXL2_Static(OnBaseSubscribe_HKEXL2_Static cbOnBaseSubscribe_HKEXL2_Static);

/// 港交所经纪人队列数据订阅数据实时回调接口
/// @param  pRealHKEXL2BrokerQueue    -- 实时数据
typedef void (*OnBaseSubscribe_HKEXL2_BrokerQueue)(const HKEXL2_BrokerQueue* pRealHKEXL2BrokerQueue);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_HKEXL2_BrokerQueue(OnBaseSubscribe_HKEXL2_BrokerQueue cbOnBaseSubscribe_HKEXL2_BrokerQueue);

/// 港交所指数行情订阅数据实时回调接口
/// @param  pRealHKEXL2Index    -- 实时数据
typedef void (*OnBaseSubscribe_HKEXL2_Index)(const HKEXL2_Index* pRealHKEXL2Index);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_HKEXL2_Index(OnBaseSubscribe_HKEXL2_Index cbOnBaseSubscribe_HKEXL2_Index);

/// 港交所市场总览订阅数据实时回调接口
/// @param  pRealHKEXL2Overview    -- 实时数据
typedef void (*OnBaseSubscribe_HKEXL2_Overview)(const HKEXL2_Overview* pRealHKEXL2Overview);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_HKEXL2_Overview(OnBaseSubscribe_HKEXL2_Overview cbOnBaseSubscribe_HKEXL2_Overview);

/// 深港通静态数据订阅数据实时回调接口
/// @param  pRealHKSZL1Static    -- 实时数据
typedef void (*OnBaseSubscribe_HKSZL1_Static)(const HKSZL1_Static* pRealHKSZL1Static);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_HKSZL1_Static(OnBaseSubscribe_HKSZL1_Static cbOnBaseSubscribe_HKSZL1_Static);

/// 深港通实时行情订阅数据实时回调接口
/// @param  pRealHKSZL1Quotation -- 实时数据
typedef void (*OnBaseSubscribe_HKSZL1_Quotation)(const HKSZL1_Quotation* pRealHKSZL1Quotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_HKSZL1_Quotation(OnBaseSubscribe_HKSZL1_Quotation cbOnBaseSubscribe_HKSZL1_Quotation);

/// 沪港通静态数据订阅数据实时回调接口
/// @param  pRealHKSHL1Static    -- 实时数据
typedef void (*OnBaseSubscribe_HKSHL1_Static)(const HKSHL1_Static* pRealHKSHL1Static);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_HKSHL1_Static(OnBaseSubscribe_HKSHL1_Static cbOnBaseSubscribe_HKSHL1_Static);

/// 沪港通实时行情订阅数据实时回调接口
/// @param  pRealHKSHL1Quotation -- 实时数据
typedef void (*OnBaseSubscribe_HKSHL1_Quotation)(const HKSHL1_Quotation* pRealHKSHL1Quotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_HKSHL1_Quotation(OnBaseSubscribe_HKSHL1_Quotation cbOnBaseSubscribe_HKSHL1_Quotation);

/// 港交所L2互联市场日余额订阅数据实时回调接口
/// @param  pRealHKEXL2DQB -- 实时数据
typedef void (*OnBaseSubscribe_HKEXL2_DQB)(const HKEXL2_DQB* pRealHKEXL2DQB);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_HKEXL2_DQB(OnBaseSubscribe_HKEXL2_DQB cbOnBaseSubscribe_HKEXL2_DQB);

/// 港交所L2资金流向订阅数据实时回调接口
/// @param  pRealHKEXL2DQB -- 实时数据
typedef void (*OnBaseSubscribe_HKEXL2_MoneyFlow)(const HKEXL2_MoneyFlow* pRealHKEXL2MoneyFlow);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_HKEXL2_MoneyFlow(OnBaseSubscribe_HKEXL2_MoneyFlow cbOnBaseSubscribe_HKEXL2_MoneyFlow);

/// 沪港通L1市场总览订阅数据实时回调接口
/// @param  pRealHKSHL1Overview -- 实时数据
typedef void (*OnBaseSubscribe_HKSHL1_Overview)(const HKSHL1_Overview* pRealHKSHL1Overview);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_HKSHL1_Overview(OnBaseSubscribe_HKSHL1_Overview cbOnBaseSubscribe_HKSHL1_Overview);

/// 深港通L1市场总览订阅数据实时回调接口
/// @param  pRealHKSZL1Overview -- 实时数据
typedef void (*OnBaseSubscribe_HKSZL1_Overview)(const HKSZL1_Overview* pRealHKSZL1Overview);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_HKSZL1_Overview(OnBaseSubscribe_HKSZL1_Overview cbOnBaseSubscribe_HKSZL1_Overview);

/// 上交所L2逐笔委托订阅数据实时回调接口
/// @param  pRealSSEL2Order -- 实时数据
typedef void (*OnBaseSubscribe_SSEL2_Order)(const SSEL2_Order* pRealSSEL2Order);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SSEL2_Order(OnBaseSubscribe_SSEL2_Order cbOnBaseSubscribe_SSEL2_Order);

/// 深交所L2国证指数订阅数据实时回调接口
/// @param  pRealSZSEL2Cnindex -- 实时数据
typedef void (*OnBaseSubscribe_SZSEL2_Cnindex)(const SZSEL2_Cnindex* pRealSZSEL2Cnindex);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEL2_Cnindex(OnBaseSubscribe_SZSEL2_Cnindex cbOnBaseSubscribe_SZSEL2_Cnindex);

/// 上交所L2债券静态数据订阅数据实时回调接口
/// @param  pRealSSEL2BondStatic -- 实时数据
typedef void (*OnBaseSubscribe_SSEL2_BondStatic)(const SSEL2_BondStatic* pRealSSEL2BondStatic);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SSEL2_BondStatic(OnBaseSubscribe_SSEL2_BondStatic cbOnBaseSubscribe_SSEL2_BondStatic);

/// 上交所L2债券实时行情订阅数据实时回调接口
/// @param  pRealSSEL2BondQuotation -- 实时数据
typedef void (*OnBaseSubscribe_SSEL2_BondQuotation)(const SSEL2_BondQuotation* pRealSSEL2BondQuotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SSEL2_BondQuotation(OnBaseSubscribe_SSEL2_BondQuotation cbOnBaseSubscribe_SSEL2_BondQuotation);

/// 上交所L2债券逐笔数据订阅数据实时回调接口
/// @param  pRealSSEL2BondTick -- 实时数据
typedef void (*OnBaseSubscribe_SSEL2_BondTick)(const SSEL2_BondTick* pRealSSEL2BondTick);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SSEL2_BondTick(OnBaseSubscribe_SSEL2_BondTick cbOnBaseSubscribe_SSEL2_BondTick);

/// 上交所L2债券市场总览订阅数据实时回调接口
/// @param  pRealSSEL2BondOverview -- 实时数据
typedef void (*OnBaseSubscribe_SSEL2_BondOverview)(const SSEL2_BondOverview* pRealSSEL2BondOverview);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SSEL2_BondOverview(OnBaseSubscribe_SSEL2_BondOverview cbOnBaseSubscribe_SSEL2_BondOverview);

/// 上交所L1债券静态数据订阅数据实时回调接口
/// @param  pRealSSEL1BondStatic -- 实时数据
typedef void (*OnBaseSubscribe_SSEL1_BondStatic)(const SSEL1_BondStatic* pRealSSEL1BondStatic);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SSEL1_BondStatic(OnBaseSubscribe_SSEL1_BondStatic cbOnBaseSubscribe_SSEL1_BondStatic);

/// 上交所L1债券实时行情订阅数据实时回调接口
/// @param  pRealSSEL1BondQuotation -- 实时数据
typedef void (*OnBaseSubscribe_SSEL1_BondQuotation)(const SSEL1_BondQuotation* pRealSSEL1BondQuotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SSEL1_BondQuotation(OnBaseSubscribe_SSEL1_BondQuotation cbOnBaseSubscribe_SSEL1_BondQuotation);


/// 深交所L2债券静态数据订阅数据实时回调接口
/// @param  pRealSZSEL2BondStatic -- 实时数据
typedef void (*OnBaseSubscribe_SZSEL2_BondStatic)(const SZSEL2_BondStatic* pRealSZSEL2BondStatic);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEL2_BondStatic(OnBaseSubscribe_SZSEL2_BondStatic cbOnBaseSubscribe_SZSEL2_BondStatic);

/// 深交所L2债券实时行情数据订阅数据实时回调接口
/// @param  pRealSZSEL2BondStatic -- 实时数据
typedef void (*OnBaseSubscribe_SZSEL2_BondQuotation)(const SZSEL2_BondQuotation* pRealSZSEL2BondQuotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEL2_BondQuotation(OnBaseSubscribe_SZSEL2_BondQuotation cbOnBaseSubscribe_SZSEL2_BondQuotation);

/// 深交所L2债券逐笔成交数据订阅数据实时回调接口
/// @param  pRealSZSEL2BondTransaction -- 实时数据
typedef void (*OnBaseSubscribe_SZSEL2_BondTransaction)(const SZSEL2_BondTransaction* pRealSZSEL2BondTransaction);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEL2_BondTransaction(OnBaseSubscribe_SZSEL2_BondTransaction cbOnBaseSubscribe_SZSEL2_BondTransaction);

/// 深交所L2债券逐笔委托订阅数据实时回调接口
/// @param  pRealSZSEL2BondOrder -- 实时数据
typedef void (*OnBaseSubscribe_SZSEL2_BondOrder)(const SZSEL2_BondOrder* pRealSZSEL2BondOrder);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEL2_BondOrder(OnBaseSubscribe_SZSEL2_BondOrder cbOnBaseSubscribe_SZSEL2_BondOrder);

/// 深交所L2债券逐比消息订阅数据实时回调接口
/// @param  pRealSZSEL2BondTick -- 实时数据
typedef void (*OnBaseSubscribe_SZSEL2_BondTick)(const SZSEL2_BondTick* pRealSZSEL2BondTick);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEL2_BondTick(OnBaseSubscribe_SZSEL2_BondTick cbOnBaseSubscribe_SZSEL2_BondTick);

/// 深交所L2债券交易商信息订阅数据实时回调接口
/// @param  pRealSZSEL2BondTick -- 实时数据
typedef void (*OnBaseSubscribe_SZSEL2_BondMbrInfo)(const SZSEL2_BondMbrInfo* pRealSZSEL2BondMbrInfo);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEL2_BondMbrInfo(OnBaseSubscribe_SZSEL2_BondMbrInfo cbOnBaseSubscribe_SZSEL2_BondMbrInfo);

/// 深交所L2债券交易主体信息订阅数据实时回调接口
/// @param  pRealSZSEL2BondInvestorInfo -- 实时数据
typedef void (*OnBaseSubscribe_SZSEL2_BondInvestorInfo)(const SZSEL2_BondInvestorInfo* pRealSZSEL2BondInvestorInfo);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEL2_BondInvestorInfo(OnBaseSubscribe_SZSEL2_BondInvestorInfo cbOnBaseSubscribe_SZSEL2_BondInvestorInfo);

/// 深交所L2债券交易员信息订阅数据实时回调接口
/// @param  pRealSZSEL2BondInvestorInfo -- 实时数据
typedef void (*OnBaseSubscribe_SZSEL2_BondTraderInfo)(const SZSEL2_BondTraderInfo* pRealSZSEL2BondTraderInfo);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEL2_BondTraderInfo(OnBaseSubscribe_SZSEL2_BondTraderInfo cbOnBaseSubscribe_SZSEL2_BondTraderInfo);

/// 深交所L1债券静态数据信息订阅数据实时回调接口
/// @param  pRealSZSEL1BondStatic -- 实时数据
typedef void (*OnBaseSubscribe_SZSEL1_BondStatic)(const SZSEL1_BondStatic* pRealSZSEL1BondStatic);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEL1_BondStatic(OnBaseSubscribe_SZSEL1_BondStatic cbOnBaseSubscribe_SZSEL1_BondStatic);

/// 深交所L1债券实时行情订阅数据实时回调接口
/// @param  pRealSZSEL1BondQuotation -- 实时数据
typedef void (*OnBaseSubscribe_SZSEL1_BondQuotation)(const SZSEL1_BondQuotation* pRealSZSEL1BondQuotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEL1_BondQuotation(OnBaseSubscribe_SZSEL1_BondQuotation cbOnBaseSubscribe_SZSEL1_BondQuotation);

/// 北交所L1静态数据订阅数据实时回调接口
/// @param  pRealBSEL1Static    -- 实时数据
typedef void (*OnBaseSubscribe_BSEL1_Static)(const BSEL1_Static* pRealBSEL1Static);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_BSEL1_Static(OnBaseSubscribe_BSEL1_Static cbOnBaseSubscribe_BSEL1_Static);

/// 北交所L1实时行情订阅数据实时回调接口
/// @param  pRealSZSEL1Quotation -- 实时数据
typedef void (*OnBaseSubscribe_BSEL1_Quotation)(const BSEL1_Quotation* pRealBSEL1Quotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_BSEL1_Quotation(OnBaseSubscribe_BSEL1_Quotation cbOnBaseSubscribe_BSEL1_Quotation);

/// 深交所L2债券市场竞买预约信息订阅数据实时回调接口
/// @param  pRealSZSEL2BondBidBookingInfo -- 实时数据
typedef void (*OnBaseSubscribe_SZSEL2_BondBidBookingInfo)(const SZSEL2_BondBidBookingInfo* pRealSZSEL2BondBidBookingInfo);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEL2_BondBidBookingInfo(OnBaseSubscribe_SZSEL2_BondBidBookingInfo cbOnBaseSubscribe_SZSEL2_BondBidBookingInfo);

/// 郑商所L2静态数据订阅数据实时回调接口
/// @param  pRealCZCEL2Static -- 实时数据
typedef void (*OnBaseSubscribe_CZCEL2_Static)(const CZCEL2_Static* pRealCZCEL2Static);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_CZCEL2_Static(OnBaseSubscribe_CZCEL2_Static cbOnBaseSubscribe_CZCEL2_Static);

/// 郑商所L2市场状态订阅数据实时回调接口
/// @param  pRealCZCEL2MktStatus -- 实时数据
typedef void (*OnBaseSubscribe_CZCEL2_MktStatus)(const CZCEL2_MktStatus* pRealCZCEL2MktStatus);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_CZCEL2_MktStatus(OnBaseSubscribe_CZCEL2_MktStatus cbOnBaseSubscribe_CZCEL2_MktStatus);

/// 郑商所L2品种状态订阅数据实时回调接口
/// @param  pRealCZCEL2Status -- 实时数据
typedef void (*OnBaseSubscribe_CZCEL2_Status)(const CZCEL2_Status* pRealCZCEL2Status);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_CZCEL2_Status(OnBaseSubscribe_CZCEL2_Status cbOnBaseSubscribe_CZCEL2_Status);

/// 郑商所L2实时行情订阅数据实时回调接口
/// @param  pRealCZCEL2Quotation -- 实时数据
typedef void (*OnBaseSubscribe_CZCEL2_Quotation)(const CZCEL2_Quotation* pRealCZCEL2Quotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_CZCEL2_Quotation(OnBaseSubscribe_CZCEL2_Quotation cbOnBaseSubscribe_CZCEL2_Quotation);

/// 郑商所L2五档深度委托行情订阅数据实时回调接口
/// @param  pRealCZCEL2LevelQuotation -- 实时数据
typedef void (*OnBaseSubscribe_CZCEL2_LevelQuotation)(const CZCEL2_LevelQuotation* pRealCZCEL2LevelQuotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_CZCEL2_LevelQuotation(OnBaseSubscribe_CZCEL2_LevelQuotation cbOnBaseSubscribe_CZCEL2_LevelQuotation);

/// 郑商所L2套利行情订阅数据回调接口
/// @param  pData -- 实时数据
typedef void (*OnBaseSubscribe_CZCEL2_ArbiQuotation)(const CZCEL2_ArbiQuotation* pData);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_CZCEL2_ArbiQuotation(OnBaseSubscribe_CZCEL2_ArbiQuotation cbOnBaseSubscribe_CZCEL2_ArbiQuotation);

/// 郑商所L2套利五档深度行情订阅数据回调接口
/// @param  pData -- 实时数据
typedef void (*OnBaseSubscribe_CZCEL2_ArbiLevelQuotation)(const CZCEL2_ArbiLevelQuotation* pData);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_CZCEL2_ArbiLevelQuotation(OnBaseSubscribe_CZCEL2_ArbiLevelQuotation cbOnBaseSubscribe_CZCEL2_ArbiLevelQuotation);/// 行情系统服务接口，连接服务器进行认证，订阅数据，查询快照.

/// 上交所L2精简实时行情订阅数据实时回调接口
/// @param  pRealSSEL2ShortQuotation -- 实时数据
typedef void (*OnBaseSubscribe_SSEL2_ShortQuotation)(const SSEL2_ShortQuotation* pRealSSEL2ShortQuotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SSEL2_ShortQuotation(OnBaseSubscribe_SSEL2_ShortQuotation cbOnBaseSubscribe_SSEL2_ShortQuotation);

/// 上交所L2委托队列订阅数据实时回调接口
/// @param  pRealSSEL2OrderQueue -- 实时数据
typedef void (*OnBaseSubscribe_SSEL2_OrderQueue)(const SSEL2_OrderQueue* pRealSSEL2OrderQueue);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SSEL2_OrderQueue(OnBaseSubscribe_SSEL2_OrderQueue cbOnBaseSubscribe_SSEL2_OrderQueue);

/// 深交所L2精简实时行情订阅数据实时回调接口
/// @param  pRealSZSEL2ShortQuotation -- 实时数据
typedef void (*OnBaseSubscribe_SZSEL2_ShortQuotation)(const SZSEL2_ShortQuotation* pRealSZSEL2ShortQuotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEL2_ShortQuotation(OnBaseSubscribe_SZSEL2_ShortQuotation cbOnBaseSubscribe_SZSEL2_ShortQuotation);

/// 深交所L2委托队列订阅数据实时回调接口
/// @param  pReal -- 实时数据
typedef void (*OnBaseSubscribe_SZSEL2_OrderQueue)(const SZSEL2_OrderQueue* pRealSZSEL2OrderQueue);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEL2_OrderQueue(OnBaseSubscribe_SZSEL2_OrderQueue cbOnBaseSubscribe_SZSEL2_OrderQueue);

/// 上交所L2 ETF统计数据订阅数据回调接口
/// @param  pData -- 实时数据
typedef void (*OnBaseSubscribe_SSEL2_ETF)(const SSEL2_ETF* pData);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SSEL2_ETF(OnBaseSubscribe_SSEL2_ETF cbOnBaseSubscribe_SSEL2_ETF);

/// 广期所L1静态数据订阅数据回调接口
/// @param  pData -- 实时数据
typedef void (*OnBaseSubscribe_GFEXL1_Static)(const GFEXL1_Static* pData);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_GFEXL1_Static(OnBaseSubscribe_GFEXL1_Static cbOnBaseSubscribe_GFEXL1_Static);
/// 广期所L1套利行情订阅数据回调接口
/// @param  pData -- 实时数据
typedef void (*OnBaseSubscribe_GFEXL1_ArbiQuotation)(const GFEXL1_ArbiQuotation* pData);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_GFEXL1_ArbiQuotation(OnBaseSubscribe_GFEXL1_ArbiQuotation cbOnBaseSubscribe_GFEXL1_ArbiQuotation);
/// 广期所L1实时行情订阅数据回调接口
/// @param  pData -- 实时数据
typedef void (*OnBaseSubscribe_GFEXL1_Quotation)(const GFEXL1_Quotation* pData);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_GFEXL1_Quotation(OnBaseSubscribe_GFEXL1_Quotation cbOnBaseSubscribe_GFEXL1_Quotation);
/// 广期所L1期权参数订阅数据回调接口
/// @param  pData -- 实时数据
typedef void (*OnBaseSubscribe_GFEXL1_OptionParameter)(const GFEXL1_OptionParameter* pData);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_GFEXL1_OptionParameter(OnBaseSubscribe_GFEXL1_OptionParameter cbOnBaseSubscribe_GFEXL1_OptionParameter);
/// 广期所L2静态数据订阅数据回调接口
/// @param  pData -- 实时数据
typedef void (*OnBaseSubscribe_GFEXL2_Static)(const GFEXL2_Static* pData);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_GFEXL2_Static(OnBaseSubscribe_GFEXL2_Static cbOnBaseSubscribe_GFEXL2_Static);
/// 广期所L2套利行情订阅数据回调接口
/// @param  pData -- 实时数据
typedef void (*OnBaseSubscribe_GFEXL2_ArbiQuotation)(const GFEXL2_ArbiQuotation* pData);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_GFEXL2_ArbiQuotation(OnBaseSubscribe_GFEXL2_ArbiQuotation cbOnBaseSubscribe_GFEXL2_ArbiQuotation);
/// 广期所L2实时行情订阅数据回调接口
/// @param  pData -- 实时数据
typedef void (*OnBaseSubscribe_GFEXL2_Quotation)(const GFEXL2_Quotation* pData);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_GFEXL2_Quotation(OnBaseSubscribe_GFEXL2_Quotation cbOnBaseSubscribe_GFEXL2_Quotation);
/// 广期所L2委托队列行情订阅数据回调接口
/// @param  pData -- 实时数据
typedef void (*OnBaseSubscribe_GFEXL2_OrderQueue)(const GFEXL2_OrderQueue* pData);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_GFEXL2_OrderQueue(OnBaseSubscribe_GFEXL2_OrderQueue cbOnBaseSubscribe_GFEXL2_OrderQueue);
/// 广期所L2委托统计行情订阅数据回调接口
/// @param  pData -- 实时数据
typedef void (*OnBaseSubscribe_GFEXL2_OrderStatistic)(const GFEXL2_OrderStatistic* pData);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_GFEXL2_OrderStatistic(OnBaseSubscribe_GFEXL2_OrderStatistic cbOnBaseSubscribe_GFEXL2_OrderStatistic);
/// 广期所L2实时结算价订阅数据回调接口
/// @param  pData -- 实时数据
typedef void (*OnBaseSubscribe_GFEXL2_RealTimePrice)(const GFEXL2_RealTimePrice* pData);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_GFEXL2_RealTimePrice(OnBaseSubscribe_GFEXL2_RealTimePrice cbOnBaseSubscribe_GFEXL2_RealTimePrice);
/// 广期所L2分价成交量行情订阅数据回调接口
/// @param  pData -- 实时数据
typedef void (*OnBaseSubscribe_GFEXL2_MarchPriceQty)(const GFEXL2_MarchPriceQty* pData);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_GFEXL2_MarchPriceQty(OnBaseSubscribe_GFEXL2_MarchPriceQty cbOnBaseSubscribe_GFEXL2_MarchPriceQty);
/// 广期所L2期权参数订阅数据回调接口
/// @param  pData -- 实时数据
typedef void (*OnBaseSubscribe_GFEXL2_OptionParameter)(const GFEXL2_OptionParameter* pData);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_GFEXL2_OptionParameter(OnBaseSubscribe_GFEXL2_OptionParameter cbOnBaseSubscribe_GFEXL2_OptionParameter);
/// 广期所L2套利委托队列订阅数据回调接口
/// @param  pData -- 实时数据
typedef void (*OnBaseSubscribe_GFEXL2_ArbiOrderQueue)(const GFEXL2_ArbiOrderQueue* pData);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_GFEXL2_ArbiOrderQueue(OnBaseSubscribe_GFEXL2_ArbiOrderQueue cbOnBaseSubscribe_GFEXL2_ArbiOrderQueue);

/// 上交所L2盘后固定价格逐笔成交订阅数据回调接口
/// @param  pData -- 实时数据
typedef void (*OnBaseSubscribe_SSEL2_FpTransaction)(const SSEL2_FpTransaction* pData);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SSEL2_FpTransaction(OnBaseSubscribe_SSEL2_FpTransaction cbOnBaseSubscribe_SSEL2_FpTransaction);
/// 上交所L2逐笔行情订阅数据回调接口
/// @param  pData -- 实时数据
typedef void (*OnBaseSubscribe_SSEL2_Tick)(const SSEL2_Tick* pData);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SSEL2_Tick(OnBaseSubscribe_SSEL2_Tick cbOnBaseSubscribe_SSEL2_Tick);

/// 大商所指数行情 订阅数据回调接口
/// @param  pData -- 实时数据
typedef void (*OnBaseSubscribe_DCE_Index)(const DCE_Index* pData);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_DCE_Index(OnBaseSubscribe_DCE_Index cbOnBaseSubscribe_DCE_Index);

/// 上交所L1盘后固定价格行情订阅数据回调接口
/// @param  pData -- 实时数据
typedef void (*OnBaseSubscribe_SSEL1_FpQuotation)(const SSEL1_FpQuotation* pData);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SSEL1_FpQuotation(OnBaseSubscribe_SSEL1_FpQuotation cbOnBaseSubscribe_SSEL1_FpQuotation);

/// 上交所L2盘后固定价格行情订阅数据回调接口
/// @param  pData -- 实时数据
typedef void (*OnBaseSubscribe_SSEL2_FpQuotation)(const SSEL2_FpQuotation* pData);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SSEL2_FpQuotation(OnBaseSubscribe_SSEL2_FpQuotation cbOnBaseSubscribe_SSEL2_FpQuotation);
/// 深交所L1盘后固定价格行情订阅数据回调接口
/// @param  pData -- 实时数据
typedef void (*OnBaseSubscribe_SZSEL1_FpQuotation)(const SZSEL1_FpQuotation* pData);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEL1_FpQuotation(OnBaseSubscribe_SZSEL1_FpQuotation cbOnBaseSubscribe_SZSEL1_FpQuotation);
/// 深交所L2盘后固定价格行情订阅数据回调接口
/// @param  pData -- 实时数据
typedef void (*OnBaseSubscribe_SZSEL2_FpQuotation)(const SZSEL2_FpQuotation* pData);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEL2_FpQuotation(OnBaseSubscribe_SZSEL2_FpQuotation cbOnBaseSubscribe_SZSEL2_FpQuotation);

/// 大商所指数行情行情订阅数据回调接口
/// @param  pData -- 实时数据
typedef void (*OnBaseSubscribe_DCE_Index)(const DCE_Index* pData);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_DCE_Index(OnBaseSubscribe_DCE_Index cbOnBaseSubscribe_DCE_Index);

/// 基础API环境初始化
void GTA_API_EXPORT _CDECL BaseInit();

/// 基础API环境反初始化
void GTA_API_EXPORT _CDECL BaseUninit();

/// 设置超时时间，设置即时生效，同步调用的超时时间
/// @param  nSenond     -- 超时时间，单位秒
void GTA_API_EXPORT _CDECL BaseSetTimeout(int nSecond = TIMEOUT_DEFAULT);

/// 设置心跳间隔时长，设置即时生效，
/// @param  nSenond     -- 心跳间隔时长，单位秒
void GTA_API_EXPORT _CDECL BaseSetHeartBeatTime(int nSecond = HEART_BEAT_INTERVAL);

/// API到行情网络延迟测试
/// @param  bSwitch     -- 开关,true 开启  false  关闭
/// @return RetCode     -- 错误代码, Ret_Success:成功;Ret_Error:失败(未订阅行情)
RetCode GTA_API_EXPORT _CDECL BaseDelayTest(bool bSwitch);

/// 注册行情服务器地址，可注册多个不同地址，实现高可用，连接顺序为顺序连接，断线时先重连当前使用地址
/// @param  pIP         -- 服务器地址.
/// @param  uPort       -- 服务器端口.
/// @return RetCode     -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseRegisterService(const char* pIP, unsigned short uPort);

/// 设置深交所L2 Tick数据多线程回调
/// @param  bMTCallBack     -- 开关,true 开启  false  关闭
void GTA_API_EXPORT _CDECL BaseSetSZL2TickMTCallBack(bool bMTCallBack = false);

/// 连接行情服务器，发送用户认证信息，同步请求
/// @param  pUserName   -- 账号名称
/// @param  pPassword   -- 账号密码，加密后密码
/// @return RetCode     -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseLogin(const char* pUserName, const char* pPassword);

/// 连接行情服务器，发送用户认证信息，同步请求
/// @param  pUserName   -- 账号名称
/// @param  pPassword   -- 账号密码，加密后密码
/// @param  pOptions    -- 可选附加参数，可填充null。当有多个参数时，用逗号（，）分隔
///                        标识        可选值       说明
///                        NetType     0，1         连接网络类型，0表示外网(默认值)，1表示特殊网络
///                        示例，如："NetType=1"
/// @return RetCode     -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseLoginX(const char* pUserName, const char* pPassword, const char* pOptions);

/// 获取行情订阅服务器地址信息
/// @param  ppServerAddrs               -- 存放服务器地址对象，需调用BaseReleaseValue释放内存
/// @param  pSize                       -- 返回数据个数
/// @return RetCode      -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseGetServerAddr(ServerState** ppServerAddrs, int* pSize);

/// 获取有权限的数据类型.
/// @param  ppDataTypeList  -- 数据类型指针，数据大小即为有权限的数据个数，需调用BaseReleaseValue释放内存
/// @param  pSize           -- 返回数据个数
/// @return RetCode         -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseGetMsgTypeList(MsgType** ppDataTypeList, int* pSize);

/// 订阅行情，重复订阅同一个数据多次时取并集
/// @param  msgType     -- 需要订阅的数据类型
/// @param  pCodeList   -- 个股订阅列表，以“,”分割，末尾必须以\0结束，为NULL时订阅全市场
/// @return RetCode     -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseSubscribe(MsgType msgType, char* pCodeList = NULL);

/// 取消订阅
/// @param  msgType     -- 需要取消订阅的数据类型
/// @param  pCodeList   -- 取消个股列表，以“,”分割，末尾必须以\0结束，为NULL时取消所有代码
/// @return RetCode     -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseUnsubscribe(MsgType msgType, char* pCodeList = NULL);

/// 取消所有订阅信息
/// @return RetCode     -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseUnsubscribeAll();

/// 获取股票列表
/// @param  pMarketTag  -- 市场标识，以“,”分割多个市场标识，末尾必须以\0结束，不允许为NULL
/// @param  ppStockList -- 返回的个股代码列表，带市场标志，需调用BaseReleaseValue释放内存
/// @param  pSize       -- 返回数据个数
/// @return RetCode     -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseGetStockList(char* pMarketTag, StockSymbol** ppStockList, int* pSize);

/// 查询上交所L1静态数据
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapSSEL1Static     -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode              -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SSEL1_Static(char* pCodeList, SSEL1_Static** ppSnapSSEL1Static, int* pSize);

/// 查询上交所L1实时行情
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapSSEQuotation     -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize                       -- 返回数据个数
/// @return RetCode              -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SSEL1_Quotation(char* pCodeList, SSEL1_Quotation** ppSnapSSEQuotation, int* pSize);

/// 查询上交所指数通数据
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  SnapSSEIndexPress   -- 快照数据列表
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SSE_IndexPress(char* pCodeList, SSE_IndexPress** ppSnapSSEIndexPress, int* pSize);

/// 查询上交所L2静态数据
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapSSEStatic        -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize                       -- 返回数据个数
/// @return RetCode              -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SSEL2_Static(char* pCodeList, SSEL2_Static** ppSnapSSEL2Static, int* pSize);

/// 查询上交所L2实时行情快照
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapSSEL2Quotation    -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize                       -- 返回数据个数
/// @return RetCode              -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SSEL2_Quotation(char* pCodeList, SSEL2_Quotation** ppSnapSSEL2Quotation, int* pSize);

/// 查询上交所L2指数行情
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapSSEL2Index        -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize                       -- 返回数据个数
/// @return RetCode              -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SSEL2_Index(char* pCodeList, SSEL2_Index** ppSnapSSEL2Index, int* pSize);

/// 查询上交所L2逐笔成交
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapSSEL2Transation   -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize                       -- 返回数据个数
/// @return RetCode              -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SSEL2_Transaction(char* pCodeList, SSEL2_Transaction** ppSnapSSEL2Transation, int* pSize);

/// 查询上交所L2虚拟集合竞价
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapSSEL2Auction    -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize                       -- 返回数据个数
/// @return RetCode              -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SSEL2_Auction(char* pCodeList, SSEL2_Auction** ppSnapSSEL2Auction, int* pSize);

/// 查询上交所L2市场总览
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapSSEL2Overview   -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize                       -- 返回数据个数
/// @return RetCode              -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SSEL2_Overview(char* pCodeList, SSEL2_Overview** ppSnapSSEL2Overview, int* pSize);

/// 查询上交所个股期权静态数据
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapSSEIOL1Static   -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize                       -- 返回数据个数
/// @return RetCode              -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SSEIOL1_Static(char* pCodeList, SSEIOL1_Static** ppSnapSSEIOL1Static, int* pSize);

/// 查询上交所个股期权实时行情
/// @param  pCodeList               -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapSSEIOL1Quotation    -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize                       -- 返回数据个数
/// @return RetCode                 -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SSEIOL1_Quotation(char* pCodeList, SSEIOL1_Quotation** ppSnapSSEIOL1Quotation, int* pSize);

/// 查询上交所个股期权L1组合策略
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapSSEIOL1Strategy    -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize                       -- 返回数据个数
/// @return RetCode              -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SSEIOL1_Strategy(char* pCodeList, SSEIOL1_Strategy** ppSnapSSEIOL1Strategy, int* pSize);

/// 查询深交所L1静态数据
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapSZSEL1Static    -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize                       -- 返回数据个数
/// @return RetCode              -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEL1_Static(char* pCodeList, SZSEL1_Static** ppSnapSZSEL1Static, int* pSize);

/// 查询深交所L1实时行情
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapSZSEL1Quotation -- 快照数据列表，需调用BaseReleaseBulletinValue释放内存
/// @param  pSize                       -- 返回数据个数
/// @return RetCode              -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEL1_Quotation(char* pCodeList, SZSEL1_Quotation** ppSnapSZSEL1Quotation, int* pSize);

/// 查询深交所L1公告
/// @param  pCodeList              -- 快照订阅列表，只能为NULL，输入将被忽略
/// @param  ppSnapSZSEL1Bullet -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize                       -- 返回数据个数
/// @return RetCode              -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEL1_Bulletin(char* pCodeList, SZSEL1_Bulletin** ppSnapSZSEL1Bullet, int* pSize);

/// 查询深交所个股期权L1静态数据
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapSZSEIOL1Static    -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize                       -- 返回数据个数
/// @return RetCode              -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEIOL1_Static(char* pCodeList, SZSEIOL1_Static** ppSnapSZSEIOL1Static, int* pSize);

/// 查询深交所个股期权L1实时行情
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapSZSEIOL1Quotation    -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize                       -- 返回数据个数
/// @return RetCode              -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEIOL1_Quotation(char* pCodeList, SZSEIOL1_Quotation** ppSnapSZSEIOL1Quotation, int* pSize);

/// 查询深交所个股期权L1证券状态
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapSZSEIOL1Status    -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize                       -- 返回数据个数
/// @return RetCode              -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEIOL1_Status(char* pCodeList, SZSEIOL1_Status** ppSnapSZSEIOL1Status, int* pSize);

/// 查询深交所个股期权L1组合策略
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapSZSEIOL1Strategy    -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize                       -- 返回数据个数
/// @return RetCode              -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEIOL1_Strategy(char* pCodeList, SZSEIOL1_Strategy** ppSnapSZSEIOL1Strategy, int* pSize);



/// 查询深交所L2静态数据
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapSZSEL2Static    -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize                       -- 返回数据个数
/// @return RetCode              -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEL2_Static(char* pCodeList, SZSEL2_Static** ppSnapSZSEL2Static, int* pSize);

/// 查询深交所L2实时行情
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapSZSEL2Quotation    -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize                       -- 返回数据个数
/// @return RetCode              -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEL2_Quotation(char* pCodeList, SZSEL2_Quotation** ppSnapSZSEL2Quotation, int* pSize);

/// 查询深交所L2证券状态
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapSZSEL2StockStatus  -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize                       -- 返回数据个数
/// @return RetCode              -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEL2_Status(char* pCodeList, SZSEL2_Status** ppSnapSZSEL2StockStatus, int* pSize);

/// 查询深交所L2指数行情
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapSZSEL2Index        -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize                       -- 返回数据个数
/// @return RetCode              -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEL2_Index(char* pCodeList, SZSEL2_Index** ppSnapSZSEL2Index, int* pSize);

/// 查询深交所L2逐笔委托
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapSZSEL2Order        -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize                       -- 返回数据个数
/// @return RetCode              -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEL2_Order(char* pCodeList, SZSEL2_Order** ppSnapSZSEL2Order, int* pSize);

/// 查询深交所L2逐笔成交
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapSZSEL2Transaction  -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize                       -- 返回数据个数
/// @return RetCode              -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEL2_Transaction(char* pCodeList, SZSEL2_Transaction** ppSnapSZSEL2Transaction, int* pSize);

/// 查询深交所L2逐笔
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapSZSEL2Tick  -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize                       -- 返回数据个数
/// @return RetCode              -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEL2_Tick(char* pCodeList, SZSEL2_Tick** ppSnapSZSEL2Tick, int* pSize);

/// 查询中金所静态数据
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapCFFEXL2Static       -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize                       -- 返回数据个数
/// @return RetCode              -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_CFFEXL2_Static(char* pCodeList, CFFEXL2_Static** ppSnapCFFEXL2Static, int* pSize);

/// 查询中金所L2实时行情
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapCFFEXL2Quotation    -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize                       -- 返回数据个数
/// @return RetCode              -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_CFFEXL2_Quotation(char* pCodeList, CFFEXL2_Quotation** ppSnapCFFEXL2Quotation, int* pSize);


/// 查询郑商所静态数据
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapCZCEL1Static    -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize                       -- 返回数据个数
/// @return RetCode              -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_CZCEL1_Static(char* pCodeList, CZCEL1_Static** ppSnapCZCEL1Static, int* pSize);

/// 查询郑商所实时行情
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapCZCEL1Quotation -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize                       -- 返回数据个数
/// @return RetCode              -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_CZCEL1_Quotation(char* pCodeList, CZCEL1_Quotation** ppSnapCZCEL1Quotation, int* pSize);

/// 查询郑商所套利行情
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapCZCEL1ArbiQuotation -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize                       -- 返回数据个数
/// @return RetCode              -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_CZCEL1_ArbiQuotation(char* pCodeList, CZCEL1_ArbiQuotation** ppSnapCZCEL1ArbiQuotation, int* pSize);

/// 查询郑商所L1市场状态
/// @param  pCodeList				-- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapCZCEL1MktStatus	-- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize                   -- 返回数据个数
/// @return RetCode					-- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_CZCEL1_MktStatus(char* pCodeList, CZCEL1_MktStatus** ppSnapCZCEL1MktStatus, int* pSize);

/// 查询郑商所L1品种状态
/// @param  pCodeList				-- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapCZCEL1Status		-- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize					-- 返回数据个数
/// @return RetCode					-- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_CZCEL1_Status(char* pCodeList, CZCEL1_Status** ppSnapCZCEL1Status, int* pSize);

/// 查询易盛指数行情
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapESUNNY_Index -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize                       -- 返回数据个数
/// @return RetCode              -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_ESUNNY_Index(char* pCodeList, ESUNNY_Index** ppSnapESUNNY_Index, int* pSize);

/// 查询大商所L1静态数据
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapDCEL1Static     -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize                       -- 返回数据个数
/// @return RetCode              -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_DCEL1_Static(char* pCodeList, DCEL1_Static** ppSnapDCEL1Static, int* pSize);

/// 查询大商所L1市场状态
/// @param  pCodeList                      -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapDCEL1MktStatus           -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize                          -- 返回数据个数
/// @return RetCode                        -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_DCEL1_MktStatus(char* pCodeList, DCEL1_MktStatus** ppSnapDCEL1MktStatus, int* pSize);

/// 查询大商所L1交易状态
/// @param  pCodeList                      -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapDCEL1Status              -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize                          -- 返回数据个数
/// @return RetCode                        -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_DCEL1_Status(char* pCodeList, DCEL1_Status** ppSnapDCEL1Status, int* pSize);

/// 查询大商所L1期权参数
/// @param  pCodeList                      -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapDCEL1OptionParameter     -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize                          -- 返回数据个数
/// @return RetCode                        -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_DCEL1_OptionParameter(char* pCodeList, DCEL1_OptionParameter** ppSnapDCEL1OptionParameter, int* pSize);

/// 查询大商所L1实时行情
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapDCEL1Quotation  -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize                       -- 返回数据个数
/// @return RetCode              -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_DCEL1_Quotation(char* pCodeList, DCEL1_Quotation** ppSnapDCEL1Quotation, int* pSize);

/// 查询大商所L1套利行情
/// @param  pCodeList               -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapDCEL1ArbiQuotation  -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize                       -- 返回数据个数
/// @return RetCode                 -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_DCEL1_ArbiQuotation(char* pCodeList, DCEL1_ArbiQuotation** ppSnapDCEL1ArbiQuotation, int* pSize);

/// 查询大商所L2静态数据
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapDCEL2Static     -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize                       -- 返回数据个数
/// @return RetCode              -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_DCEL2_Static(char* pCodeList, DCEL2_Static** ppSnapDCEL2Static, int* pSize);

/// 查询大商所L2实时行情
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapDCEL2Quotation  -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize                       -- 返回数据个数
/// @return RetCode              -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_DCEL2_Quotation(char* pCodeList, DCEL2_Quotation** ppSnapDCEL2Quotation, int* pSize);

/// 查询大商所L2套利行情
/// @param  pCodeList               -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapDCEL2ArbiQuotation  -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize                       -- 返回数据个数
/// @return RetCode                 -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_DCEL2_ArbiQuotation(char* pCodeList, DCEL2_ArbiQuotation** ppSnapDCEL2ArbiQuotation, int* pSize);

/// 查询大商所L2实时结算价
/// @param  pCodeList               -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapDCEL2RealTimePrice  -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize                       -- 返回数据个数
/// @return RetCode                 -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_DCEL2_RealTimePrice(char* pCodeList, DCEL2_RealTimePrice** ppSnapDCEL2RealTimePrice, int* pSize);

/// 查询大商所L2委托统计行情
/// @param  pCodeList               -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapDCEL2OrderStatistic -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize                       -- 返回数据个数
/// @return RetCode                 -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_DCEL2_OrderStatistic(char* pCodeList, DCEL2_OrderStatistic** ppSnapDCEL2OrderStatistic, int* pSize);

/// 查询大商所L2分价成交量行情
/// @param  pCodeList               -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapDCEL2MarchPriceQty  -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize                   -- 返回数据个数
/// @return RetCode                 -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_DCEL2_MarchPriceQty(char* pCodeList, DCEL2_MarchPriceQty** ppSnapDCEL2MarchPriceQty, int* pSize);

/// 查询大商所L2市场状态
/// @param  pCodeList                      -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapDCEL2MktStatus           -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize                          -- 返回数据个数
/// @return RetCode                        -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_DCEL2_MktStatus(char* pCodeList, DCEL2_MktStatus** ppSnapDCEL2MktStatus, int* pSize);

/// 查询大商所L2交易状态
/// @param  pCodeList                      -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapDCEL2Status              -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize                          -- 返回数据个数
/// @return RetCode                        -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_DCEL2_Status(char* pCodeList, DCEL2_Status** ppSnapDCEL2Status, int* pSize);

/// 查询大商所L2期权参数
/// @param  pCodeList                      -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapDCEL2OptionParameter     -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize                          -- 返回数据个数
/// @return RetCode                        -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_DCEL2_OptionParameter(char* pCodeList, DCEL2_OptionParameter** ppSnapDCEL2OptionParameter, int* pSize);

/// 查询大商所L2五档深度行情
/// @param  pCodeList                      -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapDCEL2LevelQuotation      -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize                          -- 返回数据个数
/// @return RetCode                        -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_DCEL2_LevelQuotation(char* pCodeList, DCEL2_LevelQuotation** ppSnapDCEL2LevelQuotation, int* pSize);

/// 查询大商所L2委托队列行情
/// @param  pCodeList                      -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapDCEL2OrderQueue     -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize                          -- 返回数据个数
/// @return RetCode                        -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_DCEL2_OrderQueue(char* pCodeList, DCEL2_OrderQueue** ppSnapDCEL2OrderQueue, int* pSize);


/// 查询上期所静态数据
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapSHFEL1Static    -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SHFEL1_Static(char* pCodeList, SHFEL1_Static** ppSnapSHFEL1Static, int* pSize);

/// 查询上期所实时行情
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapSHFEL1Quotation -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SHFEL1_Quotation(char* pCodeList, SHFEL1_Quotation** ppSnapSHFEL1Quotation, int* pSize);

/// 查询上期能源静态数据
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapINEL1Static    -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_INEL1_Static(char* pCodeList, INEL1_Static** ppSnapINEL1Static, int* pSize);

/// 查询上期能源实时行情
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapINEL1Quotation -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_INEL1_Quotation(char* pCodeList, INEL1_Quotation** ppSnapINEL1Quotation, int* pSize);


//上期二代查询快照
/// 查询上期所L2静态数据
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapSHFEL2Static    -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SHFEL2_Static(char* pCodeList, SHFEL2_Static** ppSnapSHFEL2Static, int* pSize);

/// 查询上期所L2实时行情
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapSHFEL2Quotation -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SHFEL2_Quotation(char* pCodeList, SHFEL2_Quotation** ppSnapSHFEL2Quotation, int* pSize);

/// 查询上期能源L2静态数据
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapINEL2Static    -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_INEL2_Static(char* pCodeList, INEL2_Static** ppSnapINEL2Static, int* pSize);

/// 查询上期能源L2实时行情
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapINEL2Quotation -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_INEL2_Quotation(char* pCodeList, INEL2_Quotation** ppSnapINEL2Quotation, int* pSize);

/// 查询港交所实时行情
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapHKEXL2Quotation -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_HKEXL2_Quotation(char* pCodeList, HKEXL2_Quotation** ppSnapHKEXL2Quotation, int* pSize);

/// 查询港交所静态数据
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapHKEXL2Static  -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_HKEXL2_Static(char* pCodeList, HKEXL2_Static** ppSnapHKEXL2Static, int* pSize);

/// 查询港交所经纪人队列
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapHKEXL2BrokerQueue    -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_HKEXL2_BrokerQueue(char* pCodeList, HKEXL2_BrokerQueue** ppSnapHKEXL2BrokerQueue, int* pSize);

/// 查询港交所指数行情
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapHKEXL2Index   -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_HKEXL2_Index(char* pCodeList, HKEXL2_Index** ppSnapHKEXL2Index, int* pSize);

/// 查询港交所市场总览
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapHKEXL2Overview  -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_HKEXL2_Overview(char* pCodeList, HKEXL2_Overview** ppSnapHKEXL2Overview, int* pSize);

/// 查询深港通静态数据
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapHKSZL1Static  -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_HKSZL1_Static(char* pCodeList, HKSZL1_Static** ppSnapHKSZL1Static, int* pSize);

/// 查询深港通实时行情
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapHKSZL1Quotation -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_HKSZL1_Quotation(char* pCodeList, HKSZL1_Quotation** ppSnapHKSZL1Quotation, int* pSize);

/// 查询沪港通静态数据
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapHKSHL1Static  -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_HKSHL1_Static(char* pCodeList, HKSHL1_Static** ppSnapHKSHL1Static, int* pSize);

/// 查询沪港通实时行情
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapHKSHL1Quotation -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_HKSHL1_Quotation(char* pCodeList, HKSHL1_Quotation** ppSnapHKSHL1Quotation, int* pSize);

/// 查询港交所L2互联市场日余额
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapHKEXL2DQB -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_HKEXL2_DQB(char* pCodeList, HKEXL2_DQB** ppSnapHKEXL2DQB, int* pSize);

/// 查询港交所L2资金流向
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapHKEXL2MoneyFlow -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_HKEXL2_MoneyFlow(char* pCodeList, HKEXL2_MoneyFlow** ppSnapHKEXL2MoneyFlow, int* pSize);

/// 查询沪港通L1市场总览
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapHKEXL2MoneyFlow -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_HKSHL1_Overview(char* pCodeList, HKSHL1_Overview** ppSnapHKSHL1Overview, int* pSize);

/// 查询深港通L1市场总览
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapHKSZL1Overview -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_HKSZL1_Overview(char* pCodeList, HKSZL1_Overview** ppSnapHKSZL1Overview, int* pSize);

/// 查询上交所L2逐笔委托
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapSSEL2Order    -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SSEL2_Order(char* pCodeList, SSEL2_Order** ppSnapSSEL2Order, int* pSize);

/// 查询深交所L2国证指数
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapSZSEL2Cnindex -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEL2_Cnindex(char* pCodeList, SZSEL2_Cnindex** ppSnapSZSEL2Cnindex, int* pSize);

/// 查询上交所L2债券静态数据
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapSSEL2BondStatic -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SSEL2_BondStatic(char* pCodeList, SSEL2_BondStatic** ppSnapSSEL2BondStatic, int* pSize);

/// 查询上交所L2债券实时行情
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapSSEL2BondQuotation -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SSEL2_BondQuotation(char* pCodeList, SSEL2_BondQuotation** ppSnapSSEL2BondQuotation, int* pSize);

/// 查询上交所L2债券逐笔数据
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapSSEL2BondTick -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SSEL2_BondTick(char* pCodeList,SSEL2_BondTick** ppSnapSSEL2BondTick, int* pSize);

/// 查询上交所L2债券市场总览
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapSSEL2BondOverview -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SSEL2_BondOverview(char* pCodeList, SSEL2_BondOverview** ppSnapSSEL2BondOverview, int* pSize);

/// 查询上交所L1债券静态数据
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapSSEL1BondStatic -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SSEL1_BondStatic(char* pCodeList, SSEL1_BondStatic** ppSnapSSEL1BondStatic, int* pSize);

/// 查询上交所L1债券实时行情
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapSSEL1BondQuotation -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SSEL1_BondQuotation(char* pCodeList, SSEL1_BondQuotation** ppSnapSSEL1BondQuotation, int* pSize);

/// 查询深交所L2债券静态数据
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapSZSEL2BondStatic -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEL2_BondStatic(char* pCodeList, SZSEL2_BondStatic** ppSnapSZSEL2BondStatic, int* pSize);

/// 查询深交所L2债券实时行情
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapSZSEL2BondQuotation -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEL2_BondQuotation(char* pCodeList, SZSEL2_BondQuotation** ppSnapSZSEL2BondQuotation, int* pSize);

/// 查询深交所L2债券逐笔成交
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapSZSEL2BondTransaction -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEL2_BondTransaction(char* pCodeList, SZSEL2_BondTransaction** ppSnapSZSEL2BondTransaction, int* pSize);

/// 查询深交所L2债券逐笔委托
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapSZSEL2BondOrder -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEL2_BondOrder(char* pCodeList, SZSEL2_BondOrder** ppSnapSZSEL2BondOrder, int* pSize);

/// 查询深交所L2债券逐比消息
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapSZSEL2BondTick -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEL2_BondTick(char* pCodeList, SZSEL2_BondTick** ppSnapSZSEL2BondTick, int* pSize);

/// 查询深交所L2债券交易商信息
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapSZSEL2BondMbrInfo -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEL2_BondMbrInfo(char* pCodeList, SZSEL2_BondMbrInfo** ppSnapSZSEL2BondMbrInfo, int* pSize);

/// 查询深交所L2债券交易主体信息
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapSZSEL2BondMbrInfo -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEL2_BondInvestorInfo(char* pCodeList, SZSEL2_BondInvestorInfo** ppSnapSZSEL2BondInvestorInfo, int* pSize);

/// 查询深交所L2债券交易员信息
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapSZSEL2BondTraderInfo -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEL2_BondTraderInfo(char* pCodeList, SZSEL2_BondTraderInfo** ppSnapSZSEL2BondTraderInfo, int* pSize);

/// 查询深交所L1债券静态数据
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapSZSEL1BondStatic -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEL1_BondStatic(char* pCodeList, SZSEL1_BondStatic** ppSnapSZSEL1BondStatic, int* pSize);

/// 查询深交所L1债券实时行情
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapSZSEL1BondQuotation -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEL1_BondQuotation(char* pCodeList, SZSEL1_BondQuotation** ppSnapSZSEL1BondQuotation, int* pSize);

/// 查询北交所L1静态数据
/// @param  pCodeList          -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapBSEL1Static -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize              -- 返回数据个数
/// @return RetCode            -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_BSEL1_Static(char* pCodeList, BSEL1_Static** ppSnapBSEL1Static, int* pSize);

/// 查询北交所L1实时行情
/// @param  pCodeList            -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapBSEL1Quotation -- 快照数据列表，需调用BaseReleaseBulletinValue释放内存
/// @param  pSize                -- 返回数据个数
/// @return RetCode              -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_BSEL1_Quotation(char* pCodeList, BSEL1_Quotation** ppSnapBSEL1Quotation, int* pSize);

/// 查询深交所L2债券市场竞买预约信息
/// @param  pCodeList                      -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapSZSEL2BondBidBookingInfo -- 快照数据列表，需调用BaseReleaseBulletinValue释放内存
/// @param  pSize                -- 返回数据个数
/// @return RetCode              -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEL2_BondBidBookingInfo(char* pCodeList, SZSEL2_BondBidBookingInfo** ppSnapSZSEL2BondBidBookingInfo, int* pSize);

/// 查询郑商所L2静态数据
/// @param  pCodeList                      -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapCZCEL2Static -- 快照数据列表，需调用BaseReleaseBulletinValue释放内存
/// @param  pSize                -- 返回数据个数
/// @return RetCode              -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_CZCEL2_Static(char* pCodeList, CZCEL2_Static** ppSnapCZCEL2Static, int* pSize);

/// 查询郑商所L2市场状态
/// @param  pCodeList                      -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapCZCEL2MktStatus -- 快照数据列表，需调用BaseReleaseBulletinValue释放内存
/// @param  pSize                -- 返回数据个数
/// @return RetCode              -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_CZCEL2_MktStatus(char* pCodeList, CZCEL2_MktStatus** ppSnapCZCEL2MktStatus, int* pSize);

/// 查询郑商所L2品种状态
/// @param  pCodeList                      -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapCZCEL2Status -- 快照数据列表，需调用BaseReleaseBulletinValue释放内存
/// @param  pSize                -- 返回数据个数
/// @return RetCode              -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_CZCEL2_Status(char* pCodeList, CZCEL2_Status** ppSnapCZCEL2Status, int* pSize);

/// 查询郑商所L2实时行情
/// @param  pCodeList                      -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapCZCEL2Quotation -- 快照数据列表，需调用BaseReleaseBulletinValue释放内存
/// @param  pSize                -- 返回数据个数
/// @return RetCode              -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_CZCEL2_Quotation(char* pCodeList, CZCEL2_Quotation** ppSnapCZCEL2Quotation, int* pSize);

/// 查询郑商所L2五档深度委托行情
/// @param  pCodeList                      -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapCZCEL2LevelQuotation -- 快照数据列表，需调用BaseReleaseBulletinValue释放内存
/// @param  pSize                -- 返回数据个数
/// @return RetCode              -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_CZCEL2_LevelQuotation(char* pCodeList, CZCEL2_LevelQuotation** ppSnapCZCEL2LevelQuotation, int* pSize);

/// 查询郑商所L2套利行情快照
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppData -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_CZCEL2_ArbiQuotation(char* pCodeList, CZCEL2_ArbiQuotation** ppData, int* pSize);

/// 查询郑商所L2套利五档深度行情快照
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppData -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_CZCEL2_ArbiLevelQuotation(char* pCodeList, CZCEL2_ArbiLevelQuotation** ppData, int* pSize);

/// 查询上交所L2精简实时行情
/// @param  pCodeList                      -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapSSEL2ShortQuotation -- 快照数据列表，需调用BaseReleaseBulletinValue释放内存
/// @param  pSize                -- 返回数据个数
/// @return RetCode              -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SSEL2_ShortQuotation(char* pCodeList, SSEL2_ShortQuotation** ppSnapSSEL2ShortQuotation, int* pSize);

/// 查询上交所L2委托队列
/// @param  pCodeList                      -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapSSEL2OrderQueue -- 快照数据列表，需调用BaseReleaseBulletinValue释放内存
/// @param  pSize                -- 返回数据个数
/// @return RetCode              -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SSEL2_OrderQueue(char* pCodeList, SSEL2_OrderQueue** ppSnapSSEL2OrderQueue, int* pSize);

/// 查询深交所L2精简实时行情
/// @param  pCodeList                      -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapSZSEL2ShortQuotation -- 快照数据列表，需调用BaseReleaseBulletinValue释放内存
/// @param  pSize                -- 返回数据个数
/// @return RetCode              -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEL2_ShortQuotation(char* pCodeList, SZSEL2_ShortQuotation** ppSnapSZSEL2ShortQuotation, int* pSize);

/// 查询深交所L2委托队列
/// @param  pCodeList                      -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppSnapSZSEL2OrderQueue -- 快照数据列表，需调用BaseReleaseBulletinValue释放内存
/// @param  pSize                -- 返回数据个数
/// @return RetCode              -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEL2_OrderQueue(char* pCodeList, SZSEL2_OrderQueue** ppSnapSZSEL2OrderQueue, int* pSize);

/// 查询上交所L2 ETF统计数据快照
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppData -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SSEL2_ETF(char* pCodeList, SSEL2_ETF** ppData, int* pSize);

/// 查询广期所L1静态数据快照
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppData -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_GFEXL1_Static(char* pCodeList, GFEXL1_Static** ppData, int* pSize);
/// 查询广期所L1套利行情快照
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppData -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_GFEXL1_ArbiQuotation(char* pCodeList, GFEXL1_ArbiQuotation** ppData, int* pSize);
/// 查询广期所L1实时行情快照
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppData -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_GFEXL1_Quotation(char* pCodeList, GFEXL1_Quotation** ppData, int* pSize);
/// 查询广期所L1期权参数快照
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppData -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_GFEXL1_OptionParameter(char* pCodeList, GFEXL1_OptionParameter** ppData, int* pSize);
/// 查询广期所L2静态数据快照
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppData -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_GFEXL2_Static(char* pCodeList, GFEXL2_Static** ppData, int* pSize);
/// 查询广期所L2套利行情快照
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppData -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_GFEXL2_ArbiQuotation(char* pCodeList, GFEXL2_ArbiQuotation** ppData, int* pSize);
/// 查询广期所L2实时行情快照
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppData -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_GFEXL2_Quotation(char* pCodeList, GFEXL2_Quotation** ppData, int* pSize);
/// 查询广期所L2委托队列行情快照
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppData -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_GFEXL2_OrderQueue(char* pCodeList, GFEXL2_OrderQueue** ppData, int* pSize);
/// 查询广期所L2委托统计行情快照
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppData -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_GFEXL2_OrderStatistic(char* pCodeList, GFEXL2_OrderStatistic** ppData, int* pSize);
/// 查询广期所L2实时结算价快照
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppData -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_GFEXL2_RealTimePrice(char* pCodeList, GFEXL2_RealTimePrice** ppData, int* pSize);
/// 查询广期所L2分价成交量行情快照
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppData -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_GFEXL2_MarchPriceQty(char* pCodeList, GFEXL2_MarchPriceQty** ppData, int* pSize);
/// 查询广期所L2期权参数快照
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppData -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_GFEXL2_OptionParameter(char* pCodeList, GFEXL2_OptionParameter** ppData, int* pSize);

/// 查询广期所L2套利委托队列快照
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppData -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_GFEXL2_ArbiOrderQueue(char* pCodeList, GFEXL2_ArbiOrderQueue** ppData, int* pSize);
/// 查询上交所L2盘后固定价格逐笔成交快照
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppData -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SSEL2_FpTransaction(char* pCodeList, SSEL2_FpTransaction** ppData, int* pSize);
/// 查询上交所L2逐笔行情快照
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppData -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SSEL2_Tick(char* pCodeList, SSEL2_Tick** ppData, int* pSize);

/// 查询大商所指数行情 快照
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppData -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_DCE_Index(char* pCodeList, DCE_Index** ppData, int* pSize);

/// 查询上交所L1盘后固定价格行情快照
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppData -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SSEL1_FpQuotation(char* pCodeList, SSEL1_FpQuotation** ppData, int* pSize);

/// 查询上交所L2盘后固定价格行情快照
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppData -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SSEL2_FpQuotation(char* pCodeList, SSEL2_FpQuotation** ppData, int* pSize);
/// 查询深交所L1盘后固定价格行情快照
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppData -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEL1_FpQuotation(char* pCodeList, SZSEL1_FpQuotation** ppData, int* pSize);
/// 查询深交所L2盘后固定价格行情快照
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppData -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEL2_FpQuotation(char* pCodeList, SZSEL2_FpQuotation** ppData, int* pSize);
/// 查询大商所指数行情快照
/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
/// @param  ppData -- 快照数据列表，需调用BaseReleaseValue释放内存
/// @param  pSize               -- 返回数据个数
/// @return RetCode             -- 错误代码
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_DCE_Index(char* pCodeList, DCE_Index** ppData, int* pSize);

/// @param  ppDataValue         -- 需释放内存
void GTA_API_EXPORT _CDECL BaseReleaseValue(void** ppDataValue);
};

#endif // GTA_QTS_INTERFACE_BASE_C_H_
