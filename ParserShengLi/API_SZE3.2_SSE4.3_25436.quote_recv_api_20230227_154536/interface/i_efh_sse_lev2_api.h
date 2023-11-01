/*!****************************************************************************
@node		Copyright (coffee), 2005-2020, Shengli Tech. Co., Ltd.
@file		i_efh_sse_lev2_api.h
@date		2020/12/14 08:30
@author		shengli

@brief		本接口实现上海证券交易所盛立行情接收，支持缓存输出，支持三类网卡
			init_sse失败后需调用close_sse，不得直接调用start_sse
******************************************************************************/
#pragma once
#include "efh_lev2_define.h"
#include "sse_hpf_define.h"

class efh_sse_lev2_api_event
{
public:
	virtual ~efh_sse_lev2_api_event() {}
	/*
	@上报上交所快照数据
	@param p_snap:  lev2 快照数据结构
	*/
	virtual void on_report_efh_sse_lev2_snap(sse_hpf_lev2* p_snap) = 0;
	/*
	@上报指数数据
	@param p_idx:	指数数据结构
	*/
	virtual void on_report_efh_sse_lev2_idx(sse_hpf_idx* p_idx) = 0;

	/*
	@上报委托订单数据
	@param msg_type: 消息类型
	@param p_order: 订单结构
	@param p_exe:	成交类型
	*/
	virtual void on_report_efh_sse_lev2_tick(int msg_type, sse_hpf_order* p_order, sse_hpf_exe* p_exe) = 0;

	/*
	@上报建树消息类型
	@param p_tree:	建树结构
	*/
	virtual void on_report_efh_sse_lev2_tree(sse_hpf_tree* p_tree) = 0;

	/*
	@上报期权消息类型
	@param p_opt:	期权结构
	*/
	virtual void on_report_efh_sse_lev2_option(sse_hpf_stock_option* p_opt) = 0;

	/*
	@上报债券快照消息类型
	@param p_bond:	债券快照结构
	*/
	virtual void on_report_efh_sse_lev2_bond_snap(sse_hpf_bond_snap* p_bond) = 0;

	/*
	@上报债券逐笔消息类型
	@param p_tick:	债券逐笔结构
	*/
	virtual void on_report_efh_sse_lev2_bond_tick(sse_hpf_bond_tick* p_tick) = 0;
	/*
	@上报逐笔合并消息类型
	@param p_tick:	逐笔合并结构
	*/
	virtual void on_report_efh_sse_lev2_tick_merge(sse_hpf_tick_merge* p_tick) = 0;
	/*
	@上报 ETF 统计消息类型
	@param p_tick:	ETF 统计结构
	*/
	virtual void on_report_efh_sse_lev2_etf(sse_hpf_etf* p_tick) = 0;
};

class efh_sse_lev2_api_depend
{
public:
	virtual ~efh_sse_lev2_api_depend() {}
	/// 记录日志
	/// 上报调试信息
	virtual void efh_sse_lev2_debug(const char* msg, int len) {}
	/// 上报错误
	virtual void efh_sse_lev2_error(const char* msg, int len) {}
	/// 上报相关信息
	virtual void efh_sse_lev2_info(const char* msg, int len) {}
};

class i_efh_sse_lev2_api
{
public:
	virtual ~i_efh_sse_lev2_api(void) {}

	/*
	@获取版本信息
	*/
	virtual void get_version(efh_version& version) = 0;
	/*
	@ 获取静态信息
	*/
	virtual int get_static_info(const char* symbol ,sse_static_msg_body& static_info) = 0;
	/*
	@初始化接口
	@param p_udp_param: 通道配置数组
	@param num: 配置数组个数
	@param p_event:数据上报指针
	@param p_depend：日志指针
	*/
	virtual bool init_sse(socket_udp_param* p_udp_param, int num, efh_sse_lev2_api_event* p_event, efh_sse_lev2_api_depend* p_depend = NULL) = 0;
	/*
	@启动接口
	@param net_type: 使用的网卡类型
	*/
	virtual bool start_sse(enum_efh_nic_type nic_type) = 0;
	/*
	@停止接口
	*/
	virtual void stop_sse() = 0;
	/*
	@brief: 关闭
	*/
	virtual void close_sse() = 0;

};

#define CREATE_EFH_SSE_LEV2_API_FUNCTION		"create_efh_sse_lev2_api_function"
#define DESTROY_EFH_SSE_LEV2_API_FUNCTION		"destroy_efh_sse_lev2_api_function"

typedef i_efh_sse_lev2_api* (*func_create_efh_sse_lev2_api)();
typedef void (*func_destroy_efh_sse_lev2_api)(i_efh_sse_lev2_api*);
