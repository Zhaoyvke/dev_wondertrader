#include "sse_report.h"
#include <string>
#include <stdio.h>
#include <time.h>
#include <cstdarg>

#ifndef WINDOWS
#include <dlfcn.h>
#include <arpa/inet.h>
#endif

sse_report::sse_report()
{
	m_fp_lev2			= NULL;
	m_fp_idx			= NULL;
	m_fp_option			= NULL;
	m_fp_exe			= NULL;
	m_fp_order			= NULL;
	m_fp_tick			= NULL;
	m_fp_tree			= NULL;
	m_fp_bond			= NULL;
	m_fp_bond_tick		= NULL;
	m_fp_tick_merge		= NULL;
	m_fp_etf			= NULL;
	m_p_quote			= NULL;

#ifdef TEST_SHENGLI_CODE_CONVERSION
	m_fp_it_code_lev2			= NULL;
	m_fp_it_code_option			= NULL;
	m_fp_it_code_tree			= NULL;
	m_fp_it_code_bond			= NULL;
	m_fp_it_code_bond_tick		= NULL;
#endif
	m_b_snap_rollback_flag = false;
	m_b_idx_rollback_flag = false;
	m_b_option_rollback_flag = false;
	m_b_tick_rollback_flag = false;
	m_b_tree_rollback_flag = false;
	m_b_bond_rollback_flag = false;
	m_b_bond_tick_rollback_flag = false;
	m_b_tick_merge_rollback_flag = false;
	m_b_etf_rollback_flag = false;

	m_ll_idx_count		= 0;
	m_ll_snap_count		= 0;
	m_ll_option_count	= 0;
	m_ll_exe_count		= 0;
	m_ll_order_count	= 0;
	m_ll_tree_count		= 0;
	m_ll_bond_count		= 0;
	m_ll_bond_tick_count= 0;
	m_ll_tick_merge_count = 0;
	m_ll_etf_count		= 0;

	m_i_snap_num		= 0;
	m_i_idx_num			= 0;
	m_i_option_num		= 0;
	m_i_tick_num		= 0;
	m_i_tree_num		= 0;
	m_i_bond_num		= 0;
	m_i_bond_tick_num	= 0;
	m_i_tick_merge_num	= 0;
	m_i_etf_num			= 0;

	m_i_lost_idx_count	= 0;
	m_i_lost_snap_count = 0;
	m_i_lost_option_count	= 0;
	m_i_lost_tick_count = 0;
	m_i_lost_tree_count = 0;
	m_i_lost_bond_count	= 0;
	m_i_lost_bond_tick_count = 0;
	m_i_lost_tick_merge_count = 0;
	m_i_lost_etf_count = 0;

	m_b_report_quit		= false;
	m_b_tick_detach		= false;

	m_snap 				= NULL;
	m_idx 				= NULL;
	m_option 			= NULL;
	m_exe 				= NULL;
	m_order 			= NULL;
	m_tree 				= NULL;
	m_bond_snap 		= NULL;
	m_bond_tick 		= NULL;
	m_tick_merge		= NULL;
	m_etf				= NULL;

#ifdef WINDOWS
	m_h_core = NULL;
#else
	m_h_core = NULL;
#endif
}

sse_report::~sse_report()
{
}

bool sse_report::init(socket_udp_param* p_param, int num)
{
	const char* err_address = "operation::init:";
#ifdef WINDOWS
	m_h_core = LoadLibraryA( DLL_EFH_LEV2_DLL_NAME );
	if (m_h_core == NULL)
	{
		string msg = format_str("%s init：load dll:%s !\n", err_address, DLL_EFH_LEV2_DLL_NAME);
		efh_sse_lev2_error(msg.c_str(), msg.length());
		return false;
	}

	func_create_efh_sse_lev2_api func_create = (func_create_efh_sse_lev2_api)GetProcAddress(m_h_core, CREATE_EFH_SSE_LEV2_API_FUNCTION);
	if (func_create == NULL)
	{
		string msg = format_str("%s get create sqs function ptr failed.\n", err_address);
		efh_sse_lev2_error(msg.c_str(), msg.length());
		return false;
	}
#else
	m_h_core = dlopen(DLL_EFH_LEV2_DLL_NAME, RTLD_LAZY);
	if (m_h_core == NULL)
	{
		string msg = format_str("%s init：load dll:%s error：%s!\n", err_address, DLL_EFH_LEV2_DLL_NAME, dlerror());
		efh_sse_lev2_error(msg.c_str(), msg.length());
		return false;
	}

	func_create_efh_sse_lev2_api func_create = (func_create_efh_sse_lev2_api)dlsym(m_h_core, CREATE_EFH_SSE_LEV2_API_FUNCTION);
	if (func_create == NULL)
	{
		string msg = format_str("%s get create sqs function ptr failed.\n", err_address);
		efh_sse_lev2_error(msg.c_str(), msg.length());
		return false;
	}
#endif
	m_p_quote = func_create();
	if (m_p_quote == NULL)
	{
		string msg = format_str("%s create sqs function ptr null.\n", err_address);
		efh_sse_lev2_error(msg.c_str(), msg.length());
		return false;
	}

	if (!m_p_quote->init_sse(p_param, num, static_cast<efh_sse_lev2_api_event*>(this), static_cast<efh_sse_lev2_api_depend*>(this)))
	{
		string msg = format_str("%s init parse! error\n", err_address);
		efh_sse_lev2_error(msg.c_str(), msg.length());
		return false;
	}

	return true;
}

void sse_report::set_tick_detach( bool enable )
{
	m_b_tick_detach = enable;
}

void sse_report::run(int nic_type, bool b_report_quit, const char* symbol)
{
	m_b_report_quit = b_report_quit;

	if (m_b_report_quit)
	{

		m_snap      = new sse_qt_node_snap[QT_SSE_QUOTE_COUNT];
		m_idx       = new sse_qt_node_index[QT_SSE_QUOTE_COUNT];
		m_option    = new sse_qt_node_stock_option[QT_SSE_QUOTE_COUNT];
		m_exe       = new sse_qt_node_exe[QT_SSE_QUOTE_COUNT];
		m_order     = new sse_qt_node_order[QT_SSE_QUOTE_COUNT];
		m_tree      = new sse_qt_node_tree[QT_SSE_QUOTE_COUNT];
		m_bond_snap = new sse_qt_node_bond_snap[QT_SSE_QUOTE_COUNT];
		m_bond_tick = new sse_qt_node_bond_tick[QT_SSE_QUOTE_COUNT];
		m_tick_merge = new sse_qt_node_tick_merge[QT_SSE_QUOTE_COUNT];
		m_etf = new sse_qt_node_etf[QT_SSE_QUOTE_COUNT];

		memset( m_snap , 0 , sizeof( sse_qt_node_snap ) * QT_SSE_QUOTE_COUNT );
		memset( m_idx , 0 , sizeof( sse_qt_node_index ) * QT_SSE_QUOTE_COUNT );
		memset( m_option , 0 , sizeof( sse_qt_node_stock_option ) * QT_SSE_QUOTE_COUNT );
		memset( m_exe , 0 , sizeof( sse_qt_node_exe ) * QT_SSE_QUOTE_COUNT );
		memset( m_order , 0 , sizeof( sse_qt_node_order ) * QT_SSE_QUOTE_COUNT );
		memset( m_tree , 0 , sizeof( sse_qt_node_tree ) * QT_SSE_QUOTE_COUNT );
		memset( m_bond_snap , 0 , sizeof( sse_qt_node_bond_snap ) * QT_SSE_QUOTE_COUNT );
		memset( m_bond_tick , 0 , sizeof( sse_qt_node_bond_tick ) * QT_SSE_QUOTE_COUNT );
		memset( m_tick_merge , 0 , sizeof( sse_qt_node_tick_merge ) * QT_SSE_QUOTE_COUNT );
		memset( m_etf , 0 , sizeof( sse_qt_node_etf ) * QT_SSE_QUOTE_COUNT );

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
		if (!m_p_quote->start_sse( enum_efh_nic_type(nic_type) ))
		{
			string msg = format_str( "start parse error\n" );
			efh_sse_lev2_error( msg.c_str( ) , msg.length( ) );
		}
	}
	break;

	default:
	{
		string msg = format_str( "nic type:%d error\n" , nic_type );
		efh_sse_lev2_error( msg.c_str( ) , msg.length( ) );
	}
	break;
	}

}

void sse_report::show()
{
	printf( "--------------------< SSE Info >--------------------\n" );
	printf("SSE_snap count: %lld, lost_count:%u, rollback_flag:%d.\n",
		m_ll_snap_count, m_i_lost_snap_count, (int)m_b_snap_rollback_flag);
	printf("SSE_idx count: %lld, lost_count:%u, rollback_flag:%d.\n",
		m_ll_idx_count, m_i_lost_idx_count, (int)m_b_idx_rollback_flag);
	printf("SSE_tick count:%lld, lost_count:%u, rollback_flag:%d.\n",
		m_ll_order_count + m_ll_exe_count, m_i_lost_tick_count, (int)m_b_tick_rollback_flag);
	printf("SSE_exe count: %lld\n", m_ll_exe_count);
	printf("SSE_order count: %lld\n",m_ll_order_count);
	printf("SSE_tree count: %lld, lost_count:%u, rollback_flag:%d.\n",
		m_ll_tree_count, m_i_lost_tree_count, (int)m_b_tree_rollback_flag);
	printf("SSE_option count: %lld, lost_count:%u, rollback_flag:%d.\n",
		m_ll_option_count, m_i_lost_option_count, (int)m_b_option_rollback_flag);
	printf("SSE_bond_snap count: %lld, lost_count:%u, rollback_flag:%d.\n",
		m_ll_bond_count, m_i_lost_bond_count, (int)m_b_bond_rollback_flag);
	printf("SSE_bond_tick count: %lld, lost_count:%u, rollback_flag:%d.\n",
		m_ll_bond_tick_count, m_i_lost_bond_tick_count, (int)m_b_bond_tick_rollback_flag);

	printf("SSE_tick_merge count: %lld, lost_count:%u, rollback_flag:%d.\n",
		m_ll_tick_merge_count, m_i_lost_tick_merge_count, (int)m_b_tick_merge_rollback_flag);

	printf("SSE_etf count: %lld, lost_count:%u, rollback_flag:%d.\n",
		m_ll_etf_count, m_i_lost_etf_count, (int)m_b_etf_rollback_flag);

	fflush(stdout);
}

void sse_report::show_rdp(const char* symbol)
{
	sse_static_msg_body msg;
	int ret = m_p_quote->get_static_info( symbol , msg );
	switch (ret)
	{
	case SSE_STATIC_INFO_OK:
		printf("%u,%s,%u,%s,%d,%lf,%lf,%llu,%llu,%llu,%llu,%lf,%llu,%llu,%s,%u,%u,%d,%d,%s,%s\n"
			, msg.m_exchange_id                             // 交易所id
			, msg.m_symbol                                  // 证券代码
			, msg.m_send_time                               // 行情发送时间，时分秒毫秒
			, msg.m_static_file_date                        // 静态文件日期，YYYYMMDD, 以'\0'结束

			, msg.m_price_limit_type                        // 跌涨停限制类型
			, msg.m_up_limit_price                          // 涨停价
			, msg.m_down_limit_price                        // 跌停价
			, msg.m_bid_qty_unit                            // 买数量单位
			, msg.m_ask_qty_unit                            // 卖数量单位
			, msg.m_limit_upper_qty                         // 限价申报数量上限
			, msg.m_limit_lower_qty                         // 限价申报数量下限
			, msg.m_price_changge_unit                      // 申报最小变价单位
			, msg.m_market_upper_qty                        // 市价申报数量上限
			, msg.m_market_lower_qty                        // 市价申报数量下限
			, msg.m_security_name                           // 证券名称，以'\0'结束
			, msg.m_ssecurity_type                          // 证券类型
			, msg.m_sub_ssecurity_type                      // 证券子类型
			, msg.m_finance_target_mark                     // 融资标的标志
			, msg.m_ssecurity_target_mark                   // 融券标的标志
			, msg.m_product_status                          // 产品状态, 以'\0'结束
			, msg.m_listing_date                            // 上市日期，格式为YYYYMMDD, 以'\0'结束
		);
		break;
	case SSE_STATIC_INFO_SYMBOL_IS_INCORRECT:
		printf("symbol <%s> is incorrect\n",symbol);
		break;
	case SSE_STATIC_INFO_NOT_FOUND_SYMBOL:
		printf("symbol <%s> is not found\n",symbol);
		break;
	case SSE_STATIC_INFO_FOUND_SYMBOL_BUT_NO_VALUE:
		printf("symbol <%s> is found in map, but no value is available\n",symbol);
		break;
	default:
		break;
	}
	fflush(stdout);

}

void sse_report::close()
{
	if (m_p_quote == NULL)
	{
		return;
	}

	m_p_quote->stop_sse();
	m_p_quote->close_sse();
#ifdef WINDOWS
	func_destroy_efh_sse_lev2_api func_destroy = (func_destroy_efh_sse_lev2_api)GetProcAddress(m_h_core, DESTROY_EFH_SSE_LEV2_API_FUNCTION);
#else
	func_destroy_efh_sse_lev2_api func_destroy = (func_destroy_efh_sse_lev2_api)dlsym(m_h_core, DESTROY_EFH_SSE_LEV2_API_FUNCTION);
#endif
	if (func_destroy == NULL)
	{
		return;
	}

	if (m_b_report_quit)
	{
		report_efh_sse_lev2_snap();
		report_efh_sse_lev2_idx();
		report_efh_sse_lev2_option();
		report_efh_sse_lev2_exe();
		report_efh_sse_lev2_order();
		report_efh_sse_lev2_tree();
		report_efh_sse_lev2_bond_snap();
		report_efh_sse_lev2_bond_tick();
		report_efh_sse_lev2_tick_merge();
		report_efh_sse_lev2_etf();
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

	if (m_fp_option)
	{
		fclose(m_fp_option);
		m_fp_option = NULL;
	}

	if (m_fp_exe)
	{
		fclose(m_fp_exe);
		m_fp_exe = NULL;
	}

	if (m_fp_order)
	{
		fclose(m_fp_order);
		m_fp_order = NULL;
	}

	if (m_fp_tick)
	{
		fclose(m_fp_tick);
		m_fp_tick = NULL;
	}

	if(m_fp_tree)
	{
		fclose(m_fp_tree);
		m_fp_tree = NULL;
	}

	if (m_fp_bond)
	{
		fclose(m_fp_bond);
		m_fp_bond = NULL;
	}

	if(m_fp_bond_tick)
	{
		fclose(m_fp_bond_tick);
		m_fp_bond_tick = NULL;
	}

	if(m_fp_tick_merge)
	{
		fclose(m_fp_tick_merge);
		m_fp_tick_merge = NULL;
	}
	if(m_fp_etf)
	{
		fclose(m_fp_etf);
		m_fp_etf = NULL;
	}

#ifdef TEST_SHENGLI_CODE_CONVERSION
	if ( m_fp_it_code_lev2 )
	{
		fclose( m_fp_it_code_lev2 );
		m_fp_it_code_lev2 = NULL;
	}

	if ( m_fp_it_code_option )
	{
		fclose( m_fp_it_code_option );
		m_fp_it_code_option = NULL;
	}

	if ( m_fp_it_code_tree )
	{
		fclose( m_fp_it_code_tree );
		m_fp_it_code_tree = NULL;
	}

	if ( m_fp_it_code_bond )
	{
		fclose( m_fp_it_code_bond );
		m_fp_it_code_bond = NULL;
	}

	if ( m_fp_it_code_bond_tick )
	{
		fclose( m_fp_it_code_bond_tick );
		m_fp_it_code_bond_tick = NULL;
	}
#endif

	func_destroy(m_p_quote);
#ifdef WINDOWS
	FreeLibrary(m_h_core);
#else
	dlclose(m_h_core);
#endif
}

void sse_report::on_report_efh_sse_lev2_idx(sse_hpf_idx* p_index)
{
	if (!m_b_report_quit)
	{
		if (m_fp_idx == NULL)
		{
			time_t now = time(NULL);
			tm* ltm = localtime(&now);


			char	str_full_name[1024];
			memset(str_full_name, 0, sizeof(str_full_name));
			sprintf(str_full_name, "%04d%02d%02d_sse_idx.dat", ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday);

			m_fp_idx = fopen(str_full_name, "wb+");
			if (m_fp_idx == NULL)
			{
				return;
			}
		}

		int ret = fwrite((char*)p_index, 1, sizeof(sse_hpf_idx), m_fp_idx);
		fflush(m_fp_idx);
		if (ret <= 0)
		{
			printf("write sze_idx.csv error!\n");
		}

		++m_ll_idx_count;
		if (m_i_idx_num != 0)
		{
			m_i_lost_idx_count += p_index->m_head.m_sequence - m_i_idx_num - 1;
			if (p_index->m_head.m_sequence <= m_i_idx_num)
			{
				m_b_idx_rollback_flag = true;
				printf("SSE idx out of seq, last seq[%u], cur seq[%u].\n", m_i_idx_num, p_index->m_head.m_sequence);
			}
		}

		m_i_idx_num = p_index->m_head.m_sequence;

		return;
	}

	if (!is_vaild_symbol((char*)(p_index->m_symbol)))
	{
		return;
	}

	timespec					ts;
	clock_gettime(CLOCK_REALTIME, &ts);

	int node = m_ll_idx_count % QT_SSE_QUOTE_COUNT;
	memcpy(m_idx[node].m_symbol, p_index->m_symbol, 9);
	m_idx[node].m_local_time        = (ts.tv_sec) * 1000000000 + ts.tv_nsec;
	m_idx[node].m_quote_update_time = p_index->m_quote_update_time;
	m_idx[node].m_pre_close_price   = p_index->m_pre_close_price;
	m_idx[node].m_open_price        = p_index->m_open_price;
	m_idx[node].m_day_high_price    = p_index->m_day_high_price;
	m_idx[node].m_day_low_price     = p_index->m_day_low_price;
	m_idx[node].m_last_price        = p_index->m_last_price;
	m_idx[node].m_total_quantity    = p_index->m_total_quantity;

	++m_ll_idx_count;
}

void sse_report::on_report_efh_sse_lev2_snap(sse_hpf_lev2* p_snap)
{
#ifdef TEST_SHENGLI_CODE_CONVERSION
	if (m_fp_it_code_lev2 == NULL)
	{
		time_t now = time(NULL);
		tm* ltm = localtime(&now);

		char	str_full_name[1024];
		memset(str_full_name, 0, sizeof(str_full_name));
		sprintf(str_full_name, "%04d%02d%02d_sse_lev2_it.csv", ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday);

		m_fp_it_code_lev2 = fopen(str_full_name, "wb+");
		if (m_fp_it_code_lev2 == NULL)
		{
			return;
		}
	}
	fprintf( m_fp_it_code_lev2 , "%d,%s,%d,%s\n"
		, p_snap->m_instrument_status
		, get_sse_src_instrument_status( p_snap->m_instrument_status ).c_str()
		, p_snap->m_trading_status
		, get_sse_src_trading_phase_code( p_snap->m_trading_status ).c_str() );
#endif
	if (!m_b_report_quit)
	{
		if (m_fp_lev2 == NULL)
		{
			time_t now = time(NULL);
			tm* ltm = localtime(&now);

			char	str_full_name[1024];
			memset(str_full_name, 0, sizeof(str_full_name));
			sprintf(str_full_name, "%04d%02d%02d_sse_snap.dat", ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday);

			m_fp_lev2 = fopen(str_full_name, "wb+");
			if (m_fp_lev2 == NULL)
			{
				return;
			}
		}

		int ret = fwrite((char*)p_snap, 1, sizeof(sse_hpf_lev2), m_fp_lev2);
		fflush(m_fp_lev2);
		if (ret <= 0)
		{
			printf("write sze_snap.dat error!\n");
		}

		++m_ll_snap_count;
		if (m_i_snap_num != 0)
		{
			m_i_lost_snap_count += p_snap->m_head.m_sequence - m_i_snap_num - 1;
			if (p_snap->m_head.m_sequence <= m_i_snap_num)
			{
				m_b_snap_rollback_flag = true;
				printf("SSE snap out of seq, last seq[%u], cur seq[%u].\n", m_i_snap_num, p_snap->m_head.m_sequence);
			}
		}

		m_i_snap_num = p_snap->m_head.m_sequence;

		return;
	}

	if (!is_vaild_symbol((char*)(p_snap->m_symbol)))
	{
		return;
	}

	timespec			ts;
	clock_gettime(CLOCK_REALTIME, &ts);

	int node = m_ll_snap_count % QT_SSE_QUOTE_COUNT;

	m_snap[node].m_bid_1_price			= p_snap->m_bid_unit[0].m_price;
	m_snap[node].m_bid_1_quantity		= p_snap->m_bid_unit[0].m_quantity;
	m_snap[node].m_bid_10_price			= p_snap->m_bid_unit[9].m_price;
	m_snap[node].m_bid_10_quantity		= p_snap->m_bid_unit[9].m_quantity;
	m_snap[node].m_ask_1_price			= p_snap->m_ask_unit[0].m_price;
	m_snap[node].m_ask_1_quantity		= p_snap->m_ask_unit[0].m_quantity;
	m_snap[node].m_ask_10_price			= p_snap->m_ask_unit[9].m_price;
	m_snap[node].m_ask10_quantity		= p_snap->m_ask_unit[9].m_quantity;

	memcpy(m_snap[node].m_symbol, p_snap->m_symbol, 9);
	m_snap[node].m_quote_update_time 	= p_snap->m_quote_update_time;
	m_snap[node].m_local_time			= ((long long)(ts.tv_sec)) * 1000000000 + ts.tv_nsec;
	m_snap[node].m_last_price			= p_snap->m_last_price;
	m_snap[node].m_total_quantity		= p_snap->m_total_quantity;
	m_snap[node].m_total_value			= p_snap->m_total_value;

	++m_ll_snap_count;
}

void sse_report::on_report_efh_sse_lev2_option(sse_hpf_stock_option* p_option)
{
#ifdef TEST_SHENGLI_CODE_CONVERSION
	if (m_fp_it_code_option == NULL)
	{
		time_t now = time(NULL);
		tm* ltm = localtime(&now);

		char	str_full_name[1024];
		memset(str_full_name, 0, sizeof(str_full_name));
		sprintf(str_full_name, "%04d%02d%02d_sse_option_it.csv", ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday);

		m_fp_it_code_option = fopen(str_full_name, "wb+");
		if (m_fp_it_code_option == NULL)
		{
			return;
		}
	}
	fprintf( m_fp_it_code_option , "%d,%s\n"
		, p_option->m_trading_status
		, get_sse_src_trading_phase_code( p_option->m_trading_status ).c_str() );
#endif
	if (!m_b_report_quit)
	{
		if (m_fp_option == NULL)
		{
			time_t now = time(NULL);
			tm* ltm = localtime(&now);

			char	str_full_name[1024];
			memset(str_full_name, 0, sizeof(str_full_name));
			sprintf(str_full_name, "%04d%02d%02d_sse_option.dat", ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday);

			m_fp_option = fopen(str_full_name, "wb+");
			if (m_fp_option == NULL)
			{
				return;
			}
		}

		int ret = fwrite((char*)p_option, sizeof(sse_hpf_stock_option), 1, m_fp_option);
		fflush(m_fp_option);
		if (ret <= 0)
		{
			printf("write sse_option.dat error!\n");
		}

		++m_ll_option_count;

		if (m_i_option_num != 0)
		{
			m_i_lost_option_count += p_option->m_head.m_sequence - m_i_option_num - 1;
			if (p_option->m_head.m_sequence <= m_i_option_num)
			{
				m_b_option_rollback_flag = true;
				printf("SSE option out of seq, last seq[%u], cur seq[%u].\n", m_i_option_num, p_option->m_head.m_sequence);
			}
		}

		m_i_option_num = p_option->m_head.m_sequence;

		return;
	}

	if (!is_vaild_symbol((char*)(p_option->m_symbol)))
	{
		return;
	}

	timespec				ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	int node = m_ll_option_count % QT_SSE_QUOTE_COUNT;

	m_option[node].m_bid_1_price	= p_option->m_bid_unit[0].m_price;
	m_option[node].m_bid_1_quantity	= p_option->m_bid_unit[0].m_quantity;
	m_option[node].m_bid_5_price	= p_option->m_bid_unit[4].m_price;
	m_option[node].m_bid_5_quantity	= p_option->m_bid_unit[4].m_quantity;
	m_option[node].m_ask_1_price	= p_option->m_ask_unit[0].m_price;
	m_option[node].m_ask_1_quantity	= p_option->m_ask_unit[0].m_quantity;
	m_option[node].m_ask_5_price	= p_option->m_ask_unit[4].m_price;
	m_option[node].m_ask_5_quantity	= p_option->m_ask_unit[4].m_quantity;
	m_option[node].m_last_price		= p_option->m_last_price;

	memcpy(m_option[node].m_symbol, p_option->m_symbol, 9);
	m_option[node].m_quote_update_time	= p_option->m_quote_update_time;
	m_option[node].m_local_time			= (ts.tv_sec) * 1000000000 + ts.tv_nsec;
	m_option[node].m_total_value		= p_option->m_total_value;

	++m_ll_option_count;
}

void sse_report::on_report_efh_sse_lev2_tick(int msg_type, sse_hpf_order* p_order, sse_hpf_exe* p_exe)
{
	if (msg_type == SSE_LEV2_EXE_MSG_TYPE)
	{
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
					sprintf(str_full_name, "%04d%02d%02d_sse_exe.dat", ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday);

					m_fp_exe = fopen(str_full_name, "wb+");
					if (m_fp_exe == NULL)
					{
						return;
					}
				}

				int ret = fwrite((char*)p_exe, sizeof(sse_hpf_exe), 1, m_fp_exe);
				fflush(m_fp_exe);
				if (ret <= 0)
				{
					printf("write sse_exe.csv error!\n");
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
					sprintf(str_full_name, "%04d%02d%02d_sse_tick.dat", ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday);

					m_fp_tick = fopen(str_full_name, "wb+");
					if (m_fp_tick == NULL)
					{
						return;
					}
				}

				int ret = fwrite((char*)p_exe, sizeof(sse_hpf_exe), 1, m_fp_tick);
				fflush(m_fp_tick);
				if (ret <= 0)
				{
					printf("write sse_tick.csv error!\n");
				}

			}

			++m_ll_exe_count;

			if (m_i_tick_num != 0)
			{
				m_i_lost_tick_count += p_exe->m_head.m_sequence - m_i_tick_num - 1;
				if (p_exe->m_head.m_sequence <= m_i_tick_num)
				{
					m_b_tick_rollback_flag = true;
					printf("SSE tick out of seq, last seq[%u], cur seq[%u].\n", m_i_tick_num, p_exe->m_head.m_sequence);
				}
			}

			m_i_tick_num = p_exe->m_head.m_sequence;

			return;
		}

		if (!is_vaild_symbol((char*)(p_exe->m_symbol)))
		{
			return;
		}

		timespec			ts;
		clock_gettime(CLOCK_REALTIME, &ts);
		int node = m_ll_exe_count % QT_SSE_QUOTE_COUNT;

		memcpy(m_exe[node].m_symbol, p_exe->m_symbol, 9);
		m_exe[node].m_trade_price    = p_exe->m_trade_price;
		m_exe[node].m_trade_quantity = p_exe->m_trade_quantity;
		m_exe[node].m_local_time     = (ts.tv_sec) * 1000000000 + ts.tv_nsec;
		m_exe[node].m_channel_num    = p_exe->m_channel_num;
		m_exe[node].m_trade_index    = p_exe->m_trade_index;
		m_exe[node].m_trade_time     = p_exe->m_trade_time;
		m_exe[node].m_trade_bs_flag  = p_exe->m_trade_bs_flag;

		++m_ll_exe_count;
	}
	else if (msg_type == SSE_LEV2_ORDER_MSG_TYPE)
	{
		if (!m_b_report_quit)
		{
			if ( m_b_tick_detach )
			{
				if ( m_fp_order == NULL )
				{
					time_t now = time( NULL );
					tm* ltm = localtime( &now );

					char	str_full_name[1024];
					memset( str_full_name , 0 , sizeof( str_full_name ) );
					sprintf( str_full_name , "%04d%02d%02d_sse_order.dat" , ltm->tm_year + 1900 , ltm->tm_mon + 1 , ltm->tm_mday );

					m_fp_order = fopen( str_full_name , "wb+" );
					if ( m_fp_order == NULL )
					{
						return;
					}
				}

				int ret = fwrite( (char*) p_order , sizeof( sse_hpf_order ) , 1 , m_fp_order );
				fflush( m_fp_order );
				if ( ret <= 0 )
				{
					printf( "write sse_order.csv error!\n" );
				}
			}
			else
			{
				if ( m_fp_tick == NULL )
				{
					time_t now = time( NULL );
					tm* ltm = localtime( &now );

					char	str_full_name[1024];
					memset( str_full_name , 0 , sizeof( str_full_name ) );
					sprintf( str_full_name , "%04d%02d%02d_sse_tick.dat" , ltm->tm_year + 1900 , ltm->tm_mon + 1 , ltm->tm_mday );

					m_fp_tick = fopen( str_full_name , "wb+" );
					if ( m_fp_tick == NULL )
					{
						return;
					}
				}

				int ret = fwrite( (char*) p_order , sizeof( sse_hpf_order ) , 1 , m_fp_tick );
				fflush( m_fp_tick );
				if ( ret <= 0 )
				{
					printf( "write sse_tick.csv error!\n" );
				}
			}

			++m_ll_order_count;
			if (m_i_tick_num != 0)
			{
				m_i_lost_tick_count += p_order->m_head.m_sequence - m_i_tick_num - 1;
				if (p_order->m_head.m_sequence <= m_i_tick_num)
				{
					m_b_tick_rollback_flag = true;
					printf("SSE tick out of seq, last seq[%u], cur seq[%u].\n", m_i_tick_num, p_order->m_head.m_sequence);
				}
			}

			m_i_tick_num = p_order->m_head.m_sequence;

			return;
		}

		if (!is_vaild_symbol((char*)(p_order->m_symbol)))
		{
			return;
		}

		timespec			ts;
		clock_gettime(CLOCK_REALTIME, &ts);
		int node = m_ll_order_count % QT_SSE_QUOTE_COUNT;

		memcpy(m_order[node].m_symbol, p_order->m_symbol, 9);
		m_order[node].m_order_price = p_order->m_order_price;
		m_order[node].m_balance     = p_order->m_balance;
		m_order[node].m_local_time  = (ts.tv_sec) * 1000000000 + ts.tv_nsec;
		m_order[node].m_channel_num = p_order->m_channel_num;
		m_order[node].m_order_index = p_order->m_order_index;
		m_order[node].m_order_time  = p_order->m_order_time;
		m_order[node].m_side_flag   = p_order->m_side_flag;

		++m_ll_order_count;
	}

}

void sse_report::on_report_efh_sse_lev2_tree(sse_hpf_tree* p_tree)
{
#ifdef TEST_SHENGLI_CODE_CONVERSION
	if (m_fp_it_code_tree == NULL)
	{
		time_t now = time(NULL);
		tm* ltm = localtime(&now);

		char	str_full_name[1024];
		memset(str_full_name, 0, sizeof(str_full_name));
		sprintf(str_full_name, "%04d%02d%02d_sse_tree_it.csv", ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday);

		m_fp_it_code_tree = fopen(str_full_name, "wb+");
		if (m_fp_it_code_tree == NULL)
		{
			return;
		}
	}
	fprintf( m_fp_it_code_tree , "%d,%s,%d,%s\n"
		, p_tree->m_instrument_status
		, get_sse_src_instrument_status( p_tree->m_instrument_status ).c_str()
		, p_tree->m_trading_status
		, get_sse_src_trading_phase_code( p_tree->m_trading_status ).c_str() );
#endif
	if (!m_b_report_quit)
	{
		if (m_fp_tree == NULL)
		{
			time_t now = time(NULL);
			tm* ltm = localtime(&now);

			char	str_full_name[1024];
			memset(str_full_name, 0, sizeof(str_full_name));
			sprintf(str_full_name, "%04d%02d%02d_sse_tree.dat", ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday);

			m_fp_tree = fopen(str_full_name, "wb+");
			if (m_fp_tree == NULL)
			{
				return;
			}
		}

		int ret = fwrite((char*)p_tree, sizeof(sse_hpf_tree), 1, m_fp_tree);
		fflush(m_fp_tree);
		if (ret <= 0)
		{
			printf("write sse_tree.dat error!\n");
		}

		++m_ll_tree_count;
		if (m_i_tree_num != 0)
		{
			m_i_lost_tree_count += p_tree->m_head.m_sequence - m_i_tree_num - 1;
			if (p_tree->m_head.m_sequence <= m_i_tree_num)
			{
				m_b_tree_rollback_flag = true;
				printf("SSE tick out of seq, last seq[%u], cur seq[%u].\n", m_i_tree_num, p_tree->m_head.m_sequence);
			}
		}

		m_i_tree_num = p_tree->m_head.m_sequence;

		return;
	}

	if (!is_vaild_symbol((char*)(p_tree->m_symbol)))
	{
		return;
	}

	timespec				ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	int node = m_ll_tree_count % QT_SSE_QUOTE_COUNT;

	m_tree[node].m_bid_1_price = p_tree->m_bid_unit[0].m_price;
	m_tree[node].m_bid_1_quantity = p_tree->m_bid_unit[0].m_quantity;
	m_tree[node].m_bid_10_price = p_tree->m_bid_unit[9].m_price;
	m_tree[node].m_bid_10_quantity = p_tree->m_bid_unit[9].m_quantity;
	m_tree[node].m_ask_1_price = p_tree->m_ask_unit[0].m_price;
	m_tree[node].m_ask_1_quantity = p_tree->m_ask_unit[0].m_quantity;
	m_tree[node].m_ask_10_price = p_tree->m_ask_unit[9].m_price;
	m_tree[node].m_ask_10_quantity = p_tree->m_ask_unit[9].m_quantity;

	memcpy(m_tree[node].m_symbol, p_tree->m_symbol, 9);
	m_tree[node].m_i_quote_update_time = p_tree->m_quote_update_time;
	m_tree[node].m_local_time = ((long long)(ts.tv_sec)) * 1000000000 + ts.tv_nsec;
	m_tree[node].m_last_price = p_tree->m_last_price;
	m_tree[node].m_total_quantity = p_tree->m_total_quantity;
	m_tree[node].m_total_value = p_tree->m_total_value;

	++m_ll_tree_count;

}

void sse_report::on_report_efh_sse_lev2_bond_snap(sse_hpf_bond_snap* p_bond)
{
#ifdef TEST_SHENGLI_CODE_CONVERSION
	if (m_fp_it_code_bond == NULL)
	{
		time_t now = time(NULL);
		tm* ltm = localtime(&now);

		char	str_full_name[1024];
		memset(str_full_name, 0, sizeof(str_full_name));
		sprintf(str_full_name, "%04d%02d%02d_sse_bond_it.csv", ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday);

		m_fp_it_code_bond = fopen(str_full_name, "wb+");
		if (m_fp_it_code_bond == NULL)
		{
			return;
		}
	}
	/// 债券行情结构不存在 trading_phase_code ,只能由 instrument_status 推算 原生交易状态值
	fprintf( m_fp_it_code_bond , "%d,%s,%s\n"
		, p_bond->m_instrument_status
		, get_sse_src_instrument_status( p_bond->m_instrument_status ).c_str()
		, get_sse_bond_trading_phase_code_by_instrument_status( p_bond->m_instrument_status ).c_str() );
#endif
	if (!m_b_report_quit)
	{
		if (m_fp_bond == NULL)
		{
			time_t now = time(NULL);
			tm* ltm = localtime(&now);

			char	str_full_name[1024];
			memset(str_full_name, 0, sizeof(str_full_name));
			sprintf(str_full_name, "%04d%02d%02d_sse_bond_snap.dat", ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday);

			m_fp_bond = fopen(str_full_name, "wb+");
			if (m_fp_bond == NULL)
			{
				return;
			}
		}

		int ret = fwrite((char*)p_bond, sizeof(sse_hpf_bond_snap), 1, m_fp_bond);
		fflush(m_fp_bond);
		if (ret <= 0)
		{
			printf("write sse_bond_snap.dat error!\n");
		}

		++m_ll_bond_count;
		if (m_i_bond_num != 0)
		{
			m_i_lost_bond_count += p_bond->m_head.m_sequence - m_i_bond_num - 1;
			if (p_bond->m_head.m_sequence <= m_i_bond_num)
			{
				m_b_bond_rollback_flag = true;
				printf("SSE bond_snap out of seq, last seq[%u], cur seq[%u].\n", m_i_bond_num, p_bond->m_head.m_sequence);
			}
		}

		m_i_bond_num = p_bond->m_head.m_sequence;

		return;
	}

	if (!is_vaild_symbol((char*)(p_bond->m_symbol)))
	{
		return;
	}

	timespec				ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	int node = m_ll_bond_count % QT_SSE_QUOTE_COUNT;

	memset(m_bond_snap[node].m_symbol, 0, 9);
	memcpy(m_bond_snap[node].m_symbol, p_bond->m_symbol, 8);
	m_bond_snap[node].m_seq					= p_bond->m_head.m_sequence;
	m_bond_snap[node].m_quote_update_time	= p_bond->m_quote_update_time;
	m_bond_snap[node].m_pre_close_px		= p_bond->m_head.m_category_id;
	m_bond_snap[node].m_open_px				= p_bond->m_head.m_msg_seq_id;
	m_bond_snap[node].m_day_high			= p_bond->m_day_high_price;
	m_bond_snap[node].m_day_low				= p_bond->m_day_low_price;
	m_bond_snap[node].m_last_px				= p_bond->m_last_price;
	m_bond_snap[node].m_instrument_status	= p_bond->m_instrument_status;
	m_bond_snap[node].m_trade_num			= p_bond->m_total_trade_num;
	m_bond_snap[node].m_trade_volumn		= p_bond->m_total_quantity;
	m_bond_snap[node].m_trade_value			= p_bond->m_total_value;
	m_bond_snap[node].m_total_bid_number	= p_bond->m_total_bid_num;
	m_bond_snap[node].m_total_offer_number	= p_bond->m_total_ask_num;
	m_bond_snap[node].m_bid_price1			= p_bond->m_bid_unit[0].m_price;
	m_bond_snap[node].m_bid_volumn1			= p_bond->m_bid_unit[0].m_quantity;
	m_bond_snap[node].m_bid_price10			= p_bond->m_bid_unit[9].m_price;
	m_bond_snap[node].m_bid_volumn10		= p_bond->m_bid_unit[9].m_quantity;
	m_bond_snap[node].m_ask_price1			= p_bond->m_ask_unit[0].m_price;
	m_bond_snap[node].m_ask_volumn1			= p_bond->m_ask_unit[0].m_quantity;
	m_bond_snap[node].m_ask_price10			= p_bond->m_ask_unit[9].m_price;
	m_bond_snap[node].m_ask_volumn10		= p_bond->m_ask_unit[9].m_quantity;
	m_bond_snap[node].m_local_time = ((long long)(ts.tv_sec)) * 1000000000 + ts.tv_nsec;

	++m_ll_bond_count;
}

void sse_report::on_report_efh_sse_lev2_bond_tick(sse_hpf_bond_tick* p_tick)
{
#ifdef TEST_SHENGLI_CODE_CONVERSION
	if (m_fp_it_code_bond_tick == NULL)
	{
		time_t now = time(NULL);
		tm* ltm = localtime(&now);

		char	str_full_name[1024];
		memset(str_full_name, 0, sizeof(str_full_name));
		sprintf(str_full_name, "%04d%02d%02d_sse_bond_tick_it.csv", ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday);

		m_fp_it_code_bond_tick = fopen(str_full_name, "wb+");
		if (m_fp_it_code_bond_tick == NULL)
		{
			return;
		}
	}
	/// 债券行情结构不存在 trading_phase_code ,只能由 instrument_status 推算 原生交易状态值
	fprintf( m_fp_it_code_bond_tick , "%d,%s,%s\n"
		, p_tick->m_instrument_status
		, get_sse_src_instrument_status( p_tick->m_instrument_status ).c_str()
		, get_sse_bond_trading_phase_code_by_instrument_status( p_tick->m_instrument_status ).c_str() );
#endif
	if (!m_b_report_quit)
	{
		if (m_fp_bond_tick == NULL)
		{
			time_t now = time(NULL);
			tm* ltm = localtime(&now);

			char	str_full_name[1024];
			memset(str_full_name, 0, sizeof(str_full_name));
			sprintf(str_full_name, "%04d%02d%02d_sse_bond_tick.dat", ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday);

			m_fp_bond_tick = fopen(str_full_name, "wb+");
			if (m_fp_bond_tick == NULL)
			{
				return;
			}
		}

		int ret = fwrite((char*)p_tick, sizeof(sse_hpf_bond_tick), 1, m_fp_bond_tick);
		fflush(m_fp_bond_tick);
		if (ret <= 0)
		{
			printf("write sse_bond_tick.dat error!\n");
		}

		++m_ll_bond_tick_count;
		if (m_i_bond_tick_num != 0)
		{
			m_i_lost_bond_tick_count += p_tick->m_head.m_sequence - m_i_bond_tick_num - 1;
			if (p_tick->m_head.m_sequence <= m_i_bond_tick_num)
			{
				m_b_bond_tick_rollback_flag = true;
				printf("SSE bond_tick out of seq, last seq[%u], cur seq[%u].\n", m_i_bond_tick_num, p_tick->m_head.m_sequence);
			}
		}

		m_i_bond_tick_num = p_tick->m_head.m_sequence;

		return;
	}

	if (!is_vaild_symbol((char*)(p_tick->m_symbol)))
	{
		return;
	}

	timespec				ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	int node = m_ll_bond_tick_count % QT_SSE_QUOTE_COUNT;

	m_bond_tick[node].m_seq = p_tick->m_head.m_sequence;
	m_bond_tick[node].m_tick_index = p_tick->m_tick_index;
	m_bond_tick[node].m_channel_id = p_tick->m_channel_num;
	m_bond_tick[node].m_tick_time = p_tick->m_tick_time;
	m_bond_tick[node].m_tick_type = p_tick->m_tick_type;
	m_bond_tick[node].m_buy_order_no = p_tick->m_buy_num;
	memset(m_bond_tick[node].m_symbol, 0, 9);
	memcpy(m_bond_tick[node].m_symbol, p_tick->m_symbol, 8);
	m_bond_tick[node].m_sell_order_no = p_tick->m_sell_num;
	m_bond_tick[node].m_price = p_tick->m_price;
	m_bond_tick[node].m_qty = p_tick->m_quantity;
	m_bond_tick[node].m_trade_money = p_tick->m_trade_value;
	m_bond_tick[node].m_side_flag = p_tick->m_side_flag;
	m_bond_tick[node].m_instrument_status = p_tick->m_instrument_status;
	m_bond_tick[node].m_local_time = ((long long)(ts.tv_sec)) * 1000000000 + ts.tv_nsec;

	++m_ll_bond_tick_count;
}

void sse_report::on_report_efh_sse_lev2_tick_merge(sse_hpf_tick_merge* p_tick)
{
	if (!m_b_report_quit)
	{
		if (m_fp_tick_merge == NULL)
		{
			time_t now = time(NULL);
			tm* ltm = localtime(&now);

			char	str_full_name[1024];
			memset(str_full_name, 0, sizeof(str_full_name));
			sprintf(str_full_name, "%04d%02d%02d_sse_tick_merge.dat", ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday);

			m_fp_tick_merge = fopen(str_full_name, "wb+");
			if (m_fp_tick_merge == NULL)
			{
				return;
			}
		}

		int ret = fwrite((char*)p_tick, sizeof(sse_hpf_tick_merge), 1, m_fp_tick_merge);
		fflush(m_fp_tick_merge);
		if (ret <= 0)
		{
			printf("write sse_tick_merge.dat error!\n");
		}

		++m_ll_tick_merge_count;
		if (m_i_tick_merge_num != 0)
		{
			m_i_lost_tick_merge_count += p_tick->m_head.m_sequence - m_i_tick_merge_num - 1;
			if (p_tick->m_head.m_sequence <= m_i_tick_merge_num)
			{
				m_b_tick_merge_rollback_flag = true;
				printf("SSE tick_merge out of seq, last seq[%u], cur seq[%u].\n", m_i_tick_merge_num, p_tick->m_head.m_sequence);
			}
		}

		m_i_tick_merge_num = p_tick->m_head.m_sequence;

		return;
	}

	if (!is_vaild_symbol((char*)(p_tick->m_symbol)))
	{
		return;
	}

	timespec				ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	int node = m_ll_tick_merge_count % QT_SSE_QUOTE_COUNT;

	m_tick_merge[node].m_seq = p_tick->m_head.m_sequence;
	m_tick_merge[node].m_tick_index = p_tick->m_tick_index;
	m_tick_merge[node].m_channel_id = p_tick->m_channel_num;
	m_tick_merge[node].m_tick_time = p_tick->m_tick_time;
	m_tick_merge[node].m_tick_type = p_tick->m_tick_type;
	m_tick_merge[node].m_buy_order_no = p_tick->m_buy_num;
	memset(m_tick_merge[node].m_symbol, 0, 9);
	memcpy(m_tick_merge[node].m_symbol, p_tick->m_symbol, 8);
	m_tick_merge[node].m_sell_order_no = p_tick->m_sell_num;
	m_tick_merge[node].m_price = p_tick->m_price;
	m_tick_merge[node].m_qty = p_tick->m_quantity;
	m_tick_merge[node].m_trade_money = p_tick->m_trade_value;
	m_tick_merge[node].m_side_flag = p_tick->m_side_flag;
	m_tick_merge[node].m_instrument_status = p_tick->m_instrument_status;
	m_tick_merge[node].m_local_time = ((long long)(ts.tv_sec)) * 1000000000 + ts.tv_nsec;

	++m_ll_tick_merge_count;
}

void sse_report::on_report_efh_sse_lev2_etf(sse_hpf_etf* p_tick)
{
	if (!m_b_report_quit)
	{
		if (m_fp_etf == NULL)
		{
			time_t now = time(NULL);
			tm* ltm = localtime(&now);

			char	str_full_name[1024];
			memset(str_full_name, 0, sizeof(str_full_name));
			sprintf(str_full_name, "%04d%02d%02d_sse_etf.dat", ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday);

			m_fp_etf = fopen(str_full_name, "wb+");
			if (m_fp_etf == NULL)
			{
				return;
			}
		}

		int ret = fwrite((char*)p_tick, sizeof(sse_hpf_etf), 1, m_fp_etf);
		fflush(m_fp_etf);
		if (ret <= 0)
		{
			printf("write sse_etf.dat error!\n");
		}

		++m_ll_etf_count;
		if (m_i_etf_num != 0)
		{
			m_i_lost_etf_count += p_tick->m_head.m_sequence - m_i_etf_num - 1;
			if (p_tick->m_head.m_sequence <= m_i_etf_num)
			{
				m_b_etf_rollback_flag = true;
				printf("SSE etf out of seq, last seq[%u], cur seq[%u].\n", m_i_etf_num, p_tick->m_head.m_sequence);
			}
		}

		m_i_etf_num = p_tick->m_head.m_sequence;

		return;
	}

	if (!is_vaild_symbol((char*)(p_tick->m_symbol)))
	{
		return;
	}

	timespec				ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	int node = m_ll_etf_count % QT_SSE_QUOTE_COUNT;

	m_etf[node].quote = *p_tick;
	m_etf[node].m_local_time = ((long long)(ts.tv_sec)) * 1000000000 + ts.tv_nsec;

	++m_ll_etf_count;
}

void sse_report::efh_sse_lev2_debug(const char* msg, int len)
{
	printf("[DEBUG] %s\n", msg);
}

void sse_report::efh_sse_lev2_error(const char* msg, int len)
{
	printf("[ERROR] %s\n", msg);
}

void sse_report::efh_sse_lev2_info(const char* msg, int len)
{
	printf("[INFO] %s\n", msg);
}


string sse_report::format_str(const char* pFormat, ...)
{
	va_list args;
	va_start(args, pFormat);
	char buffer[40960];
	vsnprintf(buffer, 40960, pFormat, args);
	va_end(args);
	return string(buffer);
}

void sse_report::report_efh_sse_lev2_idx()
{
	if (m_fp_idx == NULL)
	{
		time_t now = time(NULL);
		tm* ltm = localtime(&now);

		char	str_full_name[1024];
		memset(str_full_name, 0, sizeof(str_full_name));
		sprintf(str_full_name, "%04d%02d%02d_sse_idx.csv", ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday);

		m_fp_idx = fopen(str_full_name, "wt+");
		if (m_fp_idx == NULL)
		{
			return;
		}
	}

	if (m_ll_idx_count > 0)
	{
		for (int i = 0; i < QT_SSE_QUOTE_COUNT; i++)
		{
			if (m_idx[i].m_local_time == 0)
			{
				return;
			}

			char ch_buffer[1024];
			memset(ch_buffer, 0, sizeof(ch_buffer));

			sprintf(ch_buffer, "%s, 0, %u, %lld, %lld, %u, %u, %u, %u, %u\n",
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
				printf("write sse_idx.csv error!\t msg : %s\n", ch_buffer);
			}
		}
	}
}

void sse_report::report_efh_sse_lev2_snap()
{
	if (m_fp_lev2 == NULL)
	{
		time_t now = time(NULL);
		tm* ltm = localtime(&now);

		char	str_full_name[1024];
		memset(str_full_name, 0, sizeof(str_full_name));
		sprintf(str_full_name, "%04d%02d%02d_sse_snap.csv", ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday);

		m_fp_lev2 = fopen(str_full_name, "wt+");
		if (m_fp_lev2 == NULL)
		{
			return;
		}
	}

	if (m_ll_snap_count > 0)
	{
		for (int i = 0; i < QT_SSE_QUOTE_COUNT; i++)
		{
			if (m_snap[i].m_local_time == 0)
			{
				return;
			}

			char ch_buffer[1024];
			memset(ch_buffer, 0, sizeof(ch_buffer));

			sprintf(ch_buffer, "%s, 0, %u, %lld, %u, %lld, %lld, %u, %lld, %u, %lld, %u, %lld, %u, %lld\n",
				m_snap[i].m_symbol,
				m_snap[i].m_quote_update_time,
				m_snap[i].m_local_time,
				m_snap[i].m_last_price,
				m_snap[i].m_total_quantity,
				m_snap[i].m_total_value,
				m_snap[i].m_bid_1_price,
				m_snap[i].m_bid_1_quantity,
				m_snap[i].m_ask_1_price,
				m_snap[i].m_ask_1_quantity,
				m_snap[i].m_bid_10_price,
				m_snap[i].m_bid_10_quantity,
				m_snap[i].m_ask_10_price,
				m_snap[i].m_ask10_quantity);

			int ret = fwrite(ch_buffer, strlen(ch_buffer), 1, m_fp_lev2);
			fflush(m_fp_lev2);
			if (ret <= 0)
			{
				printf("write sse_snap.csv error!\t msg : %s\n", ch_buffer);
			}
		}
	}
}

void sse_report::report_efh_sse_lev2_option()
{
	if (m_fp_option == NULL)
	{
		time_t now = time(NULL);
		tm* ltm = localtime(&now);

		char	str_full_name[1024];
		memset(str_full_name, 0, sizeof(str_full_name));
		sprintf(str_full_name, "%04d%02d%02d_sse_option.csv", ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday);

		m_fp_option = fopen(str_full_name, "wt+");
		if (m_fp_option == NULL)
		{
			return;
		}
	}
	if (m_ll_option_count > 0)
	{
		for (int i = 0; i < QT_SSE_QUOTE_COUNT; i++)
		{
			if (m_option[i].m_local_time == 0)
			{
				return;
			}

			char ch_buffer[1024];
			memset(ch_buffer, 0, sizeof(ch_buffer));
			sprintf(ch_buffer, "%s, 0, %u, %lld, %u, %lld, %u, %lld, %u, %lld, %u, %lld, %u, %lld\n",
				m_option[i].m_symbol,
				m_option[i].m_quote_update_time,
				m_option[i].m_local_time,
				m_option[i].m_last_price,
				m_option[i].m_total_value,
				m_option[i].m_bid_1_price,
				m_option[i].m_bid_1_quantity,
				m_option[i].m_ask_1_price,
				m_option[i].m_ask_1_quantity,
				m_option[i].m_bid_5_price,
				m_option[i].m_bid_5_quantity,
				m_option[i].m_ask_5_price,
				m_option[i].m_ask_5_quantity);

			int ret = fwrite(ch_buffer, 1, strlen(ch_buffer), m_fp_option);
			fflush(m_fp_option);
			if (ret <= 0)
			{
				printf("write sse_option.csv error!\t msg : %s\n", ch_buffer);
			}
		}
	}
}

void sse_report::report_efh_sse_lev2_exe()
{
	if (m_fp_order == NULL)
	{
		time_t now = time(NULL);
		tm* ltm = localtime(&now);

		char	str_full_name[1024];
		memset(str_full_name, 0, sizeof(str_full_name));
		sprintf(str_full_name, "%04d%02d%02d_sse_exe.csv", ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday);

		m_fp_exe = fopen(str_full_name, "wt+");
		if (m_fp_exe == NULL)
		{
			return;
		}
	}

	if (m_ll_exe_count > 0)
	{
		for (int i = 0; i < QT_SSE_QUOTE_COUNT; i++)
		{
			if (m_exe[i].m_local_time == 0)
			{
				return;
			}

			char ch_buffer[1024];
			memset(ch_buffer, 0, sizeof(ch_buffer));

			sprintf(ch_buffer, "%s, %d, %lld, %u, %lld, %u, %lld, %d\n",
				m_exe[i].m_symbol,
				m_exe[i].m_channel_num,
				m_exe[i].m_trade_index,
				m_exe[i].m_trade_time,
				m_exe[i].m_local_time,
				m_exe[i].m_trade_price,
				m_exe[i].m_trade_quantity,
				m_exe[i].m_trade_bs_flag);

			int ret = fwrite(ch_buffer, strlen(ch_buffer), 1, m_fp_exe);
			fflush(m_fp_exe);
			if (ret <= 0)
			{
				printf("write sse_exe.csv error!\t msg : %s\n", ch_buffer);
			}
		}
	}
}


void sse_report::report_efh_sse_lev2_order()
{
	if (m_fp_order == NULL)
	{
		time_t now = time(NULL);
		tm* ltm = localtime(&now);

		char	str_full_name[1024];
		memset(str_full_name, 0, sizeof(str_full_name));
		sprintf(str_full_name, "%04d%02d%02d_sse_order.csv", ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday);

		m_fp_order = fopen(str_full_name, "wt+");
		if (m_fp_order == NULL)
		{
			return;
		}
	}

	if (m_ll_order_count > 0)
	{
		for (int i = 0; i < QT_SSE_QUOTE_COUNT; i++)
		{
			if (m_order[i].m_local_time == 0)
			{
				return;
			}

			char ch_buffer[1024];
			memset(ch_buffer, 0, sizeof(ch_buffer));

			sprintf(ch_buffer, "%s, %d, %lld, %u, %lld, %u, %lld, %d\n",
				m_order[i].m_symbol,
				m_order[i].m_channel_num,
				m_order[i].m_order_index,
				m_order[i].m_order_time,
				m_order[i].m_local_time,
				m_order[i].m_order_price,
				m_order[i].m_balance,
				m_order[i].m_side_flag);

			int ret = fwrite(ch_buffer, strlen(ch_buffer), 1, m_fp_order);
			fflush(m_fp_order);
			if (ret <= 0)
			{
				printf("write sse_order.csv error!\t msg : %s\n", ch_buffer);
			}
		}
	}

}

void sse_report::report_efh_sse_lev2_tree()
{
	if (m_fp_tree == NULL)
	{
		time_t now = time(NULL);
		tm* ltm = localtime(&now);

		char	str_full_name[1024];
		memset(str_full_name, 0, sizeof(str_full_name));
		sprintf(str_full_name, "%04d%02d%02d_sse_tree.csv", ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday);

		m_fp_tree = fopen(str_full_name, "wt+");
		if (m_fp_tree == NULL)
		{
			return;
		}
	}

	if (m_ll_tree_count > 0)
	{
		for (int i = 0; i < QT_SSE_QUOTE_COUNT; i++)
		{
			if (m_tree[i].m_local_time == 0)
			{
				return;
			}

			char ch_buffer[1024];
			memset(ch_buffer, 0, sizeof(ch_buffer));

			sprintf(ch_buffer, "%s, 0, %u, %lld, %u, %lld, %lld, %u, %lld, %u, %lld, %u, %lld, %u, %lld\n",
				m_tree[i].m_symbol,
				m_tree[i].m_i_quote_update_time,
				m_tree[i].m_local_time,
				m_tree[i].m_last_price,
				m_tree[i].m_total_quantity,
				m_tree[i].m_total_value,
				m_tree[i].m_bid_1_price,
				m_tree[i].m_bid_1_quantity,
				m_tree[i].m_ask_1_price,
				m_tree[i].m_ask_1_quantity,
				m_tree[i].m_bid_10_price,
				m_tree[i].m_bid_10_quantity,
				m_tree[i].m_ask_10_price,
				m_tree[i].m_ask_10_quantity);

			int ret = fwrite(ch_buffer, strlen(ch_buffer), 1, m_fp_tree);
			fflush(m_fp_tree);
			if (ret <= 0)
			{
				printf("write sse_tree.csv error!\t msg : %s\n", ch_buffer);
			}
		}
	}
}

void sse_report::report_efh_sse_lev2_bond_snap()
{
	if (m_fp_bond == NULL)
	{
		time_t now = time(NULL);
		tm* ltm = localtime(&now);

		char	str_full_name[1024];
		memset(str_full_name, 0, sizeof(str_full_name));
		sprintf(str_full_name, "%04d%02d%02d_sse_bond_snap.csv", ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday);

		m_fp_bond = fopen(str_full_name, "wt+");
		if (m_fp_bond == NULL)
		{
			return;
		}
	}

	if (m_ll_bond_count > 0)
	{
		for (int i = 0; i < QT_SSE_QUOTE_COUNT; i++)
		{
			if (m_bond_snap[i].m_local_time == 0)
			{
				break;
			}

			char ch_buffer[1024];
			memset(ch_buffer, 0, sizeof(ch_buffer));
			sprintf(ch_buffer, "%u, %llu, %u, %s, %d, %u, %u, %u, %u, %d, %u, %llu, %llu, %u, %u, %u, %llu, %u, %llu, %u, %llu, %u, %llu\n",
							   m_bond_snap[i].m_seq,
							   m_bond_snap[i].m_local_time,
							   m_bond_snap[i].m_quote_update_time,
							   m_bond_snap[i].m_symbol,
							   m_bond_snap[i].m_pre_close_px,
							   m_bond_snap[i].m_open_px,
							   m_bond_snap[i].m_day_high,
							   m_bond_snap[i].m_day_low,
							   m_bond_snap[i].m_last_px,
							   m_bond_snap[i].m_instrument_status,
							   m_bond_snap[i].m_trade_num,
							   m_bond_snap[i].m_trade_volumn,
							   m_bond_snap[i].m_trade_value,
							   m_bond_snap[i].m_total_bid_number,
							   m_bond_snap[i].m_total_offer_number,
							   m_bond_snap[i].m_bid_price1,
							   m_bond_snap[i].m_bid_volumn1,
							   m_bond_snap[i].m_bid_price10,
							   m_bond_snap[i].m_bid_volumn10,
							   m_bond_snap[i].m_ask_price1,
							   m_bond_snap[i].m_ask_volumn1,
							   m_bond_snap[i].m_ask_price10,
							   m_bond_snap[i].m_ask_volumn10);

			int ret = fwrite(ch_buffer, strlen(ch_buffer), 1, m_fp_bond);
			fflush(m_fp_bond);
			if (ret <= 0)
			{
				printf("write sse_bond_snap.csv error!\t msg : %s\n", ch_buffer);
			}
		}
	}
}

void sse_report::report_efh_sse_lev2_bond_tick()
{
	if (m_fp_bond_tick == NULL)
	{
		time_t now = time(NULL);
		tm* ltm = localtime(&now);

		char	str_full_name[1024];
		memset(str_full_name, 0, sizeof(str_full_name));
		sprintf(str_full_name, "%04d%02d%02d_sse_bond_tick.csv", ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday);

		m_fp_bond_tick = fopen(str_full_name, "wt+");
		if (m_fp_bond_tick == NULL)
		{
			return;
		}
	}

	if (m_ll_bond_tick_count > 0)
	{
		for (int i = 0; i < QT_SSE_QUOTE_COUNT; i++)
		{
			if (m_bond_tick[i].m_local_time == 0)
			{
				break;
			}

			char ch_buffer[1024];
			memset(ch_buffer, 0, sizeof(ch_buffer));
			sprintf(ch_buffer, "%u, %llu, %u, %u, %s, %u, %d, %llu, %llu, %u, %llu, %llu, %u, %u\n",
				m_bond_tick[i].m_seq,
				m_bond_tick[i].m_local_time,
				m_bond_tick[i].m_tick_index,
				m_bond_tick[i].m_channel_id,
				m_bond_tick[i].m_symbol,
				m_bond_tick[i].m_tick_time,
				m_bond_tick[i].m_tick_type,
				m_bond_tick[i].m_buy_order_no,
				m_bond_tick[i].m_sell_order_no,
				m_bond_tick[i].m_price,
				m_bond_tick[i].m_qty,
				m_bond_tick[i].m_trade_money,
				m_bond_tick[i].m_side_flag,
				m_bond_tick[i].m_instrument_status);

			int ret = fwrite(ch_buffer, strlen(ch_buffer), 1, m_fp_bond_tick);
			fflush(m_fp_bond_tick);
			if (ret <= 0)
			{
				printf("write sse_bond_tick.csv error!\t msg : %s\n", ch_buffer);
			}
		}
	}
}

void sse_report::report_efh_sse_lev2_tick_merge()
{
	if (m_fp_tick_merge == NULL)
	{
		time_t now = time(NULL);
		tm* ltm = localtime(&now);

		char	str_full_name[1024];
		memset(str_full_name, 0, sizeof(str_full_name));
		sprintf(str_full_name, "%04d%02d%02d_sse_tick_merge.csv", ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday);

		m_fp_tick_merge = fopen(str_full_name, "wt+");
		if (m_fp_tick_merge == NULL)
		{
			return;
		}
	}

	if (m_ll_tick_merge_count > 0)
	{
		for (int i = 0; i < QT_SSE_QUOTE_COUNT; i++)
		{
			if (m_tick_merge[i].m_local_time == 0)
			{
				break;
			}

			char ch_buffer[1024];
			memset(ch_buffer, 0, sizeof(ch_buffer));
			sprintf(ch_buffer, "%u, %llu, %u, %u, %s, %u, %d, %llu, %llu, %u, %llu, %llu, %u, %u\n",
				m_tick_merge[i].m_seq,
				m_tick_merge[i].m_local_time,
				m_tick_merge[i].m_tick_index,
				m_tick_merge[i].m_channel_id,
				m_tick_merge[i].m_symbol,
				m_tick_merge[i].m_tick_time,
				m_tick_merge[i].m_tick_type,
				m_tick_merge[i].m_buy_order_no,
				m_tick_merge[i].m_sell_order_no,
				m_tick_merge[i].m_price,
				m_tick_merge[i].m_qty,
				m_tick_merge[i].m_trade_money,
				m_tick_merge[i].m_side_flag,
				m_tick_merge[i].m_instrument_status);

			int ret = fwrite(ch_buffer, strlen(ch_buffer), 1, m_fp_tick_merge);
			fflush(m_fp_tick_merge);
			if (ret <= 0)
			{
				printf("write sse_bond_tick.csv error!\t msg : %s\n", ch_buffer);
			}
		}
	}
}

void sse_report::report_efh_sse_lev2_etf()
{
	if (m_fp_etf == NULL)
	{
		time_t now = time(NULL);
		tm* ltm = localtime(&now);

		char	str_full_name[1024];
		memset(str_full_name, 0, sizeof(str_full_name));
		sprintf(str_full_name, "%04d%02d%02d_sse_etf.csv", ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday);

		m_fp_etf = fopen(str_full_name, "wt+");
		if (m_fp_etf == NULL)
		{
			return;
		}
	}

	if (m_ll_etf_count > 0)
	{
		for (int i = 0; i < QT_SSE_QUOTE_COUNT; i++)
		{
			if (m_etf[i].m_local_time == 0)
			{
				break;
			}

			char ch_buffer[1024];
			memset(ch_buffer, 0, sizeof(ch_buffer));

			sprintf(ch_buffer,"%u,%llu,%u,%s,%u,%u,%u,%llu,%llu,%u,%llu,%llu\n"
				,m_etf[i].quote.m_head.m_sequence
				,m_etf[i].m_local_time
				,m_etf[i].quote.m_quote_update_time
				,m_etf[i].quote.m_symbol
				,m_etf[i].quote.m_security_type
				,m_etf[i].quote.m_sub_security_type
				,m_etf[i].quote.m_buy_number
				,m_etf[i].quote.m_buy_amount
				,m_etf[i].quote.m_buy_money
				,m_etf[i].quote.m_sell_number
				,m_etf[i].quote.m_sell_amount
				,m_etf[i].quote.m_sell_money
			);




			int ret = fwrite(ch_buffer, strlen(ch_buffer), 1, m_fp_etf);
			fflush(m_fp_etf);
			if (ret <= 0)
			{
				printf("write sse_etf.csv error!\t msg : %s\n", ch_buffer);
			}
		}
	}
}


string sse_report::get_sse_src_trading_phase_code(char ch_sl_trading_phase_code)
{
	char	ch_buf[1024];
	memset(ch_buf, 0, sizeof(ch_buf));
	char ch_first = ' ';
	switch (ch_sl_trading_phase_code & 0xF0)
	{
	case 0x00:
		ch_first = 'S';
		break;
	case 0x10:
		ch_first = 'C';
		break;
	case 0x20:
		ch_first = 'T';
		break;
	case 0x30:
		ch_first = 'E';
		break;
	case 0x40:
		ch_first = 'P';
		break;
	case 0x50:
		ch_first = 'M';
		break;
	case 0x60:
		ch_first = 'N';
		break;
	case 0x70:
		ch_first = 'U';
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

	if ((ch_sl_trading_phase_code & 0x04) == 0x04)
	{
		ch_buf[2] = '1';
	}
	else if ((ch_sl_trading_phase_code & 0x04) == 0x00)
	{
		ch_buf[2] = '0';
	}

	if ((ch_sl_trading_phase_code & 0x02) == 0x02)
	{
		ch_buf[3] = '1';
	}
	else if ((ch_sl_trading_phase_code & 0x02) == 0x00)
	{
		ch_buf[3] = '0';
	}

	string str_buf = ch_buf;
	return str_buf;
}

string sse_report::get_sse_src_instrument_status(char ch_sl_instrument_status)
{
	char	ch_buf[1024];
	memset(ch_buf, 0, sizeof(ch_buf));
	switch (ch_sl_instrument_status)
	{
	case 0:
		strcpy(ch_buf, " ");
		break;
	case 1:
		strcpy(ch_buf, "START");
		break;
	case 2:
		strcpy(ch_buf, "OCALL");
		break;
	case 3:
		strcpy(ch_buf, "TRADE");
		break;
	case 4:
		strcpy(ch_buf, "SUSP");
		break;
	case 5:
		strcpy(ch_buf, "CCALL");
		break;
	case 6:
		strcpy(ch_buf, "CLOSE");
		break;
	case 7:
		strcpy(ch_buf, "ENDTR");
		break;
	case 8:
		strcpy(ch_buf,"ADD");
		break;
	default:
		strcpy(ch_buf," ");
		break;
	}

	string str_buf = ch_buf;
	return str_buf;
}


string sse_report::get_sse_bond_trading_phase_code_by_instrument_status(char ch_sl_instrument_status)
{
	char	ch_buf[9];
	memset(ch_buf, 0, sizeof(ch_buf));
	switch (ch_sl_instrument_status)
	{
	case 0:
		strcpy(ch_buf, "        ");
		break;
	case 1:
		strcpy(ch_buf, "S       ");
		break;
	case 2:
		strcpy(ch_buf, "C       ");
		break;
	case 3:
		strcpy(ch_buf, "T       ");
		break;
	case 4:
		strcpy(ch_buf, "P       ");
		break;
	case 6:
		strcpy(ch_buf, "E       ");
		break;
	case 7:
		strcpy(ch_buf, "E       ");
		break;
	case 8:
		strcpy(ch_buf, "        ");
		break;
	default:
		strcpy(ch_buf, "        ");
		break;
	}

	string str_buf = ch_buf;
	return str_buf;
}
