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
	*	所属执行器工厂名称
	*/
	virtual const char* getFactName() override;

	/*
	*	执行单元名称
	*/
	virtual const char* getName() override;

	/*
	*	初始化执行单元
	*	ctx		执行单元运行环境
	*	code	管理的合约代码
	*/
	virtual void init(ExecuteContext* ctx, const char* stdCode, WTSVariant* cfg) override;
	
	/*挂单超时检查
	*/
	virtual void check_sticky(uint32_t localid);

	/*
	*	订单回报
	*	localid	本地单号
	*	code	合约代码
	*	isBuy	买or卖
	*	leftover剩余数量
	*	price	委托价格
	*	isCanceled	是否已撤销
	*/
	virtual void on_order(uint32_t localid, const char* stdCode, bool isBuy, double leftover, double price, bool isCanceled) override;

	/*
	*	tick数据回调
	*	newTick	最新的tick数据
	*/
	virtual void on_tick(WTSTickData* newTick) override;

	/*
	*	成交回报
	*	code	合约代码
	*	isBuy	买or卖
	*	vol		成交数量,这里没有正负,通过isBuy确定买入还是卖出
	*	price	成交价格
	*/
	virtual void on_trade(uint32_t localid, const char* stdCode, bool isBuy, double vol, double price) override;

	/*
	*	下单结果回报
	*/
	virtual void on_entrust(uint32_t localid, const char* stdCode, bool bSuccess, const char* message) override;

	/*
	*	设置新的总目标主动腿仓位
	*	code	合约代码
	*	newVol	新的目标主动腿仓位
	*/
	virtual void set_position(const char* stdCode, double newVol) override;
	/*
	*	清理全部持仓
	*	stdCode	合约代码
	*/
	virtual void clear_all_position(const char* stdCode) override;
	//检查订单是否完成
	bool is_order_finish();

	double calculate_spread_volume(const char * stdcode, double leg_volume);


	bool is_hedge_finsh();

	void hedge_passive_legs();

	void deal_active_leg(const char * stdCode);

	void send_leg_order(const char * stdCode, double vol);

	/*
	*	交易通道就绪回调
	*/
	virtual void on_channel_ready() override;

	/*
	*	交易通道丢失回调
	*/
	virtual void on_channel_lost() override;

	typedef struct Leg_Infoion {
		// 价格乘数&&交易乘数  由外部传入，合并为交易乘数，见explain
		double		_trade_multiplier;	//交易乘数
		double		_min_open_lots;		//最小开仓数量
		double		_order_lots;		//单次发单手数
	};

	vector<Leg_Infoion>passivelegs;
	typedef std::shared_ptr<Leg_Infoion>	LegPtr;
	unordered_map<string, LegPtr> LegInfoMap;
	LegPtr	v_pLeg_Infoion;

private:
	uint32_t		 active_localid;			//主动腿订单localid
	uint32_t		 passtive_localid;			//被动腿订单localid	
	WTSTickData*	 _last_tick;				//上一笔行情
	WTSTickData*	active_last_tick;			//主动腿上一笔行情
	WTSTickData*	B_last_tick;				//B被动动腿上一笔行情
	double			_active_final_target_pos;	//总目标主动腿仓位
	double			_active_target_pos;			//每份的主动腿目标仓位
	uint32_t		_target_portion;			//目标执行份数
	bool			_channel_ready;
	double			B_target_pos;				//每份执行的B被动腿目标仓位
	double			_price_multiplier;			//价格乘数
	double			_trading_multiplier;		//交易乘数 
	double			_Spread_Price;				//价差，在First tick 时候计算
	uint32_t		_error_times;				//下单错误次数
	uint64_t		_error_nowTM;				//委托错误时间
	StdUniqueMutex	_mtx_calc;

	WTSCommodityInfo* _comm_info;
	WTSSessionInfo*	_sess_info;
	uint32_t		_cancel_times;//撤单次数
	const char* A_stdCode;
	const char* B_stdCode;
	//////////////////////////////////////////////////////////////////////////
	//执行参数
	WtOrdMon		_orders_mon;
	uint32_t		_cancel_cnt;//在途撤单量
	//////////////////////////////////////////////////////////////////////////
	//参数
	uint32_t		_total_secs;	//执行总时间,单位s
	uint32_t		_total_times;	//总执行次数
	uint32_t		_tail_secs;		//执行尾部时间
	uint32_t		_ord_sticky;	//挂单时限,单位s
	uint32_t		_price_mode;	//价格模式: 0-最新价,1-最优价,2-对手价
	uint32_t		_price_offset;	//挂单价格偏移,相对于几乎价格偏移,买+卖-
	uint32_t        _begin_time;	//开始时间 （1000-》10:00）
	uint32_t		_end_time;		//结束时间 （1030-》10:30）
	double			_min_open_lots;	//最小开仓数量
	double			_order_lots;	//单次发单手数
	bool			isCanCancel;
	//////////////////////////////////////////////////////////////////////////
	//临时变量
	double			_this_target;	//本轮目标仓位
	uint32_t		_fire_span;		//发单间隔//ms
	uint32_t		_fired_times;	//已执行次数
	uint64_t		_last_fire_time; //上次已执行的时间
	uint64_t		_last_place_time;//上个下单时间
	uint64_t		_last_tick_time;//上个tick时间
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



