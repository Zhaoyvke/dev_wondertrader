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
	static const uint64_t kRDMA = 0x00000001;               // RDMA ץ����ʽ��ȡ����
    static const uint64_t kEXA  = 0x00000002;               // EXA ץ����ʽ��ȡ����
    static const uint64_t kAMI  = 0x00000004;               // AMI �鲥��ʽ��ȡ����
    static const uint64_t kTCP  = 0x00000008;               // TCP ����ʽ��ȡ����
    static const uint64_t kPCAP = 0x00000010;               // Libpcap ץ����ʽ��ȡ����
    static const uint64_t kMDDP = 0x00000020;               // Mddp �鲥��ʽ��ȡ����
    static const uint64_t kFPGA = 0x00000040;               // FPGA ��ʽ��ȡ����
	*/
	

	cfg.channel_mode = config->getInt32("channel_mode");
	//TCP�������ݷ�ʽ: 0 ��ѹ�� 1 �����Զ���ѹ�� 2 zstdѹ��(��TCPģʽ��Ч)
	//cfg->tcp_compress_mode = config->getInt32("tcp_compress_mode");
	cfg.tcp_compress_mode = config->getInt32("tcp_compress_mode");
	/*
	ͨ���߿���ģʽ����
	1. cfg.channel_mode Ϊ��ͨ��ʱ,��������ֵΪkMasterSlaveA / kMasterSlaveB
	2. cfg.channel_mode ��Ͽ������ͨ��ʱ,�����������ò�ͬ��ֵ
		1) �����Ҫ���ͨ��Ϊ���ģʽ����,������kRegularDataFilterֵ
		2) �����Ҫ���ͨ����Ϊ�������룬������ֵΪkMasterSlaveA / kMasterSlaveB,kMasterSlaveA / kMasterSlaveB �����ο�ע��˵��
		   ͨ�����ȼ��Ӹߵ�������Ϊ kRDMA/kEXA/kMDDP/kAMI/kTCP/kPCAP
	    kMasterSlaveA    = 0,    // �����л�ģʽ��Aģʽ����ģʽ��ֻҪ�����ȼ�ͨ��������һֱʹ�ã���ʹ��ǰ�ڵ����ȼ�ʱҲ�����л������ȼ�
        kMasterSlaveB        = 1,   // �����л�ģʽ��Bģʽ����ģʽ�½���ÿ�γ����л�ʱѡ����õĸ����ȼ���ͨ��������ִ���л�����
        kRegularDataFilter   = 2     // �������ݹ���ģʽ����ģʽ�¶Ե������������е�����ֵ�����ˣ�������������и���ͨ���ź�������ִ�����ݹ���
	*/
	cfg.ha_mode = config->getInt32("ha_mode");
	/*
	    kInfo  = 1, ///< ��ͨ�¼�
        kWarn  = 2, ///< �澯�¼�
        kError = 3  ///< �����¼����Ƚ����أ���Ҫ���봦��*/

	cfg.min_log_level = config->getInt32("min_log_level");
	/*
	�����Ƿ�����������: true(��), false(��), �������ͨ��OnIndicator �ص���������
	������ݸ�ʽΪjson, ��Ҫ������ݰ���������Ϣ�����ݽ�������ͳ��
	������ͳ��:�������������ͳɹ��·�������ͳ��,���߲�ֵΪ���˵�������ͳ��
	eg: "RecvSnapshot": "5926", "SuccessSnapshot": "5925",��ʾ�����˿�������5926��,�ɹ��·�5925������������Ϊ 5926 - 5925 = 1 ��
		���˵������п���Ϊ�ظ����ݻ��߷Ƕ�������
	*/
	cfg.is_output_mon_data = config->getBoolean("is_output_mon_data");
	/*
	 ������ʱ��򿪹�: true(����������) , false(�رձ�����)
	 ��ҪУ����ʳɽ����ݺ����ί�������Ƿ��ж�ʧ,�����ʧ���и澯��־,����������ݲ��ȴ�keep_order_timeout_ms(��λms)ʱ��ȴ����������ش�,
	 ���������ʱ��,ֱ���·���������,Ĭ�������Ѿ���ʧ,���֮��ʧ���ݹ���Ҳ�ᶪ����
	 ͬʱ�������ں��Ϻ�����������ͨ�����������,������˱��򿪹�,���붩��ȫ��������������,����һ������Żᱻ���Ĺ���,�������ݳ�ʱ�ȴ��Լ���ʧ�澯��
	*/
	cfg.keep_order = config->getBoolean("keep_order");
	cfg.keep_order_timeout_ms = config->getInt32("keep_order_timeout_ms");
	//����Ĭ�϶���: true ����Ĭ��ȫ������, false ����Ĭ��ȫ��������
	cfg.is_subscribe_full = config->getBoolean("is_subscribe_full");
	/*
	����UMS��Ϣ:
	username/password �˻���/����, һ���˻�ֻ�ܱ���һ�����ӽ��루ע��: �����Ҫʹ��ί�в����ܣ�ע���˺���Ҫ��ί�в�����Ȩ�ޣ�
	ums��ַ����:
		1) ums��ַ��������1-8�� ����ֵΪ2 ��Ϊ����, ums_server_cnt Ϊ�˴�����UMS��ַ�ĸ���
		2) ums_servers ΪUMS��ַ��Ϣ���ݽṹ:
			local_ip Ϊ���ص�ַ,��0.0.0.0 ���߱���ip
			server_ip Ϊums����˵�ַ
			server_port Ϊums����˶˿�
	*/
	strcpy(cfg.username, config->getCString("user"));
	strcpy(cfg.password, config->getCString("password"));
	cfg.ums_server_cnt = config->getInt32("ums_server_cnt");
	strcpy(cfg.ums_servers[0].local_ip, config->getCString("local_ip"));
	strcpy(cfg.ums_servers[0].server_ip, config->getCString("server_ip"));
	cfg.ums_servers[0].server_port = config->getInt32("server_port");
	cfg.is_thread_safe = config->getBoolean("is_thread_safe");
	/*
	ί�в�ǰ�������������(��ί�в��汾API��Ч������ί�в��汾����������Ч):
		1) ����������ã�������λ�����Լ�ÿ����Ӧ��ί�ж�������
		2��ί�в���������������ã������߳������Լ��ݽ��������
	 * @brief ί�в��������Ͷ���
	static const uint8_t kNone = 0;   // ������ί�в�
	static const uint8_t kLocalOrderBook = 1;   // ���ع���ί�в�,����������ͬ����ʼ״̬(��Ҫ������Ӧ����ʳɽ������ί��)
	static const uint8_t kServerOrderBook = 2;   // �����Զ��TCP��ʽ����ί�в�����
	*/
	cfg.enable_order_book = config->getInt32("enable_order_book");
	cfg.entry_size = config->getInt32("entry_size");//ί�в�������λ����(�ݽ���ί�в����ݵ�λС�ڵ���entry_size)
	cfg.order_queue_size = config->getInt32("order_queue_size");//ÿ����λ�����ί�ж��н�ʾ(�������50)
	cfg.thread_num = config->getInt32("thread_num");//����ί�в����߳�����(�ݽ��͹���ÿһ���г��ֱ���thread_num���߳�)
	 /*�ݽ�����Сʱ����(��λ:΢��, ί�в��ݽ�������ڵ���order_book_deliver_interval_microsecond)����Ϊ0ʱ, ί�в�ʵʱ�·�, ����ȴ����.*/
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
		//�Ͻ���-�ֻ���������
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
		//���-�ֻ���������
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
		kFailure = -100,                                    // ʧ��
        kInited,                                            // �ѳ�ʼ��
        kUnInited,                                          // δ��ʼ��
        kNullSpi,                                           // δ�������������ָ��
        kParamIllegal,                                      // �����Ƿ�
        kSuccess = 0,                                       // �ɹ�
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
	kNone = 0,                ///< ��ʾȫ�г� 
	kNEEQ = 2,                ///< ������
	kSHFE = 3,                ///< ������
	kCFFEX = 4,                ///< �н���
	kDCE = 5,                ///< ������
	kCZCE = 6,                ///< ֣����
	kINE = 7,                ///< ������Դ
	kSSE = 101,              ///< �Ͻ���
	kSZSE = 102,              ///< ���
	kHKEx = 103,              /* < �۽���(�۹�ͨ��������ͨ��������Ͻ����ĸ۹�ͨ��ȡ, �г�����ΪkSZSE/kSSE,
											��ҵ�۹������Ӧ���г�����ΪkHKEx) */
	kCFETS = 104,              ///< ���м��г�
	kMax = 150               ///< �г��������ֵ
};

//�����ֻ��������ݻص�
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

		quote.price = snapshot->last_price;// ���¼�(����:�۸�Price)
		quote.open = snapshot->open_price;
		quote.high = snapshot->high_price;
		quote.low = snapshot->low_price;
		quote.total_volume = (uint32_t)snapshot->total_volume_trade; // �ɽ�����(����:����Qty)
		quote.total_turnover = snapshot->total_value_trade;   // �ɽ��ܽ��(����:���Amt)

		quote.pre_close = snapshot->pre_close_price;// ���ռ�(����:�۸�Price)

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

		////�������
		//if (snapshot->num_bid_orders > 0)//����ί�ж���
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
		//	buyOS.qsize = min((int)buyOS.order_items, 50); // ���ֻ��50��������

		//	for (uint32_t i = 0; i < buyOS.qsize; i++)
		//	{
		//		buyOS.volumes[i] = (uint32_t)snapshot->num_bid_orders[i];
		//	}

		//	if (m_parserSink)
		//		m_parserSink->handleOrderQueue(buyQue);

		//	buyQue->release();
		//}
	}

	/* �ֶ��ͷ������ڴ�, ���������ڴ�й¶ */
	amd::ama::IAMDApi::FreeMemory(snapshot);
}


//������ʳɽ����ݻص�
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
			ts.action_time = tick->exec_time;  // ��ȷ��10ms���ٷ�֮һ�룬143025060

			ts.price = tick->exec_price;
			ts.volume = tick->exec_volume;
			ts.askorder = (uint32_t)tick->bid_appl_seq_num;
			ts.bidorder = (uint32_t)tick->offer_appl_seq_num;
			ts.index = (uint32_t)tick->biz_index;  // �ɽ����
			ts.side = tick->side;//��������(���Ϻ���Ч B-����,������ S-����,������ N-δ֪)
			ts.ttype = tick->exec_type;// �ɽ�����(����: 4-���� F-�ɽ�, �Ϻ�: F-�ɽ�)

			if (m_parserSink)
				m_parserSink->handleTransaction(trans);

	}
	/* �ֶ��ͷ������ڴ�, ���������ڴ�й¶ */
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
			buyOS.order_items = ob.bid_order_book[0].order_queue_size;  //// ί�ж��д�С   �� ί�ж�������, ����ʾ50��  ��

			buyOS.qsize = min((int)buyOS.order_items, 50); // ���ֻ��50��������

			for (uint32_t i = 0; i < buyOS.qsize; i++)
			{
				buyOS.volumes[i] = (uint32_t)ob.bid_order_book[0].order_queue_size;  //// ί�ж��д�С   �� ί�ж�������, ����ʾ50��  ��
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
			sellOS.order_items = ob.bid_order_book[0].order_queue_size;  //// ί�ж��д�С   �� ί�ж�������, ����ʾ50��  ��

			sellOS.qsize = min((int)sellOS.order_items, 50); // ���ֻ��50��������

			for (uint32_t i = 0; i < 10; i++)
			{
				sellOS.volumes[i] = (uint32_t)ob.bid_order_book[0].order_queue_size;  //// ί�ж��д�С   �� ί�ж�������, ����ʾ50��  ��
			}

			if (m_parserSink)
				m_parserSink->handleOrderQueue(sellQue);

			sellQue->release();
		}

	}
}



//�������ί�����ݻص�������
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

		ods.side = ticks->side; //�Ϻ��г�:(B:��, S : ����)
		ods.price = ticks->order_price;
		ods.volume = ticks->order_volume;// �����г�:ί������, �Ϻ��г� : ʣ��ί������(���� : ����Qty)
		ods.index = (uint32_t)ticks->biz_index;
		ods.otype = ticks->order_type;  //�����г�:(1-�м� 2-�޼� U-��������) �Ϻ��г�:(A:����ί��,D:ɾ��ί��)

		if (m_parserSink)
			m_parserSink->handleOrderDetail(order);

		order->release();
	}
		/* �ֶ��ͷ������ڴ�, ���������ڴ�й¶ */
		FreeMemory(ticks);
}

void ParserAMA::DoSubscribeMD(bool isAll)
{ 
	std::cout<<"DoSubscribeMD"<<std::endl;
	//amd::ama::SubscribeItem sub[3];
	//memset(sub, 0, sizeof(sub));

	///* �������ȫ��֤ȯ�����ί�к���ʳɽ����� */
	//sub[0].market = amd::ama::MarketType::kSZSE;
	//sub[0].flag = amd::ama::SubscribeDataType::kTickOrder | amd::ama::SubscribeDataType::kTickExecution;
	//sub[0].security_code[0] = '\0';

	/* �����Ͻ���600000��600004��ֻ֤ȯ��ȫ������ */
	//sub[1].market = amd::ama::MarketType::kSSE;
	//sub[1].flag = amd::ama::SubscribeDataType::kNone;
	//strcpy(sub[1].security_code, "600000");
	//sub[2].market = amd::ama::MarketType::kSSE;
	//sub[2].flag = amd::ama::SubscribeDataType::kNone;
	//strcpy(sub[2].security_code, "600004");
	//amd::ama::IAMDApi::SubscribeData(amd::ama::SubscribeType::kSet, sub, 3);
	///* ������ */
	//return ;
	CodeSet codeFilter = m_fitSHSubs;
	//�Ͻ���-�ֻ���������
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




