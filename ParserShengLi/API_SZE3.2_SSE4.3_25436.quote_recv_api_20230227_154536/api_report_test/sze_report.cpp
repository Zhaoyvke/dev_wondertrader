#include "sze_report.h"
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <cstdarg>

#ifndef WINDOWS
#include <dlfcn.h>
#include <arpa/inet.h>
#endif

sze_report::sze_report()
{
	m_fp_lev2			= NULL;
	m_fp_idx			= NULL;
	m_fp_ord			= NULL;
	m_fp_exe			= NULL;
	m_fp_close_px		= NULL;
	m_fp_tree			= NULL;
	m_fp_ibr_tree		= NULL;
	m_fp_turnover		= NULL;
	m_fp_bond_snap		= NULL;
	m_fp_bond_ord		= NULL;
	m_fp_bond_exe		= NULL;
	m_fp_tick			= NULL;
	m_fp_bond_tick		= NULL;
	m_p_quote			= NULL;

	m_snap 				= NULL;
	m_idx 				= NULL;
	m_order 			= NULL;
	m_exe 				= NULL;
	m_after_close		= NULL;
	m_tree 				= NULL;
	m_ibr_tree 			= NULL;
	m_turnover 			= NULL;
	m_bond_snap			= NULL;
	m_bond_order		= NULL;
	m_bond_exe			= NULL;

	m_b_snap_rollback_flag = false;
	m_b_idx_rollback_flag = false;
	m_b_tick_rollback_flag = false;
	m_b_after_rollback_flag = false;
	m_b_tree_rollback_flag = false;
	m_b_ibr_tree_rollback_flag = false;
	m_b_turnover_rollback_flag = false;
	m_b_bond_snap_rollback_flag = false;
	m_b_bond_tick_rollback_flag = false;

	m_ll_snap_count_lost = 0;
	m_ll_idx_count_lost = 0;
	m_ll_tick_count_lost = 0;
	m_ll_after_count_lost = 0;
	m_ll_tree_count_lost = 0;
	m_ll_ibr_tree_count_lost = 0;
	m_ll_turnover_count_lost = 0;
	m_ll_bond_snap_count_lost = 0;
	m_ll_bond_tick_count_lost = 0;
	m_i_snap_seq = 0;
	m_i_idx_seq = 0;
	m_i_tick_seq = 0;
	m_i_after_seq = 0;
	m_i_tree_seq = 0;
	m_i_ibr_tree_seq = 0;
	m_i_turnover_seq = 0;
	m_i_bond_snap_seq = 0;
	m_i_bond_tick_seq = 0;

	m_ll_snap_count		= 0;
	m_ll_idx_count		= 0;
	m_ll_order_count	= 0;
	m_ll_exe_count		= 0;
	m_ll_after_count	= 0;
	m_ll_tree_count		= 0;
	m_ll_ibr_tree_count	= 0;
	m_ll_turnover_count = 0;
	m_ll_bond_snap_count = 0;
	m_ll_bond_order_count= 0;
	m_ll_bond_exe_count =0;
	m_b_report_quit		= false;
	m_b_tick_detach		= false;

#ifdef WINDOWS
	m_h_core = NULL;
#else
	m_h_core = NULL;
#endif
}

sze_report::~sze_report()
{
}

bool sze_report::init(socket_udp_param* param, int num)
{
	const char* err_address = "operation::init：";
#ifdef WINDOWS
	m_h_core = LoadLibraryA( DLL_EFH_LEV2_DLL_NAME );
	if (m_h_core == NULL)
	{
		string msg = format_str("%s init：load dll:%s!\n", err_address, DLL_EFH_LEV2_DLL_NAME);
		efh_sze_lev2_error(msg.c_str(), msg.length());
		return false;
	}

	func_create_efh_sze_lev2_api func_create = (func_create_efh_sze_lev2_api)GetProcAddress(m_h_core, CREATE_EFH_SZE_LEV2_API_FUNCTION);
	if (func_create == NULL)
	{
		string msg = format_str("%s get create sqs function ptr failed.\n", err_address);
		efh_sze_lev2_error(msg.c_str(), msg.length());
		return false;
	}
#else
	m_h_core = dlopen(DLL_EFH_LEV2_DLL_NAME, RTLD_LAZY);
	if (m_h_core == NULL)
	{
		string msg = format_str("%s init：load dll:%s error:%s!\n", err_address, DLL_EFH_LEV2_DLL_NAME,dlerror());
		efh_sze_lev2_error(msg.c_str(), msg.length());
		return false;
	}

	func_create_efh_sze_lev2_api func_create = (func_create_efh_sze_lev2_api)dlsym(m_h_core, CREATE_EFH_SZE_LEV2_API_FUNCTION);
	if (func_create == NULL)
	{
		string msg = format_str("%s get create sqs function ptr failed.\n", err_address);
		efh_sze_lev2_error(msg.c_str(), msg.length());
		return false;
	}
#endif

	m_p_quote = func_create();
	if (m_p_quote == NULL)
	{
		string msg = format_str("%s create sqs function ptr null.\n", err_address);
		efh_sze_lev2_error(msg.c_str(), msg.length());
		return false;
	}

	if (!m_p_quote->init_sze(param, num, static_cast<efh_sze_lev2_api_event*>(this), static_cast<efh_sze_lev2_api_depend*>(this)))
	{
		string msg = format_str("%s init parse! error\n", err_address);
		efh_sze_lev2_error(msg.c_str(), msg.length());
		return false;
	}

	return true;
}

void sze_report::set_tick_detach( bool enable )
{
	m_b_tick_detach = enable;
}

void sze_report::run(int nic_type, bool b_report_quit, const char* symbol)
{
	m_b_report_quit = b_report_quit;

	if (m_b_report_quit)
	{
		m_snap        = new sze_qt_node_snap[QT_SZE_QUOTE_COUNT];
		m_idx         = new sze_qt_node_index[QT_SZE_QUOTE_COUNT];
		m_order       = new sze_qt_node_order[QT_SZE_QUOTE_COUNT];
		m_exe         = new sze_qt_node_exe[QT_SZE_QUOTE_COUNT];
		m_after_close = new sze_qt_node_after_close[QT_SZE_QUOTE_COUNT];
		m_tree        = new sze_qt_node_tree[QT_SZE_QUOTE_COUNT];
		m_ibr_tree    = new sze_qt_node_ibr_tree[QT_SZE_QUOTE_COUNT];
		m_turnover    = new sze_qt_node_turnover[QT_SZE_QUOTE_COUNT];
		m_bond_snap   = new sze_qt_node_snap[QT_SZE_QUOTE_COUNT];
		m_bond_order  = new sze_qt_node_order[QT_SZE_QUOTE_COUNT];
		m_bond_exe    = new sze_qt_node_exe[QT_SZE_QUOTE_COUNT];

		memset( m_snap , 0 , sizeof( sze_qt_node_snap ) * QT_SZE_QUOTE_COUNT );
		memset( m_idx , 0 , sizeof( sze_qt_node_index ) * QT_SZE_QUOTE_COUNT );
		memset( m_order , 0 , sizeof( sze_qt_node_order ) * QT_SZE_QUOTE_COUNT );
		memset( m_exe , 0 , sizeof( sze_qt_node_exe ) * QT_SZE_QUOTE_COUNT );
		memset( m_after_close , 0 , sizeof( sze_qt_node_after_close ) * QT_SZE_QUOTE_COUNT );
		memset( m_tree , 0 , sizeof( sze_qt_node_tree ) * QT_SZE_QUOTE_COUNT );
		memset( m_ibr_tree , 0 , sizeof( sze_qt_node_ibr_tree ) * QT_SZE_QUOTE_COUNT );
		memset( m_turnover , 0 , sizeof( sze_qt_node_turnover ) * QT_SZE_QUOTE_COUNT );
		memset( m_bond_snap , 0 , sizeof( sze_qt_node_snap ) * QT_SZE_QUOTE_COUNT );
		memset( m_bond_order , 0 , sizeof( sze_qt_node_order ) * QT_SZE_QUOTE_COUNT );
		memset( m_bond_exe , 0 , sizeof( sze_qt_node_exe ) * QT_SZE_QUOTE_COUNT );
		string_split(symbol, m_vec_symbol, ",");

		for (int i = 0; i < (int)m_vec_symbol.size(); i++)
		{
			push_filter_symbol(m_vec_symbol[i].c_str());
		}
	}

	switch (nic_type)
	{
	case enum_nic_normal:
	case enum_nic_solarflare_efvi:
	case enum_nic_exablaze_exanic:
	case enum_nic_x710_win_speed:
	case enum_nic_solarflare_win_speed:
	{
		if (!m_p_quote->start_sze( enum_efh_nic_type(nic_type) ))
		{
			string msg = format_str( "start parse error\n" );
			efh_sze_lev2_error( msg.c_str( ) , msg.length( ) );
		}
	}
	break;

	default:
	{
		string msg = format_str( "nic type:%d error\n" , nic_type );
		efh_sze_lev2_error( msg.c_str( ) , msg.length( ) );
	}
	break;
	}


}

void sze_report::show()
{
	printf( "--------------------< SZE Info >--------------------\n" );
	printf("SZE_snap count: %lld, lost_count:%lld, rollback_flag:%d.\n",
		m_ll_snap_count, m_ll_snap_count_lost, (int)m_b_snap_rollback_flag);
	printf("SZE_idx count: %lld, lost_count:%lld, rollback_flag:%d.\n",
		m_ll_idx_count, m_ll_idx_count_lost, (int)m_b_idx_rollback_flag);
	printf("SZE_after count: %lld, lost_count:%lld, rollback_flag:%d.\n",
		m_ll_after_count, m_ll_after_count_lost, (int)m_b_after_rollback_flag);
	printf("SZE_tree count: %lld, lost_count:%lld, rollback_flag:%d.\n",
		m_ll_tree_count, m_ll_tree_count_lost, (int)m_b_tree_rollback_flag);
	printf("SZE_order count: %lld\n", m_ll_order_count);
	printf("SZE_exe count: %lld\n", m_ll_exe_count);
	printf("SZE_tick count: %lld, lost_count:%lld, rollback_flag:%d.\n",
		m_ll_order_count + m_ll_exe_count, m_ll_tick_count_lost, (int)m_b_tick_rollback_flag);
	printf("SZE_ibr_tree count: %lld, lost_count:%lld, rollback_flag:%d.\n",
		m_ll_ibr_tree_count, m_ll_ibr_tree_count_lost, (int)m_b_ibr_tree_rollback_flag);
	printf("SZE_turnover count: %lld, lost_count:%lld, rollback_flag:%d.\n",
		m_ll_turnover_count, m_ll_turnover_count_lost, (int)m_b_turnover_rollback_flag);
	printf("SZE_bond_snap count: %lld, lost_count:%lld, rollback_flag:%d.\n",
		m_ll_bond_snap_count, m_ll_bond_snap_count_lost, (int)m_b_bond_snap_rollback_flag);
	printf("SZE_bond_order count: %lld\n",m_ll_bond_order_count);
	printf("SZE_bond_exe count: %lld\n",m_ll_bond_exe_count);
	printf("SZE_bond_tick count: %lld, lost_count:%lld, rollback_flag:%d.\n",
		m_ll_bond_order_count + m_ll_bond_exe_count, m_ll_bond_tick_count_lost, (int)m_b_bond_tick_rollback_flag);
	fflush(stdout);
}

void sze_report::close()
{
	if (m_p_quote == NULL)
	{
		return;
	}

	m_p_quote->stop_sze();
	m_p_quote->close_sze();
#ifdef WINDOWS
	func_destroy_efh_sze_lev2_api func_destroy = (func_destroy_efh_sze_lev2_api)GetProcAddress(m_h_core, DESTROY_EFH_SZE_LEV2_API_FUNCTION);
#else
	func_destroy_efh_sze_lev2_api func_destroy = (func_destroy_efh_sze_lev2_api)dlsym(m_h_core, DESTROY_EFH_SZE_LEV2_API_FUNCTION);
#endif
	if (func_destroy == NULL)
	{
		return;
	}
	if (m_b_report_quit)
	{
		report_efh_sze_lev2_snap();
		report_efh_sze_lev2_order();
		report_efh_sze_lev2_exe();
		report_efh_sze_lev2_after_close();
		report_efh_sze_lev2_idx();
		report_efh_sze_lev2_tree();
		report_efh_sze_lev2_ibr_tree();
		report_efh_sze_lev2_turnover();
		report_efh_sze_lev2_bond_snap();
		report_efh_sze_lev2_bond_order();
		report_efh_sze_lev2_bond_exe();
	}
	if (m_fp_lev2)
	{
		fclose(m_fp_lev2);
		m_fp_lev2 = NULL;
	}
	if (m_fp_idx)
	{
		fclose(m_fp_idx);
		m_fp_idx = NULL;
	}
	if (m_fp_ord)
	{
		fclose(m_fp_ord);
		m_fp_ord = NULL;
	}
	if (m_fp_exe)
	{
		fclose(m_fp_exe);
		m_fp_exe = NULL;
	}
	if (m_fp_close_px)
	{
		fclose(m_fp_close_px);
		m_fp_close_px = NULL;
	}
	if (m_fp_tree)
	{
		fclose(m_fp_tree);
		m_fp_tree = NULL;
	}
	if (m_fp_ibr_tree)
	{
		fclose(m_fp_ibr_tree);
		m_fp_ibr_tree = NULL;
	}
	if (m_fp_turnover)
	{
		fclose(m_fp_turnover);
		m_fp_turnover = NULL;
	}
	if (m_fp_bond_snap)
	{
		fclose(m_fp_bond_snap);
		m_fp_bond_snap = NULL;
	}
	if (m_fp_bond_ord)
	{
		fclose(m_fp_bond_ord);
		m_fp_bond_ord = NULL;
	}
	if (m_fp_bond_exe)
	{
		fclose(m_fp_bond_exe);
		m_fp_bond_exe = NULL;
	}
	if (m_fp_tick)
	{
		fclose(m_fp_tick);
		m_fp_tick = NULL;
	}
	if (m_fp_bond_tick)
	{
		fclose(m_fp_bond_tick);
		m_fp_bond_tick = NULL;
	}
	if ( m_snap )
	{
		delete[]m_snap;
		m_snap = NULL;
	}
	if ( m_idx )
	{
		delete[]m_idx;
		m_idx = NULL;
	}
	if ( m_order )
	{
		delete[]m_order;
		m_order = NULL;
	}
	if ( m_exe )
	{
		delete[]m_exe;
		m_exe = NULL;
	}
	if ( m_after_close )
	{
		delete[]m_after_close;
		m_after_close = NULL;
	}
	if ( m_tree )
	{
		delete[]m_tree;
		m_tree = NULL;
	}
	if ( m_ibr_tree )
	{
		delete[]m_ibr_tree;
		m_ibr_tree = NULL;
	}
	if ( m_turnover )
	{
		delete[]m_turnover;
		m_turnover = NULL;
	}
	if ( m_bond_snap )
	{
		delete[]m_bond_snap;
		m_bond_snap = NULL;
	}
	if ( m_bond_order )
	{
		delete[]m_bond_order;
		m_bond_order = NULL;
	}
	if ( m_bond_exe )
	{
		delete[]m_bond_exe;
		m_bond_exe = NULL;
	}

	func_destroy(m_p_quote);
#ifdef WINDOWS
	FreeLibrary(m_h_core);
#else
	dlclose(m_h_core);
#endif
}

void sze_report::on_report_efh_sze_lev2_after_close(sze_hpf_after_close* p_close_px)
{
	if(m_i_after_seq!=0)
	{
		if (p_close_px->m_head.m_sequence <= m_i_after_seq)
		{
			m_b_after_rollback_flag = true;
			printf("SZE after out of seq, last seq[%u], cur seq[%u].\n", m_i_after_seq, p_close_px->m_head.m_sequence);
		}
		else
		{
			m_ll_after_count_lost += p_close_px->m_head.m_sequence - m_i_after_seq - 1;
		}
	}
	m_i_after_seq = p_close_px->m_head.m_sequence;
	if (!m_b_report_quit)
	{
		if (m_fp_close_px == NULL)
		{
			time_t now = time(NULL);
			tm* ltm = localtime(&now);

			char	str_full_name[1024];
			memset(str_full_name, 0, sizeof(str_full_name));
			sprintf(str_full_name, "%04d%02d%02d_sze_after_close.dat", ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday);

			m_fp_close_px = fopen(str_full_name, "wb+");
			if (m_fp_close_px == NULL)
			{
				return;
			}
		}

		int ret = fwrite((char*)p_close_px, 1, sizeof(*p_close_px), m_fp_close_px);
		fflush(m_fp_close_px);
		if (ret <= 0)
		{
			printf("write sze_after_close.dat error!\n");
		}
		++m_ll_after_count;

		return;
	}

	if (!is_vaild_symbol((char*)(p_close_px->m_head.m_symbol)))
	{
		return;
	}

	timespec				ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	int node = m_ll_after_count % QT_SZE_QUOTE_COUNT;

	memcpy(m_after_close[node].m_symbol, p_close_px->m_head.m_symbol, 9);
	m_after_close[node].m_channel_num			= p_close_px->m_head.m_channel_num;
	m_after_close[node].m_sequence_num			= p_close_px->m_head.m_sequence_num;
	m_after_close[node].m_quote_update_time		= p_close_px->m_head.m_quote_update_time;
	m_after_close[node].m_local_time			= ((long long)(ts.tv_sec)) * 1000000000 + ts.tv_nsec;
	m_after_close[node].m_trading_status	= p_close_px->m_trading_status;
	m_after_close[node].m_total_trade_num		= p_close_px->m_total_trade_num;
	m_after_close[node].m_total_quantity		= p_close_px->m_total_quantity;
	m_after_close[node].m_total_value			= p_close_px->m_total_value;
	m_after_close[node].m_pre_close_price		= p_close_px->m_pre_close_price;
	m_after_close[node].m_exe_price				= p_close_px->m_trade_price;
	m_after_close[node].m_bid_quantity			= p_close_px->m_bid_quantity;
	m_after_close[node].m_ask_quantity			= p_close_px->m_ask_quantity;

	++m_ll_after_count;
}

void sze_report::on_report_efh_sze_lev2_snap(sze_hpf_lev2* p_snap)
{
	if(m_i_snap_seq!=0)
	{
		if (p_snap->m_head.m_sequence <= m_i_snap_seq)
		{
			m_b_snap_rollback_flag = true;
			printf("SZE snap out of seq, last seq[%u], cur seq[%u].\n", m_i_snap_seq, p_snap->m_head.m_sequence);
		}
		else
		{
			m_ll_snap_count_lost += p_snap->m_head.m_sequence - m_i_snap_seq - 1;
		}
	}
	m_i_snap_seq = p_snap->m_head.m_sequence;

	if (!m_b_report_quit)
	{
		if (m_fp_lev2 == NULL)
		{
			time_t now = time(NULL);
			tm* ltm = localtime(&now);

			char	str_full_name[1024];
			memset(str_full_name, 0, sizeof(str_full_name));
			sprintf(str_full_name, "%04d%02d%02d_sze_snap.dat", ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday);

			m_fp_lev2 = fopen(str_full_name, "wb+");
			if (m_fp_lev2 == NULL)
			{
				return;
			}
		}

		int ret = fwrite((char*)p_snap, 1, sizeof(*p_snap), m_fp_lev2);
		fflush(m_fp_lev2);
		if (ret <= 0)
		{
			printf("write sze_snap.csv error!\t msg\n");
		}
		++m_ll_snap_count;

		return;
	}

	if (!is_vaild_symbol((char*)(p_snap->m_head.m_symbol)))
	{
		return;
	}

	timespec			ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	int node = m_ll_snap_count % QT_SZE_QUOTE_COUNT;

	memcpy(m_snap[node].m_symbol, p_snap->m_head.m_symbol, 9);
	m_snap[node].m_quote_update_time	= p_snap->m_head.m_quote_update_time;
	m_snap[node].m_local_time			= ((long long)(ts.tv_sec)) * 1000000000 + ts.tv_nsec;

	m_snap[node].m_bid_lev_1.m_price		= p_snap->m_bid_unit[0].m_price;
	m_snap[node].m_bid_lev_1.m_quantity		= p_snap->m_bid_unit[0].m_quantity;
	m_snap[node].m_bid_lev_10.m_price	= p_snap->m_bid_unit[9].m_price;
	m_snap[node].m_bid_lev_10.m_quantity		= p_snap->m_bid_unit[9].m_quantity;
	m_snap[node].m_ask_lev_1.m_price		= p_snap->m_ask_unit[0].m_price;
	m_snap[node].m_ask_lev_1.m_quantity		= p_snap->m_ask_unit[0].m_quantity;
	m_snap[node].m_ask_lev_10.m_price	= p_snap->m_ask_unit[9].m_price;
	m_snap[node].m_ask_lev_10.m_quantity		= p_snap->m_ask_unit[9].m_quantity;


	m_snap[node].m_bid_lev_5.m_price = p_snap->m_bid_unit[4].m_price;
	m_snap[node].m_bid_lev_5.m_quantity = p_snap->m_bid_unit[4].m_quantity;
	m_snap[node].m_ask_lev_5.m_price = p_snap->m_ask_unit[4].m_price;
	m_snap[node].m_ask_lev_5.m_quantity = p_snap->m_ask_unit[4].m_quantity;

	m_snap[node].m_last_price			= p_snap->m_last_price;
	m_snap[node].m_total_quantity			= p_snap->m_total_quantity;
	m_snap[node].m_total_value			= p_snap->m_total_value;

	++m_ll_snap_count;
}

void sze_report::on_report_efh_sze_lev2_tick(int msg_type, sze_hpf_order* p_order, sze_hpf_exe* p_exe)
{
	if (msg_type == SZE_LEV2_ORDER_MSG_TYPE)
	{
		if (!p_order)
		{
			string msg = format_str("write lev2 tick error: p_order == NULL\n");
			efh_sze_lev2_error(msg.c_str(), msg.length());
			return;
		}
		if(m_i_tick_seq!=0)
		{
			if (p_order->m_head.m_sequence <= m_i_tick_seq)
			{
				m_b_tick_rollback_flag = true;
				printf("SZE tick out of seq, last seq[%u], cur seq[%u].\n", m_i_tick_seq, p_order->m_head.m_sequence);
			}
			else
			{
				m_ll_tick_count_lost += p_order->m_head.m_sequence - m_i_tick_seq - 1;
			}
		}
		m_i_tick_seq = p_order->m_head.m_sequence;

		if (!m_b_report_quit)
		{
			if(m_b_tick_detach)
			{
				if (m_fp_ord == NULL)
				{
					time_t now = time(NULL);
					tm* ltm = localtime(&now);

					char	str_full_name[1024];
					memset(str_full_name, 0, sizeof(str_full_name));
					sprintf(str_full_name, "%04d%02d%02d_sze_order.dat", ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday);

					m_fp_ord = fopen(str_full_name, "wb+");
					if (m_fp_ord == NULL)
					{
						return;
					}
				}

				int ret = fwrite((char*)p_order, 1, sizeof(*p_order), m_fp_ord);
				fflush(m_fp_ord);
				if (ret <= 0)
				{
					printf("write sze_order.dat error!\t");
				}
			}
			else
			{
				if (m_fp_tick == NULL)
				{
					time_t now = time(NULL);
					tm* ltm = localtime(&now);

					char	str_full_name[1024];
					memset(str_full_name, 0, sizeof(str_full_name));
					sprintf(str_full_name, "%04d%02d%02d_sze_tick.dat", ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday);

					m_fp_tick = fopen(str_full_name, "wb+");
					if (m_fp_tick == NULL)
					{
						return;
					}
				}

				int ret = fwrite((char*)p_order, 1, sizeof(*p_order), m_fp_tick);
				fflush(m_fp_tick);
				if (ret <= 0)
				{
					printf("write sze_tick.dat error!\t");
				}
			}

			++m_ll_order_count;
			return;
		}

		if (!is_vaild_symbol((char*)(p_order->m_head.m_symbol)))
		{
			return;
		}

		timespec				ts;
		clock_gettime(CLOCK_REALTIME, &ts);
		int node = m_ll_order_count % QT_SZE_QUOTE_COUNT;

		memcpy(m_order[node].m_symbol, p_order->m_head.m_symbol, 9);
		m_order[node].m_local_time        = ((long long)(ts.tv_sec)) * 1000000000 + ts.tv_nsec;
		m_order[node].m_channel_num       = p_order->m_head.m_channel_num;
		m_order[node].m_sequence_num      = p_order->m_head.m_sequence_num;
		m_order[node].m_quote_update_time = p_order->m_head.m_quote_update_time;
		m_order[node].m_order_quantity    = p_order->m_order_quantity;
		m_order[node].m_order_price       = p_order->m_order_price;
		m_order[node].m_order_type        = p_order->m_order_type;

		++m_ll_order_count;
	}
	else if (msg_type == SZE_LEV2_EXE_MSG_TYPE)
	{
		if (!p_exe)
		{
			string msg = format_str("write lev2 tick error: p_exe == NULL\n");
			efh_sze_lev2_error(msg.c_str(), msg.length());
			return;
		}
		if(m_i_tick_seq!=0)
		{
			if(p_exe->m_head.m_sequence <= m_i_tick_seq)
			{
				m_b_tick_rollback_flag = true;
				printf("SZE tick out of seq, last seq[%u], cur seq[%u].\n", m_i_tick_seq, p_exe->m_head.m_sequence);
			}
			else
			{
				m_ll_tick_count_lost += p_exe->m_head.m_sequence - m_i_tick_seq - 1;
			}
		}
		m_i_tick_seq = p_exe->m_head.m_sequence;
		if (!m_b_report_quit)
		{
			if(m_b_tick_detach)
			{
				if (m_fp_exe == NULL)
				{
					time_t now = time(NULL);
					tm* ltm = localtime(&now);

					char	str_full_name[1024];
					memset(str_full_name, 0, sizeof(str_full_name));
					sprintf(str_full_name, "%04d%02d%02d_sze_exe.dat", ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday);

					m_fp_exe = fopen(str_full_name, "wb+");
					if (m_fp_exe == NULL)
					{
						return;
					}
				}

				int ret = fwrite((char*)p_exe, 1, sizeof(*p_exe), m_fp_exe);
				fflush(m_fp_exe);
				if (ret <= 0)
				{
					printf("write sze_exe.dat error!\n");
				}
			}
			else
			{
				if (m_fp_tick == NULL)
				{
					time_t now = time(NULL);
					tm* ltm = localtime(&now);

					char	str_full_name[1024];
					memset(str_full_name, 0, sizeof(str_full_name));
					sprintf(str_full_name, "%04d%02d%02d_sze_tick.dat", ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday);

					m_fp_tick = fopen(str_full_name, "wb+");
					if (m_fp_tick == NULL)
					{
						return;
					}
				}

				int ret = fwrite((char*)p_exe, 1, sizeof(*p_exe), m_fp_tick);
				fflush(m_fp_tick);
				if (ret <= 0)
				{
					printf("write sze_tick.dat error!\t");
				}
			}
			++m_ll_exe_count;

			return;
		}

		if (!is_vaild_symbol((char*)(p_exe->m_head.m_symbol)))
		{
			return;
		}

		timespec			ts;
		clock_gettime(CLOCK_REALTIME, &ts);
		int node = m_ll_exe_count % QT_SZE_QUOTE_COUNT;

		memcpy(m_exe[node].m_symbol, p_exe->m_head.m_symbol, 9);
		m_exe[node].m_local_time		= ((long long)(ts.tv_sec)) * 1000000000 + ts.tv_nsec;
		m_exe[node].m_channel_num		= p_exe->m_head.m_channel_num;
		m_exe[node].m_sequence_num		= p_exe->m_head.m_sequence_num;
		m_exe[node].m_quote_update_time	= p_exe->m_head.m_quote_update_time;
		m_exe[node].m_trade_price		= p_exe->m_trade_price;
		m_exe[node].m_trade_quantity	= p_exe->m_trade_quantity;
		m_exe[node].m_trade_type		= p_exe->m_trade_type;

		++m_ll_exe_count;
	}
}

void sze_report::on_report_efh_sze_lev2_idx(sze_hpf_idx* p_idx)
{
	if(m_i_idx_seq!=0)
	{
		if(p_idx->m_head.m_sequence <= m_i_idx_seq)
		{
			m_b_idx_rollback_flag = true;
			printf("SZE idx out of seq, last seq[%u], cur seq[%u].\n", m_i_idx_seq, p_idx->m_head.m_sequence);
		}
		else
		{
			m_ll_idx_count_lost += p_idx->m_head.m_sequence - m_i_idx_seq - 1;
		}
	}
	m_i_idx_seq = p_idx->m_head.m_sequence;
	if (!m_b_report_quit)
	{
		if (m_fp_idx == NULL)
		{
			time_t now = time(NULL);
			tm* ltm = localtime(&now);

			char	str_full_name[1024];
			memset(str_full_name, 0, sizeof(str_full_name));
			sprintf(str_full_name, "%04d%02d%02d_sze_idx.dat", ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday);

			m_fp_idx = fopen(str_full_name, "wb+");
			if (m_fp_idx == NULL)
			{
				return;
			}
		}

		int ret = fwrite((char*)p_idx, 1, sizeof(*p_idx), m_fp_idx);
		fflush(m_fp_idx);
		if (ret <= 0)
		{
			printf("write sze_idx.dat error!\n");
		}

		++m_ll_idx_count;

		return;
	}

	if (!is_vaild_symbol((char*)(p_idx->m_head.m_symbol)))
	{
		return;
	}

	timespec					ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	int node = m_ll_idx_count % QT_SZE_QUOTE_COUNT;

	memcpy(m_idx[node].m_symbol, p_idx->m_head.m_symbol, 9);
	m_idx[node].m_local_time		= ((long long)(ts.tv_sec)) * 1000000000 + ts.tv_nsec;
	m_idx[node].m_quote_update_time = p_idx->m_head.m_quote_update_time;
	m_idx[node].m_pre_close_price	= p_idx->m_pre_close_price;
	m_idx[node].m_open_price		= p_idx->m_open_price;
	m_idx[node].m_day_high_price	= p_idx->m_day_high_price;
	m_idx[node].m_day_low_price		= p_idx->m_day_low_price;
	m_idx[node].m_last_price		= p_idx->m_last_price;
	m_idx[node].m_total_quantity	= p_idx->m_total_quantity;

	++m_ll_idx_count;
}

void sze_report::on_report_efh_sze_lev2_tree(sze_hpf_tree* p_tree)
{
	if(m_i_tree_seq!=0)
	{
		if(p_tree->m_head.m_sequence <= m_i_tree_seq)
		{
			m_b_tree_rollback_flag = true;
			printf("SZE tree out of seq, last seq[%u], cur seq[%u].\n", m_i_tree_seq, p_tree->m_head.m_sequence);
		}
		else
		{
			m_ll_tree_count_lost += p_tree->m_head.m_sequence - m_i_tree_seq - 1;
		}
	}
	m_i_tree_seq = p_tree->m_head.m_sequence;
	if (!m_b_report_quit)
	{
		if (m_fp_tree == NULL)
		{
			time_t now = time(NULL);
			tm* ltm = localtime(&now);

			char	str_full_name[1024];
			memset(str_full_name, 0, sizeof(str_full_name));
			sprintf(str_full_name, "%04d%02d%02d_sze_tree.dat", ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday);

			m_fp_tree = fopen(str_full_name, "wb+");
			if (m_fp_tree == NULL)
			{
				return;
			}
		}

		int ret = fwrite((char*)p_tree, 1, sizeof(*p_tree), m_fp_tree);
		fflush(m_fp_tree);
		if (ret <= 0)
		{
			printf("write sze_tree.csv error!\t msg\n");
		}

		++m_ll_tree_count;

		return;
	}

	if (!is_vaild_symbol((char*)(p_tree->m_head.m_symbol)))
	{
		return;
	}

	timespec					ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	int node = m_ll_tree_count % QT_SZE_QUOTE_COUNT;

	memcpy(m_tree[node].m_symbol, p_tree->m_head.m_symbol, 9);
	m_tree[node].m_quote_update_time		= p_tree->m_head.m_quote_update_time;
	m_tree[node].m_local_time				= ((long long)(ts.tv_sec)) * 1000000000 + ts.tv_nsec;

	m_tree[node].m_sequence              = p_tree->m_head.m_sequence;
	m_tree[node].m_bid_lev_1.m_price     = p_tree->m_bid_unit[0].m_price;
	m_tree[node].m_bid_lev_1.m_quantity  = p_tree->m_bid_unit[0].m_quantity;
	m_tree[node].m_bid_lev_10.m_price    = p_tree->m_bid_unit[9].m_price;
	m_tree[node].m_bid_lev_10.m_quantity = p_tree->m_bid_unit[9].m_quantity;
	m_tree[node].m_ask_lev_1.m_price     = p_tree->m_ask_unit[0].m_price;
	m_tree[node].m_ask_lev_1.m_quantity  = p_tree->m_ask_unit[0].m_quantity;
	m_tree[node].m_ask_lev_10.m_price    = p_tree->m_ask_unit[9].m_price;
	m_tree[node].m_ask_lev_10.m_quantity = p_tree->m_ask_unit[9].m_quantity;

	m_tree[node].m_open_price                   = p_tree->m_open_price;
	m_tree[node].m_day_high_price               = p_tree->m_day_high_price;
	m_tree[node].m_day_low_price                = p_tree->m_day_low_price;
	m_tree[node].m_last_price                   = p_tree->m_last_price;
	m_tree[node].m_total_quantity               = p_tree->m_total_quantity;
	m_tree[node].m_total_bid_quantity           = p_tree->m_total_bid_quantity;
	m_tree[node].m_total_ask_quantity           = p_tree->m_total_ask_quantity;
	m_tree[node].m_pre_close_price              = p_tree->m_pre_close_price;
	m_tree[node].m_total_value                  = p_tree->m_total_value;
	m_tree[node].m_total_bid_weighted_avg_price = p_tree->m_total_bid_weighted_avg_price;
	m_tree[node].m_total_ask_weighted_avg_price = p_tree->m_total_ask_weighted_avg_price;
	m_tree[node].m_bid_depth                    = p_tree->m_bid_depth;
	m_tree[node].m_ask_depth                    = p_tree->m_ask_depth;
	m_tree[node].m_market_open_total_bid        = p_tree->m_market_open_total_bid;
	m_tree[node].m_market_open_total_ask        = p_tree->m_market_open_total_ask;


	++m_ll_tree_count;
}

void sze_report::on_report_efh_sze_lev2_ibr_tree(sze_hpf_ibr_tree* p_ibr_tree)
{
	if(m_i_ibr_tree_seq!=0)
	{
		if(p_ibr_tree->m_head.m_sequence <= m_i_ibr_tree_seq)
		{
			m_b_ibr_tree_rollback_flag = true;
			printf("SZE ibr_tree out of seq, last seq[%u], cur seq[%u].\n", m_i_ibr_tree_seq, p_ibr_tree->m_head.m_sequence);
		}
		else
		{
			m_ll_ibr_tree_count_lost += p_ibr_tree->m_head.m_sequence - m_i_ibr_tree_seq - 1;
		}
	}
	m_i_ibr_tree_seq = p_ibr_tree->m_head.m_sequence;
	if (!m_b_report_quit)
	{
		if (m_fp_ibr_tree == NULL)
		{
			time_t now = time(NULL);
			tm* ltm = localtime(&now);

			char	str_full_name[1024];
			memset(str_full_name, 0, sizeof(str_full_name));
			sprintf(str_full_name, "%04d%02d%02d_sze_ibr_tree.dat", ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday);

			m_fp_ibr_tree = fopen(str_full_name, "wb+");
			if (m_fp_ibr_tree == NULL)
			{
				return;
			}
		}

		int ret = fwrite((char*)p_ibr_tree, 1, sizeof(*p_ibr_tree), m_fp_ibr_tree);
		fflush(m_fp_ibr_tree);
		if (ret <= 0)
		{
			printf("write sze_ibr_tree.csv error!\t msg\n");
		}

		++m_ll_ibr_tree_count;

		return;
	}

	if (!is_vaild_symbol((char*)(p_ibr_tree->m_head.m_symbol)))
	{
		return;
	}


	timespec					ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	int node = m_ll_ibr_tree_count % QT_SZE_QUOTE_COUNT;

	memcpy(m_ibr_tree[node].m_symbol, p_ibr_tree->m_head.m_symbol, 9);
	m_ibr_tree[node].m_quote_update_time    = p_ibr_tree->m_head.m_quote_update_time;
	m_ibr_tree[node].m_local_time           = ((long long)(ts.tv_sec)) * 1000000000 + ts.tv_nsec;
	m_ibr_tree[node].m_sequence             = p_ibr_tree->m_head.m_sequence;
	m_ibr_tree[node].m_bid_lev_1.m_price    = p_ibr_tree->m_bid_unit[0].m_price;
	m_ibr_tree[node].m_bid_lev_1.m_quantity = p_ibr_tree->m_bid_unit[0].m_quantity;
	m_ibr_tree[node].m_bid_lev_5.m_price    = p_ibr_tree->m_bid_unit[4].m_price;
	m_ibr_tree[node].m_bid_lev_5.m_quantity = p_ibr_tree->m_bid_unit[4].m_quantity;
	m_ibr_tree[node].m_ask_lev_1.m_price    = p_ibr_tree->m_bid_unit[0].m_price;
	m_ibr_tree[node].m_ask_lev_1.m_quantity = p_ibr_tree->m_bid_unit[0].m_quantity;
	m_ibr_tree[node].m_ask_lev_5.m_price    = p_ibr_tree->m_bid_unit[4].m_price;
	m_ibr_tree[node].m_ask_lev_5.m_quantity = p_ibr_tree->m_bid_unit[4].m_quantity;
	m_ibr_tree[node].m_ask_depth            = p_ibr_tree->m_ask_depth;
	m_ibr_tree[node].m_bid_depth            = p_ibr_tree->m_bid_depth;


	++m_ll_ibr_tree_count;
}

void sze_report::on_report_efh_sze_lev2_turnover(sze_hpf_turnover* p_turnover)
{
	if(m_i_turnover_seq!=0)
	{
		if(p_turnover->m_head.m_sequence <= m_i_turnover_seq)
		{
			m_b_turnover_rollback_flag = true;
			printf("SZE turnover out of seq, last seq[%u], cur seq[%u].\n", m_i_turnover_seq, p_turnover->m_head.m_sequence);
		}
		else
		{
			m_ll_turnover_count_lost += p_turnover->m_head.m_sequence - m_i_turnover_seq - 1;
		}
	}
	m_i_turnover_seq = p_turnover->m_head.m_sequence;
	if (!m_b_report_quit)
	{
		if (m_fp_turnover == NULL)
		{
			time_t now = time(NULL);
			tm* ltm = localtime(&now);

			char	str_full_name[1024];
			memset(str_full_name, 0, sizeof(str_full_name));
			sprintf(str_full_name, "%04d%02d%02d_sze_turnover.dat", ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday);

			m_fp_turnover = fopen(str_full_name, "wb+");
			if (m_fp_turnover == NULL)
			{
				return;
			}
		}

		int ret = fwrite((char*)p_turnover, 1, sizeof(*p_turnover), m_fp_turnover);
		fflush(m_fp_turnover);
		if (ret <= 0)
		{
			printf("write sze_turnover.csv error!\t msg\n");
		}

		++m_ll_turnover_count;

		return;
	}

	if (!is_vaild_symbol((char*)(p_turnover->m_head.m_symbol)))
	{
		return;
	}


	timespec					ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	int node = m_ll_turnover_count % QT_SZE_QUOTE_COUNT;

	memcpy(m_turnover[node].m_symbol, p_turnover->m_head.m_symbol, 9);
	m_turnover[node].m_quote_update_time = p_turnover->m_head.m_quote_update_time;
	m_turnover[node].m_local_time        = ((long long)(ts.tv_sec)) * 1000000000 + ts.tv_nsec;
	m_turnover[node].m_sequence          = p_turnover->m_head.m_sequence;
	m_turnover[node].m_total_trade_num   = p_turnover->m_total_trade_num;
	m_turnover[node].m_total_quantity    = p_turnover->m_total_quantity;
	m_turnover[node].m_total_value       = p_turnover->m_total_value;
	++m_ll_turnover_count;

}


void sze_report::on_report_efh_sze_lev2_bond_snap(sze_hpf_bond_snap* p_snap)
{
	if(m_i_bond_snap_seq!=0)
	{
		if(p_snap->m_head.m_sequence <= m_i_bond_snap_seq)
		{
			m_b_bond_snap_rollback_flag = true;
			printf("SZE bond_snap out of seq, last seq[%u], cur seq[%u].\n", m_i_bond_snap_seq, p_snap->m_head.m_sequence);
		}
		else
		{
			m_ll_bond_snap_count_lost += p_snap->m_head.m_sequence - m_i_bond_snap_seq - 1;
		}
	}
	m_i_bond_snap_seq = p_snap->m_head.m_sequence;
	if (!m_b_report_quit)
	{
		if (m_fp_bond_snap == NULL)
		{
			time_t now = time(NULL);
			tm* ltm = localtime(&now);

			char	str_full_name[1024];
			memset(str_full_name, 0, sizeof(str_full_name));
			sprintf(str_full_name, "%04d%02d%02d_sze_bond_snap.dat", ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday);

			m_fp_bond_snap = fopen(str_full_name, "wb+");
			if (m_fp_bond_snap == NULL)
			{
				return;
			}
		}

		int ret = fwrite((char*)p_snap, 1, sizeof(*p_snap), m_fp_bond_snap);
		fflush(m_fp_bond_snap);
		if (ret <= 0)
		{
			printf("write sze_bond_snap.csv error!\t msg\n");
		}
		++m_ll_bond_snap_count;

		return;
	}

	if (!is_vaild_symbol((char*)(p_snap->m_head.m_symbol)))
	{
		return;
	}

	timespec			ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	int node = m_ll_bond_snap_count % QT_SZE_QUOTE_COUNT;

	memcpy(m_bond_snap[node].m_symbol, p_snap->m_head.m_symbol, 9);
	m_bond_snap[node].m_quote_update_time = p_snap->m_head.m_quote_update_time;
	m_bond_snap[node].m_local_time        = ((long long)(ts.tv_sec)) * 1000000000 + ts.tv_nsec;

	m_bond_snap[node].m_bid_lev_1.m_price     = p_snap->m_bid_unit[0].m_price;
	m_bond_snap[node].m_bid_lev_1.m_quantity  = p_snap->m_bid_unit[0].m_quantity;
	m_bond_snap[node].m_bid_lev_10.m_price    = p_snap->m_bid_unit[9].m_price;
	m_bond_snap[node].m_bid_lev_10.m_quantity = p_snap->m_bid_unit[9].m_quantity;
	m_bond_snap[node].m_ask_lev_1.m_price     = p_snap->m_ask_unit[0].m_price;
	m_bond_snap[node].m_ask_lev_1.m_quantity  = p_snap->m_ask_unit[0].m_quantity;
	m_bond_snap[node].m_ask_lev_10.m_price    = p_snap->m_ask_unit[9].m_price;
	m_bond_snap[node].m_ask_lev_10.m_quantity = p_snap->m_ask_unit[9].m_quantity;


	m_bond_snap[node].m_bid_lev_5.m_price    = p_snap->m_bid_unit[4].m_price;
	m_bond_snap[node].m_bid_lev_5.m_quantity = p_snap->m_bid_unit[4].m_quantity;
	m_bond_snap[node].m_ask_lev_5.m_price    = p_snap->m_ask_unit[4].m_price;
	m_bond_snap[node].m_ask_lev_5.m_quantity = p_snap->m_ask_unit[4].m_quantity;

	m_bond_snap[node].m_last_price     = p_snap->m_last_price;
	m_bond_snap[node].m_total_quantity = p_snap->m_total_quantity;
	m_bond_snap[node].m_total_value    = p_snap->m_total_value;

	++m_ll_snap_count;
}

void sze_report::on_report_efh_sze_lev2_bond_tick(int msg_type, sze_hpf_bond_order* p_order, sze_hpf_bond_exe* p_exe)
{
	if (msg_type == SZE_LEV2_BOND_ORDER_MSG_TYPE)
	{
		if (!p_order)
		{
			string msg = format_str("write lev2 bond tick error: p_order == NULL\n");
			efh_sze_lev2_error(msg.c_str(), msg.length());
			return;
		}
		if(m_i_bond_tick_seq!=0)
		{
			if(p_order->m_head.m_sequence <= m_i_bond_tick_seq)
			{
				m_b_bond_tick_rollback_flag = true;
				printf("SZE bond_tick out of seq, last seq[%u], cur seq[%u].\n", m_i_bond_tick_seq, p_order->m_head.m_sequence);
			}
			else
			{
				m_ll_bond_tick_count_lost += p_order->m_head.m_sequence - m_i_bond_tick_seq - 1;
			}
		}
		m_i_bond_tick_seq = p_order->m_head.m_sequence;
		if (!m_b_report_quit)
		{
			if(m_b_tick_detach)
			{
				if (m_fp_bond_ord == NULL)
				{
					time_t now = time(NULL);
					tm* ltm = localtime(&now);

					char	str_full_name[1024];
					memset(str_full_name, 0, sizeof(str_full_name));
					sprintf(str_full_name, "%04d%02d%02d_sze_bond_order.dat", ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday);

					m_fp_bond_ord = fopen(str_full_name, "wb+");
					if (m_fp_bond_ord == NULL)
					{
						return;
					}
				}

				int ret = fwrite((char*)p_order, 1, sizeof(*p_order), m_fp_bond_ord);
				fflush(m_fp_bond_ord);
				if (ret <= 0)
				{
					printf("write sze_order.dat error!\t");
				}
			}
			else
			{
				if (m_fp_bond_tick == NULL)
				{
					time_t now = time(NULL);
					tm* ltm = localtime(&now);

					char	str_full_name[1024];
					memset(str_full_name, 0, sizeof(str_full_name));
					sprintf(str_full_name, "%04d%02d%02d_sze_bond_tick.dat", ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday);

					m_fp_bond_tick = fopen(str_full_name, "wb+");
					if (m_fp_bond_tick == NULL)
					{
						return;
					}
				}

				int ret = fwrite((char*)p_order, 1, sizeof(*p_order), m_fp_bond_tick);
				fflush(m_fp_bond_tick);
				if (ret <= 0)
				{
					printf("write sze_order.dat error!\t");
				}
			}
			++m_ll_bond_order_count;
			return;
		}

		if (!is_vaild_symbol((char*)(p_order->m_head.m_symbol)))
		{
			return;
		}

		timespec				ts;
		clock_gettime(CLOCK_REALTIME, &ts);
		int node = m_ll_bond_order_count % QT_SZE_QUOTE_COUNT;

		memcpy(m_bond_order[node].m_symbol, p_order->m_head.m_symbol, 9);
		m_bond_order[node].m_local_time        = ((long long)(ts.tv_sec)) * 1000000000 + ts.tv_nsec;
		m_bond_order[node].m_channel_num       = p_order->m_head.m_channel_num;
		m_bond_order[node].m_sequence_num      = p_order->m_head.m_sequence_num;
		m_bond_order[node].m_quote_update_time = p_order->m_head.m_quote_update_time;
		m_bond_order[node].m_order_quantity    = p_order->m_quantity;
		m_bond_order[node].m_order_price       = p_order->m_price;
		m_bond_order[node].m_order_type        = p_order->m_order_type;

		++m_ll_bond_order_count;
	}
	else if (msg_type == SZE_LEV2_BOND_EXE_MSG_TYPE)
	{
		if (!p_exe)
		{
			string msg = format_str("write lev2 bond tick error: p_exe == NULL\n");
			efh_sze_lev2_error(msg.c_str(), msg.length());
			return;
		}
		if(m_i_bond_tick_seq!=0)
		{
			if (p_exe->m_head.m_sequence <= m_i_bond_tick_seq)
			{
				m_b_bond_tick_rollback_flag = true;
				printf("SZE bond_tick out of seq, last seq[%u], cur seq[%u].\n", m_i_bond_tick_seq, p_exe->m_head.m_sequence);
			}
			else
			{
				m_ll_bond_tick_count_lost += p_exe->m_head.m_sequence - m_i_bond_tick_seq - 1;
			}
		}
		m_i_bond_tick_seq = p_exe->m_head.m_sequence;
		if (!m_b_report_quit)
		{
			if(m_b_tick_detach)
			{
				if (m_fp_bond_exe == NULL)
				{
					time_t now = time(NULL);
					tm* ltm = localtime(&now);

					char	str_full_name[1024];
					memset(str_full_name, 0, sizeof(str_full_name));
					sprintf(str_full_name, "%04d%02d%02d_sze_bond_exe.dat", ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday);

					m_fp_bond_exe = fopen(str_full_name, "wb+");
					if (m_fp_bond_exe == NULL)
					{
						return;
					}
				}

				int ret = fwrite((char*)p_exe, 1, sizeof(*p_exe), m_fp_bond_exe);
				fflush(m_fp_bond_exe);
				if (ret <= 0)
				{
					printf("write sze_exe.dat error!\n");
				}
			}
			else
			{
				if (m_fp_bond_tick == NULL)
				{
					time_t now = time(NULL);
					tm* ltm = localtime(&now);

					char	str_full_name[1024];
					memset(str_full_name, 0, sizeof(str_full_name));
					sprintf(str_full_name, "%04d%02d%02d_sze_bond_tick.dat", ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday);

					m_fp_bond_tick = fopen(str_full_name, "wb+");
					if (m_fp_bond_tick == NULL)
					{
						return;
					}
				}

				int ret = fwrite((char*)p_exe, 1, sizeof(*p_exe), m_fp_bond_tick);
				fflush(m_fp_bond_tick);
				if (ret <= 0)
				{
					printf("write sze_tick.dat error!\n");
				}
			}
			++m_ll_bond_exe_count;

			return;
		}

		if (!is_vaild_symbol((char*)(p_exe->m_head.m_symbol)))
		{
			return;
		}

		timespec			ts;
		clock_gettime(CLOCK_REALTIME, &ts);
		int node = m_ll_bond_exe_count % QT_SZE_QUOTE_COUNT;

		memcpy(m_bond_exe[node].m_symbol, p_exe->m_head.m_symbol, 9);
		m_bond_exe[node].m_local_time		= ((long long)(ts.tv_sec)) * 1000000000 + ts.tv_nsec;
		m_bond_exe[node].m_channel_num		= p_exe->m_head.m_channel_num;
		m_bond_exe[node].m_sequence_num		= p_exe->m_head.m_sequence_num;
		m_bond_exe[node].m_quote_update_time	= p_exe->m_head.m_quote_update_time;
		m_bond_exe[node].m_trade_price		= p_exe->m_price;
		m_bond_exe[node].m_trade_quantity	= p_exe->m_quantity;
		m_bond_exe[node].m_trade_type		= p_exe->m_type;

		++m_ll_bond_exe_count;
	}
}




void sze_report::efh_sze_lev2_debug(const char* msg, int len)
{
	printf("[DEBUG] %s\n",msg);
}

void sze_report::efh_sze_lev2_error(const char* msg, int len)
{
	printf("[ERROR] %s\n", msg);
}

void sze_report::efh_sze_lev2_info(const char* msg, int len)
{
	printf("[INFO] %s\n", msg);
}


string sze_report::format_str(const char* pFormat, ...)
{
	va_list args;
	va_start(args, pFormat);
	char buffer[40960];
	vsnprintf(buffer, 40960, pFormat, args);
	va_end(args);
	return string(buffer);
}

void sze_report::report_efh_sze_lev2_order()
{
	if (m_fp_ord == NULL)
	{
		time_t now = time(NULL);
		tm* ltm = localtime(&now);

		char	str_full_name[1024];
		memset(str_full_name, 0, sizeof(str_full_name));
		sprintf(str_full_name, "%04d%02d%02d_sze_order.csv", ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday);

		m_fp_ord = fopen(str_full_name, "wt+");
		if (m_fp_ord == NULL)
		{
			return;
		}
	}

	if (m_ll_order_count > 0)
	{
		for (int i = 0; i < QT_SZE_QUOTE_COUNT; i++)
		{
			if (m_order[i].m_local_time == 0)
			{
				return;
			}

			char ch_buffer[1024];
			memset(ch_buffer, 0, sizeof(ch_buffer));
			sprintf(ch_buffer, "%s, %d, %lld, %lld, %llu, %d, %lld, %d\n",
				m_order[i].m_symbol,
				m_order[i].m_channel_num,
				m_order[i].m_sequence_num,
				m_order[i].m_quote_update_time,
				m_order[i].m_local_time,
				m_order[i].m_order_price,
				m_order[i].m_order_quantity,
				m_order[i].m_order_type);

			int ret = fwrite(ch_buffer, 1, strlen(ch_buffer), m_fp_ord);
			fflush(m_fp_ord);
			if (ret <= 0)
			{
				printf("write sze_order.csv error!\t msg : %s\n", ch_buffer);
			}
		}
	}
}

void sze_report::report_efh_sze_lev2_exe()
{
	if (m_fp_exe == NULL)
	{
		time_t now = time(NULL);
		tm* ltm = localtime(&now);

		char	str_full_name[1024];
		memset(str_full_name, 0, sizeof(str_full_name));
		sprintf(str_full_name, "%04d%02d%02d_sze_exe.csv", ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday);

		m_fp_exe = fopen(str_full_name, "wt+");
		if (m_fp_exe == NULL)
		{
			return;
		}
	}

	if (m_ll_exe_count > 0)
	{
		for (int i = 0; i < QT_SZE_QUOTE_COUNT; i++)
		{
			if (m_exe[i].m_local_time == 0)
			{
				return;
			}

			char ch_buffer[1024];
			memset(ch_buffer, 0, sizeof(ch_buffer));
			sprintf(ch_buffer, "%s, %d, %lld, %lld, %llu, %u, %llu, %c\n",
				m_exe[i].m_symbol,
				m_exe[i].m_channel_num,
				m_exe[i].m_sequence_num,
				m_exe[i].m_quote_update_time,
				m_exe[i].m_local_time,
				m_exe[i].m_trade_price,
				m_exe[i].m_trade_quantity,
				m_exe[i].m_trade_type);

			int ret = fwrite(ch_buffer, 1, strlen(ch_buffer), m_fp_exe);
			fflush(m_fp_exe);
			if (ret <= 0)
			{
				printf("write sze_exe.csv error!\t msg : %s\n", ch_buffer);
			}
		}
	}
}

void sze_report::report_efh_sze_lev2_after_close()
{
	if (m_fp_close_px == NULL)
	{
		time_t now = time(NULL);
		tm* ltm = localtime(&now);

		char	str_full_name[1024];
		memset(str_full_name, 0, sizeof(str_full_name));
		sprintf(str_full_name, "%04d%02d%02d_sze_after_close.csv", ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday);

		m_fp_close_px = fopen(str_full_name, "wt+");
		if (m_fp_close_px == NULL)
		{
			return;
		}
	}

	if (m_ll_after_count > 0)
	{
		for (int i = 0; i < QT_SZE_QUOTE_COUNT; i++)
		{
			if (m_after_close[i].m_local_time == 0)
			{
				return;
			}

			char ch_buffer[1024];
			memset(ch_buffer, 0, sizeof(ch_buffer));
			sprintf(ch_buffer, "%s, %d, %lld, %lld, %llu, %u, %lld, %llu, %llu, %u, %u, %llu, %llu\n",
				m_after_close[i].m_symbol,
				m_after_close[i].m_channel_num,
				m_after_close[i].m_sequence_num,
				m_after_close[i].m_quote_update_time,
				m_after_close[i].m_local_time,
				m_after_close[i].m_trading_status,
				m_after_close[i].m_total_trade_num,
				m_after_close[i].m_total_quantity,
				m_after_close[i].m_total_value,
				m_after_close[i].m_pre_close_price,
				m_after_close[i].m_exe_price,
				m_after_close[i].m_bid_quantity,
				m_after_close[i].m_ask_quantity);

			int ret = fwrite(ch_buffer, 1, strlen(ch_buffer), m_fp_close_px);
			fflush(m_fp_close_px);
			if (ret <= 0)
			{
				printf("write sze_after_close.csv error!\t msg : %s\n", ch_buffer);
			}
		}
	}
}

void sze_report::report_efh_sze_lev2_snap()
{
	if (m_fp_lev2 == NULL)
	{
		time_t now = time(NULL);
		tm* ltm = localtime(&now);

		char	str_full_name[1024];
		memset(str_full_name, 0, sizeof(str_full_name));
		sprintf(str_full_name, "%04d%02d%02d_sze_snap.csv", ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday);

		m_fp_lev2 = fopen(str_full_name, "wt+");
		if (m_fp_lev2 == NULL)
		{
			return;
		}
	}

	if (m_ll_snap_count > 0)
	{
		for (int i = 0; i < QT_SZE_QUOTE_COUNT; i++)
		{
			if (m_snap[i].m_local_time == 0)
			{
				return;
			}

			char ch_buffer[1024];
			memset(ch_buffer, 0, sizeof(ch_buffer));
			sprintf(ch_buffer, "%s, %lld, %llu, %u, %llu, %llu, %u, %lld, %u, %lld, %u, %lld, %u, %lld,%u, %lld, %u, %lld,\n",
				m_snap[i].m_symbol,
				m_snap[i].m_quote_update_time,
				m_snap[i].m_local_time,
				m_snap[i].m_last_price,
				m_snap[i].m_total_quantity,
				m_snap[i].m_total_value,
				m_snap[i].m_bid_lev_1.m_price,
				m_snap[i].m_bid_lev_1.m_quantity,
				m_snap[i].m_ask_lev_1.m_price,
				m_snap[i].m_ask_lev_1.m_quantity,
				m_snap[i].m_bid_lev_10.m_price,
				m_snap[i].m_bid_lev_10.m_quantity,
				m_snap[i].m_ask_lev_10.m_price,
				m_snap[i].m_ask_lev_10.m_quantity,
				m_snap[i].m_bid_lev_5.m_price,
				m_snap[i].m_bid_lev_5.m_quantity,
				m_snap[i].m_ask_lev_5.m_price,
				m_snap[i].m_ask_lev_5.m_quantity);

			int ret = fwrite(ch_buffer, 1, strlen(ch_buffer), m_fp_lev2);
			fflush(m_fp_lev2);
			if (ret <= 0)
			{
				printf("write sze_snap.csv error!\t msg : %s\n", ch_buffer);
			}
		}
	}
}

void sze_report::report_efh_sze_lev2_idx()
{
	if (m_fp_idx == NULL)
	{
		time_t now = time(NULL);
		tm* ltm = localtime(&now);

		char	str_full_name[1024];
		memset(str_full_name, 0, sizeof(str_full_name));
		sprintf(str_full_name, "%04d%02d%02d_sze_idx.csv", ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday);

		m_fp_idx = fopen(str_full_name, "wt+");
		if (m_fp_idx == NULL)
		{
			return;
		}
	}

	if (m_ll_idx_count > 0)
	{
		for (int i = 0; i < QT_SZE_QUOTE_COUNT; i++)
		{
			if (m_idx[i].m_local_time == 0)
			{
				return;
			}

			char ch_buffer[1024];
			memset(ch_buffer, 0, sizeof(ch_buffer));
			sprintf(ch_buffer, "%s, %lld, %llu, %llu, %u, %u, %u, %u, %u\n",
				m_idx[i].m_symbol,
				m_idx[i].m_quote_update_time,
				m_idx[i].m_local_time,
				m_idx[i].m_total_quantity,
				m_idx[i].m_last_price,
				m_idx[i].m_pre_close_price,
				m_idx[i].m_open_price,
				m_idx[i].m_day_high_price,
				m_idx[i].m_day_low_price);

			int ret = fwrite(ch_buffer, 1, strlen(ch_buffer), m_fp_idx);
			fflush(m_fp_idx);
			if (ret <= 0)
			{
				printf("write sze_idx.csv error!\t msg : %s\n", ch_buffer);
			}
		}
	}
}

void sze_report::report_efh_sze_lev2_tree()
{
	if (m_fp_tree == NULL)
	{
		time_t now = time(NULL);
		tm* ltm = localtime(&now);

		char	str_full_name[1024];
		memset(str_full_name, 0, sizeof(str_full_name));
		sprintf(str_full_name, "%04d%02d%02d_sze_tree.csv", ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday);

		m_fp_tree = fopen(str_full_name, "wt+");
		if (m_fp_tree == NULL)
		{
			return;
		}
	}

	if (m_ll_tree_count > 0)
	{
		for (int i = 0; i < QT_SZE_QUOTE_COUNT; i++)
		{
			if (m_tree[i].m_local_time == 0)
			{
				return;
			}

			char ch_buffer[1024];
			memset(ch_buffer, 0, sizeof(ch_buffer));
			sprintf(ch_buffer, "%u, %s, %lld, %llu, %u, %lld, %u, %lld, %u, %lld, %u, %lld, %u, %u, %u, %u, %u, %llu, %llu, %llu, %llu, %u, %u, %u, %u, %llu, %llu\n",
				m_tree[i].m_sequence,
				m_tree[i].m_symbol,
				m_tree[i].m_quote_update_time,
				m_tree[i].m_local_time,
				m_tree[i].m_bid_lev_1.m_price,
				m_tree[i].m_bid_lev_1.m_quantity,
				m_tree[i].m_ask_lev_1.m_price,
				m_tree[i].m_ask_lev_1.m_quantity,
				m_tree[i].m_bid_lev_10.m_price,
				m_tree[i].m_bid_lev_10.m_quantity,
				m_tree[i].m_ask_lev_10.m_price,
				m_tree[i].m_ask_lev_10.m_quantity,
				m_tree[i].m_pre_close_price,
				m_tree[i].m_open_price,
				m_tree[i].m_day_high_price,
				m_tree[i].m_day_low_price,
				m_tree[i].m_last_price,
				m_tree[i].m_total_quantity,
				m_tree[i].m_total_value,
				m_tree[i].m_total_bid_quantity,
				m_tree[i].m_total_ask_quantity,
				m_tree[i].m_total_bid_weighted_avg_price,
				m_tree[i].m_total_ask_weighted_avg_price,
				m_tree[i].m_bid_depth,
				m_tree[i].m_ask_depth,
				m_tree[i].m_market_open_total_bid,
				m_tree[i].m_market_open_total_ask);

			int ret = fwrite(ch_buffer, strlen(ch_buffer), 1, m_fp_tree);
			fflush(m_fp_tree);
			if (ret <= 0)
			{
				printf("write sze_tree.csv error!\t msg : %s\n", ch_buffer);
			}
		}
	}
}

void sze_report::report_efh_sze_lev2_ibr_tree()
{
	if (m_fp_ibr_tree == NULL)
	{
		time_t now = time(NULL);
		tm* ltm = localtime(&now);

		char	str_full_name[1024];
		memset(str_full_name, 0, sizeof(str_full_name));
		sprintf(str_full_name, "%04d%02d%02d_sze_ibr_tree.csv", ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday);

		m_fp_ibr_tree = fopen(str_full_name, "wt+");
		if (m_fp_ibr_tree == NULL)
		{
			return;
		}
	}

	if (m_ll_ibr_tree_count > 0)
	{
		for (int i = 0; i < QT_SZE_QUOTE_COUNT; i++)
		{
			if (m_ibr_tree[i].m_local_time == 0)
			{
				return;
			}

			char ch_buffer[1024];
			memset(ch_buffer, 0, sizeof(ch_buffer));
			sprintf(ch_buffer, "%u, %s, %lld, %llu, %u, %lld, %u, %lld, %u, %lld, %u, %lld\n",
				m_ibr_tree[i].m_sequence,
				m_ibr_tree[i].m_symbol,
				m_ibr_tree[i].m_quote_update_time,
				m_ibr_tree[i].m_local_time,
				m_ibr_tree[i].m_bid_lev_1.m_price,
				m_ibr_tree[i].m_bid_lev_1.m_quantity,
				m_ibr_tree[i].m_ask_lev_1.m_price,
				m_ibr_tree[i].m_ask_lev_1.m_quantity,
				m_ibr_tree[i].m_bid_lev_5.m_price,
				m_ibr_tree[i].m_bid_lev_5.m_quantity,
				m_ibr_tree[i].m_ask_lev_5.m_price,
				m_ibr_tree[i].m_ask_lev_5.m_quantity);

			int ret = fwrite(ch_buffer, strlen(ch_buffer), 1, m_fp_ibr_tree);
			fflush(m_fp_ibr_tree);
			if (ret <= 0)
			{
				printf("write sze_ibr_tree.csv error!\t msg : %s\n", ch_buffer);
			}
		}
	}

}

void sze_report::report_efh_sze_lev2_turnover()
{
	if (m_fp_turnover == NULL)
	{
		time_t now = time(NULL);
		tm* ltm = localtime(&now);

		char	str_full_name[1024];
		memset(str_full_name, 0, sizeof(str_full_name));
		sprintf(str_full_name, "%04d%02d%02d_sze_turnover.csv", ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday);

		m_fp_turnover = fopen(str_full_name, "wt+");
		if (m_fp_turnover == NULL)
		{
			return;
		}
	}

	if (m_ll_turnover_count > 0)
	{
		for (int i = 0; i < QT_SZE_QUOTE_COUNT; i++)
		{
			if (m_turnover[i].m_local_time == 0)
			{
				return;
			}

			char ch_buffer[1024];
			memset(ch_buffer, 0, sizeof(ch_buffer));
			sprintf(ch_buffer, "%u, %s, %lld, %llu, %lld, %llu, %llu\n",
				m_turnover[i].m_sequence,
				m_turnover[i].m_symbol,
				m_turnover[i].m_quote_update_time,
				m_turnover[i].m_local_time,
				m_turnover[i].m_total_trade_num,
				m_turnover[i].m_total_quantity,
				m_turnover[i].m_total_value);

			int ret = fwrite(ch_buffer, strlen(ch_buffer), 1, m_fp_turnover);
			fflush(m_fp_turnover);
			if (ret <= 0)
			{
				printf("write sze_turnover.csv error!\t msg : %s\n", ch_buffer);
			}
		}
	}
}

void sze_report::report_efh_sze_lev2_bond_order()
{
	if (m_fp_bond_ord == NULL)
	{
		time_t now = time(NULL);
		tm* ltm = localtime(&now);

		char	str_full_name[1024];
		memset(str_full_name, 0, sizeof(str_full_name));
		sprintf(str_full_name, "%04d%02d%02d_sze_bond_order.csv", ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday);

		m_fp_bond_ord = fopen(str_full_name, "wt+");
		if (m_fp_bond_ord == NULL)
		{
			return;
		}
	}

	if (m_ll_bond_order_count > 0)
	{
		for (int i = 0; i < QT_SZE_QUOTE_COUNT; i++)
		{
			if (m_bond_order[i].m_local_time == 0)
			{
				return;
			}

			char ch_buffer[1024];
			memset(ch_buffer, 0, sizeof(ch_buffer));
			sprintf(ch_buffer, "%s, %d, %lld, %lld, %llu, %d, %lld, %d\n",
				m_bond_order[i].m_symbol,
				m_bond_order[i].m_channel_num,
				m_bond_order[i].m_sequence_num,
				m_bond_order[i].m_quote_update_time,
				m_bond_order[i].m_local_time,
				m_bond_order[i].m_order_price,
				m_bond_order[i].m_order_quantity,
				m_bond_order[i].m_order_type);

			int ret = fwrite(ch_buffer, 1, strlen(ch_buffer), m_fp_bond_ord);
			fflush(m_fp_bond_ord);
			if (ret <= 0)
			{
				printf("write sze_bond_order.csv error!\t msg : %s\n", ch_buffer);
			}
		}
	}
}

void sze_report::report_efh_sze_lev2_bond_exe()
{
	if (m_fp_bond_exe == NULL)
	{
		time_t now = time(NULL);
		tm* ltm = localtime(&now);

		char	str_full_name[1024];
		memset(str_full_name, 0, sizeof(str_full_name));
		sprintf(str_full_name, "%04d%02d%02d_sze_bond_exe.csv", ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday);

		m_fp_bond_exe = fopen(str_full_name, "wt+");
		if (m_fp_bond_exe == NULL)
		{
			return;
		}
	}

	if (m_ll_bond_exe_count > 0)
	{
		for (int i = 0; i < QT_SZE_QUOTE_COUNT; i++)
		{
			if (m_bond_exe[i].m_local_time == 0)
			{
				return;
			}

			char ch_buffer[1024];
			memset(ch_buffer, 0, sizeof(ch_buffer));
			sprintf(ch_buffer, "%s, %d, %lld, %lld, %llu, %u, %llu, %c\n",
				m_bond_exe[i].m_symbol,
				m_bond_exe[i].m_channel_num,
				m_bond_exe[i].m_sequence_num,
				m_bond_exe[i].m_quote_update_time,
				m_bond_exe[i].m_local_time,
				m_bond_exe[i].m_trade_price,
				m_bond_exe[i].m_trade_quantity,
				m_bond_exe[i].m_trade_type);

			int ret = fwrite(ch_buffer, 1, strlen(ch_buffer), m_fp_bond_exe);
			fflush(m_fp_bond_exe);
			if (ret <= 0)
			{
				printf("write sze_bond_exe.csv error!\t msg : %s\n", ch_buffer);
			}
		}
	}
}

void sze_report::report_efh_sze_lev2_bond_snap()
{
	if (m_fp_bond_snap == NULL)
	{
		time_t now = time(NULL);
		tm* ltm = localtime(&now);

		char	str_full_name[1024];
		memset(str_full_name, 0, sizeof(str_full_name));
		sprintf(str_full_name, "%04d%02d%02d_sze_bond_snap.csv", ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday);

		m_fp_bond_snap = fopen(str_full_name, "wt+");
		if (m_fp_bond_snap == NULL)
		{
			return;
		}
	}

	if (m_ll_bond_snap_count > 0)
	{
		for (int i = 0; i < QT_SZE_QUOTE_COUNT; i++)
		{
			if (m_bond_snap[i].m_local_time == 0)
			{
				return;
			}

			char ch_buffer[1024];
			memset(ch_buffer, 0, sizeof(ch_buffer));
			sprintf(ch_buffer, "%s, %lld, %llu, %u, %llu, %llu, %u, %lld, %u, %lld, %u, %lld, %u, %lld,%u, %lld, %u, %lld,\n",
				m_bond_snap[i].m_symbol,
				m_bond_snap[i].m_quote_update_time,
				m_bond_snap[i].m_local_time,
				m_bond_snap[i].m_last_price,
				m_bond_snap[i].m_total_quantity,
				m_bond_snap[i].m_total_value,
				m_bond_snap[i].m_bid_lev_1.m_price,
				m_bond_snap[i].m_bid_lev_1.m_quantity,
				m_bond_snap[i].m_ask_lev_1.m_price,
				m_bond_snap[i].m_ask_lev_1.m_quantity,
				m_bond_snap[i].m_bid_lev_10.m_price,
				m_bond_snap[i].m_bid_lev_10.m_quantity,
				m_bond_snap[i].m_ask_lev_10.m_price,
				m_bond_snap[i].m_ask_lev_10.m_quantity,
				m_bond_snap[i].m_bid_lev_5.m_price,
				m_bond_snap[i].m_bid_lev_5.m_quantity,
				m_bond_snap[i].m_ask_lev_5.m_price,
				m_bond_snap[i].m_ask_lev_5.m_quantity);

			int ret = fwrite(ch_buffer, 1, strlen(ch_buffer), m_fp_bond_snap);
			fflush(m_fp_bond_snap);
			if (ret <= 0)
			{
				printf("write sze_bond_snap.csv error!\t msg : %s\n", ch_buffer);
			}
		}
	}
}


string sze_report::get_sze_src_trading_phase_code(char ch_sl_trading_phase_code)
{
	char	ch_buf[8];
	memset(ch_buf, 0, sizeof(ch_buf));
	char ch_first = ' ';
	switch (ch_sl_trading_phase_code & 0xF0)
	{
	case 0x00:
		ch_first = 'S';
		break;
	case 0x10:
		ch_first = 'O';
		break;
	case 0x20:
		ch_first = 'T';
		break;
	case 0x30:
		ch_first = 'B';
		break;
	case 0x40:
		ch_first = 'C';
		break;
	case 0x50:
		ch_first= 'E';
		break;
	case 0x60:
		ch_first = 'H';
		break;
	case 0x70:
		ch_first = 'A';
		break;
	case 0x80:
		ch_first = 'V';
		break;
	default:
		break;
	}

	ch_buf[0] = ch_first;

	if ((ch_sl_trading_phase_code & 0x08) == 0x08)
	{
		ch_buf[1] = '1';
	}
	else if ((ch_sl_trading_phase_code & 0x08) == 0x00)
	{
		ch_buf[1] = '0';
	}

	string str_buf = ch_buf;
	return str_buf;
}
