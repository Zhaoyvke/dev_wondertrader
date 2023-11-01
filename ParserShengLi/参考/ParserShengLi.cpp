/*!
 * \file ParserShengLi.cpp
 * \project	WonderTrader
 *
 * \author Sunseeeeeker
 * \date 2023/08/03
 *
 * \brief
 */
#include "ParserShengLi.h"
#include "../Includes/WTSVariant.hpp"
#include "../Includes/WTSDataDef.hpp"
#include "../Includes/IBaseDataMgr.h"
#include "../Includes/WTSContractInfo.hpp"
#include "../Share/decimal.h"
#include "../Share/ModuleHelper.hpp"

#include <boost/filesystem.hpp>
#include <boost/bind.hpp>

#define	 SZE_QUOTE_PARAM_NUM	(9)
#define	 SSE_QUOTE_PARAM_NUM	(10)

#ifndef _WIN32
#include <dlfcn.h>
#include <arpa/inet.h>
#endif

#ifdef _WIN32
#pragma comment(lib, "../API/ShengLiFPGA_API_SZE3.2_SSE4.3_25436/x64/efh_lev2_api.lib")
#endif


 //By Wesley @ 2022.01.05
#include "../Share/fmtlib.h"
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

template<typename T>
inline double checkValid(const T& val, const int divider)
{
	//assert(divider > 0);

	double ret = (double)val / (double)divider;
	return (ret == DBL_MAX) ? 0.0 : ret;
}

extern "C"
{
	EXPORT_FLAG IParserApi* createParser()
	{
		ParserShengLi* parser = new ParserShengLi();
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

inline uint32_t get_sse_date(sse_hpf_head& m_head)
{
	return (uint32_t)m_head.m_quote_date_year * 10000 + (uint32_t)m_head.m_quote_date_month * 100 + (uint32_t)m_head.m_quote_date_day;
}

ParserShengLi::ParserShengLi()
	: _stopped(false)
	, _sink(NULL)
	, _prepared(false)
	, _sse_api(NULL)
	, _sze_api(NULL)
	, _m_h_core(NULL)
	, _sse_config(NULL)
	, _szse_config(NULL)
{
	//#ifdef _WIN32
	//	_m_h_core = NULL;
	//#else
	//	_m_h_core = NULL;
	//#endif
}


ParserShengLi::~ParserShengLi()
{
	if (_sse_config)
		_sse_config->release();

	if (_szse_config)
		_szse_config->release();

	if (_sze_api)
	{
		_sze_api->stop_sze();
#ifdef _WIN32
		func_destroy_efh_sze_lev2_api func_destroy = (func_destroy_efh_sze_lev2_api)GetProcAddress(_m_h_core, DESTROY_EFH_SZE_LEV2_API_FUNCTION);
#else
		func_destroy_efh_sze_lev2_api func_destroy = (func_destroy_efh_sze_lev2_api)dlsym(_m_h_core, DESTROY_EFH_SZE_LEV2_API_FUNCTION);
#endif

		if (func_destroy != NULL)
		{
			func_destroy(_sze_api);
		}
	}

	if (_sse_api)
	{
		_sse_api->stop_sse();
#ifdef _WIN32
		func_destroy_efh_sse_lev2_api func_destroy = (func_destroy_efh_sse_lev2_api)GetProcAddress(_m_h_core, DESTROY_EFH_SSE_LEV2_API_FUNCTION);
#else
		func_destroy_efh_sse_lev2_api func_destroy = (func_destroy_efh_sse_lev2_api)dlsym(_m_h_core, DESTROY_EFH_SSE_LEV2_API_FUNCTION);
#endif

		if (func_destroy != NULL)
		{
			func_destroy(_sse_api);
		}
	}

#ifdef _WIN32
	FreeLibrary(_m_h_core);
#else
	dlclose(_m_h_core);
#endif // _WIN32
}

bool ParserShengLi::init(WTSVariant* config)
{
	_nic_type = config->get("NIC_TYPE")->getInt32("nic_type");

	_sse_config = config->get("SSE_CONFIG");
	_szse_config = config->get("SZSE_CONFIG");

	std::string module = config->getCString("module");
	if (module.empty())
		module = "efh_lev2_api";

	//std::string path = StrUtil::printf("%s/%s/", m_strFlowDir.c_str(), m_strUser.c_str());
	//boost::filesystem::create_directories(path.c_str());

#ifdef _WIN32
	std::string dllpath = getBinDir() + DLLHelper::wrap_module(module.c_str(), "");
	_m_h_core = DLLHelper::load_library(dllpath.c_str());
	//_m_h_core = LoadLibraryA(DLL_EFH_LEV2_DLL_NAME);

	if (_m_h_core == NULL)
	{
		write_log(_sink, LL_ERROR, "[ParserShengLi] Load module {} failed !", DLL_EFH_LEV2_DLL_NAME);
		return false;
	}

	func_create_efh_sse_lev2_api func_sse_create = (func_create_efh_sse_lev2_api)GetProcAddress(_m_h_core, CREATE_EFH_SSE_LEV2_API_FUNCTION);
	if (func_sse_create == NULL)
	{
		write_log(_sink, LL_ERROR, "[ParserShengLi] Get create sqs function ptr failed !");
		return false;
	}

	func_create_efh_sze_lev2_api func_sze_create = (func_create_efh_sze_lev2_api)GetProcAddress(_m_h_core, CREATE_EFH_SZE_LEV2_API_FUNCTION);
	if (func_sse_create == NULL)
	{
		write_log(_sink, LL_ERROR, "[ParserShengLi] Get create sqs function ptr failed !");
		return false;
	}
#else
	module = "sl_efh_lev2_api";
	std::string dllpath = getBinDir() + DLLHelper::wrap_module(module.c_str(), "lib");
	_m_h_core = DLLHelper::load_library(dllpath.c_str());
	//_m_h_core = dlopen(DLL_EFH_LEV2_DLL_NAME, RTLD_LAZY);

	if (_m_h_core == NULL)
	{
		write_log(_sink, LL_ERROR, "[ParserShengLi] Load module {} failed !", DLL_EFH_LEV2_DLL_NAME);
		return false;
	}

	func_create_efh_sse_lev2_api func_sse_create = (func_create_efh_sse_lev2_api)dlsym(_m_h_core, CREATE_EFH_SSE_LEV2_API_FUNCTION);
	if (func_sse_create == NULL)
	{
		write_log(_sink, LL_ERROR, "[ParserShengLi] Get create sse sqs function ptr failed !");
		return false;
	}

	func_create_efh_sze_lev2_api func_sze_create = (func_create_efh_sze_lev2_api)dlsym(_m_h_core, CREATE_EFH_SZE_LEV2_API_FUNCTION);
	if (func_sse_create == NULL)
	{
		write_log(_sink, LL_ERROR, "[ParserShengLi] Get create szse sqs function ptr failed !");
		return false;
	}
#endif

	_sse_api = func_sse_create();
	_sze_api = func_sze_create();

	if (_sse_api == NULL || _sze_api == NULL)
	{
		write_log(_sink, LL_ERROR, "[ParserShengLi] Create trading function ptr failed!");
		return false;
	}

	socket_udp_param  sse_params[SSE_QUOTE_PARAM_NUM];
	int num = 0;
	{
		if (config_sock_udp_param(sse_params[num], "EFH_SSE_LEV2_IDX", true))
		{
			sse_params[num].m_efh_type = enum_efh_sse_lev2_idx;
			num++;
		}

		if (config_sock_udp_param(sse_params[num], "EFH_SSE_LEV2_TICK", true))
		{
			sse_params[num].m_efh_type = enum_efh_sse_lev2_tick;
			num++;
		}

		if (config_sock_udp_param(sse_params[num], "EFH_SSE_LEV2_SNAP", true))
		{
			sse_params[num].m_efh_type = enum_efh_sse_lev2_snap;
			num++;
		}

		if (config_sock_udp_param(sse_params[num], "EFH_SSE_LEV2_OPTION", true))
		{
			sse_params[num].m_efh_type = enum_efh_sse_lev2_opt;
			num++;
		}

		if (config_sock_udp_param(sse_params[num], "EFH_SSE_LEV2_TREE", true))
		{
			sse_params[num].m_efh_type = enum_efh_sse_lev2_tree;
			num++;
		}

		if (config_sock_udp_param(sse_params[num], "EFH_SSE_LEV2_BOND_SNAP", true))
		{
			sse_params[num].m_efh_type = enum_efh_sse_lev2_bond_snap;
			num++;
		}

		if (config_sock_udp_param(sse_params[num], "EFH_SSE_LEV2_BOND_TICK", true))
		{
			sse_params[num].m_efh_type = enum_efh_sse_lev2_bond_tick;
			num++;
		}

		if (config_sock_udp_param(sse_params[num], "EFH_SSE_LEV2_ETF", true))
		{
			sse_params[num].m_efh_type = enum_efh_sse_lev2_etf;
			num++;
		}

		if (config_sock_udp_param(sse_params[num], "EFH_SSE_LEV2_TICK_MERGE", true))
		{
			sse_params[num].m_efh_type = enum_efh_sse_lev2_tick_merge;
			num++;
		}
	}

	if (!_sse_api->init_sse(sse_params, num, static_cast<efh_sse_lev2_api_event*>(this), static_cast<efh_sse_lev2_api_depend*>(this)))
	{
		write_log(_sink, LL_ERROR, "[ParserShengLi] Init SSE parser api failed !");
		return false;
	}

	socket_udp_param szse_params[SZE_QUOTE_PARAM_NUM];
	num = 0;
	{
		if (config_sock_udp_param(szse_params[num], "EFH_SZE_LEV2_TICK", false))
		{
			szse_params[num].m_efh_type = enum_efh_sze_lev2_tick;
			num++;
		}

		if (config_sock_udp_param(szse_params[num], "EFH_SZE_LEV2_IDX", false))
		{
			szse_params[num].m_efh_type = enum_efh_sze_lev2_idx;
			num++;
		}

		if (config_sock_udp_param(szse_params[num], "EFH_SZE_LEV2_SNAP", false))
		{
			szse_params[num].m_efh_type = enum_efh_sze_lev2_snap;
			num++;
		}

		if (config_sock_udp_param(szse_params[num], "EFH_SZE_LEV2_TREE", false))
		{
			szse_params[num].m_efh_type = enum_efh_sze_lev2_tree;
			num++;
		}

		if (config_sock_udp_param(szse_params[num], "EFH_SZE_LEV2_TURNOVER", false))
		{
			szse_params[num].m_efh_type = enum_efh_sze_lev2_turnover;
			num++;
		}

		if (config_sock_udp_param(szse_params[num], "EFH_SZE_LEV2_BOND_SNAP", false))
		{
			szse_params[num].m_efh_type = enum_efh_sze_lev2_bond_snap;
			num++;
		}

		if (config_sock_udp_param(szse_params[num], "EFH_SZE_LEV2_BOND_TICK", false))
		{
			szse_params[num].m_efh_type = enum_efh_sze_lev2_bond_tick;
			num++;
		}

		if (config_sock_udp_param(szse_params[num], "EFH_SZE_LEV2_IBR_TREE", false))
		{
			szse_params[num].m_efh_type = enum_efh_sze_lev2_ibr_tree;
			num++;
		}
	}

	if (!_sze_api->init_sze(szse_params, num, static_cast<efh_sze_lev2_api_event*>(this), static_cast<efh_sze_lev2_api_depend*>(this)))
	{
		write_log(_sink, LL_ERROR, "[ParserShengLi] Init SZSE parser API failed !");
		return false;
	}

	return true;
}

bool ParserShengLi::config_sock_udp_param(socket_udp_param& quote_param, const char* section, bool is_sh)
{
	WTSVariant* config = NULL;

	if (is_sh)
		config = _sse_config->get(section);
	else
		config = _szse_config->get(section);

	if (config == NULL)
		return false;

	if (config->getInt32("enable") == 1)
	{
		quote_param.m_i_cpu_id = config->getInt32("cpu_id");

		memset(quote_param.m_ch_src_ip, 0, sizeof(quote_param.m_ch_src_ip));
		//memcpy_s(quote_param.m_ch_src_ip, sizeof(quote_param.m_ch_src_ip) / sizeof(quote_param.m_ch_src_ip[0]), config->getCString("multicast_ip"), sizeof(config->getCString("multicast_ip")) / sizeof(char));
		memcpy(quote_param.m_ch_src_ip, config->getCString("multicast_ip"), IP_LEN);

		quote_param.m_i_src_port = config->getInt32("multicast_port");

		memset(quote_param.m_ch_dest_ip, 0, sizeof(quote_param.m_ch_dest_ip));
		//memcpy_s(quote_param.m_ch_dest_ip, sizeof(quote_param.m_ch_dest_ip) / sizeof(quote_param.m_ch_dest_ip[0]), config->getCString("data_ip"), sizeof(config->getCString("data_ip")) / sizeof(char));
		memcpy(quote_param.m_ch_dest_ip, config->getCString("data_ip"), IP_LEN);

		quote_param.m_i_dest_port = config->getInt32("data_port");

		memset(quote_param.m_ch_eth_name, 0, sizeof(quote_param.m_ch_eth_name));
		//memcpy_s(quote_param.m_ch_eth_name, sizeof(quote_param.m_ch_eth_name) / sizeof(quote_param.m_ch_eth_name[0]), config->getCString("eth_name"), sizeof(config->getCString("eth_name")) / sizeof(char));
		memcpy(quote_param.m_ch_eth_name, config->getCString("eth_name"), ETH_NAME_LEN);

		quote_param.m_ll_cache_size = config->getUInt64("cache_size") * 1024ul * 1024ul;
		quote_param.m_b_force_normal_socket = (config->getInt32("force_normal_socket") != 0);
		quote_param.m_ll_proc_data_wait_time = config->getUInt64("proc_data_wait_time");
		quote_param.m_ll_normal_socket_rxbuf = config->getUInt64("normal_socket_rxbuf") * 1024ul * 1024ul;
		quote_param.m_b_out_of_order_correction = (config->getInt32("out_of_order_correction") != 0);

		return true;
	}
	else
		return false;
}

void ParserShengLi::release()
{
	disconnect();
}

bool ParserShengLi::prepare()
{
	switch (_nic_type)
	{
	case enum_nic_normal:
	case enum_nic_solarflare_efvi:
	case enum_nic_exablaze_exanic:
	case enum_nic_x710_win_speed:
	case enum_nic_solarflare_win_speed:
	{
		if (!_sze_api->start_sze(enum_efh_nic_type(_nic_type)))
		{
			write_log(_sink, LL_ERROR, "[ParserShengLi] Start SZSE parser failed, nic type: {}", _nic_type);
			return false;
		}

		if (!_sse_api->start_sse(enum_efh_nic_type(_nic_type)))
		{
			write_log(_sink, LL_ERROR, "[ParserShengLi] Start SSE parser failed, nic type: {}", _nic_type);
			return false;
		}
	}
	break;
	default:
	{
		write_log(_sink, LL_ERROR, "[ParserShengLi] Unsupported NIC type ==> {}", _nic_type);
		return false;
	}
	break;
	}

	return true;
}

void ParserShengLi::DoLogin()
{
	_prepared = prepare();

	if (!_prepared)
	{
		if (_sink)
		{
			_sink->handleEvent(WPE_Connect, -1);
			write_log(_sink, LL_ERROR, "[ParserShengLi] Sending login request failed !");
		}
	}
	else
	{
		_trading_date = TimeUtils::getCurDate();
		if (_sink)
		{
			_sink->handleEvent(WPE_Connect, 0);
			_sink->handleEvent(WPE_Login, 0);
		}

		write_log(_sink, LL_INFO, "[ParserShengLi] Connecting to success, begin to subscribe data, trading date: {}", _trading_date);

		DoSubscribeMD();
	}
}

bool ParserShengLi::connect()
{
	DoLogin();

	return true;
}

bool ParserShengLi::disconnect()
{
	_stopped = true;

	if (_sse_config)
		_sse_config->release();

	if (_szse_config)
		_szse_config->release();

	if (_sze_api)
	{
		_sze_api->stop_sze();
#ifdef _WIN32
		func_destroy_efh_sze_lev2_api func_destroy = (func_destroy_efh_sze_lev2_api)GetProcAddress(_m_h_core, DESTROY_EFH_SZE_LEV2_API_FUNCTION);
#else
		func_destroy_efh_sze_lev2_api func_destroy = (func_destroy_efh_sze_lev2_api)dlsym(_m_h_core, DESTROY_EFH_SZE_LEV2_API_FUNCTION);
#endif

		if (func_destroy != NULL)
		{
			func_destroy(_sze_api);
		}
	}

	if (_sse_api)
	{
		_sse_api->stop_sse();
#ifdef _WIN32
		func_destroy_efh_sse_lev2_api func_destroy = (func_destroy_efh_sse_lev2_api)GetProcAddress(_m_h_core, DESTROY_EFH_SSE_LEV2_API_FUNCTION);
#else
		func_destroy_efh_sse_lev2_api func_destroy = (func_destroy_efh_sse_lev2_api)dlsym(_m_h_core, DESTROY_EFH_SSE_LEV2_API_FUNCTION);
#endif

		if (func_destroy != NULL)
		{
			func_destroy(_sse_api);
		}
	}

#ifdef _WIN32
	FreeLibrary(_m_h_core);
#else
	dlclose(_m_h_core);
#endif // _WIN32

	return true;
}

bool ParserShengLi::isConnected()
{
	return _sse_api != NULL && _sze_api != NULL;
}

void ParserShengLi::DoSubscribeMD()
{
	//WTSArray* ay = _bd_mgr->getContracts();

	SubscribeMDs();
}

void ParserShengLi::SubscribeMDs(bool isAll /* = false */)
{
	CodeSet codes;

	CodeSet codeFilter = _set_subs;
	if (codeFilter.empty())
	{//如果订阅礼包只空的,则取出全部合约列表
		return;
	}

	if (!isAll)
	{
		CodeSet codeFilter = _set_subs;
		if (codeFilter.empty())
		{//如果订阅空的,则取出全部合约列表
			return;
		}

		_seq_map.clear();
		//std::string code, exchg;
		for (const auto& fullCode : codeFilter)
		{
			// 新版去掉了LongKey类，直接用std::string
			//auto ay = StrUtil::split(fullCode.c_str(), ".");
			//if (ay.size() < 2)
			//	continue;

			//code = ay[1];
			//exchg = ay[0];

			//code += ".";
			//code += exchg;

			//if (!_seq_map.contains(fullCode))
			//{
			//	SeqOrderNum _code_seq;
			//	_seq_map.insert(std::pair<std::string, SeqOrderNum>(fullCode, _code_seq));
			//	//_seq_map_[code] = _code_seq;
			//}
			
			SeqOrderNum code_seq;
			_seq_map[fullCode] = code_seq;
		}
	}

	write_log(_sink, LL_DEBUG, "[ParserShengLi] Subscribe {} contracts ...", _seq_map.size());
}

void ParserShengLi::subscribe(const CodeSet &vecSymbols)
{
	auto cit = vecSymbols.begin();
	for (; cit != vecSymbols.end(); cit++)
	{
		const auto &code = *cit;
		if (_set_subs.find(code) == _set_subs.end())
		{
			_set_subs.insert(code);
		}
	}
}

void ParserShengLi::unsubscribe(const CodeSet &setSymbols)
{

}

void ParserShengLi::registerSpi(IParserSpi* listener)
{
	bool bReplaced = (_sink != NULL);
	_sink = listener;
	if (bReplaced && _sink)
	{
		write_log(_sink, LL_DEBUG, "[ParserShengLi] Listener is replaced");
	}

	if (_sink)
		_bd_mgr = _sink->getBaseDataMgr();
}

void ParserShengLi::doOnDisconnected()
{
	if (_sink)
	{
		_sink->handleEvent(WPE_Close, 0);
	}
}

void ParserShengLi::on_report_efh_sze_lev2_snap(sze_hpf_lev2* p_snap)
{
	if (p_snap)
	{
		const char* code = (char*)p_snap->m_head.m_symbol;
		const char* exchg = "SZSE";

		//TODO: 此处过滤非订阅合约

		WTSContractInfo* ct = _bd_mgr->getContract(code, exchg);
		if (ct == NULL)
		{
			if (_sink)
				write_log(_sink, LL_ERROR, "[ParserInsight] Instrument {}.{} not exists...", exchg, code);
			return;
		}
		WTSCommodityInfo* commInfo = ct->getCommInfo();

		WTSTickData* tick = WTSTickData::create(code);
		tick->setContractInfo(ct);
		WTSTickStruct& quote = tick->getTickStruct();
		strcpy(quote.exchg, exchg);

		uint32_t actDate = (uint32_t)(p_snap->m_head.m_quote_update_time / 1000000000);
		uint32_t actTime = (uint32_t)(p_snap->m_head.m_quote_update_time % 1000000000);

		const int _DIVIDER = 10000;
		const int _DIVIDER_V = 100;
		const int _DIVIDER_T = 1000000;

		quote.price = checkValid(p_snap->m_last_price, _DIVIDER);
		quote.open = checkValid(p_snap->m_open_price, _DIVIDER);
		quote.high = checkValid(p_snap->m_day_high_price, _DIVIDER);
		quote.low = checkValid(p_snap->m_day_low_price, _DIVIDER);

		quote.total_volume = checkValid(p_snap->m_total_quantity, _DIVIDER_V);
		quote.trading_date = _trading_date;
		quote.total_turnover = checkValid(p_snap->m_total_value, _DIVIDER_T);

		quote.upper_limit = checkValid(p_snap->m_upper_limit_price, _DIVIDER);
		quote.lower_limit = checkValid(p_snap->m_low_limit_price, _DIVIDER);

		quote.pre_close = checkValid(p_snap->m_pre_close_price, _DIVIDER);

		if (p_snap->m_head.m_message_type == '6')
			quote.open_interest = checkValid(p_snap->m_open_interest, _DIVIDER);

		//委卖价格
		for (int i = 0; i < SZE_LEV2_DEPTH; i++)
		{
			quote.ask_prices[i] = checkValid(p_snap->m_ask_unit[i].m_price, _DIVIDER);
			quote.ask_qty[i] = checkValid(p_snap->m_ask_unit[i].m_quantity, _DIVIDER_V);

			quote.bid_prices[i] = checkValid(p_snap->m_bid_unit[i].m_price, _DIVIDER);
			quote.bid_qty[i] = checkValid(p_snap->m_bid_unit[i].m_quantity, _DIVIDER_V);
		}

		write_log(_sink, LL_DEBUG, "[on_report_efh_sze_lev2_snap] code: {}, exchg: {}, open: {}, price: {}, ask_px1: {}, ask_qty1: {}, bid_px1: {}, bid_qty1: {}",
			code, exchg, quote.open, quote.price, quote.ask_prices[0], quote.ask_qty[0], quote.bid_prices[0], quote.bid_qty[0]);

		if (_sink)
			_sink->handleQuote(tick, 1);

		tick->release();
	}
}

void ParserShengLi::on_report_efh_sze_lev2_tick(int msg_type, sze_hpf_order* p_order, sze_hpf_exe* p_exe)
{
	if (msg_type == SZE_LEV2_ORDER_MSG_TYPE)
	{
		// 逐笔委托
		if (p_order)
		{
			const char* code = (char*)p_order->m_head.m_symbol;
			const char* exchg = "SZSE";

			WTSContractInfo* ct = _bd_mgr->getContract(code, exchg);
			if (ct == nullptr)
				return;

			WTSCommodityInfo* commInfo = ct->getCommInfo();

			WTSOrdDtlData *ordDtl = WTSOrdDtlData::create(code);
			WTSOrdDtlStruct& ts = ordDtl->getOrdDtlStruct();
			strcpy(ts.exchg, exchg);

			uint32_t actDate = (uint32_t)(p_order->m_head.m_quote_update_time / 1000000000);
			uint32_t actTime = (uint32_t)(p_order->m_head.m_quote_update_time % 1000000000);

			ts.trading_date = _trading_date;
			ts.action_date = actDate;
			ts.action_time = actTime;

			ts.index = p_order->m_head.m_sequence;

			ts.side = (p_order->m_side_flag == '1') ? BDT_Buy : BDT_Sell;  // '1' = buy, '2' = sell

			if (p_order->m_order_type == '1')
				ts.otype = ODT_AnyPrice;
			else if (p_order->m_order_type == '2')
				ts.otype = ODT_LimitPrice;
			else if (p_order->m_order_type == '3')
				ts.otype = ODT_BestPrice;
			else
				ts.otype = ODT_Unknown;

			ts.price = checkValid(p_order->m_order_price, 10000);
			ts.volume = (uint32_t)checkValid(p_order->m_order_quantity, 100);

			write_log(_sink, LL_DEBUG, "[on_report_efh_sze_lev2_tick][order] code: {}, exchg: {}, time: {}, index: {}, price: {}, volume: {},",
				code, exchg, ts.action_time, ts.index, ts.price, ts.volume);

			if (_sink)
				_sink->handleOrderDetail(ordDtl);

			ordDtl->release();
		}
	}
	else if (msg_type == SZE_LEV2_EXE_MSG_TYPE)
	{
		// 逐笔成交
		if (p_exe)
		{
			const char* code = (char*)p_exe->m_head.m_symbol;
			const char* exchg = "SZSE";

			WTSContractInfo* ct = _bd_mgr->getContract(code, exchg);
			if (ct == nullptr)
				return;

			WTSCommodityInfo* commInfo = ct->getCommInfo();

			WTSTransData *trans = WTSTransData::create(code);
			WTSTransStruct& ts = trans->getTransStruct();
			strcpy(ts.exchg, exchg);

			uint32_t actDate = (uint32_t)(p_exe->m_head.m_quote_update_time / 1000000000);
			uint32_t actTime = (uint32_t)(p_exe->m_head.m_quote_update_time % 1000000000);

			ts.trading_date = _trading_date;
			ts.action_date = actDate;
			ts.action_time = actTime;

			ts.index = p_exe->m_head.m_sequence;

			/////////////////////////////////////// 交易方向
			//if (_seq_map.find(std::string(code)) != _seq_map.end())
			std::string key = std::string(code) + "." + std::string(exchg);
			if (_seq_map.contains(std::string(key)))
			{
				SeqOrderNum& _pre_order = _seq_map[key];
				write_log(_sink, LL_DEBUG, "[order_sequence][trans] pre_buy_num: {}, pre_sell_num: {}, new_buy_num: {}, new_sell_num: {}",
					_pre_order.buy_order_seq, _pre_order.sell_order_seq, p_exe->m_trade_buy_num, p_exe->m_trade_sell_num);

				if (_pre_order.buy_order_seq == p_exe->m_trade_buy_num && _pre_order.sell_order_seq < p_exe->m_trade_sell_num)
				{
					// 卖
					ts.side = BDT_Sell;
					_pre_order.sell_order_seq = p_exe->m_trade_sell_num;
				}
				else if (_pre_order.buy_order_seq < p_exe->m_trade_buy_num && _pre_order.sell_order_seq == p_exe->m_trade_sell_num)
				{
					// 买
					ts.side = BDT_Buy;
					_pre_order.buy_order_seq = p_exe->m_trade_buy_num;
				}
				else
					ts.side = BDT_Unknown;
			}
			else
			{
				if (p_exe->m_trade_buy_num == 0)
					ts.side = BDT_Sell;
				else if (p_exe->m_trade_sell_num == 0)
					ts.side = BDT_Buy;
				else
					ts.side = BDT_Unknown;

				SeqOrderNum _code_seq(p_exe->m_trade_buy_num, p_exe->m_trade_sell_num);
				_seq_map.insert(std::pair<std::string, SeqOrderNum>(std::string(code), _code_seq));
			}

			ts.ttype = (p_exe->m_trade_type == '4') ? TT_Match : TT_Cancel;

			ts.price = checkValid(p_exe->m_trade_price, 10000);
			ts.volume = (uint32_t)checkValid(p_exe->m_trade_quantity, 100);
			ts.bidorder = p_exe->m_trade_buy_num;
			ts.askorder = p_exe->m_trade_sell_num;

			write_log(_sink, LL_DEBUG, "[on_report_efh_sze_lev2_tick][trans] code: {}, exchg: {}, time: {}, index: {}, price: {}, volume: {}, initial time: {}",
				code, exchg, ts.action_time, ts.index, ts.price, ts.volume, p_exe->m_head.m_quote_update_time);

			if (_sink)
				_sink->handleTransaction(trans);

			trans->release();
		}
	}
}

void ParserShengLi::on_report_efh_sze_lev2_idx(sze_hpf_idx* p_idx)
{
	if (p_idx)
	{
		// 指数
		const char* code = (char*)p_idx->m_head.m_symbol;
		const char* exchg = "SZSE";

		WTSContractInfo* ct = _bd_mgr->getContract(code, exchg);
		if (ct == NULL)
		{
			if (_sink)
				write_log(_sink, LL_ERROR, "[ParserInsight] Instrument {}.{} not exists...", exchg, code);
			return;
		}

		WTSCommodityInfo* commInfo = ct->getCommInfo();

		WTSTickData* tick = WTSTickData::create(code);
		tick->setContractInfo(ct);
		WTSTickStruct& quote = tick->getTickStruct();
		strcpy(quote.exchg, exchg);

		uint32_t actDate = (uint32_t)(p_idx->m_head.m_quote_update_time / 1000000000);
		uint32_t actTime = (uint32_t)(p_idx->m_head.m_quote_update_time % 1000000000);

		quote.action_date = actDate;
		quote.action_time = actTime;
		quote.trading_date = _trading_date;

		const int _DIVIDER = 10000;
		const int _DIVIDER_V = 100;
		const int _DIVIDER_T = 1000000;

		quote.price = checkValid(p_idx->m_last_price, _DIVIDER);
		quote.open = checkValid(p_idx->m_open_price, _DIVIDER);
		quote.high = checkValid(p_idx->m_day_high_price, _DIVIDER);
		quote.low = checkValid(p_idx->m_day_low_price, _DIVIDER);

		quote.total_volume = checkValid(p_idx->m_total_quantity, _DIVIDER_V);
		quote.total_turnover = checkValid(p_idx->m_total_value, _DIVIDER_T);

		quote.pre_close = checkValid(p_idx->m_pre_close_price, _DIVIDER);

		if (_sink)
			_sink->handleQuote(tick, 1);

		tick->release();
	}
}

void ParserShengLi::on_report_efh_sze_lev2_tree(sze_hpf_tree* p_tree)
{
	if (p_tree)
	{
		const char* code = (char*)p_tree->m_head.m_symbol;
		const char* exchg = "SZSE";

		//TODO: 此处过滤非订阅合约

		WTSContractInfo* ct = _bd_mgr->getContract(code, exchg);
		if (ct == NULL)
		{
			if (_sink)
				write_log(_sink, LL_ERROR, "[ParserInsight] Instrument {}.{} not exists...", exchg, code);
			return;
		}

		WTSCommodityInfo* commInfo = ct->getCommInfo();

		WTSTickData* tick = WTSTickData::create(code);
		tick->setContractInfo(ct);
		WTSTickStruct& quote = tick->getTickStruct();
		strcpy(quote.exchg, exchg);

		uint32_t actDate = (uint32_t)(p_tree->m_head.m_quote_update_time / 1000000000);
		uint32_t actTime = (uint32_t)(p_tree->m_head.m_quote_update_time % 1000000000);
		quote.trading_date = _trading_date;

		const int _DIVIDER = 10000;
		const int _DIVIDER_V = 100;
		const int _DIVIDER_T = 1000000;

		quote.price = checkValid(p_tree->m_last_price, _DIVIDER);
		quote.open = checkValid(p_tree->m_open_price, _DIVIDER);
		quote.high = checkValid(p_tree->m_day_high_price, _DIVIDER);
		quote.low = checkValid(p_tree->m_day_low_price, _DIVIDER);

		quote.total_volume = checkValid(p_tree->m_total_quantity, _DIVIDER_V);
		quote.total_turnover = checkValid(p_tree->m_total_value, _DIVIDER_T);

		quote.upper_limit = checkValid(p_tree->m_upper_limit_price, _DIVIDER);
		quote.lower_limit = checkValid(p_tree->m_low_limit_price, _DIVIDER);

		quote.pre_close = checkValid(p_tree->m_pre_close_price, _DIVIDER);

		//if (p_tree->m_head.m_message_type == '6')
		//	quote.open_interest = checkValid(p_tree->m_open_interest);

		//委卖价格
		for (int i = 0; i < SZE_LEV2_DEPTH; i++)
		{
			quote.ask_prices[i] = checkValid(p_tree->m_ask_unit[i].m_price, _DIVIDER);
			quote.ask_qty[i] = checkValid(p_tree->m_ask_unit[i].m_quantity, _DIVIDER_V);

			quote.bid_prices[i] = checkValid(p_tree->m_bid_unit[i].m_price, _DIVIDER);
			quote.bid_qty[i] = checkValid(p_tree->m_bid_unit[i].m_quantity, _DIVIDER_V);
		}

		if (_sink)
			_sink->handleQuote(tick, 1);

		tick->release();
	}
}

void ParserShengLi::on_report_efh_sze_lev2_turnover(sze_hpf_turnover* p_turnover)
{
	// 深交所成交量消息
}

void ParserShengLi::on_report_efh_sze_lev2_bond_snap(sze_hpf_bond_snap* p_bond)
{
	if (p_bond)
	{
		const char* code = (const char*)p_bond->m_head.m_symbol;
		const char* exchg = "SZSE";

		//TODO: 此处过滤非订阅合约

		WTSContractInfo* ct = _bd_mgr->getContract(code, exchg);
		if (ct == NULL)
		{
			if (_sink)
				write_log(_sink, LL_ERROR, "[ParserInsight] Instrument {}.{} not exists...", exchg, code);
			return;
		}
		WTSCommodityInfo* commInfo = ct->getCommInfo();

		WTSTickData* tick = WTSTickData::create(code);
		tick->setContractInfo(ct);
		WTSTickStruct& quote = tick->getTickStruct();
		strcpy(quote.exchg, exchg);

		quote.action_date = (uint32_t)(p_bond->m_head.m_quote_update_time / 1000000000);
		quote.action_time = (uint32_t)(p_bond->m_head.m_quote_update_time % 1000000000);

		const int _DIVIDER = 10000;
		const int _DIVIDER_V = 100;
		const int _DIVIDER_T = 1000000;

		quote.price = checkValid(p_bond->m_last_price, _DIVIDER);
		quote.open = checkValid(p_bond->m_open_price, _DIVIDER);
		quote.high = checkValid(p_bond->m_day_high_price, _DIVIDER);
		quote.low = checkValid(p_bond->m_day_low_price, _DIVIDER);

		quote.total_volume = checkValid(p_bond->m_total_quantity, _DIVIDER_V);
		quote.trading_date = _trading_date;
		quote.total_turnover = checkValid(p_bond->m_total_value, _DIVIDER_T);

		quote.pre_close = checkValid(p_bond->m_pre_close_price, _DIVIDER);

		//委卖价格
		for (int i = 0; i < SZE_LEV2_DEPTH; i++)
		{
			quote.ask_prices[i] = checkValid(p_bond->m_ask_unit[i].m_price, _DIVIDER);
			quote.ask_qty[i] = checkValid(p_bond->m_ask_unit[i].m_quantity, _DIVIDER_V);

			quote.bid_prices[i] = checkValid(p_bond->m_bid_unit[i].m_price, _DIVIDER);
			quote.bid_qty[i] = checkValid(p_bond->m_bid_unit[i].m_quantity, _DIVIDER_V);
		}

		if (_sink)
			_sink->handleQuote(tick, 1);

		tick->release();
	}
}

void ParserShengLi::on_report_efh_sze_lev2_bond_tick(int msg_type, sze_hpf_bond_order* p_order, sze_hpf_bond_exe* p_exe)
{
	if (msg_type == SZE_LEV2_BOND_ORDER_MSG_TYPE)
	{
		if (p_order)
		{
			// 逐笔委托
			const char* code = (const char*)p_order->m_head.m_symbol;
			const char* exchg = "SZSE";

			WTSContractInfo* ct = _bd_mgr->getContract(code, exchg);
			if (ct == nullptr)
				return;

			WTSCommodityInfo* commInfo = ct->getCommInfo();

			WTSOrdDtlData *ordDtl = WTSOrdDtlData::create(code);
			WTSOrdDtlStruct& ts = ordDtl->getOrdDtlStruct();
			strcpy(ts.exchg, exchg);

			uint32_t actDate = (uint32_t)(p_order->m_head.m_quote_update_time / 1000000000);
			uint32_t actTime = (uint32_t)(p_order->m_head.m_quote_update_time % 1000000000);

			ts.trading_date = _trading_date;
			ts.action_date = actDate;
			ts.action_time = actTime;

			ts.index = p_order->m_head.m_sequence;

			ts.side = (p_order->m_side == '1') ? BDT_Buy : BDT_Sell;  // '1' = buy, '2' = sell

			if (p_order->m_order_type == '1')
				ts.otype = ODT_AnyPrice;
			else if (p_order->m_order_type == '2')
				ts.otype = ODT_LimitPrice;
			else if (p_order->m_order_type == '3')
				ts.otype = ODT_BestPrice;
			else
				ts.otype = ODT_Unknown;

			ts.price = checkValid(p_order->m_price, 10000);
			ts.volume = (uint32_t)checkValid(p_order->m_quantity, 100);

			write_log(_sink, LL_DEBUG, "[on_report_efh_sze_lev2_bond_tick][order] code: {}, exchg: {}, time: {}, index: {}, price: {}, volume: {},",
				code, exchg, ts.action_time, ts.index, ts.price, ts.volume);

			if (_sink)
				_sink->handleOrderDetail(ordDtl);

			ordDtl->release();
		}
	}
	else if (msg_type == SZE_LEV2_BOND_EXE_MSG_TYPE)
	{
		if (p_exe)
		{
			// 逐笔成交
			const char* code = (const char*)p_exe->m_head.m_symbol;
			const char* exchg = "SZSE";

			WTSContractInfo* ct = _bd_mgr->getContract(code, exchg);
			if (ct == nullptr)
				return;

			WTSCommodityInfo* commInfo = ct->getCommInfo();

			WTSTransData *trans = WTSTransData::create(code);
			WTSTransStruct& ts = trans->getTransStruct();
			strcpy(ts.exchg, exchg);

			uint32_t actDate = (uint32_t)(p_exe->m_head.m_quote_update_time / 1000000000);
			uint32_t actTime = (uint32_t)(p_exe->m_head.m_quote_update_time % 1000000000);

			ts.trading_date = _trading_date;
			ts.action_date = actDate;
			ts.action_time = actTime;

			ts.index = p_exe->m_head.m_sequence;

			//if (_seq_map.find(std::string(code)) != _seq_map.end())
			std::string key = std::string(code) + "." + std::string(exchg);
			if (_seq_map.contains(std::string(key)))
			{
				SeqOrderNum& _pre_order = _seq_map[key];
				write_log(_sink, LL_DEBUG, "[on_report_efh_sze_lev2_bond_tick][trans] pre_buy_num: {}, pre_sell_num: {}, new_buy_num: {}, new_sell_num: {}",
					_pre_order.buy_order_seq, _pre_order.sell_order_seq, p_exe->m_buy_num, p_exe->m_sell_num);

				if (_pre_order.buy_order_seq == p_exe->m_buy_num && _pre_order.sell_order_seq < p_exe->m_sell_num)
				{
					// 卖
					ts.side = BDT_Sell;
					_pre_order.sell_order_seq = p_exe->m_sell_num;
				}
				else if (_pre_order.buy_order_seq < p_exe->m_buy_num && _pre_order.sell_order_seq == p_exe->m_sell_num)
				{
					// 买
					ts.side = BDT_Buy;
					_pre_order.buy_order_seq = p_exe->m_buy_num;
				}
				else
					ts.side = BDT_Unknown;
			}
			else
			{
				if (p_exe->m_buy_num == 0)
					ts.side = BDT_Sell;
				else if (p_exe->m_sell_num == 0)
					ts.side = BDT_Buy;
				else
					ts.side = BDT_Unknown;

				SeqOrderNum _code_seq(p_exe->m_buy_num, p_exe->m_sell_num);
				_seq_map.insert(std::pair<std::string, SeqOrderNum>(std::string(code), _code_seq));
			}

			ts.ttype = (p_exe->m_type == '4') ? TT_Match : TT_Cancel;

			ts.price = checkValid(p_exe->m_price, 10000);
			ts.volume = (uint32_t)checkValid(p_exe->m_quantity, 100);
			ts.bidorder = p_exe->m_buy_num;
			ts.askorder = p_exe->m_sell_num;

			write_log(_sink, LL_DEBUG, "[on_report_efh_sze_lev2_bond_tick][transaction] code: {}, exchg: {}, time: {}, index: {}, price: {}, volume: {}, raw time: {}",
				code, exchg, ts.action_time, ts.index, ts.price, ts.volume, p_exe->m_head.m_quote_update_time);

			if (_sink)
				_sink->handleTransaction(trans);

			trans->release();
		}
	}
}

void ParserShengLi::on_report_efh_sze_lev2_after_close(sze_hpf_after_close* p_close_px)
{
	// 深交所盘后定价消息
}

void ParserShengLi::on_report_efh_sze_lev2_ibr_tree(sze_hpf_ibr_tree* p_ibr_tree)
{
	// 深交所ibr建树快照消息
}

void ParserShengLi::on_report_efh_sse_lev2_idx(sse_hpf_idx* p_idx)
{
	if (p_idx)
	{
		// 指数
		const char* code = p_idx->m_symbol;
		const char* exchg = "SSE";

		WTSContractInfo* ct = _bd_mgr->getContract(code, exchg);
		if (ct == NULL)
		{
			if (_sink)
				write_log(_sink, LL_ERROR, "[ParserInsight] Instrument {}.{} not exists...", exchg, code);
			return;
		}

		WTSCommodityInfo* commInfo = ct->getCommInfo();

		WTSTickData* tick = WTSTickData::create(code);
		tick->setContractInfo(ct);
		WTSTickStruct& quote = tick->getTickStruct();
		strcpy(quote.exchg, exchg);

		uint32_t actDate = get_sse_date(p_idx->m_head);
		uint32_t actTime = p_idx->m_quote_update_time * 1000;  // p_idx->m_head.m_send_time * 1000;  // 情更新时间,格式如：HHMMSS（秒）

		quote.action_date = actDate;
		quote.action_time = actTime;

		const int _DIVIDER_P = 100000;
		const int _DIVIDER_V = 10;

		quote.price = checkValid(p_idx->m_last_price, _DIVIDER_P);
		quote.open = checkValid(p_idx->m_open_price, _DIVIDER_P);
		quote.high = checkValid(p_idx->m_day_high_price, _DIVIDER_P);
		quote.low = checkValid(p_idx->m_day_low_price, _DIVIDER_P);

		quote.total_volume = checkValid(p_idx->m_total_quantity, _DIVIDER_P);
		quote.trading_date = _trading_date;
		quote.total_turnover = checkValid(p_idx->m_total_value, _DIVIDER_V);

		quote.pre_close = checkValid(p_idx->m_pre_close_price, _DIVIDER_P);

		if (_sink)
			_sink->handleQuote(tick, 1);

		tick->release();
	}
}

void ParserShengLi::on_report_efh_sse_lev2_snap(sse_hpf_lev2* p_snap)
{
	if (p_snap)
	{
		const char* code = p_snap->m_symbol;
		const char* exchg = "SSE";

		//TODO: 此处过滤非订阅合约

		WTSContractInfo* ct = _bd_mgr->getContract(code, exchg);
		if (ct == NULL)
		{
			if (_sink)
				write_log(_sink, LL_ERROR, "[ParserInsight] Instrument {}.{} not exists...", exchg, code);
			return;
		}
		WTSCommodityInfo* commInfo = ct->getCommInfo();

		WTSTickData* tick = WTSTickData::create(code);
		tick->setContractInfo(ct);
		WTSTickStruct& quote = tick->getTickStruct();
		strcpy(quote.exchg, exchg);

		uint32_t actDate = get_sse_date(p_snap->m_head);
		uint32_t actTime = p_snap->m_quote_update_time * 1000;  // p_snap->m_head.m_send_time;

		quote.action_date = actDate;
		quote.action_time = actTime;

		const int _DIVIDER_P = 1000;
		const int _DIVIDER_V = 100000;

		quote.price = checkValid(p_snap->m_last_price, _DIVIDER_P);
		quote.open = checkValid(p_snap->m_open_price, _DIVIDER_P);
		quote.high = checkValid(p_snap->m_day_high_price, _DIVIDER_P);
		quote.low = checkValid(p_snap->m_day_low_price, _DIVIDER_P);

		quote.total_volume = checkValid(p_snap->m_total_quantity, _DIVIDER_P);
		quote.trading_date = _trading_date;
		quote.total_turnover = checkValid(p_snap->m_total_value, _DIVIDER_V);
		quote.pre_close = checkValid(p_snap->m_pre_close_price, _DIVIDER_P);

		//委卖价格
		for (int i = 0; i < SSE_LEV2_DEPTH; i++)
		{
			quote.ask_prices[i] = checkValid(p_snap->m_ask_unit[i].m_price, _DIVIDER_P);
			quote.ask_qty[i] = checkValid(p_snap->m_ask_unit[i].m_quantity, _DIVIDER_P);

			quote.bid_prices[i] = checkValid(p_snap->m_bid_unit[i].m_price, _DIVIDER_P);
			quote.bid_qty[i] = checkValid(p_snap->m_bid_unit[i].m_quantity, _DIVIDER_P);
		}

		write_log(_sink, LL_DEBUG, "[on_report_efh_sse_lev2_snap] code: {}, exchg: {}, open: {}, price: {}, ask_px1: {}, ask_qty1: {}, bid_px1: {}, bid_qty1: {}",
			code, exchg, quote.open, quote.price, quote.ask_prices[0], quote.ask_qty[0], quote.bid_prices[0], quote.bid_qty[0]);

		if (_sink)
			_sink->handleQuote(tick, 1);

		tick->release();
	}
}

void ParserShengLi::on_report_efh_sse_lev2_option(sse_hpf_stock_option* p_option)
{
	if (p_option)
	{
		const char* code = p_option->m_symbol;
		const char* exchg = "SSE";

		//TODO: 此处过滤非订阅合约

		WTSContractInfo* ct = _bd_mgr->getContract(code, exchg);
		if (ct == NULL)
		{
			if (_sink)
				write_log(_sink, LL_ERROR, "[ParserInsight] Instrument {}.{} not exists...", exchg, code);
			return;
		}
		WTSCommodityInfo* commInfo = ct->getCommInfo();

		WTSTickData* tick = WTSTickData::create(code);
		tick->setContractInfo(ct);
		WTSTickStruct& quote = tick->getTickStruct();
		strcpy(quote.exchg, exchg);

		uint32_t actDate = get_sse_date(p_option->m_head);;
		uint32_t actTime = p_option->m_quote_update_time * 10;  // p_snap->m_head.m_send_time;  // 行情更新时间,格式如：HHMMSSss（百分之一秒）

		quote.action_date = actDate;
		quote.action_time = actTime;

		const int _DIVIDER_P = 10000;
		const int _DIVIDER_V = 1000000;
		const int _DIVIDER_Q = 100;

		quote.price = checkValid(p_option->m_last_price, _DIVIDER_P);
		quote.open = checkValid(p_option->m_open_price, _DIVIDER_P);
		quote.high = checkValid(p_option->m_day_high_price, _DIVIDER_P);
		quote.low = checkValid(p_option->m_day_low_price, _DIVIDER_P);

		quote.total_volume = checkValid(p_option->m_total_quantity, _DIVIDER_Q);
		quote.trading_date = _trading_date;
		quote.total_turnover = checkValid(p_option->m_total_value, _DIVIDER_V);
		quote.open_interest = checkValid(p_option->m_open_interest, _DIVIDER_Q);
		quote.settle_price = checkValid(p_option->m_today_settle_price, _DIVIDER_P);
		quote.pre_settle = checkValid(p_option->m_pre_settle_price, _DIVIDER_P);

		quote.pre_close = checkValid(p_option->m_pre_close_price, _DIVIDER_P);

		//委卖价格
		for (int i = 0; i < SSE_OPTION_DEPTH; i++)
		{
			quote.ask_prices[i] = checkValid(p_option->m_ask_unit[i].m_price, _DIVIDER_P);
			quote.ask_qty[i] = checkValid(p_option->m_ask_unit[i].m_quantity, _DIVIDER_Q);

			quote.bid_prices[i] = checkValid(p_option->m_bid_unit[i].m_price, _DIVIDER_P);
			quote.bid_qty[i] = checkValid(p_option->m_bid_unit[i].m_quantity, _DIVIDER_Q);
		}

		if (_sink)
			_sink->handleQuote(tick, 1);

		tick->release();
	}
}

void ParserShengLi::on_report_efh_sse_lev2_tick(int msg_type, sse_hpf_order* p_order, sse_hpf_exe* p_exe)
{
	if (msg_type == SSE_LEV2_ORDER_MSG_TYPE)
	{
		// 逐笔委托
		if (p_order)
		{
			const char* code = p_order->m_symbol;
			const char* exchg = "SSE";

			WTSContractInfo* ct = _bd_mgr->getContract(code, exchg);
			if (ct == nullptr)
				return;

			WTSCommodityInfo* commInfo = ct->getCommInfo();

			WTSOrdDtlData *ordDtl = WTSOrdDtlData::create(code);
			WTSOrdDtlStruct& ts = ordDtl->getOrdDtlStruct();
			strcpy(ts.exchg, exchg);

			uint32_t actDate = get_sse_date(p_order->m_head);
			uint32_t actTime = p_order->m_order_time * 10;  // // 委托时间,格式如：HHMMSSss（百分之一秒）

			ts.trading_date = _trading_date;
			ts.action_date = actDate;
			ts.action_time = actTime;

			ts.index = p_order->m_order_index;

			ts.side = (p_order->m_side_flag == 'B') ? BDT_Buy : BDT_Sell;  // '1' = buy, '2' = sell

			if (p_order->m_order_type == '1')
				ts.otype = ODT_AnyPrice;
			else if (p_order->m_order_type == '2')
				ts.otype = ODT_LimitPrice;
			else if (p_order->m_order_type == '3')
				ts.otype = ODT_BestPrice;
			else
				ts.otype = ODT_Unknown;

			ts.price = checkValid(p_order->m_order_price, 1000);
			ts.volume = (uint32_t)checkValid(p_order->m_balance, 1000);

			if (_sink)
				_sink->handleOrderDetail(ordDtl);

			ordDtl->release();
		}
	}
	else if (msg_type == SSE_LEV2_EXE_MSG_TYPE)
	{
		// 逐笔成交
		if (p_exe)
		{
			const char* code = (const char*)p_exe->m_symbol;
			const char* exchg = "SSE";

			WTSContractInfo* ct = _bd_mgr->getContract(code, exchg);
			if (ct == nullptr)
				return;

			WTSCommodityInfo* commInfo = ct->getCommInfo();

			WTSTransData *trans = WTSTransData::create(code);
			WTSTransStruct& ts = trans->getTransStruct();
			strcpy(ts.exchg, exchg);

			uint32_t actDate = get_sse_date(p_exe->m_head);
			uint32_t actTime = p_exe->m_trade_time * 10;  // 成交时间,格式如：HHMMSSss（百分之一秒）

			ts.trading_date = _trading_date;
			ts.action_date = actDate;
			ts.action_time = actTime;

			ts.index = p_exe->m_trade_index;  // 此处不用盛立的序列号

			if (p_exe->m_trade_bs_flag == 'B')
				ts.side = BDT_Buy;
			else if (p_exe->m_trade_bs_flag == 'S')
				ts.side = BDT_Sell;
			else
				ts.side = BDT_Unknown;

			ts.ttype = TT_Match;

			ts.price = checkValid(p_exe->m_trade_price, 1000);
			ts.volume = (uint32_t)checkValid(p_exe->m_trade_quantity, 1000);
			ts.bidorder = p_exe->m_trade_buy_num;
			ts.askorder = p_exe->m_trade_sell_num;

			if (_sink)
				_sink->handleTransaction(trans);

			trans->release();
		}
	}
}

void ParserShengLi::on_report_efh_sse_lev2_tree(sse_hpf_tree* p_tree)
{
	if (p_tree)
	{
		const char* code = p_tree->m_symbol;
		const char* exchg = "SSE";

		//TODO: 此处过滤非订阅合约

		WTSContractInfo* ct = _bd_mgr->getContract(code, exchg);
		if (ct == NULL)
		{
			if (_sink)
				write_log(_sink, LL_ERROR, "[ParserInsight] Instrument {}.{} not exists...", exchg, code);
			return;
		}
		WTSCommodityInfo* commInfo = ct->getCommInfo();

		WTSTickData* tick = WTSTickData::create(code);
		tick->setContractInfo(ct);
		WTSTickStruct& quote = tick->getTickStruct();
		strcpy(quote.exchg, exchg);

		uint32_t actDate = get_sse_date(p_tree->m_head);;
		uint32_t actTime = p_tree->m_quote_update_time * 10;  // p_snap->m_head.m_send_time;  // 行情更新时间,格式如：HHMMSSss（百分之一秒）

		quote.action_date = actDate;
		quote.action_time = actTime;

		const int _DIVIDER_P = 1000;
		const int _DIVIDER_V = 100000;

		quote.price = checkValid(p_tree->m_last_price, _DIVIDER_P);
		quote.open = checkValid(p_tree->m_open_price, _DIVIDER_P);
		quote.high = checkValid(p_tree->m_day_high_price, _DIVIDER_P);
		quote.low = checkValid(p_tree->m_day_low_price, _DIVIDER_P);

		quote.total_volume = checkValid(p_tree->m_total_quantity, _DIVIDER_P);
		quote.trading_date = _trading_date;
		quote.total_turnover = checkValid(p_tree->m_total_value, _DIVIDER_V);
		quote.pre_close = checkValid(p_tree->m_pre_close_price, _DIVIDER_P);

		//委卖价格
		for (int i = 0; i < SSE_LEV2_DEPTH; i++)
		{
			quote.ask_prices[i] = checkValid(p_tree->m_ask_unit[i].m_price, _DIVIDER_P);
			quote.ask_qty[i] = checkValid(p_tree->m_ask_unit[i].m_quantity, _DIVIDER_P);

			quote.bid_prices[i] = checkValid(p_tree->m_bid_unit[i].m_price, _DIVIDER_P);
			quote.bid_qty[i] = checkValid(p_tree->m_bid_unit[i].m_quantity, _DIVIDER_P);
		}

		if (_sink)
			_sink->handleQuote(tick, 1);

		tick->release();
	}
}

void ParserShengLi::on_report_efh_sse_lev2_bond_snap(sse_hpf_bond_snap* p_bond)
{
	if (p_bond)
	{
		const char* code = p_bond->m_symbol;
		const char* exchg = "SSE";

		//TODO: 此处过滤非订阅合约

		WTSContractInfo* ct = _bd_mgr->getContract(code, exchg);
		if (ct == NULL)
		{
			if (_sink)
				write_log(_sink, LL_ERROR, "[ParserInsight] Instrument {}.{} not exists...", exchg, code);
			return;
		}
		WTSCommodityInfo* commInfo = ct->getCommInfo();

		WTSTickData* tick = WTSTickData::create(code);
		tick->setContractInfo(ct);
		WTSTickStruct& quote = tick->getTickStruct();
		strcpy(quote.exchg, exchg);

		uint32_t actDate = get_sse_date(p_bond->m_head);;
		uint32_t actTime = p_bond->m_quote_update_time * 10;  // p_snap->m_head.m_send_time;  // 行情更新时间,格式如：HHMMSSss（百分之一秒）

		quote.action_date = actDate;
		quote.action_time = actTime;

		const int _DIVIDER_P = 1000;
		const int _DIVIDER_V = 100000;

		quote.price = checkValid(p_bond->m_last_price, _DIVIDER_P);
		quote.open = checkValid(p_bond->m_open_price, _DIVIDER_P);
		quote.high = checkValid(p_bond->m_day_high_price, _DIVIDER_P);
		quote.low = checkValid(p_bond->m_day_low_price, _DIVIDER_P);

		quote.total_volume = checkValid(p_bond->m_total_quantity, _DIVIDER_P);
		quote.trading_date = _trading_date;
		quote.total_turnover = checkValid(p_bond->m_total_value, _DIVIDER_V);
		quote.pre_close = checkValid(p_bond->m_pre_close_price, _DIVIDER_P);

		//委卖价格
		for (int i = 0; i < SSE_LEV2_DEPTH; i++)
		{
			quote.ask_prices[i] = checkValid(p_bond->m_ask_unit[i].m_price, _DIVIDER_P);
			quote.ask_qty[i] = checkValid(p_bond->m_ask_unit[i].m_quantity, _DIVIDER_P);

			quote.bid_prices[i] = checkValid(p_bond->m_bid_unit[i].m_price, _DIVIDER_P);
			quote.bid_qty[i] = checkValid(p_bond->m_bid_unit[i].m_quantity, _DIVIDER_P);
		}

		if (_sink)
			_sink->handleQuote(tick, 1);

		tick->release();
	}
}

void ParserShengLi::on_report_efh_sse_lev2_bond_tick(sse_hpf_bond_tick* p_tick)
{
	if (p_tick)
	{
		if ((const char)p_tick->m_tick_type == 'A' || (const char)p_tick->m_tick_type == 'S')
		{
			// 逐笔委托
			const char* code = p_tick->m_symbol;
			const char* exchg = "SSE";

			WTSContractInfo* ct = _bd_mgr->getContract(code, exchg);
			if (ct == nullptr)
				return;

			WTSCommodityInfo* commInfo = ct->getCommInfo();

			WTSOrdDtlData *ordDtl = WTSOrdDtlData::create(code);
			WTSOrdDtlStruct& ts = ordDtl->getOrdDtlStruct();
			strcpy(ts.exchg, exchg);

			uint32_t actDate = get_sse_date(p_tick->m_head);
			uint32_t actTime = p_tick->m_tick_time;  // // 委托时间,格式如：HHMMSSsss

			ts.trading_date = _trading_date;
			ts.action_date = actDate;
			ts.action_time = actTime;

			ts.index = p_tick->m_tick_index;

			if (p_tick->m_side_flag == 'B')
				ts.side = BDT_Buy;
			else if (p_tick->m_side_flag == 'S')
				ts.side = BDT_Sell;
			else
				ts.side = BDT_Unknown;

			//////////////////////////////////////////////////////////////////////////TODO: 价格类型
			//if (p_tick->m_order_type == '1')
			//	ts.otype = ODT_AnyPrice;
			//else if (p_tick->m_order_type == '2')
			//	ts.otype = ODT_LimitPrice;
			//else if (p_tick->m_order_type == '3')
			//	ts.otype = ODT_BestPrice;
			//else
			//	ts.otype = ODT_Unknown;

			ts.price = checkValid(p_tick->m_price, 1000);
			ts.volume = (uint32_t)checkValid(p_tick->m_quantity, 1000);

			write_log(_sink, LL_DEBUG, "[on_report_efh_sse_lev2_tick][order] code: {}, exchg: {}, time: {}, index: {}, price: {}, volume: {},",
				code, exchg, ts.action_time, ts.index, ts.price, ts.volume);

			if (_sink)
				_sink->handleOrderDetail(ordDtl);

			ordDtl->release();
		}
		else
		{
			// 逐笔成交
			const char* code = (const char*)p_tick->m_symbol;
			const char* exchg = "SSE";

			WTSContractInfo* ct = _bd_mgr->getContract(code, exchg);
			if (ct == nullptr)
				return;

			WTSCommodityInfo* commInfo = ct->getCommInfo();

			WTSTransData *trans = WTSTransData::create(code);
			WTSTransStruct& ts = trans->getTransStruct();
			strcpy(ts.exchg, exchg);

			uint32_t actDate = get_sse_date(p_tick->m_head);
			uint32_t actTime = p_tick->m_tick_time;  // 成交时间,格式如：HHMMSSsss
			ts.trading_date = _trading_date;
			ts.action_date = actDate;
			ts.action_time = actTime;

			ts.index = p_tick->m_tick_index;  // 此处不用盛立的序列号

			if (p_tick->m_side_flag == 'B')
				ts.side = BDT_Buy;
			else if (p_tick->m_side_flag == 'S')
				ts.side = BDT_Sell;
			else
				ts.side = BDT_Unknown;

			ts.ttype = ((char)p_tick->m_tick_type == 'T') ? TT_Match : TT_Cancel;

			ts.price = checkValid(p_tick->m_price, 1000);
			ts.volume = (uint32_t)checkValid(p_tick->m_quantity, 1000);
			ts.bidorder = p_tick->m_buy_num;
			ts.askorder = p_tick->m_sell_num;

			write_log(_sink, LL_DEBUG, "[on_report_efh_sse_lev2_tick][transaction] code: {}, exchg: {}, time: {}, index: {}, price: {}, volume: {},",
				code, exchg, ts.action_time, ts.index, ts.price, ts.volume);

			if (_sink)
				_sink->handleTransaction(trans);

			trans->release();
		}
	}
}

void ParserShengLi::on_report_efh_sse_lev2_etf(sse_hpf_etf* p_tick)
{
	// ETF 统计
}

void ParserShengLi::on_report_efh_sse_lev2_tick_merge(sse_hpf_tick_merge* p_tick)
{
	if (p_tick)
	{
		if ((const char)p_tick->m_tick_type == 'A' || (const char)p_tick->m_tick_type == 'S')
		{
			// 逐笔委托
			const char* code = p_tick->m_symbol;
			const char* exchg = "SSE";

			WTSContractInfo* ct = _bd_mgr->getContract(code, exchg);
			if (ct == nullptr)
				return;

			WTSCommodityInfo* commInfo = ct->getCommInfo();

			WTSOrdDtlData *ordDtl = WTSOrdDtlData::create(code);
			WTSOrdDtlStruct& ts = ordDtl->getOrdDtlStruct();
			strcpy(ts.exchg, exchg);

			uint32_t actDate = get_sse_date(p_tick->m_head);
			uint32_t actTime = p_tick->m_tick_time;  // // 委托时间,格式如：HHMMSSsss

			ts.trading_date = _trading_date;
			ts.action_date = actDate;
			ts.action_time = actTime;

			ts.index = p_tick->m_tick_index;

			if (p_tick->m_side_flag == 'B')
				ts.side = BDT_Buy;
			else if (p_tick->m_side_flag == 'S')
				ts.side = BDT_Sell;
			else
				ts.side = BDT_Unknown;

			//if (p_tick->m_order_type == '1')
			//	ts.otype = ODT_AnyPrice;
			//else if (p_tick->m_order_type == '2')
			//	ts.otype = ODT_LimitPrice;
			//else if (p_tick->m_order_type == '3')
			//	ts.otype = ODT_BestPrice;
			//else
			//	ts.otype = ODT_Unknown;

			ts.price = checkValid(p_tick->m_price, 1000);
			ts.volume = (uint32_t)checkValid(p_tick->m_quantity, 1000);

			if (_sink)
				_sink->handleOrderDetail(ordDtl);

			ordDtl->release();
		}
		else
		{
			// 逐笔成交
			const char* code = (const char*)p_tick->m_symbol;
			const char* exchg = "SSE";

			WTSContractInfo* ct = _bd_mgr->getContract(code, exchg);
			if (ct == nullptr)
				return;

			WTSCommodityInfo* commInfo = ct->getCommInfo();

			WTSTransData *trans = WTSTransData::create(code);
			WTSTransStruct& ts = trans->getTransStruct();
			strcpy(ts.exchg, exchg);

			uint32_t actDate = get_sse_date(p_tick->m_head);
			uint32_t actTime = (uint32_t)p_tick->m_tick_time;  // 成交时间,格式如：HHMMSSsss
			ts.trading_date = _trading_date;
			ts.action_date = actDate;
			ts.action_time = actTime;

			ts.index = p_tick->m_tick_index;  // 此处不用盛立的序列号

			if (p_tick->m_side_flag == 'B')
				ts.side = BDT_Buy;
			else if (p_tick->m_side_flag == 'S')
				ts.side = BDT_Sell;
			else
				ts.side = BDT_Unknown;

			ts.ttype = ((char)p_tick->m_tick_type == 'T') ? TT_Match : TT_Cancel;

			ts.price = checkValid(p_tick->m_price, 1000);
			ts.volume = (uint32_t)checkValid(p_tick->m_quantity, 1000);
			ts.bidorder = p_tick->m_buy_num;
			ts.askorder = p_tick->m_sell_num;

			if (_sink)
				_sink->handleTransaction(trans);

			trans->release();
		}
	}
}
