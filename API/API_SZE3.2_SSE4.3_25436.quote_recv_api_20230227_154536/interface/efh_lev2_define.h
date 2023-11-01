/*!****************************************************************************
@node        Copyright (coffee), 2005-2020, Shengli Tech. Co., Ltd.
@file        efh_lev2_define.h
@date        2020/12/14 08:30
@author      shengli

@brief       本接口定义接收需要的公共数据接口
******************************************************************************/
#pragma once
#ifdef WINDOWS
#define        DLL_EFH_LEV2_DLL_NAME            "efh_lev2_api.dll"      // api动态库名
#else
#define        DLL_EFH_LEV2_DLL_NAME            "libsl_efh_lev2_api.so" // api动态库名
#endif
#define        EFH_API_VERSION                  "V2.1.2"                // api版本号
#define        SSE_LEV2_PROTOCOL_VERSION        "EFH_SSE_4.3"           // 上交所协议版本号
#define        SZE_LEV2_PROTOCOL_VERSION        "EFH_SZE_3.2"           // 深交所协议版本号
#define        DEFAULT_CACHE_SIZE               (512*1024*1024)         // 缓存默认大小512M
#define        IP_LEN                           (64)                    // IP字符串地址长度定义
#define        ETH_NAME_LEN                     (64)                    // Eth字符串长度定义
#define        VERSION_LEN                      (64)                    // 版本号字符串长度

// 行情类型
enum enum_efh_lev2_type
{
    enum_efh_sze_lev2_snap                      = 1,                    // 深交所lev2快照
    enum_efh_sze_lev2_tick                      = 2,                    // 深交所逐笔信息
    enum_efh_sze_lev2_idx                       = 3,                    // 深交所指数行情
    enum_efh_sze_lev2_tree                      = 4,                    // 深交所lev2建树
    enum_efh_sze_lev2_after_close               = 5,                    // 深交所盘后定价快照
    enum_efh_sze_lev2_ibr_tree                  = 6,                    // 深交所ibr建树
    enum_efh_sze_lev2_turnover                  = 7,                    // 深交所成交量统计快照

    enum_efh_sse_lev2_snap                      = 8,                    // 上交所lev2快照
    enum_efh_sse_lev2_idx                       = 9,                    // 上交所指数行情
    enum_efh_sse_lev2_tick                      = 10,                   // 上交所逐笔信息
    enum_efh_sse_lev2_opt                       = 11,                   // 上交所股票期权
    enum_efh_sse_lev2_tree                      = 12,                   // 上交所建树快照
    enum_efh_sse_lev2_bond_snap                 = 13,                   // 上交所债券快照
    enum_efh_sse_lev2_bond_tick                 = 14,                   // 上交所债券逐笔信息
    enum_efh_sse_lev2_static_info               = 15,                   // 上交所静态信息

    enum_efh_sze_lev2_bond_snap                 = 16,                   // 深交所债券 level-2 快照
    enum_efh_sze_lev2_bond_tick                 = 17,                   // 深交所债券逐笔信息

    enum_efh_sse_lev2_tick_merge                = 18,                   // 上交所逐笔合并消息
    enum_efh_sse_lev2_etf                       = 19,                   // 上交所 ETF 统计消息
};

// 接收方式
enum enum_efh_nic_type
{
    enum_nic_normal ,                                                   // 默认系统接收,操作系统协议栈
    enum_nic_solarflare_efvi ,                                          // solarflare efvi接收
    enum_nic_exablaze_exanic ,                                          // Exablaze exanic接收
    enum_nic_x710_win_speed ,                                           // win高速接收，只适用于windows
    enum_nic_solarflare_win_speed ,                                     // win高速接收，目前只适用于windows
};


#pragma pack(push, 1)

// 通道配置
struct socket_udp_param
{
    enum_efh_lev2_type                m_efh_type;                       // 通道行情类型
    bool                              m_b_polling;                      // 数据接收线程是否忙等待 true：等待 false：不等待，现在默认为false，功能未实现
    unsigned long long                m_ll_cache_size;                  // 设置缓存大小,设置为 0 表示不使用缓存,单位为字节
    int                               m_i_cpu_id;                       // 用于接收的cpu序号,当cpu赋值为-1时，则不绑定
    char                              m_ch_dest_ip[IP_LEN];             // 目标ip地址
    unsigned short                    m_i_dest_port;                    // 目标端口号
    char                              m_ch_src_ip[IP_LEN];              // 源ip地址
    unsigned short                    m_i_src_port;                     // 源端口号
    char                              m_ch_eth_name[ETH_NAME_LEN];      // 网卡名

    bool                              m_b_force_normal_socket;          // 强制使用普通模式 socket 开关
    unsigned long long                m_ll_proc_data_wait_time;         // 缓存队列空闲时, 轮询间隔, 单位 us, 默认为 10 us
    unsigned long long                m_ll_normal_socket_rxbuf;         // 设置 socket 模式 SO_RCVBUF 尺寸, 默认为 10 Mb
    bool                              m_b_out_of_order_correction;      // 乱序纠错功能开关

    socket_udp_param()
    {
		
        m_efh_type = enum_efh_sze_lev2_snap;
        m_b_polling = false;
        m_ll_cache_size = 256ul * 1024ul * 1024ul;
        m_i_cpu_id = -1;

        m_b_force_normal_socket = false;
        m_ll_proc_data_wait_time = 10;
        m_ll_normal_socket_rxbuf = 10ul * 1024ul * 1024ul;
        m_b_out_of_order_correction = false;
    }
};

// 版本
struct efh_version
{
    char                              m_ch_protocol_version[VERSION_LEN];       // 协议版本号
    char                              m_ch_api_version[VERSION_LEN];            // api版本号
};

#pragma pack(pop)
