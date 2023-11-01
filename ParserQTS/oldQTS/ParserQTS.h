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


public:
	virtual bool init(WTSVariant* config) override;

	virtual void release() override;

	virtual bool connect() override;

	virtual bool disconnect() override;

	virtual bool isConnected() override;

	virtual void subscribe(const CodeSet &vecSymbols) override;
	virtual void unsubscribe(const CodeSet &vecSymbols) override;

	virtual void registerSpi(IParserSpi* listener) override;



protected:

	void SetIoService(ParserQTS *ios);
private:

	void DoLogin();

	void SubscribeMDs(bool isAll = false);


private:
	IGTAQTSApiBase*		m_pUserAPI;


	std::string			m_strHost;
	int32_t				m_iPort;
	IParserSpi*			m_sink;
	IBaseDataMgr*		m_pBaseDataMgr;

	std::string			m_strUser;
	std::string			m_strPass;
	std::string			m_strNetType;
	int32_t				m_iHeatBeat;

	int					iCode;


};

