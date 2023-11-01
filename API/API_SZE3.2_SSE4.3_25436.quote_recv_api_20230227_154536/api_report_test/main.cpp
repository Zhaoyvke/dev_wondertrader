#include <stdio.h>
#include <string.h>
#include "sze_report.h"
#include "sse_report.h"
#include "profile.h"
#include <vector>
#include <string>

using namespace std;

#define  INI_FILE_PATH  "./efh_recv_conf.ini"
#define	 SZE_QUOTE_PARAM_NUM	(9)
#define	 SSE_QUOTE_PARAM_NUM	(10)

TIniFile ini;

bool config_sock_udp_param(socket_udp_param& quote_param, const char* section)
{
	int enable = ini.ReadInt(section, "enable", 0);
	if (enable == 1)
	{
		char msg[64];
		memset(msg, 0, sizeof(msg));

		quote_param.m_i_cpu_id = ini.ReadInt(section, "cpu_id", 0);

		int len = ini.ReadString(section, "multicast_ip", "", msg, IP_LEN);
		memset(quote_param.m_ch_src_ip, 0, sizeof(quote_param.m_ch_src_ip));
		memcpy(quote_param.m_ch_src_ip, msg, len);

		quote_param.m_i_src_port = ini.ReadInt(section, "multicast_port", 0);

		len = ini.ReadString(section, "data_ip", "", msg, IP_LEN);
		memset(quote_param.m_ch_dest_ip, 0, sizeof(quote_param.m_ch_dest_ip));
		memcpy(quote_param.m_ch_dest_ip, msg, len);

		quote_param.m_i_dest_port = ini.ReadInt(section, "data_port", 0);

		len = ini.ReadString(section, "eth_name", "", msg, ETH_NAME_LEN);
		memset(quote_param.m_ch_eth_name, 0, sizeof(quote_param.m_ch_eth_name));
		memcpy(quote_param.m_ch_eth_name, msg, len);

		quote_param.m_ll_cache_size = ini.ReadInt(section, "cache_size", 0) * 1024ul * 1024ul;
		quote_param.m_b_force_normal_socket = (ini.ReadInt(section, "force_normal_socket", 0) != 0);
		quote_param.m_ll_proc_data_wait_time = ini.ReadInt( section , "proc_data_wait_time" , 10 );
		quote_param.m_ll_normal_socket_rxbuf = ini.ReadInt(section, "normal_socket_rxbuf", 10) * 1024ul * 1024ul;
		quote_param.m_b_out_of_order_correction = (ini.ReadInt(section, "out_of_order_correction", 0) != 0);
		return true;
	}
	else
	{
		return false;
	}
}

sze_report* run_sze()
{
	socket_udp_param	quote_param[SZE_QUOTE_PARAM_NUM];
	int i = 0;

	if (config_sock_udp_param(quote_param[i], "EFH_SZE_LEV2_TICK"))
	{
		quote_param[i].m_efh_type = enum_efh_sze_lev2_tick;
		i++;
	}
	if (config_sock_udp_param(quote_param[i], "EFH_SZE_LEV2_IDX"))
	{
		quote_param[i].m_efh_type = enum_efh_sze_lev2_idx;
		i++;
	}
	if (config_sock_udp_param(quote_param[i], "EFH_SZE_LEV2_SNAP"))
	{
		quote_param[i].m_efh_type = enum_efh_sze_lev2_snap;
		i++;
	}
	if (config_sock_udp_param(quote_param[i], "EFH_SZE_LEV2_AFTER_CLOSE"))
	{
		quote_param[i].m_efh_type = enum_efh_sze_lev2_after_close;
		i++;
	}
	if (config_sock_udp_param(quote_param[i], "EFH_SZE_LEV2_TREE"))
	{
		quote_param[i].m_efh_type = enum_efh_sze_lev2_tree;
		i++;
	}

	if (config_sock_udp_param(quote_param[i], "EFH_SZE_LEV2_IBR_TREE"))
	{
		quote_param[i].m_efh_type = enum_efh_sze_lev2_ibr_tree;
		i++;
	}

	if (config_sock_udp_param(quote_param[i], "EFH_SZE_LEV2_TURNOVER"))
	{
		quote_param[i].m_efh_type = enum_efh_sze_lev2_turnover;
		i++;
	}

	if (config_sock_udp_param(quote_param[i], "EFH_SZE_LEV2_BOND_SNAP"))
	{
		quote_param[i].m_efh_type = enum_efh_sze_lev2_bond_snap;
		i++;
	}

	if (config_sock_udp_param(quote_param[i], "EFH_SZE_LEV2_BOND_TICK"))
	{
		quote_param[i].m_efh_type = enum_efh_sze_lev2_bond_tick;
		i++;
	}

	sze_report* report = new sze_report();
	if (!report->init(quote_param, i))
	{
		return NULL;
	}

	int report_quit = ini.ReadInt("REPORT", "report_when_quit", 0);
	int nic_type	= ini.ReadInt("NIC_TYPE", "nic_type", 0);
	int tick_detach_enable	= ini.ReadInt("REPORT", "tick_detach_enable", 0);
	report->set_tick_detach( tick_detach_enable );

	if ( report_quit == 1 )
	{
		char msg[1024];
		memset( msg , 0 , sizeof( msg ) );

		ini.ReadString( "REPORT" , "symbol" , "" , msg , sizeof( msg ) );
		report->run( nic_type , true , msg );
	}
	else if ( report_quit == 0 )
	{
		report->run( nic_type , false );
	}

	return report;
}

void release_sze(sze_report* report)
{
	report->close();
	delete report;
}

sse_report* run_sse()
{
	socket_udp_param	quote_param[SSE_QUOTE_PARAM_NUM];
	int i = 0;

	if (config_sock_udp_param(quote_param[i], "EFH_SSE_LEV2_TICK"))
	{
		quote_param[i].m_efh_type = enum_efh_sse_lev2_tick;
		i++;
	}
	if (config_sock_udp_param(quote_param[i], "EFH_SSE_LEV2_IDX"))
	{
		quote_param[i].m_efh_type = enum_efh_sse_lev2_idx;
		i++;
	}
	if (config_sock_udp_param(quote_param[i], "EFH_SSE_LEV2_SNAP"))
	{
		quote_param[i].m_efh_type = enum_efh_sse_lev2_snap;
		i++;
	}
	if (config_sock_udp_param(quote_param[i], "EFH_SSE_LEV2_OPTION"))
	{
		quote_param[i].m_efh_type = enum_efh_sse_lev2_opt;
		i++;
	}

	if (config_sock_udp_param(quote_param[i], "EFH_SSE_LEV2_TREE"))
	{
		quote_param[i].m_efh_type = enum_efh_sse_lev2_tree;
		i++;
	}

	if (config_sock_udp_param(quote_param[i], "EFH_SSE_LEV2_BOND_SNAP"))
	{
		quote_param[i].m_efh_type = enum_efh_sse_lev2_bond_snap;
		i++;
	}

	if (config_sock_udp_param(quote_param[i], "EFH_SSE_LEV2_BOND_TICK"))
	{
		quote_param[i].m_efh_type = enum_efh_sse_lev2_bond_tick;
		i++;
	}

	if (config_sock_udp_param(quote_param[i], "EFH_SSE_LEV2_TICK_MERGE"))
	{
		quote_param[i].m_efh_type = enum_efh_sse_lev2_tick_merge;
		i++;
	}

	if (config_sock_udp_param(quote_param[i], "EFH_SSE_LEV2_ETF"))
	{
		quote_param[i].m_efh_type = enum_efh_sse_lev2_etf;
		i++;
	}

	if (config_sock_udp_param(quote_param[i], "EFH_SSE_STATIC_INFO"))
	{
		quote_param[i].m_efh_type = enum_efh_sse_lev2_static_info;
		i++;
	}

	sse_report* report = new sse_report();
	if (!report->init(quote_param, i))
	{
		return NULL;
	}

	int report_quit = ini.ReadInt("REPORT", "report_when_quit", 0);
	int nic_type = ini.ReadInt("NIC_TYPE", "nic_type", 0);
	int tick_detach_enable	= ini.ReadInt("REPORT", "tick_detach_enable", 0);
	report->set_tick_detach( tick_detach_enable );


	if ( report_quit == 1 )
	{
		char msg[1024];
		memset( msg , 0 , sizeof( msg ) );

		ini.ReadString( "REPORT" , "symbol" , "" , msg , sizeof( msg ) );
		report->run( nic_type , true , msg );
	}
	else if ( report_quit == 0 )
	{
		report->run( nic_type , false );
	}

	return report;
}

void release_sse(sse_report* report)
{
	report->close();
	delete report;
}

// bool string_split(const char* str_src, vector<string>& str_dst, const string& str_separator);

int main()
{
	char file[64] = INI_FILE_PATH;
	ini.Open( file );

	int sse_enable = ini.ReadInt( "EFH_QUOTE_TYPE" , "enable_sse" , 0 );
	int sze_enable = ini.ReadInt( "EFH_QUOTE_TYPE" , "enable_sze" , 0 );

	if ( sse_enable == 0 && sze_enable == 0 )
	{
		printf( "sze enable and sse enable is all is 0\n" );
		return 0;
	}
	sse_report* sse = NULL;
	sze_report* sze = NULL;
	if ( sze_enable == 1 )
	{
		sze = run_sze();
	}
	if ( sse_enable == 1 )
	{
		sse = run_sse();
	}

	while ( true )
	{
		printf( "[RCV]:" );
		fflush( stdout );

		char buf[1024] = { 0 };
		fgets( buf , sizeof( buf ) , stdin );
		int len = strlen( buf );
		if ( len <= 1 )
		{
			continue;
		}
		buf[len - 1] = 0;
		printf( "\n" );
		vector<string> cmd;
		if ( false == string_split( buf , cmd , " " ) )
		{
			continue;
		}

		if ( strcmp( cmd[0].c_str() , "quit" ) == 0 )
		{
			break;
		}
		else if ( strcmp( cmd[0].c_str() , "show" ) == 0 )
		{
			if ( sze_enable == 1 )
			{
				sze->show();
			}
			if ( sse_enable == 1 )
			{
				sse->show();
			}
		}
		else if( strcmp( cmd[0].c_str() , "rdf" ) == 0 )
		{
			if ( sse_enable == 1 )
			{
				sse->show_rdp( cmd[1].c_str() );
			}
		}
	}
	if ( sze_enable == 1 )
	{
		release_sze( sze );
	}
	else if ( sse_enable == 1 )
	{
		release_sse( sse );
	}

	return 0;
}
