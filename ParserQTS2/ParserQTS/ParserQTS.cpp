/*!
 * \file ParserQTS.cpp
 * \project	WonderTrader
 *
 * \author Sunnseeeker
 * \date 2023/08/29
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
#include "../Includes/IBaseDataMgr.h"

#include <boost/filesystem.hpp>
#include <iostream>

#ifndef FLT_MAX
#define FLT_MAX 3.402823466e+38F
#endif

#ifdef _WIN32
#ifdef _WIN64
#pragma comment(lib, "../API/GTAQTSApi/x64/GTAQTSApi.lib")
#else
#pragma comment(lib, "../API/GTAQTSApi/x86/GTAQTSApi.lib")
#endif
#include <wtypes.h>
HMODULE	g_dllModule = NULL;

BOOL APIENTRY DllMain(
	HANDLE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		g_dllModule = (HMODULE)hModule;
		break;
	}
	return TRUE;
}
#else
#include <dlfcn.h>

std::string	g_moduleName;

__attribute__((constructor))
void on_load(void) {
	Dl_info dl_info;
	dladdr((void *)on_load, &dl_info);
	g_moduleName = dl_info.dli_fname;
}
#endif

//By Wesley @ 2022.01.05
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


std::string getBinDir()
{
	static std::string _bin_dir;
	if (_bin_dir.empty())
	{


#ifdef _WIN32
		char strPath[MAX_PATH];
		GetModuleFileName(g_dllModule, strPath, MAX_PATH);

		_bin_dir = StrUtil::standardisePath(strPath, false);
#else
		_bin_dir = g_moduleName;
#endif

		uint32_t nPos = _bin_dir.find_last_of('/');
		_bin_dir = _bin_dir.substr(0, nPos + 1);
	}

	return _bin_dir;
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

inline double checkValid(double val)
{
	if (val == DBL_MAX)
		return 0;

	return val;
}

inline WTSBSDirectType wrapTickDirecType(const char* tickBsFlag)
{
	char flag = tickBsFlag[0];

	if (flag == 'S' && tickBsFlag[1] == 'U')
		return BDT_Unknown;

	return (flag == 'B') ? BDT_Buy : ((flag == 'S') ? BDT_Sell : BDT_Unknown);
}

inline WTSBSDirectType wrapOrderCode(const char codeType)
{
	switch (codeType)
	{
	case '1':
		return BDT_Buy;
	case '2':
		return BDT_Sell;
	case 'G':
		return BDT_Borrow;
	case 'F':
		return BDT_Lend;
	default:
		return BDT_Unknown;
		break;
	}
}

inline WTSPriceType wrapOrderType(char orderType)
{
	switch (orderType)
	{
	case '1':
		return WPT_ANYPRICE;
	case '2':
		return WPT_LIMITPRICE;
	case 'U':
		return WPT_BESTPRICE;
	default:
		return WPT_LIMITPRICE;
		break;
	}
}

inline ContractType extractType(std::string& code, bool isSH)
{
	if (code.size() < 6)
		return M_UNKNOWN;

	char tp = code[0];
	switch (tp)
	{
	case '0':
	{
		if (isSH)
		{
			if (code.compare(0, 3, "000") == 0)
				return M_INDX;
			else
				return M_BOND;
		}
		else
			return M_STK;
	}
	break;
	case '1':
	{
		if (!isSH)
		{
			if (code.compare(0, 6, "159000") > 0)
				return M_ETF;
			else
				return M_BOND;
		}
		else
		{
			if (code.size() == 6)
				return M_BOND;
			else if (code.size() == 8)
				return M_OPT;
		}
		return M_UNKNOWN;
	}
	break;
	case '3':
	{
		if (!isSH)
		{
			if (code[1] == '9')
				return M_INDX;
			else
				return M_STK;
		}
		return M_FUT;
	}
	break;
	case '5':
	{
		return M_ETF;
	}
	break;
	case '6':
	{
		return M_STK;
	}
	case '9':
	{
		if (!isSH)
			return M_OPT;

		return M_UNKNOWN;
	}
	break;
	default:
	{
		return M_UNKNOWN;
	}
	break;
	}
}


ParserQTS::ParserQTS()
	: m_pUserAPI(NULL)
	, m_bLogined(false)
	, m_Servers(NULL)
	, m_bHasIndex(false)
	, m_bHasShare(false)
	, m_bHasBond(false)
	, m_bHasETF(false)
	, m_bHasOption(false)
	, m_bHasOrder(false)
	, m_bHasTrans(false)
	, m_bHasOrdQue(false)
	, m_bHasSnap(false)
	, m_bHasFp(false)
	, m_bHasArb(false)
	, m_bHasSSE(false)
	, m_bHasSZSE(false)
	, m_bHasL1(false)
	, m_bHasL2(false)
	, m_parserSink(NULL)
	, m_pBaseDataMgr(NULL)
{

}


ParserQTS::~ParserQTS()
{
	release();
}

bool ParserQTS::init(WTSVariant* config)
{
	m_Servers = config->get("servers");

	m_strUser = config->getCString("user");
	m_strPass = config->getCString("pass");

	m_strNetType = config->getCString("net_type");

	m_bHasOrder = config->getBoolean("has_order");
	m_bHasTrans = config->getBoolean("has_trans");
	m_bHasOrdQue = config->getBoolean("has_orderqueue");
	m_bHasSnap = config->getBoolean("has_snapshot");
	m_bHasFp = config->getBoolean("has_fp");
	m_bHasArb = config->getBoolean("has_arbitrage");

	m_bHasIndex = config->getBoolean("has_index");
	m_bHasShare = config->getBoolean("has_share");
	m_bHasETF = config->getBoolean("has_etf");
	m_bHasBond = config->getBoolean("has_bond");
	m_bHasOption = config->getBoolean("has_option");

	m_bHasL1 = config->getBoolean("has_level1");
	m_bHasL2 = config->getBoolean("has_level2");

	m_bHasSSE = config->getBoolean("has_sse");
	m_bHasSZSE = config->getBoolean("has_szse");
	
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

	std::string dllpath = getBinDir() + DLLHelper::wrap_module(module.c_str(), "");
	m_hInstQTS = DLLHelper::load_library(dllpath.c_str());

	return true;
}

void ParserQTS::release()
{
	disconnect();
}

bool ParserQTS::connect()
{
	// 订阅消息回调类
	//IGTAQTSCallbackBase m_CallbackBase;

	// 创建基础API对象
	m_pUserAPI = IGTAQTSApiBase::CreateInstance(*this);

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
		write_log(m_parserSink, LL_INFO, "[PaserQTS] Register Service Success!");
	else
	{
		write_log(m_parserSink, LL_ERROR, "[PaserQTS] Register Service Fail!");
		return false;
	}

	DoLogin();

	return true;
}

bool ParserQTS::disconnect()
{
	if(m_pUserAPI)
	{
		IGTAQTSApiBase::ReleaseInstance(m_pUserAPI);
		m_pUserAPI = NULL;
	}

	return true;
}

void ParserQTS::DoLogin()
{
	do 
	{
		// 登录，通过用户名和密码向服务器登录
		RetCode ret = m_pUserAPI->LoginX(m_strUser.c_str(), m_strPass.c_str(), m_strNetType.c_str());

		if (Ret_Success != ret)
		{
			write_log(m_parserSink, LL_ERROR, "[ParserQTS] {} Login fail: {}", m_strUser, ret);
			IGTAQTSApiBase::ReleaseInstance(m_pUserAPI);

			m_parserSink->handleEvent(WPE_Connect, -1);

			return;
		}
		else
		{
			write_log(m_parserSink, LL_INFO, "[ParserQTS] {} Login success !", m_strUser);

			m_bLogined = true;
			if (m_parserSink)
			{
				m_parserSink->handleEvent(WPE_Connect, 0);
				m_parserSink->handleEvent(WPE_Login, 0);
			}
		}
	} while (false);

	SubscribeMDs(false);
}

void ParserQTS::SubscribeMDs(bool isAll /* = false */)
{
	int iCode;
	if (m_bHasSSE && m_bHasL1)
	{
		//////////====>上交所L1实时行情订阅，包括股票和基金
		std::string codes;
		if (m_bHasShare)
			codes += m_strSHSTKCodes;

		if (m_bHasETF)
		{
			if (codes.size() > 0)
				codes += ",";
			codes += m_strSHETFCodes;
		}

		iCode = m_pUserAPI->Subscribe(Msg_SSEL1_Quotation, (char*)codes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] [ParserQTS] Fail to Subscribe,Msg_SSEL1_Quotation, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] [ParserQTS] Subscribe  Msg_SSEL1_Quotation Success,Start to Receive Data...");
		}
	}

	////////////====>上交所L1盘后固定价格行情订阅
	//if (m_bHasSHL1 && m_bHasFp)
	//{
	//	std::string codes;
	//	if (m_bHasShare)
	//		codes += m_strSHSTKCodes;

	//	if (m_bHasETF)
	//	{
	//		if (codes.size() > 0)
	//			codes += ",";
	//		codes += m_strSHETFCodes;
	//	}

	//	iCode = m_pUserAPI->Subscribe(Msg_SSEL1_FpQuotation, (char*)codes.c_str());
	//	if (Ret_Success != iCode)
	//	{
	//		write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,Msg_SSEL1_FpQuotation, ErrorCode: {}", iCode);
	//	}
	//	else
	//	{
	//		write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  Msg_SSEL1_FpQuotation Success,Start to Receive Data ...");
	//	}
	//}

	////////====>上交所L1债券实时行情订阅
	if (m_bHasBond && m_bHasSSE && m_bHasL1)
	{
		iCode = m_pUserAPI->Subscribe(Msg_SSEL1_BondQuotation, (char*)m_strSHBondCodes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,Msg_SSEL1_BondQuotation, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  Msg_SSEL1_BondQuotation Success,Start to Receive Data ...");
		}
	}

	////////====>上交所个股期权L1实时行情订阅
	if (m_bHasOption && m_bHasSSE && m_bHasL1)
	{
		iCode = m_pUserAPI->Subscribe(Msg_SSEIOL1_Quotation, (char*)m_strSHOptCodes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,Msg_SSEIOL1_Quotation, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  Msg_SSEIOL1_Quotation Success,Start to Receive Data ...");
		}
	}

	////////////====>上交所L2盘后固定价格行情订阅,暂未上线
	//if (m_bHasSHL2 && m_bHasFp)
	//{
	//	iCode = m_pUserAPI->Subscribe(Msg_SSEL2_FpQuotation, (char*)m_strSHCodes.c_str());
	//	if (Ret_Success != iCode)
	//	{
	//		write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,Msg_SSEL2_FpQuotation, ErrorCode: {}", iCode);
	//	}
	//	else
	//	{
	//		write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  Msg_SSEL2_FpQuotation Success,Start to Receive Data ...");
	//	}
	//}

	//////////====>上交所L2ETF订阅，暂未上线
	//if (m_bHasSHL2 && m_bHasETF)
	//{
	//	iCode = m_pUserAPI->Subscribe(Msg_SSEL2_ETF, (char*)m_strSHETFCodes.c_str());
	//	if (Ret_Success != iCode)
	//	{
	//		write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,Msg_SSEL2_ETF, ErrorCode: {}", iCode);
	//	}
	//	else
	//	{
	//		write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  Msg_SSEL2_ETF Success,Start to Receive Data ...");
	//	}
	//}

	//////====>上交所L2股票ETF基金逐笔行情订阅, Msg_SSEL2_Tick暂未上线
	//////////====>上交所L2精简实时行情订阅
	if (m_bHasSSE && m_bHasL2 && m_bHasSnap)
	{
		std::string codes;
		if (m_bHasShare)
			codes += m_strSHSTKCodes;

		if (m_bHasETF)
		{
			if (codes.size() > 0)
				codes += ",";
			codes += m_strSHETFCodes;
		}

		iCode = m_pUserAPI->Subscribe(Msg_SSEL2_ShortQuotation, (char*)codes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,Msg_SSEL2_ShortQuotation, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  Msg_SSEL2_ShortQuotation Success,Start to Receive Data ...");
		}
	}

	////////====>上交所L2股票委托队列订阅
	if (m_bHasSSE && m_bHasL2 && m_bHasOrdQue)
	{
		std::string codes;
		if (m_bHasShare)
			codes += m_strSHSTKCodes;

		if (m_bHasETF)
		{
			if (codes.size() > 0)
				codes += ",";
			codes += m_strSHETFCodes;
		}

		iCode = m_pUserAPI->Subscribe(Msg_SSEL2_OrderQueue, (char*)codes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,Msg_SSEL2_OrderQueue, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  Msg_SSEL2_OrderQueue Success,Start to Receive Data ...");
		}
	}

	//////====>上交所L2股票逐笔成交订阅
	if (m_bHasSSE && m_bHasL2 && m_bHasTrans)
	{
		std::string codes;
		if (m_bHasShare)
			codes += m_strSHSTKCodes;

		if (m_bHasETF)
		{
			if (codes.size() > 0)
				codes += ",";
			codes += m_strSHETFCodes;
		}

		iCode = m_pUserAPI->Subscribe(Msg_SSEL2_Transaction, (char*)codes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,Msg_SSEL2_Transaction, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  Msg_SSEL2_Transaction Success,Start to Receive Data ...");
		}
	}

	////////====>上交所L2股票逐笔委托订阅
	if (m_bHasSSE && m_bHasL2 && m_bHasOrder)
	{
		std::string codes;
		if (m_bHasShare)
			codes += m_strSHSTKCodes;

		if (m_bHasETF)
		{
			if (codes.size() > 0)
				codes += ",";
			codes += m_strSHETFCodes;
		}

		iCode = m_pUserAPI->Subscribe(Msg_SSEL2_Order, (char*)codes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,Msg_SSEL2_Order, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  Msg_SSEL2_Order Success,Start to Receive Data ...");
		}
	}

	////////====>上交所L2指数行情订阅
	if (m_bHasSSE && m_bHasL2 && m_bHasIndex)
	{
		iCode = m_pUserAPI->Subscribe(Msg_SSEL2_Index, (char*)m_strSHINDXCodes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,Msg_SSEL2_Index, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  Msg_SSEL2_Index Success,Start to Receive Data ...");
		}
	}

	////////====>上交所L2债券实时行情订阅
	if (m_bHasSSE && m_bHasL2 && m_bHasBond && m_bHasSnap)
	{
		iCode = m_pUserAPI->Subscribe(Msg_SSEL2_BondQuotation, (char*)m_strSHBondCodes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,Msg_SSEL2_BondQuotation, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  Msg_SSEL2_BondQuotation Success,Start to Receive Data ...");
		}
	}

	////////====>上交所L2债券逐笔行情订阅
	if (m_bHasSSE && m_bHasL2 && m_bHasBond && m_bHasOrder && m_bHasTrans)
	{
		iCode = m_pUserAPI->Subscribe(Msg_SSEL2_BondTick, (char*)m_strSHBondCodes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,Msg_SSEL2_BondTick, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  Msg_SSEL2_BondTick Success,Start to Receive Data ...");
		}
	}

	/////////////////////////////  深交所  /////////////////////////////////////////////
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
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,Msg_SZSEL1_Quotation, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  Msg_SZSEL1_Quotation Success,Start to Receive Data ...");
		}
	}

	////////////====>深交所L1盘后固定价格行情订阅
	//if (m_bHasSZL1 && m_bHasFp)
	//{
	//	std::string codes;
	//	if (m_bHasShare)
	//		codes += m_strSZSTKCodes;

	//	if (m_bHasETF)
	//	{
	//		if (codes.size() > 0)
	//			codes += ",";
	//		codes += m_strSZETFCodes;
	//	}

	//	iCode = m_pUserAPI->Subscribe(Msg_SZSEL1_FpQuotation, (char*)codes.c_str());
	//	if (Ret_Success != iCode)
	//	{
	//		write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,Msg_SZSEL1_FpQuotation, ErrorCode: {}", iCode);
	//	}
	//	else
	//	{
	//		write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  Msg_SZSEL1_FpQuotation Success,Start to Receive Data ...");
	//	}
	//}

	/////////====>深交所个股期权L1实时行情订阅
	if (m_bHasOption && m_bHasSZSE && m_bHasL1)
	{
		iCode = m_pUserAPI->Subscribe(Msg_SZSEIOL1_Quotation, (char*)m_strSZOptCodes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,SZSEIOL1_Quotation, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  SZSEIOL1_Quotation Success,Start to Receive Data ...");
		}
	}


	////////====>深交所L2逐笔成交订阅
	if (m_bHasSZSE && m_bHasL2 && m_bHasTrans)
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

		iCode = m_pUserAPI->Subscribe(Msg_SZSEL2_Transaction, (char*)codes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,Msg_SZSEL2_Transaction, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  Msg_SZSEL2_Transaction Success,Start to Receive Data ...");
		}
	}

	////////====>深交所L2逐笔委托订阅
	if (m_bHasSZSE && m_bHasL2 && m_bHasOrder)
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

		iCode = m_pUserAPI->Subscribe(Msg_SZSEL2_Order, (char*)codes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,SZSEL2_Order, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  SZSEL2_Order Success,Start to Receive Data ...");
		}
	}

	////////====>深交所L2指数行情订阅
	if (m_bHasSZSE && m_bHasL2 && m_bHasIndex)
	{
		iCode = m_pUserAPI->Subscribe(Msg_SZSEL2_Index, (char*)m_strSZINDXCodes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,SZSEL2_Index, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  SZSEL2_Index Success,Start to Receive Data ...");
		}
	}

	////////====>深交所L2精简实时行情订阅
	if (m_bHasSZSE && m_bHasL2 && m_bHasSnap)
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

		iCode = m_pUserAPI->Subscribe(Msg_SZSEL2_ShortQuotation, (char*)codes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,Msg_SZSEL2_ShortQuotation, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  Msg_SZSEL2_ShortQuotation Success,Start to Receive Data ...");
		}
	}

	////////====>深交所L2委托队列订阅
	if (m_bHasSZSE && m_bHasL2 && m_bHasOrdQue)
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

		iCode = m_pUserAPI->Subscribe(Msg_SZSEL2_OrderQueue, (char*)codes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,Msg_SZSEL2_OrderQueue, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  Msg_SZSEL2_OrderQueue Success,Start to Receive Data ...");
		}
	}

	//////////====>深交所L2盘后固定价格行情订阅
	//if (m_bHasSZSE && m_bHasL2 && m_bHasFp)
	//{
	//	std::string codes;
	//	if (m_bHasShare)
	//		codes += m_strSZSTKCodes;

	//	if (m_bHasETF)
	//	{
	//		if (codes.size() > 0)
	//			codes += ",";
	//		codes += m_strSZETFCodes;
	//	}

	//	iCode = m_pUserAPI->Subscribe(Msg_SZSEL2_FpQuotation, (char*)codes.c_str());
	//	if (Ret_Success != iCode)
	//	{
	//		write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,Msg_SZSEL2_FpQuotation, ErrorCode: {}", iCode);
	//	}
	//	else
	//	{
	//		write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  Msg_SZSEL2_FpQuotation Success,Start to Receive Data ...");
	//	}
	//}

	//////////====>深交所L2逐笔数据订阅
	//if (m_bHasSZSE && m_bHasL2)
	//{
	//	std::string codes;
	//	if (m_bHasShare)
	//		codes += m_strSZSTKCodes;

	//	if (m_bHasETF)
	//	{
	//		if (codes.size() > 0)
	//			codes += ",";
	//		codes += m_strSZETFCodes;
	//	}

	//	iCode = m_pUserAPI->Subscribe(Msg_SZSEL2_Tick, (char*)codes.c_str());
	//	if (Ret_Success != iCode)
	//	{
	//		write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,Msg_SZSEL2_Tick, ErrorCode: {}", iCode);
	//	}
	//	else
	//	{
	//		write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  Msg_SZSEL2_Tick Success,Start to Receive Data ...");
	//	}
	//}

	//////====>深交所L2债券实时行情订阅
	if (m_bHasSZSE && m_bHasL2 && m_bHasBond)
	{
		iCode = m_pUserAPI->Subscribe(Msg_SZSEL2_BondQuotation, (char*)m_strSZBondCodes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,Msg_SZSEL2_BondQuotation, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  Msg_SZSEL2_BondQuotation Success,Start to Receive Data ...");
		}
	}

	//////====>深交所L2债券逐笔委托订阅
	if (m_bHasSZSE && m_bHasL2 && m_bHasBond && m_bHasOrder)
	{
		iCode = m_pUserAPI->Subscribe(Msg_SZSEL2_BondOrder, (char*)m_strSZBondCodes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,Msg_SZSEL2_BondOrder, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  Msg_SZSEL2_BondOrder Success,Start to Receive Data ...");
		}
	}

	//////====>深交所L2债券逐笔成交订阅
	if (m_bHasSZSE && m_bHasL2 && m_bHasBond && m_bHasTrans)
	{
		iCode = m_pUserAPI->Subscribe(Msg_SZSEL2_BondTransaction, (char*)m_strSZBondCodes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,Msg_SZSEL2_BondTransaction, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  Msg_SZSEL2_BondTransaction Success,Start to Receive Data ...");
		}
	}

	////////====>深交所L2债券逐笔行情订阅
	//string SZSEL2_BondTick_Code_Sub = "131800,131801,131810";
	//iCode = m_pUserAPI->Subscribe(Msg_SZSEL2_BondTick, (char*)SZSEL2_BondTick_Code_Sub.c_str());
	//if (Ret_Success != iCode)
	//{
	//	write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,Msg_SZSEL2_BondTick, ErrorCode: {}", iCode);
	//}
	//else
	//{
	//	write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  Msg_SZSEL2_BondTick Success,Start to Receive Data ...");
	//}
}

void ParserQTS::subscribe(const CodeSet &vecSymbols)
{
	if (m_uTradingDate == 0)
	{
		m_filterSubs = vecSymbols;
	}
	else
	{
		m_filterSubs = vecSymbols;
	}

	int shNum = 0, szNum = 0, cffNum = 0, shfNum = 0, ineNum = 0, czcNum = 0, dceNum = 0, gfeNum = 0;

	if (vecSymbols.size() == 0)
	{

	}
	else
	{
		for (auto& fullCode : vecSymbols)
		{
			m_strSZSTKCodes.clear();
			m_strSHSTKCodes.clear();
			std::string code, exchg;

			{
				auto ay = StrUtil::split(fullCode.c_str(), ".");
				if (ay.size() < 2)
					continue;

				code = ay[1];
				exchg = ay[0];

				if (exchg == "SSE")
				{
					ContractType tp = extractType(code, true);
					switch (tp)
					{
					case M_INDX:
					{
						m_strSHINDXCodes += code;
						m_strSHINDXCodes += ",";
					}
					break;
					case M_BOND:
					{
						m_strSHBondCodes += code;
						m_strSHBondCodes += ",";
					}
					break;
					case M_STK:
					{
						m_strSHSTKCodes += code;
						m_strSHSTKCodes += ",";
					}
					break;
					case M_ETF:
					{
						m_strSHETFCodes += code;
						m_strSHETFCodes += ",";
					}
					break;
					case M_OPT:
					{
						m_strSHOptCodes += code;
						m_strSHOptCodes += ",";
					}
					break;
					case M_FUT:
					case M_UNKNOWN:
					default:
						break;
					}

					shNum++;
				}
				else if (exchg == "SZSE")
				{
					ContractType tp = extractType(code, true);
					switch (tp)
					{
					case M_INDX:
					{
						m_strSZINDXCodes += code;
						m_strSZINDXCodes += ",";
					}
					break;
					case M_BOND:
					{
						m_strSZBondCodes += code;
						m_strSZBondCodes += ",";
					}
					break;
					case M_STK:
					{
						m_strSZSTKCodes += code;
						m_strSZSTKCodes += ",";
					}
					break;
					case M_ETF:
					{
						m_strSZETFCodes += code;
						m_strSZETFCodes += ",";
					}
					break;
					case M_OPT:
					{
						m_strSZOptCodes += code;
						m_strSZOptCodes += ",";
					}
					break;
					case M_FUT:
					case M_UNKNOWN:
					default:
						break;
					}

					szNum++;
				}
			}
		}

		if (m_strSHSTKCodes.size() > 0)
			m_strSHSTKCodes.pop_back();

		if (m_strSZSTKCodes.size() > 0)
			m_strSZSTKCodes.pop_back();

		if (m_strSHOptCodes.size() > 0)
			m_strSHOptCodes.pop_back();

		if (m_strSZOptCodes.size() > 0)
			m_strSZOptCodes.pop_back();

		if (m_strSHETFCodes.size() > 0)
			m_strSHETFCodes.pop_back();

		if (m_strSZETFCodes.size() > 0)
			m_strSZETFCodes.pop_back();

		if (m_strSHBondCodes.size() > 0)
			m_strSHBondCodes.pop_back();

		if (m_strSZBondCodes.size() > 0)
			m_strSZBondCodes.pop_back();

		if (m_strSHINDXCodes.size() > 0)
			m_strSHINDXCodes.pop_back();

		if (m_strSZINDXCodes.size() > 0)
			m_strSZINDXCodes.pop_back();
	}

	//write_log(m_parserSink, LL_DEBUG, "[ParserQTS] Subscribe {} instruments from SSE market ...", shNum > 0 ? std::to_string(shNum) : "All");
	write_log(m_parserSink, LL_DEBUG, "[ParserQTS] Subscribe {} instruments from SSE market ...", shNum);
	write_log(m_parserSink, LL_DEBUG, "[ParserQTS] Subscribe {} instruments from SZSE market ...", szNum);
}

void ParserQTS::unsubscribe(const CodeSet &vecSymbols)
{
	RetCode code = m_pUserAPI->UnsubscribeAll();

	if (code != Ret_Success)
		write_log(m_parserSink, LL_ERROR, "[ParserQTS] Unsubscribe contracts fail: {}", code);
	else
		write_log(m_parserSink, LL_DEBUG, "[ParserQTS] Unsubscribe contracts success!");
}

bool ParserQTS::isConnected()
{
	return m_pUserAPI != NULL;
}

void ParserQTS::registerSpi(IParserSpi* listener)
{
	m_parserSink = listener;

	if(m_parserSink)
		m_pBaseDataMgr = m_parserSink->getBaseDataMgr();
}

void ParserQTS::OnConnectionState(MsgType msgType, RetCode errCode)
{
	if (errCode == Ret_Success)
		write_log(m_parserSink, LL_INFO, "[ParserQTS] {} Connect!", m_strUser);
	else
		write_log(m_parserSink, LL_WARN, "[ParserQTS] {} Disconnect: ErrCode: {}!", m_strUser, errCode);
}

void ParserQTS::OnSubscribe_SSEL1_Quotation(const SSEL1_Quotation& RealSSEL1Quotation)
{
	const char* code = RealSSEL1Quotation.Symbol;
	const char* exchg = "SSE";

	WTSContractInfo* ct = m_pBaseDataMgr->getContract(code, exchg);
	if (ct == NULL)
	{
		if (m_parserSink)
			//write_log(m_parserSink, LL_ERROR, "[ParserQTS] Instrument {}.{} not exists...", exchg, code);
		return;
	}

	WTSCommodityInfo* commInfo = ct->getCommInfo();

	WTSTickData* tick = WTSTickData::create(code);
	tick->setContractInfo(ct);
	WTSTickStruct& quote = tick->getTickStruct();
	strcpy(quote.exchg, exchg);

	quote.action_date = (uint32_t)(RealSSEL1Quotation.PacketTimeStamp / 1000000000);  // YYYYMMDDHHMMSSMMM
	quote.action_time = RealSSEL1Quotation.Time;  // HHMMssmmm
	quote.trading_date = TimeUtils::getCurDate();

	quote.price = checkValid(RealSSEL1Quotation.LastPrice);
	quote.open = checkValid(RealSSEL1Quotation.OpenPrice);
	quote.high = checkValid(RealSSEL1Quotation.HighPrice);
	quote.low = checkValid(RealSSEL1Quotation.LowPrice);
	quote.total_volume = (uint32_t)RealSSEL1Quotation.TotalVolume;
	quote.total_turnover = checkValid(RealSSEL1Quotation.TotalAmount);  

	quote.pre_close = checkValid(RealSSEL1Quotation.PreClosePrice);

	quote.ask_prices[0] = checkValid(RealSSEL1Quotation.SellPrice01);
	quote.ask_qty[0] = RealSSEL1Quotation.SellVolume01;
	quote.ask_prices[1] = checkValid(RealSSEL1Quotation.SellPrice02);
	quote.ask_qty[1] = RealSSEL1Quotation.SellVolume02;
	quote.ask_prices[2] = checkValid(RealSSEL1Quotation.SellPrice03);
	quote.ask_qty[2] = RealSSEL1Quotation.SellVolume03;
	quote.ask_prices[3] = checkValid(RealSSEL1Quotation.SellPrice04);
	quote.ask_qty[3] = RealSSEL1Quotation.SellVolume04;
	quote.ask_prices[4] = checkValid(RealSSEL1Quotation.SellPrice05);
	quote.ask_qty[4] = RealSSEL1Quotation.SellVolume05;

	quote.bid_prices[0] = checkValid(RealSSEL1Quotation.BuyPrice01);
	quote.bid_qty[0] = RealSSEL1Quotation.BuyVolume01;
	quote.bid_prices[1] = checkValid(RealSSEL1Quotation.BuyPrice02);
	quote.bid_qty[1] = RealSSEL1Quotation.BuyVolume02;
	quote.bid_prices[2] = checkValid(RealSSEL1Quotation.BuyPrice03);
	quote.bid_qty[2] = RealSSEL1Quotation.BuyVolume03;
	quote.bid_prices[3] = checkValid(RealSSEL1Quotation.BuyPrice04);
	quote.bid_qty[3] = RealSSEL1Quotation.BuyVolume04;
	quote.bid_prices[4] = checkValid(RealSSEL1Quotation.BuyPrice05);
	quote.bid_qty[4] = RealSSEL1Quotation.BuyVolume05;

	if (m_parserSink)
		m_parserSink->handleQuote(tick, 1);

	tick->release();
}

void ParserQTS::OnSubscribe_SSEL1_BondQuotation(const SSEL1_BondQuotation& RealSSEL1BondQuotation) 
{
	const char* code = RealSSEL1BondQuotation.Symbol;
	const char* exchg = "SSE";

	WTSContractInfo* ct = m_pBaseDataMgr->getContract(code, exchg);
	if (ct == NULL)
	{
		if (m_parserSink)
			//write_log(m_parserSink, LL_ERROR, "[ParserQTS] Instrument {}.{} not exists...", exchg, code);
		return;
	}

	WTSCommodityInfo* commInfo = ct->getCommInfo();

	WTSTickData* tick = WTSTickData::create(code);
	tick->setContractInfo(ct);
	WTSTickStruct& quote = tick->getTickStruct();
	strcpy(quote.exchg, exchg);

	uint32_t action_date = (uint32_t)(RealSSEL1BondQuotation.PacketTimeStamp / 1000000000);  // YYYYMMDDHHMMSSMMM
	quote.action_date = action_date;  
	quote.action_time = RealSSEL1BondQuotation.Time;  // HHMMssmmm
	quote.trading_date = action_date;

	quote.price = checkValid(RealSSEL1BondQuotation.LastPrice);
	quote.open = checkValid(RealSSEL1BondQuotation.OpenPrice);
	quote.high = checkValid(RealSSEL1BondQuotation.HighPrice);
	quote.low = checkValid(RealSSEL1BondQuotation.LowPrice);
	quote.total_volume = (uint32_t)RealSSEL1BondQuotation.TotalVolume;
	quote.total_turnover = checkValid(RealSSEL1BondQuotation.TotalAmount);  // 成交金额需要处理

	quote.pre_close = checkValid(RealSSEL1BondQuotation.PreClosePrice);

	quote.ask_prices[0] = checkValid(RealSSEL1BondQuotation.SellPrice01);
	quote.ask_qty[0] = RealSSEL1BondQuotation.SellVolume01;
	quote.ask_prices[1] = checkValid(RealSSEL1BondQuotation.SellPrice02);
	quote.ask_qty[1] = RealSSEL1BondQuotation.SellVolume02;
	quote.ask_prices[2] = checkValid(RealSSEL1BondQuotation.SellPrice03);
	quote.ask_qty[2] = RealSSEL1BondQuotation.SellVolume03;
	quote.ask_prices[3] = checkValid(RealSSEL1BondQuotation.SellPrice04);
	quote.ask_qty[3] = RealSSEL1BondQuotation.SellVolume04;
	quote.ask_prices[4] = checkValid(RealSSEL1BondQuotation.SellPrice05);
	quote.ask_qty[4] = RealSSEL1BondQuotation.SellVolume05;

	quote.bid_prices[0] = checkValid(RealSSEL1BondQuotation.BuyPrice01);
	quote.bid_qty[0] = RealSSEL1BondQuotation.BuyVolume01;
	quote.bid_prices[1] = checkValid(RealSSEL1BondQuotation.BuyPrice02);
	quote.bid_qty[1] = RealSSEL1BondQuotation.BuyVolume02;
	quote.bid_prices[2] = checkValid(RealSSEL1BondQuotation.BuyPrice03);
	quote.bid_qty[2] = RealSSEL1BondQuotation.BuyVolume03;
	quote.bid_prices[3] = checkValid(RealSSEL1BondQuotation.BuyPrice04);
	quote.bid_qty[3] = RealSSEL1BondQuotation.BuyVolume04;
	quote.bid_prices[4] = checkValid(RealSSEL1BondQuotation.BuyPrice05);
	quote.bid_qty[4] = RealSSEL1BondQuotation.BuyVolume05;

	if (m_parserSink)
		m_parserSink->handleQuote(tick, 1);

	tick->release();
}

void ParserQTS::OnSubscribe_SSEL2_Quotation(const SSEL2_Quotation& RealSSEL2Quotation)
{
	const char* code = RealSSEL2Quotation.Symbol;
	const char* exchg = "SSE";

	WTSContractInfo* ct = m_pBaseDataMgr->getContract(code, exchg);
	if (ct == NULL)
	{
		if (m_parserSink)
			//write_log(m_parserSink, LL_ERROR, "[ParserQTS] Instrument {}.{} not exists...", exchg, code);
		return;
	}

	WTSCommodityInfo* commInfo = ct->getCommInfo();

	WTSTickData* tick = WTSTickData::create(code);
	tick->setContractInfo(ct);
	WTSTickStruct& quote = tick->getTickStruct();
	strcpy(quote.exchg, exchg);

	uint32_t act_date = (uint32_t)(RealSSEL2Quotation.PacketTimeStamp / 1000000000);
	quote.action_date = act_date;
	quote.action_time = RealSSEL2Quotation.Time;
	quote.trading_date = act_date;

	quote.price = checkValid(RealSSEL2Quotation.LastPrice);
	quote.open = checkValid(RealSSEL2Quotation.OpenPrice);
	quote.high = checkValid(RealSSEL2Quotation.HighPrice);
	quote.low = checkValid(RealSSEL2Quotation.LowPrice);
	quote.total_volume = (uint32_t)RealSSEL2Quotation.TotalVolume;
	quote.total_turnover = checkValid(RealSSEL2Quotation.TotalAmount);  // 成交金额需要处理

	quote.pre_close = checkValid(RealSSEL2Quotation.PreClosePrice);

	quote.ask_prices[0] = checkValid(RealSSEL2Quotation.SellPrice01);
	quote.ask_qty[0] = RealSSEL2Quotation.SellVolume01;
	quote.ask_prices[1] = checkValid(RealSSEL2Quotation.SellPrice02);
	quote.ask_qty[1] = RealSSEL2Quotation.SellVolume02;
	quote.ask_prices[2] = checkValid(RealSSEL2Quotation.SellPrice03);
	quote.ask_qty[2] = RealSSEL2Quotation.SellVolume03;
	quote.ask_prices[3] = checkValid(RealSSEL2Quotation.SellPrice04);
	quote.ask_qty[3] = RealSSEL2Quotation.SellVolume04;
	quote.ask_prices[4] = checkValid(RealSSEL2Quotation.SellPrice05);
	quote.ask_qty[4] = RealSSEL2Quotation.SellVolume05;
	quote.ask_prices[5] = checkValid(RealSSEL2Quotation.SellPrice06);
	quote.ask_qty[5] = RealSSEL2Quotation.SellVolume06;
	quote.ask_prices[6] = checkValid(RealSSEL2Quotation.SellPrice07);
	quote.ask_qty[6] = RealSSEL2Quotation.SellVolume07;
	quote.ask_prices[7] = checkValid(RealSSEL2Quotation.SellPrice08);
	quote.ask_qty[7] = RealSSEL2Quotation.SellVolume08;
	quote.ask_prices[8] = checkValid(RealSSEL2Quotation.SellPrice09);
	quote.ask_qty[8] = RealSSEL2Quotation.SellVolume09;
	quote.ask_prices[9] = checkValid(RealSSEL2Quotation.SellPrice10);
	quote.ask_qty[9] = RealSSEL2Quotation.SellVolume10;

	quote.bid_prices[0] = checkValid(RealSSEL2Quotation.BuyPrice01);
	quote.bid_qty[0] = RealSSEL2Quotation.BuyVolume01;
	quote.bid_prices[1] = checkValid(RealSSEL2Quotation.BuyPrice02);
	quote.bid_qty[1] = RealSSEL2Quotation.BuyVolume02;
	quote.bid_prices[2] = checkValid(RealSSEL2Quotation.BuyPrice03);
	quote.bid_qty[2] = RealSSEL2Quotation.BuyVolume03;
	quote.bid_prices[3] = checkValid(RealSSEL2Quotation.BuyPrice04);
	quote.bid_qty[3] = RealSSEL2Quotation.BuyVolume04;
	quote.bid_prices[4] = checkValid(RealSSEL2Quotation.BuyPrice05);
	quote.bid_qty[4] = RealSSEL2Quotation.BuyVolume05;
	quote.bid_prices[5] = checkValid(RealSSEL2Quotation.BuyPrice06);
	quote.bid_qty[5] = RealSSEL2Quotation.BuyVolume06;
	quote.bid_prices[6] = checkValid(RealSSEL2Quotation.BuyPrice07);
	quote.bid_qty[6] = RealSSEL2Quotation.BuyVolume07;
	quote.bid_prices[7] = checkValid(RealSSEL2Quotation.BuyPrice08);
	quote.bid_qty[7] = RealSSEL2Quotation.BuyVolume08;
	quote.bid_prices[8] = checkValid(RealSSEL2Quotation.BuyPrice09);
	quote.bid_qty[8] = RealSSEL2Quotation.BuyVolume09;
	quote.bid_prices[9] = checkValid(RealSSEL2Quotation.BuyPrice10);
	quote.bid_qty[9] = RealSSEL2Quotation.BuyVolume10;

	if (m_parserSink)
		m_parserSink->handleQuote(tick, 1);

	//处理逐笔
	if (RealSSEL2Quotation.BuyLevelQueueNo01 != 0)
	{
		WTSOrdQueData* buyQue = WTSOrdQueData::create(code);
		buyQue->setContractInfo(ct);

		WTSOrdQueStruct& buyOS = buyQue->getOrdQueStruct();
		strcpy(buyOS.exchg, exchg);

		buyOS.trading_date = act_date;
		buyOS.action_date = act_date;
		buyOS.action_time = RealSSEL2Quotation.Time;

		buyOS.side = BDT_Buy;
		buyOS.price = RealSSEL2Quotation.BuyLevel[0].Price; // quote.bid_prices[0];
		buyOS.order_items = RealSSEL2Quotation.BuyLevel[0].TotalOrderNo;
		buyOS.qsize = ORDER_LEVEL_FIFTY; // 最大只有50档的数据

		for (uint32_t i = 0; i < ORDER_LEVEL_FIFTY; i++)
		{
			buyOS.volumes[i] = (uint32_t)RealSSEL2Quotation.BuyLevelQueue[i];
		}

		if (m_parserSink)
			m_parserSink->handleOrderQueue(buyQue);

		buyQue->release();
	}

	if (RealSSEL2Quotation.SellLevelQueueNo01 != 0)
	{
		WTSOrdQueData* sellQue = WTSOrdQueData::create(code);
		sellQue->setContractInfo(ct);

		WTSOrdQueStruct& sellOS = sellQue->getOrdQueStruct();
		strcpy(sellOS.exchg, exchg);

		sellOS.trading_date = act_date;
		sellOS.action_date = act_date;
		sellOS.action_time = RealSSEL2Quotation.Time;

		sellOS.side = BDT_Sell;
		sellOS.price = RealSSEL2Quotation.SellLevel[0].Price;
		sellOS.order_items = RealSSEL2Quotation.SellLevel[0].TotalOrderNo;
		sellOS.qsize = ORDER_LEVEL_FIFTY;

		for (uint32_t i = 0; i < sellOS.qsize; i++)
		{
			sellOS.volumes[i] = (uint32_t)RealSSEL2Quotation.SellLevelQueue[i];
		}

		if (m_parserSink)
			m_parserSink->handleOrderQueue(sellQue);

		sellQue->release();
	}

	tick->release();
}

void ParserQTS::OnSubscribe_SSEL2_Index(const SSEL2_Index& RealSSEL2Index)
{
	const char* code = RealSSEL2Index.Symbol;
	const char* exchg = "SSE";
	auto ct = m_pBaseDataMgr->getContract(code, exchg);
	if (ct == NULL)
	{
		if (m_parserSink)
			//write_log(m_parserSink, LL_ERROR, "[ParserQTS] Instrument {}.{} not exists...", exchg, code);
		return;
	}

	WTSTickData* tick = WTSTickData::create(code);
	WTSTickStruct& quote = tick->getTickStruct();
	strcpy(quote.exchg, exchg);

	quote.action_date = (uint32_t)(RealSSEL2Index.PacketTimeStamp / 1000000000);
	quote.trading_date = quote.action_date;
	quote.action_time = RealSSEL2Index.Time;

	quote.price = checkValid(RealSSEL2Index.LastPrice);
	quote.open = checkValid(RealSSEL2Index.OpenPrice);
	quote.high = checkValid(RealSSEL2Index.HighPrice);
	quote.low = checkValid(RealSSEL2Index.LastPrice);
	quote.pre_close = checkValid(RealSSEL2Index.PreClosePrice);

	quote.total_volume = RealSSEL2Index.TotalVolume; //
	quote.total_turnover = RealSSEL2Index.TotalAmount;  //

	//write_log(m_parserSink, LL_DEBUG, "[OnSubscribe_SSEL2_Index] [{}.{}] act_date: {}, act_time: {}, last price: {}, open: {}, high: {}, low: {}", 
	//	code, exchg, quote.action_date, quote.action_time, quote.price, quote.open, quote.high, quote.low);

	if (m_parserSink)
		m_parserSink->handleQuote(tick, true);

	tick->release();
}

void ParserQTS::OnSubscribe_SSEL2_BondQuotation(const SSEL2_BondQuotation& RealSSEL2BondQuotation)
{
	const char* code = RealSSEL2BondQuotation.Symbol;
	const char* exchg = "SSE";

	WTSContractInfo* ct = m_pBaseDataMgr->getContract(code, exchg);
	if (ct == NULL)
	{
		if (m_parserSink)
			//write_log(m_parserSink, LL_ERROR, "[ParserQTS] Instrument {}.{} not exists...", exchg, code);
		return;
	}

	WTSCommodityInfo* commInfo = ct->getCommInfo();

	WTSTickData* tick = WTSTickData::create(code);
	tick->setContractInfo(ct);
	WTSTickStruct& quote = tick->getTickStruct();
	strcpy(quote.exchg, exchg);

	uint32_t act_date = (uint32_t)(RealSSEL2BondQuotation.PacketTimeStamp / 1000000000);
	quote.action_date = act_date;
	quote.action_time = RealSSEL2BondQuotation.Time;
	quote.trading_date = act_date;

	quote.price = checkValid(RealSSEL2BondQuotation.LastPrice);
	quote.open = checkValid(RealSSEL2BondQuotation.OpenPrice);
	quote.high = checkValid(RealSSEL2BondQuotation.HighPrice);
	quote.low = checkValid(RealSSEL2BondQuotation.LowPrice);
	quote.total_volume = (uint32_t)RealSSEL2BondQuotation.TotalVolume;
	quote.total_turnover = checkValid(RealSSEL2BondQuotation.TotalAmount);  // 成交金额需要处理

	quote.pre_close = checkValid(RealSSEL2BondQuotation.PreClosePrice);

	quote.ask_prices[0] = checkValid(RealSSEL2BondQuotation.SellPrice01);
	quote.ask_qty[0] = RealSSEL2BondQuotation.SellVolume01;
	quote.ask_prices[1] = checkValid(RealSSEL2BondQuotation.SellPrice02);
	quote.ask_qty[1] = RealSSEL2BondQuotation.SellVolume02;
	quote.ask_prices[2] = checkValid(RealSSEL2BondQuotation.SellPrice03);
	quote.ask_qty[2] = RealSSEL2BondQuotation.SellVolume03;
	quote.ask_prices[3] = checkValid(RealSSEL2BondQuotation.SellPrice04);
	quote.ask_qty[3] = RealSSEL2BondQuotation.SellVolume04;
	quote.ask_prices[4] = checkValid(RealSSEL2BondQuotation.SellPrice05);
	quote.ask_qty[4] = RealSSEL2BondQuotation.SellVolume05;
	quote.ask_prices[5] = checkValid(RealSSEL2BondQuotation.SellPrice06);
	quote.ask_qty[5] = RealSSEL2BondQuotation.SellVolume06;
	quote.ask_prices[6] = checkValid(RealSSEL2BondQuotation.SellPrice07);
	quote.ask_qty[6] = RealSSEL2BondQuotation.SellVolume07;
	quote.ask_prices[7] = checkValid(RealSSEL2BondQuotation.SellPrice08);
	quote.ask_qty[7] = RealSSEL2BondQuotation.SellVolume08;
	quote.ask_prices[8] = checkValid(RealSSEL2BondQuotation.SellPrice09);
	quote.ask_qty[8] = RealSSEL2BondQuotation.SellVolume09;
	quote.ask_prices[9] = checkValid(RealSSEL2BondQuotation.SellPrice10);
	quote.ask_qty[9] = RealSSEL2BondQuotation.SellVolume10;

	quote.bid_prices[0] = checkValid(RealSSEL2BondQuotation.BuyPrice01);
	quote.bid_qty[0] = RealSSEL2BondQuotation.BuyVolume01;
	quote.bid_prices[1] = checkValid(RealSSEL2BondQuotation.BuyPrice02);
	quote.bid_qty[1] = RealSSEL2BondQuotation.BuyVolume02;
	quote.bid_prices[2] = checkValid(RealSSEL2BondQuotation.BuyPrice03);
	quote.bid_qty[2] = RealSSEL2BondQuotation.BuyVolume03;
	quote.bid_prices[3] = checkValid(RealSSEL2BondQuotation.BuyPrice04);
	quote.bid_qty[3] = RealSSEL2BondQuotation.BuyVolume04;
	quote.bid_prices[4] = checkValid(RealSSEL2BondQuotation.BuyPrice05);
	quote.bid_qty[4] = RealSSEL2BondQuotation.BuyVolume05;
	quote.bid_prices[5] = checkValid(RealSSEL2BondQuotation.BuyPrice06);
	quote.bid_qty[5] = RealSSEL2BondQuotation.BuyVolume06;
	quote.bid_prices[6] = checkValid(RealSSEL2BondQuotation.BuyPrice07);
	quote.bid_qty[6] = RealSSEL2BondQuotation.BuyVolume07;
	quote.bid_prices[7] = checkValid(RealSSEL2BondQuotation.BuyPrice08);
	quote.bid_qty[7] = RealSSEL2BondQuotation.BuyVolume08;
	quote.bid_prices[8] = checkValid(RealSSEL2BondQuotation.BuyPrice09);
	quote.bid_qty[8] = RealSSEL2BondQuotation.BuyVolume09;
	quote.bid_prices[9] = checkValid(RealSSEL2BondQuotation.BuyPrice10);
	quote.bid_qty[9] = RealSSEL2BondQuotation.BuyVolume10;

	if (m_parserSink)
		m_parserSink->handleQuote(tick, 1);

	tick->release();

	//处理逐笔
	if (RealSSEL2BondQuotation.BuyLevelQueueNo01 != 0)
	{
		WTSOrdQueData* buyQue = WTSOrdQueData::create(code);
		buyQue->setContractInfo(ct);

		WTSOrdQueStruct& buyOS = buyQue->getOrdQueStruct();
		strcpy(buyOS.exchg, exchg);

		buyOS.trading_date = act_date;
		buyOS.action_date = act_date;
		buyOS.action_time = RealSSEL2BondQuotation.Time;

		buyOS.side = BDT_Buy;
		buyOS.price = RealSSEL2BondQuotation.BuyLevel[0].Price; // quote.bid_prices[0];
		buyOS.order_items = RealSSEL2BondQuotation.BuyLevelQueueNo01;
		buyOS.qsize = std::min(ORDER_LEVEL_FIFTY, (int)buyOS.order_items); // 最大只有50档的数据

		for (uint32_t i = 0; i < buyOS.qsize; i++)
		{
			buyOS.volumes[i] = (uint32_t)RealSSEL2BondQuotation.BuyLevelQueue[i];
		}

		if (m_parserSink)
			m_parserSink->handleOrderQueue(buyQue);

		buyQue->release();
	}

	if (RealSSEL2BondQuotation.SellLevelQueueNo01 != 0)
	{
		WTSOrdQueData* sellQue = WTSOrdQueData::create(code);
		sellQue->setContractInfo(ct);

		WTSOrdQueStruct& sellOS = sellQue->getOrdQueStruct();
		strcpy(sellOS.exchg, exchg);

		sellOS.trading_date = act_date;
		sellOS.action_date = act_date;
		sellOS.action_time = RealSSEL2BondQuotation.Time;

		sellOS.side = BDT_Sell;
		sellOS.price = RealSSEL2BondQuotation.SellLevel[0].Price;
		sellOS.order_items = RealSSEL2BondQuotation.SellLevelQueueNo01;
		sellOS.qsize = std::min(ORDER_LEVEL_FIFTY, (int)sellOS.order_items);

		for (uint32_t i = 0; i < sellOS.qsize; i++)
		{
			sellOS.volumes[i] = (uint32_t)RealSSEL2BondQuotation.SellLevelQueue[i];
		}

		if (m_parserSink)
			m_parserSink->handleOrderQueue(sellQue);

		sellQue->release();
	}
}

void ParserQTS::OnSubscribe_SSEL2_BondTick(const SSEL2_BondTick& RealSSEL2BondTick)
{
	const char* code = RealSSEL2BondTick.Symbol;
	const char* exchg = "SSE";
	auto ct = m_pBaseDataMgr->getContract(code, exchg);
	if (ct == NULL)
		return;

	uint32_t act_date = (uint32_t)(RealSSEL2BondTick.PacketTimeStamp / 1000000000);

	if (RealSSEL2BondTick.TickType == 'T')
	{
		WTSTransData* trans = WTSTransData::create(code);
		WTSTransStruct& ts = trans->getTransStruct();
		strcpy(ts.exchg, exchg);

		ts.trading_date = act_date;
		ts.action_date = act_date;
		ts.action_time = RealSSEL2BondTick.TickTime;

		ts.price = checkValid(RealSSEL2BondTick.TickPrice);
		ts.volume = RealSSEL2BondTick.TickVolume;
		ts.askorder = (uint32_t)RealSSEL2BondTick.SellOrderNO;
		ts.bidorder = (uint32_t)RealSSEL2BondTick.BuyOrderNO;
		ts.index = (uint32_t)RealSSEL2BondTick.RecID;
		ts.side = wrapTickDirecType(RealSSEL2BondTick.TickBSFlag);
		ts.ttype = TT_Match; 

		if (m_parserSink)
			m_parserSink->handleTransaction(trans);

		trans->release();
	}
	else if ((RealSSEL2BondTick.TickType == 'A') || (RealSSEL2BondTick.TickType == 'D'))
	{
		WTSOrdDtlData* order = WTSOrdDtlData::create(code);
		WTSOrdDtlStruct& ods = order->getOrdDtlStruct();
		strcpy(ods.exchg, exchg);

		ods.action_date = act_date;
		ods.trading_date = act_date;
		ods.action_time = RealSSEL2BondTick.TickTime;
		ods.side = wrapTickDirecType(RealSSEL2BondTick.TickBSFlag);
		ods.price = checkValid(RealSSEL2BondTick.TickPrice);
		ods.volume = RealSSEL2BondTick.TickVolume;
		ods.index = (uint32_t)RealSSEL2BondTick.RecID;
		ods.otype = ODT_LimitPrice;  // 没提供订单类型的字段，默认是限价单

		if (m_parserSink)
			m_parserSink->handleOrderDetail(order);

		order->release();
	}
}

void ParserQTS::OnSubscribe_SSEL2_Transaction(const SSEL2_Transaction& RealSSEL2Transaction) 
{
	const char* code = RealSSEL2Transaction.Symbol;
	const char* exchg = "SSE";
	auto ct = m_pBaseDataMgr->getContract(code, exchg);
	if (ct == NULL)
		return;

	WTSTransData* trans = WTSTransData::create(code);
	WTSTransStruct& ts = trans->getTransStruct();
	strcpy(ts.exchg, exchg);

	ts.trading_date = (uint32_t)(RealSSEL2Transaction.PacketTimeStamp / 1000000000);
	ts.action_date = ts.trading_date;
	ts.action_time = RealSSEL2Transaction.TradeTime;  // 精确到10ms，百分之一秒，143025060

	ts.price = checkValid(RealSSEL2Transaction.TradePrice);
	ts.volume = RealSSEL2Transaction.TradeVolume;
	ts.askorder = (uint32_t)RealSSEL2Transaction.BuyRecID;
	ts.bidorder = (uint32_t)RealSSEL2Transaction.SellRecID;
	ts.index = (uint32_t)RealSSEL2Transaction.RecID;  // 成交序号
	ts.side = RealSSEL2Transaction.BuySellFlag == 'B' ? BDT_Buy : BDT_Sell;
	ts.ttype = TT_Match;

	if (m_parserSink)
		m_parserSink->handleTransaction(trans);

	//write_log(m_parserSink, LL_DEBUG, "[OnSubscribe_SSEL2_Transaction] {}.{}, act_time: {}, price: {}, volume: {}, askorder: {}, bidorder: {}, index: {}", 
	//	code, exchg, ts.action_time, ts.price, ts.volume, ts.askorder, ts.bidorder, ts.index);

	trans->release();
}

void ParserQTS::OnSubscribe_SSEL2_Order(const SSEL2_Order& RealSSEL2Order) 
{
	const char* code = RealSSEL2Order.Symbol;
	const char* exchg = "SSE";
	auto ct = m_pBaseDataMgr->getContract(code, exchg);
	if (ct == NULL)
		return;

	WTSOrdDtlData* order = WTSOrdDtlData::create(code);
	WTSOrdDtlStruct& ods = order->getOrdDtlStruct();
	strcpy(ods.exchg, exchg);

	ods.action_date = (uint32_t)(RealSSEL2Order.PacketTimeStamp / 1000000000);
	ods.trading_date = ods.action_date;
	ods.action_time = RealSSEL2Order.Time;

	ods.side = wrapTickDirecType(RealSSEL2Order.OrderCode);
	ods.price = checkValid(RealSSEL2Order.OrderPrice);
	ods.volume = RealSSEL2Order.Balance;
	ods.index = (uint32_t)RealSSEL2Order.RecID;
	ods.otype = ODT_LimitPrice;  // 没提供订单类型的字段，默认是限价单

	if (m_parserSink)
		m_parserSink->handleOrderDetail(order);

	//write_log(m_parserSink, LL_DEBUG, "[OnSubscribe_SSEL2_Order] {}.{}, act_time: {}, price: {}, volume: {}, index: {}",
	//	code, exchg, ods.action_time, ods.price, ods.volume, ods.index);

	order->release();
}

void ParserQTS::OnSubscribe_SSEIOL1_Quotation(const SSEIOL1_Quotation& RealSSEIOL1Quotation) 
{
	const char* code = RealSSEIOL1Quotation.Symbol;
	const char* exchg = "SSE";

	WTSContractInfo* ct = m_pBaseDataMgr->getContract(code, exchg);
	if (ct == NULL)
	{
		if (m_parserSink)
			//write_log(m_parserSink, LL_ERROR, "[ParserQTS] Instrument {}.{} not exists...", exchg, code);
		return;
	}

	WTSCommodityInfo* commInfo = ct->getCommInfo();

	WTSTickData* tick = WTSTickData::create(code);
	tick->setContractInfo(ct);
	WTSTickStruct& quote = tick->getTickStruct();
	strcpy(quote.exchg, exchg);

	quote.action_date = (uint32_t)(RealSSEIOL1Quotation.PacketTimeStamp / 1000000000);  // YYYYMMDDHHMMSSMMM
	quote.action_time = RealSSEIOL1Quotation.Time;  // HHMMssmmm
	quote.trading_date = quote.action_date;

	quote.price = checkValid(RealSSEIOL1Quotation.LastPrice);
	quote.open = checkValid(RealSSEIOL1Quotation.OpenPrice);
	quote.high = checkValid(RealSSEIOL1Quotation.HighPrice);
	quote.low = checkValid(RealSSEIOL1Quotation.LowPrice);
	quote.total_volume = (uint32_t)RealSSEIOL1Quotation.TotalVolume;
	quote.total_turnover = checkValid(RealSSEIOL1Quotation.TotalAmount);  

	quote.pre_close = checkValid(RealSSEIOL1Quotation.PreSettlePrice);

	quote.ask_prices[0] = checkValid(RealSSEIOL1Quotation.SellPrice01);
	quote.ask_qty[0] = RealSSEIOL1Quotation.SellVolume01;
	quote.ask_prices[1] = checkValid(RealSSEIOL1Quotation.SellPrice02);
	quote.ask_qty[1] = RealSSEIOL1Quotation.SellVolume02;
	quote.ask_prices[2] = checkValid(RealSSEIOL1Quotation.SellPrice03);
	quote.ask_qty[2] = RealSSEIOL1Quotation.SellVolume03;
	quote.ask_prices[3] = checkValid(RealSSEIOL1Quotation.SellPrice04);
	quote.ask_qty[3] = RealSSEIOL1Quotation.SellVolume04;
	quote.ask_prices[4] = checkValid(RealSSEIOL1Quotation.SellPrice05);
	quote.ask_qty[4] = RealSSEIOL1Quotation.SellVolume05;

	quote.bid_prices[0] = checkValid(RealSSEIOL1Quotation.BuyPrice01);
	quote.bid_qty[0] = RealSSEIOL1Quotation.BuyVolume01;
	quote.bid_prices[1] = checkValid(RealSSEIOL1Quotation.BuyPrice02);
	quote.bid_qty[1] = RealSSEIOL1Quotation.BuyVolume02;
	quote.bid_prices[2] = checkValid(RealSSEIOL1Quotation.BuyPrice03);
	quote.bid_qty[2] = RealSSEIOL1Quotation.BuyVolume03;
	quote.bid_prices[3] = checkValid(RealSSEIOL1Quotation.BuyPrice04);
	quote.bid_qty[3] = RealSSEIOL1Quotation.BuyVolume04;
	quote.bid_prices[4] = checkValid(RealSSEIOL1Quotation.BuyPrice05);
	quote.bid_qty[4] = RealSSEIOL1Quotation.BuyVolume05;

	if (m_parserSink)
		m_parserSink->handleQuote(tick, 1);

	tick->release();
}

void ParserQTS::OnSubscribe_SSEL2_ShortQuotation(const SSEL2_ShortQuotation& RealSSEL2_ShortQuotation) 
{
	const char* code = RealSSEL2_ShortQuotation.Symbol;
	const char* exchg = "SSE";

	WTSContractInfo* ct = m_pBaseDataMgr->getContract(code, exchg);
	if (ct == NULL)
	{
		if (m_parserSink)
			//write_log(m_parserSink, LL_ERROR, "[ParserQTS] Instrument {}.{} not exists...", exchg, code);
		return;
	}

	WTSCommodityInfo* commInfo = ct->getCommInfo();

	WTSTickData* tick = WTSTickData::create(code);
	tick->setContractInfo(ct);
	WTSTickStruct& quote = tick->getTickStruct();
	strcpy(quote.exchg, exchg);

	uint32_t act_date = (uint32_t)(RealSSEL2_ShortQuotation.PacketTimeStamp / 1000000000);
	quote.action_date = act_date;
	quote.action_time = RealSSEL2_ShortQuotation.Time;
	quote.trading_date = act_date;

	quote.price = checkValid(RealSSEL2_ShortQuotation.LastPrice);
	quote.open = checkValid(RealSSEL2_ShortQuotation.OpenPrice);
	quote.high = checkValid(RealSSEL2_ShortQuotation.HighPrice);
	quote.low = checkValid(RealSSEL2_ShortQuotation.LowPrice);
	quote.total_volume = (uint32_t)RealSSEL2_ShortQuotation.TotalVolume;
	quote.total_turnover = checkValid(RealSSEL2_ShortQuotation.TotalAmount); 

	quote.ask_prices[0] = checkValid(RealSSEL2_ShortQuotation.SellPrice01);
	quote.ask_qty[0] = RealSSEL2_ShortQuotation.SellVolume01;
	quote.ask_prices[1] = checkValid(RealSSEL2_ShortQuotation.SellPrice02);
	quote.ask_qty[1] = RealSSEL2_ShortQuotation.SellVolume02;
	quote.ask_prices[2] = checkValid(RealSSEL2_ShortQuotation.SellPrice03);
	quote.ask_qty[2] = RealSSEL2_ShortQuotation.SellVolume03;
	quote.ask_prices[3] = checkValid(RealSSEL2_ShortQuotation.SellPrice04);
	quote.ask_qty[3] = RealSSEL2_ShortQuotation.SellVolume04;
	quote.ask_prices[4] = checkValid(RealSSEL2_ShortQuotation.SellPrice05);
	quote.ask_qty[4] = RealSSEL2_ShortQuotation.SellVolume05;
	quote.ask_prices[5] = checkValid(RealSSEL2_ShortQuotation.SellPrice06);
	quote.ask_qty[5] = RealSSEL2_ShortQuotation.SellVolume06;
	quote.ask_prices[6] = checkValid(RealSSEL2_ShortQuotation.SellPrice07);
	quote.ask_qty[6] = RealSSEL2_ShortQuotation.SellVolume07;
	quote.ask_prices[7] = checkValid(RealSSEL2_ShortQuotation.SellPrice08);
	quote.ask_qty[7] = RealSSEL2_ShortQuotation.SellVolume08;
	quote.ask_prices[8] = checkValid(RealSSEL2_ShortQuotation.SellPrice09);
	quote.ask_qty[8] = RealSSEL2_ShortQuotation.SellVolume09;
	quote.ask_prices[9] = checkValid(RealSSEL2_ShortQuotation.SellPrice10);
	quote.ask_qty[9] = RealSSEL2_ShortQuotation.SellVolume10;

	quote.bid_prices[0] = checkValid(RealSSEL2_ShortQuotation.BuyPrice01);
	quote.bid_qty[0] = RealSSEL2_ShortQuotation.BuyVolume01;
	quote.bid_prices[1] = checkValid(RealSSEL2_ShortQuotation.BuyPrice02);
	quote.bid_qty[1] = RealSSEL2_ShortQuotation.BuyVolume02;
	quote.bid_prices[2] = checkValid(RealSSEL2_ShortQuotation.BuyPrice03);
	quote.bid_qty[2] = RealSSEL2_ShortQuotation.BuyVolume03;
	quote.bid_prices[3] = checkValid(RealSSEL2_ShortQuotation.BuyPrice04);
	quote.bid_qty[3] = RealSSEL2_ShortQuotation.BuyVolume04;
	quote.bid_prices[4] = checkValid(RealSSEL2_ShortQuotation.BuyPrice05);
	quote.bid_qty[4] = RealSSEL2_ShortQuotation.BuyVolume05;
	quote.bid_prices[5] = checkValid(RealSSEL2_ShortQuotation.BuyPrice06);
	quote.bid_qty[5] = RealSSEL2_ShortQuotation.BuyVolume06;
	quote.bid_prices[6] = checkValid(RealSSEL2_ShortQuotation.BuyPrice07);
	quote.bid_qty[6] = RealSSEL2_ShortQuotation.BuyVolume07;
	quote.bid_prices[7] = checkValid(RealSSEL2_ShortQuotation.BuyPrice08);
	quote.bid_qty[7] = RealSSEL2_ShortQuotation.BuyVolume08;
	quote.bid_prices[8] = checkValid(RealSSEL2_ShortQuotation.BuyPrice09);
	quote.bid_qty[8] = RealSSEL2_ShortQuotation.BuyVolume09;
	quote.bid_prices[9] = checkValid(RealSSEL2_ShortQuotation.BuyPrice10);
	quote.bid_qty[9] = RealSSEL2_ShortQuotation.BuyVolume10;

	if (m_parserSink)
		m_parserSink->handleQuote(tick, 1);

	write_log(m_parserSink, LL_DEBUG, "[OnSubscribe_SSEL2_ShortQuotation] [{}.{}] act_date: {}, act_time: {}, last price: {}, open: {}, high: {}, low: {}, bid_px1: {}, bid_vol1: {}",
		code, exchg, quote.action_date, quote.action_time, quote.price, quote.open, quote.high, quote.low, quote.bid_prices[0], quote.bid_qty[0]);

	tick->release();
}

void ParserQTS::OnSubscribe_SSEL2_OrderQueue(const SSEL2_OrderQueue& RealSSEL2_OrderQueue) 
{
	const char* code = RealSSEL2_OrderQueue.Symbol;
	const char* exchg = "SSE";

	WTSContractInfo* ct = m_pBaseDataMgr->getContract(code, exchg);
	if (ct == NULL)
	{
		if (m_parserSink)
			//write_log(m_parserSink, LL_ERROR, "[ParserQTS] Instrument {}.{} not exists...", exchg, code);
		return;
	}

	uint32_t act_date = (uint32_t)(RealSSEL2_OrderQueue.PacketTimeStamp / 1000000000);

	//处理逐笔
	if (RealSSEL2_OrderQueue.BuyLevelQueueNo01 != 0)
	{
		WTSOrdQueData* buyQue = WTSOrdQueData::create(code);
		buyQue->setContractInfo(ct);

		WTSOrdQueStruct& buyOS = buyQue->getOrdQueStruct();
		strcpy(buyOS.exchg, exchg);

		buyOS.trading_date = act_date;
		buyOS.action_date = act_date;
		buyOS.action_time = RealSSEL2_OrderQueue.Time;

		buyOS.side = BDT_Buy;
		buyOS.price = RealSSEL2_OrderQueue.BuyPrice01; 
		buyOS.order_items = RealSSEL2_OrderQueue.BuyLevelQueueNo01;
		buyOS.qsize = std::min(ORDER_LEVEL_FIFTY, (int)buyOS.order_items); // 最大只有50档的数据

		buyOS.volumes[0] = RealSSEL2_OrderQueue.BuyLevelQueue01;
		buyOS.volumes[1] = RealSSEL2_OrderQueue.BuyLevelQueue02;
		buyOS.volumes[2] = RealSSEL2_OrderQueue.BuyLevelQueue03;
		buyOS.volumes[3] = RealSSEL2_OrderQueue.BuyLevelQueue04;
		buyOS.volumes[4] = RealSSEL2_OrderQueue.BuyLevelQueue05;
		buyOS.volumes[5] = RealSSEL2_OrderQueue.BuyLevelQueue06;
		buyOS.volumes[6] = RealSSEL2_OrderQueue.BuyLevelQueue07;
		buyOS.volumes[7] = RealSSEL2_OrderQueue.BuyLevelQueue08;
		buyOS.volumes[8] = RealSSEL2_OrderQueue.BuyLevelQueue09;
		buyOS.volumes[9] = RealSSEL2_OrderQueue.BuyLevelQueue10;
		buyOS.volumes[10] = RealSSEL2_OrderQueue.BuyLevelQueue11;
		buyOS.volumes[11] = RealSSEL2_OrderQueue.BuyLevelQueue12;
		buyOS.volumes[12] = RealSSEL2_OrderQueue.BuyLevelQueue13;
		buyOS.volumes[13] = RealSSEL2_OrderQueue.BuyLevelQueue14;
		buyOS.volumes[14] = RealSSEL2_OrderQueue.BuyLevelQueue15;
		buyOS.volumes[15] = RealSSEL2_OrderQueue.BuyLevelQueue16;
		buyOS.volumes[16] = RealSSEL2_OrderQueue.BuyLevelQueue17;
		buyOS.volumes[17] = RealSSEL2_OrderQueue.BuyLevelQueue18;
		buyOS.volumes[18] = RealSSEL2_OrderQueue.BuyLevelQueue19;
		buyOS.volumes[19] = RealSSEL2_OrderQueue.BuyLevelQueue20;
		buyOS.volumes[20] = RealSSEL2_OrderQueue.BuyLevelQueue21;
		buyOS.volumes[21] = RealSSEL2_OrderQueue.BuyLevelQueue22;
		buyOS.volumes[22] = RealSSEL2_OrderQueue.BuyLevelQueue23;
		buyOS.volumes[23] = RealSSEL2_OrderQueue.BuyLevelQueue24;
		buyOS.volumes[24] = RealSSEL2_OrderQueue.BuyLevelQueue25;
		buyOS.volumes[25] = RealSSEL2_OrderQueue.BuyLevelQueue26;
		buyOS.volumes[26] = RealSSEL2_OrderQueue.BuyLevelQueue27;
		buyOS.volumes[27] = RealSSEL2_OrderQueue.BuyLevelQueue28;
		buyOS.volumes[28] = RealSSEL2_OrderQueue.BuyLevelQueue29;
		buyOS.volumes[29] = RealSSEL2_OrderQueue.BuyLevelQueue30;
		buyOS.volumes[30] = RealSSEL2_OrderQueue.BuyLevelQueue31;
		buyOS.volumes[31] = RealSSEL2_OrderQueue.BuyLevelQueue32;
		buyOS.volumes[32] = RealSSEL2_OrderQueue.BuyLevelQueue33;
		buyOS.volumes[33] = RealSSEL2_OrderQueue.BuyLevelQueue34;
		buyOS.volumes[34] = RealSSEL2_OrderQueue.BuyLevelQueue35;
		buyOS.volumes[35] = RealSSEL2_OrderQueue.BuyLevelQueue36;
		buyOS.volumes[36] = RealSSEL2_OrderQueue.BuyLevelQueue37;
		buyOS.volumes[37] = RealSSEL2_OrderQueue.BuyLevelQueue38;
		buyOS.volumes[38] = RealSSEL2_OrderQueue.BuyLevelQueue39;
		buyOS.volumes[39] = RealSSEL2_OrderQueue.BuyLevelQueue40;
		buyOS.volumes[40] = RealSSEL2_OrderQueue.BuyLevelQueue41;
		buyOS.volumes[41] = RealSSEL2_OrderQueue.BuyLevelQueue42;
		buyOS.volumes[42] = RealSSEL2_OrderQueue.BuyLevelQueue43;
		buyOS.volumes[43] = RealSSEL2_OrderQueue.BuyLevelQueue44;
		buyOS.volumes[44] = RealSSEL2_OrderQueue.BuyLevelQueue45;
		buyOS.volumes[45] = RealSSEL2_OrderQueue.BuyLevelQueue46;
		buyOS.volumes[46] = RealSSEL2_OrderQueue.BuyLevelQueue47;
		buyOS.volumes[47] = RealSSEL2_OrderQueue.BuyLevelQueue48;
		buyOS.volumes[48] = RealSSEL2_OrderQueue.BuyLevelQueue49;
		buyOS.volumes[49] = RealSSEL2_OrderQueue.BuyLevelQueue50;

		if (m_parserSink)
			m_parserSink->handleOrderQueue(buyQue);

		buyQue->release();
	}

	if (RealSSEL2_OrderQueue.SellLevelQueueNo01 != 0)
	{
		WTSOrdQueData* sellQue = WTSOrdQueData::create(code);
		sellQue->setContractInfo(ct);

		WTSOrdQueStruct& sellOS = sellQue->getOrdQueStruct();
		strcpy(sellOS.exchg, exchg);

		sellOS.trading_date = act_date;
		sellOS.action_date = act_date;
		sellOS.action_time = RealSSEL2_OrderQueue.Time;

		sellOS.side = BDT_Sell;
		sellOS.price = RealSSEL2_OrderQueue.SellPrice01;
		sellOS.order_items = RealSSEL2_OrderQueue.SellLevelQueueNo01;
		sellOS.qsize = std::min(ORDER_LEVEL_FIFTY, (int)sellOS.order_items);

		sellOS.volumes[0] = RealSSEL2_OrderQueue.SellLevelQueue01;
		sellOS.volumes[1] = RealSSEL2_OrderQueue.SellLevelQueue02;
		sellOS.volumes[2] = RealSSEL2_OrderQueue.SellLevelQueue03;
		sellOS.volumes[3] = RealSSEL2_OrderQueue.SellLevelQueue04;
		sellOS.volumes[4] = RealSSEL2_OrderQueue.SellLevelQueue05;
		sellOS.volumes[5] = RealSSEL2_OrderQueue.SellLevelQueue06;
		sellOS.volumes[6] = RealSSEL2_OrderQueue.SellLevelQueue07;
		sellOS.volumes[7] = RealSSEL2_OrderQueue.SellLevelQueue08;
		sellOS.volumes[8] = RealSSEL2_OrderQueue.SellLevelQueue09;
		sellOS.volumes[9] = RealSSEL2_OrderQueue.SellLevelQueue10;
		sellOS.volumes[10] = RealSSEL2_OrderQueue.SellLevelQueue11;
		sellOS.volumes[11] = RealSSEL2_OrderQueue.SellLevelQueue12;
		sellOS.volumes[12] = RealSSEL2_OrderQueue.SellLevelQueue13;
		sellOS.volumes[13] = RealSSEL2_OrderQueue.SellLevelQueue14;
		sellOS.volumes[14] = RealSSEL2_OrderQueue.SellLevelQueue15;
		sellOS.volumes[15] = RealSSEL2_OrderQueue.SellLevelQueue16;
		sellOS.volumes[16] = RealSSEL2_OrderQueue.SellLevelQueue17;
		sellOS.volumes[17] = RealSSEL2_OrderQueue.SellLevelQueue18;
		sellOS.volumes[18] = RealSSEL2_OrderQueue.SellLevelQueue19;
		sellOS.volumes[19] = RealSSEL2_OrderQueue.SellLevelQueue20;
		sellOS.volumes[20] = RealSSEL2_OrderQueue.SellLevelQueue21;
		sellOS.volumes[21] = RealSSEL2_OrderQueue.SellLevelQueue22;
		sellOS.volumes[22] = RealSSEL2_OrderQueue.SellLevelQueue23;
		sellOS.volumes[23] = RealSSEL2_OrderQueue.SellLevelQueue24;
		sellOS.volumes[24] = RealSSEL2_OrderQueue.SellLevelQueue25;
		sellOS.volumes[25] = RealSSEL2_OrderQueue.SellLevelQueue26;
		sellOS.volumes[26] = RealSSEL2_OrderQueue.SellLevelQueue27;
		sellOS.volumes[27] = RealSSEL2_OrderQueue.SellLevelQueue28;
		sellOS.volumes[28] = RealSSEL2_OrderQueue.SellLevelQueue29;
		sellOS.volumes[29] = RealSSEL2_OrderQueue.SellLevelQueue30;
		sellOS.volumes[30] = RealSSEL2_OrderQueue.SellLevelQueue31;
		sellOS.volumes[31] = RealSSEL2_OrderQueue.SellLevelQueue32;
		sellOS.volumes[32] = RealSSEL2_OrderQueue.SellLevelQueue33;
		sellOS.volumes[33] = RealSSEL2_OrderQueue.SellLevelQueue34;
		sellOS.volumes[34] = RealSSEL2_OrderQueue.SellLevelQueue35;
		sellOS.volumes[35] = RealSSEL2_OrderQueue.SellLevelQueue36;
		sellOS.volumes[36] = RealSSEL2_OrderQueue.SellLevelQueue37;
		sellOS.volumes[37] = RealSSEL2_OrderQueue.SellLevelQueue38;
		sellOS.volumes[38] = RealSSEL2_OrderQueue.SellLevelQueue39;
		sellOS.volumes[39] = RealSSEL2_OrderQueue.SellLevelQueue40;
		sellOS.volumes[40] = RealSSEL2_OrderQueue.SellLevelQueue41;
		sellOS.volumes[41] = RealSSEL2_OrderQueue.SellLevelQueue42;
		sellOS.volumes[42] = RealSSEL2_OrderQueue.SellLevelQueue43;
		sellOS.volumes[43] = RealSSEL2_OrderQueue.SellLevelQueue44;
		sellOS.volumes[44] = RealSSEL2_OrderQueue.SellLevelQueue45;
		sellOS.volumes[45] = RealSSEL2_OrderQueue.SellLevelQueue46;
		sellOS.volumes[46] = RealSSEL2_OrderQueue.SellLevelQueue47;
		sellOS.volumes[47] = RealSSEL2_OrderQueue.SellLevelQueue48;
		sellOS.volumes[48] = RealSSEL2_OrderQueue.SellLevelQueue49;
		sellOS.volumes[49] = RealSSEL2_OrderQueue.SellLevelQueue50;

		if (m_parserSink)
			m_parserSink->handleOrderQueue(sellQue);

		sellQue->release();
	}
}

//////////////////////////////////////  深交所  ////////////////////////////////

void ParserQTS::OnSubscribe_SZSEL1_Quotation(const SZSEL1_Quotation& RealSZSEL1Quotation)
{
	const char* code = RealSZSEL1Quotation.Symbol;
	const char* exchg = "SZSE";

	WTSContractInfo* ct = m_pBaseDataMgr->getContract(code, exchg);
	if (ct == NULL)
	{
		if (m_parserSink)
			//write_log(m_parserSink, LL_ERROR, "[ParserQTS] Instrument {}.{} not exists...", exchg, code);
		return;
	}

	WTSCommodityInfo* commInfo = ct->getCommInfo();

	WTSTickData* tick = WTSTickData::create(code);
	tick->setContractInfo(ct);
	WTSTickStruct& quote = tick->getTickStruct();
	strcpy(quote.exchg, exchg);

	quote.action_date = (uint32_t)(RealSZSEL1Quotation.Time / 1000000000);  // YYYYMMDDHHMMSSMMM
	quote.action_time = (uint32_t)(RealSZSEL1Quotation.Time % 1000000000);  
	quote.trading_date = quote.action_date;

	quote.price = checkValid(RealSZSEL1Quotation.LastPrice);
	quote.open = checkValid(RealSZSEL1Quotation.OpenPrice);
	quote.high = checkValid(RealSZSEL1Quotation.HighPrice);
	quote.low = checkValid(RealSZSEL1Quotation.LowPrice);
	quote.total_volume = (uint32_t)RealSZSEL1Quotation.TotalVolume;
	quote.total_turnover = RealSZSEL1Quotation.TotalAmount;  // 成交金额需要处理

	quote.pre_close = checkValid(RealSZSEL1Quotation.PreClosePrice);

	quote.upper_limit = checkValid(RealSZSEL1Quotation.PriceUpLimit);
	quote.lower_limit = checkValid(RealSZSEL1Quotation.PriceDownLimit);

	quote.ask_prices[0] = checkValid(RealSZSEL1Quotation.SellPrice01);
	quote.ask_qty[0] = RealSZSEL1Quotation.SellVolume01;
	quote.ask_prices[1] = checkValid(RealSZSEL1Quotation.SellPrice02);
	quote.ask_qty[1] = RealSZSEL1Quotation.SellVolume02;
	quote.ask_prices[2] = checkValid(RealSZSEL1Quotation.SellPrice03);
	quote.ask_qty[2] = RealSZSEL1Quotation.SellVolume03;
	quote.ask_prices[3] = checkValid(RealSZSEL1Quotation.SellPrice04);
	quote.ask_qty[3] = RealSZSEL1Quotation.SellVolume04;
	quote.ask_prices[4] = checkValid(RealSZSEL1Quotation.SellPrice05);
	quote.ask_qty[4] = RealSZSEL1Quotation.SellVolume05;

	quote.bid_prices[0] = checkValid(RealSZSEL1Quotation.BuyPrice01);
	quote.bid_qty[0] = RealSZSEL1Quotation.BuyVolume01;
	quote.bid_prices[1] = checkValid(RealSZSEL1Quotation.BuyPrice02);
	quote.bid_qty[1] = RealSZSEL1Quotation.BuyVolume02;
	quote.bid_prices[2] = checkValid(RealSZSEL1Quotation.BuyPrice03);
	quote.bid_qty[2] = RealSZSEL1Quotation.BuyVolume03;
	quote.bid_prices[3] = checkValid(RealSZSEL1Quotation.BuyPrice04);
	quote.bid_qty[3] = RealSZSEL1Quotation.BuyVolume04;
	quote.bid_prices[4] = checkValid(RealSZSEL1Quotation.BuyPrice05);
	quote.bid_qty[4] = RealSZSEL1Quotation.BuyVolume05;

	if (m_parserSink)
		m_parserSink->handleQuote(tick, 1);

	tick->release();
}

void ParserQTS::OnSubscribe_SZSEL2_Quotation(const SZSEL2_Quotation& RealSZSEL2Quotation)
{
	const char* code = RealSZSEL2Quotation.Symbol;
	const char* exchg = "SZSE";

	WTSContractInfo* ct = m_pBaseDataMgr->getContract(code, exchg);
	if (ct == NULL)
	{
		if (m_parserSink)
			//write_log(m_parserSink, LL_ERROR, "[ParserQTS] Instrument {}.{} not exists...", exchg, code);
		return;
	}

	WTSCommodityInfo* commInfo = ct->getCommInfo();

	WTSTickData* tick = WTSTickData::create(code);
	tick->setContractInfo(ct);
	WTSTickStruct& quote = tick->getTickStruct();
	strcpy(quote.exchg, exchg);

	uint32_t act_date = (uint32_t)(RealSZSEL2Quotation.Time / 1000000000);  // YYYYMMDDHHMMSSMMM
	uint32_t act_time = (uint32_t)(RealSZSEL2Quotation.Time % 1000000000);
	quote.action_date = act_date;
	quote.trading_date = act_date;
	quote.action_time = act_time;

	quote.price = checkValid(RealSZSEL2Quotation.LastPrice);
	quote.open = checkValid(RealSZSEL2Quotation.OpenPrice);
	quote.high = checkValid(RealSZSEL2Quotation.HighPrice);
	quote.low = checkValid(RealSZSEL2Quotation.LowPrice);
	quote.total_volume = (uint32_t)RealSZSEL2Quotation.TotalVolume;
	quote.total_turnover = RealSZSEL2Quotation.TotalAmount;  // 成交金额需要处理

	quote.pre_close = checkValid(RealSZSEL2Quotation.PreClosePrice);

	quote.ask_prices[0] = checkValid(RealSZSEL2Quotation.SellPrice01);
	quote.ask_qty[0] = RealSZSEL2Quotation.SellVolume01;
	quote.ask_prices[1] = checkValid(RealSZSEL2Quotation.SellPrice02);
	quote.ask_qty[1] = RealSZSEL2Quotation.SellVolume02;
	quote.ask_prices[2] = checkValid(RealSZSEL2Quotation.SellPrice03);
	quote.ask_qty[2] = RealSZSEL2Quotation.SellVolume03;
	quote.ask_prices[3] = checkValid(RealSZSEL2Quotation.SellPrice04);
	quote.ask_qty[3] = RealSZSEL2Quotation.SellVolume04;
	quote.ask_prices[4] = checkValid(RealSZSEL2Quotation.SellPrice05);
	quote.ask_qty[4] = RealSZSEL2Quotation.SellVolume05;
	quote.ask_prices[5] = checkValid(RealSZSEL2Quotation.SellPrice06);
	quote.ask_qty[5] = RealSZSEL2Quotation.SellVolume06;
	quote.ask_prices[6] = checkValid(RealSZSEL2Quotation.SellPrice07);
	quote.ask_qty[6] = RealSZSEL2Quotation.SellVolume07;
	quote.ask_prices[7] = checkValid(RealSZSEL2Quotation.SellPrice08);
	quote.ask_qty[7] = RealSZSEL2Quotation.SellVolume08;
	quote.ask_prices[8] = checkValid(RealSZSEL2Quotation.SellPrice09);
	quote.ask_qty[8] = RealSZSEL2Quotation.SellVolume09;
	quote.ask_prices[9] = checkValid(RealSZSEL2Quotation.SellPrice10);
	quote.ask_qty[9] = RealSZSEL2Quotation.SellVolume10;

	quote.bid_prices[0] = checkValid(RealSZSEL2Quotation.BuyPrice01);
	quote.bid_qty[0] = RealSZSEL2Quotation.BuyVolume01;
	quote.bid_prices[1] = checkValid(RealSZSEL2Quotation.BuyPrice02);
	quote.bid_qty[1] = RealSZSEL2Quotation.BuyVolume02;
	quote.bid_prices[2] = checkValid(RealSZSEL2Quotation.BuyPrice03);
	quote.bid_qty[2] = RealSZSEL2Quotation.BuyVolume03;
	quote.bid_prices[3] = checkValid(RealSZSEL2Quotation.BuyPrice04);
	quote.bid_qty[3] = RealSZSEL2Quotation.BuyVolume04;
	quote.bid_prices[4] = checkValid(RealSZSEL2Quotation.BuyPrice05);
	quote.bid_qty[4] = RealSZSEL2Quotation.BuyVolume05;
	quote.bid_prices[5] = checkValid(RealSZSEL2Quotation.BuyPrice06);
	quote.bid_qty[5] = RealSZSEL2Quotation.BuyVolume06;
	quote.bid_prices[6] = checkValid(RealSZSEL2Quotation.BuyPrice07);
	quote.bid_qty[6] = RealSZSEL2Quotation.BuyVolume07;
	quote.bid_prices[7] = checkValid(RealSZSEL2Quotation.BuyPrice08);
	quote.bid_qty[7] = RealSZSEL2Quotation.BuyVolume08;
	quote.bid_prices[8] = checkValid(RealSZSEL2Quotation.BuyPrice09);
	quote.bid_qty[8] = RealSZSEL2Quotation.BuyVolume09;
	quote.bid_prices[9] = checkValid(RealSZSEL2Quotation.BuyPrice10);
	quote.bid_qty[9] = RealSZSEL2Quotation.BuyVolume10;

	if (m_parserSink)
		m_parserSink->handleQuote(tick, 1);

	//处理逐笔
	if (RealSZSEL2Quotation.BuyLevelQueueNo01 != 0)
	{
		WTSOrdQueData* buyQue = WTSOrdQueData::create(code);
		buyQue->setContractInfo(ct);

		WTSOrdQueStruct& buyOS = buyQue->getOrdQueStruct();
		strcpy(buyOS.exchg, exchg);

		buyOS.trading_date = act_date;
		buyOS.action_date = act_date;
		buyOS.action_time = act_time;

		buyOS.side = BDT_Buy;
		buyOS.price = RealSZSEL2Quotation.BuyLevel[0].Price; // quote.bid_prices[0];
		buyOS.order_items = RealSZSEL2Quotation.BuyLevel[0].TotalOrderNo;
		buyOS.qsize = std::min(ORDER_LEVEL_FIFTY, (int)RealSZSEL2Quotation.BuyLevelQueueNo01); // 最大只有50档的数据

		for (uint32_t i = 0; i < buyOS.qsize; i++)
		{
			buyOS.volumes[i] = (uint32_t)RealSZSEL2Quotation.BuyLevelQueue[i];
		}

		if (m_parserSink)
			m_parserSink->handleOrderQueue(buyQue);

		buyQue->release();
	}

	if (RealSZSEL2Quotation.SellLevelQueueNo01 != 0)
	{
		WTSOrdQueData* sellQue = WTSOrdQueData::create(code);
		sellQue->setContractInfo(ct);

		WTSOrdQueStruct& sellOS = sellQue->getOrdQueStruct();
		strcpy(sellOS.exchg, exchg);

		sellOS.trading_date = act_date;
		sellOS.action_date = act_date;
		sellOS.action_time = act_time;

		sellOS.side = BDT_Sell;
		sellOS.price = RealSZSEL2Quotation.SellLevel[0].Price;
		sellOS.order_items = RealSZSEL2Quotation.SellLevel[0].TotalOrderNo;
		sellOS.qsize = std::min(ORDER_LEVEL_FIFTY, (int)RealSZSEL2Quotation.SellLevelQueueNo01);

		for (uint32_t i = 0; i < sellOS.qsize; i++)
		{
			sellOS.volumes[i] = (uint32_t)RealSZSEL2Quotation.SellLevelQueue[i];
		}

		if (m_parserSink)
			m_parserSink->handleOrderQueue(sellQue);

		sellQue->release();
	}

	tick->release();
}

void ParserQTS::OnSubscribe_SZSEL2_Transaction(const SZSEL2_Transaction& RealSZSEL2Transaction)
{
	const char* code = RealSZSEL2Transaction.Symbol;
	const char* exchg = "SZSE";
	auto ct = m_pBaseDataMgr->getContract(code, exchg);
	if (ct == NULL)
		return;

	WTSTransData* trans = WTSTransData::create(code);
	WTSTransStruct& ts = trans->getTransStruct();
	strcpy(ts.exchg, exchg);

	uint32_t act_date = (uint32_t)(RealSZSEL2Transaction.TradeTime / 1000000000);  // YYYYMMDDHHMMSSMMM
	uint32_t act_time = (uint32_t)(RealSZSEL2Transaction.TradeTime % 1000000000);

	ts.trading_date = act_date;
	ts.action_date = act_date;
	ts.action_time = act_time;  

	ts.price = checkValid(RealSZSEL2Transaction.TradePrice);
	ts.volume = RealSZSEL2Transaction.TradeVolume;
	ts.askorder = (uint32_t)RealSZSEL2Transaction.BuyOrderID;
	ts.bidorder = (uint32_t)RealSZSEL2Transaction.SellOrderID;
	ts.index = (uint32_t)RealSZSEL2Transaction.RecID;  // 成交序号
	ts.side = BDT_Unknown;  // RealSZSEL2Transaction.TradeType == 'F' ? BDT_Buy : BDT_Sell; TODO:
	ts.ttype = RealSZSEL2Transaction.TradeType == 'F' ? TT_Match : TT_Cancel;

	if (m_parserSink)
		m_parserSink->handleTransaction(trans);

	//write_log(m_parserSink, LL_DEBUG, "[OnSubscribe_SZSEL2_Transaction] {}.{}, act_time: {}, price: {}, volume: {}, index: {}, askorder: {}, bidorder: {}",
	//	code, exchg, ts.action_time, ts.price, ts.volume, ts.index, ts.askorder, ts.bidorder);

	trans->release();
}

void ParserQTS::OnSubscribe_SZSEL2_Order(const SZSEL2_Order& RealSZSEL2Order)
{
	const char* code = RealSZSEL2Order.Symbol;
	const char* exchg = "SZSE";
	auto ct = m_pBaseDataMgr->getContract(code, exchg);
	if (ct == NULL)
		return;

	WTSOrdDtlData* order = WTSOrdDtlData::create(code);
	WTSOrdDtlStruct& ods = order->getOrdDtlStruct();
	strcpy(ods.exchg, exchg);

	uint32_t act_date = (uint32_t)(RealSZSEL2Order.Time / 1000000000);  // YYYYMMDDHHMMSSMMM
	uint32_t act_time = (uint32_t)(RealSZSEL2Order.Time % 1000000000);

	ods.action_date = act_date;
	ods.trading_date = act_date;
	ods.action_time = act_time;

	ods.side = RealSZSEL2Order.OrderCode == '1' ? BDT_Buy : BDT_Sell;
	ods.price = checkValid(RealSZSEL2Order.OrderPrice);
	ods.volume = RealSZSEL2Order.OrderVolume;
	ods.index = (uint32_t)RealSZSEL2Order.RecID;
	ods.otype = wrapOrderType(RealSZSEL2Order.OrderType);  

	if (m_parserSink)
		m_parserSink->handleOrderDetail(order);

	//write_log(m_parserSink, LL_DEBUG, "[OnSubscribe_SZSEL2_Order] {}.{}, act_time: {}, price: {}, volume: {}, index: {}",
	//	code, exchg, ods.action_time, ods.price, ods.volume, ods.index);

	order->release();
}

void ParserQTS::OnSubscribe_SZSEL2_Index(const SZSEL2_Index& RealSZSEL2Index)
{
	const char* code = RealSZSEL2Index.Symbol;
	const char* exchg = "SZSE";
	auto ct = m_pBaseDataMgr->getContract(code, exchg);
	if (ct == NULL)
	{
		if (m_parserSink)
			//write_log(m_parserSink, LL_ERROR, "[ParserQTS] Instrument {}.{} not exists...", exchg, code);
		return;
	}

	WTSTickData* tick = WTSTickData::create(code);
	WTSTickStruct& quote = tick->getTickStruct();
	strcpy(quote.exchg, exchg);

	uint32_t act_date = (uint32_t)(RealSZSEL2Index.Time / 1000000000);  // YYYYMMDDHHMMSSMMM
	uint32_t act_time = (uint32_t)(RealSZSEL2Index.Time % 1000000000);

	quote.action_date = act_date;
	quote.trading_date = quote.action_date;
	quote.action_time = act_time;

	quote.price = checkValid(RealSZSEL2Index.LastPrice);
	quote.open = checkValid(RealSZSEL2Index.OpenPrice);
	quote.high = checkValid(RealSZSEL2Index.HighPrice);
	quote.low = checkValid(RealSZSEL2Index.LastPrice);
	quote.pre_close = checkValid(RealSZSEL2Index.PreClosePrice);

	quote.total_volume = RealSZSEL2Index.TotalVolume; //
	quote.total_turnover = RealSZSEL2Index.TotalAmount; 

	if (m_parserSink)
		m_parserSink->handleQuote(tick, true);

	tick->release();
}

void ParserQTS::OnSubscribe_SZSEL2_BondQuotation(const SZSEL2_BondQuotation& RealSZSEL2BondQuotation)
{
	const char* code = RealSZSEL2BondQuotation.Symbol;
	const char* exchg = "SZSE";

	WTSContractInfo* ct = m_pBaseDataMgr->getContract(code, exchg);
	if (ct == NULL)
	{
		if (m_parserSink)
			//write_log(m_parserSink, LL_ERROR, "[ParserQTS] Instrument {}.{} not exists...", exchg, code);
		return;
	}

	WTSCommodityInfo* commInfo = ct->getCommInfo();

	WTSTickData* tick = WTSTickData::create(code);
	tick->setContractInfo(ct);
	WTSTickStruct& quote = tick->getTickStruct();
	strcpy(quote.exchg, exchg);

	uint32_t act_date = (uint32_t)(RealSZSEL2BondQuotation.Time / 1000000000);  // YYYYMMDDHHMMSSMMM
	uint32_t act_time = (uint32_t)(RealSZSEL2BondQuotation.Time % 1000000000);

	quote.action_date = act_date;
	quote.action_time = act_time;
	quote.trading_date = act_date;

	quote.price = checkValid(RealSZSEL2BondQuotation.LastPrice);
	quote.open = checkValid(RealSZSEL2BondQuotation.OpenPrice);
	quote.high = checkValid(RealSZSEL2BondQuotation.HighPrice);
	quote.low = checkValid(RealSZSEL2BondQuotation.LowPrice);
	quote.total_volume = RealSZSEL2BondQuotation.TotalVolume;
	quote.total_turnover = RealSZSEL2BondQuotation.TotalAmount;  

	quote.pre_close = checkValid(RealSZSEL2BondQuotation.PreClosePrice);

	quote.ask_prices[0] = checkValid(RealSZSEL2BondQuotation.SellPrice01);
	quote.ask_qty[0] = RealSZSEL2BondQuotation.SellVolume01;
	quote.ask_prices[1] = checkValid(RealSZSEL2BondQuotation.SellPrice02);
	quote.ask_qty[1] = RealSZSEL2BondQuotation.SellVolume02;
	quote.ask_prices[2] = checkValid(RealSZSEL2BondQuotation.SellPrice03);
	quote.ask_qty[2] = RealSZSEL2BondQuotation.SellVolume03;
	quote.ask_prices[3] = checkValid(RealSZSEL2BondQuotation.SellPrice04);
	quote.ask_qty[3] = RealSZSEL2BondQuotation.SellVolume04;
	quote.ask_prices[4] = checkValid(RealSZSEL2BondQuotation.SellPrice05);
	quote.ask_qty[4] = RealSZSEL2BondQuotation.SellVolume05;
	quote.ask_prices[5] = checkValid(RealSZSEL2BondQuotation.SellPrice06);
	quote.ask_qty[5] = RealSZSEL2BondQuotation.SellVolume06;
	quote.ask_prices[6] = checkValid(RealSZSEL2BondQuotation.SellPrice07);
	quote.ask_qty[6] = RealSZSEL2BondQuotation.SellVolume07;
	quote.ask_prices[7] = checkValid(RealSZSEL2BondQuotation.SellPrice08);
	quote.ask_qty[7] = RealSZSEL2BondQuotation.SellVolume08;
	quote.ask_prices[8] = checkValid(RealSZSEL2BondQuotation.SellPrice09);
	quote.ask_qty[8] = RealSZSEL2BondQuotation.SellVolume09;
	quote.ask_prices[9] = checkValid(RealSZSEL2BondQuotation.SellPrice10);
	quote.ask_qty[9] = RealSZSEL2BondQuotation.SellVolume10;

	quote.bid_prices[0] = checkValid(RealSZSEL2BondQuotation.BuyPrice01);
	quote.bid_qty[0] = RealSZSEL2BondQuotation.BuyVolume01;
	quote.bid_prices[1] = checkValid(RealSZSEL2BondQuotation.BuyPrice02);
	quote.bid_qty[1] = RealSZSEL2BondQuotation.BuyVolume02;
	quote.bid_prices[2] = checkValid(RealSZSEL2BondQuotation.BuyPrice03);
	quote.bid_qty[2] = RealSZSEL2BondQuotation.BuyVolume03;
	quote.bid_prices[3] = checkValid(RealSZSEL2BondQuotation.BuyPrice04);
	quote.bid_qty[3] = RealSZSEL2BondQuotation.BuyVolume04;
	quote.bid_prices[4] = checkValid(RealSZSEL2BondQuotation.BuyPrice05);
	quote.bid_qty[4] = RealSZSEL2BondQuotation.BuyVolume05;
	quote.bid_prices[5] = checkValid(RealSZSEL2BondQuotation.BuyPrice06);
	quote.bid_qty[5] = RealSZSEL2BondQuotation.BuyVolume06;
	quote.bid_prices[6] = checkValid(RealSZSEL2BondQuotation.BuyPrice07);
	quote.bid_qty[6] = RealSZSEL2BondQuotation.BuyVolume07;
	quote.bid_prices[7] = checkValid(RealSZSEL2BondQuotation.BuyPrice08);
	quote.bid_qty[7] = RealSZSEL2BondQuotation.BuyVolume08;
	quote.bid_prices[8] = checkValid(RealSZSEL2BondQuotation.BuyPrice09);
	quote.bid_qty[8] = RealSZSEL2BondQuotation.BuyVolume09;
	quote.bid_prices[9] = checkValid(RealSZSEL2BondQuotation.BuyPrice10);
	quote.bid_qty[9] = RealSZSEL2BondQuotation.BuyVolume10;

	if (m_parserSink)
		m_parserSink->handleQuote(tick, 1);

	tick->release();

	//处理逐笔
	if (RealSZSEL2BondQuotation.BuyLevelQueueNo01 != 0)
	{
		WTSOrdQueData* buyQue = WTSOrdQueData::create(code);
		buyQue->setContractInfo(ct);

		WTSOrdQueStruct& buyOS = buyQue->getOrdQueStruct();
		strcpy(buyOS.exchg, exchg);

		buyOS.trading_date = act_date;
		buyOS.action_date = act_date;
		buyOS.action_time = act_time;

		buyOS.side = BDT_Buy;
		buyOS.price = RealSZSEL2BondQuotation.BuyLevel[0].Price; // quote.bid_prices[0];
		buyOS.order_items = RealSZSEL2BondQuotation.BuyLevelQueueNo01;
		buyOS.qsize = std::min(ORDER_LEVEL_FIFTY, (int)buyOS.order_items); // 最大只有50档的数据

		for (uint32_t i = 0; i < buyOS.qsize; i++)
		{
			buyOS.volumes[i] = (uint32_t)RealSZSEL2BondQuotation.BuyLevelQueue[i];
		}

		if (m_parserSink)
			m_parserSink->handleOrderQueue(buyQue);

		buyQue->release();
	}

	if (RealSZSEL2BondQuotation.SellLevelQueueNo01 != 0)
	{
		WTSOrdQueData* sellQue = WTSOrdQueData::create(code);
		sellQue->setContractInfo(ct);

		WTSOrdQueStruct& sellOS = sellQue->getOrdQueStruct();
		strcpy(sellOS.exchg, exchg);

		sellOS.trading_date = act_date;
		sellOS.action_date = act_date;
		sellOS.action_time = RealSZSEL2BondQuotation.Time;

		sellOS.side = BDT_Sell;
		sellOS.price = RealSZSEL2BondQuotation.SellLevel[0].Price;
		sellOS.order_items = RealSZSEL2BondQuotation.SellLevelQueueNo01;
		sellOS.qsize = std::min(ORDER_LEVEL_FIFTY, (int)sellOS.order_items);

		for (uint32_t i = 0; i < sellOS.qsize; i++)
		{
			sellOS.volumes[i] = (uint32_t)RealSZSEL2BondQuotation.SellLevelQueue[i];
		}

		if (m_parserSink)
			m_parserSink->handleOrderQueue(sellQue);

		sellQue->release();
	}
}

void ParserQTS::OnSubscribe_SZSEL2_BondTransaction(const SZSEL2_BondTransaction& RealSZSEL2BondTransaction)
{
	const char* code = RealSZSEL2BondTransaction.Symbol;
	const char* exchg = "SZSE";
	auto ct = m_pBaseDataMgr->getContract(code, exchg);
	if (ct == NULL)
		return;

	WTSTransData* trans = WTSTransData::create(code);
	WTSTransStruct& ts = trans->getTransStruct();
	strcpy(ts.exchg, exchg);

	uint32_t act_date = (uint32_t)(RealSZSEL2BondTransaction.TradeTime / 1000000000);  // YYYYMMDDHHMMSSMMM
	uint32_t act_time = (uint32_t)(RealSZSEL2BondTransaction.TradeTime % 1000000000);

	ts.trading_date = act_date;
	ts.action_date = act_date;
	ts.action_time = act_time;

	ts.price = checkValid(RealSZSEL2BondTransaction.TradePrice);
	ts.volume = RealSZSEL2BondTransaction.TradeVolume;
	ts.askorder = (uint32_t)RealSZSEL2BondTransaction.BuyOrderID;
	ts.bidorder = (uint32_t)RealSZSEL2BondTransaction.SellOrderID;
	ts.index = (uint32_t)RealSZSEL2BondTransaction.RecID;  // 成交序号
	ts.side = BDT_Unknown;  // RealSZSEL2BondTransaction.BuySellFlag == 'B' ? BDT_Buy : BDT_Sell;
	ts.ttype = RealSZSEL2BondTransaction.TradeType == 'F' ? TT_Match : TT_Cancel;

	if (m_parserSink)
		m_parserSink->handleTransaction(trans);

	trans->release();
}

void ParserQTS::OnSubscribe_SZSEL2_BondOrder(const SZSEL2_BondOrder& RealSZSEL2BondOrder)
{
	const char* code = RealSZSEL2BondOrder.Symbol;
	const char* exchg = "SZSE";
	auto ct = m_pBaseDataMgr->getContract(code, exchg);
	if (ct == NULL)
		return;

	WTSOrdDtlData* order = WTSOrdDtlData::create(code);
	WTSOrdDtlStruct& ods = order->getOrdDtlStruct();
	strcpy(ods.exchg, exchg);

	uint32_t act_date = (uint32_t)(RealSZSEL2BondOrder.Time / 1000000000);  // YYYYMMDDHHMMSSMMM
	uint32_t act_time = (uint32_t)(RealSZSEL2BondOrder.Time % 1000000000);

	ods.action_date = act_date;
	ods.trading_date = ods.action_date;
	ods.action_time = act_time;

	ods.side = wrapOrderCode(RealSZSEL2BondOrder.OrderCode);
	ods.price = checkValid(RealSZSEL2BondOrder.OrderPrice);
	ods.volume = RealSZSEL2BondOrder.OrderVolume;
	ods.index = (uint32_t)RealSZSEL2BondOrder.RecID;
	ods.otype = wrapOrderType(RealSZSEL2BondOrder.OrderType); 

	if (m_parserSink)
		m_parserSink->handleOrderDetail(order);

	order->release();
}

void ParserQTS::OnSubscribe_SZSEL2_BondTick(const SZSEL2_BondTick& RealSZSEL2_BondTick)
{
	const char* code = RealSZSEL2_BondTick.Symbol;
	const char* exchg = "SZSE";
	auto ct = m_pBaseDataMgr->getContract(code, exchg);
	if (ct == NULL)
		return;

	uint32_t act_date = (uint32_t)(RealSZSEL2_BondTick.Time / 1000000000);  // YYYYMMDDHHMMSSMMM
	uint32_t act_time = (uint32_t)(RealSZSEL2_BondTick.Time % 1000000000);

	if (RealSZSEL2_BondTick.TickType == 'F' || RealSZSEL2_BondTick.TickType == '4')
	{
		WTSTransData* trans = WTSTransData::create(code);
		WTSTransStruct& ts = trans->getTransStruct();
		strcpy(ts.exchg, exchg);

		ts.trading_date = act_date;
		ts.action_date = act_date;
		ts.action_time = act_time;

		ts.price = checkValid(RealSZSEL2_BondTick.TickPrice);
		ts.volume = RealSZSEL2_BondTick.TickVolume;
		ts.askorder = (uint32_t)RealSZSEL2_BondTick.SellOrderID;
		ts.bidorder = (uint32_t)RealSZSEL2_BondTick.BuyOrderID;
		ts.index = (uint32_t)RealSZSEL2_BondTick.RecID;
		ts.side = wrapOrderCode(RealSZSEL2_BondTick.OrderCode);
		ts.ttype = RealSZSEL2_BondTick.TickType == 'F' ? TT_Match : TT_Cancel;

		if (m_parserSink)
			m_parserSink->handleTransaction(trans);

		trans->release();
	}
	else
	{
		WTSOrdDtlData* order = WTSOrdDtlData::create(code);
		WTSOrdDtlStruct& ods = order->getOrdDtlStruct();
		strcpy(ods.exchg, exchg);

		ods.action_date = act_date;
		ods.trading_date = act_date;
		ods.action_time = act_time;
		ods.side = wrapOrderCode(RealSZSEL2_BondTick.OrderCode);
		ods.price = checkValid(RealSZSEL2_BondTick.TickPrice);
		ods.volume = RealSZSEL2_BondTick.TickVolume;
		ods.index = (uint32_t)RealSZSEL2_BondTick.RecID;
		ods.otype = wrapOrderType(RealSZSEL2_BondTick.TickType);  // 没提供订单类型的字段，默认是限价单

		if (m_parserSink)
			m_parserSink->handleOrderDetail(order);

		order->release();
	}
}

void ParserQTS::OnSubscribe_SZSEL1_BondQuotation(const SZSEL1_BondQuotation& RealSZSEL1BondQuotation)
{
	const char* code = RealSZSEL1BondQuotation.Symbol;
	const char* exchg = "SZSE";

	WTSContractInfo* ct = m_pBaseDataMgr->getContract(code, exchg);
	if (ct == NULL)
	{
		if (m_parserSink)
			//write_log(m_parserSink, LL_ERROR, "[ParserQTS] Instrument {}.{} not exists...", exchg, code);
		return;
	}

	WTSCommodityInfo* commInfo = ct->getCommInfo();

	WTSTickData* tick = WTSTickData::create(code);
	tick->setContractInfo(ct);
	WTSTickStruct& quote = tick->getTickStruct();
	strcpy(quote.exchg, exchg);

	uint32_t act_date = (uint32_t)(RealSZSEL1BondQuotation.Time / 1000000000);  // YYYYMMDDHHMMSSMMM
	uint32_t act_time = (uint32_t)(RealSZSEL1BondQuotation.Time % 1000000000);

	quote.action_date = act_date;
	quote.action_time = act_time;  // HHMMssmmm
	quote.trading_date = act_date;

	quote.price = checkValid(RealSZSEL1BondQuotation.LastPrice);
	quote.open = checkValid(RealSZSEL1BondQuotation.OpenPrice);
	quote.high = checkValid(RealSZSEL1BondQuotation.HighPrice);
	quote.low = checkValid(RealSZSEL1BondQuotation.LowPrice);
	quote.total_volume = (uint32_t)RealSZSEL1BondQuotation.TotalVolume;
	quote.total_turnover = RealSZSEL1BondQuotation.TotalAmount;

	quote.pre_close = checkValid(RealSZSEL1BondQuotation.PreClosePrice);

	quote.ask_prices[0] = checkValid(RealSZSEL1BondQuotation.SellPrice01);
	quote.ask_qty[0] = RealSZSEL1BondQuotation.SellVolume01;
	quote.ask_prices[1] = checkValid(RealSZSEL1BondQuotation.SellPrice02);
	quote.ask_qty[1] = RealSZSEL1BondQuotation.SellVolume02;
	quote.ask_prices[2] = checkValid(RealSZSEL1BondQuotation.SellPrice03);
	quote.ask_qty[2] = RealSZSEL1BondQuotation.SellVolume03;
	quote.ask_prices[3] = checkValid(RealSZSEL1BondQuotation.SellPrice04);
	quote.ask_qty[3] = RealSZSEL1BondQuotation.SellVolume04;
	quote.ask_prices[4] = checkValid(RealSZSEL1BondQuotation.SellPrice05);
	quote.ask_qty[4] = RealSZSEL1BondQuotation.SellVolume05;

	quote.bid_prices[0] = checkValid(RealSZSEL1BondQuotation.BuyPrice01);
	quote.bid_qty[0] = RealSZSEL1BondQuotation.BuyVolume01;
	quote.bid_prices[1] = checkValid(RealSZSEL1BondQuotation.BuyPrice02);
	quote.bid_qty[1] = RealSZSEL1BondQuotation.BuyVolume02;
	quote.bid_prices[2] = checkValid(RealSZSEL1BondQuotation.BuyPrice03);
	quote.bid_qty[2] = RealSZSEL1BondQuotation.BuyVolume03;
	quote.bid_prices[3] = checkValid(RealSZSEL1BondQuotation.BuyPrice04);
	quote.bid_qty[3] = RealSZSEL1BondQuotation.BuyVolume04;
	quote.bid_prices[4] = checkValid(RealSZSEL1BondQuotation.BuyPrice05);
	quote.bid_qty[4] = RealSZSEL1BondQuotation.BuyVolume05;

	if (m_parserSink)
		m_parserSink->handleQuote(tick, 1);

	tick->release();
}

void ParserQTS::OnSubscribe_SZSEIOL1_Quotation(const SZSEIOL1_Quotation& RealSZSEIOL1Quotation)
{
	const char* code = RealSZSEIOL1Quotation.Symbol;
	const char* exchg = "SZSE";

	WTSContractInfo* ct = m_pBaseDataMgr->getContract(code, exchg);
	if (ct == NULL)
	{
		if (m_parserSink)
			//write_log(m_parserSink, LL_ERROR, "[ParserQTS] Instrument {}.{} not exists...", exchg, code);
		return;
	}

	WTSCommodityInfo* commInfo = ct->getCommInfo();

	WTSTickData* tick = WTSTickData::create(code);
	tick->setContractInfo(ct);
	WTSTickStruct& quote = tick->getTickStruct();
	strcpy(quote.exchg, exchg);

	uint32_t act_date = (uint32_t)(RealSZSEIOL1Quotation.Time / 1000000000);  // YYYYMMDDHHMMSSMMM
	uint32_t act_time = (uint32_t)(RealSZSEIOL1Quotation.Time % 1000000000);

	quote.action_date = act_date;
	quote.action_time = act_time;  // HHMMssmmm
	quote.trading_date = act_date;

	quote.price = checkValid(RealSZSEIOL1Quotation.LastPrice);
	quote.open = checkValid(RealSZSEIOL1Quotation.OpenPrice);
	quote.high = checkValid(RealSZSEIOL1Quotation.HighPrice);
	quote.low = checkValid(RealSZSEIOL1Quotation.LowPrice);
	quote.total_volume = (uint32_t)RealSZSEIOL1Quotation.TotalVolume;
	quote.total_turnover = RealSZSEIOL1Quotation.TotalAmount;

	quote.pre_close = checkValid(RealSZSEIOL1Quotation.PreClosePrice);

	quote.upper_limit = checkValid(RealSZSEIOL1Quotation.PriceUpLimit);
	quote.lower_limit = checkValid(RealSZSEIOL1Quotation.PriceDownLimit);

	quote.ask_prices[0] = checkValid(RealSZSEIOL1Quotation.SellPrice01);
	quote.ask_qty[0] = RealSZSEIOL1Quotation.SellVolume01;
	quote.ask_prices[1] = checkValid(RealSZSEIOL1Quotation.SellPrice02);
	quote.ask_qty[1] = RealSZSEIOL1Quotation.SellVolume02;
	quote.ask_prices[2] = checkValid(RealSZSEIOL1Quotation.SellPrice03);
	quote.ask_qty[2] = RealSZSEIOL1Quotation.SellVolume03;
	quote.ask_prices[3] = checkValid(RealSZSEIOL1Quotation.SellPrice04);
	quote.ask_qty[3] = RealSZSEIOL1Quotation.SellVolume04;
	quote.ask_prices[4] = checkValid(RealSZSEIOL1Quotation.SellPrice05);
	quote.ask_qty[4] = RealSZSEIOL1Quotation.SellVolume05;

	quote.bid_prices[0] = checkValid(RealSZSEIOL1Quotation.BuyPrice01);
	quote.bid_qty[0] = RealSZSEIOL1Quotation.BuyVolume01;
	quote.bid_prices[1] = checkValid(RealSZSEIOL1Quotation.BuyPrice02);
	quote.bid_qty[1] = RealSZSEIOL1Quotation.BuyVolume02;
	quote.bid_prices[2] = checkValid(RealSZSEIOL1Quotation.BuyPrice03);
	quote.bid_qty[2] = RealSZSEIOL1Quotation.BuyVolume03;
	quote.bid_prices[3] = checkValid(RealSZSEIOL1Quotation.BuyPrice04);
	quote.bid_qty[3] = RealSZSEIOL1Quotation.BuyVolume04;
	quote.bid_prices[4] = checkValid(RealSZSEIOL1Quotation.BuyPrice05);
	quote.bid_qty[4] = RealSZSEIOL1Quotation.BuyVolume05;

	if (m_parserSink)
		m_parserSink->handleQuote(tick, 1);

	tick->release();
}

void ParserQTS::OnSubscribe_SZSEL2_ShortQuotation(const SZSEL2_ShortQuotation& RealSZSEL2_ShortQuotation)
{
	const char* code = RealSZSEL2_ShortQuotation.Symbol;
	const char* exchg = "SZSE";

	WTSContractInfo* ct = m_pBaseDataMgr->getContract(code, exchg);
	if (ct == NULL)
	{
		if (m_parserSink)
			//write_log(m_parserSink, LL_ERROR, "[ParserQTS] Instrument {}.{} not exists...", exchg, code);
		return;
	}

	WTSCommodityInfo* commInfo = ct->getCommInfo();

	WTSTickData* tick = WTSTickData::create(code);
	tick->setContractInfo(ct);
	WTSTickStruct& quote = tick->getTickStruct();
	strcpy(quote.exchg, exchg);

	uint32_t act_date = (uint32_t)(RealSZSEL2_ShortQuotation.Time / 1000000000);  // YYYYMMDDHHMMSSMMM
	uint32_t act_time = (uint32_t)(RealSZSEL2_ShortQuotation.Time % 1000000000);

	quote.action_date = act_date;
	quote.action_time = act_time;
	quote.trading_date = act_date;

	quote.price = checkValid(RealSZSEL2_ShortQuotation.LastPrice);
	quote.open = checkValid(RealSZSEL2_ShortQuotation.OpenPrice);
	quote.high = checkValid(RealSZSEL2_ShortQuotation.HighPrice);
	quote.low = checkValid(RealSZSEL2_ShortQuotation.LowPrice);
	quote.total_volume = RealSZSEL2_ShortQuotation.TotalVolume;
	quote.total_turnover = RealSZSEL2_ShortQuotation.TotalAmount;

	quote.upper_limit = checkValid(RealSZSEL2_ShortQuotation.PriceUpLimit);
	quote.lower_limit = checkValid(RealSZSEL2_ShortQuotation.PriceDownLimit);

	quote.ask_prices[0] = checkValid(RealSZSEL2_ShortQuotation.SellPrice01);
	quote.ask_qty[0] = RealSZSEL2_ShortQuotation.SellVolume01;
	quote.ask_prices[1] = checkValid(RealSZSEL2_ShortQuotation.SellPrice02);
	quote.ask_qty[1] = RealSZSEL2_ShortQuotation.SellVolume02;
	quote.ask_prices[2] = checkValid(RealSZSEL2_ShortQuotation.SellPrice03);
	quote.ask_qty[2] = RealSZSEL2_ShortQuotation.SellVolume03;
	quote.ask_prices[3] = checkValid(RealSZSEL2_ShortQuotation.SellPrice04);
	quote.ask_qty[3] = RealSZSEL2_ShortQuotation.SellVolume04;
	quote.ask_prices[4] = checkValid(RealSZSEL2_ShortQuotation.SellPrice05);
	quote.ask_qty[4] = RealSZSEL2_ShortQuotation.SellVolume05;
	quote.ask_prices[5] = checkValid(RealSZSEL2_ShortQuotation.SellPrice06);
	quote.ask_qty[5] = RealSZSEL2_ShortQuotation.SellVolume06;
	quote.ask_prices[6] = checkValid(RealSZSEL2_ShortQuotation.SellPrice07);
	quote.ask_qty[6] = RealSZSEL2_ShortQuotation.SellVolume07;
	quote.ask_prices[7] = checkValid(RealSZSEL2_ShortQuotation.SellPrice08);
	quote.ask_qty[7] = RealSZSEL2_ShortQuotation.SellVolume08;
	quote.ask_prices[8] = checkValid(RealSZSEL2_ShortQuotation.SellPrice09);
	quote.ask_qty[8] = RealSZSEL2_ShortQuotation.SellVolume09;
	quote.ask_prices[9] = checkValid(RealSZSEL2_ShortQuotation.SellPrice10);
	quote.ask_qty[9] = RealSZSEL2_ShortQuotation.SellVolume10;

	quote.bid_prices[0] = checkValid(RealSZSEL2_ShortQuotation.BuyPrice01);
	quote.bid_qty[0] = RealSZSEL2_ShortQuotation.BuyVolume01;
	quote.bid_prices[1] = checkValid(RealSZSEL2_ShortQuotation.BuyPrice02);
	quote.bid_qty[1] = RealSZSEL2_ShortQuotation.BuyVolume02;
	quote.bid_prices[2] = checkValid(RealSZSEL2_ShortQuotation.BuyPrice03);
	quote.bid_qty[2] = RealSZSEL2_ShortQuotation.BuyVolume03;
	quote.bid_prices[3] = checkValid(RealSZSEL2_ShortQuotation.BuyPrice04);
	quote.bid_qty[3] = RealSZSEL2_ShortQuotation.BuyVolume04;
	quote.bid_prices[4] = checkValid(RealSZSEL2_ShortQuotation.BuyPrice05);
	quote.bid_qty[4] = RealSZSEL2_ShortQuotation.BuyVolume05;
	quote.bid_prices[5] = checkValid(RealSZSEL2_ShortQuotation.BuyPrice06);
	quote.bid_qty[5] = RealSZSEL2_ShortQuotation.BuyVolume06;
	quote.bid_prices[6] = checkValid(RealSZSEL2_ShortQuotation.BuyPrice07);
	quote.bid_qty[6] = RealSZSEL2_ShortQuotation.BuyVolume07;
	quote.bid_prices[7] = checkValid(RealSZSEL2_ShortQuotation.BuyPrice08);
	quote.bid_qty[7] = RealSZSEL2_ShortQuotation.BuyVolume08;
	quote.bid_prices[8] = checkValid(RealSZSEL2_ShortQuotation.BuyPrice09);
	quote.bid_qty[8] = RealSZSEL2_ShortQuotation.BuyVolume09;
	quote.bid_prices[9] = checkValid(RealSZSEL2_ShortQuotation.BuyPrice10);
	quote.bid_qty[9] = RealSZSEL2_ShortQuotation.BuyVolume10;

	if (m_parserSink)
		m_parserSink->handleQuote(tick, 1);

	write_log(m_parserSink, LL_DEBUG, "[OnSubscribe_SZSEL2_ShortQuotation] [{}.{}] act_date: {}, act_time: {}, last price: {}, open: {}, high: {}, low: {}, bid_px1: {}, bid_vol1: {}",
		code, exchg, quote.action_date, quote.action_time, quote.price, quote.open, quote.high, quote.low, quote.bid_prices[0], quote.bid_qty[0]);

	tick->release();
}

void ParserQTS::OnSubscribe_SZSEL2_OrderQueue(const SZSEL2_OrderQueue& RealSZSEL2_OrderQueue)
{
	const char* code = RealSZSEL2_OrderQueue.Symbol;
	const char* exchg = "SZSE";

	WTSContractInfo* ct = m_pBaseDataMgr->getContract(code, exchg);
	if (ct == NULL)
	{
		if (m_parserSink)
		return;
	}

	uint32_t act_date = (uint32_t)(RealSZSEL2_OrderQueue.Time / 1000000000);  // YYYYMMDDHHMMSSMMM
	uint32_t act_time = (uint32_t)(RealSZSEL2_OrderQueue.Time % 1000000000);

	//处理逐笔
	if (RealSZSEL2_OrderQueue.BuyLevelQueueNo01 != 0)
	{
		WTSOrdQueData* buyQue = WTSOrdQueData::create(code);
		buyQue->setContractInfo(ct);

		WTSOrdQueStruct& buyOS = buyQue->getOrdQueStruct();
		strcpy(buyOS.exchg, exchg);

		buyOS.trading_date = act_date;
		buyOS.action_date = act_date;
		buyOS.action_time = RealSZSEL2_OrderQueue.Time;

		buyOS.side = BDT_Buy;
		buyOS.price = RealSZSEL2_OrderQueue.BuyPrice01;
		buyOS.order_items = RealSZSEL2_OrderQueue.BuyLevelQueueNo01;
		buyOS.qsize = std::min(ORDER_LEVEL_FIFTY, (int)buyOS.order_items); // 最大只有50档的数据

		buyOS.volumes[0] = RealSZSEL2_OrderQueue.BuyLevelQueue01;
		buyOS.volumes[1] = RealSZSEL2_OrderQueue.BuyLevelQueue02;
		buyOS.volumes[2] = RealSZSEL2_OrderQueue.BuyLevelQueue03;
		buyOS.volumes[3] = RealSZSEL2_OrderQueue.BuyLevelQueue04;
		buyOS.volumes[4] = RealSZSEL2_OrderQueue.BuyLevelQueue05;
		buyOS.volumes[5] = RealSZSEL2_OrderQueue.BuyLevelQueue06;
		buyOS.volumes[6] = RealSZSEL2_OrderQueue.BuyLevelQueue07;
		buyOS.volumes[7] = RealSZSEL2_OrderQueue.BuyLevelQueue08;
		buyOS.volumes[8] = RealSZSEL2_OrderQueue.BuyLevelQueue09;
		buyOS.volumes[9] = RealSZSEL2_OrderQueue.BuyLevelQueue10;
		buyOS.volumes[10] = RealSZSEL2_OrderQueue.BuyLevelQueue11;
		buyOS.volumes[11] = RealSZSEL2_OrderQueue.BuyLevelQueue12;
		buyOS.volumes[12] = RealSZSEL2_OrderQueue.BuyLevelQueue13;
		buyOS.volumes[13] = RealSZSEL2_OrderQueue.BuyLevelQueue14;
		buyOS.volumes[14] = RealSZSEL2_OrderQueue.BuyLevelQueue15;
		buyOS.volumes[15] = RealSZSEL2_OrderQueue.BuyLevelQueue16;
		buyOS.volumes[16] = RealSZSEL2_OrderQueue.BuyLevelQueue17;
		buyOS.volumes[17] = RealSZSEL2_OrderQueue.BuyLevelQueue18;
		buyOS.volumes[18] = RealSZSEL2_OrderQueue.BuyLevelQueue19;
		buyOS.volumes[19] = RealSZSEL2_OrderQueue.BuyLevelQueue20;
		buyOS.volumes[20] = RealSZSEL2_OrderQueue.BuyLevelQueue21;
		buyOS.volumes[21] = RealSZSEL2_OrderQueue.BuyLevelQueue22;
		buyOS.volumes[22] = RealSZSEL2_OrderQueue.BuyLevelQueue23;
		buyOS.volumes[23] = RealSZSEL2_OrderQueue.BuyLevelQueue24;
		buyOS.volumes[24] = RealSZSEL2_OrderQueue.BuyLevelQueue25;
		buyOS.volumes[25] = RealSZSEL2_OrderQueue.BuyLevelQueue26;
		buyOS.volumes[26] = RealSZSEL2_OrderQueue.BuyLevelQueue27;
		buyOS.volumes[27] = RealSZSEL2_OrderQueue.BuyLevelQueue28;
		buyOS.volumes[28] = RealSZSEL2_OrderQueue.BuyLevelQueue29;
		buyOS.volumes[29] = RealSZSEL2_OrderQueue.BuyLevelQueue30;
		buyOS.volumes[30] = RealSZSEL2_OrderQueue.BuyLevelQueue31;
		buyOS.volumes[31] = RealSZSEL2_OrderQueue.BuyLevelQueue32;
		buyOS.volumes[32] = RealSZSEL2_OrderQueue.BuyLevelQueue33;
		buyOS.volumes[33] = RealSZSEL2_OrderQueue.BuyLevelQueue34;
		buyOS.volumes[34] = RealSZSEL2_OrderQueue.BuyLevelQueue35;
		buyOS.volumes[35] = RealSZSEL2_OrderQueue.BuyLevelQueue36;
		buyOS.volumes[36] = RealSZSEL2_OrderQueue.BuyLevelQueue37;
		buyOS.volumes[37] = RealSZSEL2_OrderQueue.BuyLevelQueue38;
		buyOS.volumes[38] = RealSZSEL2_OrderQueue.BuyLevelQueue39;
		buyOS.volumes[39] = RealSZSEL2_OrderQueue.BuyLevelQueue40;
		buyOS.volumes[40] = RealSZSEL2_OrderQueue.BuyLevelQueue41;
		buyOS.volumes[41] = RealSZSEL2_OrderQueue.BuyLevelQueue42;
		buyOS.volumes[42] = RealSZSEL2_OrderQueue.BuyLevelQueue43;
		buyOS.volumes[43] = RealSZSEL2_OrderQueue.BuyLevelQueue44;
		buyOS.volumes[44] = RealSZSEL2_OrderQueue.BuyLevelQueue45;
		buyOS.volumes[45] = RealSZSEL2_OrderQueue.BuyLevelQueue46;
		buyOS.volumes[46] = RealSZSEL2_OrderQueue.BuyLevelQueue47;
		buyOS.volumes[47] = RealSZSEL2_OrderQueue.BuyLevelQueue48;
		buyOS.volumes[48] = RealSZSEL2_OrderQueue.BuyLevelQueue49;
		buyOS.volumes[49] = RealSZSEL2_OrderQueue.BuyLevelQueue50;

		if (m_parserSink)
			m_parserSink->handleOrderQueue(buyQue);

		buyQue->release();
	}

	if (RealSZSEL2_OrderQueue.SellLevelQueueNo01 != 0)
	{
		WTSOrdQueData* sellQue = WTSOrdQueData::create(code);
		sellQue->setContractInfo(ct);

		WTSOrdQueStruct& sellOS = sellQue->getOrdQueStruct();
		strcpy(sellOS.exchg, exchg);

		sellOS.trading_date = act_date;
		sellOS.action_date = act_date;
		sellOS.action_time = RealSZSEL2_OrderQueue.Time;

		sellOS.side = BDT_Sell;
		sellOS.price = RealSZSEL2_OrderQueue.SellPrice01;
		sellOS.order_items = RealSZSEL2_OrderQueue.SellLevelQueueNo01;
		sellOS.qsize = std::min(ORDER_LEVEL_FIFTY, (int)sellOS.order_items);

		sellOS.volumes[0] = RealSZSEL2_OrderQueue.SellLevelQueue01;
		sellOS.volumes[1] = RealSZSEL2_OrderQueue.SellLevelQueue02;
		sellOS.volumes[2] = RealSZSEL2_OrderQueue.SellLevelQueue03;
		sellOS.volumes[3] = RealSZSEL2_OrderQueue.SellLevelQueue04;
		sellOS.volumes[4] = RealSZSEL2_OrderQueue.SellLevelQueue05;
		sellOS.volumes[5] = RealSZSEL2_OrderQueue.SellLevelQueue06;
		sellOS.volumes[6] = RealSZSEL2_OrderQueue.SellLevelQueue07;
		sellOS.volumes[7] = RealSZSEL2_OrderQueue.SellLevelQueue08;
		sellOS.volumes[8] = RealSZSEL2_OrderQueue.SellLevelQueue09;
		sellOS.volumes[9] = RealSZSEL2_OrderQueue.SellLevelQueue10;
		sellOS.volumes[10] = RealSZSEL2_OrderQueue.SellLevelQueue11;
		sellOS.volumes[11] = RealSZSEL2_OrderQueue.SellLevelQueue12;
		sellOS.volumes[12] = RealSZSEL2_OrderQueue.SellLevelQueue13;
		sellOS.volumes[13] = RealSZSEL2_OrderQueue.SellLevelQueue14;
		sellOS.volumes[14] = RealSZSEL2_OrderQueue.SellLevelQueue15;
		sellOS.volumes[15] = RealSZSEL2_OrderQueue.SellLevelQueue16;
		sellOS.volumes[16] = RealSZSEL2_OrderQueue.SellLevelQueue17;
		sellOS.volumes[17] = RealSZSEL2_OrderQueue.SellLevelQueue18;
		sellOS.volumes[18] = RealSZSEL2_OrderQueue.SellLevelQueue19;
		sellOS.volumes[19] = RealSZSEL2_OrderQueue.SellLevelQueue20;
		sellOS.volumes[20] = RealSZSEL2_OrderQueue.SellLevelQueue21;
		sellOS.volumes[21] = RealSZSEL2_OrderQueue.SellLevelQueue22;
		sellOS.volumes[22] = RealSZSEL2_OrderQueue.SellLevelQueue23;
		sellOS.volumes[23] = RealSZSEL2_OrderQueue.SellLevelQueue24;
		sellOS.volumes[24] = RealSZSEL2_OrderQueue.SellLevelQueue25;
		sellOS.volumes[25] = RealSZSEL2_OrderQueue.SellLevelQueue26;
		sellOS.volumes[26] = RealSZSEL2_OrderQueue.SellLevelQueue27;
		sellOS.volumes[27] = RealSZSEL2_OrderQueue.SellLevelQueue28;
		sellOS.volumes[28] = RealSZSEL2_OrderQueue.SellLevelQueue29;
		sellOS.volumes[29] = RealSZSEL2_OrderQueue.SellLevelQueue30;
		sellOS.volumes[30] = RealSZSEL2_OrderQueue.SellLevelQueue31;
		sellOS.volumes[31] = RealSZSEL2_OrderQueue.SellLevelQueue32;
		sellOS.volumes[32] = RealSZSEL2_OrderQueue.SellLevelQueue33;
		sellOS.volumes[33] = RealSZSEL2_OrderQueue.SellLevelQueue34;
		sellOS.volumes[34] = RealSZSEL2_OrderQueue.SellLevelQueue35;
		sellOS.volumes[35] = RealSZSEL2_OrderQueue.SellLevelQueue36;
		sellOS.volumes[36] = RealSZSEL2_OrderQueue.SellLevelQueue37;
		sellOS.volumes[37] = RealSZSEL2_OrderQueue.SellLevelQueue38;
		sellOS.volumes[38] = RealSZSEL2_OrderQueue.SellLevelQueue39;
		sellOS.volumes[39] = RealSZSEL2_OrderQueue.SellLevelQueue40;
		sellOS.volumes[40] = RealSZSEL2_OrderQueue.SellLevelQueue41;
		sellOS.volumes[41] = RealSZSEL2_OrderQueue.SellLevelQueue42;
		sellOS.volumes[42] = RealSZSEL2_OrderQueue.SellLevelQueue43;
		sellOS.volumes[43] = RealSZSEL2_OrderQueue.SellLevelQueue44;
		sellOS.volumes[44] = RealSZSEL2_OrderQueue.SellLevelQueue45;
		sellOS.volumes[45] = RealSZSEL2_OrderQueue.SellLevelQueue46;
		sellOS.volumes[46] = RealSZSEL2_OrderQueue.SellLevelQueue47;
		sellOS.volumes[47] = RealSZSEL2_OrderQueue.SellLevelQueue48;
		sellOS.volumes[48] = RealSZSEL2_OrderQueue.SellLevelQueue49;
		sellOS.volumes[49] = RealSZSEL2_OrderQueue.SellLevelQueue50;

		if (m_parserSink)
			m_parserSink->handleOrderQueue(sellQue);

		sellQue->release();
	}
}

void ParserQTS::OnSubscribe_SZSEL2_Tick(const SZSEL2_Tick& data)
{
	const char* code = data.Symbol;
	const char* exchg = "SZSE";
	auto ct = m_pBaseDataMgr->getContract(code, exchg);
	if (ct == NULL)
		return;

	uint32_t act_date = (uint32_t)(data.Time / 1000000000);  // YYYYMMDDHHMMSSMMM
	uint32_t act_time = (uint32_t)(data.Time % 1000000000);

	if (data.TickType == '4' || data.TickType == 'F')
	{
		WTSTransData* trans = WTSTransData::create(code);
		WTSTransStruct& ts = trans->getTransStruct();
		strcpy(ts.exchg, exchg);

		ts.trading_date = act_date;
		ts.action_date = act_date;
		ts.action_time = act_time;

		ts.price = checkValid(data.TickPrice);
		ts.volume = data.TickVolume;
		ts.askorder = (uint32_t)data.SellOrderID;
		ts.bidorder = (uint32_t)data.BuyOrderID;
		ts.index = (uint32_t)data.RecID;
		ts.side = wrapOrderCode(data.OrderCode);
		ts.ttype = data.TickType == 'F' ? TT_Match : TT_Cancel;

		if (m_parserSink)
			m_parserSink->handleTransaction(trans);

		trans->release();
	}
	else
	{
		WTSOrdDtlData* order = WTSOrdDtlData::create(code);
		WTSOrdDtlStruct& ods = order->getOrdDtlStruct();
		strcpy(ods.exchg, exchg);

		ods.action_date = act_date;
		ods.trading_date = act_date;
		ods.action_time = act_time;

		ods.side = wrapOrderCode(data.OrderCode);
		ods.price = checkValid(data.TickPrice);
		ods.volume = data.TickVolume;
		ods.index = (uint32_t)data.RecID;
		ods.otype = wrapOrderType(data.TickType); 

		if (m_parserSink)
			m_parserSink->handleOrderDetail(order);

		order->release();
	}
}
