//////////////////////////////////////////////////////////////////////////////
/// @file       GTAQTSInterfaceBase.h 
/// @brief      QTS����ӿڶ��壬���ṹ�巵�����ݣ��Զ�����
/// @copyright  Copyright (C), 2008-2023, CSMAR Information Tech. Co., Ltd.
/// @version    3.22
/// @date       2022.12.08
//////////////////////////////////////////////////////////////////////////////
#ifndef GTA_QTS_INTERFACE_BASE_H_
#define GTA_QTS_INTERFACE_BASE_H_
#include "QTSDataType.h"
#include "QTSStruct.h"
#include "QTSDataFieldDefine.h"

/// ʵʱ���ݡ�����״̬�ص��ӿڣ����ṹ�巵������
/// API�ڲ�ʹ�ö���߳̽������ݽ������˴���ע����߳����⡣״̬�ص�����һ���̡߳�
/// �̷߳��� �����������Ϣ������һ���̣߳��绦�����ί�У���ʳɽ�
class GTA_API_EXPORT IGTAQTSCallbackBase
{
public:
    virtual ~IGTAQTSCallbackBase(){}
    /// ��½״̬����
    /// @param  errCode  -- ʧ��ԭ��(�û����ڡ��ظ���½��ͬ�˺�ͬʱ��ص�¼��ǰ�ε�¼�˺�����API����)�û�������������д���
    virtual void OnLoginState( RetCode errCode){}

    /// ����״̬���أ����ӳɹ�/ʧ��
    /// @param  msgType      -- ��Ϣ����
    /// @param  errCode     -- ʧ��ԭ�򣬳ɹ�ʱ����0
    virtual void OnConnectionState(MsgType msgType, RetCode errCode){}

    /// ��ʱ���ݷ���
    /// @param  DelayTime      -- ��ʱ����
    virtual void OnDelayTime(const DelayTimeInfo& DelayTime){}

    /// �Ͻ���L1��̬���ݶ�������ʵʱ�ص��ӿ�
    /// @param  RealSSEL1Static     -- ʵʱ����
    virtual void OnSubscribe_SSEL1_Static(const SSEL1_Static& RealSSEL1Static){}

    /// �Ͻ���L1ʵʱ���鶩������ʵʱ�ص��ӿ�
    /// @param  RealSSEL1Quotation  -- ʵʱ����
    virtual void OnSubscribe_SSEL1_Quotation(const SSEL1_Quotation& RealSSEL1Quotation){}

    /// �Ͻ���ָ��ͨ���ݶ�������ʵʱ�ص��ӿ�
    /// @param  RealSSEIndexPress-- ʵʱ����
    virtual void OnSubscribe_SSE_IndexPress(const SSE_IndexPress& RealSSEIndexPress){}

    /// �Ͻ���L2��̬���ݶ�������ʵʱ�ص��ӿ�
    /// @param  RealSSEL2Static     -- ʵʱ����
    virtual void OnSubscribe_SSEL2_Static(const SSEL2_Static& RealSSEL2Static){}

    /// �Ͻ���L2ʵʱ������ն�������ʵʱ�ص��ӿ�
    /// @param  RealSSEL2Quotation  -- ʵʱ����
    virtual void OnSubscribe_SSEL2_Quotation(const SSEL2_Quotation& RealSSEL2Quotation){}

    /// �Ͻ���L2ָ�����鶩������ʵʱ�ص��ӿ�
    /// @param  RealSSEL2Index      -- ʵʱ����
    virtual void OnSubscribe_SSEL2_Index(const SSEL2_Index& RealSSEL2Index){}

    /// �Ͻ���L2��ʳɽ���������ʵʱ�ص��ӿ�
    /// @param  RealSSEL2Transaction    -- ʵʱ����
    virtual void OnSubscribe_SSEL2_Transaction(const SSEL2_Transaction& RealSSEL2Transaction){}

    /// �Ͻ���L2���⼯�Ͼ��۶�������ʵʱ�ص��ӿ�
    /// @param  RealSSEL2Auction    -- ʵʱ����
    virtual void OnSubscribe_SSEL2_Auction(const SSEL2_Auction& RealSSEL2Auction){}

    /// �Ͻ���L2�г�������������ʵʱ�ص��ӿ�
    /// @param  RealSSEL2Overview   -- ʵʱ����
    virtual void OnSubscribe_SSEL2_Overview(const SSEL2_Overview& RealSSEL2Overview){}

    /// �Ͻ���������Ȩ��̬���ݶ�������ʵʱ�ص��ӿ�
    /// @param  RealSSEIOL1Static   -- ʵʱ����
    virtual void OnSubscribe_SSEIOL1_Static(const SSEIOL1_Static& RealSSEIOL1Static){}

	/// �Ͻ���������ȨL1��ϲ��Զ�������ʵʱ�ص��ӿ�
	/// @param  RealSSEIOL1Strategy    -- ʵʱ����
	virtual void OnSubscribe_SSEIOL1_Strategy(const SSEIOL1_Strategy& RealSSEIOL1Strategy){}

    /// �Ͻ���������Ȩʵʱ���鶩������ʵʱ�ص��ӿ�
    /// @param  RealSSEIOL1Quotation    -- ʵʱ����
    virtual void OnSubscribe_SSEIOL1_Quotation(const SSEIOL1_Quotation& RealSSEIOL1Quotation){}

    /// ���L1��̬���ݶ�������ʵʱ�ص��ӿ�
    /// @param  RealSZSEL1Static    -- ʵʱ����
    virtual void OnSubscribe_SZSEL1_Static(const SZSEL1_Static& RealSZSEL1Static){}

    /// ���L1ʵʱ���鶩������ʵʱ�ص��ӿ�
    /// @param  RealSZSEL1Quotation -- ʵʱ����
    virtual void OnSubscribe_SZSEL1_Quotation(const SZSEL1_Quotation& RealSZSEL1Quotation){}

    /// ���L1������Ϣ�ص��ӿ�
    /// @param  RealSZSEL1Bullet -- ʵʱ����
    virtual void OnSubscribe_SZSEL1_Bulletin(const SZSEL1_Bulletin& RealSZSEL1Bullet){}

    /// ���������ȨL1��̬���ݶ�������ʵʱ�ص��ӿ�
    /// @param  RealSZSEIOL1Static    -- ʵʱ����
    virtual void OnSubscribe_SZSEIOL1_Static(const SZSEIOL1_Static& RealSZSEIOL1Static){}

    /// ���������ȨL1ʵʱ���鶩������ʵʱ�ص��ӿ�
    /// @param  RealSZSEIOL1Quotation    -- ʵʱ����
    virtual void OnSubscribe_SZSEIOL1_Quotation(const SZSEIOL1_Quotation& RealSZSEIOL1Quotation){}

    /// ���������ȨL1֤ȯ״̬��������ʵʱ�ص��ӿ�
    /// @param  RealSZSEIOL1Status    -- ʵʱ����
    virtual void OnSubscribe_SZSEIOL1_Status(const SZSEIOL1_Status& RealSZSEIOL1Status){}

    /// ���������ȨL1��ϲ��Զ�������ʵʱ�ص��ӿ�
    /// @param  RealSZSEIOL1Strategy    -- ʵʱ����
    virtual void OnSubscribe_SZSEIOL1_Strategy(const SZSEIOL1_Strategy& RealSZSEIOL1Strategy){}


    /// ���L2��̬���ݶ�������ʵʱ�ص��ӿ�
    /// @param  RealSZSEL2Static    -- ʵʱ����
    virtual void OnSubscribe_SZSEL2_Static(const SZSEL2_Static& RealSZSEL2Static){}

    /// ���L2ʵʱ���鶩������ʵʱ�ص��ӿ�
    /// @param  RealSZSEL2Quotation -- ʵʱ����
    virtual void OnSubscribe_SZSEL2_Quotation(const SZSEL2_Quotation& RealSZSEL2Quotation){}

    /// ���L2֤ȯ״̬��������ʵʱ�ص��ӿ�
    /// @param  RealSZSEL2Status    -- ʵʱ����
    virtual void OnSubscribe_SZSEL2_Status(const SZSEL2_Status& RealSZSEL2Status){}

    /// ���L2ָ�����鶩������ʵʱ�ص��ӿ�
    /// @param  RealSZSEL2Index     -- ʵʱ����
    virtual void OnSubscribe_SZSEL2_Index(const SZSEL2_Index& RealSZSEL2Index){}

    /// ���L2���ί�ж�������ʵʱ�ص��ӿ�
    /// @param  RealSZSEL2Order     -- ʵʱ����
    virtual void OnSubscribe_SZSEL2_Order(const SZSEL2_Order& RealSZSEL2Order){}

    /// ���L2��ʳɽ���������ʵʱ�ص��ӿ�
    /// @param  RealSZSEL2Transaction   -- ʵʱ����
    virtual void OnSubscribe_SZSEL2_Transaction(const SZSEL2_Transaction& RealSZSEL2Transaction){}

	/// ���L2��ʶ�������ʵʱ�ص��ӿ�
	/// @param  RealSZSEL2Tick   -- ʵʱ����
	virtual void OnSubscribe_SZSEL2_Tick(const SZSEL2_Tick& RealSZSEL2Tick){}

    /// �н�����̬���ݶ�������ʵʱ�ص��ӿ�
    /// @param  RealCFFEXL2Static   -- ʵʱ����
    virtual void OnSubscribe_CFFEXL2_Static(const CFFEXL2_Static& RealCFFEXL2Static){}

    /// �н���L2ʵʱ���鶩������ʵʱ�ص��ӿ�
    /// @param  RealCFFEXL2Quotation    -- ʵʱ����
    virtual void OnSubscribe_CFFEXL2_Quotation(const CFFEXL2_Quotation& RealCFFEXL2Quotation){}

    /// ֣������̬���ݶ�������ʵʱ�ص��ӿ�
    /// @param  RealCZCEL1Static    -- ʵʱ����
    virtual void OnSubscribe_CZCEL1_Static(const CZCEL1_Static& RealCZCEL1Static){}

    /// ֣����ʵʱ���鶩������ʵʱ�ص��ӿ�
    /// @param  RealCZCEL1Quotation -- ʵʱ����
    virtual void OnSubscribe_CZCEL1_Quotation(const CZCEL1_Quotation& RealCZCEL1Quotation){}

	/// ֣�����������鶩������ʵʱ�ص��ӿ�
	/// @param  RealCZCEL1ArbiQuotation -- ʵʱ����
	virtual void OnSubscribe_CZCEL1_ArbiQuotation(const CZCEL1_ArbiQuotation& RealCZCEL1ArbiQuotation){}

	/// ֣�����г�״̬��������ʵʱ�ص��ӿ�
	/// @param  RealCZCEL1MktStatus -- ʵʱ����
	virtual void OnSubscribe_CZCEL1_MktStatus(const CZCEL1_MktStatus& RealCZCEL1MktStatus){}

	/// ֣����Ʒ��״̬��������ʵʱ�ص��ӿ�
	/// @param  RealCZCEL1Status -- ʵʱ����
	virtual void OnSubscribe_CZCEL1_Status(const CZCEL1_Status& RealCZCEL1Status){}

    /// ��ʢָ����������ʵʱ�ص��ӿ�
    /// @param  RealESUNNY_Index -- ʵʱ����
    virtual void OnSubscribe_ESUNNY_Index(const ESUNNY_Index& RealESUNNY_Index){}

    /// ������L1��̬���ݶ�������ʵʱ�ص��ӿ�
    /// @param  RealDCEL1Static     -- ʵʱ����
    virtual void OnSubscribe_DCEL1_Static(const DCEL1_Static& RealDCEL1Static){}

	/// ������L1�г�״̬��������ʵʱ�ص��ӿ�
	/// @param  RealDCEL1MktStatus     -- ʵʱ����
	virtual void OnSubscribe_DCEL1_MktStatus(const DCEL1_MktStatus& RealDCEL1MktStatus){}

	/// ������L1����״̬��������ʵʱ�ص��ӿ�
	/// @param  RealDCEL1Status     -- ʵʱ����
	virtual void OnSubscribe_DCEL1_Status(const DCEL1_Status& RealDCEL1Status){}

	/// ������L1��̬���ݶ�������ʵʱ�ص��ӿ�
	/// @param  RealDCEL1OptionParameter     -- ʵʱ����
	virtual void OnSubscribe_DCEL1_OptionParameter(const DCEL1_OptionParameter& RealDCEL1OptionParameter){}

    /// ������L1ʵʱ���鶩������ʵʱ�ص��ӿ�
    /// @param  RealDCEL1Quotation  -- ʵʱ����
    virtual void OnSubscribe_DCEL1_Quotation(const DCEL1_Quotation& RealDCEL1Quotation){}

    /// ������L1�������鶩������ʵʱ�ص��ӿ�
    /// @param  RealDCEL1ArbiQuotation  -- ʵʱ����
    virtual void OnSubscribe_DCEL1_ArbiQuotation(const DCEL1_ArbiQuotation& RealDCEL1ArbiQuotation){}

    /// ������L2��̬���ݶ�������ʵʱ�ص��ӿ�
    /// @param  RealDCEL2Static     -- ʵʱ����
    virtual void OnSubscribe_DCEL2_Static(const DCEL2_Static& RealDCEL2Static){}

    /// ������L2ʵʱ���鶩������ʵʱ�ص��ӿ�
    /// @param  RealDCEL2Quotation  -- ʵʱ����
    virtual void OnSubscribe_DCEL2_Quotation(const DCEL2_Quotation& RealDCEL2Quotation){}

    /// ������L2�������鶩������ʵʱ�ص��ӿ�
    /// @param  RealDCEL2ArbiQuotation  -- ʵʱ����
    virtual void OnSubscribe_DCEL2_ArbiQuotation(const DCEL2_ArbiQuotation& RealDCEL2ArbiQuotation){}

    /// ������L2ʵʱ����۶�������ʵʱ�ص��ӿ�
    /// @param  RealDCEL2RealTimePrice  -- ʵʱ����
    virtual void OnSubscribe_DCEL2_RealTimePrice(const DCEL2_RealTimePrice& RealDCEL2RealTimePrice){}

    /// ������L2ί��ͳ�����鶩������ʵʱ�ص��ӿ�
    /// @param  RealDCEL2OrderStatistic -- ʵʱ����
    virtual void OnSubscribe_DCEL2_OrderStatistic(const DCEL2_OrderStatistic& RealDCEL2OrderStatistic){}

    /// ������L2�ּ۳ɽ������鶩������ʵʱ�ص��ӿ�
    /// @param  RealDCEL2MarchPriceQty  -- ʵʱ����
    virtual void OnSubscribe_DCEL2_MarchPriceQty(const DCEL2_MarchPriceQty& RealDCEL2MarchPriceQty){}

	/// ������L2�г�״̬��������ʵʱ�ص��ӿ�
	/// @param  RealDCEL2MktStatus     -- ʵʱ����
	virtual void OnSubscribe_DCEL2_MktStatus(const DCEL2_MktStatus& RealDCEL2MktStatus){}

	/// ������L2����״̬��������ʵʱ�ص��ӿ�
	/// @param  RealDCEL2Status     -- ʵʱ����
	virtual void OnSubscribe_DCEL2_Status(const DCEL2_Status& RealDCEL2Status){}

	/// ������L2��̬���ݶ�������ʵʱ�ص��ӿ�
	/// @param  RealDCEL2OptionParameter     -- ʵʱ����
	virtual void OnSubscribe_DCEL2_OptionParameter(const DCEL2_OptionParameter& RealDCEL2OptionParameter){}

	/// ������L2�嵵������鶩������ʵʱ�ص��ӿ�
	/// @param  RealDCEL2LevelQuotation     -- ʵʱ����
	virtual void OnSubscribe_DCEL2_LevelQuotation(const DCEL2_LevelQuotation& RealDCEL2LevelQuotation){}

	/// ������L2ί�ж������鶩������ʵʱ�ص��ӿ�
	/// @param  RealDCEL2OrderQueue     -- ʵʱ����
	virtual void OnSubscribe_DCEL2_OrderQueue(const DCEL2_OrderQueue& RealDCEL2OrderQueue){}

    /// ��������̬���ݶ�������ʵʱ�ص��ӿ�
    /// @param  RealSHFEL1Static    -- ʵʱ����
    virtual void OnSubscribe_SHFEL1_Static(const SHFEL1_Static& RealSHFEL1Static){}

    /// ��������ʵʱ���鶩������ʵʱ�ص��ӿ�
    /// @param  RealSHFEL1Quotation -- ʵʱ����
    virtual void OnSubscribe_SHFEL1_Quotation(const SHFEL1_Quotation& RealSHFEL1Quotation){}

    /// ������Դ��̬���ݶ�������ʵʱ�ص��ӿ�
    /// @param  RealINEL1Static    -- ʵʱ����
    virtual void OnSubscribe_INEL1_Static(const INEL1_Static& RealINEL1Static){}

    /// ������Դʵʱ���鶩������ʵʱ�ص��ӿ�
    /// @param  RealINEL1Quotation -- ʵʱ����
    virtual void OnSubscribe_INEL1_Quotation(const INEL1_Quotation& RealINEL1Quotation){}

    //����������
    /// ������L2��̬���ݶ�������ʵʱ�ص��ӿ�
    /// @param  RealSHFEL2Static    -- ʵʱ����
    virtual void OnSubscribe_SHFEL2_Static(const SHFEL2_Static& RealSHFEL2Static){}

    /// ������L2ʵʱ���鶩������ʵʱ�ص��ӿ�
    /// @param  RealSHFEL2Quotation -- ʵʱ����
    virtual void OnSubscribe_SHFEL2_Quotation(const SHFEL2_Quotation& RealSHFEL2Quotation){}

    /// ������ԴL2��̬���ݶ�������ʵʱ�ص��ӿ�
    /// @param  RealINEL2Static    -- ʵʱ����
    virtual void OnSubscribe_INEL2_Static(const INEL2_Static& RealINEL2Static){}

    /// ������ԴL2ʵʱ���鶩������ʵʱ�ص��ӿ�
    /// @param  RealINEL2Quotation -- ʵʱ����
    virtual void OnSubscribe_INEL2_Quotation(const INEL2_Quotation& RealINEL2Quotation){}

    /// �۽���ʵʱ���鶩������ʵʱ�ص��ӿ�
    /// @param  RealHKEXL2Quotation -- ʵʱ����
    virtual void OnSubscribe_HKEXL2_Quotation(const HKEXL2_Quotation& RealHKEXL2Quotation){}

    /// �۽��������˶��ж�������ʵʱ�ص��ӿ�
    /// @param  RealHKEXL2BrokerQueue -- ʵʱ����
    virtual void OnSubscribe_HKEXL2_BrokerQueue(const HKEXL2_BrokerQueue& RealHKEXL2BrokerQueue){}

    /// �۽���ָ�����鶩������ʵʱ�ص��ӿ�
    /// @param  RealHKEXL2Index -- ʵʱ����
    virtual void OnSubscribe_HKEXL2_Index(const HKEXL2_Index& RealHKEXL2Index){}

    /// �۽����г�������������ʵʱ�ص��ӿ�
    /// @param  RealHKEXL2Overview -- ʵʱ����
    virtual void OnSubscribe_HKEXL2_Overview(const HKEXL2_Overview& RealHKEXL2Overview){}

    /// �۽�����̬���ݶ�������ʵʱ�ص��ӿ�
    /// @param  RealHKEXL2Static -- ʵʱ����
    virtual void OnSubscribe_HKEXL2_Static(const HKEXL2_Static& RealHKEXL2Static){}

	/// �۽���L2�����г�����������ʵʱ�ص��ӿ�
	/// @param  RealHKEXL2DQB -- ʵʱ����
	virtual void OnSubscribe_HKEXL2_DQB(const HKEXL2_DQB& RealHKEXL2DQB){}

	/// �۽���L2�ʽ�����������ʵʱ�ص��ӿ�
	/// @param  RealHKEXL2MoneyFlow -- ʵʱ����
	virtual void OnSubscribe_HKEXL2_MoneyFlow(const HKEXL2_MoneyFlow& RealHKEXL2MoneyFlow){}

    /// ���ͨ��̬���ݶ�������ʵʱ�ص��ӿ�
    /// @param  RealHKSZL1Static -- ʵʱ����
    virtual void OnSubscribe_HKSZL1_Static(const HKSZL1_Static& RealHKSZL1Static){}

    /// ���ͨʵʱ�������ݶ�������ʵʱ�ص��ӿ�
    /// @param  RealHKSZL1Quotation -- ʵʱ����
    virtual void OnSubscribe_HKSZL1_Quotation(const HKSZL1_Quotation& RealHKSZL1Quotation){}

    /// ����ͨ��̬���ݶ�������ʵʱ�ص��ӿ�
    /// @param  RealHKSZL1Static -- ʵʱ����
    virtual void OnSubscribe_HKSHL1_Static(const HKSHL1_Static& RealHKSHL1Static){}

    /// ���ͨʵʱ�������ݶ�������ʵʱ�ص��ӿ�
    /// @param  RealHKSHL1Quotation -- ʵʱ����
    virtual void OnSubscribe_HKSHL1_Quotation(const HKSHL1_Quotation& RealHKSHL1Quotation){}

	/// ����ͨL1�г��������ݶ�������ʵʱ�ص��ӿ�
	/// @param  RealHKSHL1Overview -- ʵʱ����
	virtual void OnSubscribe_HKSHL1_Overview(const HKSHL1_Overview& RealHKSHL1Overview){}

	/// ���ͨL1�г��������ݶ�������ʵʱ�ص��ӿ�
	/// @param  RealHKSZL1Overview -- ʵʱ����
	virtual void OnSubscribe_HKSZL1_Overview(const HKSZL1_Overview& RealHKSZL1Overview){}

	/// �Ͻ���L2���ί�ж�������ʵʱ�ص��ӿ�
	/// @param  RealSSEL2Order    -- ʵʱ����
	virtual void OnSubscribe_SSEL2_Order(const SSEL2_Order& RealSSEL2Order){};

	/// ���L2��ָ֤����������ʵʱ�ص��ӿ�
	/// @param  RealSZSEL2Cnindex -- ʵʱ����
	virtual void OnSubscribe_SZSEL2_Cnindex(const SZSEL2_Cnindex& RealSZSEL2Cnindex){};

	/// �Ͻ���L2ծȯ��̬���ݶ�������ʵʱ�ص��ӿ�
	/// @param  RealSSEL2BondStatic -- ʵʱ����
	virtual void OnSubscribe_SSEL2_BondStatic(const SSEL2_BondStatic& RealSSEL2BondStatic){};

	/// �����Ͻ���L2ծȯʵʱ��������ʵʱ�ص��ӿ�
	/// @param  RealSSEL2BondQuotation -- ʵʱ����
	virtual void OnSubscribe_SSEL2_BondQuotation(const SSEL2_BondQuotation& RealSSEL2BondQuotation){};

	/// �����Ͻ���L2ծȯ�����������ʵʱ�ص��ӿ�
	/// @param  RealSSEL2BondTick -- ʵʱ����
	virtual void OnSubscribe_SSEL2_BondTick(const SSEL2_BondTick& RealSSEL2BondTick){};

	/// �����Ͻ���L2ծȯ�г���������ʵʱ�ص��ӿ�
	/// @param  RealSSEL2BondOverview -- ʵʱ����
	virtual void OnSubscribe_SSEL2_BondOverview(const SSEL2_BondOverview& RealSSEL2BondOverview){};

	/// �Ͻ���L1ծȯ��̬���ݶ�������ʵʱ�ص��ӿ�
	/// @param  RealSSEL1BondStatic -- ʵʱ����
	virtual void OnSubscribe_SSEL1_BondStatic(const SSEL1_BondStatic& RealSSEL1BondStatic){};

	/// �����Ͻ���L1ծȯʵʱ��������ʵʱ�ص��ӿ�
	/// @param  RealSSEL1BondQuotation -- ʵʱ����
	virtual void OnSubscribe_SSEL1_BondQuotation(const SSEL1_BondQuotation& RealSSEL1BondQuotation){};


	/// �������L2ծȯ��̬����ʵʱ�ص��ӿ� 
	/// @param  RealSZSEL2BondStatic -- ʵʱ����
	virtual void OnSubscribe_SZSEL2_BondStatic(const SZSEL2_BondStatic& RealSZSEL2BondStatic){};

	/// �������L2ծȯʵʱ��������ʵʱ�ص��ӿ�
	/// @param  RealSZSEL2BondQuotation -- ʵʱ����
	virtual void OnSubscribe_SZSEL2_BondQuotation(const SZSEL2_BondQuotation& RealSZSEL2BondQuotation){};

	/// �������L2ծȯ��ʳɽ�����ʵʱ�ص��ӿ�
	/// @param  RealSZSEL2BondTransaction -- ʵʱ����
	virtual void OnSubscribe_SZSEL2_BondTransaction(const SZSEL2_BondTransaction& RealSZSEL2BondTransaction){};

	/// �������L2ծȯ���ί��ʵʱ��������ʵʱ�ص��ӿ�
	/// @param  RealSZSEL2BondOrder -- ʵʱ����
	virtual void OnSubscribe_SZSEL2_BondOrder(const SZSEL2_BondOrder& RealSZSEL2BondOrder){};

	/// �������L2ծȯ�������ʵʱ�ص��ӿ� 
	/// @param  RealSZSEL2_BondTick -- ʵʱ����
	virtual void OnSubscribe_SZSEL2_BondTick(const SZSEL2_BondTick& RealSZSEL2_BondTick){};

	/// �������L2ծȯ��������Ϣʵʱ�ص��ӿ�
	/// @param  RealSZSEL2BondMbrInfo -- ʵʱ����
	virtual void OnSubscribe_SZSEL2_BondMbrInfo(const SZSEL2_BondMbrInfo& RealSZSEL2BondMbrInfo){};

	/// �������L2ծȯ����������Ϣʵʱ�ص��ӿ�
	/// @param  RealSZSEL2BondInvestorInfo -- ʵʱ����
	virtual void OnSubscribe_SZSEL2_BondInvestorInfo(const SZSEL2_BondInvestorInfo& RealSZSEL2BondInvestorInfo){};

	/// �������L2ծȯ����Ա��Ϣʵʱ�ص��ӿ�
	/// @param  RealSZSEL2BondOrder -- ʵʱ����
	virtual void OnSubscribe_SZSEL2_BondTraderInfo(const SZSEL2_BondTraderInfo& RealSZSEL2BondTraderInfo){};

	/// �������L1ծȯ��̬����ʵʱ�ص��ӿ�
	/// @param  RealSZSEL1BondStatic -- ʵʱ����
	virtual void OnSubscribe_SZSEL1_BondStatic(const SZSEL1_BondStatic& RealSZSEL1BondStatic){};

	/// �������L1ծȯʵʱ����ʵʱ�ص��ӿ�
	/// @param  RealSZSEL1BondQuotation -- ʵʱ����
	virtual void OnSubscribe_SZSEL1_BondQuotation(const SZSEL1_BondQuotation& RealSZSEL1BondQuotation){};

	//������
	/// ������L1��̬���ݶ�������ʵʱ�ص��ӿ�
	/// @param  RealBSEL1Static    -- ʵʱ����
	virtual void OnSubscribe_BSEL1_Static(const BSEL1_Static& RealBSEL1Static){}

	/// ������L1ʵʱ���鶩������ʵʱ�ص��ӿ�
	/// @param  RealBSEL1Quotation -- ʵʱ����
	virtual void OnSubscribe_BSEL1_Quotation(const BSEL1_Quotation& RealBSEL1Quotation){}

	/// ���L2ծȯ�г�����ԤԼ��Ϣ��������ʵʱ�ص��ӿ�
	/// @param  RealSZSEL2BondBidBookingInfo -- ʵʱ����
	virtual void OnSubscribe_SZSEL2_BondBidBookingInfo(const SZSEL2_BondBidBookingInfo& RealSZSEL2BondBidBookingInfo){}

	/// ֣����L2��̬���ݶ�������ʵʱ�ص��ӿ�
	/// @param  RealCZCEL2Static -- ʵʱ����
	virtual void OnSubscribe_CZCEL2_Static(const CZCEL2_Static& RealCZCEL2Static){}

	/// ֣����L2�г�״̬��������ʵʱ�ص��ӿ�
	/// @param  RealCZCEL2MktStatus -- ʵʱ����
	virtual void OnSubscribe_CZCEL2_MktStatus(const CZCEL2_MktStatus& RealCZCEL2MktStatus){}

	/// ֣����L2Ʒ��״̬��������ʵʱ�ص��ӿ�
	/// @param  RealCZCEL2Status -- ʵʱ����
	virtual void OnSubscribe_CZCEL2_Status(const CZCEL2_Status& RealCZCEL2Status){}

	/// ֣����L2ʵʱ���鶩������ʵʱ�ص��ӿ�
	/// @param  RealCZCEL2Quotation -- ʵʱ����
	virtual void OnSubscribe_CZCEL2_Quotation(const CZCEL2_Quotation& RealCZCEL2Quotation){}

	/// ֣����L2�嵵���ί�����鶩������ʵʱ�ص��ӿ�
	/// @param  RealCZCEL2LevelQuotation -- ʵʱ����
	virtual void OnSubscribe_CZCEL2_LevelQuotation(const CZCEL2_LevelQuotation& RealCZCEL2LevelQuotation){}

    /// ֣����L2�������鶩�����ݻص��ӿ�
    /// @param  data     -- ʵʱ����
    virtual void OnSubscribe_CZCEL2_ArbiQuotation(const CZCEL2_ArbiQuotation& data){}

    /// ֣����L2�����嵵������鶩�����ݻص��ӿ�
    /// @param  data     -- ʵʱ����
    virtual void OnSubscribe_CZCEL2_ArbiLevelQuotation(const CZCEL2_ArbiLevelQuotation& data){}

	/// �Ͻ���L2����ʵʱ���鶩������ʵʱ�ص��ӿ�
	/// @param  RealSSEL2ShortQuotation -- ʵʱ����
	virtual void OnSubscribe_SSEL2_ShortQuotation(const SSEL2_ShortQuotation& RealSSEL2ShortQuotation){}

	/// �Ͻ���L2ί�ж��ж�������ʵʱ�ص��ӿ�
	/// @param  RealSSEL2OrderQueue -- ʵʱ����
	virtual void OnSubscribe_SSEL2_OrderQueue(const SSEL2_OrderQueue& RealSSEL2OrderQueue){}

	/// ���L2����ʵʱ���鶩������ʵʱ�ص��ӿ�
	/// @param  RealSZSEL2ShortQuotation -- ʵʱ����
	virtual void OnSubscribe_SZSEL2_ShortQuotation(const SZSEL2_ShortQuotation& RealSZSEL2ShortQuotation){}

	/// ���L2ί�ж��ж�������ʵʱ�ص��ӿ�
	/// @param  RealSZSEL2OrderQueue -- ʵʱ����
	virtual void OnSubscribe_SZSEL2_OrderQueue(const SZSEL2_OrderQueue& RealSZSEL2OrderQueue){}
	
	/// �Ͻ���L2 ETFͳ�����ݶ������ݻص��ӿ�
	/// @param  data     -- ʵʱ����
	virtual void OnSubscribe_SSEL2_ETF(const SSEL2_ETF& data){}

    /// ������L1��̬���ݶ������ݻص��ӿ�
    /// @param  data     -- ʵʱ����
    virtual void OnSubscribe_GFEXL1_Static(const GFEXL1_Static& data){}

    /// ������L1�������鶩�����ݻص��ӿ�
    /// @param  data     -- ʵʱ����
    virtual void OnSubscribe_GFEXL1_ArbiQuotation(const GFEXL1_ArbiQuotation& data){}

    /// ������L1ʵʱ���鶩�����ݻص��ӿ�
    /// @param  data     -- ʵʱ����
    virtual void OnSubscribe_GFEXL1_Quotation(const GFEXL1_Quotation& data){}

    /// ������L1��Ȩ�����������ݻص��ӿ�
    /// @param  data     -- ʵʱ����
    virtual void OnSubscribe_GFEXL1_OptionParameter(const GFEXL1_OptionParameter& data){}

    /// ������L2��̬���ݶ������ݻص��ӿ�
    /// @param  data     -- ʵʱ����
    virtual void OnSubscribe_GFEXL2_Static(const GFEXL2_Static& data){}

    /// ������L2�������鶩�����ݻص��ӿ�
    /// @param  data     -- ʵʱ����
    virtual void OnSubscribe_GFEXL2_ArbiQuotation(const GFEXL2_ArbiQuotation& data){}

    /// ������L2ʵʱ���鶩�����ݻص��ӿ�
    /// @param  data     -- ʵʱ����
    virtual void OnSubscribe_GFEXL2_Quotation(const GFEXL2_Quotation& data){}

    /// ������L2ί�ж������鶩�����ݻص��ӿ�
    /// @param  data     -- ʵʱ����
    virtual void OnSubscribe_GFEXL2_OrderQueue(const GFEXL2_OrderQueue& data){}

    /// ������L2ί��ͳ�����鶩�����ݻص��ӿ�
    /// @param  data     -- ʵʱ����
    virtual void OnSubscribe_GFEXL2_OrderStatistic(const GFEXL2_OrderStatistic& data){}

    /// ������L2ʵʱ����۶������ݻص��ӿ�
    /// @param  data     -- ʵʱ����
    virtual void OnSubscribe_GFEXL2_RealTimePrice(const GFEXL2_RealTimePrice& data){}

    /// ������L2�ּ۳ɽ������鶩�����ݻص��ӿ�
    /// @param  data     -- ʵʱ����
    virtual void OnSubscribe_GFEXL2_MarchPriceQty(const GFEXL2_MarchPriceQty& data){}

    /// ������L2��Ȩ�����������ݻص��ӿ�
    /// @param  data     -- ʵʱ����
    virtual void OnSubscribe_GFEXL2_OptionParameter(const GFEXL2_OptionParameter& data){}

	/// ������L2����ί�ж��ж������ݻص��ӿ�
	/// @param  data     -- ʵʱ����
	virtual void OnSubscribe_GFEXL2_ArbiOrderQueue(const GFEXL2_ArbiOrderQueue& data){}

    /// �Ͻ���L2�̺�̶��۸���ʳɽ��������ݻص��ӿ�
    /// @param  data     -- ʵʱ����
    virtual void OnSubscribe_SSEL2_FpTransaction(const SSEL2_FpTransaction& data){}

    /// �Ͻ���L2������鶩�����ݻص��ӿ�
    /// @param  data     -- ʵʱ����
    virtual void OnSubscribe_SSEL2_Tick(const SSEL2_Tick& data){}

    /// ������ָ������ �������ݻص��ӿ�
    /// @param  data     -- ʵʱ����
    virtual void OnSubscribe_DCE_Index(const DCE_Index& data){}

    /// �Ͻ���L1�̺�̶��۸����鶩�����ݻص��ӿ�
    /// @param  data     -- ʵʱ����
    virtual void OnSubscribe_SSEL1_FpQuotation(const SSEL1_FpQuotation& data){}

    /// �Ͻ���L2�̺�̶��۸����鶩�����ݻص��ӿ�
    /// @param  data     -- ʵʱ����
    virtual void OnSubscribe_SSEL2_FpQuotation(const SSEL2_FpQuotation& data){}

    /// ���L1�̺�̶��۸����鶩�����ݻص��ӿ�
    /// @param  data     -- ʵʱ����
    virtual void OnSubscribe_SZSEL1_FpQuotation(const SZSEL1_FpQuotation& data){}

	/// ���L2�̺�̶��۸����鶩�����ݻص��ӿ�
	/// @param  data     -- ʵʱ����
	virtual void OnSubscribe_SZSEL2_FpQuotation(const SZSEL2_FpQuotation& data){}
};


/// ����ϵͳ����ӿڣ����ӷ�����������֤���������ݣ���ѯ����.
class GTA_API_EXPORT IGTAQTSApiBase
{
public:
    virtual ~IGTAQTSApiBase(){}
public:
    /// ����һ��ʵ��.����ʵ��������ͨ�����ӿ�����.
    /// @param  CallBack    -- ���ӻص�������֪ͨ����״̬
    /// @return NULL        -- ����ʵ��ʧ��
    static IGTAQTSApiBase* CreateInstance(IGTAQTSCallbackBase& CallBack);

    /// �ͷ�һ��ʵ��,��ʵ����ͨ��CreateInstance���ɵ�.
    /// @param  pInstance   -- ʵ������ָ��.
    static void ReleaseInstance(IGTAQTSApiBase* pInstance);

    /// ���ó�ʱʱ�䣬���ü�ʱ��Ч��ͬ�����õĳ�ʱʱ��
    /// @param  nSenond     -- ��ʱʱ�䣬��λ��
    virtual void SetTimeout(int nSecond = TIMEOUT_DEFAULT) = 0;

    /// �����������ʱ�������ü�ʱ��Ч��
    /// @param  nSenond     -- �������ʱ������λ��
    virtual void SetHeartBeatTime(int nSecond = HEART_BEAT_INTERVAL) = 0;

    /// ��ʼAPI�����������ӳٲ���
    /// @param  bSwitch     -- ����,true ����  false  �ر�
    /// @return RetCode     -- �������, Ret_Success:�ɹ�;Ret_Error:ʧ��(δ��������)
    virtual RetCode DelayTest(bool bSwitch) = 0;

    /// ע�������������ַ����ע������ͬ��ַ��ʵ�ָ߿��ã�����˳��Ϊ˳�����ӣ�����ʱ��������ǰʹ�õ�ַ
    /// @param  pIP         -- ��������ַ.
    /// @param  uPort       -- �������˿�.
    /// @return RetCode     -- �������
    virtual RetCode RegisterService(const char* pIP, unsigned short uPort) = 0;

	/// �������L2 Tick���ݶ��̻߳ص�
	/// @param  bMTCallBack     -- ����,true ����  false  �ر�
	virtual void SetSZL2TickMTCallBack(bool bMTCallBack) = 0;

    /// ��������������������û���֤��Ϣ��ͬ������
    /// @param  pUserName   -- �˺�����
    /// @param  pPassword   -- �˺����룬���ܺ�����
    /// @return RetCode     -- �������
    virtual RetCode Login(const char* pUserName, const char* pPassword) = 0;
    
    /// ��������������������û���֤��Ϣ��ͬ������
    /// @param  pUserName   -- �˺�����
    /// @param  pPassword   -- �˺����룬���ܺ�����
    /// @param  pOptions    -- ��ѡ���Ӳ����������null�����ж������ʱ���ö��ţ������ָ�
    ///                        ��ʶ        ��ѡֵ       ˵��
    ///                        NetType     0��1         �����������ͣ�0��ʾ����(Ĭ��ֵ)��1��ʾ��������
    ///                        ʾ�����磺"NetType=1"
    /// @return RetCode     -- �������
    virtual RetCode LoginX(const char* pUserName, const char* pPassword, const char* pOptions = NULL) = 0;

    /// ��ȡ���鶩�ķ�������ַ��Ϣ
    /// @param  ServerStates    -- ��ŷ�������ַ����
    /// @return RetCode         -- �������
    virtual RetCode GetConnectState(CDataBuffer<ServerState>& ServerStates) = 0;

    /// ��ȡ��Ȩ�޵���������.
    /// @param  DataTypeList-- ��������ָ�룬���ݴ�С��Ϊ��Ȩ�޵����ݸ���
    /// @return RetCode     -- �������
    virtual RetCode GetDataTypeList(CDataBuffer<MsgType>& DataTypeList) = 0;

    /// �������飬�ظ�����ͬһ�����ݶ��ʱȡ����
    /// @param  msgType     -- ��Ҫ���ĵ���������
    /// @param  pCodeList   -- ���ɶ����б��ԡ�,���ָĩβ������\0������ΪNULLʱ����ȫ�г�
    /// @return RetCode     -- �������
    virtual RetCode Subscribe(MsgType msgType, char* pCodeList = NULL) = 0;

    /// ȡ������
    /// @param  msgType     -- ��Ҫȡ�����ĵ���������
    /// @param  pCodeList   -- ȡ�������б��ԡ�,���ָĩβ������\0������ΪNULLʱȡ�����д���
    /// @return RetCode     -- �������
    virtual RetCode Unsubscribe(MsgType msgType, char* pCodeList = NULL) = 0;

    /// ȡ�����ж�����Ϣ
    /// @return RetCode     -- �������
    virtual RetCode UnsubscribeAll() = 0;

    /// ��ȡ��Ʊ�б�
    /// @param  pMarketTag  -- �г���ʶ���ԡ�,���ָ����г���ʶ��ĩβ������\0������������ΪNULL
    /// @param  StockList   -- ���صĸ��ɴ����б����г���־��
    /// @return RetCode     -- �������
    virtual RetCode GetStockList(char* pMarketTag, CDataBuffer<StockSymbol>& StockList) = 0;

    /// ��ѯ�Ͻ���L1��̬����
    /// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  SnapSSEL1Static     -- ���������б�
    /// @return RetCode             -- �������
    virtual RetCode QuerySnap_SSEL1_Static(char* pCodeList, CDataBuffer<SSEL1_Static>& SnapSSEL1Static) = 0;

    /// ��ѯ�Ͻ���L1ʵʱ����
    /// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  SnapSSEQuotation     -- ���������б�
    /// @return RetCode             -- �������
    virtual RetCode QuerySnap_SSEL1_Quotation(char* pCodeList, CDataBuffer<SSEL1_Quotation>& SnapSSEQuotation) = 0;

    /// ��ѯ�Ͻ���ָ��ͨ����
    /// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  SnapSSEIndexPress   -- ���������б�
    /// @return RetCode             -- �������
    virtual RetCode QuerySnap_SSE_IndexPress(char* pCodeList, CDataBuffer<SSE_IndexPress>& SnapSSEIndexPress) = 0;

    /// ��ѯ�Ͻ���L2��̬����
    /// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  SnapSSEStatic        -- ���������б�
    /// @return RetCode             -- �������
    virtual RetCode QuerySnap_SSEL2_Static(char* pCodeList, CDataBuffer<SSEL2_Static>& SnapSSEL2Static) = 0;

    /// ��ѯ�Ͻ���L2ʵʱ�������
    /// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  SnapSSEL2Quotation    -- ���������б�
    /// @return RetCode             -- �������
    virtual RetCode QuerySnap_SSEL2_Quotation(char* pCodeList, CDataBuffer<SSEL2_Quotation>& SnapSSEL2Quotation) = 0;

    /// ��ѯ�Ͻ���L2ָ������
    /// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  SnapSSEL2Index        -- ���������б�
    /// @return RetCode             -- �������
    virtual RetCode QuerySnap_SSEL2_Index(char* pCodeList, CDataBuffer<SSEL2_Index>& SnapSSEL2Index) = 0;

    /// ��ѯ�Ͻ���L2��ʳɽ�
    /// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  SnapSSEL2Transation   -- ���������б�
    /// @return RetCode             -- �������
    virtual RetCode QuerySnap_SSEL2_Transaction(char* pCodeList, CDataBuffer<SSEL2_Transaction>& SnapSSEL2Transation) = 0;

    /// ��ѯ�Ͻ���L2���⼯�Ͼ���
    /// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  SnapSSEL2Auction    -- ���������б�
    /// @return RetCode             -- �������
    virtual RetCode QuerySnap_SSEL2_Auction(char* pCodeList, CDataBuffer<SSEL2_Auction>& SnapSSEL2Auction) = 0;

    /// ��ѯ�Ͻ���L2�г�����
    /// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  SnapSSEL2Overview   -- ���������б�
    /// @return RetCode             -- �������
    virtual RetCode QuerySnap_SSEL2_Overview(char* pCodeList, CDataBuffer<SSEL2_Overview>& SnapSSEL2Overview) = 0;

    /// ��ѯ�Ͻ���������Ȩ��̬����
    /// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  SnapSSEIOL1Static   -- ���������б�
    /// @return RetCode             -- �������
    virtual RetCode QuerySnap_SSEIOL1_Static(char* pCodeList, CDataBuffer<SSEIOL1_Static>& SnapSSEIOL1Static) = 0;

    /// ��ѯ�Ͻ���������Ȩʵʱ����
    /// @param  pCodeList               -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  SnapSSEIOL1Quotation    -- ���������б�
    /// @return RetCode                 -- �������
    virtual RetCode QuerySnap_SSEIOL1_Quotation(char* pCodeList, CDataBuffer<SSEIOL1_Quotation>& SnapSSEIOL1Quotation) = 0;

	/// ��ѯ�Ͻ���������ȨL1��ϲ���
	/// @param  pCodeList                -- ���ն����б�NULLʱΪȫ�г���������
	/// @param  SnapSSEIOL1Strategy      -- ���������б�
	/// @return RetCode                  -- �������
	virtual RetCode QuerySnap_SSEIOL1_Strategy(char* pCodeList, CDataBuffer<SSEIOL1_Strategy>& SnapSSEIOL1Strategy) = 0;

    /// ��ѯ���L1��̬����
    /// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  SnapSZSEL1Static    -- ���������б�
    /// @return RetCode             -- �������
    virtual RetCode QuerySnap_SZSEL1_Static(char* pCodeList, CDataBuffer<SZSEL1_Static>& SnapSZSEL1Static) = 0;

    /// ��ѯ���L1ʵʱ����
    /// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  SnapSZSEL1Quotation -- ���������б�
    /// @return RetCode             -- �������
    virtual RetCode QuerySnap_SZSEL1_Quotation(char* pCodeList, CDataBuffer<SZSEL1_Quotation>& SnapSZSEL1Quotation) = 0;

    /// ��ѯ���L1����
    /// @param  pCodeList           -- ���ն����б�ֻ��ΪNULL�����뽫������
    /// @param  SnapSZSEL1Bullet    -- ���������б�
    /// @return RetCode             -- �������
    virtual RetCode QuerySnap_SZSEL1_Bulletin(char* pCodeList, CDataBuffer<SZSEL1_Bulletin>& SnapSZSEL1Bullet) = 0;

    /// ��ѯ���������ȨL1��̬����
    /// @param  pCodeList             -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  SnapSZSEIOL1Static    -- ���������б�
    /// @return RetCode               -- �������
    virtual RetCode QuerySnap_SZSEIOL1_Static(char* pCodeList, CDataBuffer<SZSEIOL1_Static>& SnapSZSEIOL1Static) = 0;

    /// ��ѯ���������ȨL1ʵʱ����
    /// @param  pCodeList                 -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  SnapSZSEIOL1Quotation    -- ���������б�
    /// @return RetCode                   -- �������
    virtual RetCode QuerySnap_SZSEIOL1_Quotation(char* pCodeList, CDataBuffer<SZSEIOL1_Quotation>& SnapSZSEIOL1Quotation) = 0;

    /// ��ѯ���������ȨL1֤ȯ״̬
    /// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  SnapSZSEIOL1Status    -- ���������б�
    /// @return RetCode             -- �������
    virtual RetCode QuerySnap_SZSEIOL1_Status(char* pCodeList, CDataBuffer<SZSEIOL1_Status>& SnapSZSEIOL1Status) = 0;

    /// ��ѯ���������ȨL1��ϲ���
    /// @param  pCodeList                -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  SnapSZSEIOL1Strategy    -- ���������б�
    /// @return RetCode                  -- �������
    virtual RetCode QuerySnap_SZSEIOL1_Strategy(char* pCodeList, CDataBuffer<SZSEIOL1_Strategy>& SnapSZSEIOL1Strategy) = 0;

    /// ��ѯ���L2��̬����
    /// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  SnapSZSEL2Static    -- ���������б�
    /// @return RetCode             -- �������
    virtual RetCode QuerySnap_SZSEL2_Static(char* pCodeList, CDataBuffer<SZSEL2_Static>& SnapSZSEL2Static) = 0;

    /// ��ѯ���L2ʵʱ����
    /// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  SnapSZSEL2Quotation    -- ���������б�
    /// @return RetCode             -- �������
    virtual RetCode QuerySnap_SZSEL2_Quotation(char* pCodeList, CDataBuffer<SZSEL2_Quotation>& SnapSZSEL2Quotation) = 0;

    /// ��ѯ���L2֤ȯ״̬
    /// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  SnapSZSEL2StockStatus  -- ���������б�
    /// @return RetCode             -- �������
    virtual RetCode QuerySnap_SZSEL2_Status(char* pCodeList, CDataBuffer<SZSEL2_Status>& SnapSZSEL2StockStatus) = 0;

    /// ��ѯ���L2ָ������
    /// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  SnapSZSEL2Index        -- ���������б�
    /// @return RetCode             -- �������
    virtual RetCode QuerySnap_SZSEL2_Index(char* pCodeList, CDataBuffer<SZSEL2_Index>& SnapSZSEL2Index) = 0;

    /// ��ѯ���L2���ί��
    /// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  SnapSZSEL2Order        -- ���������б�
    /// @return RetCode             -- �������
    virtual RetCode QuerySnap_SZSEL2_Order(char* pCodeList, CDataBuffer<SZSEL2_Order>& SnapSZSEL2Order) = 0;

    /// ��ѯ���L2��ʳɽ�
    /// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  SnapSZSEL2Transaction  -- ���������б�
    /// @return RetCode             -- �������
    virtual RetCode QuerySnap_SZSEL2_Transaction(char* pCodeList, CDataBuffer<SZSEL2_Transaction>& SnapSZSEL2Transaction) = 0;

	/// ��ѯ���L2���
	/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
	/// @param  SnapSZSEL2Tick      -- ���������б�
	/// @return RetCode             -- �������
	virtual RetCode QuerySnap_SZSEL2_Tick(char* pCodeList, CDataBuffer<SZSEL2_Tick>& SnapSZSEL2Tick) = 0;

    /// ��ѯ�н�����̬����
    /// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  SnapCFFEXL2Static       -- ���������б�
    /// @return RetCode             -- �������
    virtual RetCode QuerySnap_CFFEXL2_Static(char* pCodeList, CDataBuffer<CFFEXL2_Static>& SnapCFFEXL2Static) = 0;

    /// ��ѯ�н���L2ʵʱ����
    /// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  SnapCFFEXL2Quotation    -- ���������б�
    /// @return RetCode             -- �������
    virtual RetCode QuerySnap_CFFEXL2_Quotation(char* pCodeList, CDataBuffer<CFFEXL2_Quotation>& SnapCFFEXL2Quotation) = 0;

    /// ��ѯ֣������̬����
    /// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  SnapCZCEL1Static    -- ���������б�
    /// @return RetCode             -- �������
    virtual RetCode QuerySnap_CZCEL1_Static(char* pCodeList, CDataBuffer<CZCEL1_Static>& SnapCZCEL1Static) = 0;

    /// ��ѯ֣����ʵʱ����
    /// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  SnapCZCEL1Quotation -- ���������б�
    /// @return RetCode             -- �������
    virtual RetCode QuerySnap_CZCEL1_Quotation(char* pCodeList, CDataBuffer<CZCEL1_Quotation>& SnapCZCEL1Quotation) = 0;

	/// ��ѯ֣������������
	/// @param  pCodeList                -- ���ն����б�NULLʱΪȫ�г���������
	/// @param  SnapCZCEL1ArrbiQuotation -- ���������б�
	/// @return RetCode                  -- �������
	virtual RetCode QuerySnap_CZCEL1_ArbiQuotation(char* pCodeList, CDataBuffer<CZCEL1_ArbiQuotation>& SnapCZCEL1ArrbiQuotation) = 0;

	/// ��ѯ֣����L1�г�״̬
	/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
	/// @param  SnapCZCEL1MktStatus -- ���������б�
	/// @return RetCode             -- �������
	virtual RetCode QuerySnap_CZCEL1_MktStatus(char* pCodeList, CDataBuffer<CZCEL1_MktStatus>& SnapCZCEL1MktStatus) = 0;

	/// ��ѯ֣����L1Ʒ��״̬
	/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
	/// @param  SnapCZCEL1Status    -- ���������б�
	/// @return RetCode             -- �������
	virtual RetCode QuerySnap_CZCEL1_Status(char* pCodeList, CDataBuffer<CZCEL1_Status>& SnapCZCEL1Status) = 0;

    /// ��ѯ��ʢָ������
    /// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  SnapESUNNY_Index        -- ���������б�
    /// @return RetCode             -- �������
    virtual RetCode QuerySnap_ESUNNY_Index(char* pCodeList, CDataBuffer<ESUNNY_Index>& SnapESUNNY_Index) = 0;

    /// ��ѯ������L1��̬����
    /// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  SnapDCEL1Static     -- ���������б�
    /// @return RetCode             -- �������
    virtual RetCode QuerySnap_DCEL1_Static(char* pCodeList, CDataBuffer<DCEL1_Static>& SnapDCEL1Static) = 0;

	/// ��ѯ������L1�г�״̬
	/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
	/// @param  SnapDCEL1MktStatus     -- ���������б�
	/// @return RetCode             -- �������
	virtual RetCode QuerySnap_DCEL1_MktStatus(char* pCodeList, CDataBuffer<DCEL1_MktStatus>& SnapDCEL1MktStatus) = 0;

	/// ��ѯ������L1����״̬
	/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
	/// @param  SnapDCEL1Status     -- ���������б�
	/// @return RetCode             -- �������
	virtual RetCode QuerySnap_DCEL1_Status(char* pCodeList, CDataBuffer<DCEL1_Status>& SnapDCEL1Status) = 0;

	/// ��ѯ������L1��Ȩ����
	/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
	/// @param  SnapDCEL1OptionParameter     -- ���������б�
	/// @return RetCode             -- �������
	virtual RetCode QuerySnap_DCEL1_OptionParameter(char* pCodeList, CDataBuffer<DCEL1_OptionParameter>& SnapDCEL1OptionParameter) = 0;

    /// ��ѯ������L1ʵʱ����
    /// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  SnapDCEL1Quotation  -- ���������б�
    /// @return RetCode             -- �������
    virtual RetCode QuerySnap_DCEL1_Quotation(char* pCodeList, CDataBuffer<DCEL1_Quotation>& SnapDCEL1Quotation) = 0;

    /// ��ѯ������L1��������
    /// @param  pCodeList               -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  SnapDCEL1ArbiQuotation  -- ���������б�
    /// @return RetCode                 -- �������
    virtual RetCode QuerySnap_DCEL1_ArbiQuotation(char* pCodeList, CDataBuffer<DCEL1_ArbiQuotation>& SnapDCEL1ArbiQuotation) = 0;

    /// ��ѯ������L2��̬����
    /// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  SnapDCEL2Static     -- ���������б�
    /// @return RetCode             -- �������
    virtual RetCode QuerySnap_DCEL2_Static(char* pCodeList, CDataBuffer<DCEL2_Static>& SnapDCEL2Static) = 0;

    /// ��ѯ������L2ʵʱ����
    /// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  SnapDCEL2Quotation  -- ���������б�
    /// @return RetCode             -- �������
    virtual RetCode QuerySnap_DCEL2_Quotation(char* pCodeList, CDataBuffer<DCEL2_Quotation>& SnapDCEL2Quotation) = 0;

    /// ��ѯ������L2��������
    /// @param  pCodeList               -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  SnapDCEL2ArbiQuotation  -- ���������б�
    /// @return RetCode                 -- �������
    virtual RetCode QuerySnap_DCEL2_ArbiQuotation(char* pCodeList, CDataBuffer<DCEL2_ArbiQuotation>& SnapDCEL2ArbiQuotation) = 0;

    /// ��ѯ������L2ʵʱ�����
    /// @param  pCodeList               -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  SnapDCEL2RealTimePrice  -- ���������б�
    /// @return RetCode                 -- �������
    virtual RetCode QuerySnap_DCEL2_RealTimePrice(char* pCodeList, CDataBuffer<DCEL2_RealTimePrice>& SnapDCEL2RealTimePrice) = 0;

    /// ��ѯ������L2ί��ͳ������
    /// @param  pCodeList               -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  SnapDCEL2OrderStatistic -- ���������б�
    /// @return RetCode                 -- �������
    virtual RetCode QuerySnap_DCEL2_OrderStatistic(char* pCodeList, CDataBuffer<DCEL2_OrderStatistic>& SnapDCEL2OrderStatistic) = 0;

    /// ��ѯ������L2�ּ۳ɽ�������
    /// @param  pCodeList               -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  SnapDCEL2MarchPriceQty  -- ���������б�
    /// @return RetCode                 -- �������
    virtual RetCode QuerySnap_DCEL2_MarchPriceQty(char* pCodeList, CDataBuffer<DCEL2_MarchPriceQty>& SnapDCEL2MarchPriceQty) = 0;

	/// ��ѯ������L2�г�״̬
	/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
	/// @param  SnapDCEL2MktStatus     -- ���������б�
	/// @return RetCode             -- �������
	virtual RetCode QuerySnap_DCEL2_MktStatus(char* pCodeList, CDataBuffer<DCEL2_MktStatus>& SnapDCEL2MktStatus) = 0;

	/// ��ѯ������L2����״̬
	/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
	/// @param  SnapDCEL2Status     -- ���������б�
	/// @return RetCode             -- �������
	virtual RetCode QuerySnap_DCEL2_Status(char* pCodeList, CDataBuffer<DCEL2_Status>& SnapDCEL2Status) = 0;

	/// ��ѯ������L2��Ȩ����
	/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
	/// @param  SnapDCEL2OptionParameter     -- ���������б�
	/// @return RetCode             -- �������
	virtual RetCode QuerySnap_DCEL2_OptionParameter(char* pCodeList, CDataBuffer<DCEL2_OptionParameter>& SnapDCEL2OptionParameter) = 0;

	/// ��ѯ������L2�嵵�������
	/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
	/// @param  SnapDCEL2LevelQuotation     -- ���������б�
	/// @return RetCode             -- �������
	virtual RetCode QuerySnap_DCEL2_LevelQuotation(char* pCodeList, CDataBuffer<DCEL2_LevelQuotation>& SnapDCEL2LevelQuotation) = 0;

	/// ��ѯ������L2ί�ж�������
	/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
	/// @param  SnapDCEL2OrderQueue     -- ���������б�
	/// @return RetCode             -- �������
	virtual RetCode QuerySnap_DCEL2_OrderQueue(char* pCodeList, CDataBuffer<DCEL2_OrderQueue>& SnapDCEL2OrderQueue) = 0;

    /// ��ѯ��������̬����
    /// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  SnapSHFEL1Static    -- ���������б�
    /// @return RetCode             -- �������
    virtual RetCode QuerySnap_SHFEL1_Static(char* pCodeList, CDataBuffer<SHFEL1_Static>& SnapSHFEL1Static) = 0;

    /// ��ѯ������ʵʱ����
    /// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  SnapSHFEL1Quotation -- ���������б�
    /// @return RetCode             -- �������
    virtual RetCode QuerySnap_SHFEL1_Quotation(char* pCodeList, CDataBuffer<SHFEL1_Quotation>& SnapSHFEL1Quotation) = 0;

    /// ��ѯ������Դ��̬����
    /// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  SnapINEL1Static   -- ���������б�
    /// @return RetCode             -- �������
    virtual RetCode QuerySnap_INEL1_Static(char* pCodeList, CDataBuffer<INEL1_Static>& SnapINEL1Static) = 0;

    /// ��ѯ������Դʵʱ����
    /// @param  pCodeList            -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  SnapINEL1Quotation -- ���������б�
    /// @return RetCode              -- �������
    virtual RetCode QuerySnap_INEL1_Quotation(char* pCodeList, CDataBuffer<INEL1_Quotation>& SnapINEL1Quotation) = 0;

    //���ڶ�����ѯ����
    /// ��ѯ������L2��̬����
    /// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  SnapSHFEL2Static    -- ���������б�
    /// @return RetCode             -- �������
    virtual RetCode QuerySnap_SHFEL2_Static(char* pCodeList, CDataBuffer<SHFEL2_Static>& SnapSHFEL2Static) = 0;

    /// ��ѯ������L2ʵʱ����
    /// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  SnapSHFEL2Quotation -- ���������б�
    /// @return RetCode             -- �������
    virtual RetCode QuerySnap_SHFEL2_Quotation(char* pCodeList, CDataBuffer<SHFEL2_Quotation>& SnapSHFEL2Quotation) = 0;

    /// ��ѯ������ԴL2��̬����
    /// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  SnapINEL2Static   --   ���������б�
    /// @return RetCode             -- �������
    virtual RetCode QuerySnap_INEL2_Static(char* pCodeList, CDataBuffer<INEL2_Static>& SnapINEL2Static) = 0;

    /// ��ѯ������ԴL2ʵʱ����
    /// @param  pCodeList            -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  SnapINEL2Quotation   -- ���������б�
    /// @return RetCode              -- �������
    virtual RetCode QuerySnap_INEL2_Quotation(char* pCodeList, CDataBuffer<INEL2_Quotation>& SnapINEL2Quotation) = 0;

    /// ��ѯ�۽�����ʵʱ����
    /// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  SnapHKEXL2Quotation   -- ���������б�
    /// @return RetCode             -- �������
    virtual RetCode QuerySnap_HKEXL2_Quotation(char* pCodeList, CDataBuffer<HKEXL2_Quotation>& SnapHKEXL2Quotation) = 0;

    /// ��ѯ�۽�������̬����
    /// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  SnapHKEXL2Static      -- ���������б�
    /// @return RetCode             -- �������
    virtual RetCode QuerySnap_HKEXL2_Static(char* pCodeList, CDataBuffer<HKEXL2_Static>& SnapHKEXL2Static) = 0;

    /// ��ѯ�۽����������˶���
    /// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  SnapHKEXL2BrokerQueue   -- ���������б�
    /// @return RetCode             -- �������
    virtual RetCode QuerySnap_HKEXL2_BrokerQueue(char* pCodeList, CDataBuffer<HKEXL2_BrokerQueue>& SnapHKEXL2BrokerQueue) = 0;

    /// ��ѯ�۽�����ָ������
    /// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  SnapHKEXL2Index      -- ���������б�
    /// @return RetCode             -- �������
    virtual RetCode QuerySnap_HKEXL2_Index(char* pCodeList, CDataBuffer<HKEXL2_Index>& SnapHKEXL2Index) = 0;

    /// ��ѯ�۽������г�����
    /// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  SnapHKEXL2Overview   -- ���������б�
    /// @return RetCode             -- �������
    virtual RetCode QuerySnap_HKEXL2_Overview(char* pCodeList, CDataBuffer<HKEXL2_Overview>& SnapHKEXL2Overview) = 0;

	/// ��ѯ�۽���L2�����г������
	/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
	/// @param  SnapHKEXL2DQB       -- ���������б�
	/// @return RetCode             -- �������
	virtual RetCode QuerySnap_HKEXL2_DQB(char* pCodeList, CDataBuffer<HKEXL2_DQB>& SnapHKEXL2DQB) = 0;

	/// ��ѯ�۽���L2�ʽ�����
	/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
	/// @param  SnapHKEXL2MoneyFlow -- ���������б�
	/// @return RetCode             -- �������
	virtual RetCode QuerySnap_HKEXL2_MoneyFlow(char* pCodeList, CDataBuffer<HKEXL2_MoneyFlow>& SnapHKEXL2MoneyFlow) = 0;

    /// ��ѯ���ͨ��̬����
    /// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  SnapHKSZL1Static   -- ���������б�
    /// @return RetCode             -- �������
    virtual RetCode QuerySnap_HKSZL1_Static(char* pCodeList, CDataBuffer<HKSZL1_Static>& SnapHKSZL1Static) = 0;

    /// ��ѯ���ͨʵʱ��������
    /// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  SnapHKSZL1Quotation   -- ���������б�
    /// @return RetCode             -- �������
    virtual RetCode QuerySnap_HKSZL1_Quotation(char* pCodeList, CDataBuffer<HKSZL1_Quotation>& SnapHKSZL1Quotation) = 0;

    /// ��ѯ����ͨ��̬����
    /// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  SnapHKSHL1Static   -- ���������б�
    /// @return RetCode             -- �������
    virtual RetCode QuerySnap_HKSHL1_Static(char* pCodeList, CDataBuffer<HKSHL1_Static>& SnapHKSHL1Static) = 0;

    /// ��ѯ����ͨʵʱ��������
    /// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  SnapHKSHL1Quotation   -- ���������б�
    /// @return RetCode             -- �������
    virtual RetCode QuerySnap_HKSHL1_Quotation(char* pCodeList, CDataBuffer<HKSHL1_Quotation>& SnapHKSHL1Quotation) = 0;

	/// ��ѯ����ͨL1�г���������
	/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
	/// @param  SnapHKSHL1Overview   -- ���������б�
	/// @return RetCode             -- �������
	virtual RetCode QuerySnap_HKSHL1_Overview(char* pCodeList, CDataBuffer<HKSHL1_Overview>& SnapHKSHL1Overview) = 0;

	/// ��ѯ���ͨL1�г���������
	/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
	/// @param  SnapHKSZL1Overview  -- ���������б�
	/// @return RetCode             -- �������
	virtual RetCode QuerySnap_HKSZL1_Overview(char* pCodeList, CDataBuffer<HKSZL1_Overview>& SnapHKSZL1Overview) = 0;

	/// ��ѯ�Ͻ���L2���ί��
	/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
	/// @param  SnapSSEL2Order      -- ���������б�
	/// @return RetCode             -- �������
	virtual RetCode QuerySnap_SSEL2_Order(char* pCodeList, CDataBuffer<SSEL2_Order>& SnapSSEL2Order) = 0;

	/// ��ѯ���L2��ָ֤��
	/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
	/// @param  SnapSZSEL2Cnindex   -- ���������б�
	/// @return RetCode             -- �������
	virtual RetCode QuerySnap_SZSEL2_Cnindex(char* pCodeList, CDataBuffer<SZSEL2_Cnindex>& SnapSZSEL2Cnindex) = 0;

	/// ��ѯ�Ͻ���L2ծȯ��̬����
	/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
	/// @param  SnapSSEL2BondStatic -- ���������б�
	/// @return RetCode             -- �������
	virtual RetCode QuerySnap_SSEL2_BondStatic(char* pCodeList, CDataBuffer<SSEL2_BondStatic>& SnapSSEL2BondStatic) = 0;

	/// ��ѯ�Ͻ���L2ծȯʵʱ����
	/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
	/// @param  SnapSSEL2BondQuotation   -- ���������б�
	/// @return RetCode             -- �������
	virtual RetCode QuerySnap_SSEL2_BondQuotation(char* pCodeList, CDataBuffer<SSEL2_BondQuotation>& SnapSSEL2BondQuotation) = 0;

	/// ��ѯ�Ͻ���L2ծȯ�������
	/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
	/// @param  SnapSSEL2BondTick   -- ���������б�
	/// @return RetCode             -- �������
	virtual RetCode QuerySnap_SSEL2_BondTick(char* pCodeList, CDataBuffer<SSEL2_BondTick>& SnapSSEL2BondTick) = 0;

	/// ��ѯ�Ͻ���L2ծȯ�г�����
	/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
	/// @param  SnapSSEL2BondOverview   -- ���������б�
	/// @return RetCode             -- �������
	virtual RetCode QuerySnap_SSEL2_BondOverview(char* pCodeList, CDataBuffer<SSEL2_BondOverview>& SnapSSEL2BondOverview) = 0;

	/// ��ѯ�Ͻ���L1ծȯ��̬����
	/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
	/// @param  SnapSSEL1BondStatic -- ���������б�
	/// @return RetCode             -- �������
	virtual RetCode QuerySnap_SSEL1_BondStatic(char* pCodeList, CDataBuffer<SSEL1_BondStatic>& SnapSSEL1BondStatic) = 0;

	/// ��ѯ�Ͻ���L1ծȯʵʱ����
	/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
	/// @param  SnapSSEL1BondQuotation   -- ���������б�
	/// @return RetCode             -- �������
	virtual RetCode QuerySnap_SSEL1_BondQuotation(char* pCodeList, CDataBuffer<SSEL1_BondQuotation>& SnapSSEL1BondQuotation) = 0;

	/// ��ѯ���L2ծȯ��̬����
	/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
	/// @param  SnapSZSEL2BondStatic -- ���������б�
	/// @return RetCode             -- �������
	virtual RetCode QuerySnap_SZSEL2_BondStatic(char* pCodeList, CDataBuffer<SZSEL2_BondStatic>& SnapSZSEL2BondStatic) = 0;

	/// ��ѯ���L2ծȯʵʱ����
	/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
	/// @param  SnapSZSEL2BondQuotation -- ���������б�
	/// @return RetCode             -- �������
	virtual RetCode QuerySnap_SZSEL2_BondQuotation(char* pCodeList, CDataBuffer<SZSEL2_BondQuotation>& SnapSZSEL2BondQuotation) = 0;

	/// ��ѯ���L2ծȯ��ʳɽ�
	/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
	/// @param  SnapSZSEL2BondTransaction -- ���������б�
	/// @return RetCode             -- �������
	virtual RetCode QuerySnap_SZSEL2_BondTransaction(char* pCodeList, CDataBuffer<SZSEL2_BondTransaction>& SnapSZSEL2BondTransaction) = 0;

	/// ��ѯ���L2ծȯ���ί��
	/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
	/// @param  SnapSZSEL2BondOrder -- ���������б�
	/// @return RetCode             -- �������
	virtual RetCode QuerySnap_SZSEL2_BondOrder(char* pCodeList, CDataBuffer<SZSEL2_BondOrder>& SnapSZSEL2BondOrder) = 0;

	/// ��ѯ���L2ծȯ�����Ϣ
	/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
	/// @param  SnapSZSEL2BondTick  -- ���������б�
	/// @return RetCode             -- �������
	virtual RetCode QuerySnap_SZSEL2_BondTick(char* pCodeList, CDataBuffer<SZSEL2_BondTick>& SnapSZSEL2BondTick) = 0;

	/// ��ѯ���L2ծȯ��������Ϣ
	/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
	/// @param  SnapSZSEL2BondMbrInfo  -- ���������б�
	/// @return RetCode             -- �������
	virtual RetCode QuerySnap_SZSEL2_BondMbrInfo(char* pCodeList, CDataBuffer<SZSEL2_BondMbrInfo>& SnapSZSEL2BondMbrInfo) = 0;

	/// ��ѯ���L2ծȯ����������Ϣ
	/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
	/// @param  SnapSZSEL2BondInvestorInfo  -- ���������б�
	/// @return RetCode             -- �������
	virtual RetCode QuerySnap_SZSEL2_BondInvestorInfo(char* pCodeList, CDataBuffer<SZSEL2_BondInvestorInfo>& SnapSZSEL2BondInvestorInfo) = 0;

	/// ��ѯ���L2ծȯ����Ա��Ϣ
	/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
	/// @param  SnapSZSEL2BondTraderInfo  -- ���������б�
	/// @return RetCode             -- �������
	virtual RetCode QuerySnap_SZSEL2_BondTraderInfo(char* pCodeList, CDataBuffer<SZSEL2_BondTraderInfo>& SnapSZSEL2BondTraderInfo) = 0;

	/// ��ѯ���L1ծȯ��̬����
	/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
	/// @param  SnapSZSEL1BondStatic  -- ���������б�
	/// @return RetCode             -- �������
	virtual RetCode QuerySnap_SZSEL1_BondStatic(char* pCodeList, CDataBuffer<SZSEL1_BondStatic>& SnapSZSEL1BondStatic) = 0;

	/// ��ѯ���L1ծȯʵʱ����
	/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
	/// @param  SnapSZSEL1BondQuotation  -- ���������б�
	/// @return RetCode             -- �������
	virtual RetCode QuerySnap_SZSEL1_BondQuotation(char* pCodeList, CDataBuffer<SZSEL1_BondQuotation>& SnapSZSEL1BondQuotation) = 0;

	/// ��ѯ������L1��̬����
	/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
	/// @param  SnapBSEL1Static     -- ���������б�
	/// @return RetCode             -- �������
	virtual RetCode QuerySnap_BSEL1_Static(char* pCodeList, CDataBuffer<BSEL1_Static>& SnapBSEL1Static) = 0;

	/// ��ѯ������L1ʵʱ����
	/// @param  pCodeList           -- ���ն����б�NULLʱΪȫ�г���������
	/// @param  SnapBSEL1Quotation  -- ���������б�
	/// @return RetCode             -- �������
	virtual RetCode QuerySnap_BSEL1_Quotation(char* pCodeList, CDataBuffer<BSEL1_Quotation>& SnapBSEL1Quotation) = 0;

	/// ��ѯ���L2ծȯ�г�����ԤԼ��Ϣ
	/// @param  pCodeList                     -- ���ն����б�NULLʱΪȫ�г���������
	/// @param  SnapSZSEL2BondBidBookingInfo  -- ���������б�
	/// @return RetCode                       -- �������
	virtual RetCode QuerySnap_SZSEL2_BondBidBookingInfo(char* pCodeList, CDataBuffer<SZSEL2_BondBidBookingInfo>& SnapSZSEL2BondBidBookingInfo) = 0;

	/// ��ѯ֣����L2��̬����
	/// @param  pCodeList                     -- ���ն����б�NULLʱΪȫ�г���������
	/// @param  SnapCZCEL2Static  -- ���������б�
	/// @return RetCode                       -- �������
	virtual RetCode QuerySnap_CZCEL2_Static(char* pCodeList, CDataBuffer<CZCEL2_Static>& SnapCZCEL2Static) = 0;

	/// ��ѯ֣����L2�г�״̬
	/// @param  pCodeList                     -- ���ն����б�NULLʱΪȫ�г���������
	/// @param  SnapCZCEL2MktStatus  -- ���������б�
	/// @return RetCode                       -- �������
	virtual RetCode QuerySnap_CZCEL2_MktStatus(char* pCodeList, CDataBuffer<CZCEL2_MktStatus>& SnapCZCEL2MktStatus) = 0;

	/// ��ѯ֣����L2Ʒ��״̬
	/// @param  pCodeList                     -- ���ն����б�NULLʱΪȫ�г���������
	/// @param  SnapCZCEL2Status  -- ���������б�
	/// @return RetCode                       -- �������
	virtual RetCode QuerySnap_CZCEL2_Status(char* pCodeList, CDataBuffer<CZCEL2_Status>& SnapCZCEL2Status) = 0;

	/// ��ѯ֣����L2ʵʱ����
	/// @param  pCodeList                     -- ���ն����б�NULLʱΪȫ�г���������
	/// @param  SnapCZCEL2Quotation  -- ���������б�
	/// @return RetCode                       -- �������
	virtual RetCode QuerySnap_CZCEL2_Quotation(char* pCodeList, CDataBuffer<CZCEL2_Quotation>& SnapCZCEL2Quotation) = 0;

	/// ��ѯ֣����L2�嵵���ί������
	/// @param  pCodeList                     -- ���ն����б�NULLʱΪȫ�г���������
	/// @param  SnapCZCEL2LevelQuotation  -- ���������б�
	/// @return RetCode                       -- �������
	virtual RetCode QuerySnap_CZCEL2_LevelQuotation(char* pCodeList, CDataBuffer<CZCEL2_LevelQuotation>& SnapCZCEL2LevelQuotation) = 0;

    /// ��ѯ֣����L2�����������
    /// @param  pCodeList               -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  data    -- ���������б�
    /// @return RetCode                 -- �������
    virtual RetCode QuerySnap_CZCEL2_ArbiQuotation(char* pCodeList, CDataBuffer<CZCEL2_ArbiQuotation>& data) = 0;
    /// ��ѯ֣����L2�����嵵����������
    /// @param  pCodeList               -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  data    -- ���������б�
    /// @return RetCode                 -- �������
    virtual RetCode QuerySnap_CZCEL2_ArbiLevelQuotation(char* pCodeList, CDataBuffer<CZCEL2_ArbiLevelQuotation>& data) = 0;

	/// ��ѯ�Ͻ���L2����ʵʱ����
	/// @param  pCodeList                     -- ���ն����б�NULLʱΪȫ�г���������
	/// @param  SnapSSEL2ShortQuotation       -- ���������б�
	/// @return RetCode                       -- �������
	virtual RetCode QuerySnap_SSEL2_ShortQuotation(char* pCodeList, CDataBuffer<SSEL2_ShortQuotation>& SnapSSEL2ShortQuotation) = 0;

	/// ��ѯ�Ͻ���L2ί�ж���
	/// @param  pCodeList                     -- ���ն����б�NULLʱΪȫ�г���������
	/// @param  SnapSSEL2OrderQueue           -- ���������б�
	/// @return RetCode                       -- �������
	virtual RetCode QuerySnap_SSEL2_OrderQueue(char* pCodeList, CDataBuffer<SSEL2_OrderQueue>& SnapSSEL2OrderQueue) = 0;

	/// ��ѯ���L2����ʵʱ����
	/// @param  pCodeList                     -- ���ն����б�NULLʱΪȫ�г���������
	/// @param  SnapSZSEL2ShortQuotation      -- ���������б�
	/// @return RetCode                       -- �������
	virtual RetCode QuerySnap_SZSEL2_ShortQuotation(char* pCodeList, CDataBuffer<SZSEL2_ShortQuotation>& SnapSZSEL2ShortQuotation) = 0;

	/// ��ѯ���L2ί�ж���
	/// @param  pCodeList                     -- ���ն����б�NULLʱΪȫ�г���������
	/// @param  SnapSZSEL2OrderQueue          -- ���������б�
	/// @return RetCode                       -- �������
	virtual RetCode QuerySnap_SZSEL2_OrderQueue(char* pCodeList, CDataBuffer<SZSEL2_OrderQueue>& SnapSZSEL2OrderQueue) = 0;

	/// ��ѯ�Ͻ���L2 ETFͳ�����ݿ���
	/// @param  pCodeList               -- ���ն����б�NULLʱΪȫ�г���������
	/// @param  data    -- ���������б�
	/// @return RetCode                 -- �������
	virtual RetCode QuerySnap_SSEL2_ETF(char* pCodeList, CDataBuffer<SSEL2_ETF>& data) = 0;

    /// ��ѯ������L1��̬���ݿ���
    /// @param  pCodeList               -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  data    -- ���������б�
    /// @return RetCode                 -- �������
    virtual RetCode QuerySnap_GFEXL1_Static(char* pCodeList, CDataBuffer<GFEXL1_Static>& data) = 0;
    /// ��ѯ������L1�����������
    /// @param  pCodeList               -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  data    -- ���������б�
    /// @return RetCode                 -- �������
    virtual RetCode QuerySnap_GFEXL1_ArbiQuotation(char* pCodeList, CDataBuffer<GFEXL1_ArbiQuotation>& data) = 0;
    /// ��ѯ������L1ʵʱ�������
    /// @param  pCodeList               -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  data    -- ���������б�
    /// @return RetCode                 -- �������
    virtual RetCode QuerySnap_GFEXL1_Quotation(char* pCodeList, CDataBuffer<GFEXL1_Quotation>& data) = 0;
    /// ��ѯ������L1��Ȩ��������
    /// @param  pCodeList               -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  data    -- ���������б�
    /// @return RetCode                 -- �������
    virtual RetCode QuerySnap_GFEXL1_OptionParameter(char* pCodeList, CDataBuffer<GFEXL1_OptionParameter>& data) = 0;
    /// ��ѯ������L2��̬���ݿ���
    /// @param  pCodeList               -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  data    -- ���������б�
    /// @return RetCode                 -- �������
    virtual RetCode QuerySnap_GFEXL2_Static(char* pCodeList, CDataBuffer<GFEXL2_Static>& data) = 0;
    /// ��ѯ������L2�����������
    /// @param  pCodeList               -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  data    -- ���������б�
    /// @return RetCode                 -- �������
    virtual RetCode QuerySnap_GFEXL2_ArbiQuotation(char* pCodeList, CDataBuffer<GFEXL2_ArbiQuotation>& data) = 0;
    /// ��ѯ������L2ʵʱ�������
    /// @param  pCodeList               -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  data    -- ���������б�
    /// @return RetCode                 -- �������
    virtual RetCode QuerySnap_GFEXL2_Quotation(char* pCodeList, CDataBuffer<GFEXL2_Quotation>& data) = 0;
    /// ��ѯ������L2ί�ж����������
    /// @param  pCodeList               -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  data    -- ���������б�
    /// @return RetCode                 -- �������
    virtual RetCode QuerySnap_GFEXL2_OrderQueue(char* pCodeList, CDataBuffer<GFEXL2_OrderQueue>& data) = 0;
    /// ��ѯ������L2ί��ͳ���������
    /// @param  pCodeList               -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  data    -- ���������б�
    /// @return RetCode                 -- �������
    virtual RetCode QuerySnap_GFEXL2_OrderStatistic(char* pCodeList, CDataBuffer<GFEXL2_OrderStatistic>& data) = 0;
    /// ��ѯ������L2ʵʱ����ۿ���
    /// @param  pCodeList               -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  data    -- ���������б�
    /// @return RetCode                 -- �������
    virtual RetCode QuerySnap_GFEXL2_RealTimePrice(char* pCodeList, CDataBuffer<GFEXL2_RealTimePrice>& data) = 0;
    /// ��ѯ������L2�ּ۳ɽ����������
    /// @param  pCodeList               -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  data    -- ���������б�
    /// @return RetCode                 -- �������
    virtual RetCode QuerySnap_GFEXL2_MarchPriceQty(char* pCodeList, CDataBuffer<GFEXL2_MarchPriceQty>& data) = 0;
    /// ��ѯ������L2��Ȩ��������
    /// @param  pCodeList               -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  data    -- ���������б�
    /// @return RetCode                 -- �������
    virtual RetCode QuerySnap_GFEXL2_OptionParameter(char* pCodeList, CDataBuffer<GFEXL2_OptionParameter>& data) = 0;
    /// ��ѯ������L2����ί�ж��п���
    /// @param  pCodeList               -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  data    -- ���������б�
    /// @return RetCode                 -- �������
    virtual RetCode QuerySnap_GFEXL2_ArbiOrderQueue(char* pCodeList, CDataBuffer<GFEXL2_ArbiOrderQueue>& data) = 0;

    /// ��ѯ�Ͻ���L2�̺�̶��۸���ʳɽ�����
    /// @param  pCodeList               -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  data    -- ���������б�
    /// @return RetCode                 -- �������
    virtual RetCode QuerySnap_SSEL2_FpTransaction(char* pCodeList, CDataBuffer<SSEL2_FpTransaction>& data) = 0;
    /// ��ѯ�Ͻ���L2����������
    /// @param  pCodeList               -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  data    -- ���������б�
    /// @return RetCode                 -- �������
    virtual RetCode QuerySnap_SSEL2_Tick(char* pCodeList, CDataBuffer<SSEL2_Tick>& data) = 0;

    /// ��ѯ������ָ������ ����
    /// @param  pCodeList               -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  data    -- ���������б�
    /// @return RetCode                 -- �������
    virtual RetCode QuerySnap_DCE_Index(char* pCodeList, CDataBuffer<DCE_Index>& data) = 0;

    /// ��ѯ�Ͻ���L1�̺�̶��۸��������
    /// @param  pCodeList               -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  data    -- ���������б�
    /// @return RetCode                 -- �������
    virtual RetCode QuerySnap_SSEL1_FpQuotation(char* pCodeList, CDataBuffer<SSEL1_FpQuotation>& data) = 0;
    /// ��ѯ�Ͻ���L2�̺�̶��۸��������
    /// @param  pCodeList               -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  data    -- ���������б�
    /// @return RetCode                 -- �������
    virtual RetCode QuerySnap_SSEL2_FpQuotation(char* pCodeList, CDataBuffer<SSEL2_FpQuotation>& data) = 0;
    /// ��ѯ���L1�̺�̶��۸��������
    /// @param  pCodeList               -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  data    -- ���������б�
    /// @return RetCode                 -- �������
    virtual RetCode QuerySnap_SZSEL1_FpQuotation(char* pCodeList, CDataBuffer<SZSEL1_FpQuotation>& data) = 0;
    /// ��ѯ���L2�̺�̶��۸��������
    /// @param  pCodeList               -- ���ն����б�NULLʱΪȫ�г���������
    /// @param  data    -- ���������б�
    /// @return RetCode                 -- �������
    virtual RetCode QuerySnap_SZSEL2_FpQuotation(char* pCodeList, CDataBuffer<SZSEL2_FpQuotation>& data) = 0;
	};
	#endif // GTA_QTS_INTERFACE_BASE_H_