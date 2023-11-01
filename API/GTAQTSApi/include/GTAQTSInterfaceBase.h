//////////////////////////////////////////////////////////////////////////////
/// @file       GTAQTSInterfaceBase.h 
/// @brief      QTS行情接口定义，按结构体返回数据，自动重连
/// @copyright  Copyright (C), 2008-2023, CSMAR Information Tech. Co., Ltd.
/// @version    3.22
/// @date       2022.12.08
//////////////////////////////////////////////////////////////////////////////
#ifndef GTA_QTS_INTERFACE_BASE_H_
#define GTA_QTS_INTERFACE_BASE_H_
#include "QTSDataType.h"
#include "QTSStruct.h"
#include "QTSDataFieldDefine.h"

/// 实时数据、连接状态回调接口，按结构体返回数据
/// API内部使用多个线程进行数据解析，此处需注意多线程问题。状态回调单独一个线程。
/// 线程分配 数据量大的消息独立成一个线程，如沪深逐笔委托，逐笔成交
class GTA_API_EXPORT IGTAQTSCallbackBase
{
public:
    virtual ~IGTAQTSCallbackBase(){}
    /// 登陆状态返回
    /// @param  errCode  -- 失败原因(用户过期、重复登陆、同账号同时异地登录，前次登录账号所在API被踢)用户需对这两个进行处理
    virtual void OnLoginState( RetCode errCode){}

    /// 连接状态返回，连接成功/失败
    /// @param  msgType      -- 消息类型
    /// @param  errCode     -- 失败原因，成功时返回0
    virtual void OnConnectionState(MsgType msgType, RetCode errCode){}

    /// 延时数据返回
    /// @param  DelayTime      -- 延时数据
    virtual void OnDelayTime(const DelayTimeInfo& DelayTime){}

    /// 上交所L1静态数据订阅数据实时回调接口
    /// @param  RealSSEL1Static     -- 实时数据
    virtual void OnSubscribe_SSEL1_Static(const SSEL1_Static& RealSSEL1Static){}

    /// 上交所L1实时行情订阅数据实时回调接口
    /// @param  RealSSEL1Quotation  -- 实时数据
    virtual void OnSubscribe_SSEL1_Quotation(const SSEL1_Quotation& RealSSEL1Quotation){}

    /// 上交所指数通数据订阅数据实时回调接口
    /// @param  RealSSEIndexPress-- 实时数据
    virtual void OnSubscribe_SSE_IndexPress(const SSE_IndexPress& RealSSEIndexPress){}

    /// 上交所L2静态数据订阅数据实时回调接口
    /// @param  RealSSEL2Static     -- 实时数据
    virtual void OnSubscribe_SSEL2_Static(const SSEL2_Static& RealSSEL2Static){}

    /// 上交所L2实时行情快照订阅数据实时回调接口
    /// @param  RealSSEL2Quotation  -- 实时数据
    virtual void OnSubscribe_SSEL2_Quotation(const SSEL2_Quotation& RealSSEL2Quotation){}

    /// 上交所L2指数行情订阅数据实时回调接口
    /// @param  RealSSEL2Index      -- 实时数据
    virtual void OnSubscribe_SSEL2_Index(const SSEL2_Index& RealSSEL2Index){}

    /// 上交所L2逐笔成交订阅数据实时回调接口
    /// @param  RealSSEL2Transaction    -- 实时数据
    virtual void OnSubscribe_SSEL2_Transaction(const SSEL2_Transaction& RealSSEL2Transaction){}

    /// 上交所L2虚拟集合竞价订阅数据实时回调接口
    /// @param  RealSSEL2Auction    -- 实时数据
    virtual void OnSubscribe_SSEL2_Auction(const SSEL2_Auction& RealSSEL2Auction){}

    /// 上交所L2市场总览订阅数据实时回调接口
    /// @param  RealSSEL2Overview   -- 实时数据
    virtual void OnSubscribe_SSEL2_Overview(const SSEL2_Overview& RealSSEL2Overview){}

    /// 上交所个股期权静态数据订阅数据实时回调接口
    /// @param  RealSSEIOL1Static   -- 实时数据
    virtual void OnSubscribe_SSEIOL1_Static(const SSEIOL1_Static& RealSSEIOL1Static){}

	/// 上交所个股期权L1组合策略订阅数据实时回调接口
	/// @param  RealSSEIOL1Strategy    -- 实时数据
	virtual void OnSubscribe_SSEIOL1_Strategy(const SSEIOL1_Strategy& RealSSEIOL1Strategy){}

    /// 上交所个股期权实时行情订阅数据实时回调接口
    /// @param  RealSSEIOL1Quotation    -- 实时数据
    virtual void OnSubscribe_SSEIOL1_Quotation(const SSEIOL1_Quotation& RealSSEIOL1Quotation){}

    /// 深交所L1静态数据订阅数据实时回调接口
    /// @param  RealSZSEL1Static    -- 实时数据
    virtual void OnSubscribe_SZSEL1_Static(const SZSEL1_Static& RealSZSEL1Static){}

    /// 深交所L1实时行情订阅数据实时回调接口
    /// @param  RealSZSEL1Quotation -- 实时数据
    virtual void OnSubscribe_SZSEL1_Quotation(const SZSEL1_Quotation& RealSZSEL1Quotation){}

    /// 深交所L1公告信息回调接口
    /// @param  RealSZSEL1Bullet -- 实时数据
    virtual void OnSubscribe_SZSEL1_Bulletin(const SZSEL1_Bulletin& RealSZSEL1Bullet){}

    /// 深交所个股期权L1静态数据订阅数据实时回调接口
    /// @param  RealSZSEIOL1Static    -- 实时数据
    virtual void OnSubscribe_SZSEIOL1_Static(const SZSEIOL1_Static& RealSZSEIOL1Static){}

    /// 深交所个股期权L1实时行情订阅数据实时回调接口
    /// @param  RealSZSEIOL1Quotation    -- 实时数据
    virtual void OnSubscribe_SZSEIOL1_Quotation(const SZSEIOL1_Quotation& RealSZSEIOL1Quotation){}

    /// 深交所个股期权L1证券状态订阅数据实时回调接口
    /// @param  RealSZSEIOL1Status    -- 实时数据
    virtual void OnSubscribe_SZSEIOL1_Status(const SZSEIOL1_Status& RealSZSEIOL1Status){}

    /// 深交所个股期权L1组合策略订阅数据实时回调接口
    /// @param  RealSZSEIOL1Strategy    -- 实时数据
    virtual void OnSubscribe_SZSEIOL1_Strategy(const SZSEIOL1_Strategy& RealSZSEIOL1Strategy){}


    /// 深交所L2静态数据订阅数据实时回调接口
    /// @param  RealSZSEL2Static    -- 实时数据
    virtual void OnSubscribe_SZSEL2_Static(const SZSEL2_Static& RealSZSEL2Static){}

    /// 深交所L2实时行情订阅数据实时回调接口
    /// @param  RealSZSEL2Quotation -- 实时数据
    virtual void OnSubscribe_SZSEL2_Quotation(const SZSEL2_Quotation& RealSZSEL2Quotation){}

    /// 深交所L2证券状态订阅数据实时回调接口
    /// @param  RealSZSEL2Status    -- 实时数据
    virtual void OnSubscribe_SZSEL2_Status(const SZSEL2_Status& RealSZSEL2Status){}

    /// 深交所L2指数行情订阅数据实时回调接口
    /// @param  RealSZSEL2Index     -- 实时数据
    virtual void OnSubscribe_SZSEL2_Index(const SZSEL2_Index& RealSZSEL2Index){}

    /// 深交所L2逐笔委托订阅数据实时回调接口
    /// @param  RealSZSEL2Order     -- 实时数据
    virtual void OnSubscribe_SZSEL2_Order(const SZSEL2_Order& RealSZSEL2Order){}

    /// 深交所L2逐笔成交订阅数据实时回调接口
    /// @param  RealSZSEL2Transaction   -- 实时数据
    virtual void OnSubscribe_SZSEL2_Transaction(const SZSEL2_Transaction& RealSZSEL2Transaction){}

	/// 深交所L2逐笔订阅数据实时回调接口
	/// @param  RealSZSEL2Tick   -- 实时数据
	virtual void OnSubscribe_SZSEL2_Tick(const SZSEL2_Tick& RealSZSEL2Tick){}

    /// 中金所静态数据订阅数据实时回调接口
    /// @param  RealCFFEXL2Static   -- 实时数据
    virtual void OnSubscribe_CFFEXL2_Static(const CFFEXL2_Static& RealCFFEXL2Static){}

    /// 中金所L2实时行情订阅数据实时回调接口
    /// @param  RealCFFEXL2Quotation    -- 实时数据
    virtual void OnSubscribe_CFFEXL2_Quotation(const CFFEXL2_Quotation& RealCFFEXL2Quotation){}

    /// 郑商所静态数据订阅数据实时回调接口
    /// @param  RealCZCEL1Static    -- 实时数据
    virtual void OnSubscribe_CZCEL1_Static(const CZCEL1_Static& RealCZCEL1Static){}

    /// 郑商所实时行情订阅数据实时回调接口
    /// @param  RealCZCEL1Quotation -- 实时数据
    virtual void OnSubscribe_CZCEL1_Quotation(const CZCEL1_Quotation& RealCZCEL1Quotation){}

	/// 郑商所套利行情订阅数据实时回调接口
	/// @param  RealCZCEL1ArbiQuotation -- 实时数据
	virtual void OnSubscribe_CZCEL1_ArbiQuotation(const CZCEL1_ArbiQuotation& RealCZCEL1ArbiQuotation){}

	/// 郑商所市场状态订阅数据实时回调接口
	/// @param  RealCZCEL1MktStatus -- 实时数据
	virtual void OnSubscribe_CZCEL1_MktStatus(const CZCEL1_MktStatus& RealCZCEL1MktStatus){}

	/// 郑商所品种状态订阅数据实时回调接口
	/// @param  RealCZCEL1Status -- 实时数据
	virtual void OnSubscribe_CZCEL1_Status(const CZCEL1_Status& RealCZCEL1Status){}

    /// 易盛指数订阅数据实时回调接口
    /// @param  RealESUNNY_Index -- 实时数据
    virtual void OnSubscribe_ESUNNY_Index(const ESUNNY_Index& RealESUNNY_Index){}

    /// 大商所L1静态数据订阅数据实时回调接口
    /// @param  RealDCEL1Static     -- 实时数据
    virtual void OnSubscribe_DCEL1_Static(const DCEL1_Static& RealDCEL1Static){}

	/// 大商所L1市场状态订阅数据实时回调接口
	/// @param  RealDCEL1MktStatus     -- 实时数据
	virtual void OnSubscribe_DCEL1_MktStatus(const DCEL1_MktStatus& RealDCEL1MktStatus){}

	/// 大商所L1交易状态订阅数据实时回调接口
	/// @param  RealDCEL1Status     -- 实时数据
	virtual void OnSubscribe_DCEL1_Status(const DCEL1_Status& RealDCEL1Status){}

	/// 大商所L1静态数据订阅数据实时回调接口
	/// @param  RealDCEL1OptionParameter     -- 实时数据
	virtual void OnSubscribe_DCEL1_OptionParameter(const DCEL1_OptionParameter& RealDCEL1OptionParameter){}

    /// 大商所L1实时行情订阅数据实时回调接口
    /// @param  RealDCEL1Quotation  -- 实时数据
    virtual void OnSubscribe_DCEL1_Quotation(const DCEL1_Quotation& RealDCEL1Quotation){}

    /// 大商所L1套利行情订阅数据实时回调接口
    /// @param  RealDCEL1ArbiQuotation  -- 实时数据
    virtual void OnSubscribe_DCEL1_ArbiQuotation(const DCEL1_ArbiQuotation& RealDCEL1ArbiQuotation){}

    /// 大商所L2静态数据订阅数据实时回调接口
    /// @param  RealDCEL2Static     -- 实时数据
    virtual void OnSubscribe_DCEL2_Static(const DCEL2_Static& RealDCEL2Static){}

    /// 大商所L2实时行情订阅数据实时回调接口
    /// @param  RealDCEL2Quotation  -- 实时数据
    virtual void OnSubscribe_DCEL2_Quotation(const DCEL2_Quotation& RealDCEL2Quotation){}

    /// 大商所L2套利行情订阅数据实时回调接口
    /// @param  RealDCEL2ArbiQuotation  -- 实时数据
    virtual void OnSubscribe_DCEL2_ArbiQuotation(const DCEL2_ArbiQuotation& RealDCEL2ArbiQuotation){}

    /// 大商所L2实时结算价订阅数据实时回调接口
    /// @param  RealDCEL2RealTimePrice  -- 实时数据
    virtual void OnSubscribe_DCEL2_RealTimePrice(const DCEL2_RealTimePrice& RealDCEL2RealTimePrice){}

    /// 大商所L2委托统计行情订阅数据实时回调接口
    /// @param  RealDCEL2OrderStatistic -- 实时数据
    virtual void OnSubscribe_DCEL2_OrderStatistic(const DCEL2_OrderStatistic& RealDCEL2OrderStatistic){}

    /// 大商所L2分价成交量行情订阅数据实时回调接口
    /// @param  RealDCEL2MarchPriceQty  -- 实时数据
    virtual void OnSubscribe_DCEL2_MarchPriceQty(const DCEL2_MarchPriceQty& RealDCEL2MarchPriceQty){}

	/// 大商所L2市场状态订阅数据实时回调接口
	/// @param  RealDCEL2MktStatus     -- 实时数据
	virtual void OnSubscribe_DCEL2_MktStatus(const DCEL2_MktStatus& RealDCEL2MktStatus){}

	/// 大商所L2交易状态订阅数据实时回调接口
	/// @param  RealDCEL2Status     -- 实时数据
	virtual void OnSubscribe_DCEL2_Status(const DCEL2_Status& RealDCEL2Status){}

	/// 大商所L2静态数据订阅数据实时回调接口
	/// @param  RealDCEL2OptionParameter     -- 实时数据
	virtual void OnSubscribe_DCEL2_OptionParameter(const DCEL2_OptionParameter& RealDCEL2OptionParameter){}

	/// 大商所L2五档深度行情订阅数据实时回调接口
	/// @param  RealDCEL2LevelQuotation     -- 实时数据
	virtual void OnSubscribe_DCEL2_LevelQuotation(const DCEL2_LevelQuotation& RealDCEL2LevelQuotation){}

	/// 大商所L2委托队列行情订阅数据实时回调接口
	/// @param  RealDCEL2OrderQueue     -- 实时数据
	virtual void OnSubscribe_DCEL2_OrderQueue(const DCEL2_OrderQueue& RealDCEL2OrderQueue){}

    /// 上期所静态数据订阅数据实时回调接口
    /// @param  RealSHFEL1Static    -- 实时数据
    virtual void OnSubscribe_SHFEL1_Static(const SHFEL1_Static& RealSHFEL1Static){}

    /// 上期所所实时行情订阅数据实时回调接口
    /// @param  RealSHFEL1Quotation -- 实时数据
    virtual void OnSubscribe_SHFEL1_Quotation(const SHFEL1_Quotation& RealSHFEL1Quotation){}

    /// 上期能源静态数据订阅数据实时回调接口
    /// @param  RealINEL1Static    -- 实时数据
    virtual void OnSubscribe_INEL1_Static(const INEL1_Static& RealINEL1Static){}

    /// 上期能源实时行情订阅数据实时回调接口
    /// @param  RealINEL1Quotation -- 实时数据
    virtual void OnSubscribe_INEL1_Quotation(const INEL1_Quotation& RealINEL1Quotation){}

    //上期所二代
    /// 上期所L2静态数据订阅数据实时回调接口
    /// @param  RealSHFEL2Static    -- 实时数据
    virtual void OnSubscribe_SHFEL2_Static(const SHFEL2_Static& RealSHFEL2Static){}

    /// 上期所L2实时行情订阅数据实时回调接口
    /// @param  RealSHFEL2Quotation -- 实时数据
    virtual void OnSubscribe_SHFEL2_Quotation(const SHFEL2_Quotation& RealSHFEL2Quotation){}

    /// 上期能源L2静态数据订阅数据实时回调接口
    /// @param  RealINEL2Static    -- 实时数据
    virtual void OnSubscribe_INEL2_Static(const INEL2_Static& RealINEL2Static){}

    /// 上期能源L2实时行情订阅数据实时回调接口
    /// @param  RealINEL2Quotation -- 实时数据
    virtual void OnSubscribe_INEL2_Quotation(const INEL2_Quotation& RealINEL2Quotation){}

    /// 港交所实时行情订阅数据实时回调接口
    /// @param  RealHKEXL2Quotation -- 实时数据
    virtual void OnSubscribe_HKEXL2_Quotation(const HKEXL2_Quotation& RealHKEXL2Quotation){}

    /// 港交所经纪人队列订阅数据实时回调接口
    /// @param  RealHKEXL2BrokerQueue -- 实时数据
    virtual void OnSubscribe_HKEXL2_BrokerQueue(const HKEXL2_BrokerQueue& RealHKEXL2BrokerQueue){}

    /// 港交所指数行情订阅数据实时回调接口
    /// @param  RealHKEXL2Index -- 实时数据
    virtual void OnSubscribe_HKEXL2_Index(const HKEXL2_Index& RealHKEXL2Index){}

    /// 港交所市场总览订阅数据实时回调接口
    /// @param  RealHKEXL2Overview -- 实时数据
    virtual void OnSubscribe_HKEXL2_Overview(const HKEXL2_Overview& RealHKEXL2Overview){}

    /// 港交所静态数据订阅数据实时回调接口
    /// @param  RealHKEXL2Static -- 实时数据
    virtual void OnSubscribe_HKEXL2_Static(const HKEXL2_Static& RealHKEXL2Static){}

	/// 港交所L2互联市场日余额订阅数据实时回调接口
	/// @param  RealHKEXL2DQB -- 实时数据
	virtual void OnSubscribe_HKEXL2_DQB(const HKEXL2_DQB& RealHKEXL2DQB){}

	/// 港交所L2资金流向订阅数据实时回调接口
	/// @param  RealHKEXL2MoneyFlow -- 实时数据
	virtual void OnSubscribe_HKEXL2_MoneyFlow(const HKEXL2_MoneyFlow& RealHKEXL2MoneyFlow){}

    /// 深港通静态数据订阅数据实时回调接口
    /// @param  RealHKSZL1Static -- 实时数据
    virtual void OnSubscribe_HKSZL1_Static(const HKSZL1_Static& RealHKSZL1Static){}

    /// 深港通实时行情数据订阅数据实时回调接口
    /// @param  RealHKSZL1Quotation -- 实时数据
    virtual void OnSubscribe_HKSZL1_Quotation(const HKSZL1_Quotation& RealHKSZL1Quotation){}

    /// 沪港通静态数据订阅数据实时回调接口
    /// @param  RealHKSZL1Static -- 实时数据
    virtual void OnSubscribe_HKSHL1_Static(const HKSHL1_Static& RealHKSHL1Static){}

    /// 深港通实时行情数据订阅数据实时回调接口
    /// @param  RealHKSHL1Quotation -- 实时数据
    virtual void OnSubscribe_HKSHL1_Quotation(const HKSHL1_Quotation& RealHKSHL1Quotation){}

	/// 沪港通L1市场总览数据订阅数据实时回调接口
	/// @param  RealHKSHL1Overview -- 实时数据
	virtual void OnSubscribe_HKSHL1_Overview(const HKSHL1_Overview& RealHKSHL1Overview){}

	/// 深港通L1市场总览数据订阅数据实时回调接口
	/// @param  RealHKSZL1Overview -- 实时数据
	virtual void OnSubscribe_HKSZL1_Overview(const HKSZL1_Overview& RealHKSZL1Overview){}

	/// 上交所L2逐笔委托订阅数据实时回调接口
	/// @param  RealSSEL2Order    -- 实时数据
	virtual void OnSubscribe_SSEL2_Order(const SSEL2_Order& RealSSEL2Order){};

	/// 深交所L2国证指数订阅数据实时回调接口
	/// @param  RealSZSEL2Cnindex -- 实时数据
	virtual void OnSubscribe_SZSEL2_Cnindex(const SZSEL2_Cnindex& RealSZSEL2Cnindex){};

	/// 上交所L2债券静态数据订阅数据实时回调接口
	/// @param  RealSSEL2BondStatic -- 实时数据
	virtual void OnSubscribe_SSEL2_BondStatic(const SSEL2_BondStatic& RealSSEL2BondStatic){};

	/// 订阅上交所L2债券实时行情数据实时回调接口
	/// @param  RealSSEL2BondQuotation -- 实时数据
	virtual void OnSubscribe_SSEL2_BondQuotation(const SSEL2_BondQuotation& RealSSEL2BondQuotation){};

	/// 订阅上交所L2债券逐笔数据数据实时回调接口
	/// @param  RealSSEL2BondTick -- 实时数据
	virtual void OnSubscribe_SSEL2_BondTick(const SSEL2_BondTick& RealSSEL2BondTick){};

	/// 订阅上交所L2债券市场总览数据实时回调接口
	/// @param  RealSSEL2BondOverview -- 实时数据
	virtual void OnSubscribe_SSEL2_BondOverview(const SSEL2_BondOverview& RealSSEL2BondOverview){};

	/// 上交所L1债券静态数据订阅数据实时回调接口
	/// @param  RealSSEL1BondStatic -- 实时数据
	virtual void OnSubscribe_SSEL1_BondStatic(const SSEL1_BondStatic& RealSSEL1BondStatic){};

	/// 订阅上交所L1债券实时行情数据实时回调接口
	/// @param  RealSSEL1BondQuotation -- 实时数据
	virtual void OnSubscribe_SSEL1_BondQuotation(const SSEL1_BondQuotation& RealSSEL1BondQuotation){};


	/// 订阅深交所L2债券静态数据实时回调接口 
	/// @param  RealSZSEL2BondStatic -- 实时数据
	virtual void OnSubscribe_SZSEL2_BondStatic(const SZSEL2_BondStatic& RealSZSEL2BondStatic){};

	/// 订阅深交所L2债券实时行情数据实时回调接口
	/// @param  RealSZSEL2BondQuotation -- 实时数据
	virtual void OnSubscribe_SZSEL2_BondQuotation(const SZSEL2_BondQuotation& RealSZSEL2BondQuotation){};

	/// 订阅深交所L2债券逐笔成交数据实时回调接口
	/// @param  RealSZSEL2BondTransaction -- 实时数据
	virtual void OnSubscribe_SZSEL2_BondTransaction(const SZSEL2_BondTransaction& RealSZSEL2BondTransaction){};

	/// 订阅深交所L2债券逐笔委托实时行情数据实时回调接口
	/// @param  RealSZSEL2BondOrder -- 实时数据
	virtual void OnSubscribe_SZSEL2_BondOrder(const SZSEL2_BondOrder& RealSZSEL2BondOrder){};

	/// 订阅深交所L2债券逐比数据实时回调接口 
	/// @param  RealSZSEL2_BondTick -- 实时数据
	virtual void OnSubscribe_SZSEL2_BondTick(const SZSEL2_BondTick& RealSZSEL2_BondTick){};

	/// 订阅深交所L2债券交易商信息实时回调接口
	/// @param  RealSZSEL2BondMbrInfo -- 实时数据
	virtual void OnSubscribe_SZSEL2_BondMbrInfo(const SZSEL2_BondMbrInfo& RealSZSEL2BondMbrInfo){};

	/// 订阅深交所L2债券交易主体信息实时回调接口
	/// @param  RealSZSEL2BondInvestorInfo -- 实时数据
	virtual void OnSubscribe_SZSEL2_BondInvestorInfo(const SZSEL2_BondInvestorInfo& RealSZSEL2BondInvestorInfo){};

	/// 订阅深交所L2债券交易员信息实时回调接口
	/// @param  RealSZSEL2BondOrder -- 实时数据
	virtual void OnSubscribe_SZSEL2_BondTraderInfo(const SZSEL2_BondTraderInfo& RealSZSEL2BondTraderInfo){};

	/// 订阅深交所L1债券静态数据实时回调接口
	/// @param  RealSZSEL1BondStatic -- 实时数据
	virtual void OnSubscribe_SZSEL1_BondStatic(const SZSEL1_BondStatic& RealSZSEL1BondStatic){};

	/// 订阅深交所L1债券实时行情实时回调接口
	/// @param  RealSZSEL1BondQuotation -- 实时数据
	virtual void OnSubscribe_SZSEL1_BondQuotation(const SZSEL1_BondQuotation& RealSZSEL1BondQuotation){};

	//北交所
	/// 北交所L1静态数据订阅数据实时回调接口
	/// @param  RealBSEL1Static    -- 实时数据
	virtual void OnSubscribe_BSEL1_Static(const BSEL1_Static& RealBSEL1Static){}

	/// 北交所L1实时行情订阅数据实时回调接口
	/// @param  RealBSEL1Quotation -- 实时数据
	virtual void OnSubscribe_BSEL1_Quotation(const BSEL1_Quotation& RealBSEL1Quotation){}

	/// 深交所L2债券市场竞买预约信息订阅数据实时回调接口
	/// @param  RealSZSEL2BondBidBookingInfo -- 实时数据
	virtual void OnSubscribe_SZSEL2_BondBidBookingInfo(const SZSEL2_BondBidBookingInfo& RealSZSEL2BondBidBookingInfo){}

	/// 郑商所L2静态数据订阅数据实时回调接口
	/// @param  RealCZCEL2Static -- 实时数据
	virtual void OnSubscribe_CZCEL2_Static(const CZCEL2_Static& RealCZCEL2Static){}

	/// 郑商所L2市场状态订阅数据实时回调接口
	/// @param  RealCZCEL2MktStatus -- 实时数据
	virtual void OnSubscribe_CZCEL2_MktStatus(const CZCEL2_MktStatus& RealCZCEL2MktStatus){}

	/// 郑商所L2品种状态订阅数据实时回调接口
	/// @param  RealCZCEL2Status -- 实时数据
	virtual void OnSubscribe_CZCEL2_Status(const CZCEL2_Status& RealCZCEL2Status){}

	/// 郑商所L2实时行情订阅数据实时回调接口
	/// @param  RealCZCEL2Quotation -- 实时数据
	virtual void OnSubscribe_CZCEL2_Quotation(const CZCEL2_Quotation& RealCZCEL2Quotation){}

	/// 郑商所L2五档深度委托行情订阅数据实时回调接口
	/// @param  RealCZCEL2LevelQuotation -- 实时数据
	virtual void OnSubscribe_CZCEL2_LevelQuotation(const CZCEL2_LevelQuotation& RealCZCEL2LevelQuotation){}

    /// 郑商所L2套利行情订阅数据回调接口
    /// @param  data     -- 实时数据
    virtual void OnSubscribe_CZCEL2_ArbiQuotation(const CZCEL2_ArbiQuotation& data){}

    /// 郑商所L2套利五档深度行情订阅数据回调接口
    /// @param  data     -- 实时数据
    virtual void OnSubscribe_CZCEL2_ArbiLevelQuotation(const CZCEL2_ArbiLevelQuotation& data){}

	/// 上交所L2精简实时行情订阅数据实时回调接口
	/// @param  RealSSEL2ShortQuotation -- 实时数据
	virtual void OnSubscribe_SSEL2_ShortQuotation(const SSEL2_ShortQuotation& RealSSEL2ShortQuotation){}

	/// 上交所L2委托队列订阅数据实时回调接口
	/// @param  RealSSEL2OrderQueue -- 实时数据
	virtual void OnSubscribe_SSEL2_OrderQueue(const SSEL2_OrderQueue& RealSSEL2OrderQueue){}

	/// 深交所L2精简实时行情订阅数据实时回调接口
	/// @param  RealSZSEL2ShortQuotation -- 实时数据
	virtual void OnSubscribe_SZSEL2_ShortQuotation(const SZSEL2_ShortQuotation& RealSZSEL2ShortQuotation){}

	/// 深交所L2委托队列订阅数据实时回调接口
	/// @param  RealSZSEL2OrderQueue -- 实时数据
	virtual void OnSubscribe_SZSEL2_OrderQueue(const SZSEL2_OrderQueue& RealSZSEL2OrderQueue){}
	
	/// 上交所L2 ETF统计数据订阅数据回调接口
	/// @param  data     -- 实时数据
	virtual void OnSubscribe_SSEL2_ETF(const SSEL2_ETF& data){}

    /// 广期所L1静态数据订阅数据回调接口
    /// @param  data     -- 实时数据
    virtual void OnSubscribe_GFEXL1_Static(const GFEXL1_Static& data){}

    /// 广期所L1套利行情订阅数据回调接口
    /// @param  data     -- 实时数据
    virtual void OnSubscribe_GFEXL1_ArbiQuotation(const GFEXL1_ArbiQuotation& data){}

    /// 广期所L1实时行情订阅数据回调接口
    /// @param  data     -- 实时数据
    virtual void OnSubscribe_GFEXL1_Quotation(const GFEXL1_Quotation& data){}

    /// 广期所L1期权参数订阅数据回调接口
    /// @param  data     -- 实时数据
    virtual void OnSubscribe_GFEXL1_OptionParameter(const GFEXL1_OptionParameter& data){}

    /// 广期所L2静态数据订阅数据回调接口
    /// @param  data     -- 实时数据
    virtual void OnSubscribe_GFEXL2_Static(const GFEXL2_Static& data){}

    /// 广期所L2套利行情订阅数据回调接口
    /// @param  data     -- 实时数据
    virtual void OnSubscribe_GFEXL2_ArbiQuotation(const GFEXL2_ArbiQuotation& data){}

    /// 广期所L2实时行情订阅数据回调接口
    /// @param  data     -- 实时数据
    virtual void OnSubscribe_GFEXL2_Quotation(const GFEXL2_Quotation& data){}

    /// 广期所L2委托队列行情订阅数据回调接口
    /// @param  data     -- 实时数据
    virtual void OnSubscribe_GFEXL2_OrderQueue(const GFEXL2_OrderQueue& data){}

    /// 广期所L2委托统计行情订阅数据回调接口
    /// @param  data     -- 实时数据
    virtual void OnSubscribe_GFEXL2_OrderStatistic(const GFEXL2_OrderStatistic& data){}

    /// 广期所L2实时结算价订阅数据回调接口
    /// @param  data     -- 实时数据
    virtual void OnSubscribe_GFEXL2_RealTimePrice(const GFEXL2_RealTimePrice& data){}

    /// 广期所L2分价成交量行情订阅数据回调接口
    /// @param  data     -- 实时数据
    virtual void OnSubscribe_GFEXL2_MarchPriceQty(const GFEXL2_MarchPriceQty& data){}

    /// 广期所L2期权参数订阅数据回调接口
    /// @param  data     -- 实时数据
    virtual void OnSubscribe_GFEXL2_OptionParameter(const GFEXL2_OptionParameter& data){}

	/// 广期所L2套利委托队列订阅数据回调接口
	/// @param  data     -- 实时数据
	virtual void OnSubscribe_GFEXL2_ArbiOrderQueue(const GFEXL2_ArbiOrderQueue& data){}

    /// 上交所L2盘后固定价格逐笔成交订阅数据回调接口
    /// @param  data     -- 实时数据
    virtual void OnSubscribe_SSEL2_FpTransaction(const SSEL2_FpTransaction& data){}

    /// 上交所L2逐笔行情订阅数据回调接口
    /// @param  data     -- 实时数据
    virtual void OnSubscribe_SSEL2_Tick(const SSEL2_Tick& data){}

    /// 大商所指数行情 订阅数据回调接口
    /// @param  data     -- 实时数据
    virtual void OnSubscribe_DCE_Index(const DCE_Index& data){}

    /// 上交所L1盘后固定价格行情订阅数据回调接口
    /// @param  data     -- 实时数据
    virtual void OnSubscribe_SSEL1_FpQuotation(const SSEL1_FpQuotation& data){}

    /// 上交所L2盘后固定价格行情订阅数据回调接口
    /// @param  data     -- 实时数据
    virtual void OnSubscribe_SSEL2_FpQuotation(const SSEL2_FpQuotation& data){}

    /// 深交所L1盘后固定价格行情订阅数据回调接口
    /// @param  data     -- 实时数据
    virtual void OnSubscribe_SZSEL1_FpQuotation(const SZSEL1_FpQuotation& data){}

	/// 深交所L2盘后固定价格行情订阅数据回调接口
	/// @param  data     -- 实时数据
	virtual void OnSubscribe_SZSEL2_FpQuotation(const SZSEL2_FpQuotation& data){}
};


/// 行情系统服务接口，连接服务器进行认证，订阅数据，查询快照.
class GTA_API_EXPORT IGTAQTSApiBase
{
public:
    virtual ~IGTAQTSApiBase(){}
public:
    /// 创建一个实例.所有实例都必须通过本接口生成.
    /// @param  CallBack    -- 连接回调函数，通知连接状态
    /// @return NULL        -- 创建实例失败
    static IGTAQTSApiBase* CreateInstance(IGTAQTSCallbackBase& CallBack);

    /// 释放一个实例,该实例是通过CreateInstance生成的.
    /// @param  pInstance   -- 实例对象指针.
    static void ReleaseInstance(IGTAQTSApiBase* pInstance);

    /// 设置超时时间，设置即时生效，同步调用的超时时间
    /// @param  nSenond     -- 超时时间，单位秒
    virtual void SetTimeout(int nSecond = TIMEOUT_DEFAULT) = 0;

    /// 设置心跳间隔时长，设置即时生效，
    /// @param  nSenond     -- 心跳间隔时长，单位秒
    virtual void SetHeartBeatTime(int nSecond = HEART_BEAT_INTERVAL) = 0;

    /// 开始API到行情网络延迟测试
    /// @param  bSwitch     -- 开关,true 开启  false  关闭
    /// @return RetCode     -- 错误代码, Ret_Success:成功;Ret_Error:失败(未订阅行情)
    virtual RetCode DelayTest(bool bSwitch) = 0;

    /// 注册行情服务器地址，可注册多个不同地址，实现高可用，连接顺序为顺序连接，断线时先重连当前使用地址
    /// @param  pIP         -- 服务器地址.
    /// @param  uPort       -- 服务器端口.
    /// @return RetCode     -- 错误代码
    virtual RetCode RegisterService(const char* pIP, unsigned short uPort) = 0;

	/// 设置深交所L2 Tick数据多线程回调
	/// @param  bMTCallBack     -- 开关,true 开启  false  关闭
	virtual void SetSZL2TickMTCallBack(bool bMTCallBack) = 0;

    /// 连接行情服务器，发送用户认证信息，同步请求
    /// @param  pUserName   -- 账号名称
    /// @param  pPassword   -- 账号密码，加密后密码
    /// @return RetCode     -- 错误代码
    virtual RetCode Login(const char* pUserName, const char* pPassword) = 0;
    
    /// 连接行情服务器，发送用户认证信息，同步请求
    /// @param  pUserName   -- 账号名称
    /// @param  pPassword   -- 账号密码，加密后密码
    /// @param  pOptions    -- 可选附加参数，可填充null。当有多个参数时，用逗号（，）分隔
    ///                        标识        可选值       说明
    ///                        NetType     0，1         连接网络类型，0表示外网(默认值)，1表示特殊网络
    ///                        示例，如："NetType=1"
    /// @return RetCode     -- 错误代码
    virtual RetCode LoginX(const char* pUserName, const char* pPassword, const char* pOptions = NULL) = 0;

    /// 获取行情订阅服务器地址信息
    /// @param  ServerStates    -- 存放服务器地址对象
    /// @return RetCode         -- 错误代码
    virtual RetCode GetConnectState(CDataBuffer<ServerState>& ServerStates) = 0;

    /// 获取有权限的数据类型.
    /// @param  DataTypeList-- 数据类型指针，数据大小即为有权限的数据个数
    /// @return RetCode     -- 错误代码
    virtual RetCode GetDataTypeList(CDataBuffer<MsgType>& DataTypeList) = 0;

    /// 订阅行情，重复订阅同一个数据多次时取并集
    /// @param  msgType     -- 需要订阅的数据类型
    /// @param  pCodeList   -- 个股订阅列表，以“,”分割，末尾必须以\0结束，为NULL时订阅全市场
    /// @return RetCode     -- 错误代码
    virtual RetCode Subscribe(MsgType msgType, char* pCodeList = NULL) = 0;

    /// 取消订阅
    /// @param  msgType     -- 需要取消订阅的数据类型
    /// @param  pCodeList   -- 取消个股列表，以“,”分割，末尾必须以\0结束，为NULL时取消所有代码
    /// @return RetCode     -- 错误代码
    virtual RetCode Unsubscribe(MsgType msgType, char* pCodeList = NULL) = 0;

    /// 取消所有订阅信息
    /// @return RetCode     -- 错误代码
    virtual RetCode UnsubscribeAll() = 0;

    /// 获取股票列表
    /// @param  pMarketTag  -- 市场标识，以“,”分割多个市场标识，末尾必须以\0结束，不允许为NULL
    /// @param  StockList   -- 返回的个股代码列表，带市场标志。
    /// @return RetCode     -- 错误代码
    virtual RetCode GetStockList(char* pMarketTag, CDataBuffer<StockSymbol>& StockList) = 0;

    /// 查询上交所L1静态数据
    /// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  SnapSSEL1Static     -- 快照数据列表
    /// @return RetCode             -- 错误代码
    virtual RetCode QuerySnap_SSEL1_Static(char* pCodeList, CDataBuffer<SSEL1_Static>& SnapSSEL1Static) = 0;

    /// 查询上交所L1实时行情
    /// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  SnapSSEQuotation     -- 快照数据列表
    /// @return RetCode             -- 错误代码
    virtual RetCode QuerySnap_SSEL1_Quotation(char* pCodeList, CDataBuffer<SSEL1_Quotation>& SnapSSEQuotation) = 0;

    /// 查询上交所指数通数据
    /// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  SnapSSEIndexPress   -- 快照数据列表
    /// @return RetCode             -- 错误代码
    virtual RetCode QuerySnap_SSE_IndexPress(char* pCodeList, CDataBuffer<SSE_IndexPress>& SnapSSEIndexPress) = 0;

    /// 查询上交所L2静态数据
    /// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  SnapSSEStatic        -- 快照数据列表
    /// @return RetCode             -- 错误代码
    virtual RetCode QuerySnap_SSEL2_Static(char* pCodeList, CDataBuffer<SSEL2_Static>& SnapSSEL2Static) = 0;

    /// 查询上交所L2实时行情快照
    /// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  SnapSSEL2Quotation    -- 快照数据列表
    /// @return RetCode             -- 错误代码
    virtual RetCode QuerySnap_SSEL2_Quotation(char* pCodeList, CDataBuffer<SSEL2_Quotation>& SnapSSEL2Quotation) = 0;

    /// 查询上交所L2指数行情
    /// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  SnapSSEL2Index        -- 快照数据列表
    /// @return RetCode             -- 错误代码
    virtual RetCode QuerySnap_SSEL2_Index(char* pCodeList, CDataBuffer<SSEL2_Index>& SnapSSEL2Index) = 0;

    /// 查询上交所L2逐笔成交
    /// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  SnapSSEL2Transation   -- 快照数据列表
    /// @return RetCode             -- 错误代码
    virtual RetCode QuerySnap_SSEL2_Transaction(char* pCodeList, CDataBuffer<SSEL2_Transaction>& SnapSSEL2Transation) = 0;

    /// 查询上交所L2虚拟集合竞价
    /// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  SnapSSEL2Auction    -- 快照数据列表
    /// @return RetCode             -- 错误代码
    virtual RetCode QuerySnap_SSEL2_Auction(char* pCodeList, CDataBuffer<SSEL2_Auction>& SnapSSEL2Auction) = 0;

    /// 查询上交所L2市场总览
    /// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  SnapSSEL2Overview   -- 快照数据列表
    /// @return RetCode             -- 错误代码
    virtual RetCode QuerySnap_SSEL2_Overview(char* pCodeList, CDataBuffer<SSEL2_Overview>& SnapSSEL2Overview) = 0;

    /// 查询上交所个股期权静态数据
    /// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  SnapSSEIOL1Static   -- 快照数据列表
    /// @return RetCode             -- 错误代码
    virtual RetCode QuerySnap_SSEIOL1_Static(char* pCodeList, CDataBuffer<SSEIOL1_Static>& SnapSSEIOL1Static) = 0;

    /// 查询上交所个股期权实时行情
    /// @param  pCodeList               -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  SnapSSEIOL1Quotation    -- 快照数据列表
    /// @return RetCode                 -- 错误代码
    virtual RetCode QuerySnap_SSEIOL1_Quotation(char* pCodeList, CDataBuffer<SSEIOL1_Quotation>& SnapSSEIOL1Quotation) = 0;

	/// 查询上交所个股期权L1组合策略
	/// @param  pCodeList                -- 快照订阅列表，NULL时为全市场快照数据
	/// @param  SnapSSEIOL1Strategy      -- 快照数据列表
	/// @return RetCode                  -- 错误代码
	virtual RetCode QuerySnap_SSEIOL1_Strategy(char* pCodeList, CDataBuffer<SSEIOL1_Strategy>& SnapSSEIOL1Strategy) = 0;

    /// 查询深交所L1静态数据
    /// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  SnapSZSEL1Static    -- 快照数据列表
    /// @return RetCode             -- 错误代码
    virtual RetCode QuerySnap_SZSEL1_Static(char* pCodeList, CDataBuffer<SZSEL1_Static>& SnapSZSEL1Static) = 0;

    /// 查询深交所L1实时行情
    /// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  SnapSZSEL1Quotation -- 快照数据列表
    /// @return RetCode             -- 错误代码
    virtual RetCode QuerySnap_SZSEL1_Quotation(char* pCodeList, CDataBuffer<SZSEL1_Quotation>& SnapSZSEL1Quotation) = 0;

    /// 查询深交所L1公告
    /// @param  pCodeList           -- 快照订阅列表，只能为NULL，输入将被忽略
    /// @param  SnapSZSEL1Bullet    -- 快照数据列表
    /// @return RetCode             -- 错误代码
    virtual RetCode QuerySnap_SZSEL1_Bulletin(char* pCodeList, CDataBuffer<SZSEL1_Bulletin>& SnapSZSEL1Bullet) = 0;

    /// 查询深交所个股期权L1静态数据
    /// @param  pCodeList             -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  SnapSZSEIOL1Static    -- 快照数据列表
    /// @return RetCode               -- 错误代码
    virtual RetCode QuerySnap_SZSEIOL1_Static(char* pCodeList, CDataBuffer<SZSEIOL1_Static>& SnapSZSEIOL1Static) = 0;

    /// 查询深交所个股期权L1实时行情
    /// @param  pCodeList                 -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  SnapSZSEIOL1Quotation    -- 快照数据列表
    /// @return RetCode                   -- 错误代码
    virtual RetCode QuerySnap_SZSEIOL1_Quotation(char* pCodeList, CDataBuffer<SZSEIOL1_Quotation>& SnapSZSEIOL1Quotation) = 0;

    /// 查询深交所个股期权L1证券状态
    /// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  SnapSZSEIOL1Status    -- 快照数据列表
    /// @return RetCode             -- 错误代码
    virtual RetCode QuerySnap_SZSEIOL1_Status(char* pCodeList, CDataBuffer<SZSEIOL1_Status>& SnapSZSEIOL1Status) = 0;

    /// 查询深交所个股期权L1组合策略
    /// @param  pCodeList                -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  SnapSZSEIOL1Strategy    -- 快照数据列表
    /// @return RetCode                  -- 错误代码
    virtual RetCode QuerySnap_SZSEIOL1_Strategy(char* pCodeList, CDataBuffer<SZSEIOL1_Strategy>& SnapSZSEIOL1Strategy) = 0;

    /// 查询深交所L2静态数据
    /// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  SnapSZSEL2Static    -- 快照数据列表
    /// @return RetCode             -- 错误代码
    virtual RetCode QuerySnap_SZSEL2_Static(char* pCodeList, CDataBuffer<SZSEL2_Static>& SnapSZSEL2Static) = 0;

    /// 查询深交所L2实时行情
    /// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  SnapSZSEL2Quotation    -- 快照数据列表
    /// @return RetCode             -- 错误代码
    virtual RetCode QuerySnap_SZSEL2_Quotation(char* pCodeList, CDataBuffer<SZSEL2_Quotation>& SnapSZSEL2Quotation) = 0;

    /// 查询深交所L2证券状态
    /// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  SnapSZSEL2StockStatus  -- 快照数据列表
    /// @return RetCode             -- 错误代码
    virtual RetCode QuerySnap_SZSEL2_Status(char* pCodeList, CDataBuffer<SZSEL2_Status>& SnapSZSEL2StockStatus) = 0;

    /// 查询深交所L2指数行情
    /// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  SnapSZSEL2Index        -- 快照数据列表
    /// @return RetCode             -- 错误代码
    virtual RetCode QuerySnap_SZSEL2_Index(char* pCodeList, CDataBuffer<SZSEL2_Index>& SnapSZSEL2Index) = 0;

    /// 查询深交所L2逐笔委托
    /// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  SnapSZSEL2Order        -- 快照数据列表
    /// @return RetCode             -- 错误代码
    virtual RetCode QuerySnap_SZSEL2_Order(char* pCodeList, CDataBuffer<SZSEL2_Order>& SnapSZSEL2Order) = 0;

    /// 查询深交所L2逐笔成交
    /// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  SnapSZSEL2Transaction  -- 快照数据列表
    /// @return RetCode             -- 错误代码
    virtual RetCode QuerySnap_SZSEL2_Transaction(char* pCodeList, CDataBuffer<SZSEL2_Transaction>& SnapSZSEL2Transaction) = 0;

	/// 查询深交所L2逐笔
	/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
	/// @param  SnapSZSEL2Tick      -- 快照数据列表
	/// @return RetCode             -- 错误代码
	virtual RetCode QuerySnap_SZSEL2_Tick(char* pCodeList, CDataBuffer<SZSEL2_Tick>& SnapSZSEL2Tick) = 0;

    /// 查询中金所静态数据
    /// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  SnapCFFEXL2Static       -- 快照数据列表
    /// @return RetCode             -- 错误代码
    virtual RetCode QuerySnap_CFFEXL2_Static(char* pCodeList, CDataBuffer<CFFEXL2_Static>& SnapCFFEXL2Static) = 0;

    /// 查询中金所L2实时行情
    /// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  SnapCFFEXL2Quotation    -- 快照数据列表
    /// @return RetCode             -- 错误代码
    virtual RetCode QuerySnap_CFFEXL2_Quotation(char* pCodeList, CDataBuffer<CFFEXL2_Quotation>& SnapCFFEXL2Quotation) = 0;

    /// 查询郑商所静态数据
    /// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  SnapCZCEL1Static    -- 快照数据列表
    /// @return RetCode             -- 错误代码
    virtual RetCode QuerySnap_CZCEL1_Static(char* pCodeList, CDataBuffer<CZCEL1_Static>& SnapCZCEL1Static) = 0;

    /// 查询郑商所实时行情
    /// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  SnapCZCEL1Quotation -- 快照数据列表
    /// @return RetCode             -- 错误代码
    virtual RetCode QuerySnap_CZCEL1_Quotation(char* pCodeList, CDataBuffer<CZCEL1_Quotation>& SnapCZCEL1Quotation) = 0;

	/// 查询郑商所套利行情
	/// @param  pCodeList                -- 快照订阅列表，NULL时为全市场快照数据
	/// @param  SnapCZCEL1ArrbiQuotation -- 快照数据列表
	/// @return RetCode                  -- 错误代码
	virtual RetCode QuerySnap_CZCEL1_ArbiQuotation(char* pCodeList, CDataBuffer<CZCEL1_ArbiQuotation>& SnapCZCEL1ArrbiQuotation) = 0;

	/// 查询郑商所L1市场状态
	/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
	/// @param  SnapCZCEL1MktStatus -- 快照数据列表
	/// @return RetCode             -- 错误代码
	virtual RetCode QuerySnap_CZCEL1_MktStatus(char* pCodeList, CDataBuffer<CZCEL1_MktStatus>& SnapCZCEL1MktStatus) = 0;

	/// 查询郑商所L1品种状态
	/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
	/// @param  SnapCZCEL1Status    -- 快照数据列表
	/// @return RetCode             -- 错误代码
	virtual RetCode QuerySnap_CZCEL1_Status(char* pCodeList, CDataBuffer<CZCEL1_Status>& SnapCZCEL1Status) = 0;

    /// 查询易盛指数行情
    /// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  SnapESUNNY_Index        -- 快照数据列表
    /// @return RetCode             -- 错误代码
    virtual RetCode QuerySnap_ESUNNY_Index(char* pCodeList, CDataBuffer<ESUNNY_Index>& SnapESUNNY_Index) = 0;

    /// 查询大商所L1静态数据
    /// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  SnapDCEL1Static     -- 快照数据列表
    /// @return RetCode             -- 错误代码
    virtual RetCode QuerySnap_DCEL1_Static(char* pCodeList, CDataBuffer<DCEL1_Static>& SnapDCEL1Static) = 0;

	/// 查询大商所L1市场状态
	/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
	/// @param  SnapDCEL1MktStatus     -- 快照数据列表
	/// @return RetCode             -- 错误代码
	virtual RetCode QuerySnap_DCEL1_MktStatus(char* pCodeList, CDataBuffer<DCEL1_MktStatus>& SnapDCEL1MktStatus) = 0;

	/// 查询大商所L1交易状态
	/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
	/// @param  SnapDCEL1Status     -- 快照数据列表
	/// @return RetCode             -- 错误代码
	virtual RetCode QuerySnap_DCEL1_Status(char* pCodeList, CDataBuffer<DCEL1_Status>& SnapDCEL1Status) = 0;

	/// 查询大商所L1期权参数
	/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
	/// @param  SnapDCEL1OptionParameter     -- 快照数据列表
	/// @return RetCode             -- 错误代码
	virtual RetCode QuerySnap_DCEL1_OptionParameter(char* pCodeList, CDataBuffer<DCEL1_OptionParameter>& SnapDCEL1OptionParameter) = 0;

    /// 查询大商所L1实时行情
    /// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  SnapDCEL1Quotation  -- 快照数据列表
    /// @return RetCode             -- 错误代码
    virtual RetCode QuerySnap_DCEL1_Quotation(char* pCodeList, CDataBuffer<DCEL1_Quotation>& SnapDCEL1Quotation) = 0;

    /// 查询大商所L1套利行情
    /// @param  pCodeList               -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  SnapDCEL1ArbiQuotation  -- 快照数据列表
    /// @return RetCode                 -- 错误代码
    virtual RetCode QuerySnap_DCEL1_ArbiQuotation(char* pCodeList, CDataBuffer<DCEL1_ArbiQuotation>& SnapDCEL1ArbiQuotation) = 0;

    /// 查询大商所L2静态数据
    /// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  SnapDCEL2Static     -- 快照数据列表
    /// @return RetCode             -- 错误代码
    virtual RetCode QuerySnap_DCEL2_Static(char* pCodeList, CDataBuffer<DCEL2_Static>& SnapDCEL2Static) = 0;

    /// 查询大商所L2实时行情
    /// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  SnapDCEL2Quotation  -- 快照数据列表
    /// @return RetCode             -- 错误代码
    virtual RetCode QuerySnap_DCEL2_Quotation(char* pCodeList, CDataBuffer<DCEL2_Quotation>& SnapDCEL2Quotation) = 0;

    /// 查询大商所L2套利行情
    /// @param  pCodeList               -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  SnapDCEL2ArbiQuotation  -- 快照数据列表
    /// @return RetCode                 -- 错误代码
    virtual RetCode QuerySnap_DCEL2_ArbiQuotation(char* pCodeList, CDataBuffer<DCEL2_ArbiQuotation>& SnapDCEL2ArbiQuotation) = 0;

    /// 查询大商所L2实时结算价
    /// @param  pCodeList               -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  SnapDCEL2RealTimePrice  -- 快照数据列表
    /// @return RetCode                 -- 错误代码
    virtual RetCode QuerySnap_DCEL2_RealTimePrice(char* pCodeList, CDataBuffer<DCEL2_RealTimePrice>& SnapDCEL2RealTimePrice) = 0;

    /// 查询大商所L2委托统计行情
    /// @param  pCodeList               -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  SnapDCEL2OrderStatistic -- 快照数据列表
    /// @return RetCode                 -- 错误代码
    virtual RetCode QuerySnap_DCEL2_OrderStatistic(char* pCodeList, CDataBuffer<DCEL2_OrderStatistic>& SnapDCEL2OrderStatistic) = 0;

    /// 查询大商所L2分价成交量行情
    /// @param  pCodeList               -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  SnapDCEL2MarchPriceQty  -- 快照数据列表
    /// @return RetCode                 -- 错误代码
    virtual RetCode QuerySnap_DCEL2_MarchPriceQty(char* pCodeList, CDataBuffer<DCEL2_MarchPriceQty>& SnapDCEL2MarchPriceQty) = 0;

	/// 查询大商所L2市场状态
	/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
	/// @param  SnapDCEL2MktStatus     -- 快照数据列表
	/// @return RetCode             -- 错误代码
	virtual RetCode QuerySnap_DCEL2_MktStatus(char* pCodeList, CDataBuffer<DCEL2_MktStatus>& SnapDCEL2MktStatus) = 0;

	/// 查询大商所L2交易状态
	/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
	/// @param  SnapDCEL2Status     -- 快照数据列表
	/// @return RetCode             -- 错误代码
	virtual RetCode QuerySnap_DCEL2_Status(char* pCodeList, CDataBuffer<DCEL2_Status>& SnapDCEL2Status) = 0;

	/// 查询大商所L2期权参数
	/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
	/// @param  SnapDCEL2OptionParameter     -- 快照数据列表
	/// @return RetCode             -- 错误代码
	virtual RetCode QuerySnap_DCEL2_OptionParameter(char* pCodeList, CDataBuffer<DCEL2_OptionParameter>& SnapDCEL2OptionParameter) = 0;

	/// 查询大商所L2五档深度行情
	/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
	/// @param  SnapDCEL2LevelQuotation     -- 快照数据列表
	/// @return RetCode             -- 错误代码
	virtual RetCode QuerySnap_DCEL2_LevelQuotation(char* pCodeList, CDataBuffer<DCEL2_LevelQuotation>& SnapDCEL2LevelQuotation) = 0;

	/// 查询大商所L2委托队列行情
	/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
	/// @param  SnapDCEL2OrderQueue     -- 快照数据列表
	/// @return RetCode             -- 错误代码
	virtual RetCode QuerySnap_DCEL2_OrderQueue(char* pCodeList, CDataBuffer<DCEL2_OrderQueue>& SnapDCEL2OrderQueue) = 0;

    /// 查询上期所静态数据
    /// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  SnapSHFEL1Static    -- 快照数据列表
    /// @return RetCode             -- 错误代码
    virtual RetCode QuerySnap_SHFEL1_Static(char* pCodeList, CDataBuffer<SHFEL1_Static>& SnapSHFEL1Static) = 0;

    /// 查询上期所实时行情
    /// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  SnapSHFEL1Quotation -- 快照数据列表
    /// @return RetCode             -- 错误代码
    virtual RetCode QuerySnap_SHFEL1_Quotation(char* pCodeList, CDataBuffer<SHFEL1_Quotation>& SnapSHFEL1Quotation) = 0;

    /// 查询上期能源静态数据
    /// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  SnapINEL1Static   -- 快照数据列表
    /// @return RetCode             -- 错误代码
    virtual RetCode QuerySnap_INEL1_Static(char* pCodeList, CDataBuffer<INEL1_Static>& SnapINEL1Static) = 0;

    /// 查询上期能源实时行情
    /// @param  pCodeList            -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  SnapINEL1Quotation -- 快照数据列表
    /// @return RetCode              -- 错误代码
    virtual RetCode QuerySnap_INEL1_Quotation(char* pCodeList, CDataBuffer<INEL1_Quotation>& SnapINEL1Quotation) = 0;

    //上期二代查询快照
    /// 查询上期所L2静态数据
    /// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  SnapSHFEL2Static    -- 快照数据列表
    /// @return RetCode             -- 错误代码
    virtual RetCode QuerySnap_SHFEL2_Static(char* pCodeList, CDataBuffer<SHFEL2_Static>& SnapSHFEL2Static) = 0;

    /// 查询上期所L2实时行情
    /// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  SnapSHFEL2Quotation -- 快照数据列表
    /// @return RetCode             -- 错误代码
    virtual RetCode QuerySnap_SHFEL2_Quotation(char* pCodeList, CDataBuffer<SHFEL2_Quotation>& SnapSHFEL2Quotation) = 0;

    /// 查询上期能源L2静态数据
    /// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  SnapINEL2Static   --   快照数据列表
    /// @return RetCode             -- 错误代码
    virtual RetCode QuerySnap_INEL2_Static(char* pCodeList, CDataBuffer<INEL2_Static>& SnapINEL2Static) = 0;

    /// 查询上期能源L2实时行情
    /// @param  pCodeList            -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  SnapINEL2Quotation   -- 快照数据列表
    /// @return RetCode              -- 错误代码
    virtual RetCode QuerySnap_INEL2_Quotation(char* pCodeList, CDataBuffer<INEL2_Quotation>& SnapINEL2Quotation) = 0;

    /// 查询港交所所实时行情
    /// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  SnapHKEXL2Quotation   -- 快照数据列表
    /// @return RetCode             -- 错误代码
    virtual RetCode QuerySnap_HKEXL2_Quotation(char* pCodeList, CDataBuffer<HKEXL2_Quotation>& SnapHKEXL2Quotation) = 0;

    /// 查询港交所所静态数据
    /// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  SnapHKEXL2Static      -- 快照数据列表
    /// @return RetCode             -- 错误代码
    virtual RetCode QuerySnap_HKEXL2_Static(char* pCodeList, CDataBuffer<HKEXL2_Static>& SnapHKEXL2Static) = 0;

    /// 查询港交所所经纪人队列
    /// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  SnapHKEXL2BrokerQueue   -- 快照数据列表
    /// @return RetCode             -- 错误代码
    virtual RetCode QuerySnap_HKEXL2_BrokerQueue(char* pCodeList, CDataBuffer<HKEXL2_BrokerQueue>& SnapHKEXL2BrokerQueue) = 0;

    /// 查询港交所所指数行情
    /// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  SnapHKEXL2Index      -- 快照数据列表
    /// @return RetCode             -- 错误代码
    virtual RetCode QuerySnap_HKEXL2_Index(char* pCodeList, CDataBuffer<HKEXL2_Index>& SnapHKEXL2Index) = 0;

    /// 查询港交所所市场总览
    /// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  SnapHKEXL2Overview   -- 快照数据列表
    /// @return RetCode             -- 错误代码
    virtual RetCode QuerySnap_HKEXL2_Overview(char* pCodeList, CDataBuffer<HKEXL2_Overview>& SnapHKEXL2Overview) = 0;

	/// 查询港交所L2互联市场日余额
	/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
	/// @param  SnapHKEXL2DQB       -- 快照数据列表
	/// @return RetCode             -- 错误代码
	virtual RetCode QuerySnap_HKEXL2_DQB(char* pCodeList, CDataBuffer<HKEXL2_DQB>& SnapHKEXL2DQB) = 0;

	/// 查询港交所L2资金流向
	/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
	/// @param  SnapHKEXL2MoneyFlow -- 快照数据列表
	/// @return RetCode             -- 错误代码
	virtual RetCode QuerySnap_HKEXL2_MoneyFlow(char* pCodeList, CDataBuffer<HKEXL2_MoneyFlow>& SnapHKEXL2MoneyFlow) = 0;

    /// 查询深港通静态数据
    /// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  SnapHKSZL1Static   -- 快照数据列表
    /// @return RetCode             -- 错误代码
    virtual RetCode QuerySnap_HKSZL1_Static(char* pCodeList, CDataBuffer<HKSZL1_Static>& SnapHKSZL1Static) = 0;

    /// 查询深港通实时行情数据
    /// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  SnapHKSZL1Quotation   -- 快照数据列表
    /// @return RetCode             -- 错误代码
    virtual RetCode QuerySnap_HKSZL1_Quotation(char* pCodeList, CDataBuffer<HKSZL1_Quotation>& SnapHKSZL1Quotation) = 0;

    /// 查询沪港通静态数据
    /// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  SnapHKSHL1Static   -- 快照数据列表
    /// @return RetCode             -- 错误代码
    virtual RetCode QuerySnap_HKSHL1_Static(char* pCodeList, CDataBuffer<HKSHL1_Static>& SnapHKSHL1Static) = 0;

    /// 查询沪港通实时行情数据
    /// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  SnapHKSHL1Quotation   -- 快照数据列表
    /// @return RetCode             -- 错误代码
    virtual RetCode QuerySnap_HKSHL1_Quotation(char* pCodeList, CDataBuffer<HKSHL1_Quotation>& SnapHKSHL1Quotation) = 0;

	/// 查询沪港通L1市场总览数据
	/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
	/// @param  SnapHKSHL1Overview   -- 快照数据列表
	/// @return RetCode             -- 错误代码
	virtual RetCode QuerySnap_HKSHL1_Overview(char* pCodeList, CDataBuffer<HKSHL1_Overview>& SnapHKSHL1Overview) = 0;

	/// 查询深港通L1市场总览数据
	/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
	/// @param  SnapHKSZL1Overview  -- 快照数据列表
	/// @return RetCode             -- 错误代码
	virtual RetCode QuerySnap_HKSZL1_Overview(char* pCodeList, CDataBuffer<HKSZL1_Overview>& SnapHKSZL1Overview) = 0;

	/// 查询上交所L2逐笔委托
	/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
	/// @param  SnapSSEL2Order      -- 快照数据列表
	/// @return RetCode             -- 错误代码
	virtual RetCode QuerySnap_SSEL2_Order(char* pCodeList, CDataBuffer<SSEL2_Order>& SnapSSEL2Order) = 0;

	/// 查询深交所L2国证指数
	/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
	/// @param  SnapSZSEL2Cnindex   -- 快照数据列表
	/// @return RetCode             -- 错误代码
	virtual RetCode QuerySnap_SZSEL2_Cnindex(char* pCodeList, CDataBuffer<SZSEL2_Cnindex>& SnapSZSEL2Cnindex) = 0;

	/// 查询上交所L2债券静态数据
	/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
	/// @param  SnapSSEL2BondStatic -- 快照数据列表
	/// @return RetCode             -- 错误代码
	virtual RetCode QuerySnap_SSEL2_BondStatic(char* pCodeList, CDataBuffer<SSEL2_BondStatic>& SnapSSEL2BondStatic) = 0;

	/// 查询上交所L2债券实时行情
	/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
	/// @param  SnapSSEL2BondQuotation   -- 快照数据列表
	/// @return RetCode             -- 错误代码
	virtual RetCode QuerySnap_SSEL2_BondQuotation(char* pCodeList, CDataBuffer<SSEL2_BondQuotation>& SnapSSEL2BondQuotation) = 0;

	/// 查询上交所L2债券逐笔数据
	/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
	/// @param  SnapSSEL2BondTick   -- 快照数据列表
	/// @return RetCode             -- 错误代码
	virtual RetCode QuerySnap_SSEL2_BondTick(char* pCodeList, CDataBuffer<SSEL2_BondTick>& SnapSSEL2BondTick) = 0;

	/// 查询上交所L2债券市场总览
	/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
	/// @param  SnapSSEL2BondOverview   -- 快照数据列表
	/// @return RetCode             -- 错误代码
	virtual RetCode QuerySnap_SSEL2_BondOverview(char* pCodeList, CDataBuffer<SSEL2_BondOverview>& SnapSSEL2BondOverview) = 0;

	/// 查询上交所L1债券静态数据
	/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
	/// @param  SnapSSEL1BondStatic -- 快照数据列表
	/// @return RetCode             -- 错误代码
	virtual RetCode QuerySnap_SSEL1_BondStatic(char* pCodeList, CDataBuffer<SSEL1_BondStatic>& SnapSSEL1BondStatic) = 0;

	/// 查询上交所L1债券实时行情
	/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
	/// @param  SnapSSEL1BondQuotation   -- 快照数据列表
	/// @return RetCode             -- 错误代码
	virtual RetCode QuerySnap_SSEL1_BondQuotation(char* pCodeList, CDataBuffer<SSEL1_BondQuotation>& SnapSSEL1BondQuotation) = 0;

	/// 查询深交所L2债券静态数据
	/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
	/// @param  SnapSZSEL2BondStatic -- 快照数据列表
	/// @return RetCode             -- 错误代码
	virtual RetCode QuerySnap_SZSEL2_BondStatic(char* pCodeList, CDataBuffer<SZSEL2_BondStatic>& SnapSZSEL2BondStatic) = 0;

	/// 查询深交所L2债券实时行情
	/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
	/// @param  SnapSZSEL2BondQuotation -- 快照数据列表
	/// @return RetCode             -- 错误代码
	virtual RetCode QuerySnap_SZSEL2_BondQuotation(char* pCodeList, CDataBuffer<SZSEL2_BondQuotation>& SnapSZSEL2BondQuotation) = 0;

	/// 查询深交所L2债券逐笔成交
	/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
	/// @param  SnapSZSEL2BondTransaction -- 快照数据列表
	/// @return RetCode             -- 错误代码
	virtual RetCode QuerySnap_SZSEL2_BondTransaction(char* pCodeList, CDataBuffer<SZSEL2_BondTransaction>& SnapSZSEL2BondTransaction) = 0;

	/// 查询深交所L2债券逐笔委托
	/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
	/// @param  SnapSZSEL2BondOrder -- 快照数据列表
	/// @return RetCode             -- 错误代码
	virtual RetCode QuerySnap_SZSEL2_BondOrder(char* pCodeList, CDataBuffer<SZSEL2_BondOrder>& SnapSZSEL2BondOrder) = 0;

	/// 查询深交所L2债券逐比消息
	/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
	/// @param  SnapSZSEL2BondTick  -- 快照数据列表
	/// @return RetCode             -- 错误代码
	virtual RetCode QuerySnap_SZSEL2_BondTick(char* pCodeList, CDataBuffer<SZSEL2_BondTick>& SnapSZSEL2BondTick) = 0;

	/// 查询深交所L2债券交易商信息
	/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
	/// @param  SnapSZSEL2BondMbrInfo  -- 快照数据列表
	/// @return RetCode             -- 错误代码
	virtual RetCode QuerySnap_SZSEL2_BondMbrInfo(char* pCodeList, CDataBuffer<SZSEL2_BondMbrInfo>& SnapSZSEL2BondMbrInfo) = 0;

	/// 查询深交所L2债券交易主体信息
	/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
	/// @param  SnapSZSEL2BondInvestorInfo  -- 快照数据列表
	/// @return RetCode             -- 错误代码
	virtual RetCode QuerySnap_SZSEL2_BondInvestorInfo(char* pCodeList, CDataBuffer<SZSEL2_BondInvestorInfo>& SnapSZSEL2BondInvestorInfo) = 0;

	/// 查询深交所L2债券交易员信息
	/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
	/// @param  SnapSZSEL2BondTraderInfo  -- 快照数据列表
	/// @return RetCode             -- 错误代码
	virtual RetCode QuerySnap_SZSEL2_BondTraderInfo(char* pCodeList, CDataBuffer<SZSEL2_BondTraderInfo>& SnapSZSEL2BondTraderInfo) = 0;

	/// 查询深交所L1债券静态数据
	/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
	/// @param  SnapSZSEL1BondStatic  -- 快照数据列表
	/// @return RetCode             -- 错误代码
	virtual RetCode QuerySnap_SZSEL1_BondStatic(char* pCodeList, CDataBuffer<SZSEL1_BondStatic>& SnapSZSEL1BondStatic) = 0;

	/// 查询深交所L1债券实时行情
	/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
	/// @param  SnapSZSEL1BondQuotation  -- 快照数据列表
	/// @return RetCode             -- 错误代码
	virtual RetCode QuerySnap_SZSEL1_BondQuotation(char* pCodeList, CDataBuffer<SZSEL1_BondQuotation>& SnapSZSEL1BondQuotation) = 0;

	/// 查询北交所L1静态数据
	/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
	/// @param  SnapBSEL1Static     -- 快照数据列表
	/// @return RetCode             -- 错误代码
	virtual RetCode QuerySnap_BSEL1_Static(char* pCodeList, CDataBuffer<BSEL1_Static>& SnapBSEL1Static) = 0;

	/// 查询北交所L1实时行情
	/// @param  pCodeList           -- 快照订阅列表，NULL时为全市场快照数据
	/// @param  SnapBSEL1Quotation  -- 快照数据列表
	/// @return RetCode             -- 错误代码
	virtual RetCode QuerySnap_BSEL1_Quotation(char* pCodeList, CDataBuffer<BSEL1_Quotation>& SnapBSEL1Quotation) = 0;

	/// 查询深交所L2债券市场竞买预约信息
	/// @param  pCodeList                     -- 快照订阅列表，NULL时为全市场快照数据
	/// @param  SnapSZSEL2BondBidBookingInfo  -- 快照数据列表
	/// @return RetCode                       -- 错误代码
	virtual RetCode QuerySnap_SZSEL2_BondBidBookingInfo(char* pCodeList, CDataBuffer<SZSEL2_BondBidBookingInfo>& SnapSZSEL2BondBidBookingInfo) = 0;

	/// 查询郑商所L2静态数据
	/// @param  pCodeList                     -- 快照订阅列表，NULL时为全市场快照数据
	/// @param  SnapCZCEL2Static  -- 快照数据列表
	/// @return RetCode                       -- 错误代码
	virtual RetCode QuerySnap_CZCEL2_Static(char* pCodeList, CDataBuffer<CZCEL2_Static>& SnapCZCEL2Static) = 0;

	/// 查询郑商所L2市场状态
	/// @param  pCodeList                     -- 快照订阅列表，NULL时为全市场快照数据
	/// @param  SnapCZCEL2MktStatus  -- 快照数据列表
	/// @return RetCode                       -- 错误代码
	virtual RetCode QuerySnap_CZCEL2_MktStatus(char* pCodeList, CDataBuffer<CZCEL2_MktStatus>& SnapCZCEL2MktStatus) = 0;

	/// 查询郑商所L2品种状态
	/// @param  pCodeList                     -- 快照订阅列表，NULL时为全市场快照数据
	/// @param  SnapCZCEL2Status  -- 快照数据列表
	/// @return RetCode                       -- 错误代码
	virtual RetCode QuerySnap_CZCEL2_Status(char* pCodeList, CDataBuffer<CZCEL2_Status>& SnapCZCEL2Status) = 0;

	/// 查询郑商所L2实时行情
	/// @param  pCodeList                     -- 快照订阅列表，NULL时为全市场快照数据
	/// @param  SnapCZCEL2Quotation  -- 快照数据列表
	/// @return RetCode                       -- 错误代码
	virtual RetCode QuerySnap_CZCEL2_Quotation(char* pCodeList, CDataBuffer<CZCEL2_Quotation>& SnapCZCEL2Quotation) = 0;

	/// 查询郑商所L2五档深度委托行情
	/// @param  pCodeList                     -- 快照订阅列表，NULL时为全市场快照数据
	/// @param  SnapCZCEL2LevelQuotation  -- 快照数据列表
	/// @return RetCode                       -- 错误代码
	virtual RetCode QuerySnap_CZCEL2_LevelQuotation(char* pCodeList, CDataBuffer<CZCEL2_LevelQuotation>& SnapCZCEL2LevelQuotation) = 0;

    /// 查询郑商所L2套利行情快照
    /// @param  pCodeList               -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  data    -- 快照数据列表
    /// @return RetCode                 -- 错误代码
    virtual RetCode QuerySnap_CZCEL2_ArbiQuotation(char* pCodeList, CDataBuffer<CZCEL2_ArbiQuotation>& data) = 0;
    /// 查询郑商所L2套利五档深度行情快照
    /// @param  pCodeList               -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  data    -- 快照数据列表
    /// @return RetCode                 -- 错误代码
    virtual RetCode QuerySnap_CZCEL2_ArbiLevelQuotation(char* pCodeList, CDataBuffer<CZCEL2_ArbiLevelQuotation>& data) = 0;

	/// 查询上交所L2精简实时行情
	/// @param  pCodeList                     -- 快照订阅列表，NULL时为全市场快照数据
	/// @param  SnapSSEL2ShortQuotation       -- 快照数据列表
	/// @return RetCode                       -- 错误代码
	virtual RetCode QuerySnap_SSEL2_ShortQuotation(char* pCodeList, CDataBuffer<SSEL2_ShortQuotation>& SnapSSEL2ShortQuotation) = 0;

	/// 查询上交所L2委托队列
	/// @param  pCodeList                     -- 快照订阅列表，NULL时为全市场快照数据
	/// @param  SnapSSEL2OrderQueue           -- 快照数据列表
	/// @return RetCode                       -- 错误代码
	virtual RetCode QuerySnap_SSEL2_OrderQueue(char* pCodeList, CDataBuffer<SSEL2_OrderQueue>& SnapSSEL2OrderQueue) = 0;

	/// 查询深交所L2精简实时行情
	/// @param  pCodeList                     -- 快照订阅列表，NULL时为全市场快照数据
	/// @param  SnapSZSEL2ShortQuotation      -- 快照数据列表
	/// @return RetCode                       -- 错误代码
	virtual RetCode QuerySnap_SZSEL2_ShortQuotation(char* pCodeList, CDataBuffer<SZSEL2_ShortQuotation>& SnapSZSEL2ShortQuotation) = 0;

	/// 查询深交所L2委托队列
	/// @param  pCodeList                     -- 快照订阅列表，NULL时为全市场快照数据
	/// @param  SnapSZSEL2OrderQueue          -- 快照数据列表
	/// @return RetCode                       -- 错误代码
	virtual RetCode QuerySnap_SZSEL2_OrderQueue(char* pCodeList, CDataBuffer<SZSEL2_OrderQueue>& SnapSZSEL2OrderQueue) = 0;

	/// 查询上交所L2 ETF统计数据快照
	/// @param  pCodeList               -- 快照订阅列表，NULL时为全市场快照数据
	/// @param  data    -- 快照数据列表
	/// @return RetCode                 -- 错误代码
	virtual RetCode QuerySnap_SSEL2_ETF(char* pCodeList, CDataBuffer<SSEL2_ETF>& data) = 0;

    /// 查询广期所L1静态数据快照
    /// @param  pCodeList               -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  data    -- 快照数据列表
    /// @return RetCode                 -- 错误代码
    virtual RetCode QuerySnap_GFEXL1_Static(char* pCodeList, CDataBuffer<GFEXL1_Static>& data) = 0;
    /// 查询广期所L1套利行情快照
    /// @param  pCodeList               -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  data    -- 快照数据列表
    /// @return RetCode                 -- 错误代码
    virtual RetCode QuerySnap_GFEXL1_ArbiQuotation(char* pCodeList, CDataBuffer<GFEXL1_ArbiQuotation>& data) = 0;
    /// 查询广期所L1实时行情快照
    /// @param  pCodeList               -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  data    -- 快照数据列表
    /// @return RetCode                 -- 错误代码
    virtual RetCode QuerySnap_GFEXL1_Quotation(char* pCodeList, CDataBuffer<GFEXL1_Quotation>& data) = 0;
    /// 查询广期所L1期权参数快照
    /// @param  pCodeList               -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  data    -- 快照数据列表
    /// @return RetCode                 -- 错误代码
    virtual RetCode QuerySnap_GFEXL1_OptionParameter(char* pCodeList, CDataBuffer<GFEXL1_OptionParameter>& data) = 0;
    /// 查询广期所L2静态数据快照
    /// @param  pCodeList               -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  data    -- 快照数据列表
    /// @return RetCode                 -- 错误代码
    virtual RetCode QuerySnap_GFEXL2_Static(char* pCodeList, CDataBuffer<GFEXL2_Static>& data) = 0;
    /// 查询广期所L2套利行情快照
    /// @param  pCodeList               -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  data    -- 快照数据列表
    /// @return RetCode                 -- 错误代码
    virtual RetCode QuerySnap_GFEXL2_ArbiQuotation(char* pCodeList, CDataBuffer<GFEXL2_ArbiQuotation>& data) = 0;
    /// 查询广期所L2实时行情快照
    /// @param  pCodeList               -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  data    -- 快照数据列表
    /// @return RetCode                 -- 错误代码
    virtual RetCode QuerySnap_GFEXL2_Quotation(char* pCodeList, CDataBuffer<GFEXL2_Quotation>& data) = 0;
    /// 查询广期所L2委托队列行情快照
    /// @param  pCodeList               -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  data    -- 快照数据列表
    /// @return RetCode                 -- 错误代码
    virtual RetCode QuerySnap_GFEXL2_OrderQueue(char* pCodeList, CDataBuffer<GFEXL2_OrderQueue>& data) = 0;
    /// 查询广期所L2委托统计行情快照
    /// @param  pCodeList               -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  data    -- 快照数据列表
    /// @return RetCode                 -- 错误代码
    virtual RetCode QuerySnap_GFEXL2_OrderStatistic(char* pCodeList, CDataBuffer<GFEXL2_OrderStatistic>& data) = 0;
    /// 查询广期所L2实时结算价快照
    /// @param  pCodeList               -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  data    -- 快照数据列表
    /// @return RetCode                 -- 错误代码
    virtual RetCode QuerySnap_GFEXL2_RealTimePrice(char* pCodeList, CDataBuffer<GFEXL2_RealTimePrice>& data) = 0;
    /// 查询广期所L2分价成交量行情快照
    /// @param  pCodeList               -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  data    -- 快照数据列表
    /// @return RetCode                 -- 错误代码
    virtual RetCode QuerySnap_GFEXL2_MarchPriceQty(char* pCodeList, CDataBuffer<GFEXL2_MarchPriceQty>& data) = 0;
    /// 查询广期所L2期权参数快照
    /// @param  pCodeList               -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  data    -- 快照数据列表
    /// @return RetCode                 -- 错误代码
    virtual RetCode QuerySnap_GFEXL2_OptionParameter(char* pCodeList, CDataBuffer<GFEXL2_OptionParameter>& data) = 0;
    /// 查询广期所L2套利委托队列快照
    /// @param  pCodeList               -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  data    -- 快照数据列表
    /// @return RetCode                 -- 错误代码
    virtual RetCode QuerySnap_GFEXL2_ArbiOrderQueue(char* pCodeList, CDataBuffer<GFEXL2_ArbiOrderQueue>& data) = 0;

    /// 查询上交所L2盘后固定价格逐笔成交快照
    /// @param  pCodeList               -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  data    -- 快照数据列表
    /// @return RetCode                 -- 错误代码
    virtual RetCode QuerySnap_SSEL2_FpTransaction(char* pCodeList, CDataBuffer<SSEL2_FpTransaction>& data) = 0;
    /// 查询上交所L2逐笔行情快照
    /// @param  pCodeList               -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  data    -- 快照数据列表
    /// @return RetCode                 -- 错误代码
    virtual RetCode QuerySnap_SSEL2_Tick(char* pCodeList, CDataBuffer<SSEL2_Tick>& data) = 0;

    /// 查询大商所指数行情 快照
    /// @param  pCodeList               -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  data    -- 快照数据列表
    /// @return RetCode                 -- 错误代码
    virtual RetCode QuerySnap_DCE_Index(char* pCodeList, CDataBuffer<DCE_Index>& data) = 0;

    /// 查询上交所L1盘后固定价格行情快照
    /// @param  pCodeList               -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  data    -- 快照数据列表
    /// @return RetCode                 -- 错误代码
    virtual RetCode QuerySnap_SSEL1_FpQuotation(char* pCodeList, CDataBuffer<SSEL1_FpQuotation>& data) = 0;
    /// 查询上交所L2盘后固定价格行情快照
    /// @param  pCodeList               -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  data    -- 快照数据列表
    /// @return RetCode                 -- 错误代码
    virtual RetCode QuerySnap_SSEL2_FpQuotation(char* pCodeList, CDataBuffer<SSEL2_FpQuotation>& data) = 0;
    /// 查询深交所L1盘后固定价格行情快照
    /// @param  pCodeList               -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  data    -- 快照数据列表
    /// @return RetCode                 -- 错误代码
    virtual RetCode QuerySnap_SZSEL1_FpQuotation(char* pCodeList, CDataBuffer<SZSEL1_FpQuotation>& data) = 0;
    /// 查询深交所L2盘后固定价格行情快照
    /// @param  pCodeList               -- 快照订阅列表，NULL时为全市场快照数据
    /// @param  data    -- 快照数据列表
    /// @return RetCode                 -- 错误代码
    virtual RetCode QuerySnap_SZSEL2_FpQuotation(char* pCodeList, CDataBuffer<SZSEL2_FpQuotation>& data) = 0;
	};
	#endif // GTA_QTS_INTERFACE_BASE_H_