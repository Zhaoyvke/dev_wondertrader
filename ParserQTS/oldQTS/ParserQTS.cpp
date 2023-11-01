// dllmain.cpp : ���� DLL Ӧ�ó������ڵ㡣

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
	//��½����������
	// NetType��ʾ�����������ͣ���ѡֵΪ0��1.
	// 0��ʾ��������(Ĭ��ֵ)��1��ʾ����������루������ר�ߵȣ����磺"NetType=1".
	// �ı�NetTypeֵ���µ�¼��ʵ�ֲ�ͬ�����绷�������л�
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
	{//����ʧ��
		cout << "fail to CreateInstance of  IGTAQTSApiBase,and return " << endl;
		return false;
	}
	//m_pUserAPI = IGTAQTSApiBase::CreateInstance(*this);

	m_pUserAPI->SetTimeout(30);  // ��ʱ30s
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
		//ios.Stop();//�˳������߳�
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
	{//�������������ʧ��
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
