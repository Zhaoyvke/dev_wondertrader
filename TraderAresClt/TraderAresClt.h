/*!
 * \file TraderAresClt.h
 * \project	WonderTrader
 *
 * \author Suuuuunseeker
 * \date 2022/08/11
 *
 * \brief
 */
#pragma once

#include <string>
#include <queue>
#include <stdint.h>
#include <unordered_map>
#include <boost/asio/io_service.hpp>

#include "../Includes/WTSTypes.h"
#include "../Includes/ITraderApi.h"
#include "../Includes/WTSCollection.hpp"

#include "../API/AresClt/include/ITDErebusApi.h"


#include "../Share/IniHelper.hpp"
#include "../Share/StdUtils.hpp"
#include "../Share/DLLHelper.hpp"
#include "../Share/WtKVCache.hpp"


USING_NS_WTP;

class TraderAresClt : public ITraderApi, public IOptTraderApi, public ITDErebusSpi
{
public:
	TraderAresClt();
	virtual ~TraderAresClt();

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
	int queryConfirm();

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
	//AresClt���׽ӿ�ʵ��
public:
	virtual		void	OnFrontConnected() override;
	virtual		void	OnFrontDisconnected() override;

	virtual		void	OnRspUserLogin(tagXTRspUserLoginField* data, tagXTRspInfoField* error, int id, bool last) override;

	virtual		void	OnRspQryTradingAccount(tagXTRspAccountField* data, tagXTRspInfoField* error, int id, bool last) override;
	virtual		void	OnRspQryInvestorPosition(tagXTRspPositionField* data, tagXTRspInfoField* error, int id, bool last) override;
	virtual		void	OnRspQryOrder(tagXTOrderField* data, tagXTRspInfoField* error, int id, bool last) override;
	virtual		void	OnRspQryTrade(tagXTTradeField* data, tagXTRspInfoField* error, int id, bool last) override;

	virtual		void	OnRspOrderInsert(tagXTReqOrderInsertField* data, tagXTRspInfoField* error, int id, bool last) override;
	virtual		void	OnErrRtnOrderInsert(tagXTReqOrderInsertField* data, tagXTRspInfoField* error) override;

	virtual		void	OnRspOrderAction(tagXTReqOrderCancelField* data, tagXTRspInfoField* error, int id, bool last) override;
	virtual		void	OnErrRtnOrderAction(tagXTReqOrderCancelField* data, tagXTRspInfoField* error) override;

	virtual		void	OnRtnOrder(tagXTOrderField* data) override;
	virtual		void	OnRtnTrade(tagXTTradeField* data) override;

public:
	void				FreeApi(ITDErebusApi* p);
	void				FreeApp(ITDErebusApp* p);

private:
	//void				reconnect();

protected:
	/*
	*	��������Ϣ
	*/
	bool IsErrorRspInfo(tagXTRspInfoField *pRspInfo);

	WTSOrderInfo*	makeOrderInfo(tagXTOrderField* orderField);
	//WTSOrderInfo*	makeOrderInfo(tagXTReqOrderInsertField* orderInsertField);
	WTSEntrust*		makeEntrust(tagXTReqOrderInsertField *entrustField);
	WTSEntrust*		makeEntrust(tagXTReqOrderCancelField *entrustField);
	WTSError*		makeError(tagXTRspInfoField* rspInfo);
	WTSError*		makeError(tagXTRspInfoField* rspInfo, WTSErroCode ec);
	WTSTradeInfo*	makeTradeRecord(tagXTTradeField *tradeField);

	void			generateEntrustID(char* buffer, uint32_t orderRef);
	bool			extractEntrustID(const char* entrustid, uint32_t &orderRef);

	uint32_t		genRequestID();

protected:
	int				m_nLine;
	std::string		m_strUserType;
	std::string		m_strUserID;
	std::string		m_strInvestorID;

	std::string		m_strPasswd;
	std::string		m_strExtraPasswd;

	std::string		m_strIP;
	int				m_nPort;

	std::string		m_strFlowDir;

	std::string		m_strTag;

	uint32_t		m_nCurIdx;  // �µ����ر��룬��Ҫ�ֶ�����

	std::string		m_strErrInfo;  // ���ش�����Ϣ

	ITraderSpi*		m_bscSink;
	uint64_t		m_uLastQryTime;

	uint32_t					m_lDate;
	std::atomic<uint32_t>		m_orderRef;		//��������

	WrapperState				m_wrapperState;

	ITDErebusApi*				m_pUserAPI;
	ITDErebusApp*				m_pApp;  // //[3]��������App����ʼ��
	std::atomic<uint32_t>		m_iRequestID;

	typedef WTSHashMap<std::string> PositionMap;
	PositionMap*				m_mapPosition;
	WTSArray*					m_ayTrades;
	WTSArray*					m_ayOrders;
	WTSArray*					m_ayPosDetail;

	std::unordered_map<std::string, std::string> m_OrderIdMap;  // ���ټ�¼�Ѿ���ɵĶ�����Ϣ
	std::unordered_map<std::string, std::vector<int>> umapOrderPos;  // �����ֲ֣�ͳ��ÿ����Լͬһ��OrderID��ί�����

	IBaseDataMgr*				m_bdMgr;

	//typedef std::queue<CommonExecuter>	QueryQue;
	//QueryQue				m_queQuery;
	bool					m_bInQuery;
	StdUniqueMutex			m_mtxQuery;
	uint64_t				m_lastQryTime;

	bool					m_bStopped;
	StdThreadPtr			m_thrdWorker;

	boost::asio::io_service		_asyncio;

	std::string		m_strModule;
	DllHandle		m_hInstAres;
	typedef void*	(*pfnCreateObj)(const char*, const char*);
	typedef void(*pfnDestroyObj)(void*);
	pfnCreateObj	m_pfnCreateObj;
	pfnDestroyObj	m_pfnDestroyObj;

	IniHelper		m_iniHelper;

	//ί�е���ǻ�����
	//WtKVCache		m_eidCache;
	//������ǻ�����
	//WtKVCache		m_oidCache;
};

