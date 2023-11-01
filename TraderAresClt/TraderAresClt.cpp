/*!
 * \file TraderAresClt.cpp
 * \project	WonderTrader
 *
 * \author Suuuuunseeker
 * \date 2022/08/11
 *
 * \brief
 */
#include "TraderAresClt.h"

#include "../Includes/WTSError.hpp"
#include "../Includes/WTSContractInfo.hpp"
#include "../Includes/WTSSessionInfo.hpp"
#include "../Includes/WTSTradeDef.hpp"
#include "../Includes/WTSDataDef.hpp"
#include "../Includes/WTSVariant.hpp"
#include "../Includes/IBaseDataMgr.h"

#include "../Share/ModuleHelper.hpp"
#include "../Share/decimal.h"

#include <boost/filesystem.hpp>
#include <iostream>

#include <boost/thread/thread.hpp>

const char* ENTRUST_SECTION = "entrusts";
const char* ORDER_SECTION = "orders";

//By Wesley @ 2022.01.05
#include "../Share/fmtlib.h"
template<typename... Args>
inline void write_log(ITraderSpi* sink, WTSLogLevel ll, const char* format, const Args&... args)
{
	if (sink == NULL)
		return;

	const char* buffer = fmtutil::format(format, args...);

	sink->handleTraderLog(ll, buffer);
}

extern "C"
{
	EXPORT_FLAG ITraderApi* createTrader()
	{
		TraderAresClt *instance = new TraderAresClt();
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

inline void split(const std::string& str, const std::string& delim, std::vector<std::string> &res)
{
	if (str.empty())
		return;

	char * strs = new char[str.length() + 1];
	strcpy(strs, str.c_str());

	char * sep = new char[delim.length() + 1];
	strcpy(sep, delim.c_str());

	char * next_tok = NULL;
	char * p = strtok_s(strs, sep, &next_tok);

	while (p)
	{
		std::string ret = p;
		res.push_back(ret);
		p = strtok_s(NULL, sep, &next_tok);
	}
}

inline char wrapDirectionType(WTSDirectionType dirType, WTSOffsetType offsetType)
{
	if (WDT_LONG == dirType)
		if (offsetType == WOT_OPEN)
			return '0';
		else
			return '1';
	else
		if (offsetType == WOT_OPEN)
			return '1';
		else
			return '0';
}

//inline char wrapPosDirType(WTSDirectionType dirType)
//{
//	if (WDT_LONG == dirType)
//		return '2';
//	else if (WDT_SHORT == dirType)
//		return '3';
//}
//
//inline WTSDirectionType wrapPosDirType(char dirType)
//{
//	if ('2' == dirType)
//		return WDT_LONG;
//	else if ('3' == dirType)
//		return WDT_SHORT;
//	else
//		return WDT_NET;
//}

inline WTSDirectionType wrapDirectionType(char dirType, char offsetType)
{
	if ('0' == dirType)  // ��
		if (offsetType == '0')  // ����
			return WDT_LONG;
		else
			return WDT_SHORT;
	else  // ��
		if (offsetType == '0')  // ����
			return WDT_SHORT;
		else
			return WDT_LONG;
}

inline WTSDirectionType wrapPosDirection(char dirType)
{
	if ('2' == dirType)  // ��ͷ
		return WDT_LONG;
	else  // ��ͷ
		return WDT_SHORT;
}

inline char wrapOffsetType(WTSOffsetType offType)
{
	if (WOT_OPEN == offType)
		return '0';
	else if (WOT_CLOSE == offType)
		return '1';
	else if (WOT_CLOSETODAY == offType)
		return '3';
	else if (WOT_CLOSEYESTERDAY == offType)
		return '4';
	else
		return '2';
}

inline WTSOffsetType wrapOffsetType(char offType)
{
	if ('0' == offType)
		return WOT_OPEN;
	else if ('1' == offType)
		return WOT_CLOSE;
	else if ('3' == offType)
		return WOT_CLOSETODAY;
	else if ('4' == offType)
		return WOT_CLOSEYESTERDAY;
	else
		return WOT_FORCECLOSE;
}

inline int wrapPriceType(WTSPriceType priceType, bool isCFFEX /* = false */)
{
	if (WPT_ANYPRICE == priceType)
		return isCFFEX ? 'I' : '1';
	else if (WPT_LIMITPRICE == priceType)
		return isCFFEX ? 'B' : '2';
	else if (WPT_BESTPRICE == priceType)
		return 'F';
	else
		return 'L';
}

inline WTSPriceType wrapPriceType(char priceType)
{
	if ('1' == priceType || 'C' == priceType || 'D' == priceType || 'E' == priceType)
		return WPT_ANYPRICE;
	else if ('2' == priceType || 'A' == priceType || 'B' == priceType)
		return WPT_LIMITPRICE;
	else if ('L' == priceType || 'M' == priceType || 'N' == priceType)
		return WPT_LASTPRICE;
	else
		return WPT_BESTPRICE;
}

inline WTSOrderState wrapOrderState(char orderState)
{
	if (orderState == '1')  // �ѳɽ�
		return WOS_AllTraded;
	else if (orderState == '2')  // ����
		return WOS_NotTraded_NotQueuing;
	else if (orderState == '3')  // �ѱ�
		return WOS_NotTraded_Queuing;
	else if (orderState == '4')  // ����
		return WOS_NotTraded_NotQueuing;
	else if (orderState == '5')  // �ѳ�
		return WOS_Canceled;
	else if (orderState == 'a')  // �ϵ�
		return WOS_Canceled;
	else  // ����
		return WOS_Submitting;
}

inline uint32_t makeRefID()
{
	static std::atomic<uint32_t> auto_refid(0);
	if (auto_refid == 0)
	{
		//auto_refid = (uint32_t)((TimeUtils::getLocalTimeNow() - TimeUtils::makeTime(20221201, 0)) / 1000 * 10);
		uint32_t curDate = TimeUtils::getCurDate();
		curDate = curDate / 100 * 100 + 1;
		auto_refid = (uint32_t)((TimeUtils::getLocalTimeNow() - TimeUtils::makeTime(curDate, 0)) / 1000 * 50);
	}
	return auto_refid.fetch_add(1);
}

TraderAresClt::TraderAresClt()
	: m_pUserAPI(NULL)
	, m_pApp(NULL)
	, m_mapPosition(NULL)
	, m_ayOrders(NULL)
	, m_ayTrades(NULL)
	, m_ayPosDetail(NULL)
	, m_wrapperState(WS_NOTLOGIN)
	, m_uLastQryTime(0)
	, m_iRequestID(0)
	, m_bscSink(NULL)
	, m_bInQuery(false)
	, m_bStopped(NULL)
	, m_lastQryTime(0)
	, m_pfnCreateObj(NULL)
	, m_pfnDestroyObj(NULL)
	, m_orderRef(makeRefID())
{

}


TraderAresClt::~TraderAresClt()
{

}


bool TraderAresClt::init(WTSVariant* params)
{
	m_nLine = params->getInt32("line");
	m_strUserType = params->get("user_type")->asCString();
	m_strUserID = params->get("user")->asCString();
	m_strInvestorID = params->get("investor_id")->asCString();
	m_strPasswd = params->get("pass")->asCString();

	m_strIP = params->get("IP")->asCString();
	m_nPort = params->getInt32("Port");

	m_strFlowDir = params->getCString("flowdir");

	if (m_strFlowDir.empty())
		m_strFlowDir = "AresTDFlow";

	m_lDate = TimeUtils::getCurDate();

	m_strFlowDir = StrUtil::standardisePath(m_strFlowDir);

	WTSVariant* param = params->get("aresmodule");
	if (param != NULL)
		m_strModule = getBinDir() + DLLHelper::wrap_module(param->asCString(), "");
	else
		m_strModule = getBinDir() + DLLHelper::wrap_module("ErebusTdCpp", "");

	m_hInstAres = DLLHelper::load_library(m_strModule.c_str());

	m_pfnCreateObj = (pfnCreateObj)::GetProcAddress(m_hInstAres, "CreateTdObj");
	m_pfnDestroyObj = (pfnDestroyObj)::GetProcAddress(m_hInstAres, "DestroyTdObj");

	return true;
}

void TraderAresClt::release()
{
	if (m_pUserAPI)
	{
		m_pUserAPI->RegisterSpi(NULL);
		m_pUserAPI->ReqLogout();
		FreeApi(m_pUserAPI);
		//m_pUserAPI = NULL;
	}

	if (m_pApp)
	{
		m_pApp->EndWork();
		FreeApp(m_pApp);
		//m_pApp = NULL;
	}

	if (m_hInstAres)
	{
		::FreeLibrary(m_hInstAres);
		m_pfnCreateObj = 0;
		m_pfnDestroyObj = 0;
		m_hInstAres = 0;
	}

	if (m_ayOrders)
		m_ayOrders->clear();

	if (m_ayPosDetail)
		m_ayPosDetail->clear();

	if (m_mapPosition)
		m_mapPosition->clear();

	if (m_ayTrades)
		m_ayTrades->clear();
}

void TraderAresClt::connect()
{
	if (m_pfnCreateObj)
	{
		// ��������
		void* p = m_pfnCreateObj("ErebusTdAPP", 0);
		if (p)
		{
			m_pApp = (ITDErebusApp*)p;
		}

		// ����API
		char szParam[255] = { 0 };
		_snprintf(szParam, 254, "{IP:%s;Port:%d}", m_strIP.c_str(), m_nPort);
		p = m_pfnCreateObj("ErebusTdApi", szParam);
		if (p)
		{
			m_pUserAPI = (ITDErebusApi*)p;
		}
	}

	m_pUserAPI->RegisterSpi(this);

	if (m_pApp)
	{
		int iRet = m_pApp->BeginWork(0);

		if (iRet < 0)
		{
			// ����ʧ��
			write_log(m_bscSink, LL_ERROR, "[TraderAresClt] Creating environment failed, code: {}", iRet);
		}
	}

	if (m_thrdWorker == NULL)
	{
		boost::asio::io_service::work work(_asyncio);
		m_thrdWorker.reset(new StdThread([this]() {
			while (true)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(2));
				_asyncio.run_one();
				//m_asyncIO.run();
			}
		}));
	}

	_asyncio.post([this]() {
		if (m_bscSink)
			m_bscSink->handleEvent(WTE_Connect, 0);
	});
}

void TraderAresClt::disconnect()
{
	//std::cout << "disconnect ..." << std::endl;

	//if (m_thrdWorker)
	//{
	//	m_thrdWorker->join();
	//	m_thrdWorker = NULL;
	//}

	release();
}

bool TraderAresClt::makeEntrustID(char* buffer, int length)
{
	if (buffer == NULL || length == 0)
		return false;

	try
	{
		memset(buffer, 0, length);
		uint32_t orderref = m_orderRef.fetch_add(1) + 1;
		fmtutil::format_to(buffer, "{}#{}#{}", m_strUserID, m_nLine, orderref);
		return true;
	}
	catch (...)
	{

	}

	return false;
}

void TraderAresClt::registerSpi(ITraderSpi *listener)
{
	m_bscSink = listener;
	if (m_bscSink)
	{
		m_bdMgr = listener->getBaseDataMgr();
	}
}

uint32_t TraderAresClt::genRequestID()
{
	return m_iRequestID.fetch_add(1) + 1;
}

int TraderAresClt::login(const char* user, const char* pass, const char* productInfo)
{
	m_strUserID = user;
	m_strPasswd = pass;

	if (m_pUserAPI == NULL)
	{
		return -1;
	}

	doLogin();

	return 0;
}

int TraderAresClt::doLogin()
{
	m_wrapperState = WS_LOGINING;

	tagXTReqUserLoginField req = { 0 };
	{
		memset(&req, 0, sizeof(req));

		req.Line = m_nLine;
		req.UserType[0] = m_strUserType.at(0);

		strcpy(req.UserID, m_strUserID.c_str());
		strcpy(req.InvestorID, m_strInvestorID.c_str());
		strcpy(req.Password, m_strPasswd.c_str());  // ʹ���ʽ��˻������¼
	}

	int iResult = m_pUserAPI->ReqLogon(&req);
	if (iResult <= 0)
	{
		std::string msg = m_strErrInfo;

		write_log(m_bscSink, LL_ERROR, "[TraderAresClt] Sending login request failed: {}", msg);
		m_wrapperState = WS_LOGINFAILED;

		_asyncio.post([this, msg] {
			m_bscSink->onLoginResult(false, msg.c_str(), 0);
		});
	}
	else
	{
		write_log(m_bscSink, LL_INFO, "[TraderAresClt] Sending login request successed: {}", iResult);

		//m_wrapperState = WS_LOGINED;

		//_asyncio.post([this] {
		//	m_wrapperState = WS_ALLREADY;
		//	m_bscSink->onLoginResult(true, 0, m_lDate);
		//});
	}

	return 0;
}

int TraderAresClt::logout()
{
	if (m_pUserAPI == NULL)
	{
		return -1;
	}

	m_pUserAPI->ReqLogout();

	return 0;
}

int TraderAresClt::orderInsert(WTSEntrust* entrust)
{
	if (m_pUserAPI == NULL || m_wrapperState != WS_ALLREADY)
	{
		return -1;
	}

	WTSContractInfo* ct = entrust->getContractInfo();
	if (ct != NULL)
	{
		tagXTReqOrderInsertField req;
		memset(&req, 0, sizeof(req));

		///�û�����
		req.Line = m_nLine;
		req.UserType[0] = m_strUserType.at(0);
		strcpy(req.UserID, m_strUserID.c_str());
		///Ͷ���ߴ���
		strcpy(req.InvestorID, m_strInvestorID.c_str());
		///��Լ����
		strcpy(req.Code, entrust->getCode());
		/// ����������
		strcpy(req.Exchange, entrust->getExchg());

		if (strlen(entrust->getUserTag()) == 0)
		{
			///��������
			fmt::format_to(req.OrderRef, "{}", m_orderRef.fetch_add(0));
		}
		else
		{
			uint32_t orderref;
			extractEntrustID(entrust->getEntrustID(), orderref);
			///��������
			fmt::format_to(req.OrderRef, "{}", orderref);
		}

		if (strlen(entrust->getUserTag()) > 0)
		{
			m_iniHelper.writeString(ENTRUST_SECTION, entrust->getEntrustID(), entrust->getUserTag());
			m_iniHelper.save();
		}

		WTSCommodityInfo* commInfo = ct->getCommInfo();

		///�û�����
		///�����۸�����: �޼�
		req.PriceType[0] = wrapPriceType(entrust->getPriceType(), strcmp(commInfo->getExchg(), "CFFEX") == 0);
		///��������: 
		req.Direction[0] = wrapDirectionType(entrust->getDirection(), entrust->getOffsetType());
		///��Ͽ�ƽ��־: ����
		req.Offset[0] = wrapOffsetType(entrust->getOffsetType());
		///���Ͷ���ױ���־
		req.Hedge[0] = '1';  // Ͷ��
		///�۸�
		req.LimitPrice = entrust->getPrice();
		///����: 1
		req.VolumeOrigin = (int)entrust->getVolume();

		int iResult = m_pUserAPI->ReqOrderInsert(&req);
		if (iResult <= 0)
		{
			write_log(m_bscSink, LL_ERROR, "[TraderAresClt] Order inserting failed: {}", iResult);
		}
		else
		{
			//write_log(m_bscSink, LL_DEBUG, "[OrderInsert] code: {}, volume: {}, price: {}, orderref: {}, usertag: {}, entrustid: {}", req.Code, req.VolumeOrigin, req.LimitPrice, req.OrderRef, entrust->getUserTag(), entrust->getEntrustID());
		}

		return 0;
	}

	write_log(m_bscSink, LL_ERROR, "[TraderAresClt] Order inserting failed: Invalid contract!");
	return -1;
}

int TraderAresClt::orderAction(WTSEntrustAction* action)
{
	if (m_wrapperState != WS_ALLREADY)
		return -1;


	uint32_t orderref;

	if (extractEntrustID(action->getEntrustID(), orderref))
	{
		tagXTReqOrderCancelField req;
		memset(&req, 0, sizeof(req));
		///���͹�˾����
		wt_strcpy(req.UserID, m_strUserID.c_str(), m_strUserID.size());
		wt_strcpy(req.InvestorID, m_strInvestorID.c_str(), m_strInvestorID.size());
		///��������
		fmt::format_to(req.ActionRef, "{}", orderref);
		///������
		strcpy(req.OrderSysID, action->getOrderID());
		///��Լ����
		strcpy(req.Code, action->getCode());
		strcpy(req.Exchange, action->getExchg());

		//write_log(m_bscSink, LL_INFO, "[orderAction]code: {}, actionRef: {}, orderSysID: {}, entrustid: {}", req.Code, req.ActionRef, req.OrderSysID, action->getEntrustID());

		int iResult = m_pUserAPI->ReqOrderCancel(&req);
		if (iResult <= 0)
		{
			write_log(m_bscSink, LL_ERROR, "[TraderAresClt] Sending cancel request failed: {}", iResult);
		}

		return 0;
	}

	return -1;
}

int TraderAresClt::queryAccount()
{
	if (m_pUserAPI == NULL || m_wrapperState != WS_ALLREADY)
	{
		write_log(m_bscSink, LL_ERROR, "[TraderAresClt] Trading api is null or system is not ready");
		return -1;
	}

	tagXTReqQryAccountField req;
	memset(&req, 0, sizeof(req));

	req.Line = m_nLine;
	req.UserType[0] = m_strUserType.at(0);
	strncpy(req.UserID, m_strUserID.c_str(), sizeof(req.UserID));
	strncpy(req.InvestorID, m_strInvestorID.c_str(), sizeof(req.InvestorID));
	int iRet = m_pUserAPI->ReqQryAccount(&req);

	if (iRet <= 0)
	{
		write_log(m_bscSink, LL_ERROR, "[TraderAresClt] Account querying failed: {}", iRet);
	}
;
	return 0;
}

int TraderAresClt::queryPositions()
{
	if (m_pUserAPI == NULL || m_wrapperState != WS_ALLREADY)
	{
		write_log(m_bscSink, LL_ERROR, "[TraderAresClt] Trading api is null or system is not ready");
		return -1;
	}

	tagXTReqQryPositionField req;
	memset(&req, 0, sizeof(req));

	strcpy(req.UserID, m_strUserID.c_str());
	strcpy(req.InvestorID, m_strInvestorID.c_str());
	int iRet = m_pUserAPI->ReqQryPosition(&req);

	if (iRet <= 0)
		write_log(m_bscSink, LL_ERROR, "[TraderAresClt] Failed to query position info, Code: {}", iRet);
	//else
	//	write_log(m_bscSink, LL_INFO, "[TraderAresClt] Succeeded to query position info, Code: {}", iRet);

	return 0;
}

int TraderAresClt::queryOrders()
{
	if (m_pUserAPI == NULL || m_wrapperState != WS_ALLREADY)
	{
		return -1;
	}

	tagXTReqQryOrderField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.UserID, m_strUserID.c_str());
	strcpy(req.InvestorID, m_strInvestorID.c_str());

	int iRet = m_pUserAPI->ReqQryOrder(&req);
	if (iRet <= 0)
	{
		write_log(m_bscSink, LL_ERROR, "[TraderAresClt]ReqQryOrder failed, Code: {}", iRet);
	}

	return 0;
}

int TraderAresClt::queryTrades()
{
	if (m_pUserAPI == NULL || m_wrapperState != WS_ALLREADY)
	{
		return -1;
	}

	tagXTReqQryTradeField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.UserID, m_strUserID.c_str());
	strcpy(req.InvestorID, m_strInvestorID.c_str());

	int iRet = m_pUserAPI->ReqQryTrade(&req);
	if (iRet <= 0)
	{
		write_log(m_bscSink, LL_ERROR, "[TraderAresClt]ReqQryTrade failed, Code: {}", iRet);
	}

	return 0;
}

int TraderAresClt::querySettlement(uint32_t uDate)
{
	if (m_pUserAPI == NULL || m_wrapperState != WS_ALLREADY)
	{
		return -1;
	}

	_asyncio.post([this, uDate]() {
		if (m_bscSink)
			m_bscSink->onRspSettlementInfo(uDate, "No settlement data");
	});

	return 0;
}

bool TraderAresClt::IsErrorRspInfo(tagXTRspInfoField *pRspInfo)
{
	if (pRspInfo && pRspInfo->ErrorID != 0)
		return true;

	return false;
}

void TraderAresClt::OnFrontConnected()
{
	/// �ȵ���ReLoginOn���Ż�����������ӣ����ӳɹ���ص��˺���
	write_log(m_bscSink, LL_INFO, "[TraderAresClt] Server connected");
}

void TraderAresClt::OnFrontDisconnected()
{
	m_wrapperState = WS_NOTLOGIN;
	if (m_bscSink)
		m_bscSink->handleEvent(WTE_Close, -1);

	// �Ͽ�����
	_asyncio.post([this]() {
		write_log(m_bscSink, LL_WARN, "[TraderAresClt] Connection lost, relogin in 2 seconds...");
		std::this_thread::sleep_for(std::chrono::seconds(2));
		doLogin();

		// �Ͽ�����֮����Ҫ����һ�β�ѯί�кͳɽ�
		// ���ڵ�¼��ʱ���Ѿ�������һ��ί�У���������ֻ��Ҫ����һ�γɽ�
		std::this_thread::sleep_for(std::chrono::seconds(2));

		{
			tagXTReqQryPositionField req;
			memset(&req, 0, sizeof(req));

			strcpy(req.UserID, m_strUserID.c_str());
			strcpy(req.InvestorID, m_strInvestorID.c_str());
			int iRet = m_pUserAPI->ReqQryPosition(&req);

			if (iRet <= 0)
				write_log(m_bscSink, LL_ERROR, "[TraderAresClt] Failed to query position info, Code: {}", iRet);
			//else
				//write_log(m_bscSink, LL_DEBUG, "[TraderAresClt] Succeeded to query position info, Code: {}", iRet);
		}

		std::this_thread::sleep_for(std::chrono::seconds(1));
	});
}

void TraderAresClt::OnRspUserLogin(tagXTRspUserLoginField* pRspUserLogin, tagXTRspInfoField* error, int id, bool last)
{
	if (error)
		m_strErrInfo = error->ErrorMsg;

	if (!IsErrorRspInfo(error))
	{
		_asyncio.post([this]() {
			m_wrapperState = WS_LOGINED;

			{
				///��ȡ��ǰ������
				m_lDate = TimeUtils::getCurDate();

				write_log(m_bscSink, LL_INFO, "[TraderAresClt][{}-{}] Login succeed, sessionid: {}, trading date: {}...", m_strUserID.c_str(), m_strInvestorID.c_str(), m_nLine, m_lDate);

				{
					std::stringstream ss;
					ss << m_strFlowDir << "local/" << m_strUserID << "/";
					std::string path = StrUtil::standardisePath(ss.str());
					if (!StdFile::exists(path.c_str()))
						boost::filesystem::create_directories(path.c_str());
					ss << m_strUserID << ".dat";

					m_iniHelper.load(ss.str().c_str());

					uint32_t lastDate = m_iniHelper.readUInt("marker", "date", 0);
					if (lastDate != m_lDate)
					{
						//�����ղ�ͬ,�����ԭ��������
						m_iniHelper.removeSection(ENTRUST_SECTION);
						m_iniHelper.removeSection(ORDER_SECTION);
						m_iniHelper.writeUInt("marker", "date", m_lDate);
						m_iniHelper.save();

						write_log(m_bscSink, LL_INFO, "[TraderAresClt][{}-{}] Trading date changed [{} -> {}], local cache cleared...", m_strUserID.c_str(), m_strInvestorID.c_str(), lastDate, m_lDate);
					}
				}
			}

			m_wrapperState = WS_ALLREADY;

			if (m_bscSink)
				m_bscSink->onLoginResult(true, 0, m_lDate);

			// ��¼��ʱ�򼴲�ѯһ��
			{
				tagXTReqQryOrderField req;
				memset(&req, 0, sizeof(req));
				strcpy(req.UserID, m_strUserID.c_str());
				strcpy(req.InvestorID, m_strInvestorID.c_str());

				int iRet = m_pUserAPI->ReqQryOrder(&req);
				if (iRet <= 0)
				{
					write_log(m_bscSink, LL_ERROR, "[TraderAresClt]ReqQryOrder failed, {}", iRet);
				}
				else
				{
					write_log(m_bscSink, LL_DEBUG, "[TraderAresClt]ReqQryOrder successed, {}", iRet);
				}
			}

			std::this_thread::sleep_for(std::chrono::seconds(5));  // �ֲֲ�ѯ�ص���Ҫʱ�仺������
		});
	}
	else
	{
		write_log(m_bscSink, LL_ERROR, "[TraderAresClt][{}-{}] Login failed: {}", m_strUserID.c_str(), m_strInvestorID.c_str(), error->ErrorMsg);
		m_wrapperState = WS_LOGINFAILED;

		if (m_bscSink)
			m_bscSink->onLoginResult(false, error->ErrorMsg, 0);
	}
}

void TraderAresClt::OnRspOrderInsert(tagXTReqOrderInsertField* data, tagXTRspInfoField* error, int id, bool last)
{
	WTSEntrust* entrust = makeEntrust(data);

	if (entrust)
	{
		//write_log(m_bscSink, LL_INFO, "[OnRspOrderInsert][{}] code: {}, volume: {}, price: {}, id: {}, order ref: {}, usertag: {}, entrustid: {}",
		//	data->UserID, data->Code, data->VolumeOrigin, data->LimitPrice, id, data->OrderRef, entrust->getUserTag(), entrust->getEntrustID());

		if (IsErrorRspInfo(error))
		{
			WTSError *err = makeError(error, WEC_ORDERINSERT);
			if (m_bscSink)
				m_bscSink->onRspEntrust(entrust, err);
			err->release();
		}
		else
		{
			if (m_bscSink)
				m_bscSink->onRspEntrust(entrust, NULL);
		}
	}
	else if (IsErrorRspInfo(error))
	{
		WTSError *err = makeError(error, WEC_ORDERINSERT);
		if (m_bscSink)
			m_bscSink->onTraderError(err);
		err->release();
	}

	if (entrust)
		entrust->release();
}

void TraderAresClt::OnRspOrderAction(tagXTReqOrderCancelField* data, tagXTRspInfoField* pRspInfo, int id, bool last)
{
	if (IsErrorRspInfo(pRspInfo))
	{
		//write_log(m_bscSink, LL_INFO, "[TraderAresClt][OnRspOrderAction] Cancell order failed, msg: {}", pRspInfo->ErrorMsg);
		WTSError* error = WTSError::create(WEC_ORDERCANCEL, pRspInfo->ErrorMsg);
		if (m_bscSink)
			m_bscSink->onTraderError(error);
	}
	else
	{
		//write_log(m_bscSink, LL_DEBUG, "[TraderAresClt][OnRspOrderAction] Cancell order successed, msg: {}", pRspInfo->ErrorMsg);
	}
}

void TraderAresClt::OnErrRtnOrderAction(tagXTReqOrderCancelField* orderCancelField, tagXTRspInfoField* pRspInfo)
{
	WTSEntrust* entrust = makeEntrust(orderCancelField);
	if (entrust)
	{
		WTSError *err = makeError(pRspInfo, WEC_ORDERCANCEL);
		if (m_bscSink)
			m_bscSink->onRspEntrust(entrust, err);

		entrust->release();
		err->release();
	}
}

void TraderAresClt::OnRspQryTradingAccount(tagXTRspAccountField* pTradingAccount, tagXTRspInfoField* pRspInfo, int id, bool bIsLast)
{
	if (pRspInfo)
	{
		m_strErrInfo = pRspInfo->ErrorMsg;
		//write_log(m_bscSink, LL_INFO, "OnRspQryTradingAccount: {}", pRspInfo->ErrorMsg);
	}

	if (bIsLast)
	{
		m_bInQuery = false;
	}

	if (bIsLast && !IsErrorRspInfo(pRspInfo) && pTradingAccount)
	{
		WTSAccountInfo* accountInfo = WTSAccountInfo::create();
		accountInfo->setPreBalance(0);
		accountInfo->setCloseProfit(0);
		accountInfo->setDynProfit(0);
		accountInfo->setMargin(0);
		accountInfo->setAvailable(pTradingAccount->Available);
		accountInfo->setCommission(0);
		accountInfo->setFrozenMargin(pTradingAccount->FrozenMargin);
		//accountInfo->setFrozenCommission(pTradingAccount->FrozenCommission);
		accountInfo->setDeposit(0);
		accountInfo->setWithdraw(0);
		accountInfo->setBalance(accountInfo->getPreBalance() + accountInfo->getCloseProfit() - accountInfo->getCommission() + accountInfo->getDeposit() - accountInfo->getWithdraw());
		accountInfo->setCurrency("CNY");

		WTSArray * ay = WTSArray::create();
		ay->append(accountInfo, false);
		if (m_bscSink)
			m_bscSink->onRspAccount(ay);

		ay->release();
	}
}

void TraderAresClt::OnRspQryInvestorPosition(tagXTRspPositionField* pInvestorPosition, tagXTRspInfoField* pRspInfo, int id, bool bIsLast)
{
	if (pRspInfo)
	{
		m_strErrInfo = pRspInfo->ErrorMsg;
	}

	if (bIsLast)
	{
		m_bInQuery = false;
		//triggerQuery();
	}

	if (!IsErrorRspInfo(pRspInfo) && pInvestorPosition)
	{
		if (NULL == m_mapPosition)
			m_mapPosition = PositionMap::create();

		WTSContractInfo* contract = m_bdMgr->getContract(pInvestorPosition->Code, pInvestorPosition->Exchange);

		if (contract)
		{
			WTSCommodityInfo* commInfo = contract->getCommInfo();
			std::string key = fmt::format("{}-{}", pInvestorPosition->Code, pInvestorPosition->PosiDirection);

			WTSPositionItem* pos = (WTSPositionItem*)m_mapPosition->get(key);
			if (pos == NULL)
			{
				pos = WTSPositionItem::create(pInvestorPosition->Code, commInfo->getCurrency(), commInfo->getExchg());
				pos->setContractInfo(contract);
				m_mapPosition->add(key, pos, false);
			}
			pos->setDirection(wrapPosDirection(pInvestorPosition->PosiDirection[0]));
			pos->setNewPosition(pInvestorPosition->Position);  // ���ܳ�
			pos->setPrePosition(pInvestorPosition->YdPosition);  // ���ܳ�

			pos->setMargin(pos->getMargin() + pInvestorPosition->UseMargin);
			pos->setPositionCost(pos->getPositionCost() + pInvestorPosition->PositionCost);

			if (pos->getTotalPosition() != 0)
			{
				pos->setAvgPrice(pos->getPositionCost() / pos->getTotalPosition() / commInfo->getVolScale());
			}
			else
			{
				pos->setAvgPrice(0);
			}

			if (commInfo->getCategoty() != CC_Combination)
			{
				int availPre = pInvestorPosition->YdPosition;
				int frzPos = pInvestorPosition->FrozenPosition;
				pos->setAvailPrePos(frzPos);  // ���ｫ����ó�ʼ��Ϊ����ֲ�

				int availNew = pInvestorPosition->Position;
				pos->setAvailNewPos(availNew);  // ������ó�ʼ��Ϊ���ܳ�
			}
			else
			{

			}

			//write_log(m_bscSink, LL_DEBUG, "[OnRspTradePosition][{}]Code: {}, CurTotalPos: {}, YdPos: {}, frzPos: {}, Side: {}", pInvestorPosition->UserID, pInvestorPosition->Code, pInvestorPosition->Position, pInvestorPosition->YdPosition, pInvestorPosition->FrozenPosition, pInvestorPosition->PosiDirection);
		}
	}

	if (bIsLast)
	{
		_asyncio.post([this] {
			WTSArray* ayPos = WTSArray::create();

			if (m_mapPosition && m_mapPosition->size() > 0)
			{
				for (auto it = m_mapPosition->begin(); it != m_mapPosition->end(); it++)
				{
					WTSPositionItem* pos = (WTSPositionItem*)it->second;

					const char* code = pos->getCode();
					char direction = (pos->getDirection() == WDT_LONG) ? '0' : '1';

					int tdOpen{ 0 }, coveredPos{ 0 }, unCoveredPos{ 0 };  // ����ƽ��,δƽ
					double availNewPos = pos->getNewPosition();   // ���ܳ�
					double availPrePos = pos->getPrePosition();  // ��ֲ�
					double frzPos = pos->getAvailPrePos();  // ����ֲ�

					if (umapOrderPos.size() > 0)
					{
						for (auto it1 = umapOrderPos.begin(); it1 != umapOrderPos.end(); it1++)
						{
							std::string key = (std::string)it1->first;
							std::vector<std::string> res;
							res.reserve(4);

							split(key, "#", res);

							if (strcmp(pos->getCode(), res.at(0).c_str()) != 0)
								continue;

							if (strcmp(res.at(3).c_str(), "0") == 0)  // ����
							{
								if (direction == res.at(2)[0])  // ����һ��
								{
									tdOpen += umapOrderPos[key][1];
								}
							}
							else //if (strcmp(res.at(3).c_str(), "1") == 0)  // ƽ��,�����ֿ�ƽ��־����ƽ����ƽ��
							{
								if (direction != res.at(2)[0])
								{
									availNewPos -= umapOrderPos[key][0];
									unCoveredPos += umapOrderPos[key][0];

									if (umapOrderPos[key][3] == 5)  // �ѳ�
									{
										availNewPos += umapOrderPos[key][2];  // ƽ��δ����Ҫ�ӻ�ȥ
										unCoveredPos -= umapOrderPos[key][2];
										coveredPos += umapOrderPos[key][1];
									}
								}
							}
						}
					}

					// ����� = ��� - max(0, ���� - ��)
					double availPre = pos->getPrePosition() - (frzPos <= tdOpen ? 0 : (frzPos - tdOpen));
					if (availPre < 0)
						availPre = 0;

					pos->setAvailPrePos(availPre);

					// ����� = ��ֲ� + �� - ����ֲ�
					availNewPos = pos->getPrePosition() + tdOpen - frzPos;
					// ���ﻹ��Ҫ��ȥ����ã����5�֣�������δ����ʱ�����5�֣������ӦΪ0
					availNewPos -= availPre;
					if (availNewPos < 0)
						availNewPos = 0;

					pos->setAvailNewPos(availNewPos);

					string direc = "Long";
					if (direction != '0')
						direc = "Short";

					/*write_log(m_bscSink, LL_DEBUG, "Code: {}, Direction: {}, tdOpen: {}, frzPos: {}, availNew: {}, availPre: {}",
						code, direc, tdOpen, frzPos, availNewPos, availPre);*/

					if (decimal::lt(pos->getTotalPosition(), 0.0) && decimal::eq(pos->getMargin(), 0.0))
					{
						//�в�λ,���Ǳ�֤��Ϊ0,��˵����������Լ,������Լ�Ŀ��óֲ�ȫ����Ϊ0
						pos->setAvailNewPos(0);
						pos->setAvailPrePos(0);
					}

					//ayPos->append(it->second, true);
					ayPos->append(pos, true);
				}
			}

			if (m_bscSink)
				m_bscSink->onRspPosition(ayPos);


			if (m_mapPosition)
			{
				m_mapPosition->release();
				m_mapPosition = NULL;
			}

			ayPos->release();
		});
	}
}

void TraderAresClt::OnRspQryTrade(tagXTTradeField *pTrade, tagXTRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (bIsLast)
	{
		m_bInQuery = false;
		//triggerQuery();
	}

	if (!IsErrorRspInfo(pRspInfo) && pTrade)
	{
		//write_log(m_bscSink, LL_DEBUG, "[OnRspQryTrade][{}]code: {}, direction: {}, offset: {}, price: {}, volume: {}, ordersysid: {}, orderref: {}", pTrade->UserID, pTrade->Code, pTrade->Direction, pTrade->Offset, pTrade->Price, pTrade->Volume, pTrade->OrderSysID, pTrade->OrderRef);

		if (NULL == m_ayTrades)
			m_ayTrades = WTSArray::create();

		WTSTradeInfo* trade = makeTradeRecord(pTrade);
		if (trade)
		{
			m_ayTrades->append(trade, false);
		}
	}

	if (bIsLast)
	{
		if (m_bscSink)
			m_bscSink->onRspTrades(m_ayTrades);

		if (NULL != m_ayTrades)
			m_ayTrades->clear();
	}
}

void TraderAresClt::OnRspQryOrder(tagXTOrderField *pOrder, tagXTRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (pRspInfo)
		m_strErrInfo = pRspInfo->ErrorMsg;

	if (bIsLast)
	{
		m_bInQuery = false;
	}

	if (!IsErrorRspInfo(pRspInfo) && pOrder)
	{
		if (NULL == m_ayOrders)
			m_ayOrders = WTSArray::create();

		{
			std::string key = fmt::format("{}#{}#{}#{}", pOrder->Code, pOrder->OrderSysID, pOrder->Direction, pOrder->Offset);

			if (umapOrderPos.count(key) == 0)
			{
				// ���û�д��ڱ���
				std::vector<int> vecOrderPos;  // ��¼ÿ��OrderID��Volume, VolTraded, VolRemain, ����״̬
				vecOrderPos.reserve(4);

				vecOrderPos.push_back(pOrder->VolumeOrigin);
				vecOrderPos.push_back(pOrder->VolumeTraded);
				vecOrderPos.push_back(pOrder->VolumeRemain);

				int orderStat;
				if (pOrder->OrderStatus[0] == 'a')
					orderStat = 6;
				else if (pOrder->OrderStatus[0] == 'b')
					orderStat = 7;
				else
					orderStat = (int)pOrder->OrderStatus;
				vecOrderPos.push_back(orderStat);

				umapOrderPos[key] = vecOrderPos;
			}
			else
			{
				// ����һ�λ�ȡ��ǰ�ĺ�ԼOrderSysId
				// ͬһ��OrderSysIdֻȡ�ɽ������Ǹ�Order
				if (pOrder->VolumeTraded > umapOrderPos[key].at(1))
				{
					umapOrderPos[key][0] = pOrder->VolumeOrigin;
					umapOrderPos[key][1] = pOrder->VolumeTraded;
					umapOrderPos[key][2] = pOrder->VolumeRemain;

					int orderStat;
					if (pOrder->OrderStatus[0] == 'a')
						orderStat = 6;
					else if (pOrder->OrderStatus[0] == 'b')
						orderStat = 7;
					else
						orderStat = (int)pOrder->OrderStatus;
					umapOrderPos[key][3] = orderStat;
				}
			}

			//write_log(m_bscSink, LL_DEBUG, "[OnRspQryOrder][Insert] Key: {}, VolTotal: {}, VolTraded: {}, VolLeft: {}, OrderStat: {}",
			//	key, umapOrderPos[key][0], umapOrderPos[key][1], umapOrderPos[key][2], umapOrderPos[key][3]);
		}

		WTSOrderInfo* orderInfo = makeOrderInfo(pOrder);
		if (orderInfo)
		{
			m_ayOrders->append(orderInfo, false);
		}
	}

	if (bIsLast)
	{
		if (m_bscSink)
			m_bscSink->onRspOrders(m_ayOrders);

		if (m_ayOrders)
			m_ayOrders->clear();
	}
}

void TraderAresClt::OnRtnOrder(tagXTOrderField *pOrder)
{
	if (pOrder == NULL)
	{
		return;
	}
	// ������͵Ķ����Ѿ���ɣ�������
	if (m_OrderIdMap.count(pOrder->OrderSysID) == 0)
	{
		// ÿ����һ��Order������һ��
		{
			std::string key = fmt::format("{}#{}#{}#{}", pOrder->Code, pOrder->OrderSysID, pOrder->Direction, pOrder->Offset);

			if (umapOrderPos.count(key) == 0)
			{
				// ���û�д��ڱ���
				std::vector<int> vecOrderPos;  // ��¼ÿ��OrderID��Volume, VolTraded, VolRemain
				vecOrderPos.reserve(4);

				vecOrderPos.push_back(pOrder->VolumeOrigin);
				vecOrderPos.push_back(pOrder->VolumeTraded);
				vecOrderPos.push_back(pOrder->VolumeRemain);

				int orderStat;
				if (pOrder->OrderStatus[0] == 'a')
					orderStat = 6;
				else if (pOrder->OrderStatus[0] == 'b')
					orderStat = 7;
				else
					orderStat = (int)pOrder->OrderStatus;
				vecOrderPos.push_back(orderStat);

				umapOrderPos[key] = vecOrderPos;
			}
			else
			{
				// ����һ�λ�ȡ��ǰ�ĺ�ԼOrderSysId
				// ͬһ��OrderSysIdֻȡ�ɽ������Ǹ�Order
				if (pOrder->VolumeTraded > umapOrderPos[key].at(1))
				{
					umapOrderPos[key][0] = pOrder->VolumeOrigin;
					umapOrderPos[key][1] = pOrder->VolumeTraded;
					umapOrderPos[key][2] = pOrder->VolumeRemain;

					int orderStat;
					if (pOrder->OrderStatus[0] == 'a')
						orderStat = 6;
					else if (pOrder->OrderStatus[0] == 'b')
						orderStat = 7;
					else
						orderStat = (int)pOrder->OrderStatus;
					umapOrderPos[key][3] = orderStat;
				}
			}
		}

		WTSOrderInfo *orderInfo = makeOrderInfo(pOrder);

		if (orderInfo)
		{
			_asyncio.post([this, orderInfo] {
				if (m_bscSink)
					m_bscSink->onPushOrder(orderInfo);

				orderInfo->release();
			});
		}
	}
	else
	{
		//write_log(m_bscSink, LL_WARN, "[OnRtnOrder][{}] has finished, Code: {}, OrderState: {}, Value: {}", pOrder->OrderSysID, pOrder->Code, pOrder->OrderStatus, m_OrderIdMap.at(pOrder->OrderSysID));
	}
}

void TraderAresClt::OnRtnTrade(tagXTTradeField *pTrade)
{
	if (pTrade == NULL)
		return;

	WTSTradeInfo *tRecord = makeTradeRecord(pTrade);
	if (tRecord)
	{
		_asyncio.post([this, tRecord] {
			std::this_thread::sleep_for(std::chrono::milliseconds(10));

			if (m_bscSink)
				m_bscSink->onPushTrade(tRecord);

			tRecord->release();
		});
	}
}


WTSOrderInfo* TraderAresClt::makeOrderInfo(tagXTOrderField* orderField)
{
	if (orderField == NULL)
	{
		write_log(m_bscSink, LL_ERROR, "[makeOrderInfo][tagXTReqOrderField] orderField is NULL!");
		return NULL;
	}

	WTSContractInfo* contract = m_bdMgr->getContract(orderField->Code, orderField->Exchange);
	if (contract == NULL)
	{
		//write_log(m_bscSink, LL_ERROR, "[makeOrderInfo][tagXTReqOrderField] Contract is NULL! Code: {}, OrderRef: {}, OrderSysID: {}, OrderState: {}, Side: {}", orderField->Code, orderField->OrderRef, orderField->OrderSysID, orderField->OrderStatus, orderField->Direction);
		return NULL;
	}

	WTSOrderInfo* pRet = WTSOrderInfo::create();
	pRet->setPrice(orderField->LimitPrice);
	pRet->setVolume(orderField->VolumeOrigin);
	pRet->setDirection(wrapDirectionType(orderField->Direction[0], orderField->Offset[0]));
	pRet->setPriceType(wrapPriceType(orderField->PriceType[0]));
	pRet->setOffsetType(wrapOffsetType(orderField->Offset[0]));
	pRet->setContractInfo(contract);

	pRet->setOrderFlag(WOF_NOR);

	pRet->setVolTraded(orderField->VolumeTraded);
	pRet->setVolLeft(orderField->VolumeRemain);

	pRet->setCode(orderField->Code);
	pRet->setExchange(contract->getExchg());

	pRet->setOrderDate(orderField->InsertDate);
	pRet->setOrderTime(orderField->InsertTime);

	pRet->setOrderState(wrapOrderState(orderField->OrderStatus[0]));  // ��������״̬
	//pRet->setEntrustID(generateEntrustID(orderField->FrontID, orderField->SessionID, atoi(orderField->OrderRef)).c_str());
	generateEntrustID(pRet->getEntrustID(), atoi(orderField->OrderRef));
	pRet->setOrderID(orderField->OrderSysID);

	pRet->setStateMsg(orderField->StatusMsg);

	if (orderField->OrderStatus[0] == 'a')  // �ϵ�
	{
		pRet->setError(true);
		pRet->setStateMsg("Abandoned");

		//m_OrderIdMap.emplace(std::make_pair(orderField->OrderSysID, "Abandoned"));
	}
	else if (orderField->OrderStatus[0] == '2')  // ����
	{
		//pRet->setError(true);
		pRet->setOrderState(WOS_NotTraded_NotQueuing);
		pRet->setStateMsg("Submit_Waited");
		//m_OrderIdMap.emplace(std::make_pair(orderField->OrderSysID, "Submit_Waited"));
	}
	else if (orderField->OrderStatus[0] == '4')  // ����
	{
		pRet->setOrderState(WOS_Cancelling);
		pRet->setStateMsg("Canceling");
	}
	else if (orderField->OrderStatus[0] == '5')  // �ѳ�
	{
		pRet->setOrderState(WOS_Canceled);
		pRet->setStateMsg("Canceled");
		//m_OrderIdMap.emplace(std::make_pair(orderField->OrderSysID, "Canceled"));
	}
	else if (orderField->OrderStatus[0] == '1')  // �ѳɽ�
	{
		//if (orderField->VolumeRemain == 0)  // ��̨���ܻ��η�����Ϣ����ʣ������Ϊ0ʱ���������������Ѿ�����
		//	m_OrderIdMap.emplace(std::make_pair(orderField->OrderSysID, "Traded"));
	}

	std::string usertag1 = m_iniHelper.readString(ENTRUST_SECTION, pRet->getEntrustID(), "");
	if (usertag1.empty())
	{
		pRet->setUserTag(pRet->getEntrustID());
	}
	else
	{
		pRet->setUserTag(usertag1.c_str());

		if (strlen(pRet->getOrderID()) > 0)
		{
			m_iniHelper.writeString(ORDER_SECTION, StrUtil::trim(pRet->getOrderID()).c_str(), usertag1.c_str());
			m_iniHelper.save();
			//write_log(m_bscSink, LL_INFO, "[makeOrderInfo][{}][m_iniHelper]saving order_section: orderid: {}, usertag: {}", orderField->UserID, pRet->getOrderID(), usertag1);
		}
	}

	// �жϵ�ǰ�����Ƿ��Ѿ���ɣ����⵱��̨�ر���Ϣ����˳������ʱ���ظ�����
	if (m_OrderIdMap.count(orderField->OrderSysID) == 0)
	{
		if (orderField->OrderStatus[0] == 'a')  // �ϵ�
		{
			m_OrderIdMap.emplace(std::make_pair(orderField->OrderSysID, "Abandoned"));
		}
		else if (orderField->OrderStatus[0] == '2')  // ����
		{
			//m_OrderIdMap.emplace(std::make_pair(orderField->OrderSysID, "Submit_Waited"));
		}
		else if (orderField->OrderStatus[0] == '5')  // �ѳ�
		{
			m_OrderIdMap.emplace(std::make_pair(orderField->OrderSysID, "Canceled"));
		}
		else if (orderField->OrderStatus[0] == '1')  // �ѳɽ�
		{
			if (orderField->VolumeRemain == 0)  // ��̨���ܻ��η�����Ϣ����ʣ������Ϊ0ʱ���������������Ѿ�����
				m_OrderIdMap.emplace(std::make_pair(orderField->OrderSysID, "Traded"));
		}
	}

	//write_log(m_bscSink, LL_INFO, "[makeOrderInfo] orderstatus: {}, orderid: {}, usertag: {}, entrustid: {}, init orderref: {}", orderField->OrderStatus, pRet->getOrderID(), usertag1, pRet->getEntrustID(), orderField->OrderRef);
	//write_log(m_bscSink, LL_DEBUG, "[OnRtnOrder] VolumeTotal: {}, VolLeft: {}, VolTraded: {}", orderField->VolumeOrigin, orderField->VolumeRemain, orderField->VolumeTraded);
	return pRet;
}

WTSEntrust* TraderAresClt::makeEntrust(tagXTReqOrderInsertField *entrustField)
{
	if (entrustField == NULL)
	{
		write_log(m_bscSink, LL_ERROR, "[makeEntrust][tagXTReqOrderInsertField] entrustField is NULL!");
		return NULL;
	}

	WTSContractInfo* ct = m_bdMgr->getContract(entrustField->Code, entrustField->Exchange);
	if (ct == NULL)
	{
		//write_log(m_bscSink, LL_ERROR, "[makeEntrust][tagXTReqOrderInsertField] Contract is NULL! Code: {}, OrderRef: {}", entrustField->Code, entrustField->OrderRef);
		return NULL;
	}

	WTSEntrust* pRet = WTSEntrust::create(
		entrustField->Code,
		entrustField->VolumeOrigin,
		entrustField->LimitPrice,
		ct->getExchg());

	pRet->setContractInfo(ct);
	pRet->setDirection(wrapDirectionType(entrustField->Direction[0], entrustField->Offset[0]));
	pRet->setPriceType(wrapPriceType(entrustField->PriceType[0]));
	pRet->setOffsetType(wrapOffsetType(entrustField->Offset[0]));

	pRet->setOrderFlag(WOF_NOR);

	//pRet->setEntrustID(generateEntrustID(m_frontID, m_sessionID, atoi(entrustField->OrderRef)).c_str());
	std::string tmp = entrustField->OrderRef;

	uint32_t ul;
	char * endptr;
	ul = strtoul(entrustField->OrderRef, &endptr, 10);
	generateEntrustID(pRet->getEntrustID(), ul);
	//write_log(m_bscSink, LL_INFO, "[makeEntrust] raw orderref: {}, strtoul ordref: {}", tmp, ul);

	//StringMap::iterator it = m_mapEntrustTag.find(pRet->getEntrustID());
	//if (it != m_mapEntrustTag.end())
	//{
	//	pRet->setUserTag(it->second.c_str());
	//}

	std::string usertag = m_iniHelper.readString(ENTRUST_SECTION, pRet->getEntrustID());
	if (!usertag.empty())
		pRet->setUserTag(usertag.c_str());

	//write_log(m_bscSink, LL_INFO, "[makeEnturst][tagXTReqOrderInsertField]code: {}, entrustid: {}, usertag: {}, init orderref: {}", pRet->getCode(), pRet->getEntrustID(), pRet->getUserTag(), entrustField->OrderRef);

	return pRet;
}

WTSEntrust* TraderAresClt::makeEntrust(tagXTReqOrderCancelField *entrustField)
{
	if (entrustField == NULL)
	{
		//write_log(m_bscSink, LL_ERROR, "[makeEntrust][tagXTReqOrderCancelField] entrustField is NULL!");
		return NULL;
	}

	WTSContractInfo* ct = m_bdMgr->getContract(entrustField->Code, entrustField->Exchange);
	if (ct == NULL)
	{
		//write_log(m_bscSink, LL_ERROR, "[makeEntrust][tagXTReqOrderCancelField] Contract is NULL! Code: {}, OrderSysID: {}", entrustField->Code, entrustField->OrderSysID);
		return NULL;
	}

	WTSEntrust* pRet = WTSEntrust::create(
		entrustField->Code,
		0,
		0,
		ct->getExchg());

	pRet->setContractInfo(ct);

	generateEntrustID(pRet->getEntrustID(), atoi(entrustField->OrderSysID));

	//StringMap::iterator it = m_mapEntrustTag.find(pRet->getEntrustID());
	//if (it != m_mapEntrustTag.end())
	//{
	//	pRet->setUserTag(it->second.c_str());
	//}

	std::string usertag = m_iniHelper.readString(ENTRUST_SECTION, pRet->getEntrustID());
	if (!usertag.empty())
		pRet->setUserTag(usertag.c_str());

	//write_log(m_bscSink, LL_INFO, "[makeEnturst][tagXTReqOrderCancelField][{}]code: {}, entrustid: {}, usertag: {}", entrustField->UserID, pRet->getCode(), pRet->getEntrustID(), pRet->getUserTag());

	return pRet;
}

WTSError* TraderAresClt::makeError(tagXTRspInfoField* rspInfo)
{
	WTSError* pRet = WTSError::create((WTSErroCode)rspInfo->ErrorID, rspInfo->ErrorMsg);
	return pRet;
}

WTSError* TraderAresClt::makeError(tagXTRspInfoField* rspInfo, WTSErroCode ec)
{
	WTSError* pRet = WTSError::create(ec, rspInfo->ErrorMsg);
	return pRet;
}

WTSTradeInfo* TraderAresClt::makeTradeRecord(tagXTTradeField *tradeField)
{
	if (tradeField == NULL)
		return NULL;

	WTSContractInfo* contract = m_bdMgr->getContract(tradeField->Code, tradeField->Exchange);
	if (contract == NULL)
		return NULL;

	//write_log(m_bscSink, LL_INFO, "[OnRtnTrade] Code: {}, OrderRef: {}, Vol: {}, Price: {}, Time: {}, TrdID: {}, OrdSysID: {}",
	//	tradeField->Code, tradeField->OrderRef, tradeField->Volume, tradeField->Price, tradeField->TradeTime, tradeField->TradeID, tradeField->OrderSysID);

	WTSCommodityInfo* commInfo = contract->getCommInfo();

	WTSTradeInfo *pRet = WTSTradeInfo::create(tradeField->Code, commInfo->getExchg());
	pRet->setVolume(tradeField->Volume);
	pRet->setPrice(tradeField->Price);
	pRet->setTradeID(tradeField->TradeID);
	pRet->setContractInfo(contract);

	uint32_t uTime = tradeField->TradeTime;
	uint32_t uDate = tradeField->TradeDate;

	//if(uDate == m_pContractMgr->getTradingDate())
	//{
	//	//�����ǰ���ںͽ�����һ��,��ʱ�����21��,˵����ҹ��,Ҳ����ʵ������Ҫ��������
	//	if (uTime / 10000 >= 21)
	//	{
	//		uDate = m_pMarketMgr->getPrevTDate(commInfo->getExchg(), uDate, 1);
	//	}
	//	else if(uTime <= 3)
	//	{
	//		//�����3������,��Ҫ�Ȼ�ȡ��һ��������,�ٻ�ȡ��һ����Ȼ��
	//		//��������Ŀ����,�����������ϵ����,���Դ������
	//		uDate = m_pMarketMgr->getPrevTDate(commInfo->getExchg(), uDate, 1);
	//		uDate = TimeUtils::getNextDate(uDate);
	//	}
	//}

	pRet->setTradeDate(uDate);
	pRet->setTradeTime(TimeUtils::makeTime(uDate, uTime * 1000));

	WTSDirectionType dType = wrapDirectionType(tradeField->Direction[0], tradeField->Offset[0]);

	pRet->setDirection(dType);
	pRet->setOffsetType(wrapOffsetType(tradeField->Offset[0]));
	pRet->setRefOrder(tradeField->OrderSysID);
	//pRet->setTradeType((WTSTradeType)tradeField->TradeType);

	double amount = commInfo->getVolScale() * tradeField->Volume * pRet->getPrice();
	pRet->setAmount(amount);

	//StringMap::iterator it = m_mapOrderTag.find(pRet->getRefOrder());
	//if (it != m_mapOrderTag.end())
	//{
	//	pRet->setUserTag(it->second.c_str());
	//}

	if (strlen(tradeField->OrderRef) > 0)
	{
		thread_local char entrustid[64] = { 0 };
		generateEntrustID(entrustid, atoi(tradeField->OrderRef));
		std::string usertag = m_iniHelper.readString(ENTRUST_SECTION, entrustid);
		if (!usertag.empty())
			pRet->setUserTag(usertag.c_str());
	}
	else
	{
		std::string usertag = m_iniHelper.readString(ORDER_SECTION, StrUtil::trim(pRet->getRefOrder()).c_str());
		if (!usertag.empty())
			pRet->setUserTag(usertag.c_str());
	}

	//write_log(m_bscSink, LL_INFO, "[Trade Record] code {} | price {} | volume {} | trade ordersysid {} | set reforder {} | usertag {}", pRet->getCode(), pRet->getPrice(), pRet->getVolume(), tradeField->OrderSysID, pRet->getRefOrder(), pRet->getUserTag());

	return pRet;
}

void TraderAresClt::generateEntrustID(char* buffer, uint32_t orderRef)
{
	fmtutil::format_to(buffer, "{}#{}#{}", m_strUserID, m_nLine, orderRef);
	//write_log(m_bscSink, LL_INFO, "[generateEntrustID]generate entrustid: {}, orderref: {}", buffer, orderRef);
}

bool TraderAresClt::extractEntrustID(const char* entrustid, uint32_t &orderRef)
{
	auto idx = StrUtil::findLast(entrustid, '#');
	if (idx == std::string::npos)
		return false;

	orderRef = strtoul(entrustid + idx + 1, NULL, 10);

	//write_log(m_bscSink, LL_INFO, "[extractEntrustID]entrust id: {}, extract entrust id: {}", entrustid, orderRef);

	return true;
}

void TraderAresClt::OnErrRtnOrderInsert(tagXTReqOrderInsertField *pInputOrder, tagXTRspInfoField *pRspInfo)
{
	if (pRspInfo)
		m_strErrInfo = pRspInfo->ErrorMsg;

	WTSEntrust* entrust = makeEntrust(pInputOrder);
	if (entrust)
	{
		WTSError *err = makeError(pRspInfo, WEC_ORDERINSERT);
		if (m_bscSink)
			m_bscSink->onRspEntrust(entrust, err);

		entrust->release();
		err->release();
	}
}

bool TraderAresClt::isConnected()
{
	return (m_wrapperState == WS_ALLREADY);
}

int TraderAresClt::queryConfirm()
{
	if (m_pUserAPI == NULL || m_wrapperState != WS_LOGINED)
	{
		return -1;
	}

	return 0;
}

void TraderAresClt::FreeApi(ITDErebusApi* p)
{
	if (m_pfnDestroyObj)
		m_pfnDestroyObj(p);
}

void TraderAresClt::FreeApp(ITDErebusApp* p)
{
	if (m_pfnDestroyObj)
		m_pfnDestroyObj(p);
}
