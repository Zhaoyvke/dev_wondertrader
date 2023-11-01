/*!****************************************************************************
@node        Copyright (coffee), 2005-2020, Shengli Tech. Co., Ltd.
@file        sse_hpf_define.h
@date        2020/12/14 08:30
@author      shengli

@brief       本接口定义上交所盛立行情输出结构
******************************************************************************/
#pragma once
#include <string.h>

#define        SSE_EXCHANGE_ID                  (100)                   // 上交所exchange_id
#define        SSE_SYMBOL_LEN                   (9)                     // 上交所合约长度
#define        SSE_LEV2_DEPTH                   (10)                    // 上交所lev2档位深度
#define        SSE_OPTION_DEPTH                 (5)                     // 上交所期权档位深度

#define        SSE_SECURITY_NAME_LEN            (9)
#define        SSE_PRODUCT_STATUS_LEN           (21)
#define        DATE_LEN                         (9)

#define        SSE_LEV2_IDX_MSG_TYPE            (33)                    // 上交所lev2指数消息类型
#define        SSE_LEV2_EXE_MSG_TYPE            (34)                    // 上交所lev2成交消息类型
#define        SSE_LEV2_OPT_MSG_TYPE            (35)                    // 上交所lev2期权消息类型
#define        SSE_LEV2_SNAP_MSG_TYPE           (39)                    // 上交所lev2快照消息类型
#define        SSE_LEV2_ORDER_MSG_TYPE          (32)                    // 上交所lev2委托信息类型
#define        SSE_LEV2_TREE_MSG_TYPE           (38)                    // 上交所lev2建树信息类型
#define        SSE_LEV2_BOND_SNAP_MSG_TYPE      (60)                    // 上交所lev2债券快照信息类型
#define        SSE_LEV2_BOND_TICK_MSG_TYPE      (61)                    // 上交所lev2债券逐笔信息类型
#define        SSE_LEV2_TICK_MERGE_MSG_TYPE     (62)                    // 上交所lev2逐笔合并信息类型
#define        SSE_LEV2_ETF_MSG_TYPE            (63)                    // 上交所lev2 ETF 信息类型
#define        SSE_LEV2_STATIC_INFO_MSG_TYPE    (255)                   // 上交所lev2静态信息类型

#define        SSE_LEV2_IDX_HT_MSG_TYPE         (133)                   // 上交所lev2指数心跳消息类型,api内部消化
#define        SSE_LEV2_TICK_HT_MSG_TYPE        (132)                   // 上交所lev2逐笔心跳信息类型,api内部消化
#define        SSE_LEV2_OPT_HT_MSG_TYPE         (135)                   // 上交所lev2期权心跳信息类型,api内部消化
#define        SSE_LEV2_TREE_HT_MSG_TYPE        (138)                   // 上交所lev2建树心跳信息类型,api内部消化
#define        SSE_LEV2_SNAP_HT_MSG_TYPE        (139)                   // 上交所lev2快照心跳信息类型,api内部消化
#define        SSE_LEV2_BOND_SNAP_HT_MSG_TYPE   (160)                   // 上交所lev2债券快照心跳信息类型,api内部消化
#define        SSE_LEV2_BOND_TICK_HT_MSG_TYPE   (161)                   // 上交所lev2债券逐笔心跳信息类型,api内部消化
#define        SSE_LEV2_TICK_MERGE_HT_MSG_TYPE  (162)                   // 上交所lev2逐笔合并心跳信息类型,api内部消化
#define        SSE_LEV2_ETF_HT_MSG_TYPE         (163)                   // 上交所lev2 ETF 心跳信息类型,api内部消化

#pragma pack(push, 1)

// 上交所心跳消息
struct sse_heartbeat
{
    unsigned int                      m_sequence;                       // 盛立行情序号
    unsigned short                    m_reserved_1;                     // 保留字段
    unsigned short                    m_reserved_2;                     // 保留字段
    unsigned char                     m_message_type;                   // 行情类型
    unsigned short                    m_message_len;                    // 包括此消息头的长度
    unsigned char                     m_reserved_3[5];                  // 保留字段
};

// 上交所消息协议头
struct sse_hpf_head
{
    unsigned int                      m_sequence;                       // 盛立行情序号
    unsigned short                    m_reserved_1;                     // 保留字段
    unsigned short                    m_reserved_2;                     // 保留字段
    unsigned char                     m_message_type;                   // 行情类型
    unsigned short                    m_message_len;                    // 包括此消息头的长度
    unsigned char                     m_exchange_id;                    // 交易所编号
    unsigned short                    m_quote_date_year;                // 行情更新年份
    unsigned char                     m_quote_date_month;               // 行情更新月份
    unsigned char                     m_quote_date_day;                 // 行情更新日期
    unsigned int                      m_send_time;                      // 交易所发送时间
    unsigned char                     m_category_id;                    // 数据产品类别
    unsigned int                      m_msg_seq_id;                     // 行情包序号
    unsigned char                     m_seq_lost_flag;                  // 1=有丢包，0=没有丢包
};

// 上交所lev2快照档位单元
struct sse_lev2_price_quantity_unit
{
    sse_lev2_price_quantity_unit()
    {
        memset(this, 0, sizeof(sse_lev2_price_quantity_unit));          // 初始化结构体
    }

    char                              m_lev_operator;                   // 辅助信息
    char                              m_reserved[3];                    // 保留字段
    unsigned int                      m_price;                          // 价格，   实际值需要除以 1000
    unsigned long long                m_quantity;                       // 数量，   实际值需要除以 1000
};

// 上交所lev2快照消息
struct sse_hpf_lev2
{
    sse_hpf_lev2()
    {
        memset(this, 0, sizeof(sse_hpf_lev2));                          // 初始化结构体
    }

    sse_hpf_head                      m_head;                           // 包头
    unsigned int                      m_quote_update_time;              // 行情更新时间,格式如：HHMMSS（秒）HH=00-23, MM=00-59, SS=00-60(秒)
    char                              m_symbol[SSE_SYMBOL_LEN];         // 证券代码
    //******************************************************************************************
    //0：指数    1：股票 2：基金 3：债券 4：回购 5：权证 6：期货 7：外汇 8：利率 9：期权 10：其他
    //******************************************************************************************
    unsigned char                     m_security_type;                  // 证券类型
    unsigned char                     m_image_status;                   // 快照类型，1=全量，2=更新
    unsigned char                     m_reserved_1;                     // 保留字段
    unsigned int                      m_pre_close_price;                // 昨收价，    实际值需要除以 1000
    unsigned int                      m_open_price;                     // 开盘价，    实际值需要除以 1000
    unsigned int                      m_day_high_price;                 // 最高价，    实际值需要除以 1000
    unsigned int                      m_day_low_price;                  // 最低价，    实际值需要除以 1000
    unsigned int                      m_last_price;                     // 最新价，    实际值需要除以 1000
    unsigned int                      m_today_close_price;              // 收盘价，    实际值需要除以 1000
    unsigned char                     m_instrument_status;              // 当前品种交易状态
    //******************************************************************************************
    // 0:其他    1:启动 2:开市集合竞价 3:连续自动撮合 4:停牌 5:收盘集合竞价
    // 6:闭市，自动计算闭市价格  7:交易结束  8:产品未上市
    //******************************************************************************************
    unsigned char                     m_trading_status;                 // 当前产品状态
    //******************************************************************************************
    // Bit7-Bit4(m_trading_status&0xF0):
    // 0x00:表示启动（开市前)
    // 0x10:表示开盘集合竞价
    // 0x20:表示连续交易
    // 0x30:表示闭市
    // 0x40:表示合约停牌
    // 0x50:表示可恢复交易的熔断时段（盘中集合竞价）
    // 0x60:表示不可恢复交易的熔断时段（暂停交易至闭市）
    // 0x70:表示收盘集合竞价时段
    // 0x80:表示波动性中断（适用于股票期权)
    // 0x15:表示无意义
    // Bit3(m_trading_status&0x08):
    // 0x08:‟表示此合约可正常交易 0x00:表示此合约不可正常交易
    // Bit2(m_trading_status&0x04):
    // 0x04:表示已上市 0x00:表示未上市
    // Bit1(m_trading_status&0x02):
    // 0x02:表示此合约在当前时段接受订单申报 0x00:表示此合约在当前时段不接受订单申报
    // Bit0:预留
    //******************************************************************************************
    unsigned short                    m_reserved_2;                     // 保留字段
    unsigned int                      m_total_trade_num;                // 成交笔数
    unsigned long long                m_total_quantity;                 // 成交总量，               实际值需要除以 1000
    unsigned long long                m_total_value;                    // 成交总额，               实际值需要除以 100000
    unsigned long long                m_total_bid_quantity;             // 委托买入总量，           实际值需要除以 1000
    unsigned int                      m_total_bid_weighted_avg_price;   // 买方加权平均委买价格，   实际值需要除以 1000
    unsigned long long                m_total_ask_quantity;             // 委托卖出总量，           实际值需要除以 1000
    unsigned int                      m_total_ask_weighted_avg_price;   // 卖方加权平均委买价格，   实际值需要除以 1000
    unsigned int                      m_yield_to_maturity;              // 债券到期收益率
    unsigned char                     m_bid_depth;                      // 买盘价位数量
    unsigned char                     m_ask_depth;                      // 卖盘价位数量
    sse_lev2_price_quantity_unit      m_bid_unit[SSE_LEV2_DEPTH];       // 买边十档
    sse_lev2_price_quantity_unit      m_ask_unit[SSE_LEV2_DEPTH];       // 卖边十档
};

// 上交所lev2指数消息
struct sse_hpf_idx
{
    sse_hpf_idx()
    {
        memset(this, 0, sizeof(sse_hpf_idx));                           // 初始化结构体
    }

    sse_hpf_head                      m_head;                           // 包头
    unsigned int                      m_quote_update_time;              // 行情更新时间,格式如：HHMMSS（秒）HH=00-23, MM=00-59, SS=00-60(秒)
    char                              m_symbol[SSE_SYMBOL_LEN];         // 证券代码
    //******************************************************************************************
    //0：指数    1：股票 2：基金 3：债券 4：回购 5：权证 6：期货 7：外汇 8：利率 9：期权 10：其他
    //******************************************************************************************
    unsigned char                     m_security_type;                  // 0=指数， 10=其他
    unsigned int                      m_pre_close_price;                // 昨收价，     实际值需要除以 100000
    unsigned int                      m_open_price;                     // 开盘价，     实际值需要除以 100000
    unsigned long long                m_total_value;                    // 成交额，     实际值需要除以 10
    unsigned int                      m_day_high_price;                 // 最高价，     实际值需要除以 100000
    unsigned int                      m_day_low_price;                  // 最低价，     实际值需要除以 100000
    unsigned int                      m_last_price;                     // 最新价，     实际值需要除以 100000
    unsigned long long                m_total_quantity;                 // 成交量，     实际值需要除以 100000
    unsigned int                      m_today_close_price;              // 今收盘价，   实际值需要除以 100000
};

// 上交所lev2成交消息
struct sse_hpf_exe
{
    sse_hpf_exe()
    {
        memset(this, 0, sizeof(sse_hpf_exe));
    }

    sse_hpf_head                      m_head;                           // 包头
    unsigned int                      m_trade_index;                    // 成交序号
    unsigned int                      m_channel_num;                    // 频道号
    char                              m_symbol[SSE_SYMBOL_LEN];         // 证券代码
    unsigned int                      m_trade_time;                     // 成交时间,格式如：HHMMSSss（百分之一秒）HH=00-23, MM=00-59, SS=00-60(秒) ss=00-99(百分之一秒)
    unsigned int                      m_trade_price;                    // 成交价，     实际值需要除以 1000
    unsigned long long                m_trade_quantity;                 // 成交量，     实际值需要除以 1000
    unsigned long long                m_trade_value;                    // 成交额，     实际值需要除以 100000
    unsigned long long                m_trade_buy_num;                  // 买方订单号
    unsigned long long                m_trade_sell_num;                 // 卖方订单号
    char                              m_trade_bs_flag;                  // 内外盘标志：B-外盘，主动买；S-内盘，主动卖；N-未知
    unsigned long long                m_biz_index;                      // 业务序号
    unsigned char                     m_reserved[4];                    // 保留字段
};

// 上交所逐笔委托消息
struct sse_hpf_order
{
    sse_hpf_order()
    {
        memset(this, 0, sizeof(sse_hpf_order));
    }

    sse_hpf_head                      m_head;                           // 消息头
    unsigned int                      m_order_index;                    // 委托序号
    unsigned int                      m_channel_num;                    // 通道号
    char                              m_symbol[SSE_SYMBOL_LEN];         // 标的名称
    unsigned int                      m_order_time;                     // 委托时间,格式如：HHMMSSss（百分之一秒）HH=00-23, MM=00-59, SS=00-60(秒) ss=00-99(百分之一秒)
    char                              m_order_type;                     // 订单类型，A=新增订单，D=删除订单
    unsigned long long                m_order_num;                      // 原始订单号
    unsigned int                      m_order_price;                    // 委托价格，       实际值需要除以 1000
    unsigned long long                m_balance;                        // 剩余委托量，     实际值需要除以 1000
    unsigned char                     m_reserved_0[15];                 // 15个字节保留字段
    char                              m_side_flag;                      // 买卖标志,B-主动买；S-主动卖
    unsigned long long                m_biz_index;                      // 业务序列号
    unsigned char                     m_reserved_1[4];                  // 4个字节的保留字段
};

// 上交所lev2建树档位单元
struct sse_lev2_tree_price_quantity_unit
{
    sse_lev2_tree_price_quantity_unit()
    {
        memset(this, 0, sizeof(sse_lev2_tree_price_quantity_unit));
    }
    unsigned int                      m_price;                          // 价格，   实际值需要除以 1000
    unsigned long long                m_quantity;                       // 数量，   实际值需要除以 1000
};

// 上交所lev2 建树消息
struct sse_hpf_tree
{
    sse_hpf_tree()
    {
        memset(this, 0, sizeof(sse_hpf_tree));
    }

    sse_hpf_head                      m_head;
    unsigned int                      m_channel_num;                    // 通道号
    unsigned int                      m_quote_update_time;              // 行情时间
    char                              m_symbol[SSE_SYMBOL_LEN];         // 标的名称
    //******************************************************************************************
    //0：指数    1：股票 2：基金 3：债券 4：回购 5：权证 6：期货 7：外汇 8：利率 9：期权 10：其他
    //******************************************************************************************
    unsigned char                     m_security_type;                  // 股票/合约类型
    unsigned long long                m_biz_index;                      // 业务序列号
    unsigned int                      m_pre_close_price;                // 昨收价，                 实际值需要除以 1000
    unsigned int                      m_open_price;                     // 开盘价，                 实际值需要除以 1000
    unsigned int                      m_day_high_price;                 // 最高价，                 实际值需要除以 1000
    unsigned int                      m_day_low_price;                  // 最低价，                 实际值需要除以 1000
    unsigned int                      m_last_price;                     // 最新价，                 实际值需要除以 1000
    unsigned int                      m_today_close_price;              // 今收盘价，               实际值需要除以 1000
    unsigned char                     m_instrument_status;              // 当前品种交易状态
    //******************************************************************************************
    // 0:其他    1:启动 2:开市集合竞价 3:连续自动撮合 4:停牌 5:收盘集合竞价
    // 6:闭市，自动计算闭市价格  7:交易结束  8:产品未上市
    //******************************************************************************************
    unsigned char                     m_trading_status;                 // 当前产品状态
    //******************************************************************************************
    // Bit7-Bit4(m_trading_status&0xF0):
    // 0x00:表示启动（开市前)
    // 0x10:表示开盘集合竞价
    // 0x20:表示连续交易
    // 0x30:表示闭市
    // 0x40:表示合约停牌
    // 0x50:表示可恢复交易的熔断时段（盘中集合竞价）
    // 0x60:表示不可恢复交易的熔断时段（暂停交易至闭市）
    // 0x70:表示收盘集合竞价时段
    // 0x80:表示波动性中断（适用于股票期权)
    // 0x15:表示无意义
    // Bit3(m_trading_status&0x08):
    // 0x08:‟表示此合约可正常交易 0x00:表示此合约不可正常交易
    // Bit2(m_trading_status&0x04):
    // 0x04:表示已上市 0x00:表示未上市
    // Bit1(m_trading_status&0x02):
    // 0x02:表示此合约在当前时段接受订单申报 0x00:表示此合约在当前时段不接受订单申报
    // Bit0:预留
    //******************************************************************************************
    unsigned char                     m_reserved_0[6];                  // 6个字节的保留字段
    unsigned int                      m_total_trade_num;                // 总成交笔数
    unsigned long long                m_total_quantity;                 // 总成交量，               实际值需要除以 1000
    unsigned long long                m_total_value;                    // 总成交金额，             实际值需要除以 100000
    unsigned long long                m_total_bid_quantity;             // 委托买入数量，           实际值需要除以 1000
    unsigned int                      m_total_bid_weighted_avg_price;   // 加权平均委托买入价格，   实际值需要除以 1000
    unsigned long long                m_total_ask_quantity;             // 委托卖出总量，           实际值需要除以 1000
    int                               m_total_ask_weighted_avg_price;   // 加权平均委托卖价格，     实际值需要除以 1000
    int                               m_yield_to_maturity;              // 债券到期收益率
    unsigned char                     m_bid_depth;                      // 申买档位数
    unsigned char                     m_ask_depth;                      // 申卖档位数
    unsigned char                     m_reserved_1[2];                  // 2字节的保留字段
    sse_lev2_tree_price_quantity_unit m_bid_unit[SSE_LEV2_DEPTH];       // 申买档位信息
    sse_lev2_tree_price_quantity_unit m_ask_unit[SSE_LEV2_DEPTH];       // 申卖档位信息
};

// 上交所lev2期权档位单元
struct sse_option_price_quantity_unit
{
    sse_option_price_quantity_unit()
    {
        memset(this, 0, sizeof(sse_option_price_quantity_unit));
    }
    unsigned int                      m_price;                          // 价格，实际值需要除以 10000
    unsigned long long                m_quantity;                       // 数量，实际值需要除以 100
};

// 上交所lev2期权消息
struct sse_hpf_stock_option
{
    sse_hpf_stock_option()
    {
        memset(this, 0, sizeof(sse_hpf_stock_option));                  // 初始化结构体
    }

    sse_hpf_head                      m_head;                           // 包头
    unsigned int                      m_quote_update_time;              // 行情更新时间,格式如：HHMMSSss（百分之一秒）HH=00-23, MM=00-59, SS=00-60(秒) ss=00-99(百分之一秒)
    //******************************************************************************************
    //0：指数    1：股票 2：基金 3：债券 4：回购 5：权证 6：期货 7：外汇 8：利率 9：期权 10：其他
    //******************************************************************************************
    unsigned char                     m_security_type;                  // 证券类型
    char                              m_symbol[SSE_SYMBOL_LEN];         // 合约
    unsigned char                     m_image_status;                   // 快照类型
    unsigned char                     m_reserved_1;                     // 保留字段
    unsigned int                      m_pre_close_price;                // 昨收盘价，       实际值需要除以 10000
    unsigned int                      m_pre_settle_price;               // 昨结算价，       实际值需要除以 10000
    unsigned int                      m_open_price;                     // 开盘价，         实际值需要除以 10000
    unsigned int                      m_day_high_price;                 // 最高价，         实际值需要除以 10000
    unsigned int                      m_day_low_price;                  // 最低价 ，        实际值需要除以 10000
    unsigned int                      m_last_price;                     // 最新价，         实际值需要除以 10000
    unsigned int                      m_today_close_price;              // 收盘价，         实际值需要除以 10000
    unsigned int                      m_today_settle_price;             // 今结算价，       实际值需要除以 10000
    unsigned int                      m_dynamic_price;                  // 动态参考价格，   实际值需要除以 10000
    unsigned char                     m_reserved_2[3];                  // 保留字段
    unsigned char                     m_trading_status;                 // 当前产品状态
    //******************************************************************************************
    // Bit7-Bit4(m_trading_status&0xF0):
    // 0x00:表示启动（开市前)
    // 0x10:表示开盘集合竞价
    // 0x20:表示连续交易
    // 0x30:表示闭市
    // 0x40:表示合约停牌
    // 0x50:表示可恢复交易的熔断时段（盘中集合竞价）
    // 0x60:表示不可恢复交易的熔断时段（暂停交易至闭市）
    // 0x70:表示收盘集合竞价时段
    // 0x80:表示波动性中断（适用于股票期权)
    // 0x15:表示无意义
    // Bit3(m_trading_status&0x08):
    // 0x08:‟表示此合约可正常交易 0x00:表示此合约不可正常交易
    // Bit2(m_trading_status&0x04):
    // 0x04:表示已上市 0x00:表示未上市
    // Bit1(m_trading_status&0x02):
    // 0x02:表示此合约在当前时段接受订单申报 0x00:表示此合约在当前时段不接受订单申报
    // Bit0:预留
    //******************************************************************************************
    unsigned long long                m_open_interest;                  // 未平仓合约数量， 实际值需要除以 100
    unsigned int                      m_total_trade_num;                // 成交笔数
    unsigned long long                m_total_quantity;                 // 成交量，         实际值需要除以 100
    unsigned long long                m_total_value;                    // 成交金额，       实际值需要除以 1000000
    unsigned char                     m_bid_depth;                      // 申买档位数
    unsigned char                     m_ask_depth;                      // 申卖档位数
    sse_option_price_quantity_unit    m_bid_unit[SSE_OPTION_DEPTH];     // 买边五档
    sse_option_price_quantity_unit    m_ask_unit[SSE_OPTION_DEPTH];     // 卖边五档
};


// 上交所lev2债券快照档位单元
struct sse_lev2_bond_price_quantity_unit
{
    sse_lev2_bond_price_quantity_unit()
    {
        memset(this, 0, sizeof(sse_lev2_bond_price_quantity_unit));
    }
    unsigned int                      m_price;                          // 价格，   实际值需要除以 1000
    unsigned long long                m_quantity;                       // 数量，   实际值需要除以 1000
};
// 上交所lev2债券快照
struct sse_hpf_bond_snap
{
    sse_hpf_bond_snap()
    {
        memset(this, 0, sizeof(sse_hpf_bond_snap));                  // 初始化结构体
    }

    sse_hpf_head                      m_head;                           // 包头
    unsigned int                      m_quote_update_time;              // 行情更新时间,格式如：HHMMSSsss（千分之一秒）HH=00-23, MM=00-59, SS=00-60(秒) ss=000-999(千分之一秒) 如 143025002 表示 14:30:25.002
    unsigned char                     m_reserved_0[4];                  // 保留字段
    char                              m_symbol[SSE_SYMBOL_LEN];         // 合约
    // *******************************************************************************************
    //     证券类型            证券子类型
    //     0            指数   0              指数
    //                         255            未分类
    //     1            股票   0              主板
    //                         1
    //                         2              创业板
    //                         3              B股
    //                         4              H股
    //                         5              科创板
    //                         255            未分类
    //     2            基金   248            LOF基金
    //                         249            ETF基金
    //                         255            未分类
    //     3            债券   248            国债
    //                         249            可转债
    //                         255            未分类
    //     4            回购   248            国债回购
    //                         255            未分类
    //     9            期权   248            股票期权
    //                         249            ETF期权
    //                         255            未分类
    //     10           其他
    // *******************************************************************************************
    unsigned char                     m_security_type;                  // 证券类型
    unsigned char                     m_sub_security_type;              // 证券子类型
    unsigned char                     m_reserved_1[3];                  // 保留字段
    unsigned int                      m_pre_close_price;                // 昨收盘价，       实际值需要除以 1000
    unsigned int                      m_open_price;                     // 开盘价，         实际值需要除以 1000
    unsigned int                      m_day_high_price;                 // 最高价，         实际值需要除以 1000
    unsigned int                      m_day_low_price;                  // 最低价 ，        实际值需要除以 1000
    unsigned int                      m_last_price;                     // 最新价，         实际值需要除以 1000
    unsigned int                      m_today_close_price;              // 收盘价，         实际值需要除以 1000
    unsigned char                     m_instrument_status;              // 当前品种交易状态
    //******************************************************************************************
    // 0:其他    1:启动 2:开市集合竞价 3:连续自动撮合 4:停牌
    // 6:闭市，自动计算闭市价格  7:交易结束  8:产品未上市
    //******************************************************************************************
    unsigned char                     m_reserved_2[3];                  // 保留字段
    unsigned int                      m_total_trade_num;                // 总成交笔数
    unsigned long long                m_total_quantity;                 // 总成交量，               实际值需要除以 1000
    unsigned long long                m_total_value;                    // 总成交金额，             实际值需要除以 100000
    unsigned long long                m_total_bid_quantity;             // 委托买入数量，           实际值需要除以 1000
    unsigned int                      m_total_bid_weighted_avg_price;   // 加权平均委托买入价格，   实际值需要除以 1000
    unsigned long long                m_total_ask_quantity;             // 委托卖出总量，           实际值需要除以 1000
    unsigned int                      m_total_ask_weighted_avg_price;   // 加权平均委托卖价格，     实际值需要除以 1000

    unsigned int                      m_withdraw_bid_num;               // 买入撤单笔数
    unsigned long long                m_withdraw_bid_amount;            // 买入撤单数量             实际值需要除以 1000
    unsigned long long                m_withdraw_bid_price;             // 买入撤单金额             实际值需要除以 100000
    unsigned int                      m_withdraw_ask_num;               // 卖出撤单笔数
    unsigned long long                m_withdraw_ask_amount;            // 卖出撤单数量             实际值需要除以 1000
    unsigned long long                m_withdraw_ask_price;             // 卖出撤单金额             实际值需要除以 100000
    unsigned int                      m_total_bid_num;                  // 买入总笔数
    unsigned int                      m_total_ask_num;                  // 卖出总笔数
    unsigned int                      m_bid_trade_max_duration;         // 买入委托成交最大等待时间
    unsigned int                      m_ask_trade_max_duration;         // 卖出委托成交最大等待时间

    unsigned char                     m_bid_depth;                      // 申买档位数
    unsigned char                     m_ask_depth;                      // 申卖档位数
    unsigned char                     m_reserved_3[6];                  // 保留字段
    sse_lev2_bond_price_quantity_unit   m_bid_unit[SSE_LEV2_DEPTH];     // 申买信息
    sse_lev2_bond_price_quantity_unit   m_ask_unit[SSE_LEV2_DEPTH];     // 申卖信息
};

// 债券逐笔
struct sse_hpf_bond_tick
{
    sse_hpf_bond_tick()
    {
        memset(this, 0, sizeof(sse_hpf_bond_tick));                  // 初始化结构体
    }

    sse_hpf_head                      m_head;                           // 包头
    unsigned int                      m_tick_index;                     // 序号从1开始,按通道连续
    unsigned int                      m_channel_num;                    // 通道号
    char                              m_symbol[SSE_SYMBOL_LEN];         // 标的名称
    // *******************************************************************************************
    //     证券类型            证券子类型
    //     0            指数   0              指数
    //                         255            未分类
    //     1            股票   0              主板
    //                         1
    //                         2              创业板
    //                         3              B股
    //                         4              H股
    //                         5              科创板
    //                         255            未分类
    //     2            基金   248            LOF基金
    //                         249            ETF基金
    //                         255            未分类
    //     3            债券   248            国债
    //                         249            可转债
    //                         255            未分类
    //     4            回购   248            国债回购
    //                         255            未分类
    //     9            期权   248            股票期权
    //                         249            ETF期权
    //                         255            未分类
    //     10           其他
    // *******************************************************************************************
    unsigned char                     m_security_type;                  // 证券类型
    unsigned char                     m_sub_security_type;              // 证券子类型
    unsigned int                      m_tick_time;                      // 订单或成交时间,格式如：HHMMSSsss（千分之一秒）HH=00-23, MM=00-59, SS=00-60(秒) ss=000-999(千分之一秒) 如 143025002 表示 14:30:25.002
    unsigned char                     m_tick_type;                      // 类型， A新增订单， D删除订单， 删除订单， S产品状态订单， T成交
    unsigned long long                m_buy_num;                        // 买方订单号
    unsigned long long                m_sell_num;                       // 卖方订单号
    unsigned int                      m_price;                          // 价格，对产品状态订单无意义       实际值需要除以 1000
    unsigned long long                m_quantity;                       // 数量（手），对产品状态订单无意义 实际值需要除以 1000
    unsigned long long                m_trade_value;                    // 成交额，仅适用于成交消息         实际值需要除以 100000
    char                              m_side_flag;                      // 买卖标志，若为订单： B - 买单， S - 卖单；若为成交： B - 外盘， 主动买， S - 内盘， 主动卖 N - 未知 。
    unsigned char                     m_instrument_status;              // 当前品种交易状态，仅适用于产品状态订单
    //******************************************************************************************
    // 0:其他    1:启动 2:开市集合竞价 3:连续自动撮合 4:停牌
    // 6:闭市，自动计算闭市价格  7:交易结束  8:产品未上市
    //******************************************************************************************
    unsigned char                     m_reserved[8];                    // 保留字段
};

// 逐笔合并
struct sse_hpf_tick_merge
{
    sse_hpf_tick_merge()
    {
        memset(this, 0, sizeof(sse_hpf_tick_merge));                  // 初始化结构体
    }

    sse_hpf_head                      m_head;                           // 包头
    unsigned long long                m_tick_index;                     // 序号从1开始,按通道连续
    unsigned int                      m_channel_num;                    // 通道号
    char                              m_symbol[SSE_SYMBOL_LEN];         // 标的名称
    // *******************************************************************************************
    //     证券类型            证券子类型
    //     0            指数   0              指数
    //                         255            未分类
    //     1            股票   0              主板
    //                         1
    //                         2              创业板
    //                         3              B股
    //                         4              H股
    //                         5              科创板
    //                         255            未分类
    //     2            基金   248            LOF基金
    //                         249            ETF基金
    //                         255            未分类
    //     3            债券   248            国债
    //                         249            可转债
    //                         255            未分类
    //     4            回购   248            国债回购
    //                         255            未分类
    //     9            期权   248            股票期权
    //                         249            ETF期权
    //                         255            未分类
    //     10           其他
    // *******************************************************************************************
    unsigned char                     m_security_type;                  // 证券类型
    unsigned char                     m_sub_security_type;              // 证券子类型
    unsigned int                      m_tick_time;                      // 订单或成交时间,格式如：HHMMSSsss（千分之一秒）HH=00-23, MM=00-59, SS=00-60(秒) ss=000-999(千分之一秒) 如 143025002 表示 14:30:25.002
    unsigned char                     m_tick_type;                      // 类型， A新增订单， D删除订单， 删除订单， S产品状态订单， T成交
    unsigned long long                m_buy_num;                        // 买方订单号
    unsigned long long                m_sell_num;                       // 卖方订单号
    unsigned int                      m_price;                          // 价格，对产品状态订单无意义       实际值需要除以 1000
    unsigned long long                m_quantity;                       // 数量（手），对产品状态订单无意义 实际值需要除以 1000
    unsigned long long                m_trade_value;
    // 对于新增委托，表示已成交的委托数量， 字段类型为 uInt64_1000, 实际值需要除以 1000；
    // 对于成交，表示成交金额 （ 元 ）， 字段类型为 uInt64_100000, 实际值需要除以 100000；
    // 其他无意义

    char                              m_side_flag;                      // 买卖标志，若为订单： B - 买单， S - 卖单；若为成交： B - 外盘， 主动买， S - 内盘， 主动卖 N - 未知 。
    unsigned char                     m_instrument_status;              // 当前品种交易状态，仅适用于产品状态订单
    //******************************************************************************************
    // 0:其他    1:启动 2:开市集合竞价 3:连续自动撮合 4:停牌
    // 6:闭市，自动计算闭市价格  7:交易结束  8:产品未上市
    //******************************************************************************************
    unsigned char                     m_reserved[4];                    // 保留字段
};

// ETF 统计
struct sse_hpf_etf
{
    sse_hpf_etf()
    {
        memset(this, 0, sizeof(sse_hpf_etf));                  // 初始化结构体
    }
    sse_hpf_head                    m_head;                           // 包头
    unsigned int                    m_quote_update_time;              // 行情更新时间,格式如：HHMMSSsss（千分之一秒）HH=00-23, MM=00-59, SS=00-60(秒) ss=000-999(千分之一秒) 如 143025002 表示 14:30:25.002
    char                            m_symbol[SSE_SYMBOL_LEN];         // 合约
    // *******************************************************************************************
    //     证券类型            证券子类型
    //     0            指数   0              指数
    //                         255            未分类
    //     1            股票   0              主板
    //                         1
    //                         2              创业板
    //                         3              B股
    //                         4              H股
    //                         5              科创板
    //                         255            未分类
    //     2            基金   248            LOF基金
    //                         249            ETF基金
    //                         255            未分类
    //     3            债券   248            国债
    //                         249            可转债
    //                         255            未分类
    //     4            回购   248            国债回购
    //                         255            未分类
    //     9            期权   248            股票期权
    //                         249            ETF期权
    //                         255            未分类
    //     10           其他
    // *******************************************************************************************
    unsigned char                             m_security_type;                  // 证券类型
    unsigned char                             m_sub_security_type;              // 证券子类型
    unsigned int                              m_buy_number;                     // ETF 申购笔数，取值范围>=0
    unsigned long long                        m_buy_amount;                     // ETF 申购数量，取值范围>=0
    unsigned long long                        m_buy_money;                      // ETF 申购金额，取值范围>=0
    unsigned int                              m_sell_number;                    // ETF 赎回笔数，取值范围>=0
    unsigned long long                        m_sell_amount;                    // ETF 赎回数量，取值范围>=0
    unsigned long long                        m_sell_money;                     // ETF 赎回金额，取值范围>=0
    unsigned char                             m_reserved_0[15];                 // 保留字段
};

struct sse_static_msg_header
{
    unsigned int                    m_seq_num;                          // 发包的消息序号 从1开始
    unsigned char                   m_reserved[4];                      // 4个字节保留字段
    unsigned char                   m_msg_type;                         // 消息类型
    unsigned short                  m_msg_body_len;                     // 消息体长度
    bool                            m_batch_finish_flag;                // 当前静态文件发送完成标志
};

struct sse_static_msg_body
{
    unsigned char                   m_exchange_id;                             // 交易所id
    char                            m_symbol[SSE_SYMBOL_LEN];                  // 证券代码
    unsigned int                    m_send_time;                               // 行情发送时间，时分秒毫秒
    char                            m_static_file_date[DATE_LEN];              // 静态文件日期，YYYYMMDD, 以'\0'结束
    //涨跌幅限制类型：******************************************************************************************
    // ‘ ’ (十进制数字为32)表示无定义
    // ‘N’表示交易规则（2013修订版）3.4.13规定的有涨跌幅限制类型或者权证管理办法第22条规定
    // ‘R’表示交易规则（2013修订版）3.4.15和3.4.16规定的无涨跌幅限制类型
    // ‘S’表示回购涨跌幅控制类型
    // ‘F’表示基于参考价格的涨跌幅控制
    // ‘P’表示IPO上市首日的涨跌幅控制类型
    // ‘U’表示无任何价格涨跌幅控制类型
    //******************************************************************************************
    char                            m_price_limit_type;                        // 跌涨停限制类型
    double                          m_up_limit_price;                          // 涨停价
    double                          m_down_limit_price;                        // 跌停价
    unsigned long long int          m_bid_qty_unit;                            // 买数量单位
    unsigned long long int          m_ask_qty_unit;                            // 卖数量单位
    unsigned long long int          m_limit_upper_qty;                         // 限价申报数量上限
    unsigned long long int          m_limit_lower_qty;                         // 限价申报数量下限
    double                          m_price_changge_unit;                      // 申报最小变价单位
    unsigned long long int          m_market_upper_qty;                        // 市价申报数量上限
    unsigned long long int          m_market_lower_qty;                        // 市价申报数量下限
    char                            m_security_name[SSE_SECURITY_NAME_LEN];    // 证券名称，以'\0'结束
    // *******************************************************************************************
    //     证券类型            证券子类型
    //     0            指数   0              指数
    //                         255            未分类
    //     1            股票   0              主板
    //                         1
    //                         2              创业板
    //                         3              B股
    //                         4              H股
    //                         5              科创板
    //                         255            未分类
    //     2            基金   248            LOF基金
    //                         249            ETF基金
    //                         255            未分类
    //     3            债券   248            国债
    //                         249            可转债
    //                         255            未分类
    //     4            回购   248            国债回购
    //                         255            未分类
    //     9            期权   248            股票期权
    //                         249            ETF期权
    //                         255            未分类
    //     10           其他
    // *******************************************************************************************
    unsigned char                   m_ssecurity_type;                          // 证券类型
    unsigned char                   m_sub_ssecurity_type;                      // 证券子类型
    char                            m_finance_target_mark;                     // 融资标的标志 ‘ ’ (十进制数字为32)表示无定义,‘T’表示是融资标的证券,‘F’表示不是融资标的证券。
    char                            m_ssecurity_target_mark;                   // 融券标的标志 ‘ ’ (十进制数字为32)表示无定义,‘T’表示是融资标的证券,‘F’表示不是融资标的证券。
    char                            m_product_status[SSE_PRODUCT_STATUS_LEN];  // 产品状态, 以'\0'结束
    char                            m_listing_date[DATE_LEN];                  // 上市日期，格式为YYYYMMDD, 以'\0'结束
};


struct sse_static_msg
{
    sse_static_msg_header           m_header;
    sse_static_msg_body             m_body;
};
#pragma pack(pop)

#define SSE_STATIC_INFO_OK (0)
#define SSE_STATIC_INFO_SYMBOL_IS_INCORRECT (-1)
#define SSE_STATIC_INFO_NOT_FOUND_SYMBOL (-2)
#define SSE_STATIC_INFO_FOUND_SYMBOL_BUT_NO_VALUE (-3)
