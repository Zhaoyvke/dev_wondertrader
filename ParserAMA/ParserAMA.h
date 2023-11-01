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


			///AMA �ӿ�
		public:
			void OnEvent(uint32_t level, uint32_t code, const char* event_msg, uint32_t len);//�����¼�֪ͨ�ص�,ʹ���߿ɸ��ݸûص��¼�����Ӧ�Ĵ���

			void OnMDSnapshot(MDSnapshot* snapshot, uint32_t cnt);//�����ֻ��������ݻص�

			void OnMDTickOrder(MDTickOrder* ticks, uint32_t cnt);	//�������ί�����ݻص�

			void OnMDTickExecution(amd::ama::MDTickExecution* tick, uint32_t cnt);//������ʳɽ����ݻص�
			
			void OnMDOrderBook(std::vector<amd::ama::MDOrderBook>& order_book); //����ί�в����ݻص�
			//void OnLog(const int32_t& level, const char* log, uint32_t len);


			//void OnMDOptionSnapshot(MDOptionSnapshot* snapshot, uint32_t cnt);//������Ȩ�������ݻص�

			//void OnMDIndexSnapshot(MDIndexSnapshot* snapshot, uint32_t cnt);//����ָ���������ݻص�



			//void OnMDAfterHourFixedPriceSnapshot(MDAfterHourFixedPriceSnapshot* snapshots, uint32_t cnt);//�����̺󶨼۽��׿������ݻص�

			//void OnMDAfterHourFixedPriceTickExecution(MDAfterHourFixedPriceTickExecution* ticks, uint32_t cnt);//�����̺󶨼۽�����ʳɽ����ݻص�

			

			//void OnMDBondSnapshot(MDBondSnapshot* snapshots, uint32_t cnt);//����ծȯ�������ݻص�

			//void OnMDBondTickOrder(MDBondTickOrder * tick, uint32_t cnt);//����ծȯ���ί�����ݻص�

			//void OnMDBondTickExecution(MDBondTickExecution * tick, uint32_t cnt);//����ծȯ��ʳɽ����ݻص�

			//void OnMDOrderBookSnapshot(MDOrderBookSnapshot* order_book_snapshots, uint32_t cnt);//����ί�в��������ݻص�


		private:
			/*
			 *	���͵�¼����
			 */
			void DoLogin();
			/*
			 *	����Ʒ������
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

			std::string			m_strFlowDir;  // ����log
			std::string			m_strCertDir;  // �ܳ��ļ�

			// �����Ӷ����ص�ַ��Ϊ����
			std::string			m_strFontBackup;  // ��ip:port;ip:port������ʽ���ַ���

			uint32_t			m_iService;  // ����߳���
			WTSVariant*			m_Servers;
			int					m_iValue;

			bool				m_bLogin;		//��¼ 
			bool				m_bNoConns;		//��������
			bool				m_bReconnect;  // �Ƿ�����
			int					m_iReconnect;  // ������

			std::string			m_strCodes;  // ��������
			bool				m_has_transaction_order;//�Ƿ�����������

			bool				m_has_tick;		//�Ƿ����tick

			//CodeSet				m_fitSHSubs;
			//CodeSet				m_fitSZSubs;
			int					m_iRequestID;


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

			CodeSet				m_fitSHSubs;
			CodeSet				m_fitSZSubs;

			bool				m_bHasSSE;  // �Ƿ����Ͻ���
			bool				m_bHasSZSE;  // �Ƿ����������
			bool				m_bHasL1;	//L1����
			bool				m_bHasL2;	//L2����

			//���ĵĺ�Լ����
			bool				m_bHasOrder;
			bool				m_bHasOrdQue;
			bool				m_bHasTrans;
			bool				m_bHasSnap;
			bool				m_bHasFp;  // �̺�̶��۸�����
			bool				m_bHasArb;  // ͳ����������

			// ���ĵĺ�Լ����
			bool				m_bHasIndex;  // ָ��
			bool				m_bHasShare;  // ��Ʊ
			bool				m_bHasOption;  // ��Ȩ
			bool				m_bHasBond;  // ծȯ
			bool				m_bHasETF;
		private:
			IParserSpi*			m_parserSink;
			IBaseDataMgr*		m_pBaseDataMgr;
			DllHandle			m_hAmaInst;
		};


