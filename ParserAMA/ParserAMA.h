#pragma once
/*!
 * \file ParserOTS.h
 * \project	WonderTrader
 *
 * \author kanavi
 * \date 2023/09/18
 *
 * \brief
 */
#if _WIN32
#ifndef WINAPI
#define WINAPI __stdcall
#endif
#ifndef BOOL
typedef int BOOL;
#endif
#endif
#include <stdio.h>
#include <map>
#include <string>

#include "../Share/DLLHelper.hpp"
#include "../Includes/IParserApi.h"
#include "../API/AMD_ama_msvc-14.0_Windows_x64_V4.0.1.220812-rc4.0_20220812-100112/include/ama.h"
#include "../API/AMD_ama_msvc-14.0_Windows_x64_V4.0.1.220812-rc4.0_20220812-100112/include/ama_tools.h"
#include "../API/AMD_ama_msvc-14.0_Windows_x64_V4.0.1.220812-rc4.0_20220812-100112/include/ama_struct.h"
#include "../API/AMD_ama_msvc-14.0_Windows_x64_V4.0.1.220812-rc4.0_20220812-100112/include/hkex/ama_hkex_spi.h"

#pragma comment(lib, "../API/AMD_ama_msvc-14.0_Windows_x64_V4.0.1.220812-rc4.0_20220812-100112/lib/ama.lib")


NS_WTP_BEGIN
class WTSTickData;
NS_WTP_END
#define	USING_NS_NNN	using namespace amd::ama
USING_NS_NNN;
USING_NS_WTP;


class ParserAMA :public IParserApi, amd::ama::IAMDApi, amd::ama::IAMDSpi
		{
		public:
			ParserAMA();
			virtual ~ParserAMA();

		public:
			enum LoginStatus
			{
				LS_NOTLOGIN,
				LS_LOGINING,
				LS_LOGINED
			};

			//IQuoteParser 接口
		public:
			virtual bool init(WTSVariant* config) override;

			virtual void release() override;

			virtual bool connect() override;

			virtual bool disconnect() override;

			virtual bool isConnected() override;

			virtual void subscribe(const CodeSet &vecSymbols) override;
			virtual void unsubscribe(const CodeSet &vecSymbols) override;

			virtual void registerSpi(IParserSpi* listener) override;


			///AMA 接口
		public:
			void OnEvent(uint32_t level, uint32_t code, const char* event_msg, uint32_t len);//接收事件通知回调,使用者可根据该回调事件做相应的处理。

			void OnMDSnapshot(MDSnapshot* snapshot, uint32_t cnt);//接收现货快照数据回调

			void OnMDTickOrder(MDTickOrder* ticks, uint32_t cnt);	//接收逐笔委托数据回调

			void OnMDTickExecution(amd::ama::MDTickExecution* tick, uint32_t cnt);//接收逐笔成交数据回调
			
			void OnMDOrderBook(std::vector<amd::ama::MDOrderBook>& order_book); //接收委托簿数据回调
			//void OnLog(const int32_t& level, const char* log, uint32_t len);


			//void OnMDOptionSnapshot(MDOptionSnapshot* snapshot, uint32_t cnt);//接收期权快照数据回调

			//void OnMDIndexSnapshot(MDIndexSnapshot* snapshot, uint32_t cnt);//接收指数快照数据回调



			//void OnMDAfterHourFixedPriceSnapshot(MDAfterHourFixedPriceSnapshot* snapshots, uint32_t cnt);//接收盘后定价交易快照数据回调

			//void OnMDAfterHourFixedPriceTickExecution(MDAfterHourFixedPriceTickExecution* ticks, uint32_t cnt);//接收盘后定价交易逐笔成交数据回调

			

			//void OnMDBondSnapshot(MDBondSnapshot* snapshots, uint32_t cnt);//接收债券快照数据回调

			//void OnMDBondTickOrder(MDBondTickOrder * tick, uint32_t cnt);//接收债券逐笔委托数据回调

			//void OnMDBondTickExecution(MDBondTickExecution * tick, uint32_t cnt);//接收债券逐笔成交数据回调

			//void OnMDOrderBookSnapshot(MDOrderBookSnapshot* order_book_snapshots, uint32_t cnt);//接收委托簿快照数据回调


		private:
			/*
			 *	发送登录请求
			 */
			void DoLogin();
			/*
			 *	订阅品种行情
			 */
			void DoSubscribeMD(bool isAll = false);

		private:
			uint32_t			m_uTradingDate;
			LoginStatus			m_loginState;
			//Cfg*				m_pUserAPI;
			Cfg					cfg;
			//amd::ama::IAMDApi*	m_pUserAPI;
			std::string			m_strHost;
			int32_t				m_iPort;

			std::string			m_strUser;
			std::string			m_strPass;

			std::string			m_strFlowDir;  // 本地log
			std::string			m_strCertDir;  // 密匙文件

			// 务必添加多网关地址作为备用
			std::string			m_strFontBackup;  // 以ip:port;ip:port这种形式的字符串

			uint32_t			m_iService;  // 添加线程数
			WTSVariant*			m_Servers;
			int					m_iValue;

			bool				m_bLogin;		//登录 
			bool				m_bNoConns;		//不重连？
			bool				m_bReconnect;  // 是否重连
			int					m_iReconnect;  // 重连数

			std::string			m_strCodes;  // 订阅数量
			bool				m_has_transaction_order;//是否加入逐笔数据

			bool				m_has_tick;		//是否加入tick

			//CodeSet				m_fitSHSubs;
			//CodeSet				m_fitSZSubs;
			int					m_iRequestID;


	private:
			std::string			m_strSHSTKCodes;  // 上交所订阅股票合约
			std::string			m_strSZSTKCodes;  // 深交所订阅股票合约
			std::string			m_strSHOptCodes;  // 期权
			std::string			m_strSZOptCodes;
			std::string			m_strSHETFCodes;  // ETF
			std::string			m_strSZETFCodes;
			std::string			m_strSHBondCodes;  // 债券
			std::string			m_strSZBondCodes;
			std::string			m_strSHINDXCodes;  // 指数
			std::string			m_strSZINDXCodes;

			CodeSet				m_fitSHSubs;
			CodeSet				m_fitSZSubs;

			bool				m_bHasSSE;  // 是否订阅上交所
			bool				m_bHasSZSE;  // 是否订阅深交所数据
			bool				m_bHasL1;	//L1数据
			bool				m_bHasL2;	//L2数据

			//订阅的合约类型
			bool				m_bHasOrder;
			bool				m_bHasOrdQue;
			bool				m_bHasTrans;
			bool				m_bHasSnap;
			bool				m_bHasFp;  // 盘后固定价格行情
			bool				m_bHasArb;  // 统计套利行情

			// 订阅的合约类型
			bool				m_bHasIndex;  // 指数
			bool				m_bHasShare;  // 股票
			bool				m_bHasOption;  // 期权
			bool				m_bHasBond;  // 债券
			bool				m_bHasETF;
		private:
			IParserSpi*			m_parserSink;
			IBaseDataMgr*		m_pBaseDataMgr;
			DllHandle			m_hAmaInst;
		};


