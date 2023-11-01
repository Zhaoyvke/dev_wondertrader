/*!
 * \file ParserQTS.h
 * \project	WonderTrader
 *
 * \author Suuuuuuuuunseeker
 * \date 2023/08/29
 * 
 * \brief 
 */
#pragma once
#include <map>
#include <string>

#if _WIN32
#ifndef WINAPI
#define WINAPI __stdcall
#endif
#endif

#include "../Includes/IParserApi.h"
#include "../Share/DLLHelper.hpp"
#include "../API/GTAQTSApi/include/QTSDataType.h"
#include "../API/GTAQTSApi/include/QTSStruct.h"
#include "../API/GTAQTSApi/include/GTAQTSInterfaceBase.h"


NS_WTP_BEGIN
class WTSTickData;
NS_WTP_END

USING_NS_WTP;

enum ContractType
{
	M_INDX,
	M_BOND,
	M_STK,
	M_ETF,
	M_OPT,
	M_FUT,
	M_UNKNOWN
};

class ParserQTS : public IParserApi, public IGTAQTSCallbackBase
{
public:
	ParserQTS();
	virtual ~ParserQTS();

public:
	enum LoginStatus
	{
		LS_NOTLOGIN,
		LS_LOGINING,
		LS_LOGINED
	};

//IQuoteParser �ӿ�
public:
	virtual bool init(WTSVariant* config) override;

	virtual void release() override;

	virtual bool connect() override;

	virtual bool disconnect() override;

	virtual bool isConnected() override;

	virtual void subscribe(const CodeSet &vecSymbols) override;
	virtual void unsubscribe(const CodeSet &vecSymbols) override;

	virtual void registerSpi(IParserSpi* listener) override;

//////////////////////////////////////////////////////////////////////////
public:
	/// ����״̬���أ����ӳɹ�/ʧ��
	/// @param  msgType     -- ��Ϣ����
	/// @param  errCode     -- ʧ��ԭ�򣬳ɹ�ʱ����0 
	///                     �����ʵʱ����ϵͳV2.X �û��ֲᡷ5.5�����뺬���б�RetCode �½� 
	virtual void OnConnectionState(MsgType msgType, RetCode errCode);

	//////////////////////////////////////  �Ͻ���  ////////////////////////////////
	/// �Ͻ���L1ʵʱ������ն�������ʵʱ�ص��ӿ�
	/// @param  RealSSEL1Quotation  -- ʵʱ����
	virtual void OnSubscribe_SSEL1_Quotation(const SSEL1_Quotation& RealSSEL1Quotation);

	/// �Ͻ���L1ծȯʵʱ���鶩������ʵʱ�ص��ӿ�
	/// @param  RealSSEL1BondQuotation  -- ʵʱծȯ����
	virtual void OnSubscribe_SSEL1_BondQuotation(const SSEL1_BondQuotation& RealSSEL1BondQuotation);

	/// �Ͻ���L2ʵʱ������ն�������ʵʱ�ص��ӿ�
	/// @param  RealSSEL2Quotation  -- ʵʱ����
	virtual void OnSubscribe_SSEL2_Quotation(const SSEL2_Quotation& RealSSEL2Quotation);

	/// �Ͻ���L2ָ�����鶩������ʵʱ�ص��ӿ�
	/// @param  ReRealSSEL2Index    -- ʵʱ����
	virtual void OnSubscribe_SSEL2_Index(const SSEL2_Index& RealSSEL2Index);

	/// �Ͻ���L2ծȯʵʱ���鶩������ʵʱ�ص��ӿ�
	/// @param  RealSSEL2BondQuotation  -- ʵʱծȯ����
	virtual void OnSubscribe_SSEL2_BondQuotation(const SSEL2_BondQuotation& RealSSEL2BondQuotation);

	/// �Ͻ���L2ծȯ��ʶ�������ʵʱ�ص��ӿ�
	/// @param  RealSSEL2BondQuotation  -- ʵʱծȯ����
	virtual void OnSubscribe_SSEL2_BondTick(const SSEL2_BondTick& RealSSEL2BondTick);

	/// �Ͻ���L2��ʳɽ���������ʵʱ�ص��ӿ�
	/// @param  RealSSEL2Transaction    -- ʵʱ����
	virtual void OnSubscribe_SSEL2_Transaction(const SSEL2_Transaction& RealSSEL2Transaction);

	/// �Ͻ���L2���ί�ж�������ʵʱ�ص��ӿ�
	/// @param  RealSSEL2Order    -- ʵʱ����
	virtual void OnSubscribe_SSEL2_Order(const SSEL2_Order& RealSSEL2Order);

	/// �Ͻ���������ȨL1ʵʱ���鶩������ʵʱ�ص��ӿ�
	/// @param  RealSSEIOL1Quotation  -- ʵʱ����
	virtual void OnSubscribe_SSEIOL1_Quotation(const SSEIOL1_Quotation& RealSSEIOL1Quotation);


	/// �Ͻ���L2 ETFͳ�����ݶ�������ʵʱ�ص��ӿ�
	/// @param  RealSSEL2Order    -- ʵʱ����
	/// ��δ����
	//virtual void OnSubscribe_SSEL2_ETF(const SSEL2_ETF& data);

	virtual void OnSubscribe_SSEL2_ShortQuotation(const SSEL2_ShortQuotation& RealSSEL2_ShortQuotation);

	virtual void OnSubscribe_SSEL2_OrderQueue(const SSEL2_OrderQueue& RealSSEL2_OrderQueue);

	// ��δ����
	//virtual void OnSubscribe_SSEL2_Tick(const SSEL2_Tick& data);

	//virtual void OnSubscribe_SSEL1_FpQuotation(const SSEL1_FpQuotation& data);

	//virtual void OnSubscribe_SSEL2_FpQuotation(const SSEL2_FpQuotation& data);

	//////////////////////////////////////  ���  ////////////////////////////////

	/// ���L1ʵʱ���鶩������ʵʱ�ص��ӿ�
	/// @param  RealSZSEL1Quotation -- ʵʱ����
	virtual void OnSubscribe_SZSEL1_Quotation(const SZSEL1_Quotation& RealSZSEL1Quotation);

	/// ���L2ʵʱ���鶩������ʵʱ�ص��ӿ�
	/// @param  RealSZSEL2Quotation -- ʵʱ����
	virtual void OnSubscribe_SZSEL2_Quotation(const SZSEL2_Quotation& RealSZSEL2Quotation);

	/// ���L2���ί�����鶩������ʵʱ�ص��ӿ�
	/// @param  RealSZSEL2Transaction -- ʵʱ����
	virtual void OnSubscribe_SZSEL2_Transaction(const SZSEL2_Transaction& RealSZSEL2Transaction);

	/// ���L2���ί�����鶩������ʵʱ�ص��ӿ�
	/// @param  RealSZSEL2Order -- ʵʱ����
	virtual void OnSubscribe_SZSEL2_Order(const SZSEL2_Order& RealSZSEL2Order);

	/// ���L2ָ�����鶩������ʵʱ�ص��ӿ�
	/// @param  RealSZSEL2Index -- ʵʱ����
	virtual void OnSubscribe_SZSEL2_Index(const SZSEL2_Index& RealSZSEL2Index);

	/// �������L2ծȯʵʱ��������ʵʱ�ص��ӿ�
	/// @param  RealSZSEL2BondQuotation -- ʵʱ����
	virtual void OnSubscribe_SZSEL2_BondQuotation(const SZSEL2_BondQuotation& RealSZSEL2BondQuotation);

	/// �������L2ծȯ��ʳɽ�����ʵʱ�ص��ӿ�
	/// @param  RealSZSEL2BondTransaction -- ʵʱ����
	virtual void OnSubscribe_SZSEL2_BondTransaction(const SZSEL2_BondTransaction& RealSZSEL2BondTransaction);

	/// �������L2ծȯ���ί��ʵʱ��������ʵʱ�ص��ӿ�
	/// @param  RealSZSEL2BondOrder -- ʵʱ����
	virtual void OnSubscribe_SZSEL2_BondOrder(const SZSEL2_BondOrder& RealSZSEL2BondOrder);

	/// �������L2ծȯ�������ʵʱ�ص��ӿ� 
	/// @param  RealSZSEL2_BondTick -- ʵʱ����
	virtual void OnSubscribe_SZSEL2_BondTick(const SZSEL2_BondTick& RealSZSEL2_BondTick);

	/// �������L1ծȯʵʱ����ʵʱ�ص��ӿ�
	/// @param  RealSZSEL1BondQuotation -- ʵʱ����
	virtual void OnSubscribe_SZSEL1_BondQuotation(const SZSEL1_BondQuotation& RealSZSEL1BondQuotation);

	/// ���L2ծȯ�г�����ԤԼ��Ϣ��������ʵʱ�ص��ӿ�
	/// @param  RealSZSEL2BondBidBookingInfo -- ʵʱ����
	//virtual void OnSubscribe_SZSEL2_BondBidBookingInfo(const SZSEL2_BondBidBookingInfo& RealSZSEL2BondBidBookingInfo);

	/// �������������ȨL1ʵʱ����ʵʱ�ص��ӿ�
	/// @param  RealSZSEIOL1Quotation -- ʵʱ����
	virtual void OnSubscribe_SZSEIOL1_Quotation(const SZSEIOL1_Quotation& RealSZSEIOL1Quotation);

	virtual void OnSubscribe_SZSEL2_ShortQuotation(const SZSEL2_ShortQuotation& RealSZSEL2_ShortQuotation);
	virtual void OnSubscribe_SZSEL2_OrderQueue(const SZSEL2_OrderQueue& RealSZSEL2_OrderQueue);
	//virtual void OnSubscribe_SZSEL1_FpQuotation(const SZSEL1_FpQuotation& data);
	//virtual void OnSubscribe_SZSEL2_FpQuotation(const SZSEL2_FpQuotation& data);
	virtual void OnSubscribe_SZSEL2_Tick(const SZSEL2_Tick& data);

private:
	/*
	 *	���͵�¼����
	 */
	void DoLogin();
	/*
	 *	����Ʒ������
	 */
	void SubscribeMDs(bool isAll = false);

private:
	uint32_t			m_uTradingDate;

	LoginStatus			m_loginState;
	IGTAQTSApiBase*		m_pUserAPI;  // ����API����

	std::string			m_strUser;
	std::string			m_strPass;

	// ��¼��������
	// NetType��ʾ�����������ͣ���ѡֵΪ0��1.
	// 0��ʾ��������(Ĭ��ֵ)��1��ʾ����������루������ר�ߵȣ����磺"NetType=1".
	// �ı�NetTypeֵ���µ�¼��ʵ�ֲ�ͬ�����绷�������л�
	std::string			m_strNetType; 
	
	WTSVariant*			m_Servers;

	std::string			m_strLogdir;  // ��־Ŀ¼

	std::string			m_strSHSTKCodes;  // �Ͻ������Ĺ�Ʊ��Լ
	std::string			m_strSZSTKCodes;  // ������Ĺ�Ʊ��Լ
	std::string			m_strSHOptCodes;  // ��Ȩ
	std::string			m_strSZOptCodes;
	std::string			m_strSHETFCodes;  // ETF
	std::string			m_strSZETFCodes;
	std::string			m_strSHBondCodes;  // ծȯ
	std::string			m_strSZBondCodes;
	std::string			m_strSHINDXCodes;  // ָ��
	std::string			m_strSZINDXCodes;

	// ���ĵ���������
	bool				m_bHasOrder;
	bool				m_bHasOrdQue;
	bool				m_bHasTrans;
	bool				m_bHasSnap;
	bool				m_bHasFp;  // �̺�̶��۸�����
	bool				m_bHasArb;  // ͳ����������

	bool				m_bHasL1;  // L1����
	bool				m_bHasL2;  // L2����

	bool				m_bHasSSE;  // �Ƿ����Ͻ���
	bool				m_bHasSZSE;  // �Ƿ����������

	// ���ĵĺ�Լ����
	bool				m_bHasIndex;  // ָ��
	bool				m_bHasShare;  // ��Ʊ
	bool				m_bHasOption;  // ��Ȩ
	bool				m_bHasBond;  // ծȯ
	bool				m_bHasETF;

	bool				m_bLogined;

	CodeSet				m_filterSubs;
	CodeSet				m_ignoreCodes;

	DllHandle			m_hInstQTS;

private:
	IParserSpi*			m_parserSink;
	IBaseDataMgr*		m_pBaseDataMgr;
};

