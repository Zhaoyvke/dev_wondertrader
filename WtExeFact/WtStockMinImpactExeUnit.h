/*!
 * \file WtStockMinImpactExeUnit.h
 *
 * \author Wesley
 * \date 2020/03/30
 *
 * ��С���ִ�е�Ԫ
 */
#pragma once
#include "../Includes/ExecuteDefs.h"
#include "WtOrdMon.h"
#include "../Includes/WTSVariant.hpp"
#include "../Includes/WTSContractInfo.hpp"
#include "../Includes/WTSSessionInfo.hpp"
#include "../Share/decimal.h"
#include "../Share/StrUtil.hpp"
#include "../Share/fmtlib.h"

USING_NS_WTP;

#define BESTPX -1 // ��������
#define LASTPX 0  // ���¼�
#define MARKET 1  // ���ּ�
#define AUTOPX 2  // �Զ�

typedef std::function<void(const char*, bool, double, double, double, double)> FuncEnumChnlPosCallBack;

class WtStockMinImpactExeUnit : public ExecuteUnit
{
private:
	const char* cbondStr = "CBOND";
	const char* stockStr = "STK";

	enum class TargetMode
	{
		stocks = 0,
		amount,
		ratio,
	};

private:
	std::vector<std::string> PriceModeNames = {
		"BESTPX",		//���ż�
		"LASTPX",		//���¼�
		"MARKET",		//���ּ�
		"AUTOPX"		//�Զ�
	};

public:
	WtStockMinImpactExeUnit();
	virtual ~WtStockMinImpactExeUnit();

private:
	void	do_calc();
	bool	is_clear();
public:
	/*
	 *	����ִ������������
	 */
	virtual const char* getFactName() override;

	/*
	 *	ִ�е�Ԫ����
	 */
	virtual const char* getName() override;

	/*
	 *	��ʼ��ִ�е�Ԫ
	 *	ctx		ִ�е�Ԫ���л���
	 *	code	����ĺ�Լ����
	 */
	virtual void init(ExecuteContext* ctx, const char* stdCode, WTSVariant* cfg) override;

	/*
	 *	�����ر�
	 *	localid	���ص���
	 *	code	��Լ����
	 *	isBuy	��or��
	 *	leftover	ʣ������
	 *	price	ί�м۸�
	 *	isCanceled	�Ƿ��ѳ���
	 */
	virtual void on_order(uint32_t localid, const char* stdCode, bool isBuy, double leftover, double price, bool isCanceled) override;

	/*
	 *	tick���ݻص�
	 *	newTick	���µ�tick����
	 */
	virtual void on_tick(WTSTickData* newTick) override;

	/*
	 *	�ɽ��ر�
	 *	code	��Լ����
	 *	isBuy	��or��
	 *	vol		�ɽ�����,����û������,ͨ��isBuyȷ�����뻹������
	 *	price	�ɽ��۸�
	 */
	virtual void on_trade(uint32_t localid, const char* stdCode, bool isBuy, double vol, double price) override;

	/*
	 *	�µ�����ر�
	 */
	virtual void on_entrust(uint32_t localid, const char* stdCode, bool bSuccess, const char* message) override;

	/*
	 *	�����µ�Ŀ���λ
	 *	code	��Լ����
	 *	newVol	�µ�Ŀ���λ
	 */
	virtual void set_position(const char* stdCode, double newVol) override;

	/*
	 *	����ȫ���ֲ�
	 *	stdCode	��Լ����
	 */
	virtual void clear_all_position(const char* stdCode) override;

	/*
	 *	����ͨ�������ص�
	 */
	virtual void on_channel_ready() override;

	/*
	 *	����ͨ����ʧ�ص�
	 */
	virtual void on_channel_lost() override;

	/*
	 *	�˻���Ϣ�ص�
	 */
	virtual void on_account(const char* currency, double prebalance, double balance, double dynbalance, double avaliable, double closeprofit, double dynprofit, double margin, double fee, double deposit, double withdraw) override;

private:
	void check_unmanager_order();

private:
	WTSTickData* _last_tick;	//��һ������
	double		_target_pos;	//Ŀ���λ
	double		_target_amount;  // Ŀ����
	double		_target_ratio;  // Ŀ��ֱֲ���

	double		_avaliable{ 0 }; // �˻�����

	StdUniqueMutex	_mtx_calc;
	WTSCommodityInfo* _comm_info;
	WTSSessionInfo* _sess_info;

	//////////////////////////////////////////////////////////////////////////
	//ִ�в���
	int32_t		_price_offset;
	uint32_t	_expire_secs;
	int32_t		_price_mode;
	uint32_t	_entrust_span;
	bool		_by_rate;
	double		_order_lots;
	double		_qty_rate;
	double		_min_order;
	bool		_is_finish;
	uint64_t	_start_time;
	double		_start_price{ 0 };
	bool		_is_first_tick{ true };
	double		_max_cancel_time{ 3 }; //��󳷵�����������������������Ȼδ��������˵���Ǵ�
	double		_total_money{ -1 }; // ���ʱ�
	double		_is_t0{ false }; // ����תծ����˵�������Ҫ��true����ƱΪfalse
	faster_hashmap< uint32_t, uint32_t > _cancel_map{};

	WtOrdMon	_orders_mon;
	//uint32_t	_cancel_cnt;
	uint32_t	_cancel_times;
	bool		_is_cancel_unmanaged_order{ true };
	uint64_t	_last_place_time;
	uint64_t	_last_tick_time;
	bool		_is_clear;
	TargetMode  _target_mode{ TargetMode::stocks };
	bool		_is_KC{ false };			// �Ƿ��ǿƴ����Ʊ
	double		_min_hands{ 0 };
	bool		_is_ready{ false };
	bool		_is_total_money_ready{ false };
	std::map<std::string, double> _market_value{};
	uint64_t _now;

public:
	inline int round_hands(double hands, double min_hands)
	{
		return (int)((hands + min_hands / 2) / min_hands) * min_hands;
	}

	inline double get_minOrderQty(std::string stdCode)
	{
		int code = std::stoi(StrUtil::split(stdCode, ".")[2]);
		bool is_KC = false;
		if (code >= 688000)
		{
			is_KC = true;
		}
		WTSCommodityInfo* comm_info = _ctx->getCommodityInfo(stdCode.c_str());
		double min_order = 1.0;
		if (strcmp(comm_info->getProduct(), cbondStr) == 0)
			min_order = 10.0;
		else if (strcmp(comm_info->getProduct(), stockStr) == 0)
			if (is_KC)
				min_order = 200.0;
			else
				min_order = 100.0;
		if (comm_info)
			comm_info->release();
		return min_order;
	}
};

