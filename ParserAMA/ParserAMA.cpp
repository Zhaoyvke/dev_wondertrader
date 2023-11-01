#pragma once
/*!
 * \file ParserOTS.h
 * \project	WonderTrader
 *
 * \author kanavi
 * \date 2023/09/18
 *
 * \brief
 */

#include "ParserAMA.h"

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
#include <wtypes.h>
HMODULE	g_dllModule = NULL;
static std::mutex g_mutex;
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
		ParserAMA* parser = new ParserAMA();
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

ParserAMA::ParserAMA()
: m_parserSink(NULL)
, m_pBaseDataMgr(NULL)
, m_bHasArb(false)
, m_bHasSSE(false)
, m_bHasIndex(false)
, m_bHasShare(false)
, m_bHasBond(false)
, m_bHasETF(false)
, m_bHasOption(false)
, m_bHasOrder(false)
, m_bHasTrans(false)
, m_bHasOrdQue(false)
, m_bHasSnap(false)
{

}

ParserAMA::~ParserAMA()
{
	release();
}

bool ParserAMA::init(WTSVariant * config)
{
	//m_Servers = config->get("servers");

	//m_bHasL1 = config->getBoolean("has_level1");
	//m_bHasL2 = config->getBoolean("has_level2");

	//m_bHasSSE = config->getBoolean("has_sse");
	//m_bHasSZSE = config->getBoolean("has_szse");
	/*
	static const uint64_t kRDMA = 0x00000001;               // RDMA 抓包方式获取数据
    static const uint64_t kEXA  = 0x00000002;               // EXA 抓包方式获取数据
    static const uint64_t kAMI  = 0x00000004;               // AMI 组播方式获取数据
    static const uint64_t kTCP  = 0x00000008;               // TCP 流方式获取数据
    static const uint64_t kPCAP = 0x00000010;               // Libpcap 抓包方式获取数据
    static const uint64_t kMDDP = 0x00000020;               // Mddp 组播方式获取数据
    static const uint64_t kFPGA = 0x00000040;               // FPGA 方式获取数据
	*/
	

	cfg.channel_mode = config->getInt32("channel_mode");
	//TCP传输数据方式: 0 不压缩 1 华锐自定义压缩 2 zstd压缩(仅TCP模式有效)
	//cfg->tcp_compress_mode = config->getInt32("tcp_compress_mode");
	cfg.tcp_compress_mode = config->getInt32("tcp_compress_mode");
	/*
	通道高可用模式设置
	1. cfg.channel_mode 为单通道时,建议设置值为kMasterSlaveA / kMasterSlaveB
	2. cfg.channel_mode 混合开启多个通道时,根据需求设置不同的值
		1) 如果需要多个通道为多活模式接入,请设置kRegularDataFilter值
		2) 如果需要多个通道互为主备接入，请设置值为kMasterSlaveA / kMasterSlaveB,kMasterSlaveA / kMasterSlaveB 差别请参看注释说明
		   通道优先级从高到低依次为 kRDMA/kEXA/kMDDP/kAMI/kTCP/kPCAP
	    kMasterSlaveA    = 0,    // 主备切换模式的A模式，该模式下只要高优先级通道可用则一直使用，即使当前在低优先级时也立马切换高优先级
        kMasterSlaveB        = 1,   // 主备切换模式的B模式，该模式下仅在每次出现切换时选择可用的高优先级的通道，否则不执行切换动作
        kRegularDataFilter   = 2     // 规则数据过滤模式，该模式下对单独类型数据中的特征值做过滤，比如逐笔数据中根据通道号和逐笔序号执行数据过滤
	*/
	cfg.ha_mode = config->getInt32("ha_mode");
	/*
	    kInfo  = 1, ///< 普通事件
        kWarn  = 2, ///< 告警事件
        kError = 3  ///< 错误事件，比较严重，需要介入处理*/

	cfg.min_log_level = config->getInt32("min_log_level");
	/*
	设置是否输出监控数据: true(是), false(否), 监控数据通过OnIndicator 回调函数返回
	监控数据格式为json, 主要监控数据包括订阅信息，数据接收数量统计
	数据量统计:包括接收数量和成功下发的数量统计,两者差值为过滤的数据量统计
	eg: "RecvSnapshot": "5926", "SuccessSnapshot": "5925",表示接收了快照数据5926个,成功下发5925个，过滤数据为 5926 - 5925 = 1 个
		过滤的数据有可能为重复数据或者非订阅数据
	*/
	cfg.is_output_mon_data = config->getBoolean("is_output_mon_data");
	/*
	 设置逐笔保序开关: true(开启保序功能) , false(关闭保序功能)
	 主要校验逐笔成交数据和逐笔委托数据是否有丢失,如果丢失会有告警日志,缓存逐笔数据并等待keep_order_timeout_ms(单位ms)时间等待上游数据重传,
	 如果超过此时间,直接下发缓存数据,默认数据已经丢失,如果之后丢失数据过来也会丢弃。
	 同时由于深圳和上海交易所都是通道内序号连续,如果打开了保序开关,必须订阅全部代码的逐笔数据,否则一部分序号会被订阅过滤,导致数据超时等待以及丢失告警。
	*/
	cfg.keep_order = config->getBoolean("keep_order");
	cfg.keep_order_timeout_ms = config->getInt32("keep_order_timeout_ms");
	//设置默认订阅: true 代表默认全部订阅, false 代表默认全部不订阅
	cfg.is_subscribe_full = config->getBoolean("is_subscribe_full");
	/*
	配置UMS信息:
	username/password 账户名/密码, 一个账户只能保持一个连接接入（注意: 如果需要使用委托簿功能，注意账号需要有委托簿功能权限）
	ums地址配置:
		1) ums地址可以配置1-8个 建议值为2 互为主备, ums_server_cnt 为此次配置UMS地址的个数
		2) ums_servers 为UMS地址信息数据结构:
			local_ip 为本地地址,填0.0.0.0 或者本机ip
			server_ip 为ums服务端地址
			server_port 为ums服务端端口
	*/
	strcpy(cfg.username, config->getCString("user"));
	strcpy(cfg.password, config->getCString("password"));
	cfg.ums_server_cnt = config->getInt32("ums_server_cnt");
	strcpy(cfg.ums_servers[0].local_ip, config->getCString("local_ip"));
	strcpy(cfg.ums_servers[0].server_ip, config->getCString("server_ip"));
	cfg.ums_servers[0].server_port = config->getInt32("server_port");
	cfg.is_thread_safe = config->getBoolean("is_thread_safe");
	/*
	委托簿前档行情参数设置(仅委托簿版本API有效，若非委托簿版本参数设置无效):
		1) 行情输出设置，包括档位数量以及每档对应的委托队列数量
		2）委托簿计算输出参数设置，包括线程数量以及递交间隔设置
	 * @brief 委托簿构建类型定义
	static const uint8_t kNone = 0;   // 不开启委托簿
	static const uint8_t kLocalOrderBook = 1;   // 本地构建委托簿,向服务端请求同步初始状态(需要订阅相应的逐笔成交和逐笔委托)
	static const uint8_t kServerOrderBook = 2;   // 服务端远程TCP方式推送委托簿数据
	*/
	cfg.enable_order_book = config->getInt32("enable_order_book");
	cfg.entry_size = config->getInt32("entry_size");//委托簿输出最大档位数量(递交的委托簿数据档位小于等于entry_size)
	cfg.order_queue_size = config->getInt32("order_queue_size");//每个档位输出的委托队列揭示(最大设置50)
	cfg.thread_num = config->getInt32("thread_num");//构建委托簿的线程数量(递交和构建每一个市场分别开启thread_num个线程)
	 /*递交的最小时间间隔(单位:微妙, 委托簿递交间隔大于等于order_book_deliver_interval_microsecond)设置为0时, 委托簿实时下发, 不会等待间隔.*/
	cfg.order_book_deliver_interval_microsecond = config->getInt32("order_book_deliver_interval_microsecond");
	
	
	std::string module = config->getCString("AMAmodule");
	if (module.empty())
	{
		module = "AMAApi";
	}

	std::string dllpath = getBinDir() + DLLHelper::wrap_module(module.c_str(), "");
	m_hAmaInst = DLLHelper::load_library(dllpath.c_str());
	return true;
}

void ParserAMA::release()
{
	disconnect();
}

bool ParserAMA::connect()
{
	DoLogin();

	return true;
}

bool ParserAMA::disconnect()
{
	amd::ama::IAMDApi::Release();

	return true;
}

bool ParserAMA::isConnected()
{
	return false;
}

void ParserAMA::subscribe(const CodeSet & vecSymbols)
{
	std::cout << "subscribe" << std::endl;

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
		//上交所-现货快照数据
		if (!setSH.empty())
		{
			int SHsize = setSH.size();
			vector<SubscribeItem> subSHcode;

			const int batch(500);
			amd::ama::SubscribeItem * sub = new amd::ama::SubscribeItem[batch];

			for (auto& security_code : setSH)
			{
				SubscribeItem item;
				item.market = MarketType::kSSE;
				item.flag = SubscribeDataType::kSnapshot | SubscribeDataType::kTickExecution | SubscribeDataType::kTickOrder | SubscribeDataType::kOrderQueue;;
				strcpy(item.security_code, security_code.c_str());
				subSHcode.push_back(item);
				
			}
		/*	std::cout<< &subSHcode[0] <<std::endl;
			if (SubscribeData(SubscribeType::kSet, &subSHcode[0], SHsize) != amd::ama::ErrorCode::kSuccess)
			{
				std::lock_guard<std::mutex> _(g_mutex); std::cout << "Subscribe data failed" << std::endl;
				return;
			}
			else
			{
				write_log(m_parserSink, LL_INFO, "[Subscribe] Subscribe {} stocks from SZSE ", setSZ.size());
			}*/

			//CodeSet::iterator it = setSH.begin();
			//for (; it != setSH.end(); it++)
			//{
			//	SubscribeItem subSH[1];
			//	memset(subSH, 0, sizeof(subSH));
			//	subSH[1].market =MarketType::kSZSE;
			//	subSH[1].flag = SubscribeDataType::kSnapshot ;
			//	strcpy(subSH[1].security_code, (*it).c_str());
			//	SubscribeData(amd::ama::SubscribeType::kSet, subSH, 1);
		}
		//深交所-现货快照数据
		if (!setSZ.empty())
		{
			int SZsize = setSZ.size();
			vector<SubscribeItem> subSZcode;
			for (auto& security_code : setSZ)
			{
				SubscribeItem item;
				item.market = MarketType::kSZSE;
				item.flag = SubscribeDataType::kSnapshot | SubscribeDataType::kTickExecution | SubscribeDataType::kTickOrder | SubscribeDataType::kOrderQueue;;
				strcpy(item.security_code, security_code.c_str());
				subSZcode.push_back(item);
			}
		/*	if (SubscribeData(SubscribeType::kSet, &subSZcode[0], SZsize) != amd::ama::ErrorCode::kSuccess)
			{
				std::lock_guard<std::mutex> _(g_mutex);
				std::cout << "Subscribe data failed" << std::endl;
				return;
			}
			else
			{
				write_log(m_parserSink, LL_INFO, "[Subscribe] Subscribe {} stocks from SSE ", setSH.size());
			}*/

		}
	}
}

void ParserAMA::unsubscribe(const CodeSet & vecSymbols)
{

}

void ParserAMA::registerSpi(IParserSpi * listener)
{
	m_parserSink = listener;

	if (m_parserSink)
		m_pBaseDataMgr = m_parserSink->getBaseDataMgr();
}

void ParserAMA::OnEvent(uint32_t level, uint32_t code, const char * event_msg, uint32_t len)
{
	std::cout << "AMA event: " << event_msg << std::endl;
}





void ParserAMA::DoLogin()
{
	int32_t iResult = IAMDApi::Init(this, cfg);
	if (iResult != 0)
	{
		if (m_parserSink)
		{
			m_parserSink->handleEvent(WPE_Connect, iResult);
		/*
		kFailure = -100,                                    // 失败
        kInited,                                            // 已初始化
        kUnInited,                                          // 未初始化
        kNullSpi,                                           // 未设置输出数据类指针
        kParamIllegal,                                      // 参数非法
        kSuccess = 0,                                       // 成功
		*/
			write_log(m_parserSink, LL_ERROR, "[ParserAMA] Sending login request failed: {}", iResult);
		}
		std::cout << "Init AMA failed" << std::endl;
		disconnect();
	}
	else if (iResult == 0)
	{
		m_uTradingDate = TimeUtils::getCurDate();
		if (m_parserSink)
		{
			m_parserSink->handleEvent(WPE_Connect, 0);
			m_parserSink->handleEvent(WPE_Login, 0);
		}

		write_log(m_parserSink, LL_INFO, "[ParserAMA] Connecting to [{}] successed:{} , begin to subscibe data ...", cfg.ums_servers[0].server_ip, iResult);

		DoSubscribeMD();
	}
}
//void ParserAMA::OnLog(const int32_t& level,const char* log,uint32_t len)
//{
//
//	std::cout << "AMA log: " << "    level: " << level << "    log:   " << log << std::endl;
//}

enum
{
	kNone = 0,                ///< 表示全市场 
	kNEEQ = 2,                ///< 北交所
	kSHFE = 3,                ///< 上期所
	kCFFEX = 4,                ///< 中金所
	kDCE = 5,                ///< 大商所
	kCZCE = 6,                ///< 郑商所
	kINE = 7,                ///< 上期能源
	kSSE = 101,              ///< 上交所
	kSZSE = 102,              ///< 深交所
	kHKEx = 103,              /* < 港交所(港股通行情数据通过深交所和上交所的港股通获取, 市场类型为kSZSE/kSSE,
											商业港股行情对应的市场类型为kHKEx) */
	kCFETS = 104,              ///< 银行间市场
	kMax = 150               ///< 市场类型最大值
};

//接收现货快照数据回调
void ParserAMA::OnMDSnapshot(MDSnapshot * snapshot, uint32_t cnt)
{
	if (!snapshot)
	{
		return;
	}
	for (uint32_t i = 0; i < cnt; ++i)
	{
		//std::lock_guard<std::mutex> _(g_mutex);
		//std::cout << amd::ama::Tools::Serialize(snapshot[i]) << std::endl;
		const char* code = snapshot->security_code;
		const char* exchg = (snapshot->market_type == kSSE) ? "SSE" : "SZSE";
		WTSContractInfo* ct = m_pBaseDataMgr->getContract(code, exchg);
		if (ct == NULL)
			return;
		WTSCommodityInfo* commInfo = ct->getCommInfo();

		WTSTickData* tick = WTSTickData::create(code);
		tick->setContractInfo(ct);
		WTSTickStruct& quote = tick->getTickStruct();
		strcpy(quote.exchg, exchg);

		uint32_t act_date = (uint32_t)(snapshot->orig_time / 1000000000);
		quote.action_date = act_date;
		quote.action_time = snapshot->orig_time;
		quote.trading_date = act_date;

		quote.price = snapshot->last_price;// 最新价(类型:价格Price)
		quote.open = snapshot->open_price;
		quote.high = snapshot->high_price;
		quote.low = snapshot->low_price;
		quote.total_volume = (uint32_t)snapshot->total_volume_trade; // 成交总量(类型:数量Qty)
		quote.total_turnover = snapshot->total_value_trade;   // 成交总金额(类型:金额Amt)

		quote.pre_close = snapshot->pre_close_price;// 昨收价(类型:价格Price)

		quote.ask_prices[0] = checkValid(snapshot->offer_price[1]);
		quote.ask_qty[0] = snapshot->offer_volume[1];
		quote.ask_prices[1] = checkValid(snapshot->offer_price[2]);
		quote.ask_qty[1] = snapshot->offer_volume[2];
		quote.ask_prices[2] = checkValid(snapshot->offer_price[3]);
		quote.ask_qty[2] = snapshot->offer_volume[3];
		quote.ask_prices[3] = checkValid(snapshot->offer_price[4]);
		quote.ask_qty[3] = snapshot->offer_volume[4];
		quote.ask_prices[4] = checkValid(snapshot->offer_price[5]);
		quote.ask_qty[4] = snapshot->offer_volume[5];
		quote.ask_prices[5] = checkValid(snapshot->offer_price[6]);
		quote.ask_qty[5] = snapshot->offer_volume[6];
		quote.ask_prices[6] = checkValid(snapshot->offer_price[7]);
		quote.ask_qty[6] = snapshot->offer_volume[7];
		quote.ask_prices[7] = checkValid(snapshot->offer_price[8]);
		quote.ask_qty[7] = snapshot->offer_volume[8];
		quote.ask_prices[8] = checkValid(snapshot->offer_price[9]);
		quote.ask_qty[8] = snapshot->offer_volume[9];
		quote.ask_prices[9] = checkValid(snapshot->offer_price[10]);
		quote.ask_qty[9] = snapshot->offer_volume[10];

		quote.bid_prices[0] = checkValid(snapshot->bid_price[1]);
		quote.bid_qty[0] = snapshot->bid_volume[1];
		quote.bid_prices[1] = checkValid(snapshot->bid_price[2]);
		quote.bid_qty[1] = snapshot->bid_volume[2];
		quote.bid_prices[2] = checkValid(snapshot->bid_price[3]);
		quote.bid_qty[2] = snapshot->bid_volume[3];
		quote.bid_prices[3] = checkValid(snapshot->bid_price[4]);
		quote.bid_qty[3] = snapshot->bid_volume[4];
		quote.bid_prices[4] = checkValid(snapshot->bid_price[5]);
		quote.bid_qty[4] = snapshot->bid_volume[5];
		quote.bid_prices[5] = checkValid(snapshot->bid_price[6]);
		quote.bid_qty[5] = snapshot->bid_volume[6];
		quote.bid_prices[6] = checkValid(snapshot->bid_price[7]);
		quote.bid_qty[6] = snapshot->bid_volume[7];
		quote.bid_prices[7] = checkValid(snapshot->bid_price[8]);
		quote.bid_qty[7] = snapshot->bid_volume[8];
		quote.bid_prices[8] = checkValid(snapshot->bid_price[9]);
		quote.bid_qty[8] = snapshot->bid_volume[9];
		quote.bid_prices[9] = checkValid(snapshot->bid_price[10]);
		quote.bid_qty[9] = snapshot->bid_volume[10];

		if (m_parserSink)
			m_parserSink->handleQuote(tick, 1);

		////处理逐笔
		//if (snapshot->num_bid_orders > 0)//买入委托队列
		//{
		//	WTSOrdQueData* buyQue = WTSOrdQueData::create(code);
		//	buyQue->setContractInfo(ct);

		//	WTSOrdQueStruct& buyOS = buyQue->getOrdQueStruct();
		//	strcpy(buyOS.exchg, exchg);

		//	buyOS.trading_date = (uint32_t)act_date;
		//	buyOS.action_date = (uint32_t)act_date;
		//	buyOS.action_time = (uint32_t)snapshot->orig_time;

		//	buyOS.side = BDT_Buy;
		//	buyOS.price = quote.bid_prices[0];
		//	buyOS.order_items = 10;
		//	buyOS.qsize = min((int)buyOS.order_items, 50); // 最大只有50档的数据

		//	for (uint32_t i = 0; i < buyOS.qsize; i++)
		//	{
		//		buyOS.volumes[i] = (uint32_t)snapshot->num_bid_orders[i];
		//	}

		//	if (m_parserSink)
		//		m_parserSink->handleOrderQueue(buyQue);

		//	buyQue->release();
		//}
	}

	/* 手动释放数据内存, 否则会造成内存泄露 */
	amd::ama::IAMDApi::FreeMemory(snapshot);
}


//接收逐笔成交数据回调
void ParserAMA::OnMDTickExecution(amd::ama::MDTickExecution* tick, uint32_t cnt) 
{
	if (!tick)
	{
		return;
	}
	for (uint32_t i = 0; i < cnt; ++i)
	{
			//std::cout << amd::ama::Tools::Serialize(ticks[i]) << std::endl;
			const char* code = tick->security_code;
			const char* exchg = (tick->market_type == kSSE)? "SSE" : "SZSE";
			WTSContractInfo* ct = m_pBaseDataMgr->getContract(code, exchg);
			if (ct == NULL)
				return;
			WTSTransData* trans = WTSTransData::create(code);
			WTSTransStruct& ts = trans->getTransStruct();
			strcpy(ts.exchg, exchg);

			ts.trading_date = (uint32_t)(tick->exec_time / 1000000000);
			ts.action_date = ts.trading_date;
			ts.action_time = tick->exec_time;  // 精确到10ms，百分之一秒，143025060

			ts.price = tick->exec_price;
			ts.volume = tick->exec_volume;
			ts.askorder = (uint32_t)tick->bid_appl_seq_num;
			ts.bidorder = (uint32_t)tick->offer_appl_seq_num;
			ts.index = (uint32_t)tick->biz_index;  // 成交序号
			ts.side = tick->side;//买卖方向(仅上海有效 B-外盘,主动买 S-内盘,主动卖 N-未知)
			ts.ttype = tick->exec_type;// 成交类型(深圳: 4-撤销 F-成交, 上海: F-成交)

			if (m_parserSink)
				m_parserSink->handleTransaction(trans);

	}
	/* 手动释放数据内存, 否则会造成内存泄露 */
	FreeMemory(tick);
}

void ParserAMA::OnMDOrderBook(std::vector<amd::ama::MDOrderBook>& order_book)
{
	for (const auto& ob : order_book)
	{
		const char* code = ob.security_code;
		const char* exchg = (ob.market_type == kSSE) ? "SSE" : "SZSE";
		WTSContractInfo* ct = m_pBaseDataMgr->getContract(code, exchg);
		if (ct == NULL)
		{
			if (m_parserSink)
				write_log(m_parserSink, LL_ERROR, "[ParserAMA] Instrument {}.{} not exists...", exchg, code);
			continue;
		}
		if (ob.bid_order_book.size() > 0)
		{
			WTSOrdQueData* buyQue = WTSOrdQueData::create(code);
			buyQue->setContractInfo(ct);

			WTSOrdQueStruct& buyOS = buyQue->getOrdQueStruct();
			strcpy(buyOS.exchg, exchg);

			buyOS.trading_date = (uint32_t)(ob.last_tick_time / 1000000000);
			buyOS.action_date = buyOS.trading_date;
			buyOS.action_time = (uint32_t)ob.last_tick_time;

			buyOS.side = BDT_Buy;
			buyOS.price = ob.bid_order_book[0].price;
			buyOS.order_items = ob.bid_order_book[0].order_queue_size;  //// 委托队列大小   和 委托队列数量, 最多揭示50笔  ？

			buyOS.qsize = min((int)buyOS.order_items, 50); // 最大只有50档的数据

			for (uint32_t i = 0; i < buyOS.qsize; i++)
			{
				buyOS.volumes[i] = (uint32_t)ob.bid_order_book[0].order_queue_size;  //// 委托队列大小   和 委托队列数量, 最多揭示50笔  ？
			}

			if (m_parserSink)
				m_parserSink->handleOrderQueue(buyQue);

			buyQue->release();
		}

		if (ob.bid_order_book.size() > 0)
		{
			WTSOrdQueData* sellQue = WTSOrdQueData::create(code);
			sellQue->setContractInfo(ct);

			WTSOrdQueStruct& sellOS = sellQue->getOrdQueStruct();
			strcpy(sellOS.exchg, exchg);

			sellOS.trading_date = (uint32_t)(ob.last_tick_time / 1000000000);
			sellOS.action_date = sellOS.trading_date;
			sellOS.action_time = (uint32_t)ob.last_tick_time;

			sellOS.side = BDT_Sell;
			sellOS.price = ob.bid_order_book[0].price;
			sellOS.order_items = ob.bid_order_book[0].order_queue_size;  //// 委托队列大小   和 委托队列数量, 最多揭示50笔  ？

			sellOS.qsize = min((int)sellOS.order_items, 50); // 最大只有50档的数据

			for (uint32_t i = 0; i < 10; i++)
			{
				sellOS.volumes[i] = (uint32_t)ob.bid_order_book[0].order_queue_size;  //// 委托队列大小   和 委托队列数量, 最多揭示50笔  ？
			}

			if (m_parserSink)
				m_parserSink->handleOrderQueue(sellQue);

			sellQue->release();
		}

	}
}



//定义逐笔委托数据回调处理方法
void ParserAMA::OnMDTickOrder(amd::ama::MDTickOrder* ticks, uint32_t cnt)
{
	if (!ticks)
	{
		return;
	}	
	for (uint32_t i = 0; i < cnt; ++i)
	{
		//std::cout << amd::ama::Tools::Serialize(ticks[i]) << std::endl;
		const char* code = ticks->security_code;
		const char* exchg = (ticks->market_type == kSSE) ? "SSE" : "SZSE";
		WTSContractInfo* ct = m_pBaseDataMgr->getContract(code, exchg);
		if (ct == NULL)
			return;
		WTSOrdDtlData* order = WTSOrdDtlData::create(code);
		WTSOrdDtlStruct& ods = order->getOrdDtlStruct();
		strcpy(ods.exchg, exchg);

		ods.action_date = (uint32_t)(ticks->order_time / 1000000000);
		ods.trading_date = ods.action_date;
		ods.action_time = ticks->order_time;

		ods.side = ticks->side; //上海市场:(B:买单, S : 卖单)
		ods.price = ticks->order_price;
		ods.volume = ticks->order_volume;// 深圳市场:委托数量, 上海市场 : 剩余委托数量(类型 : 数量Qty)
		ods.index = (uint32_t)ticks->biz_index;
		ods.otype = ticks->order_type;  //深圳市场:(1-市价 2-限价 U-本方最优) 上海市场:(A:增加委托,D:删除委托)

		if (m_parserSink)
			m_parserSink->handleOrderDetail(order);

		order->release();
	}
		/* 手动释放数据内存, 否则会造成内存泄露 */
		FreeMemory(ticks);
}

void ParserAMA::DoSubscribeMD(bool isAll)
{ 
	std::cout<<"DoSubscribeMD"<<std::endl;
	//amd::ama::SubscribeItem sub[3];
	//memset(sub, 0, sizeof(sub));

	///* 订阅深交所全部证券的逐笔委托和逐笔成交数据 */
	//sub[0].market = amd::ama::MarketType::kSZSE;
	//sub[0].flag = amd::ama::SubscribeDataType::kTickOrder | amd::ama::SubscribeDataType::kTickExecution;
	//sub[0].security_code[0] = '\0';

	/* 订阅上交所600000、600004两只证券的全部数据 */
	//sub[1].market = amd::ama::MarketType::kSSE;
	//sub[1].flag = amd::ama::SubscribeDataType::kNone;
	//strcpy(sub[1].security_code, "600000");
	//sub[2].market = amd::ama::MarketType::kSSE;
	//sub[2].flag = amd::ama::SubscribeDataType::kNone;
	//strcpy(sub[2].security_code, "600004");
	//amd::ama::IAMDApi::SubscribeData(amd::ama::SubscribeType::kSet, sub, 3);
	///* 发起订阅 */
	//return ;
	CodeSet codeFilter = m_fitSHSubs;
	//上交所-现货快照数据
	if (!codeFilter.empty())
	{
			int SHsize = codeFilter.size();
			vector<SubscribeItem> subSHcode;
			for (auto& security_code : codeFilter)
			{
				SubscribeItem item;
				item.market = MarketType::kSSE;
				item.flag = SubscribeDataType::kSnapshot | SubscribeDataType::kTickExecution | SubscribeDataType::kTickOrder | SubscribeDataType::kOrderQueue;;
				strcpy(item.security_code, security_code.c_str());
				subSHcode.push_back(item);
			}
			if (SubscribeData(SubscribeType::kSet, &subSHcode[0], SHsize) != amd::ama::ErrorCode::kSuccess)
			{
				std::cout << "Subscribe data failed" << std::endl;
				return;
			}
			else
			{
				write_log(m_parserSink, LL_INFO, "[DoSubscribeMD] Subscribe {} stocks from SZSE ", codeFilter.size());
				codeFilter.clear();
			}

			//CodeSet::iterator it = setSH.begin();
			//for (; it != setSH.end(); it++)
			//{
			//	SubscribeItem subSH[1];
			//	memset(subSH, 0, sizeof(subSH));
			//	subSH[1].market =MarketType::kSZSE;
			//	subSH[1].flag = SubscribeDataType::kSnapshot ;
			//	strcpy(subSH[1].security_code, (*it).c_str());
			//	SubscribeData(amd::ama::SubscribeType::kSet, subSH, 1);
	}
	codeFilter=m_fitSZSubs;
	if (!codeFilter.empty())
	{
		int SZsize = codeFilter.size();
		vector<SubscribeItem> subSZcode;
		for (auto& security_code : codeFilter)
		{
			SubscribeItem item;
			item.market = MarketType::kSZSE;
			item.flag = SubscribeDataType::kSnapshot | SubscribeDataType::kTickExecution | SubscribeDataType::kTickOrder | SubscribeDataType::kOrderQueue;
			subSZcode.push_back(item);
		}
		if (SubscribeData(SubscribeType::kSet, &subSZcode[0], SZsize) != amd::ama::ErrorCode::kSuccess)
		{
			std::cout << "Subscribe data failed" << std::endl; 
			return ;
		}
		else
		{
			write_log(m_parserSink, LL_INFO, "[DoSubscribeMD] Subscribe {} stocks from SSE ", codeFilter.size());
			codeFilter.clear();
		}
	}
}




