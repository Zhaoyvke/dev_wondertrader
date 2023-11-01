/*!
 * \file TraderMAOpt.h
 * \project	WonderTrader
 *
 * \author Wesley
 * \date 2020/03/30
 *
 * \brief
 */
#pragma once

#include <string>
#include <queue>
#include <stdint.h>

#include <boost/asio/io_service.hpp>

#include "../Includes/WTSTypes.h"
#include "../Includes/ITraderApi.h"
#include "../Includes/WTSCollection.hpp"

#include "../API/maCliApi3.9/include/maCliOptTradeApi.h"

#include "../Share/IniHelper.hpp"
#include "../Share/StdUtils.hpp"
#include "../Share/DLLHelper.hpp"
#include "../Share/WtKVCache.hpp"


USING_NS_WTP;

USE_NAMESPACE_MACLI;


class TraderMAOpt : public ITraderApi, public IOptTraderApi, public CCliOptTradeSpi
{
public:
	TraderMAOpt();
	virtual ~TraderMAOpt();

public:
	typedef enum
	{
		WS_NOTLOGIN,		//δ��¼
		WS_LOGINING,		//���ڵ�¼
		WS_LOGINED,			//�ѵ�¼
		WS_LOGINFAILED,		//��¼ʧ��
		WS_CONFIRM_QRYED,
		WS_CONFIRMED,		//��ȷ��
		WS_ALLREADY			//ȫ������
	} WrapperState;

private:
	int doLogin();

	//////////////////////////////////////////////////////////////////////////
	//ITraderApi�ӿ�
public:
	virtual bool init(WTSVariant* params) override;

	virtual void release() override;

	virtual void registerSpi(ITraderSpi *listener) override;

	virtual bool makeEntrustID(char* buffer, int length) override;

	virtual void connect() override;

	virtual void disconnect() override;

	virtual bool isConnected() override;

	virtual int login(const char* user, const char* pass, const char* productInfo) override;

	virtual int logout() override;

	virtual int orderInsert(WTSEntrust* entrust) override;

	virtual int orderAction(WTSEntrustAction* action) override;

	virtual int queryAccount() override;

	virtual int queryPositions() override;

	virtual int queryOrders() override;

	virtual int queryTrades() override;

	virtual int querySettlement(uint32_t uDate) override;


	//////////////////////////////////////////////////////////////////////////
	//IOptTraderApi
	//virtual int orderInsertOpt(WTSEntrust* entrust) override;

	//virtual int orderActionOpt(WTSEntrustAction* action) override;

	//virtual int	queryOrdersOpt(WTSBusinessType bType) override;


	//////////////////////////////////////////////////////////////////////////
	//maCliOpt���׽ӿ�ʵ��
public:
	// �ͻ�����������ɹ�����ͨ�����Ӻ󣬸÷���������
	virtual int OnConnected(void) override;

	// �ͻ�����������ɹ���ͨ�����ӶϿ�ʱ���÷���������
	virtual int OnDisconnected(int p_nReason, const char *p_pszErrInfo) override;

	// ������Ӧ
	virtual int OnRspHeartBeat(CFirstSetField *p_pFirstSet, CRspHeartBeatField *p_pRspField, LONGLONG p_llRequestId, int p_iFieldNum, int p_iFieldIndex) override;

	// �û���¼������Ӧ
	virtual int OnRspUserLogin(CFirstSetField *p_pFirstSet, CRspOptUserLoginField *p_pRspField, LONGLONG p_llRequestId, int p_iFieldNum, int p_iFieldIndex) override;

	// �����ʽ��ѯ��Ӧ
	virtual int OnRspQryExpendableFund(CFirstSetField* p_pFirstSetField, CRspOptExpendableFundField* p_pRspField, LONGLONG p_llRequestId, int p_iFieldNum, int p_iFieldIndex) override;

	// ���ú�Լ�ʲ���ѯ��Ӧ
	virtual int OnRspQryExpendableCu(CFirstSetField* p_pFirstSetField, CRspOptExpendableCuField* p_pRspField, LONGLONG p_llRequestId, int p_iFieldNum, int p_iFieldIndex) override;


	// ������Ȩί���걨��Ӧ
	virtual int OnRspOrder(CFirstSetField *p_pFirstSet, CRspOptOrderField *p_pRspField, LONGLONG p_llRequestId, int p_iFieldNum, int p_iFieldIndex) override;

	// ������Ȩί�г�����Ӧ
	virtual int OnRspCancelOrder(CFirstSetField* p_pFirstSetField, CRspOptCancelOrderField* p_pRspField, LONGLONG p_llRequestId, int p_iFieldNum, int p_iFieldIndex) override;

	// ȷ�ϻر�
	virtual int OnRtnOrderConfirm(CRtnOptOrderConfirmField* p_pRspField) override;
	//ȷ�ϻر����ͣ����٣���Ӧ
	virtual int OnRtnOrderConfirmFlash(CRtnOptOrderConfirmFlashField *p_pRtnField);

	// �ɽ��ر�--ί����Ϣ
	virtual int OnRtnOrder(CRtnOptOrderField* p_pRspField) override;

	// �ɽ��ر�--�ɽ���Ϣ
	virtual int OnRtnOrderFill(CRtnOptOrderFillField* p_pRtnField) override;
	//�ɽ��ر�����(���٣���Ӧ
	virtual int OnRtnOrderFillFlash(CRtnOptOrderFillFlashField *p_pRtnField);

	virtual int OnRtnSubTopic(CRspSubTopicField* p_pRspField) override;

	// �ɽ��ر�--��Լ��Ϣ
	virtual int OnRtnContract(CRtnOptContractField* p_pRspField) override;

	//�ͻ����㵥��ѯ 2015/8/6
	virtual int OnRspQrySettList(CFirstSetField* p_pFirstSet, CRspOptSettListField* p_pRspField, LONGLONG p_llRequestId, int p_iFieldNum, int p_iFieldIndex) override;

	// ������Ȩ����ί�в�ѯ��Ӧ
	virtual int OnRspQryCurrDayOrder(CFirstSetField* p_pFirstSetField, CRspOptCurrDayOrderField* p_pRspField, LONGLONG p_llRequestId, int p_iFieldNum, int p_iFieldIndex) override;

	// ������Ȩ���ճɽ���ѯ��Ӧ
	virtual int OnRspQryCurrDayFill(CFirstSetField* p_pFirstSetField, CRspOptCurrDayFillField* p_pRspField, LONGLONG p_llRequestId, int p_iFieldNum, int p_iFieldIndex) override;

protected:
	/*
	*	��������Ϣ
	*/
	bool IsErrorRspInfo(CFirstSetField *pRspInfo);

	WTSOrderInfo*	makeOrderInfo(CRspOptCancelOrderField* orderField);
	WTSOrderInfo*	makeOrderInfo(CRspOptOrderField* orderField);
	WTSOrderInfo*	makeOrderInfo(CRtnOptOrderField* orderField);
	WTSOrderInfo*   makeOrderInfo(CRspOptCurrDayOrderField* daiOrderField);
	WTSEntrust*		makeEntrust(CRspOptOrderField  *entrustField);
	WTSError*		makeError(CFirstSetField* rspInfo);
	WTSTradeInfo*	makeTradeRecord(CRtnOptOrderFillField *tradeField);
	WTSTradeInfo*   makeTradeRecord(CRspOptCurrDayFillField *dayTradeField);
	WTSEntrust*		makeEntrust(CRtnOptOrderFillField  *entrustField);

	void			generateEntrustID(char* buffer, int order_bsn);
	bool			extractEntrustID(const char* entrustid, int &order_bsn);

	uint32_t		genRequestID();

protected:
	std::string		m_strChannel;  // ��������

	std::string		m_strHost;  // IP��ַ
	int				m_iPort;  // �˿�

	std::string		m_strUser;
	std::string		m_strPass;

	std::string		m_strTrdAcct;		// �����˻�
	int64_t			m_llCustCode;		// �ͻ�����
	int64_t			m_llCuacctCode;		// �ʽ𻧴���
	int32_t			m_iInitOrg;

	std::string		m_strAuthData;
	std::string		m_strEncryptKey;

	std::string		m_strUseScope;
	std::string		m_strAuthType;
	std::string		m_strEncryptType;
	std::string		m_strAcctType;
	std::string		m_strOptSite;  // ��ͷ

	std::string		m_strShPBU;  // ���׵�Ԫ
	std::string		m_strSzPBU;  // ���׵�Ԫ

	ITraderSpi*					m_bscSink;
	IOptTraderSpi*				m_optSink;

	uint32_t					m_lDate;
	std::atomic<uint32_t>		m_orderRef;		//��������

	WrapperState				m_wrapperState;

	CCliOptTradeApi*			m_pUserAPI;
	std::atomic<uint32_t>		m_iRequestID;

	typedef WTSHashMap<std::string> PositionMap;
	PositionMap*				m_mapPosition;
	WTSArray*					m_ayTrades;
	WTSArray*					m_ayOrders;

	IBaseDataMgr*				m_bdMgr;

	boost::asio::io_service		_asyncio;
	StdThreadPtr				_thrd_worker;

	std::string					m_strModule;
	DllHandle					m_hInstMA;

	//ί�е���ǻ�����
	WtKVCache					m_eidCache;

	//�ɽ�����ǻ�����
	WtKVCache					m_oidCache;
};
