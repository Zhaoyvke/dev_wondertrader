/*!
 * \file ParserTDF.h
 * \project	WonderTrader
 *
 * \author Suuuuuuuuunseeker
 * \date 2023/07/26
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
#ifndef BOOL
typedef int BOOL;
#endif
#endif

#include "../Includes/IParserApi.h"
#include "../Share/DLLHelper.hpp"
#include "../API/tdf_20220831/include/TDFAPI.h"
#include "../API/tdf_20220831/include/TDCore.h"
#include "../API/tdf_20220831/include/TDFAPIInner.h"
#include "../API/tdf_20220831/include/NonMDMsgDecoder.h"
#include "../API/tdf_20220831/include/TDNonMarketStruct.h"


NS_WTP_BEGIN
class WTSTickData;
NS_WTP_END

USING_NS_WTP;

class ParserTDF : public IParserApi
{
public:
	ParserTDF();
	virtual ~ParserTDF();

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

	//////////////////////////////////////////////////////////////////////////
public:
	static void RecvData(THANDLE hTdf, TDF_MSG* pMsgHead);

	static void RecvSys(THANDLE hTdf, TDF_MSG* pSysMsg);

private:
	/*
	 *	发送登录请求
	 */
	void DoLogin();
	/*
	 *	订阅品种行情
	 */
	void SubscribeMDs(bool isAll = false);

private:
	uint32_t			m_uTradingDate;

	LoginStatus			m_loginState;
	THANDLE				m_pUserAPI;

	TDF_OPEN_SETTING_EXT	m_OpenSetting;  // 登录配置容器

	int8_t				m_intTime;  // -1 = 从头传输，0 = 最新行情，多活不支持从头传输
	int8_t				m_intBroadcast;  // 是否为TDF分发服务，默认为0

	std::string			m_strLogdir;  // 日志目录

	bool				m_bProxy;  // 是否开代理
	int8_t				m_untProxyType;  // 0 = SOCK4, 1 = SOCK4A, 2 = SOCK5, 3 = HTTP1.1
	std::string			m_strProxyAddr;
	int					m_intProxyPort;
	std::string			m_strProxyUser;
	std::string			m_strProxyPass;

	std::string			m_strCodes;  // 订阅数量

	// 订阅的数据类型
	bool				m_bHasOrder;
	bool				m_bHasOrdQue;
	bool				m_bHasTrans;

	// 订阅的合约类型
	bool				m_bHasIndex;  // 指数
	bool				m_bHasShare;  // 股票
	bool				m_bHasOption;  // 期权
	bool				m_bHasBond;  // 债券
	bool				m_bHasFuture;  // 期货
	bool				m_bHasFund;  // 基金

	std::string			m_strSubMarkets;  // 订阅市场 SH-1-0=五档行情快照 SH-2-0=十档行情快照+逐笔+委托队列; SH-1-1=五档期权快照；SZ同 CF SHF

	bool				m_bLogined;

	CodeSet				m_filterSubs;
	CodeSet				m_ignoreCodes;

	DllHandle			m_hInstTDF;

public:
	static IParserSpi*			m_parserSink;
	static IBaseDataMgr*		m_pBaseDataMgr;
};

