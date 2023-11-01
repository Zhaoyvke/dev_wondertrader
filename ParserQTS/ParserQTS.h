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
//��ƽ̨�����ߺ���ͳһ����
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
	/// ����״̬���أ����ӳɹ�/ʧ��
	/// @param  msgType     -- ��Ϣ����
	/// @param  errCode     -- ʧ��ԭ�򣬳ɹ�ʱ����0 
	///                     �����ʵʱ����ϵͳV2.X �û��ֲᡷ5.5�����뺬���б�RetCode �½� 
	virtual void OnConnectionState(MsgType msgType, RetCode errCode);

	//////////////////////////////////////  �Ͻ���  ////////////////////////////////
	/// �Ͻ���L1ʵʱ������ն�������ʵʱ�ص��ӿ�
	/// @param  RealSSEL1Quotation  -- ʵʱ����
	virtual void OnSubscribe_SSEL1_Quotation(const SSEL1_Quotation& RealSSEL1Quotation);
private:
	/*
	 *	���͵�¼����
	 */
	void DoLogin();
	/*
	 *	����Ʒ������
	 */
	void SubscribeMDs(bool isAll = false);

	//���ĵ���������
private:

	std::string			m_strSHSTKCodes;  // �Ͻ������Ĺ�Ʊ��Լ
	std::string			m_strSZSTKCodes;  // ������Ĺ�Ʊ��Լ
	std::string			m_strSHOptCodes;  // ��Ȩ
	std::string			m_strSZOptCodes;
	std::string			m_strSHETFCodes;  // ETF
	std::string			m_strSZETFCodes;
	std::string			m_strSHBondCodes;  // ծȯ
	std::string			m_strSZBondCodes;
	std::string			m_strSHINDXCodes;  // ָ��
	std::string			m_strSZINDXCodes;

	bool				m_bHasSSE;  // �Ƿ����Ͻ���
	bool				m_bHasSZSE;  // �Ƿ����������
	bool				m_bHasL1;	//L1����
	bool				m_bHasL2;	//L2����

	//���ĵĺ�Լ����
	bool				m_bHasIndex;  // ָ��
	bool				m_bHasShare;  // ��Ʊ
	bool				m_bHasOption;  // ��Ȩ
	bool				m_bHasBond;  // ծȯ
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

