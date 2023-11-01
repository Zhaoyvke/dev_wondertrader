/*!*************************************************************************
@node		Copyright (coffee), 2005-2020, Shengli Tech. Co., Ltd.
@file   	sze_report.h
@date		2020/12/14 08:30
@author		shengli

@brief		接收、上报深交所行情
******************************************************************************/
#pragma once
#include <iostream>
#include "../interface/i_efh_sze_lev2_api.h"
#include "filter_symbol.h"

using namespace std;


#ifdef WINDOWS
#define QT_SZE_QUOTE_COUNT	(4000000)		/// 根据实际情况进行调整
#else
#define QT_SZE_QUOTE_COUNT	(8000000)		/// 根据实际情况进行调整
#endif


#ifdef WINDOWS
#include <Windows.h>
typedef		HMODULE						dll_handle_sl;
#else
typedef		void*						dll_handle_sl;
#endif

#pragma pack(push, 1)
/// 记录需求字段
struct qt_price_quantity_unit
{
	unsigned int		m_price;
	long long			m_quantity;
};

struct sze_qt_node_after_close
{
	char					m_symbol[9];				/// 合约
	short					m_channel_num;				/// 频道号
	long long				m_sequence_num;				/// 消息记录号
	long long				m_quote_update_time;		/// 行情时间
	unsigned long long		m_local_time;				/// 本地接收时间
	unsigned char			m_trading_status;		/// 交易阶段
	long long				m_total_trade_num;			/// 成交笔数
	unsigned long long 		m_total_quantity;			/// 成交总量
	unsigned long long 		m_total_value;				/// 成交总金额
	unsigned int			m_pre_close_price;			/// 昨收价
	unsigned int			m_exe_price;				/// 执行价格，如果NoMDEntries=1 ，取MDEntryPx ；如果NoMDEntries>1，取所有 透传 MDEntryPx 的最大值
	unsigned long long 		m_bid_quantity;				/// 买方数量
	unsigned long long 		m_ask_quantity;				/// 卖方数量
};

struct sze_qt_node_snap
{
	char					m_symbol[9];				/// 合约
	long long				m_quote_update_time;		/// 行情更新时间
	unsigned long long		m_local_time;				/// 本地接收时间

	unsigned int			m_last_price;				///	最新价
	unsigned long long		m_total_quantity;			///	总成交量
	unsigned long long		m_total_value;				/// 总成交额

	// snp
	qt_price_quantity_unit			m_bid_lev_1;		/// 买向第一档
	qt_price_quantity_unit			m_bid_lev_10;		/// 买向第十档
	qt_price_quantity_unit			m_ask_lev_1;		/// 卖向第一档
	qt_price_quantity_unit			m_ask_lev_10;		/// 卖向第十档
	/// opt
	qt_price_quantity_unit			m_bid_lev_5;		/// 买向第五档
	qt_price_quantity_unit			m_ask_lev_5;		/// 买向第五档
};

struct sze_qt_node_order
{
	char					m_symbol[9];				/// 合约
	short					m_channel_num;				/// 频道号
	long long				m_sequence_num;				/// 消息记录号
	long long				m_quote_update_time;		/// 行情更新时间
	unsigned long long		m_local_time;				///	本地接收时间
	int						m_order_price;				///	价格
	long long				m_order_quantity;			/// 数量
	char					m_order_type;				/// 订单类型
};

struct sze_qt_node_exe
{
	char					m_symbol[9];				/// 合约
	short					m_channel_num;				/// 频道号
	long long				m_sequence_num;				/// 消息记录号
	long long				m_quote_update_time;		/// 行情时间
	unsigned long long		m_local_time;				/// 本地接收时间
	unsigned int			m_trade_price;				/// 成交价格
	unsigned long long		m_trade_quantity;			/// 成交量
	char					m_trade_type;				/// 成交类型
};

struct sze_qt_node_index
{
	char					m_symbol[9];				/// 合约
	long long				m_quote_update_time;		/// 原始行情时间
	unsigned long long		m_local_time;				/// 本地接收时间
	unsigned long long		m_total_quantity;			/// 总成交量
	unsigned int			m_last_price;				/// 最新价
	unsigned int			m_pre_close_price;			/// 开盘价
	unsigned int			m_open_price;				/// 成交值
	unsigned int			m_day_high_price;			/// 最高价
	unsigned int			m_day_low_price;			/// 最低价
};

struct sze_qt_node_tree
{
	unsigned int			m_sequence;					/// 盛立行情序号
	char					m_symbol[9];				/// 合约
	long long				m_quote_update_time;		/// 行情更新时间
	unsigned long long		m_local_time;				/// 本地接收时间
	qt_price_quantity_unit	m_bid_lev_1;				/// 买向第一档
	qt_price_quantity_unit	m_bid_lev_10;				/// 买向第十档
	qt_price_quantity_unit	m_ask_lev_1;				/// 卖向第一档
	qt_price_quantity_unit	m_ask_lev_10;				/// 卖向第十档
	unsigned int			m_pre_close_price;			/// 昨收价
	unsigned int			m_open_price;				///	开盘价
	unsigned int			m_day_high_price;			/// 最高价
	unsigned int			m_day_low_price;			///	最低价
	unsigned int			m_last_price;				///	最新价
	unsigned long long		m_total_quantity;			///	总成交量
	unsigned long long		m_total_value;				/// 总成交额
	unsigned long long		m_total_bid_quantity;		/// 买总量
	unsigned long long		m_total_ask_quantity;		/// 卖总量
	unsigned int			m_total_bid_weighted_avg_price;			/// 买边挂单均价
	unsigned int			m_total_ask_weighted_avg_price;			/// 卖边挂单均价
	unsigned int			m_bid_depth;				/// 买边总档位数
	unsigned int			m_ask_depth;				/// 卖边总档位数
	unsigned long long		m_market_open_total_bid;	/// 买边市价单挂单总量
	unsigned long long		m_market_open_total_ask;	/// 卖边市价单挂单总量
};

struct sze_qt_node_ibr_tree
{
	unsigned int			m_sequence;					/// 盛立行情序号
	char					m_symbol[9];				/// 合约
	long long				m_quote_update_time;		/// 行情更新时间
	unsigned long long		m_local_time;				/// 本地接收时间
	qt_price_quantity_unit	m_bid_lev_1;				/// 买向第一档
	qt_price_quantity_unit	m_bid_lev_5;				/// 买向第五档
	qt_price_quantity_unit	m_ask_lev_1;				/// 卖向第一档
	qt_price_quantity_unit	m_ask_lev_5;				/// 卖向第五档
	unsigned long long		m_bid_depth;				/// 委托买入总量，放大100倍
	unsigned long long		m_ask_depth;				/// 委托卖总量，放大100倍
};

struct sze_qt_node_turnover
{
	unsigned int			m_sequence;					/// 盛立行情序号
	char					m_symbol[9];				/// 合约
	long long				m_quote_update_time;		/// 行情更新时间
	unsigned long long		m_local_time;				/// 本地接收时间
	long long				m_total_trade_num;			/// 成交笔数
	unsigned long long		m_total_quantity;			/// 成交量，放大100倍
	unsigned long long		m_total_value;				/// 成交金额，放大1000000倍
};

#pragma pack(pop)

class sze_report:public efh_sze_lev2_api_event, public efh_sze_lev2_api_depend
{
public:
	sze_report();
	~sze_report();
	bool init(socket_udp_param* param, int num);
	void set_tick_detach(bool enable);
	void run(int nic_type, bool b_report_quit = false, const char* symbol = "");
	void show();
	void close();

protected:
	virtual void on_report_efh_sze_lev2_after_close(sze_hpf_after_close* p_close_px);
	virtual void on_report_efh_sze_lev2_snap(sze_hpf_lev2* p_snap);
	virtual void on_report_efh_sze_lev2_tick(int msg_type, sze_hpf_order* p_order, sze_hpf_exe* p_exe);
	virtual void on_report_efh_sze_lev2_idx(sze_hpf_idx* p_idx);
	virtual void on_report_efh_sze_lev2_tree(sze_hpf_tree* p_tree);
	virtual void on_report_efh_sze_lev2_ibr_tree(sze_hpf_ibr_tree* p_ibr_tree);
	virtual void on_report_efh_sze_lev2_turnover(sze_hpf_turnover* p_turnover);
	virtual void on_report_efh_sze_lev2_bond_snap(sze_hpf_bond_snap* p_bond);
	virtual void on_report_efh_sze_lev2_bond_tick(int msg_type, sze_hpf_bond_order* p_order, sze_hpf_bond_exe* p_exe);

	virtual void efh_sze_lev2_debug(const char* msg, int len);
	virtual void efh_sze_lev2_error(const char* msg, int len);
	virtual void efh_sze_lev2_info(const char* msg, int len);

	string format_str(const char* pFormat, ...);
	string get_sze_src_trading_phase_code(char ch_sl_trading_phase_code);

private:
	void report_efh_sze_lev2_order();
	void report_efh_sze_lev2_exe();
	void report_efh_sze_lev2_after_close();
	void report_efh_sze_lev2_snap();
	void report_efh_sze_lev2_idx();
	void report_efh_sze_lev2_tree();
	void report_efh_sze_lev2_ibr_tree();
	void report_efh_sze_lev2_turnover();
	void report_efh_sze_lev2_bond_snap();
	void report_efh_sze_lev2_bond_order();
	void report_efh_sze_lev2_bond_exe();

private:
	bool						m_b_report_quit;
	bool						m_b_tick_detach;
	dll_handle_sl				m_h_core;
	i_efh_sze_lev2_api*			m_p_quote;
	FILE*						m_fp_lev2;
	FILE*						m_fp_idx;
	FILE*						m_fp_ord;
	FILE*						m_fp_exe;
	FILE*						m_fp_tick;
	FILE*						m_fp_close_px;
	FILE*						m_fp_tree;
	FILE*						m_fp_ibr_tree;
	FILE*						m_fp_turnover;
	FILE*						m_fp_bond_snap;
	FILE*						m_fp_bond_ord;
	FILE*						m_fp_bond_exe;
	FILE*						m_fp_bond_tick;
	vector<string>				m_vec_symbol;

	bool						m_b_snap_rollback_flag;
	bool						m_b_idx_rollback_flag;
	bool						m_b_tick_rollback_flag;
	bool						m_b_after_rollback_flag;
	bool						m_b_tree_rollback_flag;
	bool						m_b_ibr_tree_rollback_flag;
	bool						m_b_turnover_rollback_flag;
	bool						m_b_bond_snap_rollback_flag;
	bool						m_b_bond_tick_rollback_flag;

	long long					m_ll_snap_count_lost;
	long long					m_ll_idx_count_lost;
	long long					m_ll_tick_count_lost;
	long long					m_ll_after_count_lost;
	long long					m_ll_tree_count_lost;
	long long					m_ll_ibr_tree_count_lost;
	long long					m_ll_turnover_count_lost;
	long long 					m_ll_bond_snap_count_lost;
	long long 					m_ll_bond_tick_count_lost;

	unsigned int				m_i_snap_seq;
	unsigned int				m_i_idx_seq;
	unsigned int				m_i_tick_seq;
	unsigned int				m_i_after_seq;
	unsigned int				m_i_tree_seq;
	unsigned int				m_i_ibr_tree_seq;
	unsigned int				m_i_turnover_seq;
	unsigned int 				m_i_bond_snap_seq;
	unsigned int 				m_i_bond_tick_seq;

	long long					m_ll_snap_count;
	long long					m_ll_idx_count;
	long long					m_ll_order_count;
	long long					m_ll_exe_count;
	long long					m_ll_after_count;
	long long					m_ll_tree_count;
	long long					m_ll_ibr_tree_count;
	long long					m_ll_turnover_count;
	long long 					m_ll_bond_snap_count;
	long long 					m_ll_bond_order_count;
	long long 					m_ll_bond_exe_count;

	sze_qt_node_snap*			m_snap;
	sze_qt_node_index*			m_idx;
	sze_qt_node_order*			m_order;
	sze_qt_node_exe*			m_exe;
	sze_qt_node_after_close*	m_after_close;
	sze_qt_node_tree*			m_tree;
	sze_qt_node_ibr_tree*		m_ibr_tree;
	sze_qt_node_turnover*		m_turnover;
	sze_qt_node_snap*			m_bond_snap;
	sze_qt_node_order*			m_bond_order;
	sze_qt_node_exe*			m_bond_exe;
};
