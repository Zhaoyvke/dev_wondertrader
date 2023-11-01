/*!
 * \file ParserInsight.cpp
 * \project	WonderTrader
 *
 * \author Sunseeeeeeker
 * \date 2023/05/22
 *
 * \brief
 */
#include "ParserInsight.h"

#include "../Includes/WTSDataDef.hpp"
#include "../Includes/WTSContractInfo.hpp"
#include "../Includes/WTSVariant.hpp"
#include "../Includes/IBaseDataMgr.h"

#include "../Share/TimeUtils.hpp"
#include "../Share/StrUtil.hpp"
#include "../Share/StdUtils.hpp"
#include "../Share/ModuleHelper.hpp"

#include <boost/filesystem.hpp>

 //By Wesley @ 2022.01.05
#include "../Share/fmtlib.h"


#ifdef _WIN32
#ifdef _WIN64
#pragma comment(lib, "../API/insight_tcp_gateway_3.2.7/WinX64Release/ACE.lib")
#pragma comment(lib, "../API/insight_tcp_gateway_3.2.7/WinX64Release/mdc_gateway_client.lib")
#pragma comment(lib, "../API/insight_tcp_gateway_3.2.7/WinX64Release/libprotobuf.lib")
#endif
#else
#pragma comment(lib, "../API/insight_tcp_gateway_3.2.7/x86/ACE.lib")
#pragma comment(lib, "../API/insight_tcp_gateway_3.2.7/x86/mdc_gateway_client.lib")
#pragma comment(lib, "../API/insight_tcp_gateway_3.2.7/x86/libprotobuf.lib")
#endif


template<typename... Args>
inline void write_log(IParserSpi* sink, WTSLogLevel ll, const char* format, const Args&... args)
{
	if (sink == NULL)
		return;

	static thread_local char buffer[512] = { 0 };
	memset(buffer, 0, 512);
	fmt::format_to(buffer, format, args...);

	sink->handleParserLog(ll, buffer);
}

inline void split(const std::string& str, std::vector<std::string>& vec, const std::string delim = ";")
{
	vec.clear();

	auto start = str.find_first_not_of(delim, 0);
	auto pos = str.find_first_of(delim, start);

	while (pos != std::string::npos || start != std::string::npos)
	{
		vec.emplace_back(std::move(str.substr(start, pos - start)));
		start = str.find_first_not_of(delim, pos);
		pos = str.find_first_of(delim, start);
	}
}

extern "C"
{
	EXPORT_FLAG IParserApi* createParser()
	{
		ParserInsight* parser = new ParserInsight();
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

inline uint32_t strToTime(const char* strTime)
{
	std::string str;
	const char *pos = strTime;
	while (strlen(pos) > 0)
	{
		if (pos[0] != ':')
		{
			str.append(pos, 1);
		}
		pos++;
	}

	return strtoul(str.c_str(), NULL, 10);
}

inline double checkValid(double val)
{
	if (val == DBL_MAX || val == FLT_MAX)
		return 0;

	return val;
}

inline std::string get_data_type_name(const EMarketDataType& type) {
	std::string data_type;
	switch (type) {
	case MD_TICK:			 //TICK行情数据（包含股票.指数.债券.基金.期权.权证.期货等）
	{
		data_type = "MD_TICK";
		break;
	}
	case MD_TRANSACTION:     // 逐笔成交数据
	{
		data_type = "MD_TRANSACTION";
		break;
	}
	case MD_ORDER:			 //逐笔委托数据
	{
		data_type = "MD_ORDER";
		break;
	}
	case MD_CONSTANT:		//静态数据
	{
		data_type = "MD_CONSTANT";
		break;
	}
	case MD_KLINE_15S:
	{
		data_type = "MD_KLINE_15S";
		break;
	}
	case MD_KLINE_1MIN:		//1分钟K线
	{
		data_type = "MD_KLINE_1MIN";
		break;
	}
	case MD_KLINE_5MIN:
	{
		data_type = "MD_KLINE_5MIN";
		break;
	}
	case MD_KLINE_15MIN:
	{
		data_type = "MD_KLINE_15MIN";
		break;
	}
	case MD_KLINE_30MIN:
	{
		data_type = "MD_KLINE_30MIN";
		break;
	}
	case MD_KLINE_60MIN:
	{
		data_type = "MD_KLINE_60MIN";
		break;
	}
	case MD_KLINE_1D:
	{
		data_type = "MD_KLINE_1D";
		break;
	}
	case MD_TWAP_1MIN:			//1分钟粒度的TWAP数据
	{
		data_type = "MD_TWAP_1MIN";
		break;
	}
	case MD_VWAP_1MIN:			//1分钟粒度的VWAP数据
	{
		data_type = "MD_VWAP_1MIN";
		break;
	}
	case MD_VWAP_1S:
	{
		data_type = "MD_VWAP_1S";
		break;
	}
	case MD_SIMPLE_TICK:		//证券简化Tick行情(MDSimpleTick)
	{
		data_type = "MD_SIMPLE_TICK";
		break;
	}
	case AD_UPSDOWNS_ANALYSIS:	//涨跌分析
	{
		data_type = "AD_UPSDOWNS_ANALYSIS";
		break;
	}
	case AD_INDICATORS_RANKING:	//行情指标排行榜
	{
		data_type = "AD_INDICATORS_RANKING";
		break;
	}
	case DYNAMIC_PACKET:	//动态数据包
	{
		data_type = "DYNAMIC_PACKET";
		break;
	}
	case AD_FUND_FLOW_ANALYSIS:		//资金流向分析数据
	{
		data_type = "AD_FUND_FLOW_ANALYSIS";
		break;
	}
	case AD_VOLUME_BYPRICE:			//成交分价数据
	{
		data_type = "AD_VOLUME_BYPRICE";
		break;
	}
	case MD_ETF_BASICINFO:			//	ETF成分券信息
	{
		data_type = "MD_ETF_BASICINFO";
		break;
	}
	case AD_ORDERBOOK_SNAPSHOT:		//基于订单簿推算的行情快照
	{
		data_type = "AD_ORDERBOOK_SNAPSHOT";
		break;
	}
	default:
	{
		data_type = "UnknownDataType";
	}
	}
	return data_type;
}

ParserInsight::ParserInsight()
	: m_pUserAPI(NULL)
	, m_uTradingDate(0)
	, m_bLogin(false)
	, m_bNoConns(false)
	, m_bReconnect(false)
	, m_iService(0)
	, m_iReconnect(0)
{

}


ParserInsight::~ParserInsight()
{
	ClientFactory::Uninstance();
	m_pUserAPI = NULL;
}

bool ParserInsight::init(WTSVariant* config)
{
	m_strHost = config->getCString("host");
	m_iPort = config->getInt32("port");
	m_strUser = config->getCString("user");
	m_strPass = config->getCString("pass");

	m_strFontBackup = config->getCString("front_backup");

	m_iService = config->getUInt32("service_num");
	if (m_iService == 0)
		m_iService = 5;

	m_strFlowDir = config->getCString("flowdir");
	if (m_strFlowDir.empty())
		m_strFlowDir = "CertFlow";

	m_strCertDir = config->getCString("cert");

	m_strFlowDir = StrUtil::standardisePath(m_strFlowDir);

	std::string module = config->getCString("mdcmodule");
	if (module.empty())
		module = "mdc_gateway_client";

	std::string path = StrUtil::printf("%s/%s/", m_strFlowDir.c_str(), m_strUser.c_str());
	path = StrUtil::standardisePath(path);
	if (!StdFile::exists(path.c_str()))
		boost::filesystem::create_directories(path.c_str());

	std::string dllpath;
#ifdef _WIN32
	dllpath = getBinDir() + DLLHelper::wrap_module(module.c_str(), "");
#else
	dllpath = getBinDir() + DLLHelper::wrap_module(module.c_str(), "lib");
#endif // _WIN32

	//std::string dllpath = getBinDir() + DLLHelper::wrap_module(module.c_str(), "");
	m_hInst = DLLHelper::load_library(dllpath.c_str());

	//环境初始化及常用配置
	open_file_log();
	init_env();
	open_trace();//打开trace，输出流量日志消息，默认打开
	open_heartbeat_trace();//打开heartbeat_trace，输出心跳日志消息，默认打开
	open_response_callback();//打开response_callback，自行处理response消息	
	//open_cout_log();
	//close_cout_log();
	//close_trace();
	//close_file_log();
	//close_heartbeat_trace();//关闭heartbeat_trace，不输出心跳日志消息
	//close_response_callback();//关闭response_callback，程序自动处理response消息

	m_pUserAPI = ClientFactory::Instance()->CreateClient(true, m_strCertDir.c_str());

	if (!m_pUserAPI)
	{
		write_log(m_sink, LL_ERROR, "[ParserInsight] Create client failed!");
		ClientFactory::Uninstance();
		m_pUserAPI = NULL;
		return false;
	}

	// 注册句柄
	m_pUserAPI->set_handle_pool_thread_count(m_iService);
	m_pUserAPI->RegistHandle(this);

	return true;
}

void ParserInsight::release()
{
	disconnect();
}

bool ParserInsight::connect()
{
	DoLogin();

	return true;
}

bool ParserInsight::disconnect()
{
	if (m_pUserAPI)
	{
		ClientFactory::Uninstance();

		m_pUserAPI = NULL;
	}

	return true;
}

void ParserInsight::OnServiceMessage(const ::com::htsc::mdc::insight::model::MarketDataStream& data_stream)
{
	write_log(m_sink, LL_DEBUG, "[ParserInsight] Process a Service message");
	m_iValue = 1;
}

void ParserInsight::OnMarketData(const com::htsc::mdc::insight::model::MarketData& data)
{
	if (m_pBaseDataMgr == nullptr)
	{
		return;
	}

	// 处理订阅后推送的实时行情数据
	//std::string dataType = get_data_type_name(data.marketdatatype());
	switch (data.marketdatatype())
	{
	case MD_TICK:
	{
		// Tick快照
		if (data.has_mdstock())
		{
			// 股票
			MDStock stock = data.mdstock();

			uint32_t actDate = stock.mddate();
			uint32_t actTime = stock.mdtime();

			std::string code, exchg;
			exchg = (stock.securityidsource() == XSHG) ? "SSE" : "SZSE";
			code = stock.htscsecurityid();
			auto pos = code.find_first_of(".", 0);
			code = code.substr(0, pos);

			WTSContractInfo* ct = m_pBaseDataMgr->getContract(code.c_str(), exchg.c_str());
			if (ct == NULL)
			{
				if (m_sink)
					write_log(m_sink, LL_ERROR, "[ParserInsight] Instrument {}.{} not exists...", exchg.c_str(), code.c_str());
				return;
			}
			WTSCommodityInfo* commInfo = ct->getCommInfo();

			WTSTickData* tick = WTSTickData::create(code.c_str());
			tick->setContractInfo(ct);
			WTSTickStruct& quote = tick->getTickStruct();
			strcpy(quote.exchg, exchg.c_str());

			quote.action_date = actDate;
			quote.action_time = actTime;

			quote.price = checkValid((double)stock.lastpx() / 10000.0);
			quote.open = checkValid((double)stock.openpx() / 10000.0);
			quote.high = checkValid((double)stock.highpx() / 10000.0);
			quote.low = checkValid((double)stock.lowpx() / 10000.0);

			quote.total_volume = stock.totalvolumetrade();
			quote.trading_date = m_uTradingDate;
			quote.total_turnover = stock.totalvaluetrade();
			
			quote.upper_limit = checkValid((double)stock.maxpx() / 10000.0);
			quote.lower_limit = checkValid((double)stock.minpx() / 10000.0);

			quote.pre_close = checkValid((double)stock.preclosepx() / 10000.0);

			//委卖价格
			for (int i = 0; i < 10; i++)
			{
				quote.ask_prices[i] = checkValid((double)stock.sellpricequeue()[i] / 10000.0);
				quote.ask_qty[i] = (uint32_t)stock.sellorderqtyqueue()[i];

				quote.bid_prices[i] = checkValid((double)stock.buypricequeue()[i] / 10000.0);
				quote.bid_qty[i] = (uint32_t)stock.buyorderqtyqueue()[i];
			}

			if (m_sink)
				m_sink->handleQuote(tick, 1);

			//处理逐笔
			if (stock.buyorderqueue().size() > 0)//买入委托队列
			{
				WTSOrdQueData* buyQue = WTSOrdQueData::create(code.c_str());
				buyQue->setContractInfo(ct);

				WTSOrdQueStruct& buyOS = buyQue->getOrdQueStruct();
				strcpy(buyOS.exchg, exchg.c_str());

				buyOS.trading_date = m_uTradingDate;
				buyOS.action_date = actDate;
				buyOS.action_time = actTime;

				buyOS.side = BDT_Buy;
				buyOS.price = quote.bid_prices[0];
				buyOS.order_items = stock.buyorderqueue().size();
				buyOS.qsize = std::min(stock.buyorderqueue().size(), 50); // 最大只有50档的数据

				for (uint32_t i = 0; i < buyOS.qsize; i++)
				{
					buyOS.volumes[i] = (uint32_t)stock.buyorderqueue()[i];
				}

				if (m_sink)
					m_sink->handleOrderQueue(buyQue);

				buyQue->release();
			}

			if (stock.sellorderqueue().size() > 0)
			{
				WTSOrdQueData* sellQue = WTSOrdQueData::create(code.c_str());
				sellQue->setContractInfo(ct);

				WTSOrdQueStruct& sellOS = sellQue->getOrdQueStruct();
				strcpy(sellOS.exchg, exchg.c_str());

				sellOS.trading_date = m_uTradingDate;
				sellOS.action_date = actDate;
				sellOS.action_time = actTime;

				sellOS.side = BDT_Sell;
				sellOS.price = quote.ask_prices[0];
				sellOS.order_items = stock.sellorderqueue().size();
				sellOS.qsize = std::min(stock.sellorderqueue().size(), 50);

				for (uint32_t i = 0; i < sellOS.qsize; i++)
				{
					sellOS.volumes[i] = (uint32_t)stock.sellorderqueue()[i];
				}

				if (m_sink)
					m_sink->handleOrderQueue(sellQue);

				sellQue->release();
			}

			tick->release();
		}
		else if (data.has_mdbond())
		{
			// 债券
			MDBond bond = data.mdbond();

			uint32_t actDate = bond.mddate();
			uint32_t actTime = bond.mdtime();


			std::string code, exchg;
			exchg = (bond.securityidsource() == XSHE) ? "SZSE" : "SSE";
			code = bond.htscsecurityid();
			auto pos = code.find_first_of(".", 0);
			code = code.substr(0, pos);

			WTSContractInfo* ct = m_pBaseDataMgr->getContract(code.c_str(), exchg.c_str());
			if (ct == NULL)
			{
				if (m_sink)
					write_log(m_sink, LL_ERROR, "[ParserInsight] Instrument {}.{} not exists...", exchg.c_str(), code.c_str());

				return;
			}
			WTSCommodityInfo* commInfo = ct->getCommInfo();
			WTSTickData* tick = WTSTickData::create(code.c_str());
			tick->setContractInfo(ct);
			WTSTickStruct& quote = tick->getTickStruct();
			strcpy(quote.exchg, exchg.c_str());

			quote.action_date = actDate;
			quote.action_time = actTime;

			quote.price = checkValid((double)bond.lastpx() / 10000.0);
			quote.open = checkValid((double)bond.openpx() / 10000.0);
			quote.high = checkValid((double)bond.highpx() / 10000.0);
			quote.low = checkValid((double)bond.lowpx() / 10000.0);

			quote.total_volume = bond.totalvolumetrade();
			quote.trading_date = m_uTradingDate;
			quote.total_turnover = bond.totalvaluetrade();

			quote.upper_limit = checkValid((double)bond.maxpx() / 10000.0);
			quote.lower_limit = checkValid((double)bond.minpx() / 10000.0);

			quote.pre_close = checkValid((double)bond.preclosepx() / 10000.0);

			//委卖价格
			for (int i = 0; i < 10; i++)
			{
				quote.ask_prices[i] = checkValid((double)bond.sellpricequeue()[i] / 10000.0);
				quote.ask_qty[i] = (uint32_t)bond.sellorderqtyqueue()[i];

				quote.bid_prices[i] = checkValid((double)bond.buypricequeue()[i] / 10000.0);
				quote.bid_qty[i] = (uint32_t)bond.buyorderqtyqueue()[i];
			}

			if (m_sink)
				m_sink->handleQuote(tick, 1);

			//处理逐笔
			if (bond.buyorderqueue().size() > 0)
			{
				WTSOrdQueData* buyQue = WTSOrdQueData::create(code.c_str());
				buyQue->setContractInfo(ct);

				WTSOrdQueStruct& buyOS = buyQue->getOrdQueStruct();
				strcpy(buyOS.exchg, exchg.c_str());

				buyOS.trading_date = m_uTradingDate;
				buyOS.action_date = actDate;
				buyOS.action_time = actTime;

				buyOS.side = BDT_Buy;
				buyOS.price = quote.bid_prices[0];
				buyOS.order_items = bond.buyorderqueue().size();
				buyOS.qsize = std::min(bond.buyorderqueue().size(), 50); // 最大只有50档的数据

				for (uint32_t i = 0; i < buyOS.qsize; i++)
				{
					buyOS.volumes[i] = (uint32_t)bond.buyorderqueue()[i];
				}

				if (m_sink)
					m_sink->handleOrderQueue(buyQue);

				buyQue->release();
			}

			if (bond.sellorderqueue().size() > 0)
			{
				WTSOrdQueData* sellQue = WTSOrdQueData::create(code.c_str());
				sellQue->setContractInfo(ct);

				WTSOrdQueStruct& sellOS = sellQue->getOrdQueStruct();
				strcpy(sellOS.exchg, exchg.c_str());

				sellOS.trading_date = m_uTradingDate;
				sellOS.action_date = actDate;
				sellOS.action_time = actTime;

				sellOS.side = BDT_Sell;
				sellOS.price = quote.ask_prices[0];
				sellOS.order_items = bond.sellorderqueue().size();
				sellOS.qsize = std::min(bond.sellorderqueue().size(), 50);

				for (uint32_t i = 0; i < sellOS.qsize; i++)
				{
					sellOS.volumes[i] = (uint32_t)bond.sellorderqueue()[i];
				}

				if (m_sink)
					m_sink->handleOrderQueue(sellQue);

				sellQue->release();
			}

			tick->release();

		}
		else if (data.has_mdindex())
		{
			// 指数
			MDIndex index = data.mdindex();

			uint32_t actDate = index.mddate();
			uint32_t actTime = index.mdtime();

			std::string code, exchg;
			exchg = (index.securityidsource() == XSHG) ? "SSE" : "SZSE";
			code = index.htscsecurityid();
			auto pos = code.find_first_of(".", 0);
			code = code.substr(0, pos);

			WTSContractInfo* ct = m_pBaseDataMgr->getContract(code.c_str(), exchg.c_str());
			if (ct == NULL)
			{
				if (m_sink)
					write_log(m_sink, LL_ERROR, "[ParserInsight] Instrument {}.{} not exists...", exchg.c_str(), code.c_str());
				return;
			}
			WTSCommodityInfo* commInfo = ct->getCommInfo();

			WTSTickData* tick = WTSTickData::create(code.c_str());
			tick->setContractInfo(ct);
			WTSTickStruct& quote = tick->getTickStruct();
			strcpy(quote.exchg, exchg.c_str());

			quote.action_date = actDate;
			quote.action_time = actTime;

			quote.price = checkValid((double)index.lastpx() / 10000.0);
			quote.open = checkValid((double)index.openpx() / 10000.0);
			quote.high = checkValid((double)index.highpx() / 10000.0);
			quote.low = checkValid((double)index.lowpx() / 10000.0);

			quote.total_volume = index.totalvolumetrade();
			quote.trading_date = m_uTradingDate;
			quote.total_turnover = index.totalvaluetrade();

			/*		quote.total_buy_volume = (double)index.totalbuynumber();
					quote.total_sell_volume = (double)index.totalsellnumber();
					quote.total_buy_turnover = (double)index.totalbuyvaluetrade();
					quote.total_sell_turnover = (double)index.totalsellvaluetrade();*/

					//quote.upper_limit = checkValid((double)index.maxpx() / 10000.0);
					//quote.lower_limit = checkValid((double)index.minpx() / 10000.0);

			quote.pre_close = checkValid((double)index.preclosepx() / 10000.0);

			if (m_sink)
				m_sink->handleQuote(tick, 1);

			tick->release();
		}
		else if (data.has_mdoption())
		{
			// 期权
		}
		else if (data.has_mdfuture())
		{
			// 期货
		}
		else if (data.has_mdspot())
		{
			// 现货
		}
		else if (data.has_mdfund())
		{
			// 基金
			MDFund fund = data.mdfund();

			uint32_t actDate = fund.mddate();
			uint32_t actTime = fund.mdtime();

			std::string code, exchg;
			exchg = (fund.securityidsource() == XSHG) ? "SSE" : "SZSE";
			code = fund.htscsecurityid();
			auto pos = code.find_first_of(".", 0);
			code = code.substr(0, pos);

			WTSContractInfo* ct = m_pBaseDataMgr->getContract(code.c_str(), exchg.c_str());
			if (ct == NULL)
			{
				if (m_sink)
					write_log(m_sink, LL_ERROR, "[ParserInsight] Instrument {}.{} not exists...", exchg.c_str(), code.c_str());
				return;
			}
			WTSCommodityInfo* commInfo = ct->getCommInfo();

			WTSTickData* tick = WTSTickData::create(code.c_str());
			tick->setContractInfo(ct);
			WTSTickStruct& quote = tick->getTickStruct();
			strcpy(quote.exchg, exchg.c_str());

			quote.action_date = actDate;
			quote.action_time = actTime;

			quote.price = checkValid((double)fund.lastpx() / 10000.0);
			quote.open = checkValid((double)fund.openpx() / 10000.0);
			quote.high = checkValid((double)fund.highpx() / 10000.0);
			quote.low = checkValid((double)fund.lowpx() / 10000.0);

			quote.total_volume = fund.totalvolumetrade();
			quote.trading_date = m_uTradingDate;
			quote.total_turnover = fund.totalvaluetrade();

			quote.upper_limit = checkValid((double)fund.maxpx() / 10000.0);
			quote.lower_limit = checkValid((double)fund.minpx() / 10000.0);

			quote.pre_close = checkValid((double)fund.preclosepx() / 10000.0);

			//委卖价格
			for (int i = 0; i < 10; i++)
			{
				//if(fund.sellpricequeue().size()>1)
				quote.ask_prices[i] = checkValid((double)fund.sellpricequeue()[i] / 10000.0);
				quote.ask_qty[i] = (uint32_t)fund.sellorderqtyqueue()[i];

				quote.bid_prices[i] = checkValid((double)fund.buypricequeue()[i] / 10000.0);
				quote.bid_qty[i] = (uint32_t)fund.buyorderqtyqueue()[i];
			}

			if (m_sink)
				m_sink->handleQuote(tick, 1);

			//处理逐笔
			if (fund.buyorderqueue().size() > 0)//买入委托队列
			{
				WTSOrdQueData* buyQue = WTSOrdQueData::create(code.c_str());
				buyQue->setContractInfo(ct);

				WTSOrdQueStruct& buyOS = buyQue->getOrdQueStruct();
				strcpy(buyOS.exchg, exchg.c_str());

				buyOS.trading_date = m_uTradingDate;
				buyOS.action_date = actDate;
				buyOS.action_time = actTime;

				buyOS.side = BDT_Buy;
				buyOS.price = quote.bid_prices[0];
				buyOS.order_items = fund.buyorderqueue().size();
				buyOS.qsize = std::min(fund.buyorderqueue().size(), 50); // 最大只有50档的数据

				for (uint32_t i = 0; i < buyOS.qsize; i++)
				{
					buyOS.volumes[i] = (uint32_t)fund.buyorderqueue()[i];
				}

				if (m_sink)
					m_sink->handleOrderQueue(buyQue);

				buyQue->release();
			}

			if (fund.sellorderqueue().size() > 0)
			{
				WTSOrdQueData* sellQue = WTSOrdQueData::create(code.c_str());
				sellQue->setContractInfo(ct);

				WTSOrdQueStruct& sellOS = sellQue->getOrdQueStruct();
				strcpy(sellOS.exchg, exchg.c_str());

				sellOS.trading_date = m_uTradingDate;
				sellOS.action_date = actDate;
				sellOS.action_time = actTime;

				sellOS.side = BDT_Sell;
				sellOS.price = quote.ask_prices[0];
				sellOS.order_items = fund.sellorderqueue().size();
				sellOS.qsize = std::min(fund.sellorderqueue().size(), 50);

				for (uint32_t i = 0; i < sellOS.qsize; i++)
				{
					sellOS.volumes[i] = (uint32_t)fund.sellorderqueue()[i];
				}

				if (m_sink)
					m_sink->handleOrderQueue(sellQue);

				sellQue->release();
			}

			tick->release();
		}
		break;
	}
	case MD_TRANSACTION:
	{
		//// 逐笔成交
		if (data.has_mdtransaction())
		{
			MDTransaction stock = data.mdtransaction();
			//write_log(m_sink, LL_DEBUG, "processing transaction data of {}", stock.htscsecurityid());


			uint32_t actDate = stock.mddate();
			uint32_t actTime = stock.mdtime();

			std::string code, exchg;
			exchg = (stock.securityidsource() == XSHG) ? "SSE" : "SZSE";
			code = stock.htscsecurityid();
			auto pos = code.find_first_of(".", 0);
			code = code.substr(0, pos);

			WTSContractInfo* ct = m_pBaseDataMgr->getContract(code.c_str(), exchg.c_str());
			if (ct == nullptr)
				return;

			WTSCommodityInfo* commInfo = ct->getCommInfo();

			WTSTransData *trans = WTSTransData::create(code.c_str());
			WTSTransStruct& ts = trans->getTransStruct();
			strcpy(ts.exchg, exchg.c_str());

			ts.trading_date = m_uTradingDate;
			ts.action_date = actDate;
			ts.action_time = actTime;

			ts.index = stock.applseqnum();

			if (stock.tradebsflag() == 1)
				ts.side = BDT_Buy;
			else if (stock.tradebsflag() == 2)
				ts.side = BDT_Sell;
			else
				ts.side = BDT_Unknown;

			ts.ttype = stock.tradetype() == 0 ? TT_Match : TT_Cancel;

			ts.price = (double)stock.tradeprice() / 10000.0;
			ts.volume = (uint32_t)stock.tradeqty();
			ts.bidorder = stock.tradebuyno();
			ts.askorder = stock.tradesellno();

			if (m_sink)
				m_sink->handleTransaction(trans);
		}
		break;
	}
	case MD_ORDER:
	{
		//// 逐笔委托
		if (data.has_mdorder())
		{
			MDOrder stock = data.mdorder();
			//write_log(m_sink, LL_DEBUG, "processing order data of {}", stock.htscsecurityid());

			uint32_t actDate = stock.mddate();
			uint32_t actTime = stock.mdtime();

			std::string code, exchg;
			exchg = (stock.securityidsource() == XSHG) ? "SSE" : "SZSE";
			code = stock.htscsecurityid();
			auto pos = code.find_first_of(".", 0);
			code = code.substr(0, pos);

			WTSContractInfo* ct = m_pBaseDataMgr->getContract(code.c_str(), exchg.c_str());
			if (ct == nullptr)
				return;

			WTSCommodityInfo* commInfo = ct->getCommInfo();

			WTSOrdDtlData *ordDtl = WTSOrdDtlData::create(code.c_str());
			WTSOrdDtlStruct& ts = ordDtl->getOrdDtlStruct();
			strcpy(ts.exchg, exchg.c_str());

			ts.trading_date = m_uTradingDate;
			ts.action_date = actDate;
			ts.action_time = actTime;

			ts.index = stock.applseqnum();

			if (stock.orderbsflag() == 1)
				ts.side = BDT_Buy;
			else if (stock.orderbsflag() == 2)
				ts.side = BDT_Sell;
			else
				ts.side = BDT_Unknown;

			if (stock.ordertype() == 1)
				ts.otype = ODT_AnyPrice;
			else if (stock.ordertype() == 2)
				ts.otype = ODT_LimitPrice;
			else if (stock.ordertype() == 3)
				ts.otype = ODT_BestPrice;
			else
				ts.otype = ODT_Unknown;

			ts.price = (double)stock.orderprice() / 10000.0;
			ts.volume = (uint32_t)stock.orderqty();

			if (m_sink)
				m_sink->handleOrderDetail(ordDtl);
		}
		break;
	}
	case MD_CONSTANT:
	{
		// 静态信息
		//break;
	}
	case MD_KLINE_15S:
	case MD_KLINE_1MIN:
	{
		/*if (data.has_mdkline())
		{
			switch (data.mdkline().securitytype())
			{
			case BondType:
				{
				ADKLine bondkline_1 = data.mdkline();
				write_log(m_sink, LL_DEBUG, "code: {}, date: {}", bondkline_1.htscsecurityid(), bondkline_1.mddate());
				}
			case StockType:
			default:
				break;
			}
		}
		break;*/
	}
	case MD_KLINE_5MIN:
	case MD_KLINE_1D://实时数据只提供15S和1MIN K线

	default:
		break;
	}
}

void ParserInsight::OnQueryResponse(const ::com::htsc::mdc::insight::model::MDQueryResponse& response)
{

}

void ParserInsight::OnLoginSuccess()
{
	write_log(m_sink, LL_INFO, "[ParserInsight] {} login success", m_strUser);
	m_bLogin = true;

	if (m_sink)
	{
		m_sink->handleEvent(WPE_Login, 0);
	}

	//订阅行情数据
	//SubscribeMarketData();
}

void ParserInsight::OnLoginFailed(int error_no, const std::string& message)
{
	write_log(m_sink, LL_ERROR, "[ParserInsight] {} login fail, code: {}, msg: {}", m_strUser, error_no, message);
	m_bLogin = false;

	if (m_sink)
	{
		m_sink->handleEvent(WPE_Login, 0);
	}
}

void ParserInsight::OnNoConnections()
{
	write_log(m_sink, LL_ERROR, "[ParserInsight] All connections fail. Try to connect again");
	m_bNoConns = true;
	m_bReconnect = true;

	if (m_sink)
	{
		m_sink->handleEvent(WPE_Connect, 0);
	}
}

void ParserInsight::OnReconnect()
{
	++m_iReconnect;
	write_log(m_sink, LL_WARN, "[ParserInsight] Reconnect {} times", m_iReconnect);
	m_bReconnect = true;
}

void ParserInsight::DoLogin()
{
	if (m_pUserAPI == NULL)
	{
		return;
	}

	// 添加备用发现网关地址
	std::vector<std::string> m_vFrontBackup;
	split(m_strFontBackup, m_vFrontBackup, ";");

	// 登录
	int iResult = m_pUserAPI->LoginByServiceDiscovery(m_strHost, m_iPort, m_strUser, m_strPass, false, m_vFrontBackup);

	if (iResult != 0)
	{
		if (m_sink)
		{
			m_sink->handleEvent(WPE_Connect, iResult);

			write_log(m_sink, LL_ERROR, "[ParserInsight] Sending login request failed: {}", iResult);
		}
	}
	else
	{
		m_uTradingDate = TimeUtils::getCurDate();
		if (m_sink)
		{
			m_sink->handleEvent(WPE_Connect, 0);
			m_sink->handleEvent(WPE_Login, 0);
		}

		write_log(m_sink, LL_INFO, "[ParserInsight] Connecting to [{}] successed: {}, begin to subscibe data ...", m_strHost, iResult);

		DoSubscribeMD();
	}
}

void ParserInsight::DoSubscribeMD()
{
	//订阅 SubscribeByID
	ESubscribeActionType action_type = COVERAGE;
	std::unique_ptr<SubscribeByID> id(new SubscribeByID());

	CodeSet codeFilter = m_fitSHSubs;

	if (!codeFilter.empty())
	{
		//char ** subscribe = new char*[codeFilter.size()];
		std::string code;
		CodeSet::iterator it = codeFilter.begin();
		for (; it != codeFilter.end(); it++)
		{
			//subscribe[nCount++] = (char*)(*it).c_str();
			code = std::string((char*)(*it).c_str());
			code += ".SH";

			SubscribeByIDDetail* id_detail = id->add_subscribebyiddetails();
			id_detail->set_htscsecurityid(code);

			// 订阅tick, transaction, order数据
			id_detail->add_marketdatatypes(MD_TICK);
			id_detail->add_marketdatatypes(MD_TRANSACTION);
			id_detail->add_marketdatatypes(MD_ORDER);
			//id_detail->add_marketdatatypes(MD_KLINE_1MIN);
		}

		write_log(m_sink, LL_INFO, "[DoSubscribeMD] Subscribe {} stocks from SSE", codeFilter.size());
		codeFilter.clear();
	}

	codeFilter = m_fitSZSubs;
	if (!codeFilter.empty())
	{
		//char ** subscribe = new char*[codeFilter.size()];
		std::string code;

		CodeSet::iterator it = codeFilter.begin();
		for (; it != codeFilter.end(); it++)
		{
			//subscribe[nCount++] = (char*)(*it).c_str();
			code = std::string((char*)(*it).c_str());
			code += ".SZ";
			//write_log(m_sink, LL_DEBUG, "[DoSubscribeMD] Sucribing {}th Contract [{}]", nCount, code);

			SubscribeByIDDetail* id_detail = id->add_subscribebyiddetails();
			id_detail->set_htscsecurityid(code);
			//id_detail->marketdatatypes;
			// 订阅tick, transaction, order数据
			id_detail->add_marketdatatypes(MD_TICK);
			id_detail->add_marketdatatypes(MD_TRANSACTION);
			id_detail->add_marketdatatypes(MD_ORDER);
			//id_detail->add_marketdatatypes(MD_KLINE_1MIN);
		}

		write_log(m_sink, LL_INFO, "[DoSubscribeMD] Subscribe {} stocks from SZSE", codeFilter.size());
		codeFilter.clear();
	}

	if (m_pUserAPI)
	{
		int iResult = m_pUserAPI->SubscribeByID(action_type, &(*id));
		if (iResult != 0)
		{
			if (m_sink)
			{
				write_log(m_sink, LL_ERROR, "[ParserInsight] Sending md subscribe request of {} SSE and {} SZSE failed: {}({})", m_fitSHSubs.size(), m_fitSZSubs.size(), iResult, get_error_code_value(iResult));
			}
		}
		else
		{
			if (m_sink)
				write_log(m_sink, LL_INFO, "[ParserInsight] Market data of {} instruments of SSE and {} instruments of SZSE subscribed", m_fitSHSubs.size(), m_fitSZSubs.size());
		}
	}
}

void ParserInsight::subscribe(const CodeSet &vecSymbols)
{
	if (m_uTradingDate == 0)
	{
		for (auto& code : vecSymbols)
		{
			if (strncmp(code.c_str(), "SSE.", 4) == 0)
			{
				m_fitSHSubs.insert(code.c_str() + 4);
			}
			else if (strncmp(code.c_str(), "SZSE.", 5) == 0)
			{
				m_fitSZSubs.insert(code.c_str() + 5);
			}
		}
	}
	else
	{
		for (auto& code : vecSymbols)
		{
			if (strncmp(code.c_str(), "SSE.", 4) == 0)
			{
				m_fitSHSubs.insert(code.c_str() + 4);
			}
			else if (strncmp(code.c_str(), "SZSE.", 5) == 0)
			{
				m_fitSZSubs.insert(code.c_str() + 5);
			}
		}

		//订阅 SubscribeByID
		ESubscribeActionType action_type = COVERAGE;
		std::unique_ptr<SubscribeByID> id(new SubscribeByID());

		CodeSet setSH, setSZ;
		for (auto& code : vecSymbols)
		{
			if (strncmp(code.c_str(), "SSE.", 4) == 0)
			{
				m_fitSHSubs.insert(code.c_str() + 4);
				setSH.insert(code.c_str() + 4);
			}
			else if (strncmp(code.c_str(), "SZSE.", 5) == 0)
			{
				m_fitSZSubs.insert(code.c_str() + 5);
				setSZ.insert(code.c_str() + 5);
			}
		}

		if (!setSH.empty())
		{
			//char ** subscribe = new char*[setSH.size()];
			std::string code;
			CodeSet::iterator it = setSH.begin();
			for (; it != setSH.end(); it++)
			{
				//subscribe[nCount++] = (char*)(*it).c_str();
				code = std::string((char*)(*it).c_str());
				code += ".SH";

				SubscribeByIDDetail* id_detail = id->add_subscribebyiddetails();
				id_detail->set_htscsecurityid(code);

				// 订阅tick, transaction, order数据
				id_detail->add_marketdatatypes(MD_TICK);
				id_detail->add_marketdatatypes(MD_TRANSACTION);
				id_detail->add_marketdatatypes(MD_ORDER);
				//id_detail->add_marketdatatypes(MD_KLINE_1MIN);
			}
		}

		if (!setSZ.empty())
		{
			//char ** subscribe = new char*[setSZ.size()];
			std::string code;
			CodeSet::iterator it = setSZ.begin();
			for (; it != setSZ.end(); it++)
			{
				//subscribe[nCount++] = (char*)(*it).c_str();
				code = std::string((char*)(*it).c_str());
				code += ".SH";

				SubscribeByIDDetail* id_detail = id->add_subscribebyiddetails();
				id_detail->set_htscsecurityid(code);

				// 订阅tick, transaction, order数据
				id_detail->add_marketdatatypes(MD_TICK);
				id_detail->add_marketdatatypes(MD_TRANSACTION);
				id_detail->add_marketdatatypes(MD_ORDER);
				//id_detail->add_marketdatatypes(MD_KLINE_1MIN);
			}
		}

		if (m_pUserAPI)
		{
			int iResult = m_pUserAPI->SubscribeByID(action_type, &(*id));
			if (iResult != 0)
			{
				if (m_sink)
				{
					write_log(m_sink, LL_ERROR, "“测试语句subscribe”[ParserInsight] Sending md subscribe request of {} SSE and {} SZSE failed: {}", setSH.size(), setSZ.size(), iResult);
				}
			}
			else
			{
				if (m_sink)
					write_log(m_sink, LL_INFO, "测试语句subscribe[ParserInsight] Market data of {} instruments of SSE and {} instruments of SZSE subscribed", setSH.size(), setSZ.size());
			}
		}
	}
}

void ParserInsight::unsubscribe(const CodeSet &vecSymbols)
{

}

bool ParserInsight::isConnected()
{
	return m_pUserAPI != NULL;
}

void ParserInsight::registerSpi(IParserSpi* listener)
{
	m_sink = listener;

	if (m_sink)
		m_pBaseDataMgr = m_sink->getBaseDataMgr();
}