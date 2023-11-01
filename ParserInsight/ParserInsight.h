/*!
 * \file ParserInsight.h
 * \project	WonderTrader
 *
 * \author Sunseeeeeeker
 * \date 2023/05/22
 *
 * \brief
 */ 
#pragma once
#include "../Includes/IParserApi.h"
#include "../Share/DLLHelper.hpp"
#include <map>

#include "../API/insight_tcp_gateway_3.2.7/include/mdc_gateway_client/include/base_define.h"
#include "../API/insight_tcp_gateway_3.2.7/include/mdc_gateway_client/include/mdc_client_factory.h"
#include "../API/insight_tcp_gateway_3.2.7/include/mdc_gateway_client/include/client_interface.h"
#include "../API/insight_tcp_gateway_3.2.7/include/mdc_gateway_client/include/message_handle.h"


NS_WTP_BEGIN
class WTSTickData;
NS_WTP_END

USING_NS_WTP;

using namespace com::htsc::mdc::gateway;
using namespace com::htsc::mdc::model;
using namespace com::htsc::mdc::insight::model;

class ParserInsight : public IParserApi, public MessageHandle
{
public:
	ParserInsight();
	virtual ~ParserInsight();

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


///Insight �ӿ�
public:
	/**
	* ���Ͷ�����������˻ظ���Ϣ���鿴�Ƿ��ĳɹ�
	* @param[in] data_stream
	*/
	void OnServiceMessage(const ::com::htsc::mdc::insight::model::MarketDataStream& data_stream) override;

	/**
	* �����ĺ����͵�ʵʱ��������
	* @param[in] data
	*/
	void OnMarketData(const com::htsc::mdc::insight::model::MarketData& data) override;

	/**
	* �����ѯ���󷵻ؽ��
	* @param[in] MDQueryResponse ��ѯ���󷵻ؽ��
	*/
	void OnQueryResponse(const ::com::htsc::mdc::insight::model::MDQueryResponse& response) override;

	/**
	* ��¼�ɹ�
	*/
	void OnLoginSuccess() override;

	/**
	* ��¼ʧ��
	*/
	void OnLoginFailed(int error_no, const std::string& message) override;

	/**
	* �������з��������޷����ӵ����
	*/
	void OnNoConnections() override;

	/**
	* ��������ʱ
	*/
	void OnReconnect() override;

private:
	/*
	 *	���͵�¼����
	 */
	void DoLogin();
	/*
	 *	����Ʒ������
	 */
	void DoSubscribeMD();

private:
	uint32_t			m_uTradingDate;
	LoginStatus			m_loginState;
	ClientInterface*	m_pUserAPI;

	std::string			m_strHost;
	int32_t				m_iPort;;

	std::string			m_strUser;
	std::string			m_strPass;

	std::string			m_strFlowDir;  // ����log
	std::string			m_strCertDir;  // �ܳ��ļ�
	
	// �����Ӷ����ص�ַ��Ϊ����
	std::string			m_strFontBackup;  // ��ip:port;ip:port������ʽ���ַ���

	uint32_t			m_iService;  // ����߳���

	int					m_iValue;

	bool				m_bLogin;		//��¼ 
	bool				m_bNoConns;		//��������
	bool				m_bReconnect;  // �Ƿ�����
	int					m_iReconnect;  // ������
	bool				m_bySource;		//�Ƿ��������Դ����
	bool				m_byID;			//�Ƿ����ID����
	bool				m_byFundType;
	bool				m_byStockType;
	//bool				m_has_order;	
	bool				m_has_transaction_order;//�Ƿ�����������
	bool				m_has_tick;		//�Ƿ����tick

	CodeSet				m_fitSHSubs;
	CodeSet				m_fitSZSubs;

	int					m_iRequestID;

	IParserSpi*			m_sink;
	IBaseDataMgr*		m_pBaseDataMgr;

	DllHandle		m_hInst;
};

