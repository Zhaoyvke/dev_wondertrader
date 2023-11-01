// dllmain.cpp : 定义 DLL 应用程序的入口点。

/*!
 * \file ParserOTS.h
 * \project	WonderTrader
 *
 * \author kanavi
 * \date 2023/08/30
 *
 * \brief
 */

#include "ParserQTS.h"

#include "../Share/StrUtil.hpp"
#include "../Share/TimeUtils.hpp"
#include "../Share/StdUtils.hpp"
#include "../Share/DLLHelper.hpp"
#include "../Share/charconv.hpp"

#include "../Includes/WTSDataDef.hpp"
#include "../Includes/WTSContractInfo.hpp"
#include "../Includes/WTSVariant.hpp"
#include "../Includes/IBaseDataMgr.h"

#include <boost/filesystem.hpp>
#include <iostream>

#include "../Share/fmtlib.h"
template<typename... Args>
inline void write_log(IParserSpi* sink, WTSLogLevel ll, const char* format, const Args&... args)
{
	if (sink == NULL)
		return;

	static thread_local char buffer[512] = { 0 };
	fmtutil::format_to(buffer, format, args...);

	sink->handleParserLog(ll, buffer);
}

inline double checkValid(double val)
{
	if (val == DBL_MAX)
		return 0;

	return val;
}
extern "C"
{
	EXPORT_FLAG IParserApi* createParser()
	{
		ParserQTS* parser = new ParserQTS();
		return parser;
	}

	EXPORT_FLAG void deleteParser(IParserApi* &parser)
	{
		if (NULL != parser)
		{
			delete parser;
			parser = NULL;
		}
	}
};
ParserQTS::ParserQTS()
	: m_pUserAPI(NULL)
	, m_pBaseDataMgr(NULL)
	, m_Servers(NULL)
	, m_bLogined(false)
	, iCode(-1)
	, m_bHasSSE(false)
	, m_bHasSZSE(false)
	, m_bHasL1(false)
	,m_bHasL2(false)
{
}

ParserQTS::~ParserQTS()
{
	if (m_pUserAPI)
		IGTAQTSApiBase::ReleaseInstance(m_pUserAPI);
	cout << "ReleaseAPIInstance OK,Exit... " << endl;
}

bool ParserQTS::init(WTSVariant * config)
{
	m_Servers = config->get("servers");
	m_strUser = config->getCString("user");
	m_strPass = config->getCString("pass");
	m_iHeatBeat = config->getInt32("heartbeat");
	m_bHasL1 = config->getBoolean("has_level1");
	m_bHasL2 = config->getBoolean("has_level2");

	m_bHasSSE = config->getBoolean("has_sse");
	m_bHasSZSE = config->getBoolean("has_szse");
	//登陆的网络类型
	// NetType表示连接网络类型，可选值为0，1.
	// 0表示公网接入(默认值)，1表示特殊网络接入（内网、专线等），如："NetType=1".
	// 改变NetType值重新登录可实现不同的网络环境接入切换
	m_strNetType = config->getCString("NetType");

	m_strLogdir = "log";  // 必须指定为log目录，否则API日志到不到目录会报错

	{
		std::string path = StrUtil::standardisePath(m_strLogdir);
		if (!StdFile::exists(path.c_str()))
			boost::filesystem::create_directories(path.c_str());
	}

	std::string module = config->getCString("qtsmodule");
	if (module.empty())
	{
		module = "GTAQTSApi";
	} 

	//std::string dllpath = getBinDir() + DLLHelper::wrap_module(module.c_str(), "");
	//m_hInstQTS = DLLHelper::load_library(dllpath.c_str());

	return true;

}

void ParserQTS::release()
{
	disconnect();
}

bool ParserQTS::connect()
{
	cout << "connect" << endl;


	//ParserQTS			subMsgCallInstance;
	m_pUserAPI = IGTAQTSApiBase::CreateInstance(*this);
	if (NULL == m_pUserAPI)
	{//创建失败
		cout << "fail to CreateInstance of  IGTAQTSApiBase,and return " << endl;
		return false;
	}
	//m_pUserAPI = IGTAQTSApiBase::CreateInstance(*this);

	m_pUserAPI->SetTimeout(30);  // 超时30s
	m_pUserAPI->SetHeartBeatTime(3);  // 心跳超时时长，默认3秒
	// 注册，注册FENS地址
	bool m_bConnected = false;
	if (m_Servers)
	{
		for (uint32_t i = 0; i < m_Servers->size(); i++)
		{
			WTSVariant* cfgItem = m_Servers->get(i);

			int ret = m_pUserAPI->RegisterService(cfgItem->getCString("host"), cfgItem->getUInt32("port"));
			m_bConnected = m_bConnected || (ret == 0);
		}
	}

	if (m_bConnected)
		write_log(m_sink, LL_INFO, "[PaserQTS] Register Service Success!");
	else
	{
		write_log(m_sink, LL_ERROR, "[PaserQTS] Register Service Fail!");
		return false;
	}

	DoLogin();
	return true;
}

bool ParserQTS::disconnect()
{
	if (m_pUserAPI)
	{
		//ios.Stop();//退出工作线程
		IGTAQTSApiBase::ReleaseInstance(m_pUserAPI);
		m_pUserAPI = NULL;
	}
	return false;
}

bool ParserQTS::isConnected()
{
	return m_pUserAPI != NULL;
}

void ParserQTS::subscribe(const CodeSet & vecSymbols)
{
	cout << "subsribe" << endl;
}

void ParserQTS::unsubscribe(const CodeSet & vecSymbols)
{
}

void ParserQTS::registerSpi(IParserSpi * listener)
{
	cout << "registerSpi" << endl;
	m_sink = listener;

	if (m_sink)
		m_pBaseDataMgr = m_sink->getBaseDataMgr();
	
}

void ParserQTS::OnConnectionState(MsgType msgType, RetCode errCode)
{
	if (errCode == Ret_Success)
		write_log(m_sink, LL_INFO, "[ParserQTS] {} Connect!", m_strUser);
	else
		write_log(m_sink, LL_WARN, "[ParserQTS] {} Disconnect: ErrCode: {}!", m_strUser, errCode);
}
void ParserQTS::DoLogin()
{
	cout << "DoLogin" << endl;

	iCode = m_pUserAPI->LoginX(m_strUser.c_str(), m_strPass.c_str(), m_strNetType.c_str());
	if (0 != iCode)
	{//连接行情服务器失败
		//cout << "fail to LoginX, ErrorCode: " << iCode << endl;
		write_log(m_sink, LL_ERROR, "Error: [ParserQTS] fail to LoginX, ErrorCode:{}", iCode);
		release();
		m_sink->handleEvent(WPE_Connect, -1);
		return;
	}
	else
	{
		m_bLogined = true;
		write_log(m_sink, LL_DEBUG, " [ParserQTS]Login Success !");
		if (m_sink)
		{
			m_sink->handleEvent(WPE_Connect, 0);
			m_sink->handleEvent(WPE_Login, 0);
		}
	}

	SubscribeMDs(false);

}

void ParserQTS::SubscribeMDs(bool isAll)
{
	cout << "SubscribeMDs" << endl;

	int iCode;
	if (m_bHasSSE && m_bHasL1)
	{
		////上交所L1 实时行情订阅  股票和基金
		std::string codes;
		if (m_bHasShare)
		{
			codes += m_strSHSTKCodes;
		}

		if (m_bHasETF)
		{
			if (codes.size() > 0)
				codes += ",";
			codes += m_strSHETFCodes;
		}
		iCode = m_pUserAPI->Subscribe(Msg_SSEL1_Quotation, (char*)codes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_sink, LL_WARN, "[ParserQTS] [ParserQTS] Fail to Subscribe,Msg_SSEL1_Quotation, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_sink, LL_WARN, "[ParserQTS] [ParserQTS] Subscribe  Msg_SSEL1_Quotation Success,Start to Receive Data...");
		}
	}

	////////====>深交所L1实时行情订阅
	if (m_bHasSZSE && m_bHasL1)
	{
		std::string codes;
		if (m_bHasShare)
			codes += m_strSZSTKCodes;

		if (m_bHasETF)
		{
			if (codes.size() > 0)
				codes += ",";
			codes += m_strSZETFCodes;
		}

		iCode = m_pUserAPI->Subscribe(Msg_SZSEL1_Quotation, (char*)codes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_sink, LL_WARN, "[ParserQTS] Fail to Subscribe,Msg_SZSEL1_Quotation, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_sink, LL_WARN, "[ParserQTS] Subscribe  Msg_SZSEL1_Quotation Success,Start to Receive Data ...");
		}
	}



}


void ParserQTS::OnSubscribe_SSEL1_Quotation(const SSEL1_Quotation& RealSSEL1Quotation)
{
	const char* code = RealSSEL1Quotation.Symbol;
	const char* exchg = "SSE";

	WTSContractInfo* ct = m_pBaseDataMgr->getContract(code, exchg);
	if (ct == NULL)
	{
		if (m_sink)
			//write_log(m_parserSink, LL_ERROR, "[ParserQTS] Instrument {}.{} not exists...", exchg, code);
			return;
	}

	WTSCommodityInfo* commInfo = ct->getCommInfo();

	WTSTickData* tick = WTSTickData::create(code);
	tick->setContractInfo(ct);
	//WTSTickStruct& quote = tick->getTickStruct();
	//strcpy(quote.exchg, exchg);

	//quote.action_date = (uint32_t)(RealSSEL1Quotation.PacketTimeStamp / 1000000000);  // YYYYMMDDHHMMSSMMM
	//quote.action_time = RealSSEL1Quotation.Time;  // HHMMssmmm
	//quote.trading_date = TimeUtils::getCurDate();

	//quote.price = checkValid(RealSSEL1Quotation.LastPrice);
	//quote.open = checkValid(RealSSEL1Quotation.OpenPrice);
	//quote.high = checkValid(RealSSEL1Quotation.HighPrice);
	//quote.low = checkValid(RealSSEL1Quotation.LowPrice);
	//quote.total_volume = (uint32_t)RealSSEL1Quotation.TotalVolume;
	//quote.total_turnover = checkValid(RealSSEL1Quotation.TotalAmount);

	//quote.pre_close = checkValid(RealSSEL1Quotation.PreClosePrice);

	//quote.ask_prices[0] = checkValid(RealSSEL1Quotation.SellPrice01);
	//quote.ask_qty[0] = RealSSEL1Quotation.SellVolume01;
	//quote.ask_prices[1] = checkValid(RealSSEL1Quotation.SellPrice02);
	//quote.ask_qty[1] = RealSSEL1Quotation.SellVolume02;
	//quote.ask_prices[2] = checkValid(RealSSEL1Quotation.SellPrice03);
	//quote.ask_qty[2] = RealSSEL1Quotation.SellVolume03;
	//quote.ask_prices[3] = checkValid(RealSSEL1Quotation.SellPrice04);
	//quote.ask_qty[3] = RealSSEL1Quotation.SellVolume04;
	//quote.ask_prices[4] = checkValid(RealSSEL1Quotation.SellPrice05);
	//quote.ask_qty[4] = RealSSEL1Quotation.SellVolume05;

	//quote.bid_prices[0] = checkValid(RealSSEL1Quotation.BuyPrice01);
	//quote.bid_qty[0] = RealSSEL1Quotation.BuyVolume01;
	//quote.bid_prices[1] = checkValid(RealSSEL1Quotation.BuyPrice02);
	//quote.bid_qty[1] = RealSSEL1Quotation.BuyVolume02;
	//quote.bid_prices[2] = checkValid(RealSSEL1Quotation.BuyPrice03);
	//quote.bid_qty[2] = RealSSEL1Quotation.BuyVolume03;
	//quote.bid_prices[3] = checkValid(RealSSEL1Quotation.BuyPrice04);
	//quote.bid_qty[3] = RealSSEL1Quotation.BuyVolume04;
	//quote.bid_prices[4] = checkValid(RealSSEL1Quotation.BuyPrice05);
	//quote.bid_qty[4] = RealSSEL1Quotation.BuyVolume05;

	if (m_sink)
		m_sink->handleQuote(tick, 1);

	tick->release();
}