/*!
 * \file ParserTDF.cpp
 * \project	WonderTrader
 *
 * \author Wesley
 * \date 2020/03/30
 *
 * \brief
 */
#include "ParserTDF.h"
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

#define GETRECORD(pBase, TYPE, nIndex) ((TYPE*)((char*)(pBase) + sizeof(TYPE)*(nIndex)))

#ifndef FLT_MAX
#define FLT_MAX 3.402823466e+38F
#endif

#ifdef _WIN32
#ifdef _WIN64
#pragma comment(lib, "../API/tdf_20220831/x64/TDFAPI30.lib")
#else
#pragma comment(lib, "../API/tdf_20220831/x86/TDFAPI30.lib")
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
		ParserTDF* parser = new ParserTDF();
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

/*!
* TDF 价格公式如下
* 合约价格 = （原始价格 + 0.00005） * 10000
* 合约成交额也是放大了10000倍后得到的结果
* 指数成交额单位是100元
*/
inline double checkValid(__int64 val)
{
	double price = (double)val / 10000;
	//price -= 0.00005;

	if (price == DBL_MAX)
		return 0;

	return price;
}

inline bool isNull(const void* p)
{
	return (p == NULL || p == nullptr);
}

inline const char* toStdExchg(const char* wdExchg)
{
	if (strcmp(wdExchg, "SH") == 0)
		return "SSE";
	else if (strcmp(wdExchg, "SZ") == 0)
		return "SZSE";
	else if (strcmp(wdExchg, "CF") == 0)
		return "CFFEX";
	else if (strcmp(wdExchg, "SHF") == 0)
		return "SHFE";
	else if (strcmp(wdExchg, "CZC") == 0)
		return "CZCE";
	else if (strcmp(wdExchg, "DCE") == 0)
		return "DCE";
	else
		return "";
}

inline const char* toTDFExchg(const char* stdExchg)
{
	if (strcmp(stdExchg, "SSE") == 0)
		return "SH";
	else if (strcmp(stdExchg, "SZSE") == 0)
		return "SZ";
	else if (strcmp(stdExchg, "CFFEX") == 0)
		return "CF";
	else if (strcmp(stdExchg, "SHFE") == 0)
		return "SHF";
	else if (strcmp(stdExchg, "CZCE") == 0)
		return "CZC";
	else if (strcmp(stdExchg, "DCE") == 0)
		return "DCE";
	else
		return "";
}

const char* GetErrStr(TDF_ERR nErr)
{
	std::map<TDF_ERR, const char*> mapErrStr;
	mapErrStr.insert(std::make_pair(TDF_ERR_UNKOWN, "TDF_ERR_UNKOWN"));
	mapErrStr.insert(std::make_pair(TDF_ERR_INITIALIZE_FAILURE, "TDF_ERR_INITIALIZE_FAILURE"));
	mapErrStr.insert(std::make_pair(TDF_ERR_NETWORK_ERROR, "TDF_ERR_NETWORK_ERROR"));
	mapErrStr.insert(std::make_pair(TDF_ERR_INVALID_PARAMS, "TDF_ERR_INVALID_PARAMS"));
	mapErrStr.insert(std::make_pair(TDF_ERR_VERIFY_FAILURE, "TDF_ERR_VERIFY_FAILURE"));
	mapErrStr.insert(std::make_pair(TDF_ERR_NO_AUTHORIZED_MARKET, "TDF_ERR_NO_AUTHORIZED_MARKET"));
	mapErrStr.insert(std::make_pair(TDF_ERR_NO_CODE_TABLE, "TDF_ERR_NO_CODE_TABLE"));
	mapErrStr.insert(std::make_pair(TDF_ERR_SUCCESS, "TDF_ERR_SUCCESS"));
	if (mapErrStr.find(nErr) == mapErrStr.end())
	{
		return "TDF_ERR_UNKOWN";
	}
	else
	{
		return mapErrStr[nErr];
	}
}

IParserSpi*		ParserTDF::m_parserSink = NULL;
IBaseDataMgr*	ParserTDF::m_pBaseDataMgr = NULL;

ParserTDF::ParserTDF()
	: m_pUserAPI(NULL)
	, m_bLogined(false)
	, m_bProxy(false)
	, m_bHasIndex(false)
	, m_bHasShare(false)
	, m_bHasFund(false)
	, m_bHasBond(false)
	, m_bHasFuture(false)
	, m_bHasOption(false)
	, m_bHasOrder(false)
	, m_bHasTrans(false)
	, m_bHasOrdQue(false)
{

}


ParserTDF::~ParserTDF()
{
	if (m_pUserAPI)
		TDF_Close(m_pUserAPI);
}

bool ParserTDF::init(WTSVariant* config)
{
	m_OpenSetting = { 0 };
	memset(&m_OpenSetting, 0, sizeof(TDF_OPEN_SETTING_EXT));

	WTSVariant* cfgSvrs = config->get("servers");
	if (cfgSvrs)
	{
		for (uint32_t i = 0; i < min(cfgSvrs->size(), (uint32_t)MAXSERVER); i++)
		{
			WTSVariant* cfgItem = cfgSvrs->get(i);

			strncpy(m_OpenSetting.siServer[i].szIp, cfgItem->getCString("host"), sizeof(m_OpenSetting.siServer[i].szIp) - 1);
			_snprintf(m_OpenSetting.siServer[i].szPort, sizeof(m_OpenSetting.siServer[i].szPort) - 1, "%d", cfgItem->getInt32("port"));
			strncpy(m_OpenSetting.siServer[i].szUser, cfgItem->getCString("user"), sizeof(m_OpenSetting.siServer[i].szUser) - 1);
			strncpy(m_OpenSetting.siServer[i].szPwd, cfgItem->getCString("pass"), sizeof(m_OpenSetting.siServer[i].szPwd) - 1);
		}

		m_OpenSetting.nServerNum = min(cfgSvrs->size(), (uint32_t)MAXSERVER);
	}

	m_intTime = (int8_t)config->getInt32("n_time");
	m_intBroadcast = (int8_t)config->getInt32("distribution");

	m_bHasOrder = config->getBoolean("has_order");
	m_bHasTrans = config->getBoolean("has_trans");
	m_bHasOrdQue = config->getBoolean("has_orderqueue");

	m_bHasIndex = config->getBoolean("has_index");
	m_bHasShare = config->getBoolean("has_share");
	m_bHasFund = config->getBoolean("has_fund");
	m_bHasBond = config->getBoolean("has_bond");
	m_bHasFuture = config->getBoolean("has_future");
	m_bHasOption = config->getBoolean("has option");

	m_strSubMarkets = config->getCString("sub_markets");

	m_bProxy = config->getBoolean("enable_proxy");

	m_strLogdir = "log";  // 必须指定为log目录，否则API日志到不到目录会报错

	{
		std::string path = StrUtil::standardisePath(m_strLogdir);
		if (!StdFile::exists(path.c_str()))
			boost::filesystem::create_directories(path.c_str());
	}

	std::string module = config->getCString("tdfmodule");
	if (module.empty())
	{
		module = "TDFAPI30";
	}

	std::string dllpath;
#ifdef _WIN32
	dllpath = getBinDir() + DLLHelper::wrap_module(module.c_str(), "dll");
#else
	dllpath = getBinDir() + DLLHelper::wrap_module(module.c_str(), "");
#endif

	m_hInstTDF = DLLHelper::load_library(dllpath.c_str());

	//log路径，未设置则默认保存到当前路径的log文件夹下。log文件夹不存在不会创建
	//TDF_SetLogPath("D:"); //只可设置一次，路径设置需要在所有函数之前
	//0、版本号
	write_log(m_parserSink, LL_INFO, "TDF Version: {}", TDF_Version());

	//1、环境变量设置,可不设置，直接使用默认值
	TDF_SetEnv(TDF_ENVIRON_HEART_BEAT_INTERVAL, config->getUInt32("heartbeat_gap"));
	TDF_SetEnv(TDF_ENVIRON_MISSED_BEART_COUNT, config->getUInt32("miss_count"));
	TDF_SetEnv(TDF_ENVIRON_OPEN_TIME_OUT, config->getUInt32("timeout"));

	//心跳方式设置(0: 取消发送心跳包 1：没有数据发送的时候发送心跳数据，2：有规律发送心跳数据)，默认为1
	//TDF_SetEnv(TDF_ENVIRON_HEART_BEAT_FLAG, 2);

	//log文件创建方式，默认创建到程序运行目录下的log文件夹下，设置为在当前运行路径下直接创建
	TDF_SetEnv(TDF_ENVIRON_OUT_LOG, 1);//1、当前目录下创建log，否则在当前路径有log文件夹时创建到log文件夹中

	//自定义日志文件输出目录
	TDF_SetLogPath(m_strLogdir.c_str());

	//断点续传模式
	//TDF_SetEnv(TDF_ENVIRON_BREAKPOINT, 0);

	SubscribeMDs(false);

	return true;
}

void ParserTDF::release()
{
	disconnect();
}

bool ParserTDF::connect()
{
	DoLogin();

	return true;
}

bool ParserTDF::disconnect()
{
	if (m_pUserAPI)
	{
		TDF_Close(m_pUserAPI);
	}

	return true;
}

typedef int (ParserTDF::*RecvData)(THANDLE hTdf, TDF_MSG* pMsgHead);

void ParserTDF::DoLogin()
{
	// Open
	m_OpenSetting.pfnMsgHandler = &ParserTDF::RecvData;
	//open_settings_ext.pfnMsgHandler = DoRecvData;
	m_OpenSetting.pfnSysMsgNotify = &ParserTDF::RecvSys;
	//open_settings_ext.pfnSysMsgNotify = DoRecvSys;
	//open_settings_ext.nServerNum = 1;

	m_OpenSetting.szMarkets = m_strSubMarkets.c_str(); //"SZ-2-0;SH-1-1;"; 需要订阅的市场列表，以“;”分割
	m_OpenSetting.szResvMarkets = "";  // 市场订阅(缺)！例如"SZ-2-0;SH-1-1;"，需要订阅的市场列表，以“;”分割,建议为""
	m_OpenSetting.szSubScriptions = m_strCodes.c_str();  //"";   // 代码订阅,例如"600000.sh;ag.shf;000001.sz"，需要订阅的股票(单个股票格式为原始Code+.+市场，如999999.SH)，以“;”分割，为空则订阅全市场

	m_OpenSetting.nTime = m_intTime;

	unsigned int nTypes = 0; // 0=实时行情
	if (m_bHasOrder) nTypes |= DATA_TYPE_ORDER;
	if (m_bHasOrdQue) nTypes |= DATA_TYPE_ORDERQUEUE;
	if (m_bHasTrans) nTypes |= DATA_TYPE_TRANSACTION;

	m_OpenSetting.nTypeFlags = nTypes;

	/// 订阅合约类型
	unsigned int nContractFlag = 0;
	if (m_bHasIndex) nContractFlag |= CODE_TYPE_INDEX;
	if (m_bHasShare) nContractFlag |= CODE_TYPE_SHARES;
	if (m_bHasFund) nContractFlag |= CODE_TYPE_FUND;
	if (m_bHasBond) nContractFlag |= CODE_TYPE_BOND;
	if (m_bHasFuture) nContractFlag |= CODE_TYPE_FUTURES;
	if (m_bHasOption) nContractFlag |= CODE_TYPE_OPTIONS;

	m_OpenSetting.nCodeTypeFlags = (nContractFlag == 0) ? CODE_TYPE_ALL : nContractFlag;  ////代码类型订阅！支持订阅CODE_TYPE_FLAG表示的类型，多种类型使用'|'进行计算后填入,与nTypeFlags计算方式相同

	TDF_PROXY_SETTING proxy_settings = { (TDF_PROXY_TYPE)0 };

	TDF_ERR nErr = TDF_ERR_SUCCESS;

	if (m_bProxy)
	{
		memset(&proxy_settings, 0, sizeof(proxy_settings));
		proxy_settings.nProxyType = (TDF_PROXY_TYPE)m_untProxyType;
		strncpy(proxy_settings.szProxyHostIp, m_strProxyAddr.c_str(), sizeof(proxy_settings.szProxyHostIp) - 1);
		_snprintf(proxy_settings.szProxyPort, sizeof(proxy_settings.szProxyPort) - 1, "%d", m_intProxyPort);
		strncpy(proxy_settings.szProxyUser, m_strProxyUser.c_str(), sizeof(proxy_settings.szProxyUser) - 1);
		strncpy(proxy_settings.szProxyPwd, m_strProxyAddr.c_str(), sizeof(proxy_settings.szProxyPwd) - 1);

		m_pUserAPI = TDF_OpenProxyExt(&m_OpenSetting, &proxy_settings, &nErr);
	}
	else
	{
		m_pUserAPI = TDF_OpenExt(&m_OpenSetting, &nErr);
	}

	while (nErr == TDF_ERR_NETWORK_ERROR)
	{
		write_log(m_parserSink, LL_WARN, "[ParserTDF] TDF Open fail, err code: {}, reconnect after 3 sec", nErr);

		std::this_thread::sleep_for(std::chrono::seconds(3));

		if (m_bProxy)
		{
			m_pUserAPI = TDF_OpenProxyExt(&m_OpenSetting, &proxy_settings, &nErr);
		}
		else
		{
			m_pUserAPI = TDF_OpenExt(&m_OpenSetting, &nErr);
		}
	}

	write_log(m_parserSink, LL_INFO, "TDF_Open returned: {}", GetErrStr(nErr));

	///  Open 后续处理
	if (isNull(m_pUserAPI))
	{
		if (m_parserSink)
		{
			m_parserSink->handleEvent(WPE_Connect, -1);

			write_log(m_parserSink, LL_ERROR, "[ParserTDF] TDF Open Failed!");
			exit(-1);
		}
	}
	else
	{
		write_log(m_parserSink, LL_INFO, "[ParserTDF] TDF Open Success!");

		m_bLogined = true;
		if (m_parserSink)
		{
			m_parserSink->handleEvent(WPE_Connect, 0);
			m_parserSink->handleEvent(WPE_Login, 0);
		}
	}
}

void ParserTDF::SubscribeMDs(bool isAll /* = false */)
{
	CodeSet codes;

	CodeSet codeFilter = m_filterSubs;
	if (codeFilter.empty())
	{//如果订阅礼包只空的,则取出全部合约列表
		return;
	}

	if (!isAll)
	{
		CodeSet codeFilter = m_filterSubs;
		if (m_filterSubs.empty())
		{//如果订阅空的,则取出全部合约列表
			TDF_SetSubscription(m_pUserAPI, "", SUBSCRIPTION_FULL);
			return;
		}

		m_strCodes.clear();
		std::string code, exchg;
		for (const auto& fullCode : m_filterSubs)
		{
			auto ay = StrUtil::split(fullCode.c_str(), ".");
			if (ay.size() < 2)
				continue;

			code = ay[1];
			exchg = ay[0];

			if (!m_strCodes.empty())
				m_strCodes += ";";

			m_strCodes += code;
			m_strCodes += ".";
			m_strCodes += toTDFExchg(exchg.c_str());

			//codes.push_back(s);
			//TDF_SetSubscription(m_pUserAPI, s.c_str(), SUBSCRIPTION_ADD);
		}

		TDF_SetSubscription(m_pUserAPI, m_strCodes.c_str(), SUBSCRIPTION_SET);
	}
	else
	{
		m_strCodes.clear();
		m_strCodes = "";
		TDF_SetSubscription(m_pUserAPI, "", SUBSCRIPTION_FULL);
	}

	write_log(m_parserSink, LL_DEBUG, "[ParserTDF] Subscribe contracts ...");
}

void ParserTDF::subscribe(const CodeSet &vecSymbols)
{
	if (m_uTradingDate == 0)
	{
		m_filterSubs = vecSymbols;
	}
	else
	{
		m_filterSubs = vecSymbols;
		SubscribeMDs();
	}
}

void ParserTDF::unsubscribe(const CodeSet &vecSymbols)
{
	std::string code, exchg;
	for (const auto& fullCode : vecSymbols)
	{
		auto ay = StrUtil::split(fullCode.c_str(), ".");
		if (ay.size() < 2)
			continue;

		code = ay[1];
		exchg = ay[0];

		code += ".";
		code += toTDFExchg(exchg.c_str());

		TDF_SetSubscription(m_pUserAPI, code.c_str(), SUBSCRIPTION_DEL);
	}

	write_log(m_parserSink, LL_DEBUG, "[ParserTDF] Unsubscribe contracts!");
}

bool ParserTDF::isConnected()
{
	return m_pUserAPI != NULL;
}

void ParserTDF::registerSpi(IParserSpi* listener)
{
	m_parserSink = listener;

	if (m_parserSink)
		m_pBaseDataMgr = m_parserSink->getBaseDataMgr();
}

void ParserTDF::RecvData(THANDLE hTdf, TDF_MSG* pMsgHead)
{
	if (!pMsgHead->pData)
	{
		//assert(0);
		return;
	}

	unsigned int nItemCount = pMsgHead->pAppHead->nItemCount;
	unsigned int nItemSize = pMsgHead->pAppHead->nItemSize;

	if (!nItemCount)
		return;

	switch (pMsgHead->nDataType)
	{
	case MSG_DATA_MARKET:  // 行情数据，股票
	{
		for (uint32_t i = 0; i < nItemCount; ++i)
		{
			TDF_MARKET_DATA* pLastMarket = GETRECORD(pMsgHead->pData, TDF_MARKET_DATA, i);

			write_log(m_parserSink, LL_DEBUG, "[RecvData] Wind Code: {}, Code: {}, Time: {}, Last Price: {}, TotalVolume: {}, Type: {}", pLastMarket->szWindCode, pLastMarket->szCode, pLastMarket->nTime, pLastMarket->nMatch, pLastMarket->iVolume, pLastMarket->pCodeInfo->nType);

			const char* code = pLastMarket->szCode;
			const char* exchg = toStdExchg(pLastMarket->szWindCode + strlen(pLastMarket->szCode) + 1);

			WTSContractInfo* ct = m_pBaseDataMgr->getContract(code, exchg);
			if (ct == NULL)
			{
				if (m_parserSink)
					write_log(m_parserSink, LL_ERROR, "[ParserTDF] Instrument {}.{} not exists...", exchg, code);
				continue;
			}

			WTSCommodityInfo* commInfo = ct->getCommInfo();

			WTSTickData* tick = WTSTickData::create(code);
			tick->setContractInfo(ct);
			WTSTickStruct& quote = tick->getTickStruct();
			strcpy(quote.exchg, exchg);

			quote.action_date = (uint32_t)pLastMarket->nActionDay;
			quote.action_time = (uint32_t)pLastMarket->nTime;
			quote.trading_date = (uint32_t)pLastMarket->nTradingDay;

			quote.price = checkValid(pLastMarket->nMatch);
			quote.open = checkValid(pLastMarket->nOpen);
			quote.high = checkValid(pLastMarket->nHigh);
			quote.low = checkValid(pLastMarket->nLow);
			quote.total_volume = (uint32_t)pLastMarket->iVolume;
			quote.total_turnover = checkValid(pLastMarket->iTurnover);  // 成交金额需要处理

			quote.upper_limit = checkValid(pLastMarket->nHighLimited);
			quote.lower_limit = checkValid(pLastMarket->nLowLimited);

			quote.pre_close = checkValid(pLastMarket->nPreClose);

			//委卖价格
			for (int i = 0; i < 10; i++)
			{
				quote.ask_prices[i] = checkValid(pLastMarket->nAskPrice[i]);
				quote.ask_qty[i] = (uint32_t)pLastMarket->nAskVol[i];

				quote.bid_prices[i] = checkValid(pLastMarket->nBidPrice[i]);
				quote.bid_qty[i] = (uint32_t)pLastMarket->nBidVol[i];
			}

			if (m_parserSink)
				m_parserSink->handleQuote(tick, 1);

			//处理逐笔
			if (sizeof(pLastMarket->nBidOrders) / sizeof(int) > 0)
			{
				WTSOrdQueData* buyQue = WTSOrdQueData::create(code);
				buyQue->setContractInfo(ct);

				WTSOrdQueStruct& buyOS = buyQue->getOrdQueStruct();
				strcpy(buyOS.exchg, exchg);

				buyOS.trading_date = (uint32_t)pLastMarket->nTradingDay;
				buyOS.action_date = (uint32_t)pLastMarket->nActionDay;
				buyOS.action_time = (uint32_t)pLastMarket->nTime;

				buyOS.side = BDT_Buy;
				buyOS.price = quote.bid_prices[0];
				buyOS.order_items = 10;
				buyOS.qsize = std::min((int)buyOS.order_items, 50); // 最大只有50档的数据

				for (uint32_t i = 0; i < 10; i++)
				{
					buyOS.volumes[i] = (uint32_t)pLastMarket->nBidOrders[i];
				}

				if (m_parserSink)
					m_parserSink->handleOrderQueue(buyQue);

				buyQue->release();
			}

			if (sizeof(pLastMarket->nAskOrders) / sizeof(int) > 0)
			{
				WTSOrdQueData* sellQue = WTSOrdQueData::create(code);
				sellQue->setContractInfo(ct);

				WTSOrdQueStruct& sellOS = sellQue->getOrdQueStruct();
				strcpy(sellOS.exchg, exchg);

				sellOS.trading_date = (uint32_t)pLastMarket->nTradingDay;
				sellOS.action_date = (uint32_t)pLastMarket->nActionDay;
				sellOS.action_time = (uint32_t)pLastMarket->nTime;

				sellOS.side = BDT_Sell;
				sellOS.price = quote.ask_prices[0];
				sellOS.order_items = 10;
				sellOS.qsize = std::min((int)sellOS.order_items, 50);

				for (uint32_t i = 0; i < 10; i++)
				{
					sellOS.volumes[i] = (uint32_t)pLastMarket->nAskOrders[i];
				}

				if (m_parserSink)
					m_parserSink->handleOrderQueue(sellQue);

				sellQue->release();
			}

			tick->release();
		}
	}
	break;
	case MSG_DATA_FUTURE:  // 期货期权行情数据
	{
		for (uint32_t i = 0; i < nItemCount; ++i)
		{
			TDF_FUTURE_DATA* pLastMarket = GETRECORD(pMsgHead->pData, TDF_FUTURE_DATA, i);

			write_log(m_parserSink, LL_DEBUG, "[RecvData] Wind Code: {}, Code: {}, Time: {}, Last Price: {}, TotalVolume: {}, Type: {}", pLastMarket->szWindCode, pLastMarket->szCode, pLastMarket->nTime, pLastMarket->nMatch, pLastMarket->iVolume, pLastMarket->pCodeInfo->nType);

			const char* code = pLastMarket->szCode;
			const char* exchg = toStdExchg(pLastMarket->szWindCode + strlen(pLastMarket->szCode) + 1);

			WTSContractInfo* ct = m_pBaseDataMgr->getContract(code, exchg);
			if (ct == NULL)
			{
				if (m_parserSink)
					write_log(m_parserSink, LL_ERROR, "[ParserTDF] Instrument {}.{} not exists...", exchg, code);
				continue;
			}

			WTSCommodityInfo* commInfo = ct->getCommInfo();

			WTSTickData* tick = WTSTickData::create(code);
			tick->setContractInfo(ct);
			WTSTickStruct& quote = tick->getTickStruct();
			strcpy(quote.exchg, exchg);

			quote.action_date = (uint32_t)pLastMarket->nActionDay;
			quote.action_time = (uint32_t)pLastMarket->nTime;
			quote.trading_date = (uint32_t)pLastMarket->nTradingDay;

			quote.price = checkValid(pLastMarket->nMatch);
			quote.open = checkValid(pLastMarket->nOpen);
			quote.high = checkValid(pLastMarket->nHigh);
			quote.low = checkValid(pLastMarket->nLow);
			quote.total_volume = (uint32_t)pLastMarket->iVolume;
			quote.total_turnover = checkValid(pLastMarket->iTurnover);  // 成交金额需要处理
			quote.open_interest = (uint32_t)pLastMarket->iOpenInterest;

			if (pLastMarket->nSettlePrice != 0)
			{
				quote.settle_price = checkValid(pLastMarket->nSettlePrice);
			}

			quote.upper_limit = checkValid(pLastMarket->nHighLimited);
			quote.lower_limit = checkValid(pLastMarket->nLowLimited);

			quote.pre_close = checkValid(pLastMarket->nPreClose);

			//委卖价格
			for (int i = 0; i < 5; i++)
			{
				quote.ask_prices[i] = checkValid(pLastMarket->nAskPrice[i]);
				quote.ask_qty[i] = (uint32_t)pLastMarket->nAskVol[i];

				quote.bid_prices[i] = checkValid(pLastMarket->nBidPrice[i]);
				quote.bid_qty[i] = (uint32_t)pLastMarket->nBidVol[i];
			}

			if (m_parserSink)
				m_parserSink->handleQuote(tick, 1);

			tick->release();
		}
	}
	break;
	case MSG_DATA_INDEX:  // 指数行情数据
	{
		for (uint32_t i = 0; i < nItemCount; i++)
		{
			TDF_INDEX_DATA* pLastIndex = GETRECORD(pMsgHead->pData, TDF_INDEX_DATA, i);
			const char* code = pLastIndex->szCode;
			const char* exchg = toStdExchg(pLastIndex->szWindCode + strlen(pLastIndex->szCode) + 1);
			auto ct = m_pBaseDataMgr->getContract(code, exchg);
			if (ct == NULL)
			{
				if (m_parserSink)
					write_log(m_parserSink, LL_ERROR, "[ParserTDF] Instrument {}.{} not exists...", exchg, code);
				continue;
			}

			WTSTickData* tick = WTSTickData::create(code);
			WTSTickStruct& quote = tick->getTickStruct();
			strcpy(quote.exchg, exchg);

			quote.action_date = pLastIndex->nActionDay;
			quote.trading_date = pLastIndex->nTradingDay;
			quote.action_time = pLastIndex->nTime;

			quote.price = checkValid(pLastIndex->nLastIndex);
			quote.open = checkValid(pLastIndex->nOpenIndex);
			quote.high = checkValid(pLastIndex->nHighIndex);
			quote.low = checkValid(pLastIndex->nLowIndex);
			quote.pre_close = checkValid(pLastIndex->nPreCloseIndex);

			quote.total_volume = (uint32_t)pLastIndex->iTotalVolume; //
			quote.total_turnover = checkValid(pLastIndex->iTurnover) * 100.0;  //指数成交额单位是百元

			if (m_parserSink)
				m_parserSink->handleQuote(tick, true);

			tick->release();
		}
	}
	break;
	case MSG_DATA_TRANSACTION:
	{
		for (uint32_t i = 0; i < nItemCount; i++)
		{
			TDF_TRANSACTION* pLastTrans = GETRECORD(pMsgHead->pData, TDF_TRANSACTION, i);
			const char* code = pLastTrans->szCode;
			const char* exchg = toStdExchg(pLastTrans->szWindCode + strlen(pLastTrans->szCode) + 1);
			auto ct = m_pBaseDataMgr->getContract(code, exchg);
			if (ct == NULL)
				continue;

			WTSTransData* trans = WTSTransData::create(code);
			WTSTransStruct& ts = trans->getTransStruct();
			strcpy(ts.exchg, exchg);

			ts.trading_date = pLastTrans->nActionDay;
			ts.action_date = pLastTrans->nActionDay;
			ts.action_time = pLastTrans->nTime;
			ts.price = checkValid(pLastTrans->nPrice);
			ts.volume = pLastTrans->nVolume;
			ts.askorder = (uint32_t)pLastTrans->nAskOrder;
			ts.bidorder = (uint32_t)pLastTrans->nBidOrder;
			ts.index = (uint32_t)pLastTrans->nIndex;
			ts.side = (WTSBSDirectType)pLastTrans->nBSFlag;
			ts.ttype = pLastTrans->chOrderKind == 'F' ? TT_Match : TT_Cancel;  // 4=cancel, F=trade

			if (ts.side == BDT_Unknown)
			{
				if (ts.askorder > ts.bidorder)
					ts.side = BDT_Sell;
				else
					ts.side = BDT_Buy;
			}

			if (m_parserSink)
				m_parserSink->handleTransaction(trans);

			trans->release();
		}
	}
	break;
	case MSG_DATA_ORDERQUEUE:
	{
		for (uint32_t i = 0; i < nItemCount; i++)
		{
			TDF_ORDER_QUEUE* pLastOrdQue = GETRECORD(pMsgHead->pData, TDF_ORDER_QUEUE, i);

			const char* code = pLastOrdQue->szCode;
			const char* exchg = toStdExchg(pLastOrdQue->szWindCode + strlen(pLastOrdQue->szCode) + 1);
			auto ct = m_pBaseDataMgr->getContract(code, exchg);
			if (ct == NULL)
				continue;

			WTSOrdQueData* pData = WTSOrdQueData::create(code);
			WTSOrdQueStruct& oqs = pData->getOrdQueStruct();
			strcpy(oqs.exchg, exchg);

			oqs.trading_date = pLastOrdQue->nActionDay;
			oqs.action_date = pLastOrdQue->nActionDay;
			oqs.action_time = pLastOrdQue->nTime;
			oqs.side = (WTSBSDirectType)pLastOrdQue->nSide;
			oqs.order_items = pLastOrdQue->nOrders;
			oqs.price = checkValid(pLastOrdQue->nPrice);
			oqs.qsize = pLastOrdQue->nABItems;
			//最多复制50个
			uint32_t maxCnt = oqs.qsize;
			if (maxCnt > 50)
				maxCnt = 50;
			memcpy(oqs.volumes, (uint32_t*)pLastOrdQue->nABVolume, sizeof(uint32_t)*maxCnt);

			if (m_parserSink)
				m_parserSink->handleOrderQueue(pData);

			pData->release();
		}
	}
	break;
	case MSG_DATA_ORDER:
	{
		for (uint32_t i = 0; i < nItemCount; i++)
		{
			TDF_ORDER* pLastOrder = GETRECORD(pMsgHead->pData, TDF_ORDER, i);
			const char* code = pLastOrder->szCode;
			const char* exchg = toStdExchg(pLastOrder->szWindCode + strlen(pLastOrder->szCode) + 1);
			auto ct = m_pBaseDataMgr->getContract(code, exchg);
			if (ct == NULL)
				continue;

			WTSOrdDtlData* order = WTSOrdDtlData::create(code);
			WTSOrdDtlStruct& ods = order->getOrdDtlStruct();
			strcpy(ods.exchg, exchg);
			ods.action_date = pLastOrder->nActionDay;
			ods.trading_date = pLastOrder->nActionDay;
			ods.action_time = pLastOrder->nTime;
			ods.side = (WTSBSDirectType)pLastOrder->chFunctionCode;
			ods.price = checkValid(pLastOrder->nPrice);
			ods.volume = pLastOrder->nVolume;
			ods.index = (uint32_t)pLastOrder->nOrder;
			ods.otype = (WTSOrdDetailType)pLastOrder->chOrderKind;

			if (m_parserSink)
				m_parserSink->handleOrderDetail(order);

			order->release();
		}
	}
	break;
	case MSG_DATA_BBQTRANSACTION:
	{

	}
	break;
	case MSG_DATA_BBQBID:
	{

	}
	break;
	case MSG_DATA_NON_MD:
	{

	}
	break;
	case MSG_DATA_BROKERQUEUE:
	{

	}
	case MSG_DATA_NB_ORDER:
	{

	}
	break;
	break;
	default:
	{

	}
	break;
	}
}

void ParserTDF::RecvSys(THANDLE hTdf, TDF_MSG* pSysMsg)
{
	if (!hTdf || !pSysMsg)
		return;

	switch (pSysMsg->nDataType)
	{
	case MSG_SYS_PACK_OVER:
	{
		TDF_PACK_OVER* pPackOver = (TDF_PACK_OVER*)pSysMsg->pData;
		write_log(m_parserSink, LL_DEBUG, "[ParserTDF] TDF_PACK_OVER: num: {},conID: {}", pPackOver->nDataNumber, pPackOver->nConID);
	}
	break;
	case MSG_SYS_DISCONNECT_NETWORK:
	{
		write_log(m_parserSink, LL_WARN, "[ParserTDF] DISCONNECT NETWORK");
	}
	break;
	case MSG_SYS_CONNECT_RESULT:
	{
		TDF_CONNECT_RESULT* pConnResult = (TDF_CONNECT_RESULT*)pSysMsg->pData;
		if (pConnResult && pConnResult->nConnResult)
		{
			write_log(m_parserSink, LL_INFO, "[ParserTDF] [{}] Connect {}:{} success!", pConnResult->szUser, pConnResult->szIp, pConnResult->szPort);
		}
		else
		{
			write_log(m_parserSink, LL_ERROR, "[ParserTDF] [{}] Connect {}:{} fail!", pConnResult->szUser, pConnResult->szIp, pConnResult->szPort);
		}
	}
	break;
	case MSG_SYS_LOGIN_RESULT:
	{
		TDF_LOGIN_RESULT* pLoginResult = (TDF_LOGIN_RESULT*)pSysMsg->pData;
		if (pLoginResult && pLoginResult->nLoginResult)
		{
			std::stringstream ss;
			for (int i = 0; i < pLoginResult->nMarkets; i++)
			{
				ss << "market: " << pLoginResult->szMarket[i] << ", dyn_date: " << pLoginResult->nDynDate[i] << (i >= pLoginResult->nMarkets - 1) ? "" : "\n";
			}
			write_log(m_parserSink, LL_INFO, "[ParserTDF] Login success! info: {}, nMarkets: {}, market list: {}", pLoginResult->szInfo, pLoginResult->nMarkets, ss.str());
		}
		else
		{
			write_log(m_parserSink, LL_ERROR, "[ParserTDF] Login failed. info: {}", pLoginResult->szInfo);
		}
	}
	break;
	case MSG_SYS_CODETABLE_RESULT://在此可获取代码表
	{
		TDF_CODE_RESULT* pCodeResult = (TDF_CODE_RESULT*)pSysMsg->pData;
		if (pCodeResult)
		{
			write_log(m_parserSink, LL_DEBUG, "[ParserTDF] Receive code table. info:{}, markets: {}", pCodeResult->szInfo, pCodeResult->nMarkets);
			for (int i = 0; i < pCodeResult->nMarkets; i++)
			{
				write_log(m_parserSink, LL_DEBUG, "[ParserTDF] Market: {}, total contracts: {}, date: {}", pCodeResult->szMarket[i], pCodeResult->nCodeCount[i], pCodeResult->nCodeDate[i]);
			}
		}
	}
	break;
	case MSG_SYS_HEART_BEAT:
	{
		write_log(m_parserSink, LL_DEBUG, "[ParserTDF] Receiving heartbeat message ...");
	}
	break;
	case MSG_SYS_SINGLE_CODETABLE_RESULT:
	{
		TDF_SINGLE_CODE_RESULT* pCodeResult = (TDF_SINGLE_CODE_RESULT*)pSysMsg->pData;
		if (pCodeResult)
		{
			write_log(m_parserSink, LL_DEBUG, "[ParserTDF] Receive total instruments {} from {} on {}", pCodeResult->nCodeCount, pCodeResult->szMarket, pCodeResult->nCodeDate);
			//收到此消息后，可获取pCodeResult->szMarket的代码表.
		}
	}
	break;
	case MSG_SYS_MARKET_EVENT:
	{
		TDF_MARKET_EVENT* pEvent = (TDF_MARKET_EVENT*)pSysMsg->pData;
		if (pEvent)
		{
			switch (pEvent->nEvent)
			{
			case ID_MARKET_CLEAR: //市场清盘，交易日改变
				write_log(m_parserSink, LL_DEBUG, "[ParserTDF] Recv{} market clear msg on {}", pEvent->szMarket, pEvent->nDate);
				break;
			case ID_MARKET_TRANS:   //市场转数，该市场当前交易日不再有行情数据
				write_log(m_parserSink, LL_DEBUG, "[ParserTDF] Recv {} market close msg on {}", pEvent->szMarket, pEvent->nDate);
				break;
			default:
				//其他无需处理消息
				break;
			}
		}
	}
	break;
	case MSG_SYS_FAIL_REPLAY:
	{
		TDF_FAIL_REPLAY* pFail = (TDF_FAIL_REPLAY*)pSysMsg->pData;
		if (pFail)
		{
			write_log(m_parserSink, LL_WARN, "[ParserTDF] Fail replay. market: {}, date: {}, msg: {}", pFail->szMarket, pFail->nDate, pFail->szInfo);
		}
	}
	break;
	case MSG_SYS_ADDCODE:
	{
		TDF_ADD_CODE* pAddCode = (TDF_ADD_CODE*)pSysMsg->pData;
		if (pAddCode)
		{
			write_log(m_parserSink, LL_DEBUG, "[ParserTDF] Add Code：market: {}, codedate: {}, items: {}", pAddCode->szMarket, pAddCode->nCodeDate, pAddCode->nItems);
		}
	}
	break;
	default:
		break;
	}
}