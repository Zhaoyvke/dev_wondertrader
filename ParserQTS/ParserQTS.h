#pragma once
/*!
 * \file ParserOTS.h
 * \project	WonderTrader
 *
 * \author kanavi
 * \date 2023/08/30
 *
 * \brief
 */
#include "../API/GTAQTSApi/include/GTAQTSInterfaceBase.h"
 //#include "CallBackBase.h"

#include <stdio.h>
//#include "IoService.h"

//#include "OutputHeader.hpp"
#include <map>
#include <string>
#include "../Includes/IParserApi.h"
#include "../Share/DLLHelper.hpp"

#if _WIN32
#ifndef WINAPI
#define WINAPI __stdcall
#endif
#ifndef BOOL
typedef int BOOL;
#endif
#endif
//跨平台的休眠函数统一定义
#ifdef WIN32

#define  SLEEP(t)  Sleep((t)*1000)
#else
#include <unistd.h>
#define  SLEEP(t)  sleep(t)
#endif
#ifdef  _WIN32
#pragma comment(lib,"./DebugLib/Win64/GTAQTSApi.lib")
#endif //  _WIN32
NS_WTP_BEGIN
class WTSTickData;
NS_WTP_END

USING_NS_WTP;
class ParserQTS :public IParserApi, public IGTAQTSCallbackBase
{
public:
	ParserQTS();
	virtual~ParserQTS();

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


	//////////////////////////////////////////////////////////////////////////
public:
	/// 连接状态返回，连接成功/失败
	/// @param  msgType     -- 消息类型
	/// @param  errCode     -- 失败原因，成功时返回0 
	///                     详见《实时行情系统V2.X 用户手册》5.5返回码含义列表RetCode 章节 
	virtual void OnConnectionState(MsgType msgType, RetCode errCode);

	//////////////////////////////////////  上交所  ////////////////////////////////
	/// 上交所L1实时行情快照订阅数据实时回调接口
	/// @param  RealSSEL1Quotation  -- 实时数据
	virtual void OnSubscribe_SSEL1_Quotation(const SSEL1_Quotation& RealSSEL1Quotation);
private:
	/*
	 *	发送登录请求
	 */
	void DoLogin();
	/*
	 *	订阅品种行情
	 */
	void SubscribeMDs(bool isAll = false);

	//订阅的数据类型
private:

	std::string			m_strSHSTKCodes;  // 上交所订阅股票合约
	std::string			m_strSZSTKCodes;  // 深交所订阅股票合约
	std::string			m_strSHOptCodes;  // 期权
	std::string			m_strSZOptCodes;
	std::string			m_strSHETFCodes;  // ETF
	std::string			m_strSZETFCodes;
	std::string			m_strSHBondCodes;  // 债券
	std::string			m_strSZBondCodes;
	std::string			m_strSHINDXCodes;  // 指数
	std::string			m_strSZINDXCodes;

	bool				m_bHasSSE;  // 是否订阅上交所
	bool				m_bHasSZSE;  // 是否订阅深交所数据
	bool				m_bHasL1;	//L1数据
	bool				m_bHasL2;	//L2数据

	//订阅的合约类型
	bool				m_bHasIndex;  // 指数
	bool				m_bHasShare;  // 股票
	bool				m_bHasOption;  // 期权
	bool				m_bHasBond;  // 债券
	bool				m_bHasETF;

private:
	WTSVariant*			m_Servers;
	std::string			m_strHost;
	int32_t				m_iPort;

	std::string			m_strUser;
	std::string			m_strPass;
	std::string			m_strNetType;
	std::string			m_strLogdir;
	int32_t				m_iHeatBeat;
	bool				m_bLogined;

	int					iCode;
	DllHandle			m_hInstQTS;
private:
	IGTAQTSApiBase*		m_pUserAPI;
	IParserSpi*			m_sink;
	IBaseDataMgr*		m_pBaseDataMgr;
};

