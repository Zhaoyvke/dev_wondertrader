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
	,passtive_localid(0) //��ʼ��Ϊ0��Ӧ���ڽ���ͨ������ʱ��У���ǵ�һ�ν�������ͨ�����ǶϿ�������
	,active_localid(0)

	/*
		// ���Ǹ������۸����&& ���׳����� �� trading_multiplier ����   ����ǰ����
	*/


	,_price_multiplier(1)//�۸����
	,_trading_multiplier(1)//���׳���
	,_Spread_Price(-1)//�۲��ʼ��Ϊ-1����Ϊ���ܼ����Ϊ0
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
inline uint32_t calTmSecs(uint32_t begintime, uint32_t endtime) //����ִ��ʱ�䣺s
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

	_comm_info = ctx->getCommodityInfo(stdCode);//��ȡƷ�ֲ���
	if (_comm_info)
		_comm_info->retain();
	_sess_info = ctx->getSessionInfo(stdCode);//��ȡ����ʱ��ģ����Ϣ
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
	//_order_lots = cfg->getDouble("lots");									//���η�������
	_active_final_target_pos = cfg->getUInt32("_active_final_target_pos");	//��������ִ��Ŀ���λ
	_target_portion = cfg->getUInt32("target_portion");						//�۲��Լ��ִ�з���

	//��ͨ���б����passiveLegs����Pushback Legs
	A_stdCode = stdCode;
	B_stdCode = BstdCode;
	vector<Leg_Infoion>passivelegs;
	
	//SpreadDataStruct* m_SpreadDataStruct = new SpreadDataStruct;
	//Leg_Session*	v_pLeg_Session = new Leg_Session;

	v_pLeg_Infoion->_order_lots = cfg->getDouble("Alots");
	v_pLeg_Infoion->_trade_multiplier = cfg->getDouble("A_trade_multiplier");
	if (cfg->has("Aminopenlots"))			//���ļ�������������Ҫ������Ϊ1
		v_pLeg_Infoion->_min_open_lots = cfg->getDouble("Aminopenlots");//��С��������
	LegInfoMap[A_stdCode] = v_pLeg_Infoion;
	v_pLeg_Infoion->_order_lots = cfg->getDouble("Blots");
	v_pLeg_Infoion->_trade_multiplier = cfg->getDouble("B_trade_multiplier");
	if (cfg->has("Bminopenlots"))
		v_pLeg_Infoion->_min_open_lots = cfg->getDouble("Bminopenlots");//��С��������
	LegInfoMap[B_stdCode] = v_pLeg_Infoion;
	/*
	����Ҫ���	B_leg  C_leg.....*/

	//m_SpreadDataStruct->LegInfoMap[A_stdCode] = v_pLeg_Session;
	
	/*
	���Դ˴�����Ľ��׳����ͼ۸����ͳһ��Ϊ��������ϵ�� �� A-r*mB,������ϵ������֮ǰ��Ϊ1����
	
	�����ڻ������ף���۸�����ͽ��׳�����Ϊ1��1�����۲� = y2205 - y2209������1�ּ۲��������1��y2205��ͬʱ����1��y2209��ɶԳ塣

��ע�⣬�ڶ����Ȳ����ڻ���Լ��ģ����ʱ�������۲��Լ���������һЩ���繹������ֳ��������õ��ļ۲��Լʱ�����㹫ʽ���£�

���Ƹ�����������16������ʯ����5�ֽ�̿����10�����Ƹ֡�

���ڼ۸�����ļ۲�spread = 1* RB - 1.6*I - 0.5*O��

��������������10��/�֣�����ʯ�ͽ�̿����100��/�֣��������ǽ��׳�����1��10��10��

�ʻ������Լ��������ʵ�ʽ���������ϵ��ÿ����100�����Ƹ֣�1000�֣�����Ҫ����16������ʯ��1600�֣���5�ֽ�̿��500�֣���ɶԳ塣

*/

	_total_secs = calTmSecs(_begin_time, _end_time);							//ִ����ʱ�䣺��
	_fire_span = (_total_secs - _tail_secs) / _total_times;						//���η���ʱ����,Ҫȥ��β��ʱ�����,�����Ļ�,���ʣ�����������һ�����׷����Ļ�����
	ctx->writeLog(fmt::format("ִ�е�ԪWtSpreadTradingExeUnit[{}] ��ʼ�����,������ʱ {} ��,ִ��ʱ�� {} ��,��βʱ�� {} ��,���ʱ�� {} ��", stdCode, _ord_sticky, _total_secs, _tail_secs, _fire_span).c_str());
}
/*
	�ڸ�Leg���µ������У����Գ�ʱ���������ǳ�ʱ���м���
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
	//�ݴ������������ȶ�����
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
		ֻ�������������ȳ�ʱ�����ط�����
		������Ҫ�Ķ�
		fire at once �������Ҫ��init�д�������ã�֮���޸�
	*/	
	//���ȫ�������ѳ���,���ʱ��һ��������Ҫ��ʱ�������ҵ���ʱ�� 
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
	double A_undone = _ctx->getUndoneQty(A_stdCode.c_str());//������δ�����
	double B_undone = _ctx->getUndoneQty(B_stdCode.c_str());//������δ�����

	/*
	��������
	������ready,�Ƿ���Ҫ����undoneУ�飿
	����δ��undoneУ��
	*/
	/*23/6/26
		����	����ģ�壩
		ֻ�������������Ƚ���ͨ�����ô���
		������Ҫ�Ķ�
	*/
	if (active_localid!=0||passtive_localid!=0) 
	{
		if (!decimal::eq(A_undone, 0)&&!_orders_mon.has_order(active_localid))
		{
			//��˵����δ���A�����ڼ��֮��,�ȳ���
			_ctx->writeLog(fmt::format("{} unmanaged orders of {}, cancel all", A_undone, A_stdCode).c_str());
			bool isBuy = (A_undone > 0);
			OrderIDs ids = _ctx->cancel(A_stdCode.c_str(), isBuy);
			_orders_mon.push_order(ids.data(), ids.size(), _ctx->getCurTime());
			_cancel_cnt += ids.size();
			_ctx->writeLog(fmt::format("Unmanaged order updated cancelcnt to {}", _cancel_cnt).c_str());
		}
		else if (!decimal::eq(B_undone, 0) && !_orders_mon.has_order(passtive_localid))
		{
			//��˵����Bδ��ɵ����ڼ��֮��,�ȳ���
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
		 *	���δ��ɵ�Ϊ0������OMS�����ж�����
		 *	˵��OMS���Ǵ�����Ҫ���������Ȼ��ʱ�����ͻ����
		 *	���������һ���Ƕ��������Ժ�֮ǰ�³�ȥ�Ķ�������û���������͵���̨
		 *	����������Ҫ��������ض���
		 */
		_ctx->writeLog(fmtutil::format("Local orders of {} not confirmed in trading channel, clear all", A_stdCode.c_str()));
		_orders_mon.clear_orders();
		}
		else if (decimal::eq(B_undone, 0) && _orders_mon.has_order(passtive_localid))
		{
			_ctx->writeLog(fmtutil::format("Local orders of {} not confirmed in trading channel, clear all", B_stdCode.c_str()));
			_orders_mon.clear_orders();
		}
		/*�����ڽ���ͨ���ָ�����һ��A�ɽ���Bû�ɽ���
		�������ƽ��A��֮�������¼���
		*/
		else if (A_undone == 0 &&B_undone == B_target_pos)
		{
			//ƽ��һ��A
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
		//�������ʱ�䲻�ڽ���ʱ��,�������һ���Ǽ��Ͼ��۵��������,�µ���ʧ��,����ֱ�ӹ��˵��������
		if (_sess_info != NULL && !_sess_info->isInTradingTime(newTick->actiontime() / 100000))
			return;
		//������lasttick��ΪNULL����������Legstick���õ�ʱ��isFirstTick��Ϊtrue����firsttick����spreadprice����
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
	//�µ�tick����,Ҫ����
	_last_tick = newTick;
	_last_tick->retain();

	if (isFirstTick)	//����ǵ�һ��tick,����Ŀ���λ,���������µ�
	{
		/* FirstTick 
		��Ҫ����Legs'firsttick ������,ȷ�������ȵ�tick�������ͺ���Ϊfirsttick
		��һ��tick����Ϊ�۲�Ļ���ֵ
		spread_price =A-B
		Price_mode:���ż�1		���̼�2		�ŶӼ�0
		���żۣ����ۣ�Ask1a-Bid1b��-��ۣ�Bid1a-Ask1b��
		���̼ۣ����ۣ�Bid1a-Ask1b��-��ۣ�Ask1a-Bid1b��
		�ŶӼۣ���
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
		{		//δ�� �۸��׳���
			_Spread_Price = (active_last_tick->askprice(1) - B_last_tick->bidprice(1)) - (B_last_tick->bidprice(1) - active_last_tick->askprice(1));
		
		}
		else if (_price_mode == 2)	
		{
			_Spread_Price = (B_last_tick->bidprice(1) - active_last_tick->askprice(1)) - (active_last_tick->askprice(1) - B_last_tick->bidprice(1));
		}
		else if (_price_mode == 0) 
		{//�ŶӼۺ�������
		}
		if ((!decimal::eq(A_newVol, A_undone + A_realPos))|| (!decimal::eq(B_newVol, B_undone + B_realPos))) //��λ�仯Ҫ���� 
		{
			do_calc();
		}
	}
	/*VNPY 1.����Ƿ��д��ڶ���
			if_order_finish()
			2.���Գ��������Ƿ���
			if not    is_hedge_finished()
				hedge_passive_legs
			3.���tick���޳�ʼ��
			4.����������Ƿ���Ҫ����
				if self.direction == Direction.LONG:
					if self.spread.ask_price <= self.price: #spread �����< price
						self.take_active_leg()
				else:
					if self.spread.bid_price >= self.price: # sprad ������< price
						self.take_active_leg()

		# ��װ��������  cancel_order(leg_localid)
	*/
	else  // ������FirstTick����Բ�ͬ�µ�����Ĵ���
	{
		if (on_A_order)					//�������ȵ��µ�������
		{
			check_sticky(active_localid);
		}
		else if (on_B_order)			//�ڱ����ȵ��µ�������	
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
	if (stdCode == A_stdCode && vol == _active_target_pos) //�����ȳɽ��ɹ� && �ɽ��� == ÿ��������Ŀ��ɽ��� 
	{	
		/*��ǿ��Կ�ʼ���ױ�����
			��¼���׷���
		*/
		begin_B_passtive = true;
		Sum_Act_vol += vol;

	}
	else if(stdCode == A_stdCode && vol < _active_target_pos)//�����Ƚ�����δ�ﵽĿ�꣬��������������
	{
		/*1.��Ǽ�������������
		2.�ڹҵ�����ڣ��ȴ��ɽ�������
		3.�ж��ҵ���ʱ����������������׷��
		*/
		begin_B_passtive = false;
		Sum_Act_vol += vol;
	}
	else if (stdCode == B_stdCode && vol == B_target_pos)//�����Ƚ������ﵽĿ�꣬����������
	{
		 begin_active = true;
		/*1.��ǿ��Կ�ʼ����������
		  2.��¼���׷���
		*/
		 Sum_B_vol += vol;
	}
	else if (stdCode == B_stdCode && vol < B_target_pos)//�����Ƚ�����δ�ﵽĿ�꣬�������ױ�����
	{
		/*1.��Ǽ������ױ�����
		2.�ڹҵ�����ڣ��ȴ��ɽ�������
		3.�ж��ҵ���ʱ����������������׷��
		*/
		begin_active = false;
		Sum_B_vol += vol;
	}
}

void WtSpreadTradingExeUnit::on_entrust(uint32_t localid, const char* stdCode, bool bSuccess, const char* message)
{
	if (!bSuccess)
	{
		//��������ҷ���ȥ�Ķ���,�ҾͲ�����
		if (!_orders_mon.has_order(localid))
			return;

		_orders_mon.erase_order(localid);
		_error_times++;
		//�µ�����>= 3�Σ�ֹͣ�µ���ֱ���ɹ���ָ�
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
	//���ݼ۸�ģʽ����,ȷ��ί�л�׼�۸�: 0-���¼�,1-���ż�,2-���ּ�
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
	//����ǵ�ͣ��
	isCanCancel = true;
	if (isBuy && !decimal::eq(_last_tick->upperlimit(), 0) && decimal::gt(targetPx, _last_tick->upperlimit()))
	{
		_ctx->writeLog(fmt::format("Buy price {} of {} modified to upper limit price", targetPx, _code.c_str(), _last_tick->upperlimit()).c_str());
		targetPx = _last_tick->upperlimit();
		isCanCancel = false;//����۸�����Ϊ�ǵ�ͣ�ۣ��������ɳ���
	}
	if (isBuy != 1 && !decimal::eq(_last_tick->lowerlimit(), 0) && decimal::lt(targetPx, _last_tick->lowerlimit()))
	{
		_ctx->writeLog(fmt::format("Sell price {} of {} modified to lower limit price", targetPx, _code.c_str(), _last_tick->lowerlimit()).c_str());
		targetPx = _last_tick->lowerlimit();
		isCanCancel = false;	//����۸�����Ϊ�ǵ�ͣ�ۣ��������ɳ���
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
	//�����һ��������Ҫԭ����ʵ�̹����з���
	//���޸�Ŀ���λ��ʱ�򣬻ᴥ��һ��do_calc
	//��ontickҲ�ᴥ��һ��do_calc�����ε����Ǵ������̷ֱ߳𴥷��ģ����Ի����ͬʱ���������
	//������������ͻ���������
	//���������ԭ����SimpleExecUnitû�г��֣���ΪSimpleExecUnitֻ��set_position��ʱ�򴥷�
	StdUniqueLock lock(_mtx_calc);
	const char* code;
	/*�޸�
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

	//�����ڳ����Ķ���,���ܽ�����һ�ּ���
	if (_cancel_cnt != 0)
	{
		_ctx->writeLog(fmt::format("{}����δ��ɳ���ָ��,��ʱ�˳�����ִ��", code).c_str());
		return;
	}

	if (decimal::eq(diffQty, 0))
		return;
	//ÿһ�η���Ҫ���ϳɽ�,���������δ��ɵ�,˵����һ��û���
	//��δ��ɶ�������ʵ�ʲ�λ�䶯�����෴
	//����Ҫ�������ж���
	if (decimal::lt(diffQty * undone, 0))
	{
		bool isBuy = decimal::gt(undone, 0);
		OrderIDs ids = _ctx->cancel(code, isBuy);
		if (!ids.empty())
		{
			_orders_mon.push_order(ids.data(), ids.size(), _ctx->getCurTime());
			_cancel_cnt += ids.size();
			_ctx->writeLog(fmtutil::format("[{}@{}] live opposite order of {} canceled, cancelcnt -> {}", __FILE__, __LINE__, code, _cancel_cnt));//�෴�Ķ�����ȡ��
		}
		return;
	}
	//��Ϊ����ʷ�������������в���Ҫ������δ��ɵ������ݲ�����
	if (!decimal::eq(undone, 0))
	{
		_ctx->writeLog(fmt::format("{}��һ���йҵ�δ���,��ʱ�˳�����ִ��", code).c_str());
		return;
	}
	double curPos = realPos;
	if (_last_tick == NULL)
	{
		_ctx->writeLog(fmt::format("{}û������tick����,�˳�ִ���߼�", code).c_str());
		return;
	}
	if (decimal::eq(curPos, newVol))
	{
		//��ǰ��λ�����²�λƥ��ʱ���������ȫ����ֵ�������ֱ���˳�������
		if (!is_clear(_active_final_target_pos))
			return;
		
		/*
		!!!!!!
		�˴���ͷ�߼�������Ҫ����!
		*/

		//�������ֵ����󣬻�Ҫ�ٽ��жԱ�
		//�����ͷΪ0��˵���Ѿ�ȫ��������ˣ���ֱ���˳�
		double lPos = _ctx->getPosition(code, true, 1);
		if (decimal::eq(lPos, 0))
			return;

		//������ж�ͷ��λ����Ŀ���λ����Ϊ��0��ǿ�ƴ���                      
		newVol = -min(lPos, LegInfoMap[code]->_order_lots); 
		_ctx->writeLog(fmtutil::format("Clearing process triggered, target position of {} has been set to {}", code, newVol));
	}

	//�������ϴ�û�и��µ�tick���������Ȳ��µ�����ֹ����ǰ�����µ�����ͨ������
	uint64_t curTickTime = (uint64_t)_last_tick->actiondate() * 1000000000 + _last_tick->actiontime();
	if (curTickTime <= _last_tick_time)
	{
		_ctx->writeLog(fmtutil::format("No tick of {} updated, {} <= {}, execute later", code, curTickTime, _last_tick_time));
		return;
	}
	_last_tick_time = curTickTime;
	double this_qty = LegInfoMap[code]->_order_lots; 	////���η�������
	
	/*
	���Ǵ�����Ҫ _total_times _fired_times
	*/
	uint32_t leftTimes = _total_times - _fired_times;
	_ctx->writeLog(fmt::format("�� {} �η���", _fired_times + 1).c_str());

	bool bNeedShowHand = false;
	//ʣ�����Ϊ0,ʣ��������Ϊ0,˵��Ҫȫ������ȥ��
	//ʣ�����Ϊ0,˵���Ѿ����˶���ʱ����,������ʱ����δ�������,����Ҫ����
	//���ʣ��˴�Ϊ0 ,����Ҫȫ���µ�
	//����,ȡ��(ʣ������/ʣ�����)��1�����ֵ,����СΪ_min_open_lots,����Ҫע����Ŵ���
	double curQty = 0;
	if (leftTimes == 0 && !decimal::eq(diffQty, 0))
	{
		bNeedShowHand = true;
		curQty = max(_min_open_lots, diffQty);
	}
	else {
		curQty = std::max(_min_open_lots, round(abs(diffQty) / leftTimes)) * abs(diffQty) / diffQty;
	}

	//�趨����Ŀ���λ
	_this_target = realPos + curQty;	//�ֲ�λ��+�����µ���

	WTSTickData* curTick = _last_tick;
	uint64_t now = TimeUtils::getLocalTimeNow();
	bool isBuy = decimal::gt(diffQty, 0);
	double targetPx = 0;
	//���ݼ۸�ģʽ����,ȷ��ί�л�׼�۸�: 0-���¼�,1-���ż�,2-���ּ�
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

	//�����Ҫȫ������,��۸�ƫ��5��,�Ա���ִ��
	if (bNeedShowHand) //  last showhand time
	{
		targetPx += _comm_info->getPriceTick() * 5 * (isBuy ? 1 : -1);
	}
	else if (_price_offset != 0)	//��������˼۸�ƫ��,ҲҪ����һ��
	{
		targetPx += _comm_info->getPriceTick() * _price_offset * (isBuy ? 1 : -1);
	}
	// ������۸�Ϊ0������һ������
	if (decimal::eq(targetPx, 0.0))
		targetPx = decimal::eq(_last_tick->price(), 0.0) ? _last_tick->preclose() : _last_tick->price();

	//����ǵ�ͣ��
	isCanCancel = true;
	if (isBuy && !decimal::eq(_last_tick->upperlimit(), 0) && decimal::gt(targetPx, _last_tick->upperlimit()))
	{
		_ctx->writeLog(fmt::format("Buy price {} of {} modified to upper limit price", targetPx, code, _last_tick->upperlimit()).c_str());
		targetPx = _last_tick->upperlimit();
		isCanCancel = false;//����۸�����Ϊ�ǵ�ͣ�ۣ��������ɳ���
	}
	if (isBuy != 1 && !decimal::eq(_last_tick->lowerlimit(), 0) && decimal::lt(targetPx, _last_tick->lowerlimit()))
	{
		_ctx->writeLog(fmt::format("Sell price {} of {} modified to lower limit price", targetPx, code, _last_tick->lowerlimit()).c_str());
		targetPx = _last_tick->lowerlimit();
		isCanCancel = false;	//����۸�����Ϊ�ǵ�ͣ�ۣ��������ɳ���
	}
	//��󷢳�ָ��
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
*������ִ���ڼ䴫��һ����Ҫִ�е�������A��ִ������_final_target_pos��������Ҫִ�еķ���_target_portion��
*ÿ����Ҫִ�е�������_active_target_pos==_active_final_target_pos/_target_portion
*ÿ��ִ��һ�ݽ��С�  �����ټ���涨ÿ��ִ�з�����ÿ��ִ�з������������̿ڽ���������
*������������ڹ涨ִ�з��������涨ִ�з���ִ�У��磺�涨ִ�з���Ϊ2�ݡ������Դ���3�ݣ���ÿ������ִ�С�
*�����Դ���<2�ݣ�����Сִ�з��� 1��ִ�С����Ǵ���DBL_MAX���ָ��
*/
void WtSpreadTradingExeUnit::set_position(const char* stdCode, double newVol)
{
	if (_code.compare(stdCode) != 0)
		return;
	
	if (decimal::eq(newVol, _active_final_target_pos))
		return;                          
	_active_final_target_pos = newVol;
	_active_target_pos = _active_final_target_pos / _target_portion; //ÿ��������ִ�в�λ= ��Ŀ��������ִ���� / ��Ҫִ�д���

	if (is_clear(_active_final_target_pos))
		_ctx->writeLog(fmtutil::format("{} is set to be in clearing processing", stdCode));
	else
		_ctx->writeLog(fmtutil::format("Target position of {} is set tb be {}", stdCode, _active_target_pos));
	//ÿ�ݱ�����ִ�в�λ  == ������ִ����*(B�۸����/B���׳���)
	B_target_pos = _active_target_pos * (LegInfoMap[B_stdCode]->_price_multiplier / LegInfoMap[B_stdCode]->_trade_multiplier);//������ÿ����Ҫִ�е�Ŀ�� ��λ 

	_fired_times = 0;

	do_calc();
}


void WtSpreadTradingExeUnit::clear_all_position(const char* stdCode)
{
	if (_code.compare(stdCode) != 0)
		return;
	//������������ΪDBL_MAX   ���
	_active_final_target_pos = DBL_MAX;

	do_calc();
}
//���ί���Ƿ����
bool WtSpreadTradingExeUnit::is_order_finish()
{
	double active_traded = _ctx->getPosition(A_stdCode); ;//�������ѽ���

	double spread_volume = calculate_spread_volume(A_stdCode, active_traded);


}
double WtSpreadTradingExeUnit::calculate_spread_volume(const char* stdcode,double leg_volume)
{
	double spread_volume = leg_volume / LegInfoMap[stdcode]->_trade_multiplier;
		
}
//����Ƿ�Գ����
bool WtSpreadTradingExeUnit::is_hedge_finsh()
{

}
//�Գ屻����
void WtSpreadTradingExeUnit::hedge_passive_legs()
{

}
//����������
void WtSpreadTradingExeUnit::deal_active_leg(const char* stdCode)
{

}

void WtSpreadTradingExeUnit::send_leg_order(const char* stdCode, double vol)
{

}

/*ontick  1.����Ƿ��д��ڶ���
		if_order_finish()
		2.���Գ��������Ƿ���
		if not    is_hedge_finished()
			hedge_passive_legs
		3.���tick���޳�ʼ��
		4.����������Ƿ���Ҫ����
			if self.direction == Direction.LONG:
				if self.spread.ask_price <= self.price: #spread �����< price
					self.take_active_leg()
			else:
				if self.spread.bid_price >= self.price: # sprad ������< price
					self.take_active_leg()

	# ��װ��������  cancel_order(leg_localid)
*/