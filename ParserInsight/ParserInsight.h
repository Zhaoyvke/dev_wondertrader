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

	//IQuoteParser 接口
public:
	virtual bool init(WTSVariant* config) override;

	virtual void release() override;

	virtual bool connect() override;

	virtual bool disconnect() override;

	virtual bool isConnected() override;

	virtual void subscribe(const CodeSet &vecSymbols) override;
	virtual void unsubscribe(const CodeSet &vecSymbols) override;

	virtual void registerSpi(IParserSpi* listener) override;


///Insight 接口
public:
	/**
	* 发送订阅请求后服务端回复消息，查看是否订阅成功
	* @param[in] data_stream
	*/
	void OnServiceMessage(const ::com::htsc::mdc::insight::model::MarketDataStream& data_stream) override;

	/**
	* 处理订阅后推送的实时行情数据
	* @param[in] data
	*/
	void OnMarketData(const com::htsc::mdc::insight::model::MarketData& data) override;

	/**
	* 处理查询请求返回结果
	* @param[in] MDQueryResponse 查询请求返回结果
	*/
	void OnQueryResponse(const ::com::htsc::mdc::insight::model::MDQueryResponse& response) override;

	/**
	* 登录成功
	*/
	void OnLoginSuccess() override;

	/**
	* 登录失败
	*/
	void OnLoginFailed(int error_no, const std::string& message) override;

	/**
	* 处理所有服务器都无法连接的情况
	*/
	void OnNoConnections() override;

	/**
	* 发生重连时
	*/
	void OnReconnect() override;

private:
	/*
	 *	发送登录请求
	 */
	void DoLogin();
	/*
	 *	订阅品种行情
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

	std::string			m_strFlowDir;  // 本地log
	std::string			m_strCertDir;  // 密匙文件
	
	// 务必添加多网关地址作为备用
	std::string			m_strFontBackup;  // 以ip:port;ip:port这种形式的字符串

	uint32_t			m_iService;  // 添加线程数

	int					m_iValue;

	bool				m_bLogin;		//登录 
	bool				m_bNoConns;		//不重连？
	bool				m_bReconnect;  // 是否重连
	int					m_iReconnect;  // 重连数
	bool				m_bySource;		//是否采用行情源连接
	bool				m_byID;			//是否采用ID连接
	bool				m_byFundType;
	bool				m_byStockType;
	//bool				m_has_order;	
	bool				m_has_transaction_order;//是否加入逐笔数据
	bool				m_has_tick;		//是否加入tick

	CodeSet				m_fitSHSubs;
	CodeSet				m_fitSZSubs;

	int					m_iRequestID;

	IParserSpi*			m_sink;
	IBaseDataMgr*		m_pBaseDataMgr;

	DllHandle		m_hInst;
};

