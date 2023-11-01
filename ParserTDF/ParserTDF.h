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

	//////////////////////////////////////////////////////////////////////////
public:
	static void RecvData(THANDLE hTdf, TDF_MSG* pMsgHead);

	static void RecvSys(THANDLE hTdf, TDF_MSG* pSysMsg);

private:
	/*
	 *	���͵�¼����
	 */
	void DoLogin();
	/*
	 *	����Ʒ������
	 */
	void SubscribeMDs(bool isAll = false);

private:
	uint32_t			m_uTradingDate;

	LoginStatus			m_loginState;
	THANDLE				m_pUserAPI;

	TDF_OPEN_SETTING_EXT	m_OpenSetting;  // ��¼��������

	int8_t				m_intTime;  // -1 = ��ͷ���䣬0 = �������飬��֧�ִ�ͷ����
	int8_t				m_intBroadcast;  // �Ƿ�ΪTDF�ַ�����Ĭ��Ϊ0

	std::string			m_strLogdir;  // ��־Ŀ¼

	bool				m_bProxy;  // �Ƿ񿪴���
	int8_t				m_untProxyType;  // 0 = SOCK4, 1 = SOCK4A, 2 = SOCK5, 3 = HTTP1.1
	std::string			m_strProxyAddr;
	int					m_intProxyPort;
	std::string			m_strProxyUser;
	std::string			m_strProxyPass;

	std::string			m_strCodes;  // ��������

	// ���ĵ���������
	bool				m_bHasOrder;
	bool				m_bHasOrdQue;
	bool				m_bHasTrans;

	// ���ĵĺ�Լ����
	bool				m_bHasIndex;  // ָ��
	bool				m_bHasShare;  // ��Ʊ
	bool				m_bHasOption;  // ��Ȩ
	bool				m_bHasBond;  // ծȯ
	bool				m_bHasFuture;  // �ڻ�
	bool				m_bHasFund;  // ����

	std::string			m_strSubMarkets;  // �����г� SH-1-0=�嵵������� SH-2-0=ʮ���������+���+ί�ж���; SH-1-1=�嵵��Ȩ���գ�SZͬ CF SHF

	bool				m_bLogined;

	CodeSet				m_filterSubs;
	CodeSet				m_ignoreCodes;

	DllHandle			m_hInstTDF;

public:
	static IParserSpi*			m_parserSink;
	static IBaseDataMgr*		m_pBaseDataMgr;
};

