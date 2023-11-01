/*!
 * \file TraderZSOpt.h
 * \project	WonderTrader
 *
 * \author Sunseeeeeeker
 * \date 2023/05/08
 * 
 * \brief 
 */
#pragma once

#include <stdint.h>
#include <boost/asio/io_service.hpp>

#include "../API/zsTradeApi1.6.0/include/tradeApi.h"

#include "../Includes/ITraderApi.h"
#include "../Includes/WTSCollection.hpp"

#include "../Share/StdUtils.hpp"
#include "../Share/DLLHelper.hpp"
#include "../Share/WtKVCache.hpp"

#define WAITRSP_TIMEOUT 15


USING_NS_WTP;

class TraderZSOpt : public CTradeCallback, public ITraderApi
{
public:
	TraderZSOpt();
	virtual ~TraderZSOpt();

	typedef enum
	{
		TS_NOTLOGIN,		//δ��¼
		TS_LOGINING,		//���ڵ�¼
		TS_LOGINED,			//�ѵ�¼
		TS_LOGINFAILED,		//��¼ʧ��
		TS_ALLREADY			//ȫ������
	} TraderState;

public:
	//////////////////////////////////////////////////////////////////////////
	//CTradeCallback �ӿ�
	virtual int OnConnected(void) override;

	virtual int OnDisconnected(int p_nReason, const char *p_pszErrInfo) override;

	virtual int OnAcctRegister(STFirstSet *p_pFirstSet, STRspAcctRegister *p_pRspAcctLoginState, LONGLONG  p_llReqId, int p_iNum);

	// ί����Ӧ
	virtual int OnOrderOpt(STFirstSet *p_pFirstSet, STRspOrderOpt *p_pRspField, LONGLONG  p_llReqId, int p_iNum);

	// ������Ӧ
	virtual int OnOptCancelOrder(STFirstSet *p_pFirstSet, STRspOptCancelOrder *p_pRspField, LONGLONG  p_llReqId, int p_iNum);

	// �ֲֲ�ѯ��Ӧ
	virtual int OnOptQryHolding(STFirstSet *p_pFirstSet, STRspOptQryHolding *p_pRspField, LONGLONG  p_llReqId, int p_iNum);

	// �ɽ���ѯ��Ӧ
	virtual int OnOptQryFill(STFirstSet *p_pFirstSet, STRspOptQryFill *p_pRspField, LONGLONG  p_llReqId, int p_iNum);

	// �ʽ��ѯ��Ӧ
	virtual int OnOptQryMoney(STFirstSet *p_pFirstSet, STRspOptQryMoney *p_pRspField, LONGLONG  p_llReqId, int p_iNum);

	// ί�в�ѯ��Ӧ
	virtual int OnOptQryOrder(STFirstSet *p_pFirstSet, STRspOptQryOrder *p_pRspField, LONGLONG  p_llReqId, int p_iNum);

	// ȷ�ϻر�
	virtual int OnRtnOrderConfirm(STRtnOrderConfirm *p_pRtnOrderConfirm);

	// �ɽ��ر�
	virtual int OnRtnOrderFill(STRtnOrderFill *p_pRtnOrderFill);

public:
	//////////////////////////////////////////////////////////////////////////
	//ITraderApi �ӿ�
	virtual bool init(WTSVariant *params) override;

	virtual void release() override;

	virtual void registerSpi(ITraderSpi *listener) override;

	virtual void connect() override;

	virtual void disconnect() override;

	virtual bool isConnected() override;

	virtual bool makeEntrustID(char* buffer, int length) override;

	virtual int login(const char* user, const char* pass, const char* productInfo) override;

	virtual int logout() override;

	virtual int orderInsert(WTSEntrust* eutrust) override;

	virtual int orderAction(WTSEntrustAction* action) override;

	virtual int queryAccount() override;

	virtual int queryPositions() override;

	virtual int queryOrders() override;

	virtual int queryTrades() override;

private:
	void		reconnect();
	inline uint32_t			genRequestID();
	void					doLogin();

	inline bool	extractEntrustID(const char* entrustid, uint32_t &orderRef);
	inline void	genEntrustID(char* buffer, uint32_t orderRef);

private:
	CTradeApi*		_api;
	ITraderSpi*		_sink;

	typedef WTSHashMap<std::string> PositionMap;
	PositionMap*			_positions;
	WTSArray*				_trades;
	WTSArray*				_orders;

	IBaseDataMgr*			_bd_mgr;

	std::string		_user;
	std::string		_pass;  // ʵ�ʵ�¼��Ҫ���ZSClientʹ�ã��ӿڵ�¼ʱ����Ҫ����
	std::string		_flowdir;

	std::string		_host;
	int				_port;

	bool			_inited;
	bool			_logined;

	long long		_acctCode;

	TraderState		_state;

	uint32_t		_tradingday;
	std::atomic<uint32_t>		_reqid;
	std::atomic<uint32_t>		_ordref;		//��������

	boost::asio::io_service		_asyncio;
	StdThreadPtr				_thrd_worker;

	DllHandle		m_hInstZS;

	//ί�е���ǻ�����
	WtKVCache		m_eidCache;
	//������ǻ�����
	WtKVCache		m_oidCache;
};

