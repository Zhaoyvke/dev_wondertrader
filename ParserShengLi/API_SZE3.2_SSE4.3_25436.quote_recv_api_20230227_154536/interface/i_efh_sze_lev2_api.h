/*!****************************************************************************
@node		Copyright (coffee), 2005-2020, Shengli Tech. Co., Ltd.
@file		i_efh_sze_lev2_api.h
@date		2020/12/14 08:30
@author		shengli

@brief		本接口实现深圳证券交易所盛立行情接收，支持缓存输出，支持三类网卡
			init_sze失败后需调用close_sze，不得直接调用start_sze
******************************************************************************/
#pragma once
#include "efh_lev2_define.h"
#include "sze_hpf_define.h"

class efh_sze_lev2_api_event
{
public:
	virtual ~efh_sze_lev2_api_event() {}
	/*
	@上报深交所盘后定价数据
	@param p_after_close: 盘后定价结构
	*/
	virtual void on_report_efh_sze_lev2_after_close(sze_hpf_after_close* p_after_close) = 0;
	/*
	@上报lev2快照数据
	@param p_lev2: lev2快照结构
	*/
	virtual void on_report_efh_sze_lev2_snap(sze_hpf_lev2* p_snap) = 0;
	/*
	@上报逐笔订单数据
	@param msg_type: 消息类型
	@param p_order: 订单结构
	@param p_exe:	成交类型
	*/
	virtual void on_report_efh_sze_lev2_tick(int msg_type, sze_hpf_order* p_order, sze_hpf_exe* p_exe) = 0;
	/*
	@上报指数数据
	@param p_idx:	指数数据结构
	*/
	virtual void on_report_efh_sze_lev2_idx(sze_hpf_idx* p_idx) = 0;
	/*
	@上报全新快照数据
	@param p_tree:	全新快照结构
	*/
	virtual void on_report_efh_sze_lev2_tree(sze_hpf_tree* p_tree) = 0;
	/*
	@上报ibr建树快照数据
	@param p_ibr_tree:	ibr建树快照结构
	*/
	virtual void on_report_efh_sze_lev2_ibr_tree(sze_hpf_ibr_tree* p_ibr_tree) = 0;

	/*
	@上报成交量统计快照数据
	@param p_turnover:	成交量统计快照结构
	*/
	virtual void on_report_efh_sze_lev2_turnover(sze_hpf_turnover* p_turnover) = 0;

	/*
	@上报债券快照消息类型
	@param p_bond:	债券快照结构
	*/
	virtual void on_report_efh_sze_lev2_bond_snap(sze_hpf_bond_snap* p_bond) = 0;

	/*
	@上报债券逐笔订单数据
	@param msg_type: 消息类型
	@param p_order: 订单结构
	@param p_exe:	成交类型
	*/
	virtual void on_report_efh_sze_lev2_bond_tick(int msg_type, sze_hpf_bond_order* p_order, sze_hpf_bond_exe* p_exe) = 0;

};

class efh_sze_lev2_api_depend
{
public:
	virtual ~efh_sze_lev2_api_depend() {}
	/// 记录日志
	/// 上报调试信息
	virtual void efh_sze_lev2_debug(const char* msg, int len) {}
	/// 上报错误
	virtual void efh_sze_lev2_error(const char* msg, int len) {}
	/// 上报相关信息
	virtual void efh_sze_lev2_info(const char* msg, int len) {}
};

class i_efh_sze_lev2_api
{
public:
	virtual ~i_efh_sze_lev2_api(void) {}

	/*
	@获取版本信息
	*/
	virtual void get_version(efh_version& version) = 0;
	/*
	@初始化接口
	@param p_udp_param: 通道配置数组
	@param num: 配置数组个数
	@param p_event:数据上报指针
	@param p_depend：日志指针
	*/
	virtual bool init_sze(socket_udp_param* p_udp_param, int num, efh_sze_lev2_api_event* p_event, efh_sze_lev2_api_depend* p_depend = NULL) = 0;
	/*
	@启动接口
	@param net_type: 使用的网卡类型
	@note: 需要在init_sze成功后才能调用
	*/
	virtual bool start_sze(enum_efh_nic_type nic_type) = 0;
	/*
	@停止接口
	*/
	virtual void stop_sze() = 0;
	/*
	@brief: 关闭
	*/
	virtual void close_sze() = 0;
};

#define CREATE_EFH_SZE_LEV2_API_FUNCTION		"create_efh_sze_lev2_api_function"
#define DESTROY_EFH_SZE_LEV2_API_FUNCTION		"destroy_efh_sze_lev2_api_function"

typedef i_efh_sze_lev2_api* (*func_create_efh_sze_lev2_api)();
typedef void (*func_destroy_efh_sze_lev2_api)(i_efh_sze_lev2_api*);
