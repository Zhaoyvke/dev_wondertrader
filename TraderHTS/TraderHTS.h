/*!
 * \file TraderHTS.h
 * \project	WonderTrader
 *
 * \author Suuuunseeker
 * \date 2022/09/06
 *
 * \brief
 */
#pragma once

#include <string>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <stdint.h>

#include "../API/HTS5.2.43.0/include/secitpdk.h"

#include <boost/asio/io_service.hpp>
#include <boost/asio/strand.hpp>

#include "../Includes/WTSTypes.h"
#include "../Includes/ITraderApi.h"
#include "../Includes/WTSCollection.hpp"

#include "../Share/IniHelper.hpp"
#include "../Share/StdUtils.hpp"
#include "../Share/DLLHelper.hpp"


USING_NS_WTP;


class TraderHTS : public ITraderApi
{
	//friend void callback_rtn1(const char* pTime, const char* pMsg, int nType);

public:
	TraderHTS();
	virtual ~TraderHTS();

public:
	typedef enum
	{
		WS_NOTLOGIN,		//δ��¼
		WS_LOGINING,		//���ڵ�¼
		WS_LOGINED,			//�ѵ�¼
		WS_LOGINFAILED,		//��¼ʧ��
		WS_ALLREADY			//ȫ������
	} WrapperState;

private:
	void doLogin();
	void InitializeHTS(WTSVariant* params);
	void qryGDNo();	//��ѯ�ɶ���
	//void qryCustInfo();  // ��ѯ�ͻ���Ϣ
	//void qryCustNodeInfo();  // ��ѯ�ͻ��ڵ���Ϣ

	void reconnect();

	void triggerQuery();

public:
	/* ���ն��ĵĻر����� */
	void htsGetCallback(const char* pTime, const char* pMsg, int nType);

	void OnRtnOrder(const char* pMsg);  // ί����Ϣ
	void OnRtnTrade(const char* pMsg);  // �ɽ���Ϣ

private:
	
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

	virtual int orderInsert(WTSEntrust* eutrust) override;

	virtual int orderAction(WTSEntrustAction* action) override;

	virtual int queryAccount() override;

	virtual int queryPositions() override;

	virtual int queryOrders() override;

	virtual int queryTrades() override;

protected:
	std::string		m_strUser;  // �ͻ���
	std::string		m_strPass;  // ����
	std::string		m_strNode;  // վ����Ϣ

	char			sShGdh[13];                // �Ϻ��ɶ���
	char			sSzGdh[13];                // ���ڹɶ���
	char			sShZjzh[13];               // �Ϻ��ʽ��˺�
	char			sSzZjzh[13];               // �����ʽ��˺�

	std::string		m_strWtfs;  // ί�з�ʽ
	std::string		m_strKey;  // �����ļ�����
	std::string		m_strDevName;  // �����̴���

	std::string		m_strTag;

	ITPDK_CusReqInfo cusreqinfo;  // ��¼��Ϣ

	ITraderSpi*		m_traderSink;
	uint64_t		m_uLastQryTime;

	bool			m_bUseEX;  // ʹ��EX��׺
	bool			m_bASync;  // �Ƿ�ʹ���첽

	typedef WTSHashMap<std::string> TradeDataMap;
	TradeDataMap*	m_mapLives;
	std::unordered_set<std::string>	m_tradeids;

	uint32_t					m_lDate;
	std::atomic<uint32_t>		m_orderRef;		//��������

	WrapperState				m_wrapperState;

	uint64_t					m_uSessID;

	IBaseDataMgr*				m_bdMgr;

	typedef std::queue<CommonExecuter>	QueryQue;
	QueryQue			m_queQuery;
	bool				m_bInQuery;
	StdUniqueMutex		m_mtxQuery;
	uint64_t			m_lastQryTime;

	boost::asio::io_service		m_asyncIO;
	boost::asio::io_service::strand*	m_strandIO;
	StdThreadPtr		m_thrdWorker;

	std::string		m_strModule;
	DllHandle		m_hInstDD;

	IniHelper		m_iniHelper;
};


class HTSCallMgr
{
public:
	HTSCallMgr();
	~HTSCallMgr();

public:
	bool init(WTSVariant* params);
	void setCallbackMsgFunc();
	void setHTSCallPtr(std::string khh, TraderHTS* pTrader);

private:
	static void callbackMsgFunc(const char* pTime, const char* pMsg, int nType);
	static void callbackConnFunc(const char* pKhh, const char* pConnKey, int nEvent, int nType);
};