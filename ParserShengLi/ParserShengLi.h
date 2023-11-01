#pragma once

#include <map>
#include <string>

#if _WIN32
#ifndef WINAPI
#define WINAPI __stdcall
#endif
#ifndef BOOL
typedef int BOOL;
#endif
#endif
#include <queue>

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/asio/io_service.hpp>
#include "../Share/StdUtils.hpp"
#include "../Includes/WTSCollection.hpp"
#include "../Share/TimeUtils.hpp"
#include "../Includes/IParserApi.h"
#include "../Share/DLLHelper.hpp"


#include "../API/API_SZE3.2_SSE4.3_25436.quote_recv_api_20230227_154536/interface/i_efh_sse_lev2_api.h"
#include "../API/API_SZE3.2_SSE4.3_25436.quote_recv_api_20230227_154536/interface/i_efh_sze_lev2_api.h"


#define SYMBOL_LEN	(9)
//typedef faster_hashmap<char[SYMBOL_LEN], unsigned long long> SeqCounter;
NS_WTP_BEGIN
class WTSTickData;
NS_WTP_END
struct SeqOrderNum
{
	SeqOrderNum() : buy_order_seq(0), sell_order_seq(0)
	{

	}

	SeqOrderNum(long  long bSeq, long  long sSeq) : buy_order_seq(bSeq), sell_order_seq(sSeq)
	{

	}

	long  long buy_order_seq;
	long long sell_order_seq;
};

#ifdef _WIN32
#include <Windows.h>
typedef		HMODULE						dll_handle_sl;
#else
typedef		void*						dll_handle_sl;
#endif
USING_NS_WTP;
using namespace boost::asio;
class ParserShengLi :public IParserApi, public efh_sse_lev2_api_depend, public efh_sse_lev2_api_event,public efh_sze_lev2_api_depend,public efh_sze_lev2_api_event
{
public:
	ParserShengLi();
	 ~ParserShengLi();

	

public:
	bool config_sock_udp_param(socket_udp_param & quote_param, const char * section,bool is_sh);
	void SubscribeMDs(bool isAll = false);



public:
	virtual bool init(WTSVariant* config) override;

	virtual void release() override;

	virtual bool connect() override;

	virtual bool disconnect() override;

	virtual bool isConnected() override;

	virtual void subscribe(const CodeSet &vecSymbols) override;

	virtual void unsubscribe(const CodeSet &vecSymbols) override;

	virtual void registerSpi(IParserSpi* listener) override;


protected:
	// 上交所
	virtual void on_report_efh_sse_lev2_idx(sse_hpf_idx* p_index);
	virtual void on_report_efh_sse_lev2_snap(sse_hpf_lev2* p_snap);
	virtual void on_report_efh_sse_lev2_option(sse_hpf_stock_option* p_option);
	virtual void on_report_efh_sse_lev2_tree(sse_hpf_tree* p_tree);
	virtual void on_report_efh_sse_lev2_tick(int msg_type, sse_hpf_order* p_order, sse_hpf_exe* p_exe);
	virtual void on_report_efh_sse_lev2_bond_snap(sse_hpf_bond_snap* p_bond);
	virtual void on_report_efh_sse_lev2_bond_tick(sse_hpf_bond_tick* p_tick);
	virtual void on_report_efh_sse_lev2_tick_merge(sse_hpf_tick_merge* p_tick);
	virtual void on_report_efh_sse_lev2_etf(sse_hpf_etf* p_tick);

	// 深交所
	virtual void on_report_efh_sze_lev2_after_close(sze_hpf_after_close* p_close_px);
	virtual void on_report_efh_sze_lev2_snap(sze_hpf_lev2* p_snap);
	virtual void on_report_efh_sze_lev2_tick(int msg_type, sze_hpf_order* p_order, sze_hpf_exe* p_exe);
	virtual void on_report_efh_sze_lev2_idx(sze_hpf_idx* p_idx);
	virtual void on_report_efh_sze_lev2_tree(sze_hpf_tree* p_tree);
	virtual void on_report_efh_sze_lev2_turnover(sze_hpf_turnover* p_turnover);
	virtual void on_report_efh_sze_lev2_bond_snap(sze_hpf_bond_snap* p_bond);
	virtual void on_report_efh_sze_lev2_bond_tick(int msg_type, sze_hpf_bond_order* p_order, sze_hpf_bond_exe* p_exe);
	virtual void on_report_efh_sze_lev2_ibr_tree(sze_hpf_ibr_tree* p_ibr_tree);

private:
	void doOnDisconnected();
	/*
	 *	发送登录请求
	 */
	void DoLogin();
	/*
	 *	订阅品种行情
	 */
	void DoSubscribeMD();
	/*
	*  通道准备
	*/
	bool	prepare();

private:
	uint32_t	_trading_date;
	dll_handle_sl	_m_h_core;

	i_efh_sse_lev2_api*			_sse_api;
	i_efh_sze_lev2_api*			_sze_api;

	int				_nic_type;

	IParserSpi*		_sink;
	IBaseDataMgr*	_bd_mgr;
	bool			_stopped;
	bool			_prepared;


	CodeSet			_set_subs;
	StdThreadPtr	_thrd_parser;

	int8_t m_enable_sse;	//使用上交所通道  1：使用 0：不使用
	int8_t m_enable_sze;	//使用深交所通道  1：使用 0：不使用

	WTSVariant*		_sse_config;	//sse相关配置 
	WTSVariant*		_szse_config;	//szse相关配置
	
	//fastest_hashmap<std::string, SeqOrderNum>	 _seq_map;  // 深交所逐笔数据没有提供方向，需要根据包头号判断方向
	faster_hashmap<std::string, SeqOrderNum>	 _seq_map;
};

