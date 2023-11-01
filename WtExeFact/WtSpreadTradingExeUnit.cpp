#include "WtSpreadTradingExeUnit.h"

#include "../Share/TimeUtils.hpp"
#include "../Includes/WTSVariant.hpp"
#include "../Includes/WTSContractInfo.hpp"
#include "../Share/decimal.h"
#include "../Share/fmtlib.h"
#include "../Includes/WTSSessionInfo.hpp"

extern const char* FACT_NAME;

WtSpreadTradingExeUnit::WtSpreadTradingExeUnit()
	: _last_tick(NULL)
	, active_last_tick(NULL)
	, B_last_tick(NULL)
	, _comm_info(NULL)
	, _ord_sticky(0)
	, _cancel_cnt(0)
	, _channel_ready(false)
	, _last_fire_time(0)
	//, _fired_times(0)
	//, _total_times(0)
	, _total_secs(0)
	, _price_mode(0)
	, _price_offset(0)
	, _active_target_pos(0)
	, _active_final_target_pos(0)
	, B_target_pos(0)
	, _cancel_times(0)
	, _begin_time(0)
	, _end_time(0)
	, _last_place_time(0)
	, _last_tick_time(0)
	, isCanCancel{ true }
	, _target_portion(1)
	, _error_times(0)
	,passtive_localid(0) //初始化为0，应用在交易通带建立时候校验是第一次建立交易通道还是断开后重连
	,active_localid(0)

	/*
		// 考虑改正，价格乘数&& 交易乘数都 用 trading_multiplier 代替   传入前计算
	*/


	,_price_multiplier(1)//价格乘数
	,_trading_multiplier(1)//交易乘数
	,_Spread_Price(-1)//价差初始化为-1，因为可能计算出为0
{
}


WtSpreadTradingExeUnit::~WtSpreadTradingExeUnit()
{
	if (_last_tick)
		_last_tick->release();

	if (_comm_info)
		_comm_info->release();
	
}

inline double get_real_target(double _target) {
	if (_target == DBL_MAX)
		return 0;

	return _target;
}
inline uint32_t calTmSecs(uint32_t begintime, uint32_t endtime) //计算执行时间：s
{
	return   ((endtime / 100) * 3600 + (endtime % 100) * 60) - ((begintime / 100) * 3600 + (begintime % 100) * 60);

}
inline bool is_clear(double target)
{
	return (target == DBL_MAX);
}



const char* WtSpreadTradingExeUnit::getFactName()
{
	return FACT_NAME;
}

const char* WtSpreadTradingExeUnit::getName()
{
	return "WtSpreadTradingExeUnit";
}

void WtSpreadTradingExeUnit::init(ExecuteContext* ctx, const char* stdCode, WTSVariant* cfg)
{
	ExecuteUnit::init(ctx, stdCode, cfg);

	_comm_info = ctx->getCommodityInfo(stdCode);//获取品种参数
	if (_comm_info)
		_comm_info->retain();
	_sess_info = ctx->getSessionInfo(stdCode);//获取交易时间模板信息
	if (_sess_info)
		_sess_info->retain();
	_ord_sticky = cfg->getUInt32("ord_sticky");
	_begin_time = cfg->getUInt32("begin_time");
	_end_time = cfg->getUInt32("end_time");
	_total_secs = cfg->getUInt32("total_secs");
	_tail_secs = cfg->getUInt32("tail_secs");
	_total_times = cfg->getUInt32("total_times");
	_price_mode = cfg->getUInt32("price_mode");
	_price_offset = cfg->getUInt32("price_offset");
	//_order_lots = cfg->getDouble("lots");									//单次发单手数
	_active_final_target_pos = cfg->getUInt32("_active_final_target_pos");	//主动腿总执行目标仓位
	_target_portion = cfg->getUInt32("target_portion");						//价差合约总执行份数

	//若通过列表传入多passiveLegs，可Pushback Legs
	A_stdCode = stdCode;
	B_stdCode = BstdCode;
	vector<Leg_Infoion>passivelegs;
	
	//SpreadDataStruct* m_SpreadDataStruct = new SpreadDataStruct;
	//Leg_Session*	v_pLeg_Session = new Leg_Session;

	v_pLeg_Infoion->_order_lots = cfg->getDouble("Alots");
	v_pLeg_Infoion->_trade_multiplier = cfg->getDouble("A_trade_multiplier");
	if (cfg->has("Aminopenlots"))			//在文件中若无特殊需要可设置为1
		v_pLeg_Infoion->_min_open_lots = cfg->getDouble("Aminopenlots");//最小开仓数量
	LegInfoMap[A_stdCode] = v_pLeg_Infoion;
	v_pLeg_Infoion->_order_lots = cfg->getDouble("Blots");
	v_pLeg_Infoion->_trade_multiplier = cfg->getDouble("B_trade_multiplier");
	if (cfg->has("Bminopenlots"))
		v_pLeg_Infoion->_min_open_lots = cfg->getDouble("Bminopenlots");//最小开仓数量
	LegInfoMap[B_stdCode] = v_pLeg_Infoion;
	/*
	还需要添加	B_leg  C_leg.....*/

	//m_SpreadDataStruct->LegInfoMap[A_stdCode] = v_pLeg_Session;
	
	/*
	策略此处传入的交易乘数和价格乘数统一化为各被动腿系数 如 A-r*mB,主动腿系数传入之前化为1处理
	
	豆油期货跨期套，其价格乘数和交易乘数均为1：1，即价差 = y2205 - y2209；买入1手价差等于买入1手y2205，同时卖出1手y2209完成对冲。

请注意，在多条腿并且期货合约规模不等时，构建价差合约会相对困难一些，如构建虚拟钢厂套利所用到的价差合约时，计算公式如下：

螺纹钢生产技艺是16吨铁矿石加上5吨焦炭练成10吨螺纹钢。

基于价格乘数的价差spread = 1* RB - 1.6*I - 0.5*O。

但是由于螺纹是10吨/手，铁矿石和焦炭都是100吨/手，所以他们交易乘数是1：10：10；

故基于最大公约数规则，其实际交易手数关系是每买入100手螺纹钢（1000吨），需要卖出16手铁矿石（1600吨）和5手焦炭（500吨）完成对冲。

*/

	_total_secs = calTmSecs(_begin_time, _end_time);							//执行总时间：秒
	_fire_span = (_total_secs - _tail_secs) / _total_times;						//单次发单时间间隔,要去掉尾部时间计算,这样的话,最后剩余的数量就有一个兜底发单的机制了
	ctx->writeLog(fmt::format("执行单元WtSpreadTradingExeUnit[{}] 初始化完成,订单超时 {} 秒,执行时限 {} 秒,收尾时间 {} 秒,间隔时间 {} 秒", stdCode, _ord_sticky, _total_secs, _tail_secs, _fire_span).c_str());
}
/*
	在各Leg的下单处理中，并对超时单撤单。非超时进行计算
*/
 void WtSpreadTradingExeUnit::check_sticky(uint32_t localid)
{
	uint64_t now = TimeUtils::getLocalTimeNow();
	bool hasCancel = false;
	if (_ord_sticky != 0 && _orders_mon.has_order(localid))
	{
		_orders_mon.check_orders(_ord_sticky, now, [this, &hasCancel](uint32_t localid) {
			if (_ctx->cancel(localid))
			{
				_cancel_cnt++;
				_ctx->writeLog(fmt::format("Order expired, cancelcnt updated to {}", _cancel_cnt).c_str());
				hasCancel = true; 
			}
		});
	}
	if (!hasCancel && (now - _last_fire_time >= _fire_span * 1000))
	{
		do_calc();
	}
}


void WtSpreadTradingExeUnit::on_order(uint32_t localid, const char* stdCode, bool isBuy, double leftover, double price, bool isCanceled)
{
	if (!_orders_mon.has_order(localid))
		return;
	//暂存主动、被动腿订单号
	stdCode == A_stdCode ? active_localid = localid : passtive_localid = localid;
	
	if (isCanceled || leftover == 0)
	{
		_orders_mon.erase_order(localid);

		if (_cancel_cnt > 0)
			_cancel_cnt--;

		_ctx->writeLog(fmt::format("Order {} updated cancelcnt -> {}", localid, _cancel_cnt).c_str());
	}
	if (leftover == 0 && !isCanceled)
	{
		_cancel_times = 0;
		_ctx->writeLog(fmtutil::format("Order {} has filled", localid));
	}
	/*23/6/27
		只做主被动两条腿超时撤单重发处理
		后续需要改动
		fire at once 里参数需要再init中处理后再用，之后修改
	*/	
	//如果全部订单已撤销,这个时候一般是遇到要超时撤单（挂单超时） 
	if (isCanceled && _cancel_cnt == 0)
	{
		if (localid == active_localid)
		{
			double A_realPos = _ctx->getPosition(stdCode);
			if (!decimal::eq(A_realPos, _active_target_pos))
			{
				_ctx->writeLog(fmtutil::format("Active order {} of {} canceled, re_fire will be done", localid, stdCode));
				_cancel_times++;
				fire_at_once(max(v_pLeg_Infoion->_min_open_lots, _active_target_pos - A_realPos)); 
			}
		}
		else 
		{
			double B_realPos = _ctx->getPosition(stdCode);
			if (!decimal::eq(B_realPos, B_target_pos))
			{
				_ctx->writeLog(fmtutil::format("Passitive order {} of {} canceled, re_fire will be done", localid, stdCode));
				_cancel_times++;
				fire_at_once(max(v_pLeg_Infoion->_min_open_lots, B_target_pos - B_realPos));
			}
		}
	}

	if (isCanceled&&_cancel_cnt != 0)
	{
		_ctx->writeLog(fmtutil::format("Order {} of {}  hasn't canceled, error will be return ", localid, stdCode));
		return;
	}
}

void WtSpreadTradingExeUnit::on_channel_ready()
{	
	_channel_ready = true;
	double A_undone = _ctx->getUndoneQty(A_stdCode.c_str());//主动腿未完成量
	double B_undone = _ctx->getUndoneQty(B_stdCode.c_str());//被动腿未完成量

	/*
	！！！！
	若初次ready,是否需要进行undone校验？
	以下未做undone校验
	*/
	/*23/6/26
		冗余	（改模板）
		只做主被动两条腿交易通道设置处理
		后续需要改动
	*/
	if (active_localid!=0||passtive_localid!=0) 
	{
		if (!decimal::eq(A_undone, 0)&&!_orders_mon.has_order(active_localid))
		{
			//这说明有未完成A单不在监控之中,先撤掉
			_ctx->writeLog(fmt::format("{} unmanaged orders of {}, cancel all", A_undone, A_stdCode).c_str());
			bool isBuy = (A_undone > 0);
			OrderIDs ids = _ctx->cancel(A_stdCode.c_str(), isBuy);
			_orders_mon.push_order(ids.data(), ids.size(), _ctx->getCurTime());
			_cancel_cnt += ids.size();
			_ctx->writeLog(fmt::format("Unmanaged order updated cancelcnt to {}", _cancel_cnt).c_str());
		}
		else if (!decimal::eq(B_undone, 0) && !_orders_mon.has_order(passtive_localid))
		{
			//这说明有B未完成单不在监控之中,先撤掉
			_ctx->writeLog(fmt::format("{} unmanaged orders of {}, cancel all", B_undone, B_stdCode).c_str());
			bool isBuy = (B_undone > 0);
			OrderIDs ids = _ctx->cancel(B_stdCode.c_str(), isBuy);
			_orders_mon.push_order(ids.data(), ids.size(), _ctx->getCurTime());
			_cancel_cnt += ids.size();
			_ctx->writeLog(fmt::format("Unmanaged order updated cancelcnt to {}", _cancel_cnt).c_str());
		}
		else if (decimal::eq(A_undone, 0) && _orders_mon.has_order(active_localid))
		{
		/*
		 *	By Wesey @ 2021.12.13
		 *	如果未完成单为0，但是OMS中是有订单的
		 *	说明OMS中是错单，需要清理掉，不然超时撤单就会出错
		 *	这种情况，一般是断线重连以后，之前下出去的订单，并没有真正发送到柜台
		 *	所以这里需要清理掉本地订单
		 */
		_ctx->writeLog(fmtutil::format("Local orders of {} not confirmed in trading channel, clear all", A_stdCode.c_str()));
		_orders_mon.clear_orders();
		}
		else if (decimal::eq(B_undone, 0) && _orders_mon.has_order(passtive_localid))
		{
			_ctx->writeLog(fmtutil::format("Local orders of {} not confirmed in trading channel, clear all", B_stdCode.c_str()));
			_orders_mon.clear_orders();
		}
		/*处理在交易通道恢复后，有一份A成交，B没成交，
		这种情况平仓A，之后再重新计算
		*/
		else if (A_undone == 0 &&B_undone == B_target_pos)
		{
			//平仓一份A
		}
	}
	else 
	{
		_ctx->writeLog(fmtutil::format("Unrecognized condition while channle ready,{:.2f} live orders of {} exists,local orders {}exist.{:.2f} live orders of {} exists,local orders {}exist", A_undone, A_stdCode.c_str(), _orders_mon.has_order(active_localid) ? "" : "not", B_undone, B_stdCode.c_str(), _orders_mon.has_order(passtive_localid) ? "" : "not"));
	}
	do_calc();
}

void WtSpreadTradingExeUnit::on_channel_lost()
{

}


void WtSpreadTradingExeUnit::on_tick(WTSTickData* newTick)
{
	if (newTick == NULL || _code.compare(newTick->code()) != 0)
		return;
	
	bool isFirstTick = false;
	if (_last_tick)
	{
		_last_tick->release();
	}
	else
	{
		//如果行情时间不在交易时间,这种情况一般是集合竞价的行情进来,下单会失败,所以直接过滤掉这笔行情
		if (_sess_info != NULL && !_sess_info->isInTradingTime(newTick->actiontime() / 100000))
			return;
		//主被动lasttick都为NULL，当主被动Legstick都得到时候，isFirstTick置为true，对firsttick进行spreadprice计算
		if (!active_last_tick || !B_last_tick) {
			if (newTick->code() == A_stdCode)
				active_last_tick = newTick;
			else if (newTick->code == B_stdCode)
				B_last_tick = newTick;
			return;
		}
		else {
			isFirstTick = true;
		}
	}
	//新的tick数据,要保留
	_last_tick = newTick;
	_last_tick->retain();

	if (isFirstTick)	//如果是第一笔tick,则检查目标仓位,不符合则下单
	{
		/* FirstTick 
		需要所有Legs'firsttick 都推送,确保所有腿的tick都有推送后，作为firsttick
		第一笔tick，作为价差的基本值
		spread_price =A-B
		Price_mode:最优价1		对盘价2		排队价0
		最优价：卖价（Ask1a-Bid1b）-买价（Bid1a-Ask1b）
		对盘价：卖价（Bid1a-Ask1b）-买价（Ask1a-Bid1b）
		排队价：后补
		*/
		double A_newVol = _active_target_pos;
		double B_newVol = B_target_pos;
		const char* A_stdCode = active_last_tick->code();
		const char* B_stdCode = B_last_tick->code();
		double A_undone = _ctx->getUndoneQty(A_stdCode);
		double B_undone = _ctx->getUndoneQty(B_stdCode);
		double A_realPos = _ctx->getPosition(A_stdCode);
		double B_realPos = _ctx->getPosition(B_stdCode);
		if		(_price_mode == 1)
		{		//未加 价格交易乘数
			_Spread_Price = (active_last_tick->askprice(1) - B_last_tick->bidprice(1)) - (B_last_tick->bidprice(1) - active_last_tick->askprice(1));
		
		}
		else if (_price_mode == 2)	
		{
			_Spread_Price = (B_last_tick->bidprice(1) - active_last_tick->askprice(1)) - (active_last_tick->askprice(1) - B_last_tick->bidprice(1));
		}
		else if (_price_mode == 0) 
		{//排队价后续补充
		}
		if ((!decimal::eq(A_newVol, A_undone + A_realPos))|| (!decimal::eq(B_newVol, B_undone + B_realPos))) //仓位变化要交易 
		{
			do_calc();
		}
	}
	/*VNPY 1.检查是否还有存在订单
			if_order_finish()
			2.检查对冲主动腿是否充分
			if not    is_hedge_finished()
				hedge_passive_legs
			3.检查tick有无初始化
			4.检查主动腿是否需要处理
				if self.direction == Direction.LONG:
					if self.spread.ask_price <= self.price: #spread 的买价< price
						self.take_active_leg()
				else:
					if self.spread.bid_price >= self.price: # sprad 的卖价< price
						self.take_active_leg()

		# 封装撤单函数  cancel_order(leg_localid)
	*/
	else  // 当不是FirstTick后，针对不同下单情况的处理
	{
		if (on_A_order)					//在主动腿的下单环节中
		{
			check_sticky(active_localid);
		}
		else if (on_B_order)			//在被动腿的下单环节中	
		{
			check_sticky(passtive_localid);
		}
	}
}

void WtSpreadTradingExeUnit::on_trade(uint32_t localid, const char* stdCode, bool isBuy, double vol, double price)
{
	double Sum_Act_vol = 0;
	double Sum_B_vol = 0;
	bool begin_B_passtive = false;
	bool begin_active = false;
	if (stdCode == A_stdCode && vol == _active_target_pos) //主动腿成交成功 && 成交量 == 每份主动腿目标成交量 
	{	
		/*标记可以开始交易被动腿
			记录交易份数
		*/
		begin_B_passtive = true;
		Sum_Act_vol += vol;

	}
	else if(stdCode == A_stdCode && vol < _active_target_pos)//主动腿交易量未达到目标，继续交易主动腿
	{
		/*1.标记继续交易主动腿
		2.在挂单间隔内，等待成交主动腿
		3.判定挂单超时，撤单处理，并触发追单
		*/
		begin_B_passtive = false;
		Sum_Act_vol += vol;
	}
	else if (stdCode == B_stdCode && vol == B_target_pos)//被动腿交易量达到目标，交易主动腿
	{
		 begin_active = true;
		/*1.标记可以开始交易主动腿
		  2.记录交易份数
		*/
		 Sum_B_vol += vol;
	}
	else if (stdCode == B_stdCode && vol < B_target_pos)//被动腿交易量未达到目标，继续交易被动腿
	{
		/*1.标记继续交易被动腿
		2.在挂单间隔内，等待成交被动腿
		3.判定挂单超时，撤单处理，并触发追单
		*/
		begin_active = false;
		Sum_B_vol += vol;
	}
}

void WtSpreadTradingExeUnit::on_entrust(uint32_t localid, const char* stdCode, bool bSuccess, const char* message)
{
	if (!bSuccess)
	{
		//如果不是我发出去的订单,我就不管了
		if (!_orders_mon.has_order(localid))
			return;

		_orders_mon.erase_order(localid);
		_error_times++;
		//下单错误>= 3次，停止下单，直到成功后恢复
		_ctx->writeLog(fmtutil::format(" Error order localid{} has error{} times, more than 3 times,wait for 30s", localid, _error_times));
		if (_error_times >= 3) {
			_error_nowTM = TimeUtils::getLocalTimeNow();
		}
		do_calc();
	}
	else if (bSuccess&&_error_times != 0) {
		_error_times = 0;
	}
}

void WtSpreadTradingExeUnit::fire_at_once(double qty)
{
	if (decimal::eq(qty, 0))
		return;

	_last_tick->retain();
	WTSTickData* curTick = _last_tick;
	const char* code = _code.c_str();
	uint64_t now = TimeUtils::getLocalTimeNow();
	bool isBuy = decimal::gt(qty, 0);
	double targetPx = 0;
	//根据价格模式设置,确定委托基准价格: 0-最新价,1-最优价,2-对手价
	if (_price_mode == 0)
	{
		targetPx = curTick->price();
	}
	else if (_price_mode == 1)
	{
		targetPx = isBuy ? curTick->bidprice(0) : curTick->askprice(0);
	}
	else // if(_price_mode == 2)
	{
		targetPx = isBuy ? curTick->askprice(0) : curTick->bidprice(0);
	}

	targetPx += _comm_info->getPriceTick() * _cancel_times* (isBuy ? 1 : -1);
	//检查涨跌停价
	isCanCancel = true;
	if (isBuy && !decimal::eq(_last_tick->upperlimit(), 0) && decimal::gt(targetPx, _last_tick->upperlimit()))
	{
		_ctx->writeLog(fmt::format("Buy price {} of {} modified to upper limit price", targetPx, _code.c_str(), _last_tick->upperlimit()).c_str());
		targetPx = _last_tick->upperlimit();
		isCanCancel = false;//如果价格被修正为涨跌停价，订单不可撤销
	}
	if (isBuy != 1 && !decimal::eq(_last_tick->lowerlimit(), 0) && decimal::lt(targetPx, _last_tick->lowerlimit()))
	{
		_ctx->writeLog(fmt::format("Sell price {} of {} modified to lower limit price", targetPx, _code.c_str(), _last_tick->lowerlimit()).c_str());
		targetPx = _last_tick->lowerlimit();
		isCanCancel = false;	//如果价格被修正为涨跌停价，订单不可撤销
	}

	OrderIDs ids;
	if (qty > 0)
		ids = _ctx->buy(code, targetPx, abs(qty));
	else
		ids = _ctx->sell(code, targetPx, abs(qty));

	_orders_mon.push_order(ids.data(), ids.size(), now, isCanCancel);

	curTick->release();
}

void WtSpreadTradingExeUnit::do_calc()
{
	CalcFlag flag(&_in_calc);
	if (flag)
		return;

	if (!_channel_ready)
		return;
	//这里加一个锁，主要原因是实盘过程中发现
	//在修改目标仓位的时候，会触发一次do_calc
	//而ontick也会触发一次do_calc，两次调用是从两个线程分别触发的，所以会出现同时触发的情况
	//如果不加锁，就会引起问题
	//这种情况在原来的SimpleExecUnit没有出现，因为SimpleExecUnit只在set_position的时候触发
	StdUniqueLock lock(_mtx_calc);
	const char* code;
	/*修改
	*/
	if (on_A_order)
	{
		code = A_stdCode.c_str();
	}
	else if(on_B_order)
	{
		code = B_stdCode.c_str();
	}

	double undone = _ctx->getUndoneQty(code);
	double newVol = get_real_target(_active_target_pos);
	double realPos = _ctx->getPosition(code);
	double diffQty = newVol - realPos;

	//有正在撤销的订单,则不能进行下一轮计算
	if (_cancel_cnt != 0)
	{
		_ctx->writeLog(fmt::format("{}尚有未完成撤单指令,暂时退出本轮执行", code).c_str());
		return;
	}

	if (decimal::eq(diffQty, 0))
		return;
	//每一次发单要保障成交,所以如果有未完成单,说明上一轮没完成
	//有未完成订单，与实际仓位变动方向相反
	//则需要撤销现有订单
	if (decimal::lt(diffQty * undone, 0))
	{
		bool isBuy = decimal::gt(undone, 0);
		OrderIDs ids = _ctx->cancel(code, isBuy);
		if (!ids.empty())
		{
			_orders_mon.push_order(ids.data(), ids.size(), _ctx->getCurTime());
			_cancel_cnt += ids.size();
			_ctx->writeLog(fmtutil::format("[{}@{}] live opposite order of {} canceled, cancelcnt -> {}", __FILE__, __LINE__, code, _cancel_cnt));//相反的订单已取消
		}
		return;
	}
	//因为是逐笔发单，所以如果有不需要撤销的未完成单，则暂不发单
	if (!decimal::eq(undone, 0))
	{
		_ctx->writeLog(fmt::format("{}上一轮有挂单未完成,暂时退出本轮执行", code).c_str());
		return;
	}
	double curPos = realPos;
	if (_last_tick == NULL)
	{
		_ctx->writeLog(fmt::format("{}没有最新tick数据,退出执行逻辑", code).c_str());
		return;
	}
	if (decimal::eq(curPos, newVol))
	{
		//当前仓位和最新仓位匹配时，如果不是全部清仓的需求，则直接退出计算了
		if (!is_clear(_active_final_target_pos))
			return;
		
		/*
		!!!!!!
		此处空头逻辑处理需要补充!
		*/

		//如果是清仓的需求，还要再进行对比
		//如果多头为0，说明已经全部清理掉了，则直接退出
		double lPos = _ctx->getPosition(code, true, 1);
		if (decimal::eq(lPos, 0))
			return;

		//如果还有多头仓位，则将目标仓位设置为非0，强制触发                      
		newVol = -min(lPos, LegInfoMap[code]->_order_lots); 
		_ctx->writeLog(fmtutil::format("Clearing process triggered, target position of {} has been set to {}", code, newVol));
	}

	//如果相比上次没有更新的tick进来，则先不下单，防止开盘前集中下单导致通道被封
	uint64_t curTickTime = (uint64_t)_last_tick->actiondate() * 1000000000 + _last_tick->actiontime();
	if (curTickTime <= _last_tick_time)
	{
		_ctx->writeLog(fmtutil::format("No tick of {} updated, {} <= {}, execute later", code, curTickTime, _last_tick_time));
		return;
	}
	_last_tick_time = curTickTime;
	double this_qty = LegInfoMap[code]->_order_lots; 	////单次发单手数
	
	/*
	考虑处理不需要 _total_times _fired_times
	*/
	uint32_t leftTimes = _total_times - _fired_times;
	_ctx->writeLog(fmt::format("第 {} 次发单", _fired_times + 1).c_str());

	bool bNeedShowHand = false;
	//剩余次数为0,剩余数量不为0,说明要全部发出去了
	//剩余次数为0,说明已经到了兜底时间了,如果这个时候还有未完成数量,则需要发单
	//如果剩余此处为0 ,则需要全部下单
	//否则,取整(剩余数量/剩余次数)与1的最大值,即最小为_min_open_lots,但是要注意符号处理
	double curQty = 0;
	if (leftTimes == 0 && !decimal::eq(diffQty, 0))
	{
		bNeedShowHand = true;
		curQty = max(_min_open_lots, diffQty);
	}
	else {
		curQty = std::max(_min_open_lots, round(abs(diffQty) / leftTimes)) * abs(diffQty) / diffQty;
	}

	//设定本轮目标仓位
	_this_target = realPos + curQty;	//现仓位量+本轮下单量

	WTSTickData* curTick = _last_tick;
	uint64_t now = TimeUtils::getLocalTimeNow();
	bool isBuy = decimal::gt(diffQty, 0);
	double targetPx = 0;
	//根据价格模式设置,确定委托基准价格: 0-最新价,1-最优价,2-对手价
	if (_price_mode == 0)
	{
		targetPx = curTick->price();
	}
	else if (_price_mode == 1)
	{
		targetPx = isBuy ? curTick->bidprice(0) : curTick->askprice(0);
	}
	else // if(_price_mode == 2)
	{
		targetPx = isBuy ? curTick->askprice(0) : curTick->bidprice(0);
	}

	//如果需要全部发单,则价格偏移5跳,以保障执行
	if (bNeedShowHand) //  last showhand time
	{
		targetPx += _comm_info->getPriceTick() * 5 * (isBuy ? 1 : -1);
	}
	else if (_price_offset != 0)	//如果设置了价格偏移,也要处理一下
	{
		targetPx += _comm_info->getPriceTick() * _price_offset * (isBuy ? 1 : -1);
	}
	// 如果最后价格为0，再做一个修正
	if (decimal::eq(targetPx, 0.0))
		targetPx = decimal::eq(_last_tick->price(), 0.0) ? _last_tick->preclose() : _last_tick->price();

	//检查涨跌停价
	isCanCancel = true;
	if (isBuy && !decimal::eq(_last_tick->upperlimit(), 0) && decimal::gt(targetPx, _last_tick->upperlimit()))
	{
		_ctx->writeLog(fmt::format("Buy price {} of {} modified to upper limit price", targetPx, code, _last_tick->upperlimit()).c_str());
		targetPx = _last_tick->upperlimit();
		isCanCancel = false;//如果价格被修正为涨跌停价，订单不可撤销
	}
	if (isBuy != 1 && !decimal::eq(_last_tick->lowerlimit(), 0) && decimal::lt(targetPx, _last_tick->lowerlimit()))
	{
		_ctx->writeLog(fmt::format("Sell price {} of {} modified to lower limit price", targetPx, code, _last_tick->lowerlimit()).c_str());
		targetPx = _last_tick->lowerlimit();
		isCanCancel = false;	//如果价格被修正为涨跌停价，订单不可撤销
	}
	//最后发出指令
	OrderIDs ids;
	if (curQty > 0)
		ids = _ctx->buy(code, targetPx, abs(curQty));
	else
		ids = _ctx->sell(code, targetPx, abs(curQty));

	_orders_mon.push_order(ids.data(), ids.size(), now, isCanCancel);
	_last_fire_time = now;
	_fired_times += 1;

	curTick->release();
}
/*2023/6/15 SpreadTrading0.1.1
*在套利执行期间传入一共需要执行的主动腿A的执行手数_final_target_pos，设置需要执行的份数_target_portion。
*每份需要执行的主动腿_active_target_pos==_active_final_target_pos/_target_portion
*每轮执行一份进行。  后续再加入规定每轮执行份数。每轮执行份数按策略中盘口交易量决定
*若传入份数大于规定执行份数，按规定执行份数执行，如：规定执行份数为2份。若策略传入3份，按每次两份执行。
*若策略传入<2份，按最小执行份数 1份执行。除非传入DBL_MAX清仓指令
*/
void WtSpreadTradingExeUnit::set_position(const char* stdCode, double newVol)
{
	if (_code.compare(stdCode) != 0)
		return;
	
	if (decimal::eq(newVol, _active_final_target_pos))
		return;                          
	_active_final_target_pos = newVol;
	_active_target_pos = _active_final_target_pos / _target_portion; //每份主动腿执行仓位= 总目标主动腿执行量 / 需要执行次数

	if (is_clear(_active_final_target_pos))
		_ctx->writeLog(fmtutil::format("{} is set to be in clearing processing", stdCode));
	else
		_ctx->writeLog(fmtutil::format("Target position of {} is set tb be {}", stdCode, _active_target_pos));
	//每份被动腿执行仓位  == 主动腿执行量*(B价格乘数/B交易乘数)
	B_target_pos = _active_target_pos * (LegInfoMap[B_stdCode]->_price_multiplier / LegInfoMap[B_stdCode]->_trade_multiplier);//被动腿每份需要执行的目标 仓位 

	_fired_times = 0;

	do_calc();
}


void WtSpreadTradingExeUnit::clear_all_position(const char* stdCode)
{
	if (_code.compare(stdCode) != 0)
		return;
	//总主动腿设置为DBL_MAX   清仓
	_active_final_target_pos = DBL_MAX;

	do_calc();
}
//检查委托是否结束
bool WtSpreadTradingExeUnit::is_order_finish()
{
	double active_traded = _ctx->getPosition(A_stdCode); ;//主动腿已交易

	double spread_volume = calculate_spread_volume(A_stdCode, active_traded);


}
double WtSpreadTradingExeUnit::calculate_spread_volume(const char* stdcode,double leg_volume)
{
	double spread_volume = leg_volume / LegInfoMap[stdcode]->_trade_multiplier;
		
}
//检查是否对冲完成
bool WtSpreadTradingExeUnit::is_hedge_finsh()
{

}
//对冲被动腿
void WtSpreadTradingExeUnit::hedge_passive_legs()
{

}
//处理主动腿
void WtSpreadTradingExeUnit::deal_active_leg(const char* stdCode)
{

}

void WtSpreadTradingExeUnit::send_leg_order(const char* stdCode, double vol)
{

}

/*ontick  1.检查是否还有存在订单
		if_order_finish()
		2.检查对冲主动腿是否充分
		if not    is_hedge_finished()
			hedge_passive_legs
		3.检查tick有无初始化
		4.检查主动腿是否需要处理
			if self.direction == Direction.LONG:
				if self.spread.ask_price <= self.price: #spread 的买价< price
					self.take_active_leg()
			else:
				if self.spread.bid_price >= self.price: # sprad 的卖价< price
					self.take_active_leg()

	# 封装撤单函数  cancel_order(leg_localid)
*/