/*!
 * \file ParserQTS.h
 * \project	WonderTrader
 *
 * \author Suuuuuuuuunseeker
 * \date 2023/08/29
 * 
 * \brief 
 */
#pragma once
#include <map>
#include <string>

#if _WIN32
#ifndef WINAPI
#define WINAPI __stdcall
#endif
#endif

#include "../Includes/IParserApi.h"
#include "../Share/DLLHelper.hpp"
#include "../API/GTAQTSApi/include/QTSDataType.h"
#include "../API/GTAQTSApi/include/QTSStruct.h"
#include "../API/GTAQTSApi/include/GTAQTSInterfaceBase.h"


NS_WTP_BEGIN
class WTSTickData;
NS_WTP_END

USING_NS_WTP;

enum ContractType
{
	M_INDX,
	M_BOND,
	M_STK,
	M_ETF,
	M_OPT,
	M_FUT,
	M_UNKNOWN
};

class ParserQTS : public IParserApi, public IGTAQTSCallbackBase
{
public:
	ParserQTS();
	virtual ~ParserQTS();

public:
	enum LoginStatus
	{
		LS_NOTLOGIN,
		LS_LOGINING,
		LS_LOGINED
	};

//IQuoteParser 接口
public:
	virtual bool init(WTSVariant* config) override;

	virtual void release() override;

	virtual bool connect() override;

	virtual bool disconnect() override;

	virtual bool isConnected() override;

	virtual void subscribe(const CodeSet &vecSymbols) override;
	virtual void unsubscribe(const CodeSet &vecSymbols) override;

	virtual void registerSpi(IParserSpi* listener) override;

//////////////////////////////////////////////////////////////////////////
public:
	/// 连接状态返回，连接成功/失败
	/// @param  msgType     -- 消息类型
	/// @param  errCode     -- 失败原因，成功时返回0 
	///                     详见《实时行情系统V2.X 用户手册》5.5返回码含义列表RetCode 章节 
	virtual void OnConnectionState(MsgType msgType, RetCode errCode);

	//////////////////////////////////////  上交所  ////////////////////////////////
	/// 上交所L1实时行情快照订阅数据实时回调接口
	/// @param  RealSSEL1Quotation  -- 实时数据
	virtual void OnSubscribe_SSEL1_Quotation(const SSEL1_Quotation& RealSSEL1Quotation);

	/// 上交所L1债券实时行情订阅数据实时回调接口
	/// @param  RealSSEL1BondQuotation  -- 实时债券数据
	virtual void OnSubscribe_SSEL1_BondQuotation(const SSEL1_BondQuotation& RealSSEL1BondQuotation);

	/// 上交所L2实时行情快照订阅数据实时回调接口
	/// @param  RealSSEL2Quotation  -- 实时数据
	virtual void OnSubscribe_SSEL2_Quotation(const SSEL2_Quotation& RealSSEL2Quotation);

	/// 上交所L2指数行情订阅数据实时回调接口
	/// @param  ReRealSSEL2Index    -- 实时数据
	virtual void OnSubscribe_SSEL2_Index(const SSEL2_Index& RealSSEL2Index);

	/// 上交所L2债券实时行情订阅数据实时回调接口
	/// @param  RealSSEL2BondQuotation  -- 实时债券数据
	virtual void OnSubscribe_SSEL2_BondQuotation(const SSEL2_BondQuotation& RealSSEL2BondQuotation);

	/// 上交所L2债券逐笔订阅数据实时回调接口
	/// @param  RealSSEL2BondQuotation  -- 实时债券数据
	virtual void OnSubscribe_SSEL2_BondTick(const SSEL2_BondTick& RealSSEL2BondTick);

	/// 上交所L2逐笔成交订阅数据实时回调接口
	/// @param  RealSSEL2Transaction    -- 实时数据
	virtual void OnSubscribe_SSEL2_Transaction(const SSEL2_Transaction& RealSSEL2Transaction);

	/// 上交所L2逐笔委托订阅数据实时回调接口
	/// @param  RealSSEL2Order    -- 实时数据
	virtual void OnSubscribe_SSEL2_Order(const SSEL2_Order& RealSSEL2Order);

	/// 上交所个股期权L1实时行情订阅数据实时回调接口
	/// @param  RealSSEIOL1Quotation  -- 实时数据
	virtual void OnSubscribe_SSEIOL1_Quotation(const SSEIOL1_Quotation& RealSSEIOL1Quotation);


	/// 上交所L2 ETF统计数据订阅数据实时回调接口
	/// @param  RealSSEL2Order    -- 实时数据
	/// 尚未上线
	//virtual void OnSubscribe_SSEL2_ETF(const SSEL2_ETF& data);

	virtual void OnSubscribe_SSEL2_ShortQuotation(const SSEL2_ShortQuotation& RealSSEL2_ShortQuotation);

	virtual void OnSubscribe_SSEL2_OrderQueue(const SSEL2_OrderQueue& RealSSEL2_OrderQueue);

	// 尚未上线
	//virtual void OnSubscribe_SSEL2_Tick(const SSEL2_Tick& data);

	//virtual void OnSubscribe_SSEL1_FpQuotation(const SSEL1_FpQuotation& data);

	//virtual void OnSubscribe_SSEL2_FpQuotation(const SSEL2_FpQuotation& data);

	//////////////////////////////////////  深交所  ////////////////////////////////

	/// 深交所L1实时行情订阅数据实时回调接口
	/// @param  RealSZSEL1Quotation -- 实时数据
	virtual void OnSubscribe_SZSEL1_Quotation(const SZSEL1_Quotation& RealSZSEL1Quotation);

	/// 深交所L2实时行情订阅数据实时回调接口
	/// @param  RealSZSEL2Quotation -- 实时数据
	virtual void OnSubscribe_SZSEL2_Quotation(const SZSEL2_Quotation& RealSZSEL2Quotation);

	/// 深交所L2逐笔委托行情订阅数据实时回调接口
	/// @param  RealSZSEL2Transaction -- 实时数据
	virtual void OnSubscribe_SZSEL2_Transaction(const SZSEL2_Transaction& RealSZSEL2Transaction);

	/// 深交所L2逐笔委托行情订阅数据实时回调接口
	/// @param  RealSZSEL2Order -- 实时数据
	virtual void OnSubscribe_SZSEL2_Order(const SZSEL2_Order& RealSZSEL2Order);

	/// 深交所L2指数行情订阅数据实时回调接口
	/// @param  RealSZSEL2Index -- 实时数据
	virtual void OnSubscribe_SZSEL2_Index(const SZSEL2_Index& RealSZSEL2Index);

	/// 订阅深交所L2债券实时行情数据实时回调接口
	/// @param  RealSZSEL2BondQuotation -- 实时数据
	virtual void OnSubscribe_SZSEL2_BondQuotation(const SZSEL2_BondQuotation& RealSZSEL2BondQuotation);

	/// 订阅深交所L2债券逐笔成交数据实时回调接口
	/// @param  RealSZSEL2BondTransaction -- 实时数据
	virtual void OnSubscribe_SZSEL2_BondTransaction(const SZSEL2_BondTransaction& RealSZSEL2BondTransaction);

	/// 订阅深交所L2债券逐笔委托实时行情数据实时回调接口
	/// @param  RealSZSEL2BondOrder -- 实时数据
	virtual void OnSubscribe_SZSEL2_BondOrder(const SZSEL2_BondOrder& RealSZSEL2BondOrder);

	/// 订阅深交所L2债券逐比数据实时回调接口 
	/// @param  RealSZSEL2_BondTick -- 实时数据
	virtual void OnSubscribe_SZSEL2_BondTick(const SZSEL2_BondTick& RealSZSEL2_BondTick);

	/// 订阅深交所L1债券实时行情实时回调接口
	/// @param  RealSZSEL1BondQuotation -- 实时数据
	virtual void OnSubscribe_SZSEL1_BondQuotation(const SZSEL1_BondQuotation& RealSZSEL1BondQuotation);

	/// 深交所L2债券市场竞买预约信息订阅数据实时回调接口
	/// @param  RealSZSEL2BondBidBookingInfo -- 实时数据
	//virtual void OnSubscribe_SZSEL2_BondBidBookingInfo(const SZSEL2_BondBidBookingInfo& RealSZSEL2BondBidBookingInfo);

	/// 订阅深交所个股期权L1实时行情实时回调接口
	/// @param  RealSZSEIOL1Quotation -- 实时数据
	virtual void OnSubscribe_SZSEIOL1_Quotation(const SZSEIOL1_Quotation& RealSZSEIOL1Quotation);

	virtual void OnSubscribe_SZSEL2_ShortQuotation(const SZSEL2_ShortQuotation& RealSZSEL2_ShortQuotation);
	virtual void OnSubscribe_SZSEL2_OrderQueue(const SZSEL2_OrderQueue& RealSZSEL2_OrderQueue);
	//virtual void OnSubscribe_SZSEL1_FpQuotation(const SZSEL1_FpQuotation& data);
	//virtual void OnSubscribe_SZSEL2_FpQuotation(const SZSEL2_FpQuotation& data);
	virtual void OnSubscribe_SZSEL2_Tick(const SZSEL2_Tick& data);

private:
	/*
	 *	发送登录请求
	 */
	void DoLogin();
	/*
	 *	订阅品种行情
	 */
	void SubscribeMDs(bool isAll = false);

private:
	uint32_t			m_uTradingDate;

	LoginStatus			m_loginState;
	IGTAQTSApiBase*		m_pUserAPI;  // 基础API对象

	std::string			m_strUser;
	std::string			m_strPass;

	// 登录网络类型
	// NetType表示连接网络类型，可选值为0，1.
	// 0表示公网接入(默认值)，1表示特殊网络接入（内网、专线等），如："NetType=1".
	// 改变NetType值重新登录可实现不同的网络环境接入切换
	std::string			m_strNetType; 
	
	WTSVariant*			m_Servers;

	std::string			m_strLogdir;  // 日志目录

	std::string			m_strSHSTKCodes;  // 上交所订阅股票合约
	std::string			m_strSZSTKCodes;  // 深交所订阅股票合约
	std::string			m_strSHOptCodes;  // 期权
	std::string			m_strSZOptCodes;
	std::string			m_strSHETFCodes;  // ETF
	std::string			m_strSZETFCodes;
	std::string			m_strSHBondCodes;  // 债券
	std::string			m_strSZBondCodes;
	std::string			m_strSHINDXCodes;  // 指数
	std::string			m_strSZINDXCodes;

	// 订阅的数据类型
	bool				m_bHasOrder;
	bool				m_bHasOrdQue;
	bool				m_bHasTrans;
	bool				m_bHasSnap;
	bool				m_bHasFp;  // 盘后固定价格行情
	bool				m_bHasArb;  // 统计套利行情

	bool				m_bHasL1;  // L1数据
	bool				m_bHasL2;  // L2数据

	bool				m_bHasSSE;  // 是否订阅上交所
	bool				m_bHasSZSE;  // 是否订阅深交所数据

	// 订阅的合约类型
	bool				m_bHasIndex;  // 指数
	bool				m_bHasShare;  // 股票
	bool				m_bHasOption;  // 期权
	bool				m_bHasBond;  // 债券
	bool				m_bHasETF;

	bool				m_bLogined;

	CodeSet				m_filterSubs;
	CodeSet				m_ignoreCodes;

	DllHandle			m_hInstQTS;

private:
	IParserSpi*			m_parserSink;
	IBaseDataMgr*		m_pBaseDataMgr;
};

