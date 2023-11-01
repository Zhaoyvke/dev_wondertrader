// dllmain.cpp : 定义 DLL 应用程序的入口点。

/*!
 * \file ParserOTS.h
 * \project	WonderTrader
 *
 * \author kanavi
 * \date 2023/08/30
 *
 * \brief
 */

#include "ParserQTS.h"

#include "../Share/StrUtil.hpp"
#include "../Share/TimeUtils.hpp"
#include "../Share/StdUtils.hpp"
#include "../Share/DLLHelper.hpp"
#include "../Share/charconv.hpp"

#include "../Includes/WTSDataDef.hpp"
#include "../Includes/WTSContractInfo.hpp"
#include "../Includes/WTSVariant.hpp"
#include "../Includes/IBaseDataMgr.h"

#include <boost/filesystem.hpp>
#include <iostream>

#include "../Share/fmtlib.h"
template<typename... Args>
inline void write_log(IParserSpi* sink, WTSLogLevel ll, const char* format, const Args&... args)
{
	if (sink == NULL)
		return;

	static thread_local char buffer[512] = { 0 };
	fmtutil::format_to(buffer, format, args...);

	sink->handleParserLog(ll, buffer);
}

extern "C"
{
	EXPORT_FLAG IParserApi* createParser()
	{
		ParserQTS* parser = new ParserQTS();
		return parser;
	}

	EXPORT_FLAG void deleteParser(IParserApi* &parser)
	{
		if (NULL != parser)
		{
			delete parser;
			parser = NULL;
		}
	}
};
ParserQTS::ParserQTS()
	: m_pUserAPI(NULL)
	, iCode(-1)
{
}

ParserQTS::~ParserQTS()
{
	if (m_pUserAPI)
		IGTAQTSApiBase::ReleaseInstance(m_pUserAPI);
	cout << "ReleaseAPIInstance OK,Exit... " << endl;
}

bool ParserQTS::init(WTSVariant * config)
{
	m_strHost = config->getCString("host");
	m_iPort = config->getInt32("port");
	m_strUser = config->getCString("user");
	m_strPass = config->getCString("pass");
	m_iHeatBeat = config->getInt32("heartbeat");
	//登陆的网络类型
	// NetType表示连接网络类型，可选值为0，1.
	// 0表示公网接入(默认值)，1表示特殊网络接入（内网、专线等），如："NetType=1".
	// 改变NetType值重新登录可实现不同的网络环境接入切换
	m_strNetType = config->getCString("NetType");



	iCode = m_pUserAPI->LoginX(m_strUser.c_str(), m_strPass.c_str(), m_strNetType.c_str());


	return true;
}

void ParserQTS::release()
{
	disconnect();
}

bool ParserQTS::connect()
{
	cout << "connect" << endl;
	ParserQTS			subMsgCallInstance;
	IGTAQTSApiBase* m_pUserAPI = IGTAQTSApiBase::CreateInstance(subMsgCallInstance);
	if (NULL == m_pUserAPI)
	{//创建失败
		cout << "fail to CreateInstance of  IGTAQTSApiBase,and return " << endl;
		return false;
	}
	//m_pUserAPI = IGTAQTSApiBase::CreateInstance(*this);

	m_pUserAPI->SetTimeout(30);  // 超时30s
	int iRetCode = -1;
	iRetCode = m_pUserAPI->RegisterService((m_strHost).c_str(), m_iPort);
	if (iRetCode == 0)
	{
		write_log(m_sink, LL_DEBUG, "[ParserQTS] Register Service Success!");
	}
	else
	{
		//release();
		system("pause");
		write_log(m_sink, LL_DEBUG, "Error: [ParserQTS] Register Service Fail!");
		return false;
	}

	DoLogin();
	return true;
}

bool ParserQTS::disconnect()
{
	if (m_pUserAPI)
	{
		//ios.Stop();//退出工作线程
		IGTAQTSApiBase::ReleaseInstance(m_pUserAPI);
	}
	return false;
}

bool ParserQTS::isConnected()
{
	return m_pUserAPI != NULL;
}

void ParserQTS::subscribe(const CodeSet & vecSymbols)
{
	cout << "subsribe" << endl;
}

void ParserQTS::unsubscribe(const CodeSet & vecSymbols)
{
}

void ParserQTS::registerSpi(IParserSpi * listener)
{
	cout << "registerSpi" << endl;
	m_sink = listener;
	

}

void ParserQTS::SetIoService(ParserQTS* ios)
{

	//m_ios = ios;
}

void ParserQTS::DoLogin()
{
	cout << "DoLogin" << endl;
	if (0 != iCode)
	{//连接行情服务器失败
		//cout << "fail to LoginX, ErrorCode: " << iCode << endl;
		write_log(m_sink, LL_ERROR, "Error: [ParserQTS] fail to LoginX, ErrorCode:{}", iCode);
		release();
		system("pause");
		return;
	}
	write_log(m_sink, LL_DEBUG, " [ParserQTS]Login Success !");
}

void ParserQTS::SubscribeMDs(bool isAll)
{
	cout << "SubscribeMDs" << endl;
}
