/*!
 * \file TraderMarItpdk.cpp
 * \project	WonderTrader
 *
 * \author Suuuunseeker
 * \date 2022/09/06
 *
 * \brief
 */

#include "TraderMarItpdk.h"

#include "../Includes/WTSError.hpp"
#include "../Includes/WTSContractInfo.hpp"
#include "../Includes/WTSSessionInfo.hpp"
#include "../Includes/WTSTradeDef.hpp"
#include "../Includes/WTSDataDef.hpp"
#include "../Includes/WTSVariant.hpp"
#include "../Share/StdUtils.hpp"
#include "../Share/TimeUtils.hpp"
#include "../Includes/IBaseDataMgr.h"
#include "../Share/DLLHelper.hpp"
#include "../Share/decimal.h"
#include "../Share/StrUtil.hpp"
#include <rapidjson/document.h>
#include <map>
#include <algorithm>

#include <iostream>

#include "../Share/BoostFile.hpp"
 //By Wesley @ 2022.01.05
#include "../Share/fmtlib.h"


/*
 @brief: ����ȫ��map�����ڲ��Ҷ�ʵ������ص���Ӧ��TraderMarItpdk����Ľ��
*/
std::map<std::string, TraderMarItpdk*> itpdkCallMap;

std::unique_ptr<MarItpdkCallMgr> g_callback_mgr;

template<typename... Args>
inline void write_log(ITraderSpi* sink, WTSLogLevel ll, const char* format, const Args&... args)
{
	if (sink == NULL)
		return;

	const char* buffer = fmtutil::format(format, args...);

	sink->handleTraderLog(ll, buffer);
}

void inst_hlp() {}

#ifdef _WIN32
#ifdef _WIN64
#pragma message("version@@x64")
#pragma comment(lib, "../API/cmaritpdk5.1.20.0/x64/maritpdk.lib")	//64λ�Ŀ�
#else
#pragma message("version@@x86")
#pragma comment(lib, "../API/cmaritpdk5.1.20.0/x86/maritpdk_x86.lib")	//32λ�Ŀ�
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

char PLATFORM_NAME[] = "UNIX";

std::string	g_moduleName;

const std::string& getInstPath()
{
	static std::string moduleName;
	if (moduleName.empty())
	{
		Dl_info dl_info;
		dladdr((void *)inst_hlp, &dl_info);
		moduleName = dl_info.dli_fname;
	}

	return moduleName;
}
#endif

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
		_bin_dir = getInstPath();
#endif
		boost::filesystem::path p(_bin_dir);
		_bin_dir = p.branch_path().string() + "/";
	}

	return _bin_dir;
}

const char* ENTRUST_SECTION = "entrusts";
const char* ORDER_SECTION = "orders";

inline const char* exchgI2O(const char* exchg)
{
	if (strcmp(exchg, "SSE") == 0)
		return "SH";
	else if (strcmp(exchg, "SZSE") == 0)
		return "SZ";

	return "";
}

inline const char* exchgO2I(const char* exchg)
{
	if (strcmp(exchg, "SH") == 0)
		return "SSE";
	else if (strcmp(exchg, "SZ") == 0)
		return "SZSE";

	return "";
}

WTSOrderState wrapOrderState(int state)
{
	switch (state)
	{
	case 0: return WOS_Nottouched; break;
	case 1: return WOS_Submitting; break;
	case 2: return WOS_NotTraded_NotQueuing; break;
	case 5: return WOS_PartTraded_Queuing; break;
	case 6: return WOS_AllTraded; break;
	default:
		return WOS_Canceled;
		break;
	}
}

uint32_t strToTime(const char* strTime)
{
	std::string str;
	const char *pos = strTime;
	while (strlen(pos) > 0)
	{
		if (pos[0] != ':' && pos[0] != '.')
		{
			str.append(pos, 1);
		}
		pos++;
	}

	return strtoul(str.c_str(), NULL, 10);
}

extern "C"
{
	EXPORT_FLAG ITraderApi* createTrader()
	{
		TraderMarItpdk *instance = new TraderMarItpdk();
		return instance;
	}

	EXPORT_FLAG void deleteTrader(ITraderApi* &trader)
	{
		if (NULL != trader)
		{
			delete trader;
			trader = NULL;
		}
	}
}

std::string MARITPDK_GetLastError()
{
	char msg[256];
	MARITPDK_GetLastError(msg);            //��ȡ������Ϣ
	return msg;
}

void ConnCallBack(const char* pKhh, const char* pConnKey, int nEvent, void* pData)
{
	std::cout << "Connect event khh: " << pKhh << "event: " << (0 == nEvent ? "�ָ�" : "�Ͽ�") << std::endl;
}

TraderMarItpdk::TraderMarItpdk()
	: m_wrapperState(WS_NOTLOGIN)
	, m_uLastQryTime(0)
	, m_bInQuery(false)
	, m_bUseEX(false)
	, m_bASync(false)
	, m_strandIO(NULL)
	, m_lastQryTime(0)
	, m_orderRef(1)
	, m_lDate(0)
	, m_mapLives(NULL)
	, m_bdMgr(NULL)
	, m_traderSink(NULL)
{
	m_mapLives = TradeDataMap::create();
}

TraderMarItpdk::~TraderMarItpdk()
{

}

void TraderMarItpdk::InitializeHTS(WTSVariant* params)
{
	static bool bInited = false;
	bInited = g_callback_mgr->init(params);  //��ʼ���������нӿ�ʹ��ǰ���ã���·�����ýӿ���
	if (!bInited)
	{
		write_log(m_traderSink, LL_ERROR, "[TraderMarItpdk]��ʼ��ʧ��!");
		return;
	}
}

bool TraderMarItpdk::init(WTSVariant* params)
{
	m_strCommPass = params->getCString("comm_user");
	m_strCommPass = params->getCString("comm_pass");

	m_strUser = params->getCString("user");
	m_strPass = params->getCString("pass");

	m_strWtfs = params->getCString("order_way");
	m_strNode = params->getCString("node");
	m_strKey = params->getCString("key");

	m_bUseEX = params->getBoolean("use_ex");
	m_bASync = params->getBoolean("use_async");

	m_strOrgID = params->getCString("org_id");

	m_nLoginMode = params->getInt32("login_mode");

	m_strShGdh = params->getCString("shGdh");
	m_strSzGdh = params->getCString("szGdh");
	m_strShZjzh = params->getCString("shFundAcct");
	m_strSzZjzh = params->getCString("szFundAcct");

	WTSVariant* param = params->get("ddmodule");
	if (param != NULL)
	{
		m_strModule = getBinDir() + param->asCString();
	}
	else
	{
#ifdef _WIN32
#ifdef _WIN64
		m_strModule = getBinDir() + "maritpdk.dll";
#else
		m_strModule = getBinDir() + "maritpdk_x86.dll";
#endif
#else
		m_strModule = getBinDir() + "libmaritpdk.so";
#endif
	}

	m_hInstDD = DLLHelper::load_library(m_strModule.c_str());
	write_log(m_traderSink, LL_INFO, "loading module: {}", m_strModule);

	// ��ʼ������
	InitializeHTS(params);

	// ���ûص�����������Ϣ
	try
	{
		//g_callback_mgr->setCallbackMsgFunc();  // ������Ϣ�����ûص�����
		g_callback_mgr->setHTSCallPtr(m_strUser.c_str(), this);  // ����ǰ������ӵ�ȫ��ӳ�����
	}
	catch (...)
	{
		std::string msg = MARITPDK_GetLastError();
		write_log(m_traderSink, LL_ERROR, "[TraderMarItpdk]���ĳɽ��ر�ʧ�ܣ�{}({})", msg.c_str(), -1);
	}

	return true;
}

void TraderMarItpdk::release()
{
	MARITPDK_Exit();  // �˳�������ʹ����������

	if (m_mapLives)
		m_mapLives->release();
}

void TraderMarItpdk::reconnect()
{
	//if (m_traderSink)
	//{
	//	m_traderSink->handleEvent(WTE_Connect, -1);
	//	m_traderSink->handleTraderLog(LL_ERROR, "[TraderMarItpdk]ͨѶ����ʧ��");
	//}

	//StdThreadPtr thrd(new StdThread([this]() {
	//	std::this_thread::sleep_for(std::chrono::seconds(2));
	//	if (m_traderSink)
	//	{
	//		write_log(m_traderSink, LL_WARN, "[TraderMarItpdk]�˺�{}������������", m_strUser.c_str());  //m_traderSink->handleTraderLog(LL_WARN, "[TraderMarItpdk]�˺�%s������������", m_strUser.c_str());
	//	}
	//	reconnect();
	//}));

	//registerSpi(this);						// ע���¼�

	if (m_traderSink) m_traderSink->handleEvent(WTE_Connect, 0);
}

void TraderMarItpdk::connect()
{
	if (m_thrdWorker == NULL)
	{
		m_strandIO = new boost::asio::io_service::strand(m_asyncIO);
		boost::asio::io_service::work work(m_asyncIO);
		m_thrdWorker.reset(new StdThread([this]() {
			while (true)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(2));
				m_asyncIO.run_one();
			}
		}));
	}

	reconnect();
}

void TraderMarItpdk::disconnect()
{
	m_asyncIO.post([this]() {
		release();
	});

	if (m_thrdWorker)
	{
		m_asyncIO.stop();
		m_thrdWorker->join();
		m_thrdWorker = NULL;

		delete m_strandIO;
		m_strandIO = NULL;
	}
}

void TraderMarItpdk::htsGetCallback(const char* pTime, const char* pMsg, int nType)
{
	// ��������
	write_log(m_traderSink, LL_INFO, "[TraderMarItpdk] ��ʼ���ûص�����");

	rapidjson::Document document;
	if (document.Parse(pMsg).HasParseError())
	{
		write_log(m_traderSink, LL_ERROR, "[TraderMarItpdk] ��������ί��������Ϣʧ��!");
		return;
	}

	long nWTH = (long)document["WTH"].GetInt64();
	const char* sKHH = document["KHH"].GetString();
	const char* sGDH = document["GDH"].GetString();
	const char* sZQDM = document["ZQDM"].GetString();

	if (NOTIFY_PUSH_ORDER == nType)
	{
		//ȷ�ϻر�����
		write_log(m_traderSink, LL_DEBUG, "[TraderMarItpdk] Insert order confirm callback: WTH = {}, KHH = {},GDH = {},ZQDM = {}", nWTH, sKHH, sGDH, sZQDM);

		OnRtnOrder(pMsg);
	}
	else if (NOTIFY_PUSH_MATCH == nType)
	{
		//�ɽ��ر�����
		write_log(m_traderSink, LL_DEBUG, "[TraderMarItpdk] Order match confirm callback: WTH = {}, KHH = {},GDH = {},ZQDM = {}", nWTH, sKHH, sGDH, sZQDM);

		OnRtnTrade(pMsg);
	}
	else if (NOTIFY_PUSH_WITHDRAW == nType)
	{
		// �����ر�����
		write_log(m_traderSink, LL_DEBUG, "[TraderMarItpdk] Cancel order confirm callback: WTH = {}, KHH = {},GDH = {},ZQDM = {}", nWTH, sKHH, sGDH, sZQDM);
		const char* sCXBZ = document["CXBZ"].GetString();
		int64 iSBJG = document["SBJG"].GetInt();
		const char* sJGSM = document["JGSM"].GetString();

		//����ȷ�ϻر�
		OnRtnOrder(pMsg);
	}
	else if (NOTIFY_PUSH_INVALID == nType)
	{
		//�ϵ�
		write_log(m_traderSink, LL_ERROR, "[TraderMarItpdk] Abandoned order callback: WTH = {}, KHH = {},GDH = {},ZQDM = {}", nWTH, sKHH, sGDH, sZQDM);
	}
}

void TraderMarItpdk::OnRtnOrder(const char* pMsg)
{
	//write_log(m_traderSink, LL_INFO, "[TraderMarItpdk] Push Order Report beginning ...");

	rapidjson::Document document;
	if (document.Parse(pMsg).HasParseError())   //����ʧ��
	{
		write_log(m_traderSink, LL_ERROR, "[TraderMarItpdk]��������ί��������Ϣʧ��.");
		return;
	}

	std::string code = document["ZQDM"].GetString();
	std::string exchg = exchgO2I(document["JYS"].GetString());

	bool isCancel = document["CXBZ"].GetString()[0] == 'W';  // ������־�Ƿ���W /// O��ʾί��

	//ȷ������
	//std::cout << "Accept push report message: " << " code: " << code << "  exchg: " << "  wth: " << document["WTH"].GetInt64() << "  kfsbdbh: "  << document["KFSBDBH"].GetInt64() << "  wtjg: " << document["WTJG"].GetDouble() << "  wjsl: " << document["WTSL"].GetDouble() << std::endl;

	WTSContractInfo* contract = m_bdMgr->getContract(code.c_str(), exchg.c_str());

	if (contract != NULL)
	{
		WTSOrderInfo* ordInfo = NULL;
		if (!isCancel)
		{
			char buf[256] = { 0 };
			strncpy(buf, std::to_string(document["WTH"].GetInt64()).c_str(), sizeof(buf));  /// ����ʹ��WTH�� ������������ΪKFSBDBH
			ordInfo = (WTSOrderInfo*)m_mapLives->grab(buf);

			if (ordInfo == NULL)
			{
				ordInfo = WTSOrderInfo::create();
				ordInfo->setPrice(document["WTJG"].GetDouble());
				ordInfo->setVolume(document["WTSL"].GetDouble());
				ordInfo->setDirection(WDT_LONG);
				ordInfo->setPriceType(WPT_LIMITPRICE);

				ordInfo->setOffsetType(document["JYLB"].GetInt() == 1 ? WOT_OPEN : WOT_CLOSE);

				//ordInfo->setOrderDate(m_lDate);
				//len = 256;
				//Fix_GetItem(hsess, FID_WTSJ, buf, len);
				//uint32_t uTime = strToTime(buf);
				//ordInfo->setOrderTime(TimeUtils::makeTime(ordInfo->getOrderDate(), uTime));
				ordInfo->setOrderDate(TimeUtils::getCurDate());
				ordInfo->setOrderTime(TimeUtils::getLocalTimeNow());

				ordInfo->setCode(code.c_str());
				ordInfo->setExchange(contract->getExchg());

				ordInfo->setOrderID(buf);

				strncpy(buf, std::to_string(document["KFSBDBH"].GetInt64()).c_str(), 256);
				ordInfo->setEntrustID(buf);  ///ʹ�ÿ����̱��ر��

				std::string usertag = m_iniHelper.readString(ENTRUST_SECTION, ordInfo->getEntrustID(), "");
				if (usertag.empty())
				{
					ordInfo->setUserTag(ordInfo->getEntrustID());
				}
				else
				{
					ordInfo->setUserTag(usertag.c_str());

					if (strlen(ordInfo->getOrderID()) > 0)
					{
						m_iniHelper.writeString(ORDER_SECTION, StrUtil::trim(ordInfo->getOrderID()).c_str(), usertag.c_str());
						m_iniHelper.save();
					}
				}

				if (m_mapLives == NULL)
					m_mapLives = TradeDataMap::create();

				if (ordInfo->isAlive())
					m_mapLives->add(ordInfo->getOrderID(), ordInfo, true);
			}

			int state = document["SBJG"].GetInt();
			ordInfo->setOrderState(wrapOrderState(state));
			if (state == 3)
				ordInfo->setError(true);

			double total = document["WTSL"].GetDouble();
			double traded = document["CJSL"].GetDouble();
			double canceled = document["CDSL"].GetDouble();
			ordInfo->setVolume(total);

			if (ordInfo->isAlive())
				ordInfo->setVolLeft(total - canceled - traded);
			else
				ordInfo->setVolLeft(0);

			strncpy(buf, document["JGSM"].GetString(), 256);
			ordInfo->setStateMsg(buf);

			if (!ordInfo->isAlive())
				m_mapLives->remove(ordInfo->getOrderID());
		}
		else
		{
			//write_log(m_traderSink, LL_INFO, "[Cancel]Code: {}, qty: {}, WTH: {}, CXWTH: {}, JGSM: {}", code, document["WTSL"].GetDouble(), document["WTH"].GetInt64(), document["CXWTH"].GetInt64(), document["JGSM"].GetString());

			char buf[256] = { 0 };
			strncpy(buf, std::to_string(document["WTH"].GetInt64()).c_str(), sizeof(buf));  // ��Ҫʹ��ί�к�,�����޷���ȡ������Ϣ���ص��Ῠס
			ordInfo = (WTSOrderInfo*)m_mapLives->grab(buf);

			if (ordInfo == NULL)
				return;

			m_mapLives->remove(buf);

			ordInfo->setVolLeft(0);
			ordInfo->setOrderState(WOS_Canceled);

			strncpy(buf, document["JGSM"].GetString(), 256);
			ordInfo->setStateMsg(buf);

			std::cout << "cxwth: " << document["CXWTH"].GetInt64() << " wth: " << document["WTH"].GetInt64() << " result_info: " << document["JGSM"].GetString() << std::endl;  // ʹ��ί�к�

			//if (ordInfo)
			//{
			//	m_mapLives->remove(buf);

			//	ordInfo->setVolLeft(0);
			//	ordInfo->setOrderState(WOS_Canceled);

			//	strncpy(buf, document["JGSM"].GetString(), 256);
			//	ordInfo->setStateMsg(buf);

			//	std::cout << "cxwth: " << document["CXWTH"].GetInt64() << " wth: " << document["WTH"].GetInt64() << " result_info: " << document["JGSM"].GetString() << std::endl;  // ʹ��ί�к�
			//}
		}

		if (m_traderSink)
			m_traderSink->onPushOrder(ordInfo);

		ordInfo->release();
	}
}

void TraderMarItpdk::OnRtnTrade(const char* pMsg)
{
	write_log(m_traderSink, LL_INFO, "[TraderMarItpdk] Trade Report beginning ...");

	rapidjson::Document document;
	if (document.Parse(pMsg).HasParseError())   //����ʧ��
	{
		write_log(m_traderSink, LL_ERROR, "[TraderMarItpdk]���������ɽ��ر���Ϣʧ��.");
		return;
	}

	std::string code = document["ZQDM"].GetString();
	std::string exchg = exchgO2I(document["JYS"].GetString());

	//�ɽ�����
	//std::cout << "Accept trade report message: " << " code: " << code << "  exchg: " << exchg;

	std::string tradeid = document["CJBH"].GetString();

	auto it = m_tradeids.find(tradeid);
	if (it != m_tradeids.end())
		return;

	m_tradeids.insert(tradeid);

	//���˵������ر�
	bool isCancel = document["CXBZ"].GetString()[0] == 'W';  // ������־�Ƿ���W /// O��ʾί��
	if (!decimal::eq(document["CDSL"].GetDouble(), 0.0))
		return;

	if (isCancel)
	{
		printf("filtering cancel order type");
		return;
	}

	WTSContractInfo* contract = m_bdMgr->getContract(code.c_str(), exchg.c_str());

	if (contract != NULL)
	{
		std::string orderid = std::to_string(document["WTH"].GetInt64());   // ����ʹ��ί�к���Ϊorderid

		WTSCommodityInfo* commInfo = contract->getCommInfo();
		WTSTradeInfo *trdInfo = WTSTradeInfo::create(code.c_str(), exchg.c_str());
		trdInfo->setPrice(document["CJJG"].GetDouble());
		trdInfo->setVolume(document["CJSL"].GetDouble());

		WTSOrderInfo* ordInfo = (WTSOrderInfo*)m_mapLives->get(orderid);
		if (ordInfo)
		{
			ordInfo->setVolTraded(ordInfo->getVolTraded() + trdInfo->getVolume());
			ordInfo->setVolLeft(ordInfo->getVolLeft() - trdInfo->getVolume());

			if (ordInfo->getVolLeft() == 0)
			{
				ordInfo->setOrderState(WOS_AllTraded);
				if (m_traderSink)
					m_traderSink->onPushOrder(ordInfo);
			}
		}

		trdInfo->setTradeID(tradeid.c_str());

		trdInfo->setTradeDate(m_lDate);

		const int len = 256;
		char buf[len];
		strncpy(buf, document["CJSJ"].GetString(), len);
		uint32_t uTime = strToTime(buf);
		trdInfo->setTradeTime(TimeUtils::makeTime(m_lDate, uTime));

		trdInfo->setDirection(WDT_LONG);
		trdInfo->setOffsetType(document["JYLB"].GetInt() == 1 ? WOT_OPEN : WOT_CLOSE);
		trdInfo->setRefOrder(orderid.c_str());
		trdInfo->setTradeType(WTT_Common);

		trdInfo->setAmount(document["CJJE"].GetDouble());

		std::string usertag = m_iniHelper.readString(ORDER_SECTION, StrUtil::trim(trdInfo->getRefOrder()).c_str());
		if (!usertag.empty())
			trdInfo->setUserTag(usertag.c_str());

		if (m_traderSink)
			m_traderSink->onPushTrade(trdInfo);
	}
}

bool TraderMarItpdk::makeEntrustID(char* buffer, int length)
{
	if (buffer == NULL || length == 0)
		return false;

	try
	{
		memset(buffer, 0, length);
		uint32_t orderref = m_orderRef.fetch_add(1) + 1;
		//sprintf(buffer, "%s#%u",StrUtil::fmtUInt64(m_uSessID).c_str(), orderref);
		fmt::format_to(buffer, "{}#{}", m_uSessID, orderref);
		return true;
	}
	catch (...)
	{

	}

	return false;
}

void TraderMarItpdk::registerSpi(ITraderSpi *listener)
{
	m_traderSink = listener;
	if (m_traderSink)
	{
		m_bdMgr = listener->getBaseDataMgr();
	}
}

int TraderMarItpdk::login(const char* user, const char* pass, const char* productInfo)
{
	m_strUser = user;
	m_strPass = pass;

	m_wrapperState = WS_LOGINING;

	doLogin();

	return 0;
}

void TraderMarItpdk::qryGDNo()
{
	m_strandIO->post([this]() {
		long nRet = 0;

		std::vector<ITPDK_XYGDH> argGDH;
		argGDH.reserve(5);

		if (m_bUseEX)
		{
			nRet = (long)MARITPDK_QueryAccInfoEx(cusreqinfo, argGDH);
		}
		else
		{
			nRet = (long)MARITPDK_QueryAccInfo(m_strUser.c_str(), argGDH);
		}

		if (nRet < 0)
		{
			string msg = MARITPDK_GetLastError();              //��ѯʧ�ܣ���ȡ������Ϣ
			write_log(m_traderSink, LL_ERROR, "[TraderMarItpdk]��ѯ�ɶ���Ϣʧ�ܣ� ������Ϣ: {}({}) ", msg.c_str(), nRet);
			return;
		}

		for (auto& itr : argGDH)
		{
			if (strlen(itr.SecuAccount) > 0)
			{
				m_strShGdh = itr.FundAccount;
				m_strSzGdh = itr.FundAccount;
			}
			if (0 == strcmp(itr.Market, "SH"))
			{
				m_strShGdh = itr.SecuAccount;
			}
			if (0 == strcmp(itr.Market, "SZ"))
			{
				m_strSzGdh = itr.SecuAccount;
			}
		}
	});
}

void TraderMarItpdk::doLogin()
{
	m_strandIO->post([this]() {
		if (m_bUseEX)
		{
			strncpy(cusreqinfo.AccountId, m_strUser.c_str(), sizeof(cusreqinfo.AccountId) - 1);
			//strncpy(cusreqinfo.FundAccount, m_strSzZjzh.c_str(), sizeof(cusreqinfo.FundAccount) - 1);
			strncpy(cusreqinfo.Password, m_strPass.c_str(), sizeof(cusreqinfo.Password) - 1);
			strncpy(cusreqinfo.OrgCode, m_strOrgID.c_str(), sizeof(cusreqinfo.OrgCode) - 1);
			//strncpy(cusreqinfo.SecuAccount, m_strSzGdh.c_str(), sizeof(cusreqinfo.SecuAccount) - 1);
		}

		long nRet = -1;

		if (m_nLoginMode == 2)
		{
			nRet = MARITPDK_TradeLoginByFundAcc("A5_RS", m_strFDNO.c_str(), m_strOrgID.c_str(), m_strPass.c_str(), m_strUser);
		}
		else if (m_nLoginMode == 1)
		{
			nRet = MARITPDK_TradeLogin("A5_RS", m_strUser.c_str(), m_strPass.c_str());
		}
		else if (m_nLoginMode == 3)
		{
			nRet = MARITPDK_TradeLoginEx("A5_RS", cusreqinfo);
			m_bUseEX = true;
		}
		else if (m_nLoginMode == 4)
		{
			//bRet = MARITPDK_TradeLoginByFundAccEx("A5_RS", cusreqinfo);
		}
		else
		{
			nRet = MARITPDK_TradeLogin("A5_RS", m_strUser.c_str(), m_strPass.c_str());
		}

		if (nRet <= 0)
		{
			m_wrapperState = WS_LOGINFAILED;
			std::string msg = MARITPDK_GetLastError();  //��¼ʧ�ܣ���ȡ������Ϣ
			m_traderSink->onLoginResult(false, msg.c_str(), 0);
			write_log(m_traderSink, LL_ERROR, "[TraderMarItpdk]��¼ʧ��, ������Ϣ: {}({})", msg.c_str(), nRet);
		}
		else
		{
			m_strToken = cusreqinfo.Token;
			write_log(m_traderSink, LL_INFO, "[TraderMarItpdk]��¼�ɹ��� code: {}, token: {}", nRet, m_strToken);

			std::stringstream ss;
			ss << "./MarItpdkData/local/";
			std::string path = StrUtil::standardisePath(ss.str());
			if (!StdFile::exists(path.c_str()))
				boost::filesystem::create_directories(path.c_str());
			ss << m_strUser << ".dat";

			m_iniHelper.load(ss.str().c_str());
			uint32_t lastDate = m_iniHelper.readUInt("marker", "date", 0);
			if (lastDate != m_lDate)
			{
				//�����ղ�ͬ�������ԭ��������
				m_iniHelper.removeSection(ENTRUST_SECTION);
				m_iniHelper.removeSection(ORDER_SECTION);
				m_iniHelper.writeUInt("marker", "date", m_lDate);
				m_iniHelper.save();

				write_log(m_traderSink, LL_INFO, "[TraderMarItpdk][%s]���������л�[{} -> {}]����ձ������ݻ��桭��", m_strUser.c_str(), lastDate, m_lDate);
			}

			m_wrapperState = WS_LOGINED;
			m_lDate = TimeUtils::getCurDate();

			//qryGDNo();

			write_log(m_traderSink, LL_INFO, "[TraderMarItpdk]�˻����ݳ�ʼ�����...");
			m_wrapperState = WS_ALLREADY;
			m_traderSink->onLoginResult(true, "", m_lDate);
		}
	});
}

int TraderMarItpdk::logout()
{
	MARITPDK_TradeLogout(m_strUser.c_str());

	return 0;
}

int TraderMarItpdk::orderInsert(WTSEntrust* entrust)
{
	if (m_wrapperState != WS_ALLREADY)
	{
		return -1;
	}

	entrust->retain();
	m_strandIO->post([this, entrust]() {
		bool isBuy = entrust->getOffsetType() == WOT_OPEN;
		bool isSH = strcmp(entrust->getExchg(), "SSE") == 0;

		std::string exchg = isSH ? "SH" : "SZ";
		std::string gdh = isSH ? m_strShGdh : m_strSzGdh;
		int nDirection = isBuy ? JYLB_BUY : JYLB_SALE;

		if (m_bUseEX)
			strcpy(cusreqinfo.SecuAccount, gdh.c_str());

		int64 nRet = -1;

		{
			if (m_bASync)
			{
				if (m_bUseEX)
				{
					nRet = MARITPDK_MarginEntrustEx_ASync(cusreqinfo, exchg.c_str(), entrust->getCode(), nDirection, entrust->getVolume(), entrust->getPrice(), DDLX_XJWT, 1, "", 0, 0);
				}
				else
				{
					nRet = MARITPDK_MarginEntrust_ASync(m_strUser.c_str(), exchg.c_str(), entrust->getCode(), nDirection, entrust->getVolume(), entrust->getPrice(), DDLX_XJWT, gdh.c_str(), 1, "", 0, 0);
				}
			}
			else
			{
				if (m_bUseEX)
				{
					nRet = MARITPDK_MarginEntrustEx(cusreqinfo, exchg.c_str(), entrust->getCode(), nDirection, entrust->getVolume(), entrust->getPrice(), DDLX_XJWT, 1, "", 0, 0);
				}
				else
				{
					nRet = MARITPDK_MarginEntrust(m_strUser.c_str(), exchg.c_str(), entrust->getCode(), nDirection, entrust->getVolume(), entrust->getPrice(), DDLX_XJWT, gdh.c_str(), 1, "", 0, 0);
				}
			}

			if (strlen(entrust->getUserTag()) > 0)
			{
				//m_mapEntrustTag[entrust->getEntrustID()] = entrust->getUserTag();
				m_iniHelper.writeString(ENTRUST_SECTION, entrust->getEntrustID(), entrust->getUserTag());
				m_iniHelper.save();
			}

			if (nRet <= 0)
			{
				std::string msg = MARITPDK_GetLastError();
				write_log(m_traderSink, LL_ERROR, "[TraderMarItpdk]ί��ָ���ʧ��: {}({})", msg.c_str(), nRet);

				WTSError* err = WTSError::create(WEC_ORDERINSERT, msg.c_str());
				m_traderSink->onRspEntrust(entrust, err);
			}
			else
			{
				m_traderSink->onRspEntrust(entrust, NULL);

				//�����ֶ���һ�ʻر�����Ȼ���������
				WTSOrderInfo* ordInfo = WTSOrderInfo::create(entrust);
				ordInfo->setOrderState(WOS_NotTraded_NotQueuing);
				ordInfo->setVolTraded(0);
				ordInfo->setVolLeft(ordInfo->getVolume());
				ordInfo->setOrderDate(m_lDate);
				ordInfo->setOrderTime(TimeUtils::getLocalTimeNow());

				ordInfo->setError(false);

				ordInfo->setOrderID(std::to_string(nRet).c_str());  // �˴���ί�к�

				m_iniHelper.writeString(ORDER_SECTION, StrUtil::trim(ordInfo->getOrderID()).c_str(), ordInfo->getUserTag());
				m_iniHelper.save();

				if (m_mapLives == NULL)
					m_mapLives = TradeDataMap::create();

				m_mapLives->add(ordInfo->getOrderID(), ordInfo, false);

				m_traderSink->onPushOrder(ordInfo);
			}
		}

		entrust->release();
	});

	return 0;
}

int TraderMarItpdk::orderAction(WTSEntrustAction* action)
{
	if (m_wrapperState != WS_ALLREADY)
		return -1;

	action->retain();
	m_strandIO->post([this, action]() {
		bool isSH = strcmp(action->getExchg(), "SSE") == 0;
		int64 orderID = atol(action->getOrderID());
		//std::string exchg = isSH ? "SH" : "SZ";

		WTSOrderInfo* ordInfo = (WTSOrderInfo*)m_mapLives->get(action->getOrderID());
		if (ordInfo == NULL)
		{
			write_log(m_traderSink, LL_WARN, "[TraderMarItpdk]δ���ҵ���Լ[{}]��Ϣ", orderID);
			return 0;
		}

		std::string gdh = isSH ? m_strShGdh : m_strSzGdh;

		bool isBuy = ordInfo->getOffsetType() == WOT_OPEN;
		int nDirection = isBuy ? JYLB_BUY : JYLB_SALE;

		int64 nRet = 0;

		/// ʹ�ÿ����̱��ر����Ϊ��������
		if (m_bASync)
		{
			if (m_bUseEX)
			{
				nRet = MARITPDK_MarginOrderWithdrawEx_ASync(cusreqinfo, orderID, nDirection);
			}
			else
			{
				nRet = MARITPDK_MarginOrderWithdraw_ASync(m_strUser.c_str(), gdh.c_str(), orderID, nDirection);
			}
		}
		else
		{
			if (m_bUseEX)
			{
				nRet = MARITPDK_MarginOrderWithdrawEx(cusreqinfo, orderID, nDirection);
			}
			else
			{
				nRet = MARITPDK_MarginOrderWithdraw(m_strUser.c_str(), gdh.c_str(), orderID, nDirection);
			}
		}
		;
		if (nRet <= 0)
		{
			string msg = MARITPDK_GetLastError();          //�µ�ʧ�ܣ���ȡ������Ϣ
			write_log(m_traderSink, LL_ERROR, "[TraderMarItpdk]����ָ���ʧ��: {}({})", msg.c_str(), nRet);

			WTSError* err = WTSError::create(WEC_ORDERCANCEL, msg.c_str());
			m_traderSink->onTraderError(err);
		}
		else
		{
			write_log(m_traderSink, LL_INFO, "[TraderMarItpdk] �����ɹ��� code: {}", nRet);
		}

		action->release();
	});

	return 0;
}

int TraderMarItpdk::queryAccount()
{
	if (m_wrapperState != WS_ALLREADY)
	{
		return -1;
	}

	m_queQuery.push([this]() {
		std::vector<ITPDK_XYZJZH> argFund;
		int64 nRet = 0;

		if (m_bUseEX)
		{
			nRet = (long)MARITPDK_QueryFundInfoEx(cusreqinfo, argFund);
		}
		else
		{
			nRet = (long)MARITPDK_QueryFundInfo(m_strUser.c_str(), argFund);
		}

		if (nRet < 0)  // ��ѯʧ��
		{
			std::string msg = MARITPDK_GetLastError();
			write_log(m_traderSink, LL_ERROR, "[TraderMarItpdk]�����ʽ��ѯʧ��: {}", msg.c_str());
		}
		else  // ��ѯ�ɹ�
		{
			WTSArray* ayFunds = WTSArray::create();

			for (auto& itr : argFund)
			{
				//printf("AccountId:%s;  FundAvl:%f;  TotalAsset:%f;  MarketValue:%f;  CurrentBalance:%f;  UnclearProfit:%f;  DiluteUnclearProfit:%f;  UncomeBalance:%f\n",
				//	itr.AccountId, itr.FundAvl, itr.TotalAsset, itr.MarketValue, itr.CurrentBalance, itr.UnclearProfit, itr.DiluteUnclearProfit, itr.UncomeBalance);

				WTSAccountInfo* fundInfo = WTSAccountInfo::create();
				fundInfo->setAvailable(itr.FundAvl);   /// �����ʽ�
				fundInfo->setBalance(itr.CurrentBalance);  /// �������
				fundInfo->setDynProfit(itr.UnclearProfit);  ///����ӯ��
				fundInfo->setCloseProfit(itr.DateProfit);  /// ����ӯ��
				fundInfo->setCurrency(itr.MoneyType);  /// ����
				fundInfo->setPreBalance(itr.LastBalance);  /// �������

				ayFunds->append(fundInfo, false);
			}

			if (m_traderSink) m_traderSink->onRspAccount(ayFunds);
			ayFunds->release();
		}

		m_bInQuery = false;
	});

	triggerQuery();

	return 0;
}

int TraderMarItpdk::queryPositions()
{
	if (m_wrapperState != WS_ALLREADY)
	{
		return -1;
	}

	StdUniqueLock lock(m_mtxQuery);
	m_queQuery.push([this]() {
		std::vector<ITPDK_XYZQGL> argHolding;
		const size_t nRows = 200;
		argHolding.reserve(nRows);  //��ҪԤ�����㹻�ռ䣬��ѯ�����󷵻�200��

		int64 nRet = 0;
		if (m_bUseEX)
		{
			nRet = MARITPDK_QueryPosition_MR_ZQGLEx(cusreqinfo, 0, SORT_TYPE_AES, 200, 0, "", "", argHolding);
		}
		else
		{
			nRet = MARITPDK_QueryPosition_MR_ZQGL(m_strUser.c_str(), 0, SORT_TYPE_AES, 200, 0, "", "", "", argHolding);  /// execflag�������0
		}

		if (nRet < 0)
		{
			std::string msg = MARITPDK_GetLastError();
			write_log(m_traderSink, LL_ERROR, "[TraderMarItpdk]�û��ֲֲ�ѯʧ��: {}({})", msg.c_str(), nRet);
		}
		else
		{
			// ��������浽һ���ܵ�vector����ڷ�ҳ
			std::vector<ITPDK_XYZQGL> argTotalHolding(argHolding);
			int nPageStart = 200;

			// ��������������200������Ҫ��ҳ��ѯ
			while (nRet >= 200)
			{
				argHolding.clear();

				// ѭ����ѯ��ʱ��nBrowindex+1������һ��
				if (m_bUseEX)
				{
					nRet = MARITPDK_QueryPosition_MR_ZQGLEx(cusreqinfo, 0, SORT_TYPE_AES, nRows, nPageStart, "", "", argHolding);  // ����������
				}
				else
				{
					nRet = MARITPDK_QueryPosition_MR_ZQGL(m_strUser.c_str(), 0, SORT_TYPE_AES, nRows, nPageStart, "", "", "", argHolding);
				}

				// ����ҳ��ѯ������뵽�ܵĽ����
				if (nRet >= 0)
					argTotalHolding.insert(argTotalHolding.end(), argHolding.begin(), argHolding.end());

				nPageStart += 200;
			}

			WTSArray* ayPositions = WTSArray::create();

			for (auto& itr : argTotalHolding)
			{

				const char* exchg = exchgO2I(itr.Market);
				const char* code = itr.StockCode;

				//std::cout << "query positions: " << " code: " << code << " exchange: " << exchg << " dynamic profit: " << itr.UnclearProfit << " qty: " << (long)itr.CurrentQty << " brow index: " << (long)itr.BrowIndex << std::endl;

				WTSContractInfo* contract = m_bdMgr->getContract(code, exchg);
				if (contract)
				{
					WTSCommodityInfo* commInfo = contract->getCommInfo();
					WTSPositionItem* pInfo = WTSPositionItem::create(code, commInfo->getCurrency(), exchg);
					pInfo->setDirection(WDT_LONG);

					double prevol = itr.PreQty;	//����ĳֲ֣������ǲ�����
					double newvol = itr.CurrentQty;	//����ĳֲ֣��������ʵ�ֲ�
					double openvol = itr.RealBuyQty;	//������������
					double closevol = itr.RealSellQty;	//������������

					pInfo->setPrePosition(prevol - closevol);
					pInfo->setNewPosition(openvol);
					pInfo->setAvailPrePos(pInfo->getPrePosition());

					pInfo->setAvgPrice(itr.CostPrice);  /// �ֲ־���
					pInfo->setDynProfit(itr.UnclearProfit);  /// ����ӯ��

					double cost = itr.CostBalance;  /// �ֲֳɱ�
					pInfo->setMargin(cost);
					pInfo->setPositionCost(cost);
					//if (pInfo->getTotalPosition() > 0)
					//	pInfo->setAvgPrice(cost / pInfo->getTotalPosition());

					ayPositions->append(pInfo, false);
				}
			}

			if (m_traderSink) m_traderSink->onRspPosition(ayPositions);
			ayPositions->release();
		}

		m_bInQuery = false;
	});

	triggerQuery();

	return 0;
}

int TraderMarItpdk::queryOrders()
{
	if (m_wrapperState != WS_ALLREADY)
	{
		return -1;
	}

	StdUniqueLock lock(m_mtxQuery);
	m_queQuery.push([this]() {
		std::vector<ITPDK_XYDRWT> argOrders;
		const size_t nRows = 200;
		argOrders.reserve(nRows);  //��ҪԤ�����㹻�ռ䣬��ѯ�����󷵻�200��
		
		int64 nRet = 0;
		if (m_bUseEX)
		{
			nRet = MARITPDK_QueryOrders_MR_DRWTEx(cusreqinfo, 0, SORT_TYPE_AES, 200, 0, "", "", 0, "", 0, argOrders);
		}
		else
		{
			nRet = MARITPDK_QueryOrders_MR_DRWT(m_strUser.c_str(), 0, SORT_TYPE_AES, 200, 0, "", "", 0, "", 0, argOrders);
		}

		if (nRet < 0)  /// ��ѯʧ��
		{
			std::string msg = MARITPDK_GetLastError();
			write_log(m_traderSink, LL_ERROR, "[TraderMarItpdk]�û�������ѯʧ��: {}({})", msg.c_str(), nRet);
		}
		else
		{
			// ��������浽һ���ܵ�vector����ڷ�ҳ
			std::vector<ITPDK_XYDRWT> argTotalOrders(argOrders);
			int nPageStart = nRows;

			// ������������200�����ҳ��ѯ
			while (nRet >= nRows)
			{
				argOrders.clear();

				// ѭ����ѯ��ʱ��nBrowindex+1������һ��
				if (m_bUseEX)
				{
					nRet = MARITPDK_QueryOrders_MR_DRWTEx(cusreqinfo, 0, SORT_TYPE_AES, nRows, nPageStart, "", "", 0, "", 0, argOrders);  // ����������
				}
				else
				{
					nRet = MARITPDK_QueryOrders_MR_DRWT(m_strUser.c_str(), 0, SORT_TYPE_AES, nRows, nPageStart, "", "", 0, "", 0, argOrders);
				}

				// ����ҳ��ѯ������뵽�ܵĽ����
				if (nRet >= 0)
					argTotalOrders.insert(argTotalOrders.end(), argOrders.begin(), argOrders.end());
				
				nPageStart += nRows;
			}

			WTSArray* ayOrds = WTSArray::create();

			for (auto& itr : argTotalOrders)
			{
				//printf("�˻�: %s, ί�к�: %ld, �����̱��ر��: %s, �걨ί�к�: %s, ����: %s, ������: %s, �۸�: %f, ����: %ld, ��ѯҳ��: %ld \n",
				//	itr.AccountId, (long)itr.OrderId, itr.KfsOrderNum, itr.SBWTH, itr.StockCode, itr.Market, itr.OrderPrice, (long)itr.OrderQty, (long)itr.BrowIndex);

				const char* exchg = exchgO2I(itr.Market);
				const char* code = itr.StockCode;

				WTSContractInfo* contract = m_bdMgr->getContract(code, exchg);
				if (contract)
				{
					WTSCommodityInfo* commInfo = contract->getCommInfo();
					WTSOrderInfo* ordInfo = WTSOrderInfo::create();
					ordInfo->setCode(code);
					ordInfo->setExchange(exchg);

					ordInfo->setPrice(itr.OrderPrice);
					ordInfo->setDirection(WDT_LONG);
					ordInfo->setPriceType(WPT_LIMITPRICE);  //TODO
					ordInfo->setOffsetType(itr.EntrustType == 1 ? WOT_OPEN : WOT_CLOSE);

					double total = itr.OrderQty;
					double traded = itr.MatchQty;
					double canceled = itr.WithdrawQty;
					ordInfo->setVolume(total);
					ordInfo->setVolTraded(traded);
					ordInfo->setVolLeft(total - canceled - traded);

					ordInfo->setOrderDate((uint32_t)itr.EntrustDate);  /// ί������

					uint32_t uTime = strToTime(itr.EntrustTime);   /// ί��ʱ��
					ordInfo->setOrderTime(TimeUtils::makeTime(ordInfo->getOrderDate(), uTime));

					int state = itr.OrderStatus;
					ordInfo->setOrderState(wrapOrderState(state));
					if (state == 3)
						ordInfo->setError(true);

					ordInfo->setEntrustID(itr.KfsOrderNum);  /// ����ʹ�ÿ����̱�� 

					ordInfo->setOrderID(std::to_string(itr.OrderId).c_str());  /// ����ʹ��ί�к�

					ordInfo->setStateMsg(itr.ResultInfo);  /// ���˵��

					std::string usertag = m_iniHelper.readString(ENTRUST_SECTION, ordInfo->getEntrustID(), "");
					if (usertag.empty())
					{
						ordInfo->setUserTag(ordInfo->getEntrustID());
					}
					else
					{
						ordInfo->setUserTag(usertag.c_str());

						if (strlen(ordInfo->getOrderID()) > 0)
						{
							m_iniHelper.writeString(ORDER_SECTION, StrUtil::trim(ordInfo->getOrderID()).c_str(), usertag.c_str());
							m_iniHelper.save();
						}
					}

					if (ordInfo->isAlive())
					{
						if (m_mapLives == NULL)
							m_mapLives = TradeDataMap::create();

						m_mapLives->add(ordInfo->getOrderID(), ordInfo, true);
					}

					ayOrds->append(ordInfo, false);
				}
			}


			if (m_traderSink) m_traderSink->onRspOrders(ayOrds);
			ayOrds->release();
		}

		m_bInQuery = false;
	});

	triggerQuery();

	return 0;
}

int TraderMarItpdk::queryTrades()
{
	if (m_wrapperState != WS_ALLREADY)
	{
		return -1;
	}

	StdUniqueLock lock(m_mtxQuery);
	m_queQuery.push([this]() {
		std::vector<ITPDK_XYSSCJ> argTrades;
		const size_t nRows = 200;
		argTrades.reserve(nRows);  //��ҪԤ�����㹻�ռ䣬��ѯ�����󷵻�200��

		int64 nRet = 0;
		if (m_bUseEX)
		{
			nRet = MARITPDK_QueryMatchs_MR_SSCJEx(cusreqinfo, 0, SORT_TYPE_AES, nRows, 0, "", "", 0, 0, argTrades);  // ����������
		}
		else
		{
			nRet = MARITPDK_QueryMatchs_MR_SSCJ(m_strUser.c_str(), 0, SORT_TYPE_AES, nRows, 0, "", "", 0, 0, argTrades);
		}

		if (nRet < 0)
		{
			std::string msg = MARITPDK_GetLastError();
			write_log(m_traderSink, LL_ERROR, "[TraderMarItpdk]�û��ɽ���ѯʧ��: {}({})", msg.c_str(), nRet);
		}
		else
		{
			// ��������浽һ���ܵ�vector����ڷ�ҳ
			std::vector<ITPDK_XYSSCJ> argTotalTrades(argTrades);
			int nPageStart = 200;

			// ������������200�����ҳ��ѯ
			while (nRet >= 200)
			{
				argTrades.clear();

				if (m_bUseEX)
				{
					nRet = MARITPDK_QueryMatchs_MR_SSCJEx(cusreqinfo, 0, SORT_TYPE_AES, nRows, nPageStart, "", "", 0, 0, argTrades);  // ����������
				}
				else
				{
					nRet = MARITPDK_QueryMatchs_MR_SSCJ(m_strUser.c_str(), 0, SORT_TYPE_AES, nRows, nPageStart, "", "", 0, 0, argTrades);
				}

				// ����ҳ��ѯ������뵽�ܵĽ����
				if (nRet >= 0)
					argTotalTrades.insert(argTotalTrades.end(), argTrades.begin(), argTrades.end());

				nPageStart += 200;
			}

			WTSArray* ayTrds = WTSArray::create();

			int index = 0;
			for (auto& itr : argTotalTrades)
			{
				//printf("HBXH:%I64d ;StockName:%s; WTH:%I64d; StockCode:%s; JYS:%s; CJSL:%I64d; CJBH:%s; BrowIndex:%I64d\n",
				//	itr.ReportSerialNo, itr.StockName, itr.OrderId, itr.StockCode, itr.Market, itr.MatchQty, itr.MatchSerialNo, itr.BrowIndex);

				const char* exchg = exchgO2I(itr.Market);
				const char* code = itr.StockCode;

				WTSContractInfo* contract = m_bdMgr->getContract(code, exchg);
				if (contract)
				{
					WTSCommodityInfo* commInfo = contract->getCommInfo();
					WTSTradeInfo *trdInfo = WTSTradeInfo::create(code, exchg);
					trdInfo->setPrice(itr.MatchPrice);
					trdInfo->setVolume(itr.MatchQty);
					trdInfo->setTradeID(itr.MatchSerialNo);  // ����ʹ�óɽ����

					trdInfo->setTradeDate(m_lDate);
					trdInfo->setTradeTime(strToTime(itr.MatchTime));

					// #################### ��Ҫ����###############
					trdInfo->setDirection(WDT_LONG);
					trdInfo->setOffsetType(itr.EntrustType == 1 ? WOT_OPEN : WOT_CLOSE);
					trdInfo->setRefOrder(itr.KfsOrderNum);  // ����ʹ�ÿ����̱��
					trdInfo->setTradeType(WTT_Common);

					trdInfo->setAmount(itr.MatchAmt);

					std::string usertag = m_iniHelper.readString(ORDER_SECTION, StrUtil::trim(trdInfo->getRefOrder()).c_str());
					if (!usertag.empty())
						trdInfo->setUserTag(usertag.c_str());

					ayTrds->append(trdInfo, false);
				}
			}

			if (m_traderSink) m_traderSink->onRspTrades(ayTrds);
			ayTrds->release();
		}

		m_bInQuery = false;
	});

	triggerQuery();

	return 0;
}

bool TraderMarItpdk::isConnected()
{
	return (m_wrapperState == WS_ALLREADY);
}

void TraderMarItpdk::triggerQuery()
{
	m_strandIO->post([this]() {
		if (m_queQuery.empty() || m_bInQuery)
			return;

		//����ӿں���û���������ƣ���ȥ��
		//uint64_t curTime = TimeUtils::getLocalTimeNow();
		//if (curTime - m_lastQryTime < 1000)
		//{
		//	boost::this_thread::sleep(boost::posix_time::milliseconds(50));
		//	m_strandIO->post([this](){
		//		triggerQuery();
		//	});
		//	return;
		//}

		m_bInQuery = true;
		CommonExecuter& handler = m_queQuery.front();
		handler();

		{
			StdUniqueLock lock(m_mtxQuery);
			m_queQuery.pop();
		}

		m_lastQryTime = TimeUtils::getLocalTimeNow();
	});
}

MarItpdkCallMgr::MarItpdkCallMgr()
{

}

MarItpdkCallMgr::~MarItpdkCallMgr()
{

}

bool MarItpdkCallMgr::init(WTSVariant* params)
{
	bool b_inited = false;

	if (params)
	{
		printf("CMAITPDK Version: %s \n", MARITPDK_GetVersion().c_str());

		MARITPDK_SetLogPath("./MarItpdk/Logs/");  // ��־·����MARITPDK_Init����ǰ������Ч
		MARITPDK_SetProfilePath("./");  //  ���������ļ�·����MARITPDK_Init����ǰ������Ч


		b_inited = MARITPDK_Init(0);   //��ʼ���������нӿ�ʹ��ǰ���ã���·�����ýӿ���

		if (b_inited)
		{
			MARITPDK_SetWriteLog(true);  // �Ƿ��¼��־
			MARITPDK_SetFixWriteLog(true);  // �Ƿ��¼Fix��־

			MARITPDK_SetWTFS(params->getCString("order_way"));               //����ί�з�ʽ
			MARITPDK_SetNode(params->getCString("node"));               //����վ����Ϣ
		}
	}

	return b_inited;
}

void MarItpdkCallMgr::setHTSCallPtr(std::string khh, TraderMarItpdk *pTrader)
{
	if (itpdkCallMap.find(khh) == itpdkCallMap.end())
	{
		//std::cout << "Current key not existed, insert it to address map." << std::endl;
		setCallbackMsgFunc();  // ������Ϣ

		itpdkCallMap.insert(std::pair<string, TraderMarItpdk *>(khh, pTrader));
	}
}

void MarItpdkCallMgr::callbackMsgFunc(const char* pTime, const char* pMsg, int nType)
{
	// �Ƚ������ݽ���
	//std::cout << "MarItpdkCallMgr callbackMsgFunc begins ..." << std::endl;

	rapidjson::Document document;
	if (document.Parse(pMsg).HasParseError())
	{
		cout << "HTSCallManage callbackMsgFunc Parse confirm message failed." << endl;
		return;
	}

	//long nWTH = (long)document["WTH"].GetInt64();
	const char* sKHH = document["KHH"].GetString();

	//����itpdkCallMap
	for (auto iter = itpdkCallMap.begin(); iter != itpdkCallMap.end(); ++iter) {
		string sTmp = iter->first;
		if (strcmp(sTmp.c_str(), sKHH) == 0) {
			TraderMarItpdk* pTrader = iter->second;
			if (pTrader != NULL) {
				std::cout << "HTSCallManage get report message khh: " << iter->first << "  " << iter->second << std::endl;

				//�˴����������Ľ���������ݷ�װ��һ���ṹ����ٴ���HTSCall��ʵ���У��������ν������˷���Դ���˴�ֻ�Ǽ�ʾ��������û�ж���ر��ṹ��
				pTrader->htsGetCallback(pTime, pMsg, nType);
			}
		}
	}
}

void MarItpdkCallMgr::callbackConnFunc(const char* pKhh, const char* pConnKey, int nEvent, void* pData)
{
	if (nEvent != 0)
	{
		TraderMarItpdk* pTrader = (TraderMarItpdk*)pData;
		if (pTrader != NULL)
		{
			printf("[TraderMarItpdk] ���ڳ��Իָ�����, �ͻ��� khh: %s\n", pKhh);

			//�˴����������Ľ���������ݷ�װ��һ���ṹ����ٴ���HTSCall��ʵ���У��������ν������˷���Դ���˴�ֻ�Ǽ�ʾ��������û�ж���ر��ṹ��
			pTrader->connect();
		}
	}
}

void MarItpdkCallMgr::setCallbackMsgFunc()
{
	//���ûص�����
	MARITPDK_SetConnEventCallback(&MarItpdkCallMgr::callbackConnFunc);  // �������ӻص�
	MARITPDK_SetMsgCallback(&MarItpdkCallMgr::callbackMsgFunc);  // ������Ϣ���ͻص�
	MARITPDK_SetFuncCallback(&MarItpdkCallMgr::callbackMsgFunc);  // �����첽���ͻص�
}
