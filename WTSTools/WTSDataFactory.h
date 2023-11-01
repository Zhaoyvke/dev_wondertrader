/*!
 * \file WTSDataFactory.h
 * \project	WonderTrader
 *
 * \author Wesley
 * \date 2020/03/30
 * 
 * \brief ����ƴ�ӹ����ඨ��
 */
#pragma once
#include "../Includes/IDataFactory.h"

USING_NS_WTP;

class WTSDataFactory : public IDataFactory
{
public:
	/*
	 *	����tick���ݸ���K��
	 *	@klineData	K������
	 *	@tick		tick����
	 *	@sInfo		����ʱ��ģ��
	 */
	virtual WTSBarStruct*	updateKlineData(WTSKlineData* klineData, WTSTickData* tick, WTSSessionInfo* sInfo, bool bAlignSec = false);

	/*
	 *	���û�������K�����ݸ���K��
	 *	@klineData		K������
	 *	@newBasicBar	��������K������
	 *	@sInfo			����ʱ��ģ��
	 */
	virtual WTSBarStruct*	updateKlineData(WTSKlineData* klineData, WTSBarStruct* newBasicBar, WTSSessionInfo* sInfo, bool bAlignSec = false);

	/*
	 *	�ӻ�������K��������ȡ�ǻ������ڵ�K������
	 *	@baseKline	��������K��
	 *	@period		�������ڣ�m1/m5/day
	 *	@times		���ڱ���
	 *	@sInfo		����ʱ��ģ��
	 *	@bIncludeOpen	�Ƿ����δ�պϵ�K��
	 *	@bAlignSec	�Ƿ�С�ڶ���
	 */
	virtual WTSKlineData*	extractKlineData(WTSKlineSlice* baseKline, WTSKlinePeriod period, uint32_t times, WTSSessionInfo* sInfo, bool bIncludeOpen = true, bool bAlignSec = false);

	/*
	 *	��tick������ȡ�����ڵ�K������
	 *	@ayTicks	tick����
	 *	@seconds	Ŀ������
	 *	@sInfo		����ʱ��ģ��
	 *	@bUnixTime	tickʱ����Ƿ���unixtime
	 *	@bAlignSec	�Ƿ�С�ڶ���
	 */
	virtual WTSKlineData*	extractKlineData(WTSTickSlice* ayTicks, uint32_t seconds, WTSSessionInfo* sInfo, bool bUnixTime = false, bool bAlignSec = false);

	/*
	 *	�ϲ�K��
	 *	@klineData	Ŀ��K��
	 *	@newKline	���ϲ���K��
	 */
	virtual bool			mergeKlineData(WTSKlineData* klineData, WTSKlineData* newKline);

protected:
	WTSBarStruct* updateMin1Data(WTSSessionInfo* sInfo, WTSKlineData* klineData, WTSTickData* tick, bool bAlignSec = false);
	WTSBarStruct* updateMin5Data(WTSSessionInfo* sInfo, WTSKlineData* klineData, WTSTickData* tick, bool bAlignSec = false);
	WTSBarStruct* updateDayData(WTSSessionInfo* sInfo, WTSKlineData* klineData, WTSTickData* tick);
	WTSBarStruct* updateSecData(WTSSessionInfo* sInfo, WTSKlineData* klineData, WTSTickData* tick);

	WTSBarStruct* updateMin1Data(WTSSessionInfo* sInfo, WTSKlineData* klineData, WTSBarStruct* newBasicBar, bool bAlignSec = false);
	WTSBarStruct* updateMin5Data(WTSSessionInfo* sInfo, WTSKlineData* klineData, WTSBarStruct* newBasicBar, bool bAlignSec = false);

	WTSKlineData* extractMin1Data(WTSKlineSlice* baseKline, uint32_t times, WTSSessionInfo* sInfo, bool bIncludeOpen = true, bool bAlignSec = false);
	WTSKlineData* extractMin5Data(WTSKlineSlice* baseKline, uint32_t times, WTSSessionInfo* sInfo, bool bIncludeOpen = true, bool bAlignSec = false);
	WTSKlineData* extractDayData(WTSKlineSlice* baseKline, uint32_t times, bool bIncludeOpen = true);

protected:
	static uint32_t getPrevMinute(uint32_t curMinute, int period = 1);
};

