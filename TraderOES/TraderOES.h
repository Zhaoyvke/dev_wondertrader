/*!
 * \file TraderOES.h
 * \project	WonderTrader
 *
 * \author Wesley
 * \date 2020/03/30
 * 
 * \brief 
 */
#pragma once

#include <stdint.h>
#include <boost/asio/io_service.hpp>

#include <oes_api/oes_async_api.h>

#include "../Includes/ITraderApi.h"
#include "../Includes/WTSCollection.hpp"

#include "../Share/WtKVCache.hpp"
#include "../Share/StdUtils.hpp"
#include "../Share/DLLHelper.hpp"

USING_NS_WTP;

class TraderOES : public ITraderApi
{
public:
	TraderOES();
	virtual ~TraderOES();

	typedef enum
	{
		TS_NOTLOGIN,		//δ��¼
		TS_LOGINING,		//���ڵ�¼
		TS_LOGINED,			//�ѵ�¼
		TS_LOGINFAILED,		//��¼ʧ��
		TS_ALLREADY			//ȫ������
	} TraderState;

public:
	void	handle_ord_connected(OesAsyncApiChannelT *pAsyncChannel);
	void	handle_ord_disconnected(OesAsyncApiChannelT *pAsyncChannel);
	void	handle_rpt_connected(OesAsyncApiChannelT *pAsyncChannel);
	void	handle_rpt_disconnected(OesAsyncApiChannelT *pAsyncChannel);
	void	handle_rpt_message(SMsgHeadT *pMsgHead, void *pMsgItem);
	void	handle_ord_message(SMsgHeadT *pMsgHead, void *pMsgItem);

	void	handle_rsp_account(SMsgHeadT *pMsgHead, void *pMsgItem, OesQryCursorT *pQryCursor);
	void	handle_rsp_orders(SMsgHeadT *pMsgHead, void *pMsgItem, OesQryCursorT *pQryCursor);
	void	handle_rsp_trades(SMsgHeadT *pMsgHead, void *pMsgItem, OesQryCursorT *pQryCursor);
	void	handle_rsp_positions(SMsgHeadT *pMsgHead, void *pMsgItem, OesQryCursorT *pQryCursor);

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
	void		doLogin();

	inline WTSOrderInfo*	makeOrderInfo(OesOrdCnfmT* orderField);
	inline WTSEntrust*		makeEntrust(OesOrdRejectT *entrustField);
	inline WTSTradeInfo*	makeTradeInfo(OesTrdCnfmT *tradeField);

	inline bool	extractEntrustID(const char* entrustid, uint32_t &orderRef);
	inline void	genEntrustID(char* buffer, uint32_t orderRef);

private:
	OesAsyncApiContextT*	_context;
	OesAsyncApiChannelT*	_channel;
	OesAsyncApiChannelT*	_channel_qry;
	ITraderSpi*				_sink;

	IBaseDataMgr*			_bd_mgr;

	WTSArray*		_trades;
	WTSArray*		_orders;
	WTSArray*		_funds;
	WTSArray*		_positions;

	std::string		_config;
	std::string		_user;
	bool			_inited;

	TraderState		_state;

	uint32_t		_tradingday;

	boost::asio::io_service		_asyncio;
	StdThreadPtr				_thrd_worker;

	//ί�е���ǻ�����
	WtKVCache		m_eidCache;
	//������ǻ�����
	WtKVCache		m_oidCache;
};

