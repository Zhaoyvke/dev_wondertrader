/*!
 * \file ParserQTS.cpp
 * \project	WonderTrader
 *
 * \author Sunnseeeker
 * \date 2023/08/29
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
#include "../Includes/IBaseDataMgr.h"

#include <boost/filesystem.hpp>
#include <iostream>

#ifndef FLT_MAX
#define FLT_MAX 3.402823466e+38F
#endif

#ifdef _WIN32
#ifdef _WIN64
#pragma comment(lib, "../API/GTAQTSApi/x64/GTAQTSApi.lib")
#else
#pragma comment(lib, "../API/GTAQTSApi/x86/GTAQTSApi.lib")
#endif
#include <wtypes.h>
HMODULE	g_dllModule = NULL;

BOOL APIENTRY DllMain(
	HANDLE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		g_dllModule = (HMODULE)hModule;
		break;
	}
	return TRUE;
}
#else
#include <dlfcn.h>

std::string	g_moduleName;

__attribute__((constructor))
void on_load(void) {
	Dl_info dl_info;
	dladdr((void *)on_load, &dl_info);
	g_moduleName = dl_info.dli_fname;
}
#endif

//By Wesley @ 2022.01.05
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


std::string getBinDir()
{
	static std::string _bin_dir;
	if (_bin_dir.empty())
	{


#ifdef _WIN32
		char strPath[MAX_PATH];
		GetModuleFileName(g_dllModule, strPath, MAX_PATH);

		_bin_dir = StrUtil::standardisePath(strPath, false);
#else
		_bin_dir = g_moduleName;
#endif

		uint32_t nPos = _bin_dir.find_last_of('/');
		_bin_dir = _bin_dir.substr(0, nPos + 1);
	}

	return _bin_dir;
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

/*! 
* TDF �۸�ʽ���� 
* ��Լ�۸� = ��ԭʼ�۸� + 0.00005�� * 10000
* ��Լ�ɽ���Ҳ�ǷŴ���10000����õ��Ľ��
* ָ���ɽ��λ��100Ԫ
*/
inline double checkValid(__int64 val)
{
	double price = (double)val / 10000;
	//price -= 0.00005;

	if (price == DBL_MAX)
		return 0;

	return price;
}

inline ContractType extractType(std::string& code, bool isSH)
{
	if (code.size() < 6)
		return M_UNKNOWN;

	char tp = code[0];
	switch (tp)
	{
	case '0':
	{
		if (isSH)
		{
			if (code.compare(0, 3, "000") == 0)
				return M_INDX;
			else
				return M_BOND;
		}
		else
			return M_STK;
	}
	break;
	case '1':
	{
		if (!isSH)
		{
			if (code.compare(0, 6, "159000") > 0)
				return M_ETF;
			else
				return M_BOND;
		}
		else
		{
			if (code.size() == 6)
				return M_BOND;
			else if (code.size() == 8)
				return M_OPT;
		}
		return M_UNKNOWN;
	}
	break;
	case '3':
	{
		if (!isSH)
		{
			if (code[1] == '9')
				return M_INDX;
			else
				return M_STK;
		}
		return M_FUT;
	}
	break;
	case '5':
	{
		return M_ETF;
	}
	break;
	case '6':
	{
		return M_STK;
	}
	case '9':
	{
		if (!isSH)
			return M_OPT;

		return M_UNKNOWN;
	}
	break;
	default:
	{
		return M_UNKNOWN;
	}
	break;
	}
}

inline const char* toStdExchg(const char* wdExchg)
{
	if (strcmp(wdExchg, "SH") == 0)
		return "SSE";
	else if (strcmp(wdExchg, "SZ") == 0)
		return "SZSE";
	else if (strcmp(wdExchg, "CF") == 0)
		return "CFFEX";
	else if (strcmp(wdExchg, "SHF") == 0)
		return "SHFE";
	else if (strcmp(wdExchg, "CZC") == 0)
		return "CZCE";
	else if (strcmp(wdExchg, "DCE") == 0)
		return "DCE";
	else
		return "";
}

inline const char* toTDFExchg(const char* stdExchg)
{
	if (strcmp(stdExchg, "SSE") == 0)
		return "SH";
	else if (strcmp(stdExchg, "SZSE") == 0)
		return "SZ";
	else if (strcmp(stdExchg, "CFFEX") == 0)
		return "CF";
	else if (strcmp(stdExchg, "SHFE") == 0)
		return "SHF";
	else if (strcmp(stdExchg, "CZCE") == 0)
		return "CZC";
	else if (strcmp(stdExchg, "DCE") == 0)
		return "DCE";
	else 
		return "";
}

IParserSpi*		ParserQTS::m_parserSink = NULL;
IBaseDataMgr*	ParserQTS::m_pBaseDataMgr = NULL;

ParserQTS::ParserQTS()
	: m_pUserAPI(NULL)
	, m_bLogined(false)
	, m_Servers(NULL)
	, m_bHasIndex(false)
	, m_bHasShare(false)
	, m_bHasETF(false)
	, m_bHasBond(false)
	, m_bHasFuture(false)
	, m_bHasOption(false)
	, m_bHasOrder(false)
	, m_bHasTrans(false)
	, m_bHasOrdQue(false)
{

}


ParserQTS::~ParserQTS()
{
	release();
}

bool ParserQTS::init(WTSVariant* config)
{
	m_Servers = config->get("servers");

	m_strUser = config->getCString("user");
	m_strPass = config->getCString("pass");

	m_strNetType = config->getCString("net_type");

	m_bHasOrder = config->getBoolean("has_order");
	m_bHasTrans = config->getBoolean("has_trans");
	m_bHasOrdQue = config->getBoolean("has_orderqueue");

	m_bHasIndex = config->getBoolean("has_index");
	m_bHasShare = config->getBoolean("has_share");
	m_bHasETF = config->getBoolean("has_fund");
	m_bHasBond = config->getBoolean("has_bond");
	m_bHasFuture = config->getBoolean("has_future");
	m_bHasOption = config->getBoolean("has option");

	m_strLogdir = "log";  // ����ָ��ΪlogĿ¼������API��־������Ŀ¼�ᱨ��

	{
		std::string path = StrUtil::standardisePath(m_strLogdir);
		if (!StdFile::exists(path.c_str()))
			boost::filesystem::create_directories(path.c_str());
	}

	std::string module = config->getCString("qtsmodule");
	if (module.empty())
	{
		module = "GTAQTSApi";
	}

	std::string dllpath = getBinDir() + DLLHelper::wrap_module(module.c_str(), "");
	m_hInstQTS = DLLHelper::load_library(dllpath.c_str());

	SubscribeMDs(false);

	return true;
}

void ParserQTS::release()
{
	disconnect();
}

bool ParserQTS::connect()
{
	// ������Ϣ�ص���
	//IGTAQTSCallbackBase m_CallbackBase;

	// ��������API����
	m_pUserAPI = IGTAQTSApiBase::CreateInstance(*this);

	m_pUserAPI->SetTimeout(30);  // ��ʱ30s

	// ע�ᣬע��FENS��ַ
	bool m_bConnected = false;
	if (m_Servers)
	{
		for (uint32_t i = 0; i < m_Servers->size(); i++)
		{
			WTSVariant* cfgItem = m_Servers->get(i);

			int ret = m_pUserAPI->RegisterService(cfgItem->getCString("host"), cfgItem->getUInt32("port"));
			m_bConnected = m_bConnected || (ret == 0);
		}
	}

	if (m_bConnected)
		write_log(m_parserSink, LL_INFO, "[PaserQTS] Register Service Success!");
	else
	{
		write_log(m_parserSink, LL_ERROR, "[PaserQTS] Register Service Fail!");
		return false;
	}

	DoLogin();

	return true;
}

bool ParserQTS::disconnect()
{
	if(m_pUserAPI)
	{
		IGTAQTSApiBase::ReleaseInstance(m_pUserAPI);
		m_pUserAPI = NULL;
	}

	return true;
}

void ParserQTS::DoLogin()
{
	do 
	{
		// ��¼��ͨ���û������������������¼
		RetCode ret = m_pUserAPI->LoginX(m_strUser.c_str(), m_strPass.c_str(), m_strNetType.c_str());

		if (Ret_Success != ret)
		{
			write_log(m_parserSink, LL_ERROR, "[ParserQTS] {} Login fail: {}", m_strUser, ret);
			IGTAQTSApiBase::ReleaseInstance(m_pUserAPI);

			m_parserSink->handleEvent(WPE_Connect, -1);

			return;
		}
		else
		{
			write_log(m_parserSink, LL_INFO, "[ParserQTS] {} Login success !", m_strUser);

			m_bLogined = true;
			if (m_parserSink)
			{
				m_parserSink->handleEvent(WPE_Connect, 0);
				m_parserSink->handleEvent(WPE_Login, 0);
			}
		}
	} while (false);
}

void ParserQTS::SubscribeMDs(bool isAll /* = false */)
{
	int iCode;
	if (m_bHasSHL1)
	{
		//////////====>�Ͻ���L1ʵʱ���鶩�ģ�������Ʊ�ͻ���
		std::string codes;
		if (m_bHasShare)
			codes += m_strSHSTKCodes;

		if (m_bHasETF)
		{
			if (codes.size() > 0)
				codes += ",";
			codes += m_strSHETFCodes;
		}

		iCode = m_pUserAPI->Subscribe(Msg_SSEL1_Quotation, (char*)codes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] [ParserQTS] Fail to Subscribe,Msg_SSEL1_Quotation, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] [ParserQTS] Subscribe  Msg_SSEL1_Quotation Success,Start to Receive Data...");
		}
	}

	//////////====>�Ͻ���L1�̺�̶��۸����鶩��
	if (m_bHasSHL1 && m_bHasFp)
	{
		std::string codes;
		if (m_bHasShare)
			codes += m_strSHSTKCodes;

		if (m_bHasETF)
		{
			if (codes.size() > 0)
				codes += ",";
			codes += m_strSHETFCodes;
		}

		iCode = m_pUserAPI->Subscribe(Msg_SSEL1_FpQuotation, (char*)codes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,Msg_SSEL1_FpQuotation, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  Msg_SSEL1_FpQuotation Success,Start to Receive Data ...");
		}
	}

	////////====>�Ͻ���L1ծȯʵʱ���鶩��
	if (m_bHasBond && m_bHasSHL1)
	{
		iCode = m_pUserAPI->Subscribe(Msg_SSEL1_BondQuotation, (char*)m_strSHBondCodes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,Msg_SSEL1_BondQuotation, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  Msg_SSEL1_BondQuotation Success,Start to Receive Data ...");
		}
	}

	////////====>�Ͻ���������ȨL1ʵʱ���鶩��
	if (m_bHasOption && m_bHasSHL1)
	{
		iCode = m_pUserAPI->Subscribe(Msg_SSEIOL1_Quotation, (char*)m_strSHOptCodes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,Msg_SSEIOL1_Quotation, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  Msg_SSEIOL1_Quotation Success,Start to Receive Data ...");
		}
	}

	//////////====>�Ͻ���L2�̺�̶��۸����鶩��,��δ����
	if (m_bHasSHL2 && m_bHasFp)
	{
		iCode = m_pUserAPI->Subscribe(Msg_SSEL2_FpQuotation, (char*)m_strSHCodes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,Msg_SSEL2_FpQuotation, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  Msg_SSEL2_FpQuotation Success,Start to Receive Data ...");
		}
	}

	//////////====>�Ͻ���L2ETF���ģ���δ����
	//if (m_bHasSHL2 && m_bHasETF)
	//{
	//	iCode = m_pUserAPI->Subscribe(Msg_SSEL2_ETF, (char*)m_strSHETFCodes.c_str());
	//	if (Ret_Success != iCode)
	//	{
	//		write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,Msg_SSEL2_ETF, ErrorCode: {}", iCode);
	//	}
	//	else
	//	{
	//		write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  Msg_SSEL2_ETF Success,Start to Receive Data ...");
	//	}
	//}

	//////====>�Ͻ���L2��ƱETF����������鶩��, Msg_SSEL2_Tick��δ����
	//////////====>�Ͻ���L2����ʵʱ���鶩��
	if (m_bHasSHL2 && m_bHasSnap)
	{
		std::string codes;
		if (m_bHasShare)
			codes += m_strSHSTKCodes;

		if (m_bHasETF)
		{
			if (codes.size() > 0)
				codes += ",";
			codes += m_strSHETFCodes;
		}

		iCode = m_pUserAPI->Subscribe(Msg_SSEL2_ShortQuotation, (char*)codes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,Msg_SSEL2_ShortQuotation, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  Msg_SSEL2_ShortQuotation Success,Start to Receive Data ...");
		}
	}

	////////====>�Ͻ���L2��ƱETFί�ж��ж���
	if (m_bHasSHL2 && m_bHasOrdQue)
	{
		std::string codes;
		if (m_bHasShare)
			codes += m_strSHSTKCodes;

		if (m_bHasETF)
		{
			if (codes.size() > 0)
				codes += ",";
			codes += m_strSHETFCodes;
		}

		iCode = m_pUserAPI->Subscribe(Msg_SSEL2_OrderQueue, (char*)codes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,Msg_SSEL2_OrderQueue, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  Msg_SSEL2_OrderQueue Success,Start to Receive Data ...");
		}
	}

	//////====>�Ͻ���L2��ƱETF��ʳɽ�����
	if (m_bHasSHL2 && m_bHasTrans)
	{
		std::string codes;
		if (m_bHasShare)
			codes += m_strSHSTKCodes;

		if (m_bHasETF)
		{
			if (codes.size() > 0)
				codes += ",";
			codes += m_strSHETFCodes;
		}

		iCode = m_pUserAPI->Subscribe(Msg_SSEL2_Transaction, (char*)codes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,Msg_SSEL2_Transaction, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  Msg_SSEL2_Transaction Success,Start to Receive Data ...");
		}
	}

	////////====>�Ͻ���L2��ƱETF���ί�ж���
	if (m_bHasSHL2 && m_bHasOrder)
	{
		std::string codes;
		if (m_bHasShare)
			codes += m_strSHSTKCodes;

		if (m_bHasETF)
		{
			if (codes.size() > 0)
				codes += ",";
			codes += m_strSHETFCodes;
		}

		iCode = m_pUserAPI->Subscribe(Msg_SSEL2_Order, (char*)codes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,Msg_SSEL2_Order, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  Msg_SSEL2_Order Success,Start to Receive Data ...");
		}
	}

	////////====>�Ͻ���L2ָ�����鶩��
	if (m_bHasSHL2 && m_bHasIndex)
	{
		iCode = m_pUserAPI->Subscribe(Msg_SSEL2_Index, (char*)m_strSHINDXCodes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,Msg_SSEL2_Index, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  Msg_SSEL2_Index Success,Start to Receive Data ...");
		}
	}

	////////====>�Ͻ���L2ծȯʵʱ���鶩��
	if (m_bHasSHL2 && m_bHasBond && m_bHasSnap)
	{
		iCode = m_pUserAPI->Subscribe(Msg_SSEL2_BondQuotation, (char*)m_strSHBondCodes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,Msg_SSEL2_BondQuotation, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  Msg_SSEL2_BondQuotation Success,Start to Receive Data ...");
		}
	}

	////////====>�Ͻ���L2ծȯ������鶩��
	if (m_bHasSHL2 && m_bHasBond && m_bHasOrder && m_bHasTrans)
	{
		iCode = m_pUserAPI->Subscribe(Msg_SSEL2_BondTick, (char*)m_strSHBondCodes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,Msg_SSEL2_BondTick, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  Msg_SSEL2_BondTick Success,Start to Receive Data ...");
		}
	}

	/////////////////////////////  ���  /////////////////////////////////////////////
	////////====>���L1ʵʱ���鶩��
	if (m_bHasSZL1)
	{
		std::string codes;
		if (m_bHasShare)
			codes += m_strSZSTKCodes;

		if (m_bHasETF)
		{
			if (codes.size() > 0)
				codes += ",";
			codes += m_strSZETFCodes;
		}

		iCode = m_pUserAPI->Subscribe(Msg_SZSEL1_Quotation, (char*)codes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,Msg_SZSEL1_Quotation, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  Msg_SZSEL1_Quotation Success,Start to Receive Data ...");
		}
	}

	//////////====>���L1�̺�̶��۸����鶩��
	if (m_bHasSZL1 && m_bHasFp)
	{
		std::string codes;
		if (m_bHasShare)
			codes += m_strSZSTKCodes;

		if (m_bHasETF)
		{
			if (codes.size() > 0)
				codes += ",";
			codes += m_strSZETFCodes;
		}

		iCode = m_pUserAPI->Subscribe(Msg_SZSEL1_FpQuotation, (char*)codes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,Msg_SZSEL1_FpQuotation, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  Msg_SZSEL1_FpQuotation Success,Start to Receive Data ...");
		}
	}

	/////////====>���������ȨL1ʵʱ���鶩��
	if (m_bHasOption && m_bHasSZL1)
	{
		iCode = m_pUserAPI->Subscribe(Msg_SZSEIOL1_Quotation, (char*)m_strSZOptCodes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,SZSEIOL1_Quotation, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  SZSEIOL1_Quotation Success,Start to Receive Data ...");
		}
	}


	////////====>���L2��ʳɽ�����
	if (m_bHasSZL2 && m_bHasTrans)
	{
		std::string codes;
		if (m_bHasShare)
			codes += m_strSZSTKCodes;

		if (m_bHasETF)
		{
			if (codes.size() > 0)
				codes += ",";
			codes += m_strSZETFCodes;
		}

		iCode = m_pUserAPI->Subscribe(Msg_SZSEL2_Transaction, (char*)codes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,Msg_SZSEL2_Transaction, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  Msg_SZSEL2_Transaction Success,Start to Receive Data ...");
		}
	}

	////////====>���L2���ί�ж���
	if (m_bHasSZL2 && m_bHasOrder)
	{
		std::string codes;
		if (m_bHasShare)
			codes += m_strSZSTKCodes;

		if (m_bHasETF)
		{
			if (codes.size() > 0)
				codes += ",";
			codes += m_strSZETFCodes;
		}

		iCode = m_pUserAPI->Subscribe(Msg_SZSEL2_Order, (char*)codes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,SZSEL2_Order, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  SZSEL2_Order Success,Start to Receive Data ...");
		}
	}

	////////====>���L2ָ�����鶩��
	if (m_bHasSZL2 && m_bHasIndex)
	{
		iCode = m_pUserAPI->Subscribe(Msg_SZSEL2_Index, (char*)m_strSZINDXCodes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,SZSEL2_Index, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  SZSEL2_Index Success,Start to Receive Data ...");
		}
	}

	////////====>���L2����ʵʱ���鶩��
	if (m_bHasSZL2 && m_bHasSnap)
	{
		std::string codes;
		if (m_bHasShare)
			codes += m_strSZSTKCodes;

		if (m_bHasETF)
		{
			if (codes.size() > 0)
				codes += ",";
			codes += m_strSZETFCodes;
		}

		iCode = m_pUserAPI->Subscribe(Msg_SZSEL2_ShortQuotation, (char*)codes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,Msg_SZSEL2_ShortQuotation, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  Msg_SZSEL2_ShortQuotation Success,Start to Receive Data ...");
		}
	}

	////////====>���L2ί�ж��ж���
	if (m_bHasSZL2 && m_bHasOrdQue)
	{
		std::string codes;
		if (m_bHasShare)
			codes += m_strSZSTKCodes;

		if (m_bHasETF)
		{
			if (codes.size() > 0)
				codes += ",";
			codes += m_strSZETFCodes;
		}

		iCode = m_pUserAPI->Subscribe(Msg_SZSEL2_OrderQueue, (char*)codes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,Msg_SZSEL2_OrderQueue, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  Msg_SZSEL2_OrderQueue Success,Start to Receive Data ...");
		}
	}

	////////====>���L2�̺�̶��۸����鶩��
	if (m_bHasSZL2 && m_bHasFp)
	{
		std::string codes;
		if (m_bHasShare)
			codes += m_strSZSTKCodes;

		if (m_bHasETF)
		{
			if (codes.size() > 0)
				codes += ",";
			codes += m_strSZETFCodes;
		}

		iCode = m_pUserAPI->Subscribe(Msg_SZSEL2_FpQuotation, (char*)codes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,Msg_SZSEL2_FpQuotation, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  Msg_SZSEL2_FpQuotation Success,Start to Receive Data ...");
		}
	}

	//////////====>���L2������ݶ���
	//if (m_bHasSZL2)
	//{
	//	std::string codes;
	//	if (m_bHasShare)
	//		codes += m_strSZSTKCodes;

	//	if (m_bHasETF)
	//	{
	//		if (codes.size() > 0)
	//			codes += ",";
	//		codes += m_strSZETFCodes;
	//	}

	//	iCode = m_pUserAPI->Subscribe(Msg_SZSEL2_Tick, (char*)codes.c_str());
	//	if (Ret_Success != iCode)
	//	{
	//		write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,Msg_SZSEL2_Tick, ErrorCode: {}", iCode);
	//	}
	//	else
	//	{
	//		write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  Msg_SZSEL2_Tick Success,Start to Receive Data ...");
	//	}
	//}

	//////====>���L2ծȯʵʱ���鶩��
	if (m_bHasSZL2 && m_bHasBond)
	{
		iCode = m_pUserAPI->Subscribe(Msg_SZSEL2_BondQuotation, (char*)m_strSZBondCodes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,Msg_SZSEL2_BondQuotation, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  Msg_SZSEL2_BondQuotation Success,Start to Receive Data ...");
		}
	}

	//////====>���L2ծȯ���ί�ж���
	if (m_bHasSZL2 && m_bHasBond && m_bHasOrder)
	{
		iCode = m_pUserAPI->Subscribe(Msg_SZSEL2_BondOrder, (char*)m_strSZBondCodes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,Msg_SZSEL2_BondOrder, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  Msg_SZSEL2_BondOrder Success,Start to Receive Data ...");
		}
	}

	//////====>���L2ծȯ��ʳɽ�����
	if (m_bHasSZL2 && m_bHasBond && m_bHasTrans)
	{
		iCode = m_pUserAPI->Subscribe(Msg_SZSEL2_BondTransaction, (char*)m_strSZBondCodes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,Msg_SZSEL2_BondTransaction, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  Msg_SZSEL2_BondTransaction Success,Start to Receive Data ...");
		}
	}

	////////====>���L2ծȯ������鶩��
	//string SZSEL2_BondTick_Code_Sub = "131800,131801,131810";
	//iCode = m_pUserAPI->Subscribe(Msg_SZSEL2_BondTick, (char*)SZSEL2_BondTick_Code_Sub.c_str());
	//if (Ret_Success != iCode)
	//{
	//	write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,Msg_SZSEL2_BondTick, ErrorCode: {}", iCode);
	//}
	//else
	//{
	//	write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  Msg_SZSEL2_BondTick Success,Start to Receive Data ...");
	//}

	/////////////////////////////////////   ������   ///////////////////////////////////////////////////
	//������Ҫ�ٶ������

	/////////////////////////////////////   �н���   ///////////////////////////////////////////////////
	//////====>�н���L2ʵʱ���鶩��
	if (m_bHasCFFEXL2)
	{
		iCode = m_pUserAPI->Subscribe(Msg_CFFEXL2_Quotation, (char*)m_strCFFEXFutCodes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,CFFEXL2_Quotation, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  CFFEXL2_Quotation Success,Start to Receive Data ...");
		}
	}

	/////////////////////////////////////   ������   ///////////////////////////////////////////////////
	//////====>������L1ʵʱ���鶩��
	if (m_bHasSHFEL1)
	{
		iCode = m_pUserAPI->Subscribe(Msg_SHFEL1_Quotation, (char*)m_strSHFEFutCodes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,SHFEL1_Quotation, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  SHFEL1_Quotation Success,Start to Receive Data ...");
		}
	}

	//////====>������L2ʵʱ���鶩��
	if (m_bHasSHFEL2)
	{
		iCode = m_pUserAPI->Subscribe(Msg_SHFEL2_Quotation, (char*)m_strSHFEFutCodes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,SHFEL2_Quotation, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  SHFEL2_Quotation Success,Start to Receive Data ...");
		}
	}

	/////////////////////////////////////   ������Դ   ///////////////////////////////////////////////////
	//////====>������ԴL1ʵʱ���鶩��
	if (m_bHasINEL1)
	{
		iCode = m_pUserAPI->Subscribe(Msg_INEL1_Quotation, (char*)m_strINEFutCodes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,INEL1_Quotation, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  INEL1_Quotation Success,Start to Receive Data ...");
		}
	}

	//////====>������ԴL2ʵʱ���鶩��
	if (m_bHasINEL2)
	{
		iCode = m_pUserAPI->Subscribe(Msg_INEL2_Quotation, (char*)m_strINEFutCodes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,INEL2_Quotation, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  INEL2_Quotation Success,Start to Receive Data ...");
		}
	}

	/////////////////////////////////////   ֣����   ///////////////////////////////////////////////////
	//////====>֣����L1ʵʱ���鶩��
	if (m_bHasCZCEL1)
	{
		iCode = m_pUserAPI->Subscribe(Msg_CZCEL1_Quotation, (char*)m_strCZCEFutCodes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,CZCEL1_Quotation, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  CZCEL1_Quotation Success,Start to Receive Data ...");
		}
	}

	////////====>֣����L2ʵʱ���鶩��
	if (m_bHasCZCEL2)
	{
		iCode = m_pUserAPI->Subscribe(Msg_CZCEL2_Quotation, (char*)m_strCZCEFutCodes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,CZCEL2_Quotation, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  CZCEL2_Quotation Success,Start to Receive Data ...");
		}
	}

	//////====>֣����L2�嵵������鶩��
	if (m_bHasCZCE5L)
	{
		iCode = m_pUserAPI->Subscribe(Msg_CZCEL2_LevelQuotation, (char*)m_strCZCEFutCodes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,CZCEL2_LevelQuotation, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  CZCEL2_LevelQuotation Success,Start to Receive Data ...");
		}
	}

	//////====>֣����L1�������鶩��
	if (m_bHasCZCEL1 && m_bHasArb)
	{
		iCode = m_pUserAPI->Subscribe(Msg_CZCEL1_ArbiQuotation, (char*)m_strCZCEFutCodes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,CZCEL1_ArbiQuotation, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  CZCEL1_ArbiQuotation Success,Start to Receive Data ...");
		}
	}

	////////====>֣����L2�������鶩��
	if (m_bHasCZCEL2 && m_bHasArb)
	{
		iCode = m_pUserAPI->Subscribe(Msg_CZCEL2_ArbiQuotation, (char*)m_strCZCEFutCodes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,CZCEL2_ArbiQuotation, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  CZCEL2_ArbiQuotation Success,Start to Receive Data ...");
		}
	}

	////////====>֣����L2�����嵵������鶩��
	if (m_bHasCZCE5L && m_bHasArb)
	{
		iCode = m_pUserAPI->Subscribe(Msg_CZCEL2_ArbiLevelQuotation, (char*)m_strCZCEFutCodes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,CZCEL2_ArbiLevelQuotation, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  CZCEL2_ArbiLevelQuotation Success,Start to Receive Data ...");
		}
	}

	///////////////////////////////////////   ������   /////////////////////////////////////////////////////
	//////====>������L1ʵʱ���鶩��
	if (m_bHasDCEL1)
	{
		iCode = m_pUserAPI->Subscribe(Msg_DCEL1_Quotation, (char*)m_strDCEFutCodes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,DCEL1_Quotation, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  DCEL1_Quotation Success,Start to Receive Data ...");
		}
	}

	//////====>������L1�������鶩��
	if (m_bHasDCEL1 && m_bHasArb)
	{
		string strDCEL1_ArbiQuotation_Sub = "";
		iCode = m_pUserAPI->Subscribe(Msg_DCEL1_ArbiQuotation, (char*)m_strDCEFutCodes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,DCEL1_ArbiQuotation, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  DCEL1_ArbiQuotation Success,Start to Receive Data ...");
		}
	}

	//////====>������L2ʵʱ���鶩��
	if (m_bHasDCEL2)
	{
		string strDCEL2_Quotation_Sub = "";
		iCode = m_pUserAPI->Subscribe(Msg_DCEL2_Quotation, (char*)m_strDCEFutCodes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,DCEL2_Quotation, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  DCEL2_Quotation Success,Start to Receive Data ...");
		}
	}

	//////====>������L2�嵵������鶩��
	if (m_bHasDCE5L)
	{
		string strDCEL2_LevelQuotation_Sub = "";
		iCode = m_pUserAPI->Subscribe(Msg_DCEL2_LevelQuotation, (char*)m_strDCEFutCodes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,DCEL2_LevelQuotation, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  DCEL2_LevelQuotation Success,Start to Receive Data ...");
		}
	}

	//////====>������L2ί�ж��ж���
	if (m_bHasDCEL2 && m_bHasOrdQue)
	{
		string strDCEL2_OrderQueue_Sub = "";
		iCode = m_pUserAPI->Subscribe(Msg_DCEL2_OrderQueue, (char*)m_strDCEFutCodes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,DCEL2_OrderQueue, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  DCEL2_OrderQueue Success,Start to Receive Data ...");
		}
	}

	//////====>������L2ί��ͳ�����鶩��
	if (m_bHasDCEL2 && m_bHasOrder)
	{
		string strDCEL2_OrderStatistic_Sub = "";
		iCode = m_pUserAPI->Subscribe(Msg_DCEL2_OrderStatistic, (char*)m_strDCEFutCodes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,DCEL2_OrderStatistic, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  DCEL2_OrderStatistic Success,Start to Receive Data ...");
		}
	}

	//////====>������L2�ּ۳ɽ������鶩��
	if (m_bHasDCEL2 && m_bHasTrans)
	{
		string strDCEL2_MarchPriceQty_Sub = "";
		iCode = m_pUserAPI->Subscribe(Msg_DCEL2_MarchPriceQty, (char*)m_strDCEFutCodes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,DCEL2_MarchPriceQty, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  DCEL2_MarchPriceQty Success,Start to Receive Data ...");
		}
	}

	//////====>������L2�������鶩��
	if (m_bHasDCEL2 && m_bHasArb)
	{
		string strDCEL2_ArbiQuotation_Sub = "";
		iCode = m_pUserAPI->Subscribe(Msg_DCEL2_ArbiQuotation, (char*)m_strDCEFutCodes.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,DCEL2_ArbiQuotation, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  DCEL2_ArbiQuotation Success,Start to Receive Data ...");
		}
	}

	////////====>������ָ������
	if (m_bHasDCEIndex)
	{
		string strDCE_IndexSub = "";
		iCode = m_pUserAPI->Subscribe(Msg_DCE_Index, (char*)strDCE_IndexSub.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,Msg_DCE_Index, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  Msg_DCE_Index Success,Start to Receive Data ...");
		}
	}

	//======================================  GFEX ������  =============================================================
	//////====>������L1ʵʱ���� ����
	if (m_bHasGFEXL1)
	{
		string GFEXL1_Quotation_Sub_Code = "";
		iCode = m_pUserAPI->Subscribe(Msg_GFEXL1_Quotation, (char*)GFEXL1_Quotation_Sub_Code.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,GFEXL1_Quotation, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  GFEXL1_Quotation Success,Start to Receive Data ...");
		}
	}

	//////====>������L1�������� ����
	if (m_bHasGFEXL1 && m_bHasArb)
	{
		string GFEXL1_ArbiQuotation_Sub_Code = "";
		iCode = m_pUserAPI->Subscribe(Msg_GFEXL1_ArbiQuotation, (char*)GFEXL1_ArbiQuotation_Sub_Code.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,GFEXL1_ArbiQuotation, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  GFEXL1_ArbiQuotation Success,Start to Receive Data ...");
		}
	}

	//////====>������L2ʵʱ���� ����
	if (m_bHasGFEXL2)
	{
		string GFEXL2_Quotation_Sub_Code = "";
		iCode = m_pUserAPI->Subscribe(Msg_GFEXL2_Quotation, (char*)GFEXL2_Quotation_Sub_Code.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,GFEXL2_Quotation, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  GFEXL2_Quotation Success,Start to Receive Data ...");
		}
	}

	//////====>������L2�������� ����
	if (m_bHasGFEXL2 && m_bHasArb)
	{
		string GFEXL2_ArbiQuotation_Sub_Code = "";
		iCode = m_pUserAPI->Subscribe(Msg_GFEXL2_ArbiQuotation, (char*)GFEXL2_ArbiQuotation_Sub_Code.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,GFEXL2_ArbiQuotation, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  GFEXL2_ArbiQuotation Success,Start to Receive Data ...");
		}
	}

	//////====>������L2ί�ж������� ����
	if (m_bHasGFEXL2 && m_bHasOrdQue)
	{
		string GFEXL2_OrderQueue_Sub_Code = "";
		iCode = m_pUserAPI->Subscribe(Msg_GFEXL2_OrderQueue, (char*)GFEXL2_OrderQueue_Sub_Code.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,GFEXL2_OrderQueue, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  GFEXL2_OrderQueue Success,Start to Receive Data ...");
		}
	}

	//////====>������L2ί��ͳ������ ����
	if (m_bHasGFEXL2 && m_bHasOrder)
	{
		string GFEXL2_OrderStatistic_Sub_Code = "";
		iCode = m_pUserAPI->Subscribe(Msg_GFEXL2_OrderStatistic, (char*)GFEXL2_OrderStatistic_Sub_Code.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,GFEXL2_OrderStatistic, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  GFEXL2_OrderStatistic Success,Start to Receive Data ...");
		}
	}

	//////====>������L2�ּ۳ɽ������� ����
	if (m_bHasGFEXL2 && m_bHasTrans)
	{
		string GFEXL2_MarchPriceQty_Sub_Code = "";
		iCode = m_pUserAPI->Subscribe(Msg_GFEXL2_MarchPriceQty, (char*)GFEXL2_MarchPriceQty_Sub_Code.c_str());
		if (Ret_Success != iCode)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail to Subscribe,GFEXL2_MarchPriceQty, ErrorCode: {}", iCode);
		}
		else
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Subscribe  GFEXL2_MarchPriceQty Success,Start to Receive Data ...");
		}
	}
}

void ParserQTS::subscribe(const CodeSet &vecSymbols)
{
	if (m_uTradingDate == 0)
	{
		m_filterSubs = vecSymbols;
	}
	else
	{
		m_filterSubs = vecSymbols;
	}

	int shNum = 0, szNum = 0, cffNum = 0, shfNum, ineNum = 0, czcNum = 0, dceNum = 0, gfeNum = 0;

	if (vecSymbols.size() == 0)
	{

	}
	else
	{
		for (auto& fullCode : vecSymbols)
		{
			m_strSZSTKCodes.clear();
			m_strSHSTKCodes.clear();
			std::string code, exchg;
			for (const auto& fullCode : m_filterSubs)
			{
				auto ay = StrUtil::split(fullCode.c_str(), ".");
				if (ay.size() < 2)
					continue;

				code = ay[1];
				exchg = ay[0];

				if (exchg == "SSE")
				{
					ContractType tp = extractType(code, true);
					switch (tp)
					{
					case M_INDX:
					{
						m_strSHINDXCodes += code;
						m_strSHINDXCodes += ",";
					}
					break;
					case M_BOND:
					{
						m_strSHBondCodes += code;
						m_strSHBondCodes += ",";
					}
					break;
					case M_STK:
					{
						m_strSHSTKCodes += code;
						m_strSHSTKCodes += ",";
					}
					break;
					case M_ETF:
					{
						m_strSHETFCodes += code;
						m_strSHETFCodes += ",";
					}
					break;
					case M_OPT:
					{
						m_strSHOptCodes += code;
						m_strSHOptCodes += ",";
					}
					break;
					case M_FUT:
					case M_UNKNOWN:
					default:
						break;
					}

					m_strSHCodes += code;
					m_strSHCodes += ",";

					shNum++;
				}
				else if (exchg == "SZSE")
				{
					ContractType tp = extractType(code, true);
					switch (tp)
					{
					case M_INDX:
					{
						m_strSZINDXCodes += code;
						m_strSZINDXCodes += ",";
					}
					break;
					case M_BOND:
					{
						m_strSZBondCodes += code;
						m_strSZBondCodes += ",";
					}
					break;
					case M_STK:
					{
						m_strSZSTKCodes += code;
						m_strSZSTKCodes += ",";
					}
					break;
					case M_ETF:
					{
						m_strSZETFCodes += code;
						m_strSZETFCodes += ",";
					}
					break;
					case M_OPT:
					{
						m_strSZOptCodes += code;
						m_strSZOptCodes += ",";
					}
					break;
					case M_FUT:
					case M_UNKNOWN:
					default:
						break;
					}

					m_strSZCodes += code;
					m_strSZCodes += ",";

					szNum++;
				}
				else if (exchg == "CFFEX")
				{
					m_strCFFEXFutCodes += code;
					m_strCFFEXFutCodes += ",";

					cffNum++;
				}
				else if (exchg == "CZCE")
				{
					m_strCZCEFutCodes += code;
					m_strCZCEFutCodes += ",";

					czcNum++;
				}
				else if (exchg == "DCE")
				{
					m_strDCEFutCodes += code;
					m_strDCEFutCodes += ",";

					dceNum++;
				}
				else if (exchg == "SHFE")
				{
					m_strSHFEFutCodes += code;
					m_strSHFEFutCodes += ",";

					shfNum++;
				}
				else if (exchg == "GFEX")
				{
					m_strGFEXFutCodes += code;
					m_strGFEXFutCodes += ",";

					gfeNum++;
				}
				else if (exchg == "INE")
				{
					m_strINEFutCodes += code;
					m_strINEFutCodes += ",";

					ineNum++;
				}

				write_log(m_parserSink, LL_DEBUG, "[subscribe] fullCode: {}", fullCode.c_str());
			}
		}

		if (m_strSHSTKCodes.size() > 0)
			m_strSHSTKCodes.pop_back();

		if (m_strSZSTKCodes.size() > 0)
			m_strSZSTKCodes.pop_back();

		if (m_strSHOptCodes.size() > 0)
			m_strSHOptCodes.pop_back();

		if (m_strSZOptCodes.size() > 0)
			m_strSZOptCodes.pop_back();

		if (m_strSHETFCodes.size() > 0)
			m_strSHETFCodes.pop_back();

		if (m_strSZETFCodes.size() > 0)
			m_strSZETFCodes.pop_back();

		if (m_strSHBondCodes.size() > 0)
			m_strSHBondCodes.pop_back();

		if (m_strSZBondCodes.size() > 0)
			m_strSZBondCodes.pop_back();

		if (m_strSHINDXCodes.size() > 0)
			m_strSHINDXCodes.pop_back();

		if (m_strSZINDXCodes.size() > 0)
			m_strSZINDXCodes.pop_back();

		if (m_strCFFEXFutCodes.size() > 0)
			m_strCFFEXFutCodes.pop_back();

		if (m_strCZCEFutCodes.size() > 0)
			m_strCZCEFutCodes.pop_back();

		if (m_strDCEFutCodes.size() > 0)
			m_strDCEFutCodes.pop_back();

		if (m_strGFEXFutCodes.size() > 0)
			m_strGFEXFutCodes.pop_back();

		if (m_strSHFEFutCodes.size() > 0)
			m_strSHFEFutCodes.pop_back();

		if (m_strINEFutCodes.size() > 0)
			m_strINEFutCodes.pop_back();

		if (m_strSHCodes.size() > 0)
			m_strSHCodes.pop_back();

		if (m_strSZCodes.size() > 0)
			m_strSZCodes.pop_back();
	}

	//write_log(m_parserSink, LL_DEBUG, "[ParserQTS] Subscribe {} instruments from SSE market ...", shNum > 0 ? std::to_string(shNum) : "All");
	write_log(m_parserSink, LL_DEBUG, "[ParserQTS] Subscribe {} instruments from SSE market ...", shNum);
	write_log(m_parserSink, LL_DEBUG, "[ParserQTS] Subscribe {} instruments from SZSE market ...", szNum);
	write_log(m_parserSink, LL_DEBUG, "[ParserQTS] Subscribe {} instruments from CFFEX market ...", cffNum);
	write_log(m_parserSink, LL_DEBUG, "[ParserQTS] Subscribe {} instruments from CZCE market ...", czcNum);
	write_log(m_parserSink, LL_DEBUG, "[ParserQTS] Subscribe {} instruments from DCE market ...", dceNum);
	write_log(m_parserSink, LL_DEBUG, "[ParserQTS] Subscribe {} instruments from GFEX market ...", gfeNum);
	write_log(m_parserSink, LL_DEBUG, "[ParserQTS] Subscribe {} instruments from SHFE market ...", shfNum);
	write_log(m_parserSink, LL_DEBUG, "[ParserQTS] Subscribe {} instruments from INE market ...", ineNum);
}

void ParserQTS::unsubscribe(const CodeSet &vecSymbols)
{
	RetCode code = m_pUserAPI->UnsubscribeAll();

	if (code != Ret_Success)
		write_log(m_parserSink, LL_ERROR, "[ParserQTS] Unsubscribe contracts fail: {}", code);
	else
		write_log(m_parserSink, LL_DEBUG, "[ParserQTS] Unsubscribe contracts success!");
}

bool ParserQTS::isConnected()
{
	return m_pUserAPI!=NULL;
}

void ParserQTS::registerSpi(IParserSpi* listener)
{
	m_parserSink = listener;

	if(m_parserSink)
		m_pBaseDataMgr = m_parserSink->getBaseDataMgr();
}

void ParserQTS::OnConnectionState(MsgType msgType, RetCode errCode)
{
	if (errCode == Ret_Success)
		write_log(m_parserSink, LL_INFO, "[ParserQTS] {} Connect!", m_strUser);
	else
		write_log(m_parserSink, LL_WARN, "[ParserQTS] {} Disconnect: ErrCode: {}!", m_strUser, errCode);
}

void ParserQTS::RecvData(THANDLE hTdf, TDF_MSG* pMsgHead)
{
	if (!pMsgHead->pData)
	{
		//assert(0);
		return;
	}

	unsigned int nItemCount = pMsgHead->pAppHead->nItemCount;
	unsigned int nItemSize = pMsgHead->pAppHead->nItemSize;

	if (!nItemCount)
		return;

	switch (pMsgHead->nDataType)
	{
	case MSG_DATA_MARKET:  // �������ݣ���Ʊ
	{
		for (uint32_t i = 0; i < nItemCount; ++i)
		{
			TDF_MARKET_DATA* pLastMarket = GETRECORD(pMsgHead->pData, TDF_MARKET_DATA, i);

			write_log(m_parserSink, LL_DEBUG, "[RecvData] Wind Code: {}, Code: {}, Time: {}, Last Price: {}, TotalVolume: {}, Type: {}", pLastMarket->szWindCode, pLastMarket->szCode, pLastMarket->nTime, pLastMarket->nMatch, pLastMarket->iVolume, pLastMarket->pCodeInfo->nType);

			const char* code = pLastMarket->szCode;
			const char* exchg = toStdExchg(pLastMarket->szWindCode + strlen(pLastMarket->szCode) + 1);

			WTSContractInfo* ct = m_pBaseDataMgr->getContract(code, exchg);
			if (ct == NULL)
			{
				if (m_parserSink)
					write_log(m_parserSink, LL_ERROR, "[ParserQTS] Instrument {}.{} not exists...", exchg, code);
				continue;
			}

			WTSCommodityInfo* commInfo = ct->getCommInfo();

			WTSTickData* tick = WTSTickData::create(code);
			tick->setContractInfo(ct);
			WTSTickStruct& quote = tick->getTickStruct();
			strcpy(quote.exchg, exchg);

			quote.action_date = (uint32_t)pLastMarket->nActionDay;
			quote.action_time = (uint32_t)pLastMarket->nTime;
			quote.trading_date = (uint32_t)pLastMarket->nTradingDay;

			quote.price = checkValid(pLastMarket->nMatch);
			quote.open = checkValid(pLastMarket->nOpen);
			quote.high = checkValid(pLastMarket->nHigh);
			quote.low = checkValid(pLastMarket->nLow);
			quote.total_volume = (uint32_t)pLastMarket->iVolume;
			quote.total_turnover = checkValid(pLastMarket->iTurnover);  // �ɽ������Ҫ����

			quote.upper_limit = checkValid(pLastMarket->nHighLimited);
			quote.lower_limit = checkValid(pLastMarket->nLowLimited);

			quote.pre_close = checkValid(pLastMarket->nPreClose);

			//ί���۸�
			for (int i = 0; i < 10; i++)
			{
				quote.ask_prices[i] = checkValid(pLastMarket->nAskPrice[i]);
				quote.ask_qty[i] = (uint32_t)pLastMarket->nAskVol[i];

				quote.bid_prices[i] = checkValid(pLastMarket->nBidPrice[i]);
				quote.bid_qty[i] = (uint32_t)pLastMarket->nBidVol[i];
			}

			if (m_parserSink)
				m_parserSink->handleQuote(tick, 1);

			//�������
			if (sizeof(pLastMarket->nBidOrders) / sizeof(int) > 0)
			{
				WTSOrdQueData* buyQue = WTSOrdQueData::create(code);
				buyQue->setContractInfo(ct);

				WTSOrdQueStruct& buyOS = buyQue->getOrdQueStruct();
				strcpy(buyOS.exchg, exchg);

				buyOS.trading_date = (uint32_t)pLastMarket->nTradingDay;
				buyOS.action_date = (uint32_t)pLastMarket->nActionDay;
				buyOS.action_time = (uint32_t)pLastMarket->nTime;

				buyOS.side = BDT_Buy;
				buyOS.price = quote.bid_prices[0];
				buyOS.order_items = 10;
				buyOS.qsize = std::min((int)buyOS.order_items, 50); // ���ֻ��50��������

				for (uint32_t i = 0; i < 10; i++)
				{
					buyOS.volumes[i] = (uint32_t)pLastMarket->nBidOrders[i];
				}

				if (m_parserSink)
					m_parserSink->handleOrderQueue(buyQue);

				buyQue->release();
			}

			if (sizeof(pLastMarket->nAskOrders) / sizeof(int) > 0)
			{
				WTSOrdQueData* sellQue = WTSOrdQueData::create(code);
				sellQue->setContractInfo(ct);

				WTSOrdQueStruct& sellOS = sellQue->getOrdQueStruct();
				strcpy(sellOS.exchg, exchg);

				sellOS.trading_date = (uint32_t)pLastMarket->nTradingDay;
				sellOS.action_date = (uint32_t)pLastMarket->nActionDay;
				sellOS.action_time = (uint32_t)pLastMarket->nTime;

				sellOS.side = BDT_Sell;
				sellOS.price = quote.ask_prices[0];
				sellOS.order_items = 10;
				sellOS.qsize = std::min((int)sellOS.order_items, 50);

				for (uint32_t i = 0; i < 10; i++)
				{
					sellOS.volumes[i] = (uint32_t)pLastMarket->nAskOrders[i];
				}

				if (m_parserSink)
					m_parserSink->handleOrderQueue(sellQue);

				sellQue->release();
			}

			tick->release();
		}
	}
	break;
	case MSG_DATA_FUTURE:  // �ڻ���Ȩ��������
	{
		for (uint32_t i = 0; i < nItemCount; ++i)
		{
			TDF_FUTURE_DATA* pLastMarket = GETRECORD(pMsgHead->pData, TDF_FUTURE_DATA, i);

			write_log(m_parserSink, LL_DEBUG, "[RecvData] Wind Code: {}, Code: {}, Time: {}, Last Price: {}, TotalVolume: {}, Type: {}", pLastMarket->szWindCode, pLastMarket->szCode, pLastMarket->nTime, pLastMarket->nMatch, pLastMarket->iVolume, pLastMarket->pCodeInfo->nType);

			const char* code = pLastMarket->szCode;
			const char* exchg = toStdExchg(pLastMarket->szWindCode + strlen(pLastMarket->szCode) + 1);

			WTSContractInfo* ct = m_pBaseDataMgr->getContract(code, exchg);
			if (ct == NULL)
			{
				if (m_parserSink)
					write_log(m_parserSink, LL_ERROR, "[ParserQTS] Instrument {}.{} not exists...", exchg, code);
				continue;
			}

			WTSCommodityInfo* commInfo = ct->getCommInfo();

			WTSTickData* tick = WTSTickData::create(code);
			tick->setContractInfo(ct);
			WTSTickStruct& quote = tick->getTickStruct();
			strcpy(quote.exchg, exchg);

			quote.action_date = (uint32_t)pLastMarket->nActionDay;
			quote.action_time = (uint32_t)pLastMarket->nTime;
			quote.trading_date = (uint32_t)pLastMarket->nTradingDay;

			quote.price = checkValid(pLastMarket->nMatch);
			quote.open = checkValid(pLastMarket->nOpen);
			quote.high = checkValid(pLastMarket->nHigh);
			quote.low = checkValid(pLastMarket->nLow);
			quote.total_volume = (uint32_t)pLastMarket->iVolume;
			quote.total_turnover = checkValid(pLastMarket->iTurnover);  // �ɽ������Ҫ����
			quote.open_interest = (uint32_t)pLastMarket->iOpenInterest;

			if (pLastMarket->nSettlePrice != 0)
			{
				quote.settle_price = checkValid(pLastMarket->nSettlePrice);
			}

			quote.upper_limit = checkValid(pLastMarket->nHighLimited);
			quote.lower_limit = checkValid(pLastMarket->nLowLimited);

			quote.pre_close = checkValid(pLastMarket->nPreClose);

			//ί���۸�
			for (int i = 0; i < 5; i++)
			{
				quote.ask_prices[i] = checkValid(pLastMarket->nAskPrice[i]);
				quote.ask_qty[i] = (uint32_t)pLastMarket->nAskVol[i];

				quote.bid_prices[i] = checkValid(pLastMarket->nBidPrice[i]);
				quote.bid_qty[i] = (uint32_t)pLastMarket->nBidVol[i];
			}

			if (m_parserSink)
				m_parserSink->handleQuote(tick, 1);

			tick->release();
		}
	}
	break;
	case MSG_DATA_INDEX:  // ָ����������
	{
		for (uint32_t i = 0; i < nItemCount; i++)
		{
			TDF_INDEX_DATA* pLastIndex = GETRECORD(pMsgHead->pData, TDF_INDEX_DATA, i);
			const char* code = pLastIndex->szCode;
			const char* exchg = toStdExchg(pLastIndex->szWindCode + strlen(pLastIndex->szCode) + 1);
			auto ct = m_pBaseDataMgr->getContract(code, exchg);
			if (ct == NULL)
			{
				if (m_parserSink)
					write_log(m_parserSink, LL_ERROR, "[ParserQTS] Instrument {}.{} not exists...", exchg, code);
				continue;
			}

			WTSTickData* tick = WTSTickData::create(code);
			WTSTickStruct& quote = tick->getTickStruct();
			strcpy(quote.exchg, exchg);

			quote.action_date = pLastIndex->nActionDay;
			quote.trading_date = pLastIndex->nTradingDay;
			quote.action_time = pLastIndex->nTime;

			quote.price = checkValid(pLastIndex->nLastIndex);
			quote.open = checkValid(pLastIndex->nOpenIndex);
			quote.high = checkValid(pLastIndex->nHighIndex);
			quote.low = checkValid(pLastIndex->nLowIndex);
			quote.pre_close = checkValid(pLastIndex->nPreCloseIndex);

			quote.total_volume = (uint32_t)pLastIndex->iTotalVolume; //
			quote.total_turnover = checkValid(pLastIndex->iTurnover) * 100.0;  //ָ���ɽ��λ�ǰ�Ԫ

			if (m_parserSink)
				m_parserSink->handleQuote(tick, true);

			tick->release();
		}
	}
	break;
	case MSG_DATA_TRANSACTION:
	{
		for (uint32_t i = 0; i < nItemCount; i++)
		{
			TDF_TRANSACTION* pLastTrans = GETRECORD(pMsgHead->pData, TDF_TRANSACTION, i);
			const char* code = pLastTrans->szCode;
			const char* exchg = toStdExchg(pLastTrans->szWindCode + strlen(pLastTrans->szCode) + 1);
			auto ct = m_pBaseDataMgr->getContract(code, exchg);
			if (ct == NULL)
				continue;

			WTSTransData* trans = WTSTransData::create(code);
			WTSTransStruct& ts = trans->getTransStruct();
			strcpy(ts.exchg, exchg);

			ts.trading_date = pLastTrans->nActionDay;
			ts.action_date = pLastTrans->nActionDay;
			ts.action_time = pLastTrans->nTime;
			ts.price = checkValid(pLastTrans->nPrice);
			ts.volume = pLastTrans->nVolume;
			ts.askorder = (uint32_t)pLastTrans->nAskOrder;
			ts.bidorder = (uint32_t)pLastTrans->nBidOrder;
			ts.index = (uint32_t)pLastTrans->nIndex;
			ts.side = (WTSBSDirectType)pLastTrans->nBSFlag;
			ts.ttype = pLastTrans->chOrderKind == 'F' ? TT_Match : TT_Cancel;  // 4=cancel, F=trade

			if (ts.side == BDT_Unknown)
			{
				if (ts.askorder > ts.bidorder)
					ts.side = BDT_Sell;
				else
					ts.side = BDT_Buy;
			}

			if (m_parserSink)
				m_parserSink->handleTransaction(trans);

			trans->release();
		}
	}
	break;
	case MSG_DATA_ORDERQUEUE:
	{
		for (uint32_t i = 0; i < nItemCount; i++)
		{
			TDF_ORDER_QUEUE* pLastOrdQue = GETRECORD(pMsgHead->pData, TDF_ORDER_QUEUE, i);

			const char* code = pLastOrdQue->szCode;
			const char* exchg = toStdExchg(pLastOrdQue->szWindCode + strlen(pLastOrdQue->szCode) + 1);
			auto ct = m_pBaseDataMgr->getContract(code, exchg);
			if (ct == NULL)
				continue;

			WTSOrdQueData* pData = WTSOrdQueData::create(code);
			WTSOrdQueStruct& oqs = pData->getOrdQueStruct();
			strcpy(oqs.exchg, exchg);

			oqs.trading_date = pLastOrdQue->nActionDay;
			oqs.action_date = pLastOrdQue->nActionDay;
			oqs.action_time = pLastOrdQue->nTime;
			oqs.side = (WTSBSDirectType)pLastOrdQue->nSide;
			oqs.order_items = pLastOrdQue->nOrders;
			oqs.price = checkValid(pLastOrdQue->nPrice);
			oqs.qsize = pLastOrdQue->nABItems;
			//��ิ��50��
			uint32_t maxCnt = oqs.qsize;
			if (maxCnt > 50)
				maxCnt = 50;
			memcpy(oqs.volumes, (uint32_t*)pLastOrdQue->nABVolume, sizeof(uint32_t)*maxCnt);

			if (m_parserSink)
				m_parserSink->handleOrderQueue(pData);

			pData->release();
		}
	}
	break;
	case MSG_DATA_ORDER:
	{
		for (uint32_t i = 0; i < nItemCount; i++)
		{
			TDF_ORDER* pLastOrder = GETRECORD(pMsgHead->pData, TDF_ORDER, i);
			const char* code = pLastOrder->szCode;
			const char* exchg = toStdExchg(pLastOrder->szWindCode + strlen(pLastOrder->szCode) + 1);
			auto ct = m_pBaseDataMgr->getContract(code, exchg);
			if (ct == NULL)
				continue;

			WTSOrdDtlData* order = WTSOrdDtlData::create(code);
			WTSOrdDtlStruct& ods = order->getOrdDtlStruct();
			strcpy(ods.exchg, exchg);
			ods.action_date = pLastOrder->nActionDay;
			ods.trading_date = pLastOrder->nActionDay;
			ods.action_time = pLastOrder->nTime;
			ods.side = (WTSBSDirectType)pLastOrder->chFunctionCode;
			ods.price = checkValid(pLastOrder->nPrice);
			ods.volume = pLastOrder->nVolume;
			ods.index = (uint32_t)pLastOrder->nOrder;
			ods.otype = (WTSOrdDetailType)pLastOrder->chOrderKind;

			if (m_parserSink)
				m_parserSink->handleOrderDetail(order);

			order->release();
		}
	}
	break;
	case MSG_DATA_BBQTRANSACTION:
	{

	}
	break;
	case MSG_DATA_BBQBID:
	{

	}
	break;
	case MSG_DATA_NON_MD:
	{

	}
	break;
	case MSG_DATA_BROKERQUEUE:
	{

	}
	case MSG_DATA_NB_ORDER:
	{

	}
	break;
	break;
	default:
	{

	}
	break;
	}
}

void ParserQTS::RecvSys(THANDLE hTdf, TDF_MSG* pSysMsg)
{
	if (!hTdf || !pSysMsg)
		return;

	switch (pSysMsg->nDataType)
	{
	case MSG_SYS_PACK_OVER:
	{
		TDF_PACK_OVER* pPackOver = (TDF_PACK_OVER*)pSysMsg->pData;
		write_log(m_parserSink, LL_DEBUG, "[ParserQTS] TDF_PACK_OVER: num: {},conID: {}", pPackOver->nDataNumber, pPackOver->nConID);
	}
	break;
	case MSG_SYS_DISCONNECT_NETWORK:
	{
		write_log(m_parserSink, LL_WARN, "[ParserQTS] DISCONNECT NETWORK");
	}
	break;
	case MSG_SYS_CONNECT_RESULT:
	{
		TDF_CONNECT_RESULT* pConnResult = (TDF_CONNECT_RESULT*)pSysMsg->pData;
		if (pConnResult && pConnResult->nConnResult)
		{
			write_log(m_parserSink, LL_INFO, "[ParserQTS] [{}] Connect {}:{} success!", pConnResult->szUser, pConnResult->szIp, pConnResult->szPort);
		}
		else
		{
			write_log(m_parserSink, LL_ERROR, "[ParserQTS] [{}] Connect {}:{} fail!", pConnResult->szUser, pConnResult->szIp, pConnResult->szPort);
		}
	}
	break;
	case MSG_SYS_LOGIN_RESULT:
	{
		TDF_LOGIN_RESULT* pLoginResult = (TDF_LOGIN_RESULT*)pSysMsg->pData;
		if (pLoginResult && pLoginResult->nLoginResult)
		{
			std::stringstream ss;
			for (int i = 0; i < pLoginResult->nMarkets; i++)
			{
				ss << "market: " << pLoginResult->szMarket[i] << ", dyn_date: " << pLoginResult->nDynDate[i] << (i >= pLoginResult->nMarkets - 1) ? "" : "\n";
			}
			write_log(m_parserSink, LL_INFO, "[ParserQTS] Login success! info: {}, nMarkets: {}, market list: {}", pLoginResult->szInfo, pLoginResult->nMarkets, ss.str());
		}
		else
		{
			write_log(m_parserSink, LL_ERROR, "[ParserQTS] Login failed. info: {}", pLoginResult->szInfo);
		}
	}
	break;
	case MSG_SYS_CODETABLE_RESULT://�ڴ˿ɻ�ȡ�����
	{
		TDF_CODE_RESULT* pCodeResult = (TDF_CODE_RESULT*)pSysMsg->pData;
		if (pCodeResult)
		{
			write_log(m_parserSink, LL_DEBUG, "[ParserQTS] Receive code table. info:{}, markets: {}", pCodeResult->szInfo, pCodeResult->nMarkets);
			for (int i = 0; i < pCodeResult->nMarkets; i++)
			{
				write_log(m_parserSink, LL_DEBUG, "[ParserQTS] Market: {}, total contracts: {}, date: {}", pCodeResult->szMarket[i], pCodeResult->nCodeCount[i], pCodeResult->nCodeDate[i]);
			}
		}
	}
	break;
	case MSG_SYS_HEART_BEAT:
	{
		write_log(m_parserSink, LL_DEBUG, "[ParserQTS] Receiving heartbeat message ...");
	}
	break;
	case MSG_SYS_SINGLE_CODETABLE_RESULT:
	{
		TDF_SINGLE_CODE_RESULT* pCodeResult = (TDF_SINGLE_CODE_RESULT*)pSysMsg->pData;
		if (pCodeResult)
		{
			write_log(m_parserSink, LL_DEBUG, "[ParserQTS] Receive total instruments {} from {} on {}", pCodeResult->nCodeCount, pCodeResult->szMarket, pCodeResult->nCodeDate);
			//�յ�����Ϣ�󣬿ɻ�ȡpCodeResult->szMarket�Ĵ����.
		}
	}
	break;
	case MSG_SYS_MARKET_EVENT:
	{
		TDF_MARKET_EVENT* pEvent = (TDF_MARKET_EVENT*)pSysMsg->pData;
		if (pEvent)
		{
			switch (pEvent->nEvent)
			{
			case ID_MARKET_CLEAR: //�г����̣������ոı�
				write_log(m_parserSink, LL_DEBUG, "[ParserQTS] Recv{} market clear msg on {}", pEvent->szMarket, pEvent->nDate);
				break;
			case ID_MARKET_TRANS:   //�г�ת�������г���ǰ�����ղ�������������
				write_log(m_parserSink, LL_DEBUG, "[ParserQTS] Recv {} market close msg on {}", pEvent->szMarket, pEvent->nDate);
				break;
			default:
				//�������账����Ϣ
				break;
			}
		}
	}
	break;
	case MSG_SYS_FAIL_REPLAY:
	{
		TDF_FAIL_REPLAY* pFail = (TDF_FAIL_REPLAY*)pSysMsg->pData;
		if (pFail)
		{
			write_log(m_parserSink, LL_WARN, "[ParserQTS] Fail replay. market: {}, date: {}, msg: {}", pFail->szMarket, pFail->nDate, pFail->szInfo);
		}
	}
	break;
	case MSG_SYS_ADDCODE:
	{
		TDF_ADD_CODE* pAddCode = (TDF_ADD_CODE*)pSysMsg->pData;
		if (pAddCode)
		{
			write_log(m_parserSink, LL_DEBUG, "[ParserQTS] Add Code��market: {}, codedate: {}, items: {}", pAddCode->szMarket, pAddCode->nCodeDate, pAddCode->nItems);
		}
	}
	break;
	default:
		break;
	}
}

void ParserQTS::OnSubscribe_SSEL1_Quotation(const SSEL1_Quotation& RealSSEL1Quotation)
{

}

void ParserQTS::OnSubscribe_SSEL1_BondQuotation(const SSEL1_BondQuotation& RealSSEL1BondQuotation) 
{

}

void ParserQTS::OnSubscribe_SSEL2_Quotation(const SSEL2_Quotation& RealSSEL2Quotation)
{

}

void ParserQTS::OnSubscribe_SSEL2_Index(const SSEL2_Index& RealSSEL2Index)
{

}

void ParserQTS::OnSubscribe_SSEL2_BondQuotation(const SSEL2_BondQuotation& RealSSEL2BondQuotation)
{

}

void ParserQTS::OnSubscribe_SSEL2_BondTick(const SSEL2_BondTick& RealSSEL2BondTick)
{

}

/// �Ͻ���L2��ʳɽ���������ʵʱ�ص��ӿ�
/// @param  RealSSEL2Transaction    -- ʵʱ����
void ParserQTS::OnSubscribe_SSEL2_Transaction(const SSEL2_Transaction& RealSSEL2Transaction) {}

/// �Ͻ���L2���ί�ж�������ʵʱ�ص��ӿ�
/// @param  RealSSEL2Order    -- ʵʱ����
void ParserQTS::OnSubscribe_SSEL2_Order(const SSEL2_Order& RealSSEL2Order) {}

/// �Ͻ���������ȨL1ʵʱ���鶩������ʵʱ�ص��ӿ�
/// @param  RealSSEIOL1Quotation  -- ʵʱ����
void ParserQTS::OnSubscribe_SSEIOL1_Quotation(const SSEIOL1_Quotation& RealSSEIOL1Quotation) {}


/// �Ͻ���L2 ETFͳ�����ݶ�������ʵʱ�ص��ӿ�
/// @param  RealSSEL2Order    -- ʵʱ����
void ParserQTS::OnSubscribe_SSEL2_ETF(const SSEL2_ETF& data) {}

void ParserQTS::OnSubscribe_SSEL2_ShortQuotation(const SSEL2_ShortQuotation& RealSSEL2_ShortQuotation) {}
void ParserQTS::OnSubscribe_SSEL2_OrderQueue(const SSEL2_OrderQueue& RealSSEL2_OrderQueue) {}
void ParserQTS::OnSubscribe_SSEL2_Tick(const SSEL2_Tick& data) {}
void ParserQTS::OnSubscribe_SSEL1_FpQuotation(const SSEL1_FpQuotation& data) {}
void ParserQTS::OnSubscribe_SSEL2_FpQuotation(const SSEL2_FpQuotation& data) {}

//////////////////////////////////////  ���  ////////////////////////////////

/// ���L1ʵʱ���鶩������ʵʱ�ص��ӿ�
/// @param  RealSZSEL1Quotation -- ʵʱ����
void ParserQTS::OnSubscribe_SZSEL1_Quotation(const SZSEL1_Quotation& RealSZSEL1Quotation) {}

/// ���L2ʵʱ���鶩������ʵʱ�ص��ӿ�
/// @param  RealSZSEL2Quotation -- ʵʱ����
void ParserQTS::OnSubscribe_SZSEL2_Quotation(const SZSEL2_Quotation& RealSZSEL2Quotation) {}

/// ���L2���ί�����鶩������ʵʱ�ص��ӿ�
/// @param  RealSZSEL2Transaction -- ʵʱ����
void ParserQTS::OnSubscribe_SZSEL2_Transaction(const SZSEL2_Transaction& RealSZSEL2Transaction) {}

/// ���L2���ί�����鶩������ʵʱ�ص��ӿ�
/// @param  RealSZSEL2Order -- ʵʱ����
void ParserQTS::OnSubscribe_SZSEL2_Order(const SZSEL2_Order& RealSZSEL2Order) {}

/// ���L2ָ�����鶩������ʵʱ�ص��ӿ�
/// @param  RealSZSEL2Index -- ʵʱ����
void ParserQTS::OnSubscribe_SZSEL2_Index(const SZSEL2_Index& RealSZSEL2Index) {}

/// �������L2ծȯʵʱ��������ʵʱ�ص��ӿ�
/// @param  RealSZSEL2BondQuotation -- ʵʱ����
void ParserQTS::OnSubscribe_SZSEL2_BondQuotation(const SZSEL2_BondQuotation& RealSZSEL2BondQuotation) {}

/// �������L2ծȯ��ʳɽ�����ʵʱ�ص��ӿ�
/// @param  RealSZSEL2BondTransaction -- ʵʱ����
void ParserQTS::OnSubscribe_SZSEL2_BondTransaction(const SZSEL2_BondTransaction& RealSZSEL2BondTransaction) {}

/// �������L2ծȯ���ί��ʵʱ��������ʵʱ�ص��ӿ�
/// @param  RealSZSEL2BondOrder -- ʵʱ����
void ParserQTS::OnSubscribe_SZSEL2_BondOrder(const SZSEL2_BondOrder& RealSZSEL2BondOrder) {}

/// �������L2ծȯ�������ʵʱ�ص��ӿ� 
/// @param  RealSZSEL2_BondTick -- ʵʱ����
void ParserQTS::OnSubscribe_SZSEL2_BondTick(const SZSEL2_BondTick& RealSZSEL2_BondTick) {}

/// �������L1ծȯʵʱ����ʵʱ�ص��ӿ�
/// @param  RealSZSEL1BondQuotation -- ʵʱ����
void ParserQTS::OnSubscribe_SZSEL1_BondQuotation(const SZSEL1_BondQuotation& RealSZSEL1BondQuotation) {}

/// ���L2ծȯ�г�����ԤԼ��Ϣ��������ʵʱ�ص��ӿ�
/// @param  RealSZSEL2BondBidBookingInfo -- ʵʱ����
void ParserQTS::OnSubscribe_SZSEL2_BondBidBookingInfo(const SZSEL2_BondBidBookingInfo& RealSZSEL2BondBidBookingInfo) {}

/// �������������ȨL1ʵʱ����ʵʱ�ص��ӿ�
/// @param  RealSZSEIOL1Quotation -- ʵʱ����
void ParserQTS::OnSubscribe_SZSEIOL1_Quotation(const SZSEIOL1_Quotation& RealSZSEIOL1Quotation) {}

void ParserQTS::OnSubscribe_SZSEL2_ShortQuotation(const SZSEL2_ShortQuotation& RealSZSEL2_ShortQuotation) {}
void ParserQTS::OnSubscribe_SZSEL2_OrderQueue(const SZSEL2_OrderQueue& RealSZSEL2_OrderQueue) {}
void ParserQTS::OnSubscribe_SZSEL1_FpQuotation(const SZSEL1_FpQuotation& data) {}
void ParserQTS::OnSubscribe_SZSEL2_FpQuotation(const SZSEL2_FpQuotation& data) {}
void ParserQTS::OnSubscribe_SZSEL2_Tick(const SZSEL2_Tick& data) {}

//////////////////////////////////////  ������  ////////////////////////////////

/// ������L1ʵʱ���鶩������ʵʱ�ص��ӿ�
/// @param  RealBSEL1Quotation -- ʵʱ����
void ParserQTS::OnSubscribe_BSEL1_Quotation(const BSEL1_Quotation& RealBSEL1Quotation) {}

//////////////////////////////////////  �н���  ////////////////////////////////

/// �н���L2ʵʱ���鶩������ʵʱ�ص��ӿ�
/// @param  RealCFFEXL2Quotation -- ʵʱ����
void ParserQTS::OnSubscribe_CFFEXL2_Quotation(const CFFEXL2_Quotation& RealCFFEXL2Quotation) {}

//////////////////////////////////////  ������  ////////////////////////////////

/// ������L1ʵʱ���鶩������ʵʱ�ص��ӿ�
/// @param  RealSHFEL1Quotation -- ʵʱ����
void ParserQTS::OnSubscribe_SHFEL1_Quotation(const SHFEL1_Quotation& RealSHFEL1Quotation) {}

/// ������L2ʵʱ���鶩������ʵʱ�ص��ӿ�
/// @param  RealSHFEL2Quotation -- ʵʱ����
void ParserQTS::OnSubscribe_SHFEL2_Quotation(const SHFEL2_Quotation& RealSHFEL2Quotation) {}

//////////////////////////////////////  ������Դ  ////////////////////////////////

/// ������ԴL1ʵʱ���鶩������ʵʱ�ص��ӿ�
/// @param  RealINEL1Quotation -- ʵʱ����
void ParserQTS::OnSubscribe_INEL1_Quotation(const INEL1_Quotation& RealINEL1Quotation) {}

/// ������ԴL2ʵʱ���鶩������ʵʱ�ص��ӿ�
/// @param  RealINEL2Quotation -- ʵʱ����
void ParserQTS::OnSubscribe_INEL2_Quotation(const INEL2_Quotation& RealINEL2Quotation) {}

//////////////////////////////////////  ֣����  ////////////////////////////////

/// ֣����L1ʵʱ���鶩������ʵʱ�ص��ӿ�
/// @param  RealCZCEL1Quotation -- ʵʱ����
void ParserQTS::OnSubscribe_CZCEL1_Quotation(const CZCEL1_Quotation& RealCZCEL1Quotation) {}

/// ֣����L1ʵʱ�������鶩������ʵʱ�ص��ӿ�
/// @param  RealCZCEL1ArbiQuotation -- ʵʱ����
void ParserQTS::OnSubscribe_CZCEL1_ArbiQuotation(const CZCEL1_ArbiQuotation& RealCZCEL1ArbiQuotation) {}

/// ֣����L2ʵʱ����(�����䲿��)��������ʵʱ�ص��ӿ�
/// @param  RealCZCEL2Quotation -- ʵʱ����
void ParserQTS::OnSubscribe_CZCEL2_Quotation(const CZCEL2_Quotation& RealCZCEL2Quotation) {}

/// ֣����L2�嵵���ί�����鶩������ʵʱ�ص��ӿ�
/// @param  RealCZCEL2LevelQuotation -- ʵʱ����
void ParserQTS::OnSubscribe_CZCEL2_LevelQuotation(const CZCEL2_LevelQuotation& RealCZCEL2LevelQuotation) {}

/// ֣����L2�������鶩�����ݻص��ӿ�
/// @param  data     -- ʵʱ����
void ParserQTS::OnSubscribe_CZCEL2_ArbiQuotation(const CZCEL2_ArbiQuotation& data) {}

/// ֣����L2�����嵵������鶩�����ݻص��ӿ�
/// @param  data     -- ʵʱ����
void ParserQTS::OnSubscribe_CZCEL2_ArbiLevelQuotation(const CZCEL2_ArbiLevelQuotation& data) {}

//////////////////////////////////////  ������  ////////////////////////////////
/// ������L1ʵʱ���鶩������ʵʱ�ص��ӿ�
/// @param  RealDCEL1Quotation -- ʵʱ����
void ParserQTS::OnSubscribe_DCEL1_Quotation(const DCEL1_Quotation& RealDCEL1Quotation) {}

/// ������L1�������鶩������ʵʱ�ص��ӿ�
/// @param  RealDCEL1ArbiQuotation -- ʵʱ����
void ParserQTS::OnSubscribe_DCEL1_ArbiQuotation(const DCEL1_ArbiQuotation& RealDCEL1ArbiQuotation) {}

/// ������L2ʵʱ���鶩������ʵʱ�ص��ӿ�
/// @param  RealDCEL2Quotation -- ʵʱ����
void ParserQTS::OnSubscribe_DCEL2_Quotation(const DCEL2_Quotation& RealDCEL2Quotation) {}

/// ������L2��������������ʵʱ�ص��ӿ�
/// @param  RealDCEL2ArbiQuotation -- ʵʱ����
void ParserQTS::OnSubscribe_DCEL2_ArbiQuotation(const DCEL2_ArbiQuotation& RealDCEL2ArbiQuotation) {}

/// ������L2ʵʱ����۶�������ʵʱ�ص��ӿ�
/// @param  RealDCEL2RealTimePrice -- ʵʱ����
void ParserQTS::OnSubscribe_DCEL2_RealTimePrice(const DCEL2_RealTimePrice& RealDCEL2RealTimePrice) {}

/// ������L2ί��ͳ������������ʵʱ�ص��ӿ�
/// @param  RealDCEL2OrderStatistic -- ʵʱ����
void ParserQTS::OnSubscribe_DCEL2_OrderStatistic(const DCEL2_OrderStatistic& RealDCEL2OrderStatistic) {}

/// ������L2�ּ۳ɽ������鶩������ʵʱ�ص��ӿ�
/// @param  RealDCEL2MarchPriceQty -- ʵʱ����
void ParserQTS::OnSubscribe_DCEL2_MarchPriceQty(const DCEL2_MarchPriceQty& RealDCEL2MarchPriceQty) {}

void ParserQTS::OnSubscribe_DCEL2_OrderQueue(const DCEL2_OrderQueue &RealDCEL2OrderQueue) {}
void ParserQTS::OnSubscribe_DCEL2_LevelQuotation(const DCEL2_LevelQuotation &RealDCEL2LevelQuotation) {}
void ParserQTS::OnSubscribe_DCE_Index(const DCE_Index& data) {}

//////////////////////////////////////  ������  ////////////////////////////////

void ParserQTS::OnSubscribe_GFEXL1_Quotation(const GFEXL1_Quotation &RealGFEXL1_Quotation) {}
void ParserQTS::OnSubscribe_GFEXL1_ArbiQuotation(const GFEXL1_ArbiQuotation &RealGFEXL1_ArbiQuotation) {}
void ParserQTS::OnSubscribe_GFEXL2_Quotation(const GFEXL2_Quotation &RealGFEXL2_Quotation) {}
void ParserQTS::OnSubscribe_GFEXL2_ArbiQuotation(const GFEXL2_ArbiQuotation &RealGFEXL2_ArbiQuotation) {}
void ParserQTS::OnSubscribe_GFEXL2_ArbiOrderQueue(const GFEXL2_ArbiOrderQueue &RealGFEXL2_ArbiOrderQueue) {}
void ParserQTS::OnSubscribe_GFEXL2_OrderQueue(const GFEXL2_OrderQueue &RealGFEXL2_OrderQueue) {}
void ParserQTS::OnSubscribe_GFEXL2_OrderStatistic(const GFEXL2_OrderStatistic &RealGFEXL2_OrderStatistic) {}
void ParserQTS::OnSubscribe_GFEXL2_RealTimePrice(const GFEXL2_RealTimePrice &RealGFEXL2_RealTimePrice) {}
void ParserQTS::OnSubscribe_GFEXL2_MarchPriceQty(const GFEXL2_MarchPriceQty &RealGFEXL2_MarchPriceQty) {}
