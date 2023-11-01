#pragma once
/*
23.6.2--zhaoyk--StockVWAP
*/
#include "WtOrdMon.h"
#include "../Includes/ExecuteDefs.h"
#include "../Share/StrUtil.hpp"
#include <fstream>
#include "../Share/TimeUtils.hpp"
#include "../Includes/WTSVariant.hpp"
#include "../Includes/WTSContractInfo.hpp"
#include "../Includes/WTSSessionInfo.hpp"
#include "../Share/decimal.h"
#include "../Share/fmtlib.h"
USING_NS_WTP;

class WtStockVWapExeUnit : public ExecuteUnit {

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
	WtStockVWapExeUnit();
	virtual ~WtStockVWapExeUnit();

private:
	void	do_calc();
	void	fire_at_once(double qty);

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
	*	����ͨ�������ص�
	*/
	virtual void on_channel_ready() override;

	/*
	*	����ͨ����ʧ�ص�
	*/
	virtual void on_channel_lost() override;

	virtual void clear_all_position(const char* stdCode) override;
private:
	WTSTickData* _last_tick;	//��һ������
	double		_target_pos;	//Ŀ���λ
	double		_target_amount;  // Ŀ����
	double		_avaliable{ 0 }; // �˻�����
	bool		_channel_ready;
	StdUniqueMutex	_mtx_calc;

	WTSCommodityInfo* _comm_info;
	WTSSessionInfo*	_sess_info;
	uint32_t	_cancel_times;//��������



	//////////////////////////////////////////////////////////////////////////
	//ִ�в���
	WtOrdMon		_orders_mon;
	uint32_t		_cancel_cnt;
	vector<double>	VwapAim;			//���Ӽǣ�Ŀ��VWapԤ�� �ܱ�����
	//////////////////////////////////////////////////////////////////////////
	//����
	uint32_t		_total_secs;	//ִ����ʱ��,��λs
	uint32_t		_total_times;	//��ִ�д���
	uint32_t		_tail_secs;		//ִ��β��ʱ��
	uint32_t		_ord_sticky;	//�ҵ�ʱ��,��λs
	uint32_t		_price_mode;	//�۸�ģʽ: 0-���¼�,1-���ż�,2-���ּ�
	uint32_t		_price_offset;	//�ҵ��۸�ƫ��,����ڼ����۸�ƫ��,��+��-
	uint32_t        _begin_time;	//��ʼʱ�� ��1000->10:00��
	uint32_t		_end_time;		//����ʱ�� ��1030->10:30��
	double			_min_open_lots; //��С��������
	double			_order_lots;	//���η�������
	bool			_is_KC{ false };
	bool			isCanCancel;
	TargetMode		_target_mode{ TargetMode::stocks };
	bool			_is_clear;
	double			_min_hands{ 0 };
	double			_start_price{ 0 };
	double			_is_t0{ false }; // ����תծ����˵�������Ҫ��true����ƱΪfalse
	bool			_is_finish;
	uint64_t		_start_time;
	//////////////////////////////////////////////////////////////////////////
	//��ʱ����
	double			_this_target;	//����Ŀ���λ
	uint32_t		_fire_span;		//�������//ms
	uint32_t		_fired_times;	//��ִ�д���
	uint64_t		_last_fire_time; //�ϴ���ִ�е�ʱ��
	uint64_t		_last_place_time;//�ϸ��µ�ʱ��
	uint64_t		_last_tick_time;//�ϸ�tickʱ��
	double			_Vwap_vol;		//vwap��λʱ���µ���
	double			_Vwap_prz;		//vwap�۸�
	
	std::atomic<bool> _in_calc;

	typedef struct _CalcFlag
	{
		bool _result;
		std::atomic<bool>* _flag;
		_CalcFlag(std::atomic<bool>*flag) :_flag(flag) {
			_result = _flag->exchange(true, std::memory_order_acq_rel);
		}

		~_CalcFlag() {
			if (_flag)
				_flag->exchange(false, std::memory_order_acq_rel);
		}
		operator bool() const { return _result; }
	}CalcFlag;
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