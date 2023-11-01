/*!*************************************************************************
@node		Copyright (coffee), 2005-2020, Shengli Tech. Co., Ltd.
@file   	sse_report.h
@date		2020/12/14 08:30
@author		shengli

@brief		接收、上报上交所行情
******************************************************************************/
#pragma once
#include <iostream>
#include "../interface/i_efh_sse_lev2_api.h"
#include "filter_symbol.h"

using namespace std;

#ifdef WINDOWS
#define		QT_SSE_QUOTE_COUNT			(4000000)	/// 根据实际情况进行调整
#else
#define		QT_SSE_QUOTE_COUNT			(8000000)	/// 根据实际情况进行调整
#endif

#define		SYMBOL_LEN					9
typedef		char						T_SYMBOL_TYPE[SYMBOL_LEN];
typedef		long long					T_UPDATE_TIME_TYPE;

#ifdef WINDOWS
#include <Windows.h>
typedef		HMODULE						dll_handle_sl;
#else
typedef		void*						dll_handle_sl;
#endif

#pragma pack(push, 1)
/// 记录需求字段
struct sse_qt_node_index
{
	T_SYMBOL_TYPE			m_symbol;					/// 合约
	unsigned int			m_quote_update_time;		/// 行情时间戳
	unsigned int			m_pre_close_price;			/// 前收盘指数
	unsigned int			m_open_price;				/// 今开盘指数
	unsigned int			m_day_high_price;			/// 最高指数
	unsigned int			m_day_low_price;			/// 最低指数
	unsigned int			m_last_price;				/// 最新指数
	long long				m_total_quantity;			/// 指数内所有合约的总成交量（手
	T_UPDATE_TIME_TYPE		m_local_time;				/// 当地时间
};

struct sse_qt_node_snap
{
	T_SYMBOL_TYPE		m_symbol;						/// 合约
	unsigned int		m_quote_update_time;			/// 行情时间戳
	unsigned int		m_last_price;					/// 最新价
	long long			m_total_quantity;				/// 成交总量
	long long			m_total_value;					/// 成交总额
	unsigned int		m_bid_1_price;					/// 买一价
	long long			m_bid_1_quantity;				/// 买一量
	unsigned int		m_ask_1_price;					/// 卖一价
	long long			m_ask_1_quantity;				/// 卖一量
	unsigned int		m_bid_10_price;					/// 买十价
	long long			m_bid_10_quantity;				/// 买十量
	unsigned int		m_ask_10_price;					/// 卖十价
	long long			m_ask10_quantity;				/// 卖十量
	T_UPDATE_TIME_TYPE	m_local_time;					/// 当地时间
};

struct sse_qt_node_stock_option
{
	T_SYMBOL_TYPE		m_symbol;						/// 合约
	unsigned int		m_quote_update_time;			/// 交易所行情时间戳
	unsigned int		m_last_price;					/// 最新价
	long long 			m_total_value;					/// 成交量
	unsigned int		m_bid_1_price;					/// 买一价
	long long			m_bid_1_quantity;				/// 买一量
	unsigned int		m_ask_1_price;					/// 卖一价
	long long			m_ask_1_quantity;				/// 卖一量
	unsigned int		m_bid_5_price;					/// 买五价
	long long			m_bid_5_quantity;				/// 买五量
	unsigned int		m_ask_5_price;					/// 卖五价
	long long			m_ask_5_quantity;				/// 卖五量
	T_UPDATE_TIME_TYPE	m_local_time;					/// 当地时间
};

struct sse_qt_node_exe
{
	T_SYMBOL_TYPE			m_symbol;					/// 合约
	short					m_channel_num;				/// 频道代码
	long long				m_trade_index;				/// 消息号
	unsigned int			m_trade_time;				/// 成交时间
	unsigned int			m_trade_price;				/// 成交价格，放大1000倍
	long long				m_trade_quantity;			/// 成交数量，放大1000倍
	char					m_trade_bs_flag;			/// 内外盘标志
	T_UPDATE_TIME_TYPE		m_local_time;				/// 当地时间
};

struct sse_qt_node_order
{
	T_SYMBOL_TYPE			m_symbol;					/// 合约
	short					m_channel_num;				/// 频道代码
	long long				m_order_index;				/// 消息号
	unsigned int			m_order_time;				/// 成交时间
	unsigned int			m_order_price;				/// 成交价格，放大1000倍
	long long				m_balance;					/// 成交数量，放大1000倍
	char					m_side_flag;				/// 内外盘标志
	T_UPDATE_TIME_TYPE		m_local_time;				/// 当地时间
};

struct sse_qt_node_tree
{
	T_SYMBOL_TYPE		m_symbol;						/// 合约
	unsigned int		m_i_quote_update_time;			/// 行情时间戳
	unsigned int		m_last_price;					/// 最新价
	long long			m_total_quantity;				/// 成交总量
	long long			m_total_value;					/// 成交总额
	unsigned int		m_bid_1_price;					/// 买一价
	long long			m_bid_1_quantity;				/// 买一量
	unsigned int		m_ask_1_price;					/// 卖一价
	long long			m_ask_1_quantity;				/// 卖一量
	unsigned int		m_bid_10_price;					/// 买十价
	long long			m_bid_10_quantity;				/// 买十量
	unsigned int		m_ask_10_price;					/// 卖十价
	long long			m_ask_10_quantity;				/// 卖十量
	T_UPDATE_TIME_TYPE	m_local_time;					/// 当地时间
};

struct sse_qt_node_bond_snap
{
	unsigned int		m_seq;
	unsigned int		m_quote_update_time;
	char				m_symbol[9];
	int					m_pre_close_px;					/// 昨收价
	unsigned int		m_open_px;						// 开盘价
	unsigned int		m_day_high;						// 最高价
	unsigned int		m_day_low;						// 最低价
	unsigned int		m_last_px;						// 现价
	unsigned char		m_instrument_status;			// 当前品种交易状态
	unsigned int		m_trade_num;					// 成交笔数
	unsigned long long	m_trade_volumn;					// 成交总量
	unsigned long long	m_trade_value;					// 成交总金额
	unsigned int		m_total_bid_number;				// 买入总笔数
	unsigned int		m_total_offer_number;			// 卖出总笔数

	unsigned int		m_bid_price1;
	unsigned long long	m_bid_volumn1;
	unsigned int		m_bid_price10;
	unsigned long long	m_bid_volumn10;

	unsigned int		m_ask_price1;
	unsigned long long	m_ask_volumn1;
	unsigned int		m_ask_price10;
	unsigned long long	m_ask_volumn10;
	T_UPDATE_TIME_TYPE	m_local_time;					/// 当地时间
};

struct sse_qt_node_bond_tick
{
	unsigned int			m_seq;
	unsigned int			m_tick_index;
	unsigned int			m_channel_id;
	char					m_symbol[9];
	unsigned int			m_tick_time;
	unsigned char			m_tick_type;					// 类型， A新增订单， D删除订单， S产品状态订单， T成交
	unsigned long long		m_buy_order_no;					// 买方订单号，对产品状态订单无意义
	unsigned long long		m_sell_order_no;				// 卖方订单号，对产品状态订单无意义
	unsigned int			m_price;						// 价格，对产品状态订单无意义
	unsigned long long		m_qty;							// 数量（手），对产品状态订单无意义
	unsigned long long		m_trade_money;					// 成交金额（元），仅适用于成交消息
	unsigned int			m_side_flag;					// 买卖标志，若为订单： B - 买单， S - 卖单；若为成交：  B - 外盘,主动买， S - 内盘,主动卖 N - 未知 。
	unsigned int			m_instrument_status;			// 股票 / 标的状态，仅适用于产品状态订单
	T_UPDATE_TIME_TYPE	m_local_time;					/// 当地时间
};

struct sse_qt_node_tick_merge
{
	unsigned int			m_seq;
	unsigned int			m_tick_index;
	unsigned int			m_channel_id;
	char					m_symbol[9];
	unsigned int			m_tick_time;
	unsigned char			m_tick_type;					// 类型， A新增订单， D删除订单， S产品状态订单， T成交
	unsigned long long		m_buy_order_no;					// 买方订单号，对产品状态订单无意义
	unsigned long long		m_sell_order_no;				// 卖方订单号，对产品状态订单无意义
	unsigned int			m_price;						// 价格，对产品状态订单无意义
	unsigned long long		m_qty;							// 数量（手），对产品状态订单无意义
	unsigned long long		m_trade_money;					// 成交金额（元），仅适用于成交消息
	unsigned int			m_side_flag;					// 买卖标志，若为订单： B - 买单， S - 卖单；若为成交：  B - 外盘,主动买， S - 内盘,主动卖 N - 未知 。
	unsigned int			m_instrument_status;			// 股票 / 标的状态，仅适用于产品状态订单
	T_UPDATE_TIME_TYPE	m_local_time;					/// 当地时间
};

struct sse_qt_node_etf
{
	sse_hpf_etf quote;
	T_UPDATE_TIME_TYPE	m_local_time;					/// 当地时间
};

#pragma pack(pop)

class sse_report:public efh_sse_lev2_api_event, public efh_sse_lev2_api_depend
{
public:
	sse_report();
	~sse_report();
	bool init(socket_udp_param* p_param, int num);
	void set_tick_detach(bool enable);
	void run(int nic_type, bool b_report_quit = false, const char* symbol ="");
	void show();
	void show_rdp(const char* symbol="");
	void close();

protected:
	virtual void on_report_efh_sse_lev2_idx(sse_hpf_idx* p_index);
	virtual void on_report_efh_sse_lev2_snap(sse_hpf_lev2* p_snap);
	virtual void on_report_efh_sse_lev2_option(sse_hpf_stock_option* p_option);
	virtual void on_report_efh_sse_lev2_tree(sse_hpf_tree* p_tree);
	virtual void on_report_efh_sse_lev2_tick(int msg_type, sse_hpf_order* p_order, sse_hpf_exe* p_exe);
	virtual void on_report_efh_sse_lev2_bond_snap(sse_hpf_bond_snap* p_bond);
	virtual void on_report_efh_sse_lev2_bond_tick(sse_hpf_bond_tick* p_tick);
	virtual void on_report_efh_sse_lev2_tick_merge(sse_hpf_tick_merge* p_tick);
	virtual void on_report_efh_sse_lev2_etf(sse_hpf_etf* p_tick);

	virtual void efh_sse_lev2_debug(const char* msg, int len);
	virtual void efh_sse_lev2_error(const char* msg, int len);
	virtual void efh_sse_lev2_info(const char* msg, int len);

private:
	string format_str(const char* pFormat, ...);
	virtual void report_efh_sse_lev2_idx();
	virtual void report_efh_sse_lev2_snap();
	virtual void report_efh_sse_lev2_option();
	virtual void report_efh_sse_lev2_exe();
	virtual void report_efh_sse_lev2_order();
	virtual void report_efh_sse_lev2_tree();
	virtual void report_efh_sse_lev2_bond_snap();
	virtual void report_efh_sse_lev2_bond_tick();
	virtual void report_efh_sse_lev2_tick_merge();
	virtual void report_efh_sse_lev2_etf();

	string get_sse_src_trading_phase_code(char ch_sl_trading_phase_code);
	string get_sse_src_instrument_status(char ch_sl_instrument_status);
	string get_sse_bond_trading_phase_code_by_instrument_status(char ch_sl_instrument_status);

private:
	bool						m_b_report_quit;
	bool						m_b_tick_detach;
	dll_handle_sl				m_h_core;
	i_efh_sse_lev2_api*			m_p_quote;
	FILE*						m_fp_lev2;
	FILE*						m_fp_idx;
	FILE*						m_fp_option;
	FILE*						m_fp_exe;
	FILE*						m_fp_order;
	FILE*						m_fp_tick;
	FILE*						m_fp_tree;
	FILE*						m_fp_bond;
	FILE*						m_fp_bond_tick;
	FILE*						m_fp_tick_merge;
	FILE*						m_fp_etf;

/// TEST_SHENGLI_CODE_CONVERSION 宏定义说明
/// 功能: 方便测试 "股票/标的状态码" 与 "交易状态码",在盛立编码值与原生值之间的转换函数
/// 行为说明: 在接收合约时,依次调用转换函数,将转换前的值与转换后的值,写入 CSV文件内
/// 此宏应在编译阶段于 Makefile 文件内定义,默认情况下未定义(不启用)
#ifdef TEST_SHENGLI_CODE_CONVERSION
	FILE*						m_fp_it_code_lev2;
	FILE*						m_fp_it_code_option;
	FILE*						m_fp_it_code_tree;
	FILE*						m_fp_it_code_bond;
	FILE*						m_fp_it_code_bond_tick;
#endif

	bool						m_b_snap_rollback_flag;
	bool						m_b_idx_rollback_flag;
	bool						m_b_option_rollback_flag;
	bool						m_b_tick_rollback_flag;
	bool						m_b_tree_rollback_flag;
	bool						m_b_bond_rollback_flag;
	bool						m_b_bond_tick_rollback_flag;
	bool						m_b_tick_merge_rollback_flag;
	bool						m_b_etf_rollback_flag;


	vector<string>				m_vec_symbol;
	unsigned int				m_i_snap_num;
	unsigned int				m_i_idx_num;
	unsigned int				m_i_option_num;
	unsigned int				m_i_tick_num;
	unsigned int				m_i_tree_num;
	unsigned int				m_i_bond_num;
	unsigned int				m_i_bond_tick_num;
	unsigned int				m_i_tick_merge_num;
	unsigned int				m_i_etf_num;

	unsigned int				m_i_lost_idx_count;
	unsigned int				m_i_lost_snap_count;
	unsigned int				m_i_lost_option_count;
	unsigned int				m_i_lost_tick_count;
	unsigned int				m_i_lost_tree_count;
	unsigned int				m_i_lost_bond_count;
	unsigned int				m_i_lost_bond_tick_count;
	unsigned int				m_i_lost_tick_merge_count;
	unsigned int				m_i_lost_etf_count;

	long long					m_ll_snap_count;
	long long					m_ll_idx_count;
	long long					m_ll_option_count;
	long long					m_ll_exe_count;
	long long					m_ll_order_count;
	long long					m_ll_tree_count;
	long long					m_ll_bond_count;
	long long					m_ll_bond_tick_count;
	long long					m_ll_tick_merge_count;
	long long					m_ll_etf_count;

	sse_qt_node_snap*			m_snap;
	sse_qt_node_index*			m_idx;
	sse_qt_node_stock_option*	m_option;
	sse_qt_node_exe*			m_exe;
	sse_qt_node_order*			m_order;
	sse_qt_node_tree*			m_tree;
	sse_qt_node_bond_snap*		m_bond_snap;
	sse_qt_node_bond_tick*		m_bond_tick;
	sse_qt_node_tick_merge*		m_tick_merge;
	sse_qt_node_etf*			m_etf;
};
