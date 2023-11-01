#pragma once

/*!
 * \file ParserAresClt.h
 * \project	WonderTrader
 *
 * \author Suuuuunseeker
 * \date 2022/08/10
 *
 * \brief
 */
#pragma once
#include "../Includes/IParserApi.h"
#include "../Share/DLLHelper.hpp"
#include "../API/AresClt/include/IAresCltApi.h"
#include <map>

NS_WTP_BEGIN
class WTSTickData;
NS_WTP_END

USING_NS_WTP;


class ParserAresClt : public IParserApi, public IAresCltSpi
{
public:
	ParserAresClt();
	virtual ~ParserAresClt();

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


	//IAresCltSpi �ӿ�
public:
	//֪ͨ���г����ں�ʱ��
	virtual		void			OnMarketTime(AClt_Market cMarket, tagAClt_MarketField*);
	//֪ͨ���������
	virtual		void			OnMarketData(AClt_Market cMarket, tagAClt_QuoteField*);

public:
	void						FreeApi(IAresCltApi* p);

private:
	LoginStatus			m_loginState;
	IAresCltApi*		m_pUserAPI;

	std::string			m_strUserID;
	std::string			m_strFlowDir;

	CodeSet				m_filterSubs;

	int					m_iRequestID;

	tagAClt_MarketField*	m_oMarketTime;

	IParserSpi*			m_sink;
	IBaseDataMgr*		m_pBaseDataMgr;

	std::string				m_strModule;
	DllHandle				m_hInstAres;

	typedef void*	(*pfnCreateObj)(const char*, const char*);
	typedef void(*pfnDestroyObj)(void*);

	pfnCreateObj		m_pfnCreateObj;
	pfnDestroyObj		m_pfnDestroyObj;
};


