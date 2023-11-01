/*!
 * \file TraderHuaX.h
 * \project	WonderTrader
 *
 * \author HeJ
 * \date 2022/07/31
 * 
 * \brief 
 */
#pragma once

#include <stdint.h>
#include <string>
#include <boost/asio/io_service.hpp>

#include "../Includes/ITraderApi.h"
#include "../Includes/WTSCollection.hpp"

#include "../Share/StdUtils.hpp"
#include "../Share/DLLHelper.hpp"
#include "../Share/WtKVCache.hpp"

#include "../Includes/IBaseDataMgr.h"
#include "../Includes/WTSContractInfo.hpp"
#include "../Includes/WTSSessionInfo.hpp"
#include "../Includes/WTSTradeDef.hpp"
#include "../Includes/WTSError.hpp"
#include "../Includes/WTSVariant.hpp"
#include "../Includes/WTSCollection.hpp"
#include "../Share/TimeUtils.hpp"
#include "../Share/ModuleHelper.hpp"

#include <boost/filesystem.hpp>

 //By Wesley @ 2022.01.05
#include "../Share/fmtlib.h"

#include "../API/HuaX4.03/traderapi/TORATstpTraderApi.h"

USING_NS_WTP;

using namespace TORASTOCKAPI;
typedef CTORATstpTraderSpi HuaXTraderSpi;
typedef CTORATstpTraderApi HuaXTraderApi;

const char* WT_MKT_SH_A = "SSE";
const char* WT_MKT_SZ_A = "SZSE";

class TraderHuaX : public HuaXTraderSpi, public ITraderApi
{
public:
	TraderHuaX();
	virtual ~TraderHuaX();

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
	//HuaX::API::TraderSpi �ӿ�
	virtual void OnFrontConnected() override;
	///���ͻ����뽻�׺�̨ͨ�����ӶϿ�ʱ���÷��������á���������������API���Զ��������ӣ��ͻ��˿ɲ�������
	///@param nReason ����ԭ��
	///        -3 �����ѶϿ�
	///        -4 �����ʧ��
	///        -5 ����дʧ��
	///        -6 ����������
	///        -7 ����Ŵ���
	///        -8 �������������
	///        -9 ����ı���
	///		  -15 �����ʧ��
	///		  -16 ����дʧ��
	virtual void OnFrontDisconnected(int nReason) override;

	virtual void OnRspError(CTORATstpRspInfoField* pRspInfoField, int nRequestID, bool bIsLast) override;

	virtual void OnRspOrderInsert(CTORATstpInputOrderField* pInputOrderField, CTORATstpRspInfoField* pRspInfoField, int nRequestID) override;

	virtual void OnRtnOrder(CTORATstpOrderField* pOrderField) override;

	virtual void OnErrRtnOrderInsert(CTORATstpInputOrderField* pInputOrderField, CTORATstpRspInfoField* pRspInfoField, int nRequestID) override;

	virtual void OnRtnTrade(CTORATstpTradeField* pTradeField) override;

	virtual void OnErrRtnOrderAction(CTORATstpInputOrderActionField* pInputOrderActionField, CTORATstpRspInfoField* pRspInfoField, int nRequestID) override;

	virtual void OnRspQryOrder(CTORATstpOrderField* pOrderField, CTORATstpRspInfoField* pRspInfoField, int nRequestID, bool bIsLast) override;

	virtual void OnRspQryTrade(CTORATstpTradeField* pTradeField, CTORATstpRspInfoField* pRspInfoField, int nRequestID, bool bIsLast) override;

	virtual void OnRspQryPosition(CTORATstpPositionField* pPositionField, CTORATstpRspInfoField* pRspInfoField, int nRequestID, bool bIsLast) override;

	virtual void OnRspQryTradingAccount(CTORATstpTradingAccountField* pTradingAccountField, CTORATstpRspInfoField* pRspInfoField, int nRequestID, bool bIsLast) override;

	virtual void OnRspUserLogin(CTORATstpRspUserLoginField* pRspUserLoginField, CTORATstpRspInfoField* pRspInfoField, int nRequestID) override;

	virtual void OnRspGetConnectionInfo(CTORATstpConnectionInfoField* pConnectionInfoField, CTORATstpRspInfoField* pRspInfo, int nRequestID) override;

	virtual void OnRspQryShareholderAccount(CTORATstpShareholderAccountField* pShareholderAccountField, CTORATstpRspInfoField* pRspInfoField, int nRequestID, bool bIsLast) override;

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

	inline WTSOrderInfo*	makeOrderInfo(CTORATstpOrderField* orderField);
	inline WTSEntrust*		makeEntrust(CTORATstpInputOrderField* entrustField);
	inline WTSTradeInfo*	makeTradeInfo(CTORATstpTradeField*tradeField);
	inline WTSPositionItem* makePositionItem(CTORATstpPositionField* positionField);
	inline WTSAccountInfo*	makeAccountInfo(CTORATstpTradingAccountField* accountField);

	inline WTSDirectionType wrapDirectionType(TTORATstpDirectionType dirType);
	inline WTSPriceType wrapPriceType(TTORATstpDirectionType priceType);
	inline WTSOrderState wrapOrderState(TTORATstpOrderStatusType orderState);


	inline bool	extractEntrustID(const char* entrustid, int &orderRef);
	inline void	genEntrustID(char* buffer, uint32_t orderRef);

private:
	HuaXTraderApi*	_api;
	ITraderSpi*		_sink;

	typedef WTSHashMap<std::string> PositionMap;
	WTSArray*				_positions;
	WTSArray*				_trades;
	WTSArray*				_orders;
	WTSArray*				_accounts;

	IBaseDataMgr*			_bd_mgr;

	std::string		_user;
	std::string		_pass;
	std::string		_flowdir;
	bool			_encrypt;  // Ϊ���������Ƿ���ܴ��䣬�������ݰ�ȫ�ԣ������Ի�������ʽ������ն�����Ϊ���ܴ���
	std::string		_appid; // �ն�����
	std::string		_sse_share_hold_id; // ���йɶ���
	std::string		_szse_share_hold_id; // ���йɶ���
	std::string		_terminal; // �ն����ͣ�Ĭ��PC
	std::string		_pub_ip; // ����IP
	std::string		_pub_port; // ����IP�˿ں�
	std::string		_trade_ip; // ��������IP
	std::string		_mac; // MAC��ַ���йܷ���������MAC
	std::string		_hard_disk; // Ӳ�����к�,�йܷ�����Ӳ�����к�

	std::string		_front; // ��tcp://210.14.72.16:9500

	bool			_quick; // ���ش���ʽ
	bool			_inited;

	std::string		_productInfo;

	TraderState		_state;

	uint32_t		_tradingday;
	std::atomic<uint32_t>		_reqid;
	std::atomic<int>		_ordref;		//��������

	boost::asio::io_service		_asyncio;
	StdThreadPtr				_thrd_worker;

	DllHandle		_hInstHuaX;
	typedef HuaXTraderApi* (*HuaXCreator)(const char*, bool);
	HuaXCreator		_funcCreator;

	//ί�е���ǻ�����
	WtKVCache		_eidCache;
	//������ǻ�����
	WtKVCache		_oidCache;
};

