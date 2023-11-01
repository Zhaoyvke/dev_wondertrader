/*!
 * \file TraderATP.h
 * \project	WonderTrader
 *
 * \author Wesley
 * \date 2020/03/30
 *
 * \brief
 */
#pragma once

#include <stdint.h>
#include <boost/asio/io_service.hpp>
#include <atomic>

#define TRADE_API_USE_STATIC
#include "../API/AtpQuantV1.3.6//include/atp_quant_api.h"

#include "../Includes/ITraderApi.h"
#include "../Includes/WTSCollection.hpp"

#include "../Share/StdUtils.hpp"
#include "../Share/DLLHelper.hpp"
#include "../Share/WtKVCache.hpp"

USING_NS_WTP;
using namespace atp::quant_api;

class TraderATPQuant : public ATPQuantHandler, public ITraderApi
{
public:
	TraderATPQuant();
	virtual ~TraderATPQuant();

	typedef enum
	{
		TS_NOTLOGIN,		//δ��¼
		TS_LOGINING,		//���ڵ�¼
		TS_LOGINED,			//�ѵ�¼
		TS_LOGINFAILED,		//��¼ʧ��
		TS_ALLREADY			//ȫ������
	} TraderState;

public:
	//////////////////////////////////////////////////////////////////////////
	// ATPQuantApi����ص�

	/**
	* @brief �ֻ�ί����Ӧ
	*/
	virtual void OnRspCashAuctionOrder(const ATPRspCashAuctionOrderMsg& msg, const ATPRspErrorInfo& error_info, const int64_t request_id) override;

	/**
	 * @brief �ֻ�ί�лر�
	 */
	virtual void OnRtnCashAuctionOrder(const ATPRtnCashAuctionOrderMsg& msg) override;

	/**
	 * @brief �ֻ�������Ӧ
	 */
	virtual void OnRspCashCancelOrder(const ATPRspCashCancelOrderMsg& msg, const ATPRspErrorInfo& error_info, const int64_t request_id) override;

	/**
	 * @brief �ֻ�������ѯ���
	 */
	virtual void OnRspCashOrderQueryResult(const ATPRspCashOrderQueryResultMsg& msg, const int64_t request_id, const ATPRspErrorInfo& error_info, const bool isLast) override;

	/**
	 * @brief �ֻ��ɷݲ�ѯ���
	 */
	virtual void OnRspCashShareQueryResult(const ATPRspCashShareQueryResultMsg& msg, const int64_t request_id, const ATPRspErrorInfo& error_info, const bool isLast) override;

	/**
	 * @brief �ֻ��ɽ���ѯ���
	 */
	virtual void OnRspCashTradeOrderQueryResult(const ATPRspCashTradeOrderQueryResultMsg& msg, const int64_t request_id, const ATPRspErrorInfo& error_info, const bool isLast) override;

	/**
	 * @brief �ֻ��ɽ���ѯ���
	 */
	//virtual void OnRspCashTradeCollectQueryResult(const ATPRspCashCollectQueryResultMsg& msg, const int64_t request_id, const ATPRspErrorInfo& error_info, const bool isLast) override;

	/**
	 * @brief �ֻ��ʽ��ѯ���
	 */
	virtual void OnRspCashFundQueryResult(const ATPRspCashFundQueryResultMsg& msg, const int64_t request_id, const ATPRspErrorInfo& error_info, const bool isLast) override;

	/**
	 * @brief �ֻ��ʲ���ѯ���
	 */
	virtual void OnRspCashAssetQueryResult(const ATPRspCashAssetQueryResultMsg& msg, const int64_t request_id, const ATPRspErrorInfo& error_info, const bool isLast) override;

	/**
	 * @brief ��¼�ɹ�
	 */
	//virtual void OnLogin(const ATPCustomerInfo& msg) override;  // ����ֱ��ͨ������api��ȡ���

	/**
	 * @brief �ǳ��ɹ�
	 */
	virtual void OnLogout(const char* desc) override;

	/**
	 * @brief �Զ�����
	 */
	virtual void OnRecovering(const char* desc) override;

public:
	//////////////////////////////////////////////////////////////////////////
	//ITraderApi �ӿ�
	virtual bool init(WTSVariant *params) override;

	virtual void release() override;

	virtual void registerSpi(ITraderSpi *listener) override;

	virtual void connect() override;

	virtual void disconnect() override;

	virtual bool isConnected() override;

	virtual bool makeEntrustID(char* buffer, int length) override;

	virtual int login(const char* user, const char* pass, const char* productInfo) override;

	virtual int logout() override;

	virtual int orderInsert(WTSEntrust* eutrust) override;

	virtual int orderAction(WTSEntrustAction* action) override;

	virtual int queryAccount() override;

	virtual int queryPositions() override;

	virtual int queryOrders() override;

	virtual int queryTrades() override;

private:
	void		reconnect();
	inline uint32_t	genRequestID();
	void		doLogin();

	inline WTSOrderInfo*	makeOrderInfo(const ATPRspCashAuctionOrderMsg& orderField);
	//inline WTSOrderInfo*	makeOrderInfo(const ATPRspCashCancelOrderMsg& orderField);
	inline WTSEntrust*		makeEntrust(const ATPRspCashAuctionOrderMsg& entrustField);
	//inline WTSEntrust*		makeEntrust(const ATPRspCashCancelOrderMsg& entrustField);
	inline WTSTradeInfo*	makeTradeInfo(const ATPRspCashTradeOrderQueryResultMsg& tradeField);
	inline WTSOrderInfo*	makeOrderInfo(const ATPRspCashOrderQueryResultMsg& orderField);
	inline WTSTradeInfo*	makeTradeRecord(const ATPRtnCashAuctionOrderMsg& orderField);
	inline WTSEntrust*		makeEntrust(const ATPRtnCashAuctionOrderMsg& msg);
	inline WTSOrderInfo*	makeOrderInfo(const ATPRtnCashAuctionOrderMsg& msg);

	inline bool	extractEntrustID(const char* entrustid, uint32_t &orderRef);
	inline void	genEntrustID(char* buffer, uint32_t orderRef);

private:
	ATPQuantAPI*	_api;
	ITraderSpi*		_sink;
	bool			_close_flag;

	typedef WTSHashMap<std::string> PositionMap;
	PositionMap*			_positions;
	WTSArray*				_trades;
	WTSArray*				_orders;

	IBaseDataMgr*			_bd_mgr;

	std::string 		_agw_user;
	std::string 		_agw_pass;
	std::string		_user;  // �ͻ���
	std::string		_pass;  // �ͻ�����
	int			_loginmode;  // �˻��ĵ�¼ģʽ
	int 			_encryptmode;  // 0=touchuanmoshi 1=jiamimoshi
	int			_order_way;  // ί�з�ʽ
	std::string		_client_code;  // �ն�ʶ����
	std::string		_bind_ip;  // ����������ַ
	
	std::string 	_product;

	//std::string		_sh_acctid;  // �Ϻ�֤ȯ�˻�
	//std::string		_sz_acctid;  // ����֤ȯ�˻�
	std::string		_acctid;  // ֤ȯ�˻�
	bool			_is_acct_sh;  // �˻����Ϻ����������ڻ�
	std::string		_fund_accountid;  // �ʽ��˻�
	std::string		_fund_pass;  // ��������
	std::string		_branchid;   //Ӫҵ��ID ������¼ģʽΪ2-�ʽ��˻���¼ģʽʱ�����ֶ��Ƿ��������ѯȯ�̣�  

	std::string		_front;  // ��������  AGW��ַ ��ʽΪ"ip:port;ip:port"
	
	std::string		_cur_user;

	TraderState		_state;

	uint64_t		_sessionid;
	
	uint32_t		_tradingday;
	std::atomic<uint32_t>		_reqid;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    
	std::atomic<uint32_t>		_ordref;		//��������

	std::string		_cust_id;  // �ͻ���

	boost::asio::io_service		_asyncio;
	StdThreadPtr				_thrd_worker;
	//boost::asio::io_service::work work;

	DllHandle		m_hInstATP;

	//ί�е���ǻ�����
	WtKVCache		m_eidCache;
	//������ǻ�����
	WtKVCache		m_oidCache;

	std::map<int32_t, int32_t> report_sync;

	WTSArray* ayOrders;  // ������ѯ���
	WTSArray* ayTrades;  // �ɽ���ѯ���
	WTSArray* ayFunds;  // �û��ʽ��ѯ���

	std::atomic<int> _id_counts;
};

