#pragma once
#include "WtOrdMon.h"
#include "../Includes/ExecuteDefs.h"
#include "../Share/StdUtils.hpp"

USING_NS_WTP;

class WtSpreadTradingExeUnit : public ExecuteUnit
{
public:
	WtSpreadTradingExeUnit();
	virtual ~WtSpreadTradingExeUnit();

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
	
	/*�ҵ���ʱ���
	*/
	virtual void check_sticky(uint32_t localid);

	/*
	*	�����ر�
	*	localid	���ص���
	*	code	��Լ����
	*	isBuy	��or��
	*	leftoverʣ������
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
	*	�����µ���Ŀ�������Ȳ�λ
	*	code	��Լ����
	*	newVol	�µ�Ŀ�������Ȳ�λ
	*/
	virtual void set_position(const char* stdCode, double newVol) override;
	/*
	*	����ȫ���ֲ�
	*	stdCode	��Լ����
	*/
	virtual void clear_all_position(const char* stdCode) override;
	//��鶩���Ƿ����
	bool is_order_finish();

	double calculate_spread_volume(const char * stdcode, double leg_volume);


	bool is_hedge_finsh();

	void hedge_passive_legs();

	void deal_active_leg(const char * stdCode);

	void send_leg_order(const char * stdCode, double vol);

	/*
	*	����ͨ�������ص�
	*/
	virtual void on_channel_ready() override;

	/*
	*	����ͨ����ʧ�ص�
	*/
	virtual void on_channel_lost() override;

	typedef struct Leg_Infoion {
		// �۸����&&���׳���  ���ⲿ���룬�ϲ�Ϊ���׳�������explain
		double		_trade_multiplier;	//���׳���
		double		_min_open_lots;		//��С��������
		double		_order_lots;		//���η�������
	};

	vector<Leg_Infoion>passivelegs;
	typedef std::shared_ptr<Leg_Infoion>	LegPtr;
	unordered_map<string, LegPtr> LegInfoMap;
	LegPtr	v_pLeg_Infoion;

private:
	uint32_t		 active_localid;			//�����ȶ���localid
	uint32_t		 passtive_localid;			//�����ȶ���localid	
	WTSTickData*	 _last_tick;				//��һ������
	WTSTickData*	active_last_tick;			//��������һ������
	WTSTickData*	B_last_tick;				//B����������һ������
	double			_active_final_target_pos;	//��Ŀ�������Ȳ�λ
	double			_active_target_pos;			//ÿ�ݵ�������Ŀ���λ
	uint32_t		_target_portion;			//Ŀ��ִ�з���
	bool			_channel_ready;
	double			B_target_pos;				//ÿ��ִ�е�B������Ŀ���λ
	double			_price_multiplier;			//�۸����
	double			_trading_multiplier;		//���׳��� 
	double			_Spread_Price;				//�۲��First tick ʱ�����
	uint32_t		_error_times;				//�µ��������
	uint64_t		_error_nowTM;				//ί�д���ʱ��
	StdUniqueMutex	_mtx_calc;

	WTSCommodityInfo* _comm_info;
	WTSSessionInfo*	_sess_info;
	uint32_t		_cancel_times;//��������
	const char* A_stdCode;
	const char* B_stdCode;
	//////////////////////////////////////////////////////////////////////////
	//ִ�в���
	WtOrdMon		_orders_mon;
	uint32_t		_cancel_cnt;//��;������
	//////////////////////////////////////////////////////////////////////////
	//����
	uint32_t		_total_secs;	//ִ����ʱ��,��λs
	uint32_t		_total_times;	//��ִ�д���
	uint32_t		_tail_secs;		//ִ��β��ʱ��
	uint32_t		_ord_sticky;	//�ҵ�ʱ��,��λs
	uint32_t		_price_mode;	//�۸�ģʽ: 0-���¼�,1-���ż�,2-���ּ�
	uint32_t		_price_offset;	//�ҵ��۸�ƫ��,����ڼ����۸�ƫ��,��+��-
	uint32_t        _begin_time;	//��ʼʱ�� ��1000-��10:00��
	uint32_t		_end_time;		//����ʱ�� ��1030-��10:30��
	double			_min_open_lots;	//��С��������
	double			_order_lots;	//���η�������
	bool			isCanCancel;
	//////////////////////////////////////////////////////////////////////////
	//��ʱ����
	double			_this_target;	//����Ŀ���λ
	uint32_t		_fire_span;		//�������//ms
	uint32_t		_fired_times;	//��ִ�д���
	uint64_t		_last_fire_time; //�ϴ���ִ�е�ʱ��
	uint64_t		_last_place_time;//�ϸ��µ�ʱ��
	uint64_t		_last_tick_time;//�ϸ�tickʱ��
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
};



