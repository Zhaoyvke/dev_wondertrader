//////////////////////////////////////////////////////////////////////////////
/// @file       GTAQTSInterfaceBaseC.h 
/// @brief      QTS����ӿ�C���Խӿڣ����ṹ�巵�����ݣ��Զ�����
/// @copyright  Copyright (C), 2008-2023, CSMAR Information Tech. Co., Ltd.
/// @version    3.22
/// @date       2022.12.08
//////////////////////////////////////////////////////////////////////////////
#ifndef GTA_QTS_INTERFACE_BASE_C_H_
#define GTA_QTS_INTERFACE_BASE_C_H_
#include "QTSDataType.h"
#include "QTSStruct.h"
#include "QTSDataFieldDefine.h"
extern "C"
{
/// ʵʱ���ݡ�����״̬�ص����������ṹ�巵������
/// ����ϵͳ����ӿڣ����ӷ�����������֤���������ݣ���ѯ����.
/// ��½״̬����
/// @param  errCode  -- ʧ��ԭ��(�û����ڡ��ظ���½)�û�������������д���
typedef void (*OnBaseLoginState)( RetCode errCode);
void GTA_API_EXPORT _CDECL SetOnBaseLoginState(OnBaseLoginState cbOnBaseLoginState);

/// ����״̬���أ����ӳɹ�/ʧ��
/// @param  msgType      -- ��Ϣ����
/// @param  errCode     -- ʧ��ԭ�򣬳ɹ�ʱ����0
typedef void (*OnBaseConnectionState)(MsgType msgType, RetCode errCode);
void GTA_API_EXPORT _CDECL SetOnBaseConnectionState(OnBaseConnectionState cbOnBaseConnectionState);

/// ��ʱ���ݷ���
/// @param  DelayTime      -- ��ʱ����
typedef void (*OnBaseDelayTime)(const DelayTimeInfo* pDelayTime);
void GTA_API_EXPORT _CDECL SetOnBaseDelayTime(OnBaseDelayTime cbOnBaseDelayTime);

/// �Ͻ���L1��̬���ݶ�������ʵʱ�ص��ӿ�
/// @param  pRealSSEL1Static     -- ʵʱ����
typedef void (*OnBaseSubscribe_SSEL1_Static)(const SSEL1_Static* pRealSSEL1Static);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SSEL1_Static(OnBaseSubscribe_SSEL1_Static cbOnBaseSubscribe_SSEL1_Static);

/// �Ͻ���L1ʵʱ���鶩������ʵʱ�ص��ӿ�
/// @param  pRealSSEL1Quotation  -- ʵʱ����
typedef void (*OnBaseSubscribe_SSEL1_Quotation)(const SSEL1_Quotation* pRealSSEL1Quotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SSEL1_Quotation(OnBaseSubscribe_SSEL1_Quotation cbOnBaseSubscribe_SSEL1_Quotation);

/// �Ͻ���ָ��ͨ����ʵʱ�ص��ӿ�
/// @param  pRealSSEIndexPress  -- ʵʱ����
typedef void (*OnBaseSubscribe_SSE_IndexPress)(const SSE_IndexPress* pRealSSEIndexPress);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SSE_IndexPress(OnBaseSubscribe_SSE_IndexPress cbOnBaseSubscribe_SSE_IndexPress);

/// �Ͻ���L2��̬���ݶ�������ʵʱ�ص��ӿ�
/// @param  pRealSSEL2Static     -- ʵʱ����
typedef void (*OnBaseSubscribe_SSEL2_Static)(const SSEL2_Static* pRealSSEL2Static);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SSEL2_Static(OnBaseSubscribe_SSEL2_Static cbOnBaseSubscribe_SSEL2_Static);

/// �Ͻ���L2ʵʱ������ն�������ʵʱ�ص��ӿ�
/// @param  pRealSSEL2Quotation  -- ʵʱ����
typedef void (*OnBaseSubscribe_SSEL2_Quotation)(const SSEL2_Quotation* pRealSSEL2Quotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SSEL2_Quotation(OnBaseSubscribe_SSEL2_Quotation cbOnBaseSubscribe_SSEL2_Quotation);

/// �Ͻ���L2ָ�����鶩������ʵʱ�ص��ӿ�
/// @param  pRealSSEL2Index      -- ʵʱ����
typedef void (*OnBaseSubscribe_SSEL2_Index)(const SSEL2_Index* pRealSSEL2Index);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SSEL2_Index(OnBaseSubscribe_SSEL2_Index cbOnBaseSubscribe_SSEL2_Index);

/// �Ͻ���L2��ʳɽ���������ʵʱ�ص��ӿ�
/// @param  pRealSSEL2Transaction    -- ʵʱ����
typedef void (*OnBaseSubscribe_SSEL2_Transaction)(const SSEL2_Transaction* pRealSSEL2Transaction);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SSEL2_Transaction(OnBaseSubscribe_SSEL2_Transaction cbOnBaseSubscribe_SSEL2_Transaction);

/// �Ͻ���L2���⼯�Ͼ��۶�������ʵʱ�ص��ӿ�
/// @param  pRealSSEL2Auction    -- ʵʱ����
typedef void (*OnBaseSubscribe_SSEL2_Auction)(const SSEL2_Auction* pRealSSEL2Auction);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SSEL2_Auction(OnBaseSubscribe_SSEL2_Auction cbOnBaseSubscribe_SSEL2_Auction);

/// �Ͻ���L2�г�������������ʵʱ�ص��ӿ�
/// @param  pRealSSEL2Overview   -- ʵʱ����
typedef void (*OnBaseSubscribe_SSEL2_Overview)(const SSEL2_Overview* pRealSSEL2Overview);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SSEL2_Overview(OnBaseSubscribe_SSEL2_Overview cbOnBaseSubscribe_SSEL2_Overview);

/// �Ͻ���������Ȩ��̬���ݶ�������ʵʱ�ص��ӿ�
/// @param  pRealSSEIOL1Static   -- ʵʱ����
typedef void (*OnBaseSubscribe_SSEIOL1_Static)(const SSEIOL1_Static* pRealSSEIOL1Static);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SSEIOL1_Static(OnBaseSubscribe_SSEIOL1_Static cbOnBaseSubscribe_SSEIOL1_Static);

/// �Ͻ���������Ȩʵʱ���鶩������ʵʱ�ص��ӿ�
/// @param  pRealSSEIOL1Quotation    -- ʵʱ����
typedef void (*OnBaseSubscribe_SSEIOL1_Quotation)(const SSEIOL1_Quotation* pRealSSEIOL1Quotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SSEIOL1_Quotation(OnBaseSubscribe_SSEIOL1_Quotation cbOnBaseSubscribe_SSEIOL1_Quotation);

/// �Ͻ���������ȨL1��ϲ��Զ�������ʵʱ�ص��ӿ�
/// @param  pRealSSEIOL1Strategy    -- ʵʱ����
typedef void (*OnBaseSubscribe_SSEIOL1_Strategy)(const SSEIOL1_Strategy* pRealSSEIOL1Strategy);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SSEIOL1_Strategy(OnBaseSubscribe_SSEIOL1_Strategy cbOnBaseSubscribe_SSEIOL1_Strategy);

/// ���L1��̬���ݶ�������ʵʱ�ص��ӿ�
/// @param  pRealSZSEL1Static    -- ʵʱ����
typedef void (*OnBaseSubscribe_SZSEL1_Static)(const SZSEL1_Static* pRealSZSEL1Static);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEL1_Static(OnBaseSubscribe_SZSEL1_Static cbOnBaseSubscribe_SZSEL1_Static);

/// ���L1ʵʱ���鶩������ʵʱ�ص��ӿ�
/// @param  pRealSZSEL1Quotation -- ʵʱ����
typedef void (*OnBaseSubscribe_SZSEL1_Quotation)(const SZSEL1_Quotation* pRealSZSEL1Quotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEL1_Quotation(OnBaseSubscribe_SZSEL1_Quotation cbOnBaseSubscribe_SZSEL1_Quotation);

/// ���L1����ص��ӿ�
/// @param  pRealSZSEL1Bullet -- ʵʱ����
typedef void (*OnBaseSubscribe_SZSEL1_Bulletin)(const SZSEL1_Bulletin* pRealSZSEL1Bullet);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEL1_Bulletin(OnBaseSubscribe_SZSEL1_Bulletin cbOnBaseSubscribe_SZSEL1_Bulletin);

/// ���������ȨL1��̬���ݶ�������ʵʱ�ص��ӿ�
/// @param  pRealSZSEIOL1Static    -- ʵʱ����
typedef void (*OnBaseSubscribe_SZSEIOL1_Static)(const SZSEIOL1_Static* pRealSZSEIOL1Static);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEIOL1_Static(OnBaseSubscribe_SZSEIOL1_Static cbOnBaseSubscribe_SZSEIOL1_Static);

/// ���������ȨL1ʵʱ���鶩������ʵʱ�ص��ӿ�
/// @param  pRealSZSEIOL1Quotation    -- ʵʱ����
typedef void (*OnBaseSubscribe_SZSEIOL1_Quotation)(const SZSEIOL1_Quotation* pRealSZSEIOL1Quotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEIOL1_Quotation(OnBaseSubscribe_SZSEIOL1_Quotation cbOnBaseSubscribe_SZSEIOL1_Quotation);

/// ���������ȨL1֤ȯ״̬��������ʵʱ�ص��ӿ�
/// @param  pRealSZSEIOL1Status    -- ʵʱ����
typedef void (*OnBaseSubscribe_SZSEIOL1_Status)(const SZSEIOL1_Status* pRealSZSEIOL1Status);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEIOL1_Status(OnBaseSubscribe_SZSEIOL1_Status cbOnBaseSubscribe_SZSEIOL1_Status);

/// ���������ȨL1��ϲ��Զ�������ʵʱ�ص��ӿ�
/// @param  pRealSZSEIOL1Strategy    -- ʵʱ����
typedef void (*OnBaseSubscribe_SZSEIOL1_Strategy)(const SZSEIOL1_Strategy* pRealSZSEIOL1Strategy);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEIOL1_Strategy(OnBaseSubscribe_SZSEIOL1_Strategy cbOnBaseSubscribe_SZSEIOL1_Strategy);


/// ���L2��̬���ݶ�������ʵʱ�ص��ӿ�
/// @param  pRealSZSEL2Static    -- ʵʱ����
typedef void (*OnBaseSubscribe_SZSEL2_Static)(const SZSEL2_Static* pRealSZSEL2Static);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEL2_Static(OnBaseSubscribe_SZSEL2_Static cbOnBaseSubscribe_SZSEL2_Static);

/// ���L2ʵʱ���鶩������ʵʱ�ص��ӿ�
/// @param  pRealSZSEL2Quotation -- ʵʱ����
typedef void (*OnBaseSubscribe_SZSEL2_Quotation)(const SZSEL2_Quotation* pRealSZSEL2Quotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEL2_Quotation(OnBaseSubscribe_SZSEL2_Quotation cbOnBaseSubscribe_SZSEL2_Quotation);

/// ���L2֤ȯ״̬��������ʵʱ�ص��ӿ�
/// @param  pRealSZSEL2Status    -- ʵʱ����
typedef void (*OnBaseSubscribe_SZSEL2_Status)(const SZSEL2_Status* pRealSZSEL2Status);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEL2_Status(OnBaseSubscribe_SZSEL2_Status cbOnBaseSubscribe_SZSEL2_Status);

/// ���L2ָ�����鶩������ʵʱ�ص��ӿ�
/// @param  pRealSZSEL2Index     -- ʵʱ����
typedef void (*OnBaseSubscribe_SZSEL2_Index)(const SZSEL2_Index* pRealSZSEL2Index);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEL2_Index(OnBaseSubscribe_SZSEL2_Index cbOnBaseSubscribe_SZSEL2_Index);

/// ���L2���ί�ж�������ʵʱ�ص��ӿ�
/// @param  pRealSZSEL2Order     -- ʵʱ����
typedef void (*OnBaseSubscribe_SZSEL2_Order)(const SZSEL2_Order* pRealSZSEL2Order);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEL2_Order(OnBaseSubscribe_SZSEL2_Order cbOnBaseSubscribe_SZSEL2_Order);

/// ���L2��ʳɽ���������ʵʱ�ص��ӿ�
/// @param  pRealSZSEL2Transaction   -- ʵʱ����
typedef void (*OnBaseSubscribe_SZSEL2_Transaction)(const SZSEL2_Transaction* pRealSZSEL2Transaction);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEL2_Transaction(OnBaseSubscribe_SZSEL2_Transaction cbOnBaseSubscribe_SZSEL2_Transaction);

/// ���L2��ʶ�������ʵʱ�ص��ӿ�
/// @param  pRealSZSEL2Tick   -- ʵʱ����
typedef void (*OnBaseSubscribe_SZSEL2_Tick)(const SZSEL2_Tick* pRealSZSEL2Tick);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEL2_Tick(OnBaseSubscribe_SZSEL2_Tick cbOnBaseSubscribe_SZSEL2_Tick);

/// �н�����̬���ݶ�������ʵʱ�ص��ӿ�
/// @param  pRealCFFEXL2Static   -- ʵʱ����
typedef void (*OnBaseSubscribe_CFFEXL2_Static)(const CFFEXL2_Static* pRealCFFEXL2Static);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_CFFEXL2_Static(OnBaseSubscribe_CFFEXL2_Static cbOnBaseSubscribe_CFFEXL2_Static);

/// �н���L2ʵʱ���鶩������ʵʱ�ص��ӿ�
/// @param  pRealCFFEXL2Quotation    -- ʵʱ����
typedef void (*OnBaseSubscribe_CFFEXL2_Quotation)(const CFFEXL2_Quotation* pRealCFFEXL2Quotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_CFFEXL2_Quotation(OnBaseSubscribe_CFFEXL2_Quotation cbOnBaseSubscribe_CFFEXL2_Quotation);

/// ֣������̬���ݶ�������ʵʱ�ص��ӿ�
/// @param  pRealCZCEL1Static    -- ʵʱ����
typedef void (*OnBaseSubscribe_CZCEL1_Static)(const CZCEL1_Static* pRealCZCEL1Static);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_CZCEL1_Static(OnBaseSubscribe_CZCEL1_Static cbOnBaseSubscribe_CZCEL1_Static);

/// ֣����ʵʱ���鶩������ʵʱ�ص��ӿ�
/// @param  pRealCZCEL1Quotation -- ʵʱ����
typedef void (*OnBaseSubscribe_CZCEL1_Quotation)(const CZCEL1_Quotation* pRealCZCEL1Quotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_CZCEL1_Quotation(OnBaseSubscribe_CZCEL1_Quotation cbOnBaseSubscribe_CZCEL1_Quotation);

/// ֣�����������鶩������ʵʱ�ص��ӿ�
/// @param  pRealCZCEL1ArbiQuotation -- ʵʱ����
typedef void (*OnBaseSubscribe_CZCEL1_ArbiQuotation)(const CZCEL1_ArbiQuotation* pRealCZCEL1ArbiQuotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_CZCEL1_ArbiQuotation(OnBaseSubscribe_CZCEL1_ArbiQuotation cbOnBaseSubscribe_CZCEL1_ArbiQuotation);

///֣����L1�г�״̬��������ʵʱ�ص��ӿ�
/// @param  pRealCZCEL1MktStatus -- ʵʱ����
typedef void (*OnBaseSubscribe_CZCEL1_MktStatus)(const CZCEL1_MktStatus* pRealCZCEL1MktStatus);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_CZCEL1_MktStatus(OnBaseSubscribe_CZCEL1_MktStatus cbOnBaseSubscribe_CZCEL1_MktStatus);

/// ֣����L1Ʒ��״̬��������ʵʱ�ص��ӿ�
/// @param  pRealCZCEL1Status -- ʵʱ����
typedef void (*OnBaseSubscribe_CZCEL1_Status)(const CZCEL1_Status* pRealCZCEL1Status);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_CZCEL1_Status(OnBaseSubscribe_CZCEL1_Status cbOnBaseSubscribe_CZCEL1_Status);

/// ��ʢָ����������ʵʱ�ص��ӿ�
/// @param  pRealCZCEL1Quotation -- ʵʱ����
typedef void (*OnBaseSubscribe_ESUNNY_Index)(const ESUNNY_Index* pRealESUNNY_Index);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_ESUNNY_Index(OnBaseSubscribe_ESUNNY_Index cbOnBaseSubscribe_ESUNNY_Index);

/// ������L1��̬���ݶ�������ʵʱ�ص��ӿ�
/// @param  pRealDCEL1Static     -- ʵʱ����
typedef void (*OnBaseSubscribe_DCEL1_Static)(const DCEL1_Static* pRealDCEL1Static);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_DCEL1_Static(OnBaseSubscribe_DCEL1_Static cbOnBaseSubscribe_DCEL1_Static);

/// ������L1�г�״̬��������ʵʱ�ص��ӿ�
/// @param  pRealDCEL1MktStatus     -- ʵʱ����
typedef void (*OnBaseSubscribe_DCEL1_MktStatus)(const DCEL1_MktStatus* pRealDCEL1MktStatus);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_DCEL1_MktStatus(OnBaseSubscribe_DCEL1_MktStatus cbOnBaseSubscribe_DCEL1_MktStatus);

/// ������L1����״̬��������ʵʱ�ص��ӿ�
/// @param  pRealDCEL1Status     -- ʵʱ����
typedef void (*OnBaseSubscribe_DCEL1_Status)(const DCEL1_Status* pRealDCEL1Status);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_DCEL1_Status(OnBaseSubscribe_DCEL1_Status cbOnBaseSubscribe_DCEL1_Status);

/// ������L1��Ȩ������������ʵʱ�ص��ӿ�
/// @param  pRealDCEL1OptionParameter     -- ʵʱ����
typedef void (*OnBaseSubscribe_DCEL1_OptionParameter)(const DCEL1_OptionParameter* pRealDCEL1OptionParameter);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_DCEL1_OptionParameter(OnBaseSubscribe_DCEL1_OptionParameter cbOnBaseSubscribe_DCEL1_OptionParameter);

/// ������L1ʵʱ���鶩������ʵʱ�ص��ӿ�
/// @param  pRealDCEL1Quotation  -- ʵʱ����
typedef void (*OnBaseSubscribe_DCEL1_Quotation)(const DCEL1_Quotation* pRealDCEL1Quotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_DCEL1_Quotation(OnBaseSubscribe_DCEL1_Quotation cbOnBaseSubscribe_DCEL1_Quotation);

/// ������L1�������鶩������ʵʱ�ص��ӿ�
/// @param  pRealDCEL1ArbiQuotation  -- ʵʱ����
typedef void (*OnBaseSubscribe_DCEL1_ArbiQuotation)(const DCEL1_ArbiQuotation* pRealDCEL1ArbiQuotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_DCEL1_ArbiQuotation(OnBaseSubscribe_DCEL1_ArbiQuotation cbOnBaseSubscribe_DCEL1_ArbiQuotation);

/// ������L2��̬���ݶ�������ʵʱ�ص��ӿ�
/// @param  pRealDCEL2Static     -- ʵʱ����
typedef void (*OnBaseSubscribe_DCEL2_Static)(const DCEL2_Static* pRealDCEL2Static);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_DCEL2_Static(OnBaseSubscribe_DCEL2_Static cbOnBaseSubscribe_DCEL2_Static);

/// ������L2ʵʱ���鶩������ʵʱ�ص��ӿ�
/// @param  pRealDCEL2Quotation  -- ʵʱ����
typedef void (*OnBaseSubscribe_DCEL2_Quotation)(const DCEL2_Quotation* pRealDCEL2Quotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_DCEL2_Quotation(OnBaseSubscribe_DCEL2_Quotation cbOnBaseSubscribe_DCEL2_Quotation);

/// ������L2�������鶩������ʵʱ�ص��ӿ�
/// @param  pRealDCEL2ArbiQuotation  -- ʵʱ����
typedef void (*OnBaseSubscribe_DCEL2_ArbiQuotation)(const DCEL2_ArbiQuotation* pRealDCEL2ArbiQuotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_DCEL2_ArbiQuotation(OnBaseSubscribe_DCEL2_ArbiQuotation cbOnBaseSubscribe_DCEL2_ArbiQuotation);

/// ������L2ʵʱ����۶�������ʵʱ�ص��ӿ�
/// @param  pRealDCEL2RealTimePrice  -- ʵʱ����
typedef void (*OnBaseSubscribe_DCEL2_RealTimePrice)(const DCEL2_RealTimePrice* pRealDCEL2RealTimePrice);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_DCEL2_RealTimePrice(OnBaseSubscribe_DCEL2_RealTimePrice cbOnBaseSubscribe_DCEL2_RealTimePrice);

/// ������L2ί��ͳ�����鶩������ʵʱ�ص��ӿ�
/// @param  pRealDCEL2OrderStatistic -- ʵʱ����
typedef void (*OnBaseSubscribe_DCEL2_OrderStatistic)(const DCEL2_OrderStatistic* pRealDCEL2OrderStatistic);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_DCEL2_OrderStatistic(OnBaseSubscribe_DCEL2_OrderStatistic cbOnBaseSubscribe_DCEL2_OrderStatistic);

/// ������L2�ּ۳ɽ������鶩������ʵʱ�ص��ӿ�
/// @param  pRealDCEL2MarchPriceQty  -- ʵʱ����
typedef void (*OnBaseSubscribe_DCEL2_MarchPriceQty)(const DCEL2_MarchPriceQty* pRealDCEL2MarchPriceQty);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_DCEL2_MarchPriceQty(OnBaseSubscribe_DCEL2_MarchPriceQty cbOnBaseSubscribe_DCEL2_MarchPriceQty);

/// ������L2�г�״̬��������ʵʱ�ص��ӿ�
/// @param  pRealDCEL2MktStatus     -- ʵʱ����
typedef void (*OnBaseSubscribe_DCEL2_MktStatus)(const DCEL2_MktStatus* pRealDCEL2MktStatus);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_DCEL2_MktStatus(OnBaseSubscribe_DCEL2_MktStatus cbOnBaseSubscribe_DCEL2_MktStatus);

/// ������L2����״̬��������ʵʱ�ص��ӿ�
/// @param  pRealDCEL2Status     -- ʵʱ����
typedef void (*OnBaseSubscribe_DCEL2_Status)(const DCEL2_Status* pRealDCEL2Status);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_DCEL2_Status(OnBaseSubscribe_DCEL2_Status cbOnBaseSubscribe_DCEL2_Status);

/// ������L2��Ȩ������������ʵʱ�ص��ӿ�
/// @param  pRealDCEL2OptionParameter     -- ʵʱ����
typedef void (*OnBaseSubscribe_DCEL2_OptionParameter)(const DCEL2_OptionParameter* pRealDCEL2OptionParameter);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_DCEL2_OptionParameter(OnBaseSubscribe_DCEL2_OptionParameter cbOnBaseSubscribe_DCEL2_OptionParameter);

/// ������L2�嵵������鶩������ʵʱ�ص��ӿ�
/// @param  pRealDCEL2LevelQuotation     -- ʵʱ����
typedef void (*OnBaseSubscribe_DCEL2_LevelQuotation)(const DCEL2_LevelQuotation* pRealDCEL2LevelQuotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_DCEL2_LevelQuotation(OnBaseSubscribe_DCEL2_LevelQuotation cbOnBaseSubscribe_DCEL2_LevelQuotation);

/// ������L2ί�ж�����������ʵʱ�ص��ӿ�
/// @param  pRealDCEL2OrderQueue     -- ʵʱ����
typedef void (*OnBaseSubscribe_DCEL2_OrderQueue)(const DCEL2_OrderQueue* pRealDCEL2OrderQueue);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_DCEL2_OrderQueue(OnBaseSubscribe_DCEL2_OrderQueue cbOnBaseSubscribe_DCEL2_OrderQueue);

/// ��������̬���ݶ�������ʵʱ�ص��ӿ�
/// @param  pRealSHFEL1Static    -- ʵʱ����
typedef void (*OnBaseSubscribe_SHFEL1_Static)(const SHFEL1_Static* pRealSHFEL1Static);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SHFEL1_Static(OnBaseSubscribe_SHFEL1_Static cbOnBaseSubscribe_SHFEL1_Static);

/// ������ʵʱ���鶩������ʵʱ�ص��ӿ�
/// @param  pRealSHFEL1Quotation -- ʵʱ����
typedef void (*OnBaseSubscribe_SHFEL1_Quotation)(const SHFEL1_Quotation* pRealSHFEL1Quotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SHFEL1_Quotation(OnBaseSubscribe_SHFEL1_Quotation cbOnBaseSubscribe_SHFEL1_Quotation);

/// ������Դ��̬���ݶ�������ʵʱ�ص��ӿ�
/// @param  pRealINEL1Static    -- ʵʱ����
typedef void (*OnBaseSubscribe_INEL1_Static)(const INEL1_Static* pRealINEL1Static);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_INEL1_Static(OnBaseSubscribe_INEL1_Static cbOnBaseSubscribe_INEL1_Static);

/// ������Դʵʱ���鶩������ʵʱ�ص��ӿ�
/// @param  pRealINEL1Quotation -- ʵʱ����
typedef void (*OnBaseSubscribe_INEL1_Quotation)(const INEL1_Quotation* pRealINEL1Quotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_INEL1_Quotation(OnBaseSubscribe_INEL1_Quotation cbOnBaseSubscribe_INEL1_Quotation);

//���ڶ���
/// ������L2��̬���ݶ�������ʵʱ�ص��ӿ�
/// @param  pRealSHFEL2Static    -- ʵʱ����
typedef void (*OnBaseSubscribe_SHFEL2_Static)(const SHFEL2_Static* pRealSHFEL2Static);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SHFEL2_Static(OnBaseSubscribe_SHFEL2_Static cbOnBaseSubscribe_SHFEL2_Static);

/// ������L2ʵʱ���鶩������ʵʱ�ص��ӿ�
/// @param  pRealSHFEL2Quotation -- ʵʱ����
typedef void (*OnBaseSubscribe_SHFEL2_Quotation)(const SHFEL2_Quotation* pRealSHFEL2Quotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SHFEL2_Quotation(OnBaseSubscribe_SHFEL2_Quotation cbOnBaseSubscribe_SHFEL2_Quotation);

/// ������ԴL2��̬���ݶ�������ʵʱ�ص��ӿ�
/// @param  pRealINEL2Static    -- ʵʱ����
typedef void (*OnBaseSubscribe_INEL2_Static)(const INEL2_Static* pRealINEL2Static);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_INEL2_Static(OnBaseSubscribe_INEL2_Static cbOnBaseSubscribe_INEL2_Static);

/// ������ԴL2ʵʱ���鶩������ʵʱ�ص��ӿ�
/// @param  pRealINEL2Quotation -- ʵʱ����
typedef void (*OnBaseSubscribe_INEL2_Quotation)(const INEL2_Quotation* pRealINEL2Quotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_INEL2_Quotation(OnBaseSubscribe_INEL2_Quotation cbOnBaseSubscribe_INEL2_Quotation);


/// �۽���ʵʱ���鶩������ʵʱ�ص��ӿ�
/// @param  pRealHKEXL2Quotation -- ʵʱ����
typedef void (*OnBaseSubscribe_HKEXL2_Quotation)(const HKEXL2_Quotation* pRealHKEXL2Quotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_HKEXL2_Quotation(OnBaseSubscribe_HKEXL2_Quotation cbOnBaseSubscribe_HKEXL2_Quotation);

/// �۽�����̬���ݶ�������ʵʱ�ص��ӿ�
/// @param  pRealHKEXL2Static    -- ʵʱ����
typedef void (*OnBaseSubscribe_HKEXL2_Static)(const HKEXL2_Static* pRealHKEXL2Static);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_HKEXL2_Static(OnBaseSubscribe_HKEXL2_Static cbOnBaseSubscribe_HKEXL2_Static);

/// �۽��������˶������ݶ�������ʵʱ�ص��ӿ�
/// @param  pRealHKEXL2BrokerQueue    -- ʵʱ����
typedef void (*OnBaseSubscribe_HKEXL2_BrokerQueue)(const HKEXL2_BrokerQueue* pRealHKEXL2BrokerQueue);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_HKEXL2_BrokerQueue(OnBaseSubscribe_HKEXL2_BrokerQueue cbOnBaseSubscribe_HKEXL2_BrokerQueue);

/// �۽���ָ�����鶩������ʵʱ�ص��ӿ�
/// @param  pRealHKEXL2Index    -- ʵʱ����
typedef void (*OnBaseSubscribe_HKEXL2_Index)(const HKEXL2_Index* pRealHKEXL2Index);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_HKEXL2_Index(OnBaseSubscribe_HKEXL2_Index cbOnBaseSubscribe_HKEXL2_Index);

/// �۽����г�������������ʵʱ�ص��ӿ�
/// @param  pRealHKEXL2Overview    -- ʵʱ����
typedef void (*OnBaseSubscribe_HKEXL2_Overview)(const HKEXL2_Overview* pRealHKEXL2Overview);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_HKEXL2_Overview(OnBaseSubscribe_HKEXL2_Overview cbOnBaseSubscribe_HKEXL2_Overview);

/// ���ͨ��̬���ݶ�������ʵʱ�ص��ӿ�
/// @param  pRealHKSZL1Static    -- ʵʱ����
typedef void (*OnBaseSubscribe_HKSZL1_Static)(const HKSZL1_Static* pRealHKSZL1Static);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_HKSZL1_Static(OnBaseSubscribe_HKSZL1_Static cbOnBaseSubscribe_HKSZL1_Static);

/// ���ͨʵʱ���鶩������ʵʱ�ص��ӿ�
/// @param  pRealHKSZL1Quotation -- ʵʱ����
typedef void (*OnBaseSubscribe_HKSZL1_Quotation)(const HKSZL1_Quotation* pRealHKSZL1Quotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_HKSZL1_Quotation(OnBaseSubscribe_HKSZL1_Quotation cbOnBaseSubscribe_HKSZL1_Quotation);

/// ����ͨ��̬���ݶ�������ʵʱ�ص��ӿ�
/// @param  pRealHKSHL1Static    -- ʵʱ����
typedef void (*OnBaseSubscribe_HKSHL1_Static)(const HKSHL1_Static* pRealHKSHL1Static);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_HKSHL1_Static(OnBaseSubscribe_HKSHL1_Static cbOnBaseSubscribe_HKSHL1_Static);

/// ����ͨʵʱ���鶩������ʵʱ�ص��ӿ�
/// @param  pRealHKSHL1Quotation -- ʵʱ����
typedef void (*OnBaseSubscribe_HKSHL1_Quotation)(const HKSHL1_Quotation* pRealHKSHL1Quotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_HKSHL1_Quotation(OnBaseSubscribe_HKSHL1_Quotation cbOnBaseSubscribe_HKSHL1_Quotation);

/// �۽���L2�����г�����������ʵʱ�ص��ӿ�
/// @param  pRealHKEXL2DQB -- ʵʱ����
typedef void (*OnBaseSubscribe_HKEXL2_DQB)(const HKEXL2_DQB* pRealHKEXL2DQB);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_HKEXL2_DQB(OnBaseSubscribe_HKEXL2_DQB cbOnBaseSubscribe_HKEXL2_DQB);

/// �۽���L2�ʽ�����������ʵʱ�ص��ӿ�
/// @param  pRealHKEXL2DQB -- ʵʱ����
typedef void (*OnBaseSubscribe_HKEXL2_MoneyFlow)(const HKEXL2_MoneyFlow* pRealHKEXL2MoneyFlow);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_HKEXL2_MoneyFlow(OnBaseSubscribe_HKEXL2_MoneyFlow cbOnBaseSubscribe_HKEXL2_MoneyFlow);

/// ����ͨL1�г�������������ʵʱ�ص��ӿ�
/// @param  pRealHKSHL1Overview -- ʵʱ����
typedef void (*OnBaseSubscribe_HKSHL1_Overview)(const HKSHL1_Overview* pRealHKSHL1Overview);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_HKSHL1_Overview(OnBaseSubscribe_HKSHL1_Overview cbOnBaseSubscribe_HKSHL1_Overview);

/// ���ͨL1�г�������������ʵʱ�ص��ӿ�
/// @param  pRealHKSZL1Overview -- ʵʱ����
typedef void (*OnBaseSubscribe_HKSZL1_Overview)(const HKSZL1_Overview* pRealHKSZL1Overview);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_HKSZL1_Overview(OnBaseSubscribe_HKSZL1_Overview cbOnBaseSubscribe_HKSZL1_Overview);

/// �Ͻ���L2���ί�ж�������ʵʱ�ص��ӿ�
/// @param  pRealSSEL2Order -- ʵʱ����
typedef void (*OnBaseSubscribe_SSEL2_Order)(const SSEL2_Order* pRealSSEL2Order);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SSEL2_Order(OnBaseSubscribe_SSEL2_Order cbOnBaseSubscribe_SSEL2_Order);

/// ���L2��ָ֤����������ʵʱ�ص��ӿ�
/// @param  pRealSZSEL2Cnindex -- ʵʱ����
typedef void (*OnBaseSubscribe_SZSEL2_Cnindex)(const SZSEL2_Cnindex* pRealSZSEL2Cnindex);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEL2_Cnindex(OnBaseSubscribe_SZSEL2_Cnindex cbOnBaseSubscribe_SZSEL2_Cnindex);

/// �Ͻ���L2ծȯ��̬���ݶ�������ʵʱ�ص��ӿ�
/// @param  pRealSSEL2BondStatic -- ʵʱ����
typedef void (*OnBaseSubscribe_SSEL2_BondStatic)(const SSEL2_BondStatic* pRealSSEL2BondStatic);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SSEL2_BondStatic(OnBaseSubscribe_SSEL2_BondStatic cbOnBaseSubscribe_SSEL2_BondStatic);

/// �Ͻ���L2ծȯʵʱ���鶩������ʵʱ�ص��ӿ�
/// @param  pRealSSEL2BondQuotation -- ʵʱ����
typedef void (*OnBaseSubscribe_SSEL2_BondQuotation)(const SSEL2_BondQuotation* pRealSSEL2BondQuotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SSEL2_BondQuotation(OnBaseSubscribe_SSEL2_BondQuotation cbOnBaseSubscribe_SSEL2_BondQuotation);

/// �Ͻ���L2ծȯ������ݶ�������ʵʱ�ص��ӿ�
/// @param  pRealSSEL2BondTick -- ʵʱ����
typedef void (*OnBaseSubscribe_SSEL2_BondTick)(const SSEL2_BondTick* pRealSSEL2BondTick);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SSEL2_BondTick(OnBaseSubscribe_SSEL2_BondTick cbOnBaseSubscribe_SSEL2_BondTick);

/// �Ͻ���L2ծȯ�г�������������ʵʱ�ص��ӿ�
/// @param  pRealSSEL2BondOverview -- ʵʱ����
typedef void (*OnBaseSubscribe_SSEL2_BondOverview)(const SSEL2_BondOverview* pRealSSEL2BondOverview);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SSEL2_BondOverview(OnBaseSubscribe_SSEL2_BondOverview cbOnBaseSubscribe_SSEL2_BondOverview);

/// �Ͻ���L1ծȯ��̬���ݶ�������ʵʱ�ص��ӿ�
/// @param  pRealSSEL1BondStatic -- ʵʱ����
typedef void (*OnBaseSubscribe_SSEL1_BondStatic)(const SSEL1_BondStatic* pRealSSEL1BondStatic);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SSEL1_BondStatic(OnBaseSubscribe_SSEL1_BondStatic cbOnBaseSubscribe_SSEL1_BondStatic);

/// �Ͻ���L1ծȯʵʱ���鶩������ʵʱ�ص��ӿ�
/// @param  pRealSSEL1BondQuotation -- ʵʱ����
typedef void (*OnBaseSubscribe_SSEL1_BondQuotation)(const SSEL1_BondQuotation* pRealSSEL1BondQuotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SSEL1_BondQuotation(OnBaseSubscribe_SSEL1_BondQuotation cbOnBaseSubscribe_SSEL1_BondQuotation);


/// ���L2ծȯ��̬���ݶ�������ʵʱ�ص��ӿ�
/// @param  pRealSZSEL2BondStatic -- ʵʱ����
typedef void (*OnBaseSubscribe_SZSEL2_BondStatic)(const SZSEL2_BondStatic* pRealSZSEL2BondStatic);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEL2_BondStatic(OnBaseSubscribe_SZSEL2_BondStatic cbOnBaseSubscribe_SZSEL2_BondStatic);

/// ���L2ծȯʵʱ�������ݶ�������ʵʱ�ص��ӿ�
/// @param  pRealSZSEL2BondStatic -- ʵʱ����
typedef void (*OnBaseSubscribe_SZSEL2_BondQuotation)(const SZSEL2_BondQuotation* pRealSZSEL2BondQuotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEL2_BondQuotation(OnBaseSubscribe_SZSEL2_BondQuotation cbOnBaseSubscribe_SZSEL2_BondQuotation);

/// ���L2ծȯ��ʳɽ����ݶ�������ʵʱ�ص��ӿ�
/// @param  pRealSZSEL2BondTransaction -- ʵʱ����
typedef void (*OnBaseSubscribe_SZSEL2_BondTransaction)(const SZSEL2_BondTransaction* pRealSZSEL2BondTransaction);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEL2_BondTransaction(OnBaseSubscribe_SZSEL2_BondTransaction cbOnBaseSubscribe_SZSEL2_BondTransaction);

/// ���L2ծȯ���ί�ж�������ʵʱ�ص��ӿ�
/// @param  pRealSZSEL2BondOrder -- ʵʱ����
typedef void (*OnBaseSubscribe_SZSEL2_BondOrder)(const SZSEL2_BondOrder* pRealSZSEL2BondOrder);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEL2_BondOrder(OnBaseSubscribe_SZSEL2_BondOrder cbOnBaseSubscribe_SZSEL2_BondOrder);

/// ���L2ծȯ�����Ϣ��������ʵʱ�ص��ӿ�
/// @param  pRealSZSEL2BondTick -- ʵʱ����
typedef void (*OnBaseSubscribe_SZSEL2_BondTick)(const SZSEL2_BondTick* pRealSZSEL2BondTick);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEL2_BondTick(OnBaseSubscribe_SZSEL2_BondTick cbOnBaseSubscribe_SZSEL2_BondTick);

/// ���L2ծȯ��������Ϣ��������ʵʱ�ص��ӿ�
/// @param  pRealSZSEL2BondTick -- ʵʱ����
typedef void (*OnBaseSubscribe_SZSEL2_BondMbrInfo)(const SZSEL2_BondMbrInfo* pRealSZSEL2BondMbrInfo);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEL2_BondMbrInfo(OnBaseSubscribe_SZSEL2_BondMbrInfo cbOnBaseSubscribe_SZSEL2_BondMbrInfo);

/// ���L2ծȯ����������Ϣ��������ʵʱ�ص��ӿ�
/// @param  pRealSZSEL2BondInvestorInfo -- ʵʱ����
typedef void (*OnBaseSubscribe_SZSEL2_BondInvestorInfo)(const SZSEL2_BondInvestorInfo* pRealSZSEL2BondInvestorInfo);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEL2_BondInvestorInfo(OnBaseSubscribe_SZSEL2_BondInvestorInfo cbOnBaseSubscribe_SZSEL2_BondInvestorInfo);

/// ���L2ծȯ����Ա��Ϣ��������ʵʱ�ص��ӿ�
/// @param  pRealSZSEL2BondInvestorInfo -- ʵʱ����
typedef void (*OnBaseSubscribe_SZSEL2_BondTraderInfo)(const SZSEL2_BondTraderInfo* pRealSZSEL2BondTraderInfo);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEL2_BondTraderInfo(OnBaseSubscribe_SZSEL2_BondTraderInfo cbOnBaseSubscribe_SZSEL2_BondTraderInfo);

/// ���L1ծȯ��̬������Ϣ��������ʵʱ�ص��ӿ�
/// @param  pRealSZSEL1BondStatic -- ʵʱ����
typedef void (*OnBaseSubscribe_SZSEL1_BondStatic)(const SZSEL1_BondStatic* pRealSZSEL1BondStatic);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEL1_BondStatic(OnBaseSubscribe_SZSEL1_BondStatic cbOnBaseSubscribe_SZSEL1_BondStatic);

/// ���L1ծȯʵʱ���鶩������ʵʱ�ص��ӿ�
/// @param  pRealSZSEL1BondQuotation -- ʵʱ����
typedef void (*OnBaseSubscribe_SZSEL1_BondQuotation)(const SZSEL1_BondQuotation* pRealSZSEL1BondQuotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEL1_BondQuotation(OnBaseSubscribe_SZSEL1_BondQuotation cbOnBaseSubscribe_SZSEL1_BondQuotation);

/// ������L1��̬���ݶ�������ʵʱ�ص��ӿ�
/// @param  pRealBSEL1Static    -- ʵʱ����
typedef void (*OnBaseSubscribe_BSEL1_Static)(const BSEL1_Static* pRealBSEL1Static);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_BSEL1_Static(OnBaseSubscribe_BSEL1_Static cbOnBaseSubscribe_BSEL1_Static);

/// ������L1ʵʱ���鶩������ʵʱ�ص��ӿ�
/// @param  pRealSZSEL1Quotation -- ʵʱ����
typedef void (*OnBaseSubscribe_BSEL1_Quotation)(const BSEL1_Quotation* pRealBSEL1Quotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_BSEL1_Quotation(OnBaseSubscribe_BSEL1_Quotation cbOnBaseSubscribe_BSEL1_Quotation);

/// ���L2ծȯ�г�����ԤԼ��Ϣ��������ʵʱ�ص��ӿ�
/// @param  pRealSZSEL2BondBidBookingInfo -- ʵʱ����
typedef void (*OnBaseSubscribe_SZSEL2_BondBidBookingInfo)(const SZSEL2_BondBidBookingInfo* pRealSZSEL2BondBidBookingInfo);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEL2_BondBidBookingInfo(OnBaseSubscribe_SZSEL2_BondBidBookingInfo cbOnBaseSubscribe_SZSEL2_BondBidBookingInfo);

/// ֣����L2��̬���ݶ�������ʵʱ�ص��ӿ�
/// @param  pRealCZCEL2Static -- ʵʱ����
typedef void (*OnBaseSubscribe_CZCEL2_Static)(const CZCEL2_Static* pRealCZCEL2Static);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_CZCEL2_Static(OnBaseSubscribe_CZCEL2_Static cbOnBaseSubscribe_CZCEL2_Static);

/// ֣����L2�г�״̬��������ʵʱ�ص��ӿ�
/// @param  pRealCZCEL2MktStatus -- ʵʱ����
typedef void (*OnBaseSubscribe_CZCEL2_MktStatus)(const CZCEL2_MktStatus* pRealCZCEL2MktStatus);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_CZCEL2_MktStatus(OnBaseSubscribe_CZCEL2_MktStatus cbOnBaseSubscribe_CZCEL2_MktStatus);

/// ֣����L2Ʒ��״̬��������ʵʱ�ص��ӿ�
/// @param  pRealCZCEL2Status -- ʵʱ����
typedef void (*OnBaseSubscribe_CZCEL2_Status)(const CZCEL2_Status* pRealCZCEL2Status);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_CZCEL2_Status(OnBaseSubscribe_CZCEL2_Status cbOnBaseSubscribe_CZCEL2_Status);

/// ֣����L2ʵʱ���鶩������ʵʱ�ص��ӿ�
/// @param  pRealCZCEL2Quotation -- ʵʱ����
typedef void (*OnBaseSubscribe_CZCEL2_Quotation)(const CZCEL2_Quotation* pRealCZCEL2Quotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_CZCEL2_Quotation(OnBaseSubscribe_CZCEL2_Quotation cbOnBaseSubscribe_CZCEL2_Quotation);

/// ֣����L2�嵵���ί�����鶩������ʵʱ�ص��ӿ�
/// @param  pRealCZCEL2LevelQuotation -- ʵʱ����
typedef void (*OnBaseSubscribe_CZCEL2_LevelQuotation)(const CZCEL2_LevelQuotation* pRealCZCEL2LevelQuotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_CZCEL2_LevelQuotation(OnBaseSubscribe_CZCEL2_LevelQuotation cbOnBaseSubscribe_CZCEL2_LevelQuotation);

/// ֣����L2�������鶩�����ݻص��ӿ�
/// @param  pData -- ʵʱ����
typedef void (*OnBaseSubscribe_CZCEL2_ArbiQuotation)(const CZCEL2_ArbiQuotation* pData);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_CZCEL2_ArbiQuotation(OnBaseSubscribe_CZCEL2_ArbiQuotation cbOnBaseSubscribe_CZCEL2_ArbiQuotation);

/// ֣����L2�����嵵������鶩�����ݻص��ӿ�
/// @param  pData -- ʵʱ����
typedef void (*OnBaseSubscribe_CZCEL2_ArbiLevelQuotation)(const CZCEL2_ArbiLevelQuotation* pData);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_CZCEL2_ArbiLevelQuotation(OnBaseSubscribe_CZCEL2_ArbiLevelQuotation cbOnBaseSubscribe_CZCEL2_ArbiLevelQuotation);/// ����ϵͳ����ӿڣ����ӷ�����������֤���������ݣ���ѯ����.

/// �Ͻ���L2����ʵʱ���鶩������ʵʱ�ص��ӿ�
/// @param  pRealSSEL2ShortQuotation -- ʵʱ����
typedef void (*OnBaseSubscribe_SSEL2_ShortQuotation)(const SSEL2_ShortQuotation* pRealSSEL2ShortQuotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SSEL2_ShortQuotation(OnBaseSubscribe_SSEL2_ShortQuotation cbOnBaseSubscribe_SSEL2_ShortQuotation);

/// �Ͻ���L2ί�ж��ж�������ʵʱ�ص��ӿ�
/// @param  pRealSSEL2OrderQueue -- ʵʱ����
typedef void (*OnBaseSubscribe_SSEL2_OrderQueue)(const SSEL2_OrderQueue* pRealSSEL2OrderQueue);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SSEL2_OrderQueue(OnBaseSubscribe_SSEL2_OrderQueue cbOnBaseSubscribe_SSEL2_OrderQueue);

/// ���L2����ʵʱ���鶩������ʵʱ�ص��ӿ�
/// @param  pRealSZSEL2ShortQuotation -- ʵʱ����
typedef void (*OnBaseSubscribe_SZSEL2_ShortQuotation)(const SZSEL2_ShortQuotation* pRealSZSEL2ShortQuotation);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEL2_ShortQuotation(OnBaseSubscribe_SZSEL2_ShortQuotation cbOnBaseSubscribe_SZSEL2_ShortQuotation);

/// ���L2ί�ж��ж�������ʵʱ�ص��ӿ�
/// @param  pReal -- ʵʱ����
typedef void (*OnBaseSubscribe_SZSEL2_OrderQueue)(const SZSEL2_OrderQueue* pRealSZSEL2OrderQueue);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEL2_OrderQueue(OnBaseSubscribe_SZSEL2_OrderQueue cbOnBaseSubscribe_SZSEL2_OrderQueue);

/// �Ͻ���L2 ETFͳ�����ݶ������ݻص��ӿ�
/// @param  pData -- ʵʱ����
typedef void (*OnBaseSubscribe_SSEL2_ETF)(const SSEL2_ETF* pData);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SSEL2_ETF(OnBaseSubscribe_SSEL2_ETF cbOnBaseSubscribe_SSEL2_ETF);

/// ������L1��̬���ݶ������ݻص��ӿ�
/// @param  pData -- ʵʱ����
typedef void (*OnBaseSubscribe_GFEXL1_Static)(const GFEXL1_Static* pData);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_GFEXL1_Static(OnBaseSubscribe_GFEXL1_Static cbOnBaseSubscribe_GFEXL1_Static);
/// ������L1�������鶩�����ݻص��ӿ�
/// @param  pData -- ʵʱ����
typedef void (*OnBaseSubscribe_GFEXL1_ArbiQuotation)(const GFEXL1_ArbiQuotation* pData);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_GFEXL1_ArbiQuotation(OnBaseSubscribe_GFEXL1_ArbiQuotation cbOnBaseSubscribe_GFEXL1_ArbiQuotation);
/// ������L1ʵʱ���鶩�����ݻص��ӿ�
/// @param  pData -- ʵʱ����
typedef void (*OnBaseSubscribe_GFEXL1_Quotation)(const GFEXL1_Quotation* pData);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_GFEXL1_Quotation(OnBaseSubscribe_GFEXL1_Quotation cbOnBaseSubscribe_GFEXL1_Quotation);
/// ������L1��Ȩ�����������ݻص��ӿ�
/// @param  pData -- ʵʱ����
typedef void (*OnBaseSubscribe_GFEXL1_OptionParameter)(const GFEXL1_OptionParameter* pData);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_GFEXL1_OptionParameter(OnBaseSubscribe_GFEXL1_OptionParameter cbOnBaseSubscribe_GFEXL1_OptionParameter);
/// ������L2��̬���ݶ������ݻص��ӿ�
/// @param  pData -- ʵʱ����
typedef void (*OnBaseSubscribe_GFEXL2_Static)(const GFEXL2_Static* pData);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_GFEXL2_Static(OnBaseSubscribe_GFEXL2_Static cbOnBaseSubscribe_GFEXL2_Static);
/// ������L2�������鶩�����ݻص��ӿ�
/// @param  pData -- ʵʱ����
typedef void (*OnBaseSubscribe_GFEXL2_ArbiQuotation)(const GFEXL2_ArbiQuotation* pData);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_GFEXL2_ArbiQuotation(OnBaseSubscribe_GFEXL2_ArbiQuotation cbOnBaseSubscribe_GFEXL2_ArbiQuotation);
/// ������L2ʵʱ���鶩�����ݻص��ӿ�
/// @param  pData -- ʵʱ����
typedef void (*OnBaseSubscribe_GFEXL2_Quotation)(const GFEXL2_Quotation* pData);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_GFEXL2_Quotation(OnBaseSubscribe_GFEXL2_Quotation cbOnBaseSubscribe_GFEXL2_Quotation);
/// ������L2ί�ж������鶩�����ݻص��ӿ�
/// @param  pData -- ʵʱ����
typedef void (*OnBaseSubscribe_GFEXL2_OrderQueue)(const GFEXL2_OrderQueue* pData);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_GFEXL2_OrderQueue(OnBaseSubscribe_GFEXL2_OrderQueue cbOnBaseSubscribe_GFEXL2_OrderQueue);
/// ������L2ί��ͳ�����鶩�����ݻص��ӿ�
/// @param  pData -- ʵʱ����
typedef void (*OnBaseSubscribe_GFEXL2_OrderStatistic)(const GFEXL2_OrderStatistic* pData);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_GFEXL2_OrderStatistic(OnBaseSubscribe_GFEXL2_OrderStatistic cbOnBaseSubscribe_GFEXL2_OrderStatistic);
/// ������L2ʵʱ����۶������ݻص��ӿ�
/// @param  pData -- ʵʱ����
typedef void (*OnBaseSubscribe_GFEXL2_RealTimePrice)(const GFEXL2_RealTimePrice* pData);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_GFEXL2_RealTimePrice(OnBaseSubscribe_GFEXL2_RealTimePrice cbOnBaseSubscribe_GFEXL2_RealTimePrice);
/// ������L2�ּ۳ɽ������鶩�����ݻص��ӿ�
/// @param  pData -- ʵʱ����
typedef void (*OnBaseSubscribe_GFEXL2_MarchPriceQty)(const GFEXL2_MarchPriceQty* pData);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_GFEXL2_MarchPriceQty(OnBaseSubscribe_GFEXL2_MarchPriceQty cbOnBaseSubscribe_GFEXL2_MarchPriceQty);
/// ������L2��Ȩ�����������ݻص��ӿ�
/// @param  pData -- ʵʱ����
typedef void (*OnBaseSubscribe_GFEXL2_OptionParameter)(const GFEXL2_OptionParameter* pData);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_GFEXL2_OptionParameter(OnBaseSubscribe_GFEXL2_OptionParameter cbOnBaseSubscribe_GFEXL2_OptionParameter);
/// ������L2����ί�ж��ж������ݻص��ӿ�
/// @param  pData -- ʵʱ����
typedef void (*OnBaseSubscribe_GFEXL2_ArbiOrderQueue)(const GFEXL2_ArbiOrderQueue* pData);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_GFEXL2_ArbiOrderQueue(OnBaseSubscribe_GFEXL2_ArbiOrderQueue cbOnBaseSubscribe_GFEXL2_ArbiOrderQueue);

/// �Ͻ���L2�̺�̶��۸���ʳɽ��������ݻص��ӿ�
/// @param  pData -- ʵʱ����
typedef void (*OnBaseSubscribe_SSEL2_FpTransaction)(const SSEL2_FpTransaction* pData);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SSEL2_FpTransaction(OnBaseSubscribe_SSEL2_FpTransaction cbOnBaseSubscribe_SSEL2_FpTransaction);
/// �Ͻ���L2������鶩�����ݻص��ӿ�
/// @param  pData -- ʵʱ����
typedef void (*OnBaseSubscribe_SSEL2_Tick)(const SSEL2_Tick* pData);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SSEL2_Tick(OnBaseSubscribe_SSEL2_Tick cbOnBaseSubscribe_SSEL2_Tick);

/// ������ָ������ �������ݻص��ӿ�
/// @param  pData -- ʵʱ����
typedef void (*OnBaseSubscribe_DCE_Index)(const DCE_Index* pData);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_DCE_Index(OnBaseSubscribe_DCE_Index cbOnBaseSubscribe_DCE_Index);

/// �Ͻ���L1�̺�̶��۸����鶩�����ݻص��ӿ�
/// @param  pData -- ʵʱ����
typedef void (*OnBaseSubscribe_SSEL1_FpQuotation)(const SSEL1_FpQuotation* pData);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SSEL1_FpQuotation(OnBaseSubscribe_SSEL1_FpQuotation cbOnBaseSubscribe_SSEL1_FpQuotation);

/// �Ͻ���L2�̺�̶��۸����鶩�����ݻص��ӿ�
/// @param  pData -- ʵʱ����
typedef void (*OnBaseSubscribe_SSEL2_FpQuotation)(const SSEL2_FpQuotation* pData);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SSEL2_FpQuotation(OnBaseSubscribe_SSEL2_FpQuotation cbOnBaseSubscribe_SSEL2_FpQuotation);
/// ���L1�̺�̶��۸����鶩�����ݻص��ӿ�
/// @param  pData -- ʵʱ����
typedef void (*OnBaseSubscribe_SZSEL1_FpQuotation)(const SZSEL1_FpQuotation* pData);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEL1_FpQuotation(OnBaseSubscribe_SZSEL1_FpQuotation cbOnBaseSubscribe_SZSEL1_FpQuotation);
/// ���L2�̺�̶��۸����鶩�����ݻص��ӿ�
/// @param  pData -- ʵʱ����
typedef void (*OnBaseSubscribe_SZSEL2_FpQuotation)(const SZSEL2_FpQuotation* pData);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_SZSEL2_FpQuotation(OnBaseSubscribe_SZSEL2_FpQuotation cbOnBaseSubscribe_SZSEL2_FpQuotation);

/// ������ָ���������鶩�����ݻص��ӿ�
/// @param  pData -- ʵʱ����
typedef void (*OnBaseSubscribe_DCE_Index)(const DCE_Index* pData);
void GTA_API_EXPORT _CDECL SetOnBaseSubscribe_DCE_Index(OnBaseSubscribe_DCE_Index cbOnBaseSubscribe_DCE_Index);

/// ����API������ʼ��
void GTA_API_EXPORT _CDECL BaseInit();

/// ����API��������ʼ��
void GTA_API_EXPORT _CDECL BaseUninit();

/// ���ó�ʱʱ�䣬���ü�ʱ��Ч��ͬ�����õĳ�ʱʱ��
/// @param  nSenond     -- ��ʱʱ�䣬��λ��
void GTA_API_EXPORT _CDECL BaseSetTimeout(int nSecond = TIMEOUT_DEFAULT);

/// �����������ʱ�������ü�ʱ��Ч��
/// @param  nSenond     -- �������ʱ������λ��
void GTA_API_EXPORT _CDECL BaseSetHeartBeatTime(int nSecond = HEART_BEAT_INTERVAL);

/// API�����������ӳٲ���
/// @param  bSwitch     -- ����,true ����  false  �ر�
/// @return RetCode     -- �������, Ret_Success:�ɹ�;Ret_Error:ʧ��(δ��������)
RetCode GTA_API_EXPORT _CDECL BaseDelayTest(bool bSwitch);

/// ע�������������ַ����ע������ͬ��ַ��ʵ�ָ߿��ã�����˳��Ϊ˳�����ӣ�����ʱ��������ǰʹ�õ�ַ
/// @param  pIP         -- ��������ַ.
/// @param  uPort       -- �������˿�.
/// @return RetCode     -- �������
RetCode GTA_API_EXPORT _CDECL BaseRegisterService(const char* pIP, unsigned short uPort);

/// �������L2 Tick���ݶ��̻߳ص�
/// @param  bMTCallBack     -- ����,true ����  false  �ر�
void GTA_API_EXPORT _CDECL BaseSetSZL2TickMTCallBack(bool bMTCallBack = false);

/// ��������������������û���֤��Ϣ��ͬ������
/// @param  pUserName   -- �˺�����
/// @param  pPassword   -- �˺����룬���ܺ�����
/// @return RetCode     -- �������
RetCode GTA_API_EXPORT _CDECL BaseLogin(const char* pUserName, const char* pPassword);

/// ��������������������û���֤��Ϣ��ͬ������
/// @param  pUserName   -- �˺�����
/// @param  pPassword   -- �˺����룬���ܺ�����
/// @param  pOptions    -- ��ѡ���Ӳ����������null�����ж������ʱ���ö��ţ������ָ�
///                        ��ʶ        ��ѡֵ       ˵��
///                        NetType     0��1         �����������ͣ�0��ʾ����(Ĭ��ֵ)��1��ʾ��������
///                        ʾ�����磺"NetType=1"
/// @return RetCode     -- �������
RetCode GTA_API_EXPORT _CDECL BaseLoginX(const char* pUserName, const char* pPassword, const char* pOptions);

/// ��ȡ���鶩�ķ�������ַ��Ϣ
/// @param  ppServerAddrs               -- ��ŷ�������ַ���������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize                       -- �������ݸ���
/// @return RetCode      -- �������
RetCode GTA_API_EXPORT _CDECL BaseGetServerAddr(ServerState** ppServerAddrs, int* pSize);

/// ��ȡ��Ȩ�޵���������.
/// @param  ppDataTypeList  -- ��������ָ�룬���ݴ�С��Ϊ��Ȩ�޵����ݸ����������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize           -- �������ݸ���
/// @return RetCode         -- �������
RetCode GTA_API_EXPORT _CDECL BaseGetMsgTypeList(MsgType** ppDataTypeList, int* pSize);

/// �������飬�ظ�����ͬһ�����ݶ��ʱȡ����
/// @param  msgType     -- ��Ҫ���ĵ���������
/// @param  pCodeList   -- ���ɶ����б��ԡ�,���ָĩβ������\0������ΪNULLʱ����ȫ�г�
/// @return RetCode     -- �������
RetCode GTA_API_EXPORT _CDECL BaseSubscribe(MsgType msgType, char* pCodeList = NULL);

/// ȡ������
/// @param  msgType     -- ��Ҫȡ�����ĵ���������
/// @param  pCodeList   -- ȡ�������б��ԡ�,���ָĩβ������\0������ΪNULLʱȡ�����д���
/// @return RetCode     -- �������
RetCode GTA_API_EXPORT _CDECL BaseUnsubscribe(MsgType msgType, char* pCodeList = NULL);

/// ȡ�����ж�����Ϣ
/// @return RetCode     -- �������
RetCode GTA_API_EXPORT _CDECL BaseUnsubscribeAll();

/// ��ȡ��Ʊ�б�
/// @param  pMarketTag  -- �г���ʶ���ԡ�,���ָ����г���ʶ��ĩβ������\0������������ΪNULL
/// @param  ppStockList -- ���صĸ��ɴ����б����г���־�������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize       -- �������ݸ���
/// @return RetCode     -- �������
RetCode GTA_API_EXPORT _CDECL BaseGetStockList(char* pMarketTag, StockSymbol** ppStockList, int* pSize);

/// ��ѯ�Ͻ���L1��̬����
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapSSEL1Static     -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode              -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SSEL1_Static(char* pCodeList, SSEL1_Static** ppSnapSSEL1Static, int* pSize);

/// ��ѯ�Ͻ���L1ʵʱ����
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapSSEQuotation     -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize                       -- �������ݸ���
/// @return RetCode              -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SSEL1_Quotation(char* pCodeList, SSEL1_Quotation** ppSnapSSEQuotation, int* pSize);

/// ��ѯ�Ͻ���ָ��ͨ����
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  SnapSSEIndexPress   -- ���������б�
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SSE_IndexPress(char* pCodeList, SSE_IndexPress** ppSnapSSEIndexPress, int* pSize);

/// ��ѯ�Ͻ���L2��̬����
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapSSEStatic        -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize                       -- �������ݸ���
/// @return RetCode              -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SSEL2_Static(char* pCodeList, SSEL2_Static** ppSnapSSEL2Static, int* pSize);

/// ��ѯ�Ͻ���L2ʵʱ�������
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapSSEL2Quotation    -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize                       -- �������ݸ���
/// @return RetCode              -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SSEL2_Quotation(char* pCodeList, SSEL2_Quotation** ppSnapSSEL2Quotation, int* pSize);

/// ��ѯ�Ͻ���L2ָ������
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapSSEL2Index        -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize                       -- �������ݸ���
/// @return RetCode              -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SSEL2_Index(char* pCodeList, SSEL2_Index** ppSnapSSEL2Index, int* pSize);

/// ��ѯ�Ͻ���L2��ʳɽ�
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapSSEL2Transation   -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize                       -- �������ݸ���
/// @return RetCode              -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SSEL2_Transaction(char* pCodeList, SSEL2_Transaction** ppSnapSSEL2Transation, int* pSize);

/// ��ѯ�Ͻ���L2���⼯�Ͼ���
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapSSEL2Auction    -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize                       -- �������ݸ���
/// @return RetCode              -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SSEL2_Auction(char* pCodeList, SSEL2_Auction** ppSnapSSEL2Auction, int* pSize);

/// ��ѯ�Ͻ���L2�г�����
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapSSEL2Overview   -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize                       -- �������ݸ���
/// @return RetCode              -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SSEL2_Overview(char* pCodeList, SSEL2_Overview** ppSnapSSEL2Overview, int* pSize);

/// ��ѯ�Ͻ���������Ȩ��̬����
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapSSEIOL1Static   -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize                       -- �������ݸ���
/// @return RetCode              -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SSEIOL1_Static(char* pCodeList, SSEIOL1_Static** ppSnapSSEIOL1Static, int* pSize);

/// ��ѯ�Ͻ���������Ȩʵʱ����
/// @param  pCodeList               -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapSSEIOL1Quotation    -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize                       -- �������ݸ���
/// @return RetCode                 -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SSEIOL1_Quotation(char* pCodeList, SSEIOL1_Quotation** ppSnapSSEIOL1Quotation, int* pSize);

/// ��ѯ�Ͻ���������ȨL1��ϲ���
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapSSEIOL1Strategy    -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize                       -- �������ݸ���
/// @return RetCode              -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SSEIOL1_Strategy(char* pCodeList, SSEIOL1_Strategy** ppSnapSSEIOL1Strategy, int* pSize);

/// ��ѯ���L1��̬����
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapSZSEL1Static    -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize                       -- �������ݸ���
/// @return RetCode              -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEL1_Static(char* pCodeList, SZSEL1_Static** ppSnapSZSEL1Static, int* pSize);

/// ��ѯ���L1ʵʱ����
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapSZSEL1Quotation -- ���������б������BaseReleaseBulletinValue�ͷ��ڴ�
/// @param  pSize                       -- �������ݸ���
/// @return RetCode              -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEL1_Quotation(char* pCodeList, SZSEL1_Quotation** ppSnapSZSEL1Quotation, int* pSize);

/// ��ѯ���L1����
/// @param  pCodeList              -- ���ն����б�ֻ��ΪNULL�����뽫������
/// @param  ppSnapSZSEL1Bullet -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize                       -- �������ݸ���
/// @return RetCode              -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEL1_Bulletin(char* pCodeList, SZSEL1_Bulletin** ppSnapSZSEL1Bullet, int* pSize);

/// ��ѯ���������ȨL1��̬����
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapSZSEIOL1Static    -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize                       -- �������ݸ���
/// @return RetCode              -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEIOL1_Static(char* pCodeList, SZSEIOL1_Static** ppSnapSZSEIOL1Static, int* pSize);

/// ��ѯ���������ȨL1ʵʱ����
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapSZSEIOL1Quotation    -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize                       -- �������ݸ���
/// @return RetCode              -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEIOL1_Quotation(char* pCodeList, SZSEIOL1_Quotation** ppSnapSZSEIOL1Quotation, int* pSize);

/// ��ѯ���������ȨL1֤ȯ״̬
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapSZSEIOL1Status    -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize                       -- �������ݸ���
/// @return RetCode              -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEIOL1_Status(char* pCodeList, SZSEIOL1_Status** ppSnapSZSEIOL1Status, int* pSize);

/// ��ѯ���������ȨL1��ϲ���
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapSZSEIOL1Strategy    -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize                       -- �������ݸ���
/// @return RetCode              -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEIOL1_Strategy(char* pCodeList, SZSEIOL1_Strategy** ppSnapSZSEIOL1Strategy, int* pSize);



/// ��ѯ���L2��̬����
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapSZSEL2Static    -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize                       -- �������ݸ���
/// @return RetCode              -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEL2_Static(char* pCodeList, SZSEL2_Static** ppSnapSZSEL2Static, int* pSize);

/// ��ѯ���L2ʵʱ����
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapSZSEL2Quotation    -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize                       -- �������ݸ���
/// @return RetCode              -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEL2_Quotation(char* pCodeList, SZSEL2_Quotation** ppSnapSZSEL2Quotation, int* pSize);

/// ��ѯ���L2֤ȯ״̬
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapSZSEL2StockStatus  -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize                       -- �������ݸ���
/// @return RetCode              -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEL2_Status(char* pCodeList, SZSEL2_Status** ppSnapSZSEL2StockStatus, int* pSize);

/// ��ѯ���L2ָ������
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapSZSEL2Index        -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize                       -- �������ݸ���
/// @return RetCode              -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEL2_Index(char* pCodeList, SZSEL2_Index** ppSnapSZSEL2Index, int* pSize);

/// ��ѯ���L2���ί��
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapSZSEL2Order        -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize                       -- �������ݸ���
/// @return RetCode              -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEL2_Order(char* pCodeList, SZSEL2_Order** ppSnapSZSEL2Order, int* pSize);

/// ��ѯ���L2��ʳɽ�
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapSZSEL2Transaction  -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize                       -- �������ݸ���
/// @return RetCode              -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEL2_Transaction(char* pCodeList, SZSEL2_Transaction** ppSnapSZSEL2Transaction, int* pSize);

/// ��ѯ���L2���
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapSZSEL2Tick  -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize                       -- �������ݸ���
/// @return RetCode              -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEL2_Tick(char* pCodeList, SZSEL2_Tick** ppSnapSZSEL2Tick, int* pSize);

/// ��ѯ�н�����̬����
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapCFFEXL2Static       -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize                       -- �������ݸ���
/// @return RetCode              -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_CFFEXL2_Static(char* pCodeList, CFFEXL2_Static** ppSnapCFFEXL2Static, int* pSize);

/// ��ѯ�н���L2ʵʱ����
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapCFFEXL2Quotation    -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize                       -- �������ݸ���
/// @return RetCode              -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_CFFEXL2_Quotation(char* pCodeList, CFFEXL2_Quotation** ppSnapCFFEXL2Quotation, int* pSize);


/// ��ѯ֣������̬����
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapCZCEL1Static    -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize                       -- �������ݸ���
/// @return RetCode              -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_CZCEL1_Static(char* pCodeList, CZCEL1_Static** ppSnapCZCEL1Static, int* pSize);

/// ��ѯ֣����ʵʱ����
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapCZCEL1Quotation -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize                       -- �������ݸ���
/// @return RetCode              -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_CZCEL1_Quotation(char* pCodeList, CZCEL1_Quotation** ppSnapCZCEL1Quotation, int* pSize);

/// ��ѯ֣������������
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapCZCEL1ArbiQuotation -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize                       -- �������ݸ���
/// @return RetCode              -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_CZCEL1_ArbiQuotation(char* pCodeList, CZCEL1_ArbiQuotation** ppSnapCZCEL1ArbiQuotation, int* pSize);

/// ��ѯ֣����L1�г�״̬
/// @param  pCodeList				-- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapCZCEL1MktStatus	-- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize                   -- �������ݸ���
/// @return RetCode					-- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_CZCEL1_MktStatus(char* pCodeList, CZCEL1_MktStatus** ppSnapCZCEL1MktStatus, int* pSize);

/// ��ѯ֣����L1Ʒ��״̬
/// @param  pCodeList				-- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapCZCEL1Status		-- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize					-- �������ݸ���
/// @return RetCode					-- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_CZCEL1_Status(char* pCodeList, CZCEL1_Status** ppSnapCZCEL1Status, int* pSize);

/// ��ѯ��ʢָ������
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapESUNNY_Index -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize                       -- �������ݸ���
/// @return RetCode              -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_ESUNNY_Index(char* pCodeList, ESUNNY_Index** ppSnapESUNNY_Index, int* pSize);

/// ��ѯ������L1��̬����
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapDCEL1Static     -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize                       -- �������ݸ���
/// @return RetCode              -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_DCEL1_Static(char* pCodeList, DCEL1_Static** ppSnapDCEL1Static, int* pSize);

/// ��ѯ������L1�г�״̬
/// @param  pCodeList                      -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapDCEL1MktStatus           -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize                          -- �������ݸ���
/// @return RetCode                        -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_DCEL1_MktStatus(char* pCodeList, DCEL1_MktStatus** ppSnapDCEL1MktStatus, int* pSize);

/// ��ѯ������L1����״̬
/// @param  pCodeList                      -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapDCEL1Status              -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize                          -- �������ݸ���
/// @return RetCode                        -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_DCEL1_Status(char* pCodeList, DCEL1_Status** ppSnapDCEL1Status, int* pSize);

/// ��ѯ������L1��Ȩ����
/// @param  pCodeList                      -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapDCEL1OptionParameter     -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize                          -- �������ݸ���
/// @return RetCode                        -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_DCEL1_OptionParameter(char* pCodeList, DCEL1_OptionParameter** ppSnapDCEL1OptionParameter, int* pSize);

/// ��ѯ������L1ʵʱ����
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapDCEL1Quotation  -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize                       -- �������ݸ���
/// @return RetCode              -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_DCEL1_Quotation(char* pCodeList, DCEL1_Quotation** ppSnapDCEL1Quotation, int* pSize);

/// ��ѯ������L1��������
/// @param  pCodeList               -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapDCEL1ArbiQuotation  -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize                       -- �������ݸ���
/// @return RetCode                 -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_DCEL1_ArbiQuotation(char* pCodeList, DCEL1_ArbiQuotation** ppSnapDCEL1ArbiQuotation, int* pSize);

/// ��ѯ������L2��̬����
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapDCEL2Static     -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize                       -- �������ݸ���
/// @return RetCode              -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_DCEL2_Static(char* pCodeList, DCEL2_Static** ppSnapDCEL2Static, int* pSize);

/// ��ѯ������L2ʵʱ����
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapDCEL2Quotation  -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize                       -- �������ݸ���
/// @return RetCode              -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_DCEL2_Quotation(char* pCodeList, DCEL2_Quotation** ppSnapDCEL2Quotation, int* pSize);

/// ��ѯ������L2��������
/// @param  pCodeList               -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapDCEL2ArbiQuotation  -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize                       -- �������ݸ���
/// @return RetCode                 -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_DCEL2_ArbiQuotation(char* pCodeList, DCEL2_ArbiQuotation** ppSnapDCEL2ArbiQuotation, int* pSize);

/// ��ѯ������L2ʵʱ�����
/// @param  pCodeList               -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapDCEL2RealTimePrice  -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize                       -- �������ݸ���
/// @return RetCode                 -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_DCEL2_RealTimePrice(char* pCodeList, DCEL2_RealTimePrice** ppSnapDCEL2RealTimePrice, int* pSize);

/// ��ѯ������L2ί��ͳ������
/// @param  pCodeList               -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapDCEL2OrderStatistic -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize                       -- �������ݸ���
/// @return RetCode                 -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_DCEL2_OrderStatistic(char* pCodeList, DCEL2_OrderStatistic** ppSnapDCEL2OrderStatistic, int* pSize);

/// ��ѯ������L2�ּ۳ɽ�������
/// @param  pCodeList               -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapDCEL2MarchPriceQty  -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize                   -- �������ݸ���
/// @return RetCode                 -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_DCEL2_MarchPriceQty(char* pCodeList, DCEL2_MarchPriceQty** ppSnapDCEL2MarchPriceQty, int* pSize);

/// ��ѯ������L2�г�״̬
/// @param  pCodeList                      -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapDCEL2MktStatus           -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize                          -- �������ݸ���
/// @return RetCode                        -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_DCEL2_MktStatus(char* pCodeList, DCEL2_MktStatus** ppSnapDCEL2MktStatus, int* pSize);

/// ��ѯ������L2����״̬
/// @param  pCodeList                      -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapDCEL2Status              -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize                          -- �������ݸ���
/// @return RetCode                        -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_DCEL2_Status(char* pCodeList, DCEL2_Status** ppSnapDCEL2Status, int* pSize);

/// ��ѯ������L2��Ȩ����
/// @param  pCodeList                      -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapDCEL2OptionParameter     -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize                          -- �������ݸ���
/// @return RetCode                        -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_DCEL2_OptionParameter(char* pCodeList, DCEL2_OptionParameter** ppSnapDCEL2OptionParameter, int* pSize);

/// ��ѯ������L2�嵵�������
/// @param  pCodeList                      -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapDCEL2LevelQuotation      -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize                          -- �������ݸ���
/// @return RetCode                        -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_DCEL2_LevelQuotation(char* pCodeList, DCEL2_LevelQuotation** ppSnapDCEL2LevelQuotation, int* pSize);

/// ��ѯ������L2ί�ж�������
/// @param  pCodeList                      -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapDCEL2OrderQueue     -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize                          -- �������ݸ���
/// @return RetCode                        -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_DCEL2_OrderQueue(char* pCodeList, DCEL2_OrderQueue** ppSnapDCEL2OrderQueue, int* pSize);


/// ��ѯ��������̬����
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapSHFEL1Static    -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SHFEL1_Static(char* pCodeList, SHFEL1_Static** ppSnapSHFEL1Static, int* pSize);

/// ��ѯ������ʵʱ����
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapSHFEL1Quotation -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SHFEL1_Quotation(char* pCodeList, SHFEL1_Quotation** ppSnapSHFEL1Quotation, int* pSize);

/// ��ѯ������Դ��̬����
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapINEL1Static    -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_INEL1_Static(char* pCodeList, INEL1_Static** ppSnapINEL1Static, int* pSize);

/// ��ѯ������Դʵʱ����
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapINEL1Quotation -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_INEL1_Quotation(char* pCodeList, INEL1_Quotation** ppSnapINEL1Quotation, int* pSize);


//���ڶ�����ѯ����
/// ��ѯ������L2��̬����
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapSHFEL2Static    -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SHFEL2_Static(char* pCodeList, SHFEL2_Static** ppSnapSHFEL2Static, int* pSize);

/// ��ѯ������L2ʵʱ����
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapSHFEL2Quotation -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SHFEL2_Quotation(char* pCodeList, SHFEL2_Quotation** ppSnapSHFEL2Quotation, int* pSize);

/// ��ѯ������ԴL2��̬����
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapINEL2Static    -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_INEL2_Static(char* pCodeList, INEL2_Static** ppSnapINEL2Static, int* pSize);

/// ��ѯ������ԴL2ʵʱ����
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapINEL2Quotation -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_INEL2_Quotation(char* pCodeList, INEL2_Quotation** ppSnapINEL2Quotation, int* pSize);

/// ��ѯ�۽���ʵʱ����
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapHKEXL2Quotation -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_HKEXL2_Quotation(char* pCodeList, HKEXL2_Quotation** ppSnapHKEXL2Quotation, int* pSize);

/// ��ѯ�۽�����̬����
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapHKEXL2Static  -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_HKEXL2_Static(char* pCodeList, HKEXL2_Static** ppSnapHKEXL2Static, int* pSize);

/// ��ѯ�۽��������˶���
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapHKEXL2BrokerQueue    -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_HKEXL2_BrokerQueue(char* pCodeList, HKEXL2_BrokerQueue** ppSnapHKEXL2BrokerQueue, int* pSize);

/// ��ѯ�۽���ָ������
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapHKEXL2Index   -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_HKEXL2_Index(char* pCodeList, HKEXL2_Index** ppSnapHKEXL2Index, int* pSize);

/// ��ѯ�۽����г�����
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapHKEXL2Overview  -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_HKEXL2_Overview(char* pCodeList, HKEXL2_Overview** ppSnapHKEXL2Overview, int* pSize);

/// ��ѯ���ͨ��̬����
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapHKSZL1Static  -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_HKSZL1_Static(char* pCodeList, HKSZL1_Static** ppSnapHKSZL1Static, int* pSize);

/// ��ѯ���ͨʵʱ����
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapHKSZL1Quotation -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_HKSZL1_Quotation(char* pCodeList, HKSZL1_Quotation** ppSnapHKSZL1Quotation, int* pSize);

/// ��ѯ����ͨ��̬����
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapHKSHL1Static  -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_HKSHL1_Static(char* pCodeList, HKSHL1_Static** ppSnapHKSHL1Static, int* pSize);

/// ��ѯ����ͨʵʱ����
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapHKSHL1Quotation -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_HKSHL1_Quotation(char* pCodeList, HKSHL1_Quotation** ppSnapHKSHL1Quotation, int* pSize);

/// ��ѯ�۽���L2�����г������
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapHKEXL2DQB -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_HKEXL2_DQB(char* pCodeList, HKEXL2_DQB** ppSnapHKEXL2DQB, int* pSize);

/// ��ѯ�۽���L2�ʽ�����
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapHKEXL2MoneyFlow -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_HKEXL2_MoneyFlow(char* pCodeList, HKEXL2_MoneyFlow** ppSnapHKEXL2MoneyFlow, int* pSize);

/// ��ѯ����ͨL1�г�����
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapHKEXL2MoneyFlow -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_HKSHL1_Overview(char* pCodeList, HKSHL1_Overview** ppSnapHKSHL1Overview, int* pSize);

/// ��ѯ���ͨL1�г�����
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapHKSZL1Overview -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_HKSZL1_Overview(char* pCodeList, HKSZL1_Overview** ppSnapHKSZL1Overview, int* pSize);

/// ��ѯ�Ͻ���L2���ί��
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapSSEL2Order    -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SSEL2_Order(char* pCodeList, SSEL2_Order** ppSnapSSEL2Order, int* pSize);

/// ��ѯ���L2��ָ֤��
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapSZSEL2Cnindex -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEL2_Cnindex(char* pCodeList, SZSEL2_Cnindex** ppSnapSZSEL2Cnindex, int* pSize);

/// ��ѯ�Ͻ���L2ծȯ��̬����
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapSSEL2BondStatic -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SSEL2_BondStatic(char* pCodeList, SSEL2_BondStatic** ppSnapSSEL2BondStatic, int* pSize);

/// ��ѯ�Ͻ���L2ծȯʵʱ����
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapSSEL2BondQuotation -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SSEL2_BondQuotation(char* pCodeList, SSEL2_BondQuotation** ppSnapSSEL2BondQuotation, int* pSize);

/// ��ѯ�Ͻ���L2ծȯ�������
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapSSEL2BondTick -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SSEL2_BondTick(char* pCodeList,SSEL2_BondTick** ppSnapSSEL2BondTick, int* pSize);

/// ��ѯ�Ͻ���L2ծȯ�г�����
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapSSEL2BondOverview -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SSEL2_BondOverview(char* pCodeList, SSEL2_BondOverview** ppSnapSSEL2BondOverview, int* pSize);

/// ��ѯ�Ͻ���L1ծȯ��̬����
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapSSEL1BondStatic -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SSEL1_BondStatic(char* pCodeList, SSEL1_BondStatic** ppSnapSSEL1BondStatic, int* pSize);

/// ��ѯ�Ͻ���L1ծȯʵʱ����
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapSSEL1BondQuotation -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SSEL1_BondQuotation(char* pCodeList, SSEL1_BondQuotation** ppSnapSSEL1BondQuotation, int* pSize);

/// ��ѯ���L2ծȯ��̬����
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapSZSEL2BondStatic -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEL2_BondStatic(char* pCodeList, SZSEL2_BondStatic** ppSnapSZSEL2BondStatic, int* pSize);

/// ��ѯ���L2ծȯʵʱ����
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapSZSEL2BondQuotation -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEL2_BondQuotation(char* pCodeList, SZSEL2_BondQuotation** ppSnapSZSEL2BondQuotation, int* pSize);

/// ��ѯ���L2ծȯ��ʳɽ�
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapSZSEL2BondTransaction -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEL2_BondTransaction(char* pCodeList, SZSEL2_BondTransaction** ppSnapSZSEL2BondTransaction, int* pSize);

/// ��ѯ���L2ծȯ���ί��
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapSZSEL2BondOrder -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEL2_BondOrder(char* pCodeList, SZSEL2_BondOrder** ppSnapSZSEL2BondOrder, int* pSize);

/// ��ѯ���L2ծȯ�����Ϣ
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapSZSEL2BondTick -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEL2_BondTick(char* pCodeList, SZSEL2_BondTick** ppSnapSZSEL2BondTick, int* pSize);

/// ��ѯ���L2ծȯ��������Ϣ
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapSZSEL2BondMbrInfo -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEL2_BondMbrInfo(char* pCodeList, SZSEL2_BondMbrInfo** ppSnapSZSEL2BondMbrInfo, int* pSize);

/// ��ѯ���L2ծȯ����������Ϣ
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapSZSEL2BondMbrInfo -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEL2_BondInvestorInfo(char* pCodeList, SZSEL2_BondInvestorInfo** ppSnapSZSEL2BondInvestorInfo, int* pSize);

/// ��ѯ���L2ծȯ����Ա��Ϣ
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapSZSEL2BondTraderInfo -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEL2_BondTraderInfo(char* pCodeList, SZSEL2_BondTraderInfo** ppSnapSZSEL2BondTraderInfo, int* pSize);

/// ��ѯ���L1ծȯ��̬����
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapSZSEL1BondStatic -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEL1_BondStatic(char* pCodeList, SZSEL1_BondStatic** ppSnapSZSEL1BondStatic, int* pSize);

/// ��ѯ���L1ծȯʵʱ����
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapSZSEL1BondQuotation -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEL1_BondQuotation(char* pCodeList, SZSEL1_BondQuotation** ppSnapSZSEL1BondQuotation, int* pSize);

/// ��ѯ������L1��̬����
/// @param  pCodeList          -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapBSEL1Static -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize              -- �������ݸ���
/// @return RetCode            -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_BSEL1_Static(char* pCodeList, BSEL1_Static** ppSnapBSEL1Static, int* pSize);

/// ��ѯ������L1ʵʱ����
/// @param  pCodeList            -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapBSEL1Quotation -- ���������б������BaseReleaseBulletinValue�ͷ��ڴ�
/// @param  pSize                -- �������ݸ���
/// @return RetCode              -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_BSEL1_Quotation(char* pCodeList, BSEL1_Quotation** ppSnapBSEL1Quotation, int* pSize);

/// ��ѯ���L2ծȯ�г�����ԤԼ��Ϣ
/// @param  pCodeList                      -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapSZSEL2BondBidBookingInfo -- ���������б������BaseReleaseBulletinValue�ͷ��ڴ�
/// @param  pSize                -- �������ݸ���
/// @return RetCode              -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEL2_BondBidBookingInfo(char* pCodeList, SZSEL2_BondBidBookingInfo** ppSnapSZSEL2BondBidBookingInfo, int* pSize);

/// ��ѯ֣����L2��̬����
/// @param  pCodeList                      -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapCZCEL2Static -- ���������б������BaseReleaseBulletinValue�ͷ��ڴ�
/// @param  pSize                -- �������ݸ���
/// @return RetCode              -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_CZCEL2_Static(char* pCodeList, CZCEL2_Static** ppSnapCZCEL2Static, int* pSize);

/// ��ѯ֣����L2�г�״̬
/// @param  pCodeList                      -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapCZCEL2MktStatus -- ���������б������BaseReleaseBulletinValue�ͷ��ڴ�
/// @param  pSize                -- �������ݸ���
/// @return RetCode              -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_CZCEL2_MktStatus(char* pCodeList, CZCEL2_MktStatus** ppSnapCZCEL2MktStatus, int* pSize);

/// ��ѯ֣����L2Ʒ��״̬
/// @param  pCodeList                      -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapCZCEL2Status -- ���������б������BaseReleaseBulletinValue�ͷ��ڴ�
/// @param  pSize                -- �������ݸ���
/// @return RetCode              -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_CZCEL2_Status(char* pCodeList, CZCEL2_Status** ppSnapCZCEL2Status, int* pSize);

/// ��ѯ֣����L2ʵʱ����
/// @param  pCodeList                      -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapCZCEL2Quotation -- ���������б������BaseReleaseBulletinValue�ͷ��ڴ�
/// @param  pSize                -- �������ݸ���
/// @return RetCode              -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_CZCEL2_Quotation(char* pCodeList, CZCEL2_Quotation** ppSnapCZCEL2Quotation, int* pSize);

/// ��ѯ֣����L2�嵵���ί������
/// @param  pCodeList                      -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapCZCEL2LevelQuotation -- ���������б������BaseReleaseBulletinValue�ͷ��ڴ�
/// @param  pSize                -- �������ݸ���
/// @return RetCode              -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_CZCEL2_LevelQuotation(char* pCodeList, CZCEL2_LevelQuotation** ppSnapCZCEL2LevelQuotation, int* pSize);

/// ��ѯ֣����L2�����������
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppData -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_CZCEL2_ArbiQuotation(char* pCodeList, CZCEL2_ArbiQuotation** ppData, int* pSize);

/// ��ѯ֣����L2�����嵵����������
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppData -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_CZCEL2_ArbiLevelQuotation(char* pCodeList, CZCEL2_ArbiLevelQuotation** ppData, int* pSize);

/// ��ѯ�Ͻ���L2����ʵʱ����
/// @param  pCodeList                      -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapSSEL2ShortQuotation -- ���������б������BaseReleaseBulletinValue�ͷ��ڴ�
/// @param  pSize                -- �������ݸ���
/// @return RetCode              -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SSEL2_ShortQuotation(char* pCodeList, SSEL2_ShortQuotation** ppSnapSSEL2ShortQuotation, int* pSize);

/// ��ѯ�Ͻ���L2ί�ж���
/// @param  pCodeList                      -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapSSEL2OrderQueue -- ���������б������BaseReleaseBulletinValue�ͷ��ڴ�
/// @param  pSize                -- �������ݸ���
/// @return RetCode              -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SSEL2_OrderQueue(char* pCodeList, SSEL2_OrderQueue** ppSnapSSEL2OrderQueue, int* pSize);

/// ��ѯ���L2����ʵʱ����
/// @param  pCodeList                      -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapSZSEL2ShortQuotation -- ���������б������BaseReleaseBulletinValue�ͷ��ڴ�
/// @param  pSize                -- �������ݸ���
/// @return RetCode              -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEL2_ShortQuotation(char* pCodeList, SZSEL2_ShortQuotation** ppSnapSZSEL2ShortQuotation, int* pSize);

/// ��ѯ���L2ί�ж���
/// @param  pCodeList                      -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppSnapSZSEL2OrderQueue -- ���������б������BaseReleaseBulletinValue�ͷ��ڴ�
/// @param  pSize                -- �������ݸ���
/// @return RetCode              -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEL2_OrderQueue(char* pCodeList, SZSEL2_OrderQueue** ppSnapSZSEL2OrderQueue, int* pSize);

/// ��ѯ�Ͻ���L2 ETFͳ�����ݿ���
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppData -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SSEL2_ETF(char* pCodeList, SSEL2_ETF** ppData, int* pSize);

/// ��ѯ������L1��̬���ݿ���
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppData -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_GFEXL1_Static(char* pCodeList, GFEXL1_Static** ppData, int* pSize);
/// ��ѯ������L1�����������
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppData -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_GFEXL1_ArbiQuotation(char* pCodeList, GFEXL1_ArbiQuotation** ppData, int* pSize);
/// ��ѯ������L1ʵʱ�������
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppData -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_GFEXL1_Quotation(char* pCodeList, GFEXL1_Quotation** ppData, int* pSize);
/// ��ѯ������L1��Ȩ��������
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppData -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_GFEXL1_OptionParameter(char* pCodeList, GFEXL1_OptionParameter** ppData, int* pSize);
/// ��ѯ������L2��̬���ݿ���
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppData -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_GFEXL2_Static(char* pCodeList, GFEXL2_Static** ppData, int* pSize);
/// ��ѯ������L2�����������
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppData -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_GFEXL2_ArbiQuotation(char* pCodeList, GFEXL2_ArbiQuotation** ppData, int* pSize);
/// ��ѯ������L2ʵʱ�������
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppData -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_GFEXL2_Quotation(char* pCodeList, GFEXL2_Quotation** ppData, int* pSize);
/// ��ѯ������L2ί�ж����������
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppData -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_GFEXL2_OrderQueue(char* pCodeList, GFEXL2_OrderQueue** ppData, int* pSize);
/// ��ѯ������L2ί��ͳ���������
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppData -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_GFEXL2_OrderStatistic(char* pCodeList, GFEXL2_OrderStatistic** ppData, int* pSize);
/// ��ѯ������L2ʵʱ����ۿ���
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppData -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_GFEXL2_RealTimePrice(char* pCodeList, GFEXL2_RealTimePrice** ppData, int* pSize);
/// ��ѯ������L2�ּ۳ɽ����������
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppData -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_GFEXL2_MarchPriceQty(char* pCodeList, GFEXL2_MarchPriceQty** ppData, int* pSize);
/// ��ѯ������L2��Ȩ��������
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppData -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_GFEXL2_OptionParameter(char* pCodeList, GFEXL2_OptionParameter** ppData, int* pSize);

/// ��ѯ������L2����ί�ж��п���
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppData -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_GFEXL2_ArbiOrderQueue(char* pCodeList, GFEXL2_ArbiOrderQueue** ppData, int* pSize);
/// ��ѯ�Ͻ���L2�̺�̶��۸���ʳɽ�����
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppData -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SSEL2_FpTransaction(char* pCodeList, SSEL2_FpTransaction** ppData, int* pSize);
/// ��ѯ�Ͻ���L2����������
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppData -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SSEL2_Tick(char* pCodeList, SSEL2_Tick** ppData, int* pSize);

/// ��ѯ������ָ������ ����
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppData -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_DCE_Index(char* pCodeList, DCE_Index** ppData, int* pSize);

/// ��ѯ�Ͻ���L1�̺�̶��۸��������
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppData -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SSEL1_FpQuotation(char* pCodeList, SSEL1_FpQuotation** ppData, int* pSize);

/// ��ѯ�Ͻ���L2�̺�̶��۸��������
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppData -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SSEL2_FpQuotation(char* pCodeList, SSEL2_FpQuotation** ppData, int* pSize);
/// ��ѯ���L1�̺�̶��۸��������
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppData -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEL1_FpQuotation(char* pCodeList, SZSEL1_FpQuotation** ppData, int* pSize);
/// ��ѯ���L2�̺�̶��۸��������
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppData -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_SZSEL2_FpQuotation(char* pCodeList, SZSEL2_FpQuotation** ppData, int* pSize);
/// ��ѯ������ָ���������
/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
/// @param  ppData -- ���������б������BaseReleaseValue�ͷ��ڴ�
/// @param  pSize               -- �������ݸ���
/// @return RetCode             -- �������
RetCode GTA_API_EXPORT _CDECL BaseQuerySnap_DCE_Index(char* pCodeList, DCE_Index** ppData, int* pSize);

/// @param  ppDataValue         -- ���ͷ��ڴ�
void GTA_API_EXPORT _CDECL BaseReleaseValue(void** ppDataValue);
};

#endif // GTA_QTS_INTERFACE_BASE_C_H_
