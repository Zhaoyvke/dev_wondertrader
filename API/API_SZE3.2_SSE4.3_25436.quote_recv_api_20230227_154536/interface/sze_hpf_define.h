/*!****************************************************************************
@node        Copyright (coffee), 2005-2020, Shengli Tech. Co., Ltd.
@file        sze_hpf_define.h
@date        2020/12/14 08:30
@author      shengli

@brief       本接口定义深交所盛立行情输出结构
******************************************************************************/
#pragma once
#include <string.h>


#define        SZE_EXCHANGE_ID                  (101)                    // 深交所exchange_id
#define        SZE_SYMBOL_LEN                   (9)                      // 深交所合约长度
#define        SZE_LEV2_DEPTH                   (10)                     // 深交所快照深度
#define        SZE_IBR_DEPTH                    (5)                      // 深交所ibr 建树深度

#define        SZE_LEV2_SNAP_MSG_TYPE           (21)                     // 深交所快照消息类型
#define        SZE_LEV2_IDX_MSG_TYPE            (22)                     // 深交所指数消息类型
#define        SZE_LEV2_ORDER_MSG_TYPE          (23)                     // 深交所订单消息类型
#define        SZE_LEV2_EXE_MSG_TYPE            (24)                     // 深交所成交消息类型
#define        SZE_LEV2_AF_CLOSE_MSG_TYPE       (25)                     // 深交所盘后定价消息类型
#define        SZE_LEV2_TREE_MSG_TYPE           (29)                     // 深交所全新快照消息类型
#define        SZE_LEV2_IBR_TREE_MSG_TYPE       (28)                     // 深交所IBR建树快照
#define        SZE_LEV2_TURNOVER_MSG_TYPE       (26)                     // 深交所成交量统计快照
#define        SZE_LEV2_BOND_SNAP_MSG_TYPE      (80)                     // 深交所债券快照消息类型
#define        SZE_LEV2_BOND_ORDER_MSG_TYPE     (81)                     // 深交所债券逐笔委托消息类型
#define        SZE_LEV2_BOND_EXE_MSG_TYPE       (82)                     // 深交所债券逐笔成交消息类型

#define        SZE_LEV2_SNAP_HT_MSG_TYPE        (121)                    // 深交所快照心跳消息类型,api内部消化
#define        SZE_LEV2_IDX_HT_MSG_TYPE         (122)                    // 深交所指数心跳消息类型,api内部消化
#define        SZE_LEV2_TICK_HT_MSG_TYPE        (123)                    // 深交所逐笔消息心跳消息类型,api内部消化
#define        SZE_LEV2_AF_CLOSE_HT_MSG_TYPE    (125)                    // 深交所盘后定价心跳消息类型,api内部消化
#define        SZE_LEV2_TREE_HT_MSG_TYPE        (129)                    // 深交所全新快照心跳消息类型,api内部消化
#define        SZE_LEV2_IBR_TREE_HT_MSG_TYPE    (128)                    // 深交所IBR建树快照心跳消息类型,api内部消化
#define        SZE_LEV2_TURNOVER_HT_MSG_TYPE    (126)                    // 深交所成交量统计快照心跳消息类型,api内部消化
#define        SZE_LEV2_BOND_SNAP_HT_MSG_TYPE   (180)                    // 深交所债券快照心跳消息类型,api内部消化
#define        SZE_LEV2_BOND_TICK_HT_MSG_TYPE   (181)                    // 深交所债券逐笔心跳消息类型,api内部消化


#pragma pack(push, 1)

// 深交所快照档位单元
struct price_quantity_unit
{
    // 限价单和市价单都参与建树,对于建树行情,订单价格取值方法如下表：
    //  ****************************************************************************************************************
    //  |订单类型 \ 买卖方向 |                    买（1）          |                           卖（2）                  |
    //  |     市价（1）      |                    卖一价           |                           买一价                   |
    //  |     限价（2）      |   如果小于 3 的取为 3，否则取原值   |   如果大于 999999997，取为 999999997，否则取原值   |
    //  |    本方最优(U）    |                    买一价           |                           卖一价                   |
    //  *****************************************************************************************************************
    unsigned int                      m_price;                           // 价格，实际值需要除以 10000
    unsigned long long                m_quantity;                        // 数量，实际值需要除以 100
};

// 深交所心跳消息
struct sze_heartbeat
{
    unsigned int                      m_sequence;                        // 盛立行情序号
    unsigned short                    m_reserved_1;                      // 保留字段
    unsigned short                    m_reserved_2;                      // 保留字段
    unsigned char                     m_message_type;                    // 行情类型
    unsigned char                     m_reserved_3[7];                   // 保留字段
};

// 深交所消息协议头
struct sze_hpf_head
{
    unsigned int                      m_sequence;                        // 盛立行情序号
    unsigned short                    m_reserved_1;                      // 保留字段
    unsigned short                    m_reserved_2;                      // 保留字段
    unsigned char                     m_message_type;                    // 行情类型
    //******************************************************************************************
    // 0:指数 1:股票 2:基金 3:债券 4:回购 5:权证 6:期货 7:外汇 8:利率 9:期权 10:其他
    //******************************************************************************************
    unsigned char                     m_security_type;                   // 证券类型
    //******************************************************************************************
    // 当 security_type == 0 时, 恒为0
    // 当 security_type == 1 时，0：主板 1：中小板 2：创业板 3：B股 4：H股 5：科创版 255:未分类
    // 当 security_type == 2 时，248：LOF基金 249：ETF基金 255：未分类
    // 当 security_type == 3 时，248：国债 249：可转债 255：未分类
    // 当 security_type == 4 时，248：国债回购 255：未分类
    // 当 security_type == 5/6/7/8/10时，255:没有意义
    // 当 security_type == 9 时，248：股票期权 249：ETF期权 255：未分类
    //******************************************************************************************
    unsigned char                     m_sub_security_type;               // 证券子类型
    unsigned char                     m_symbol[SZE_SYMBOL_LEN];          // 证券代码
    unsigned char                     m_exchange_id;                     // 交易所编号
    unsigned long long                m_quote_update_time;               // 行情更新时间
    //******************************************************************************************
    //格式如：YYYYMMDDHHMMSSsss（毫秒）
    //YYYY=0000-9999, MM=01-12,    DD=01-31, HH=00-23, MM=00-59, SS=00-60(秒)，sss=000-999(毫秒)。
    //******************************************************************************************
    unsigned short                    m_channel_num;                     // 频道号
    unsigned long long                m_sequence_num;                    // 包序号
    unsigned int                      m_md_stream_id;                    // 行情类别
};

// 深交所快照消息
struct sze_hpf_lev2
{
    sze_hpf_lev2()
    {
        memset(this, 0, sizeof(sze_hpf_lev2));
    }

    sze_hpf_head                      m_head;                            // 包头
    unsigned char                     m_trading_status;                  // 交易状态码
    /******************************************************************************************
     * | Bit 位    | 盛立值 | 交易所值 | 描述                                 |
     * | --------- | ------ | -------- | ------------------------------------ |
     * | Bid7-Bid4 | 0      | 'S'      | 表示启动(开市前)                     |
     * |           | 1      | 'O'      | 表示开盘集合竞价                     |
     * |           | 2      | 'T'      | 表示连续交易                         |
     * |           | 3      | 'B'      | 表示休市                             |
     * |           | 4      | 'C'      | 表示收盘集合竞价                     |
     * |           | 5      | 'E'      | 表示已闭市                           |
     * |           | 6      | 'H'      | 表示临时停牌                         |
     * |           | 7      | 'A'      | 表示盘后交易                         |
     * |           | 8      | 'V'      | 表示波动性中断                       |
     * |           | 15     | other    | 表示无意义                           |
     * | Bit3      | 0      | '0'      | 表示此正常状态                       |
     * |           | 1      | '1'      | 表示全天停牌                         |
     * |           | 0      | other    | 表示无意义，当成此产品可正常交易处理 |
     * | Bit2      | 0      |          | 预留位，值为 0                       |
     * | Bit1      | 0      |          | 预留位，值为 0                       |
     * | Bit0      | 0      |          | 预留位，值为 0                       |
    *******************************************************************************************/
    unsigned long long                m_total_trade_num;                 // 成交笔数
    unsigned long long                m_total_quantity;                  // 总成交量，          实际值需要除以 100
    unsigned long long                m_total_value;                     // 总成交额，          实际值需要除以 1000000
    unsigned int                      m_pre_close_price;                 // 昨收价，            实际值需要除以 10000
    unsigned int                      m_last_price;                      // 最新价，            实际值需要除以 10000
    unsigned int                      m_open_price;                      // 开盘价，            实际值需要除以 10000
    unsigned int                      m_day_high_price;                  // 最高价，            实际值需要除以 10000
    unsigned int                      m_day_low_price;                   // 最低价，            实际值需要除以 10000
    unsigned int                      m_today_close_price;               // 收盘价，            实际值需要除以 10000
    unsigned int                      m_total_bid_weighted_avg_price;    // 买方挂单均价，      实际值需要除以 10000
    unsigned long long                m_total_bid_quantity;              // 买总量，            实际值需要除以 100
    unsigned int                      m_total_ask_weighted_avg_price;    // 卖方挂单均价，      实际值需要除以 10000
    unsigned long long                m_total_ask_quantity;              // 卖总量，            实际值需要除以 100
    unsigned int                      m_lpv;                             // 基金 T-1 日净值，   实际值需要除以 10000
    unsigned int                      m_iopv;                            // 基金实时参考净值，  实际值需要除以 10000
    unsigned int                      m_upper_limit_price;               // 涨停价，            实际值需要除以 10000
    unsigned int                      m_low_limit_price;                 // 跌停价，            实际值需要除以 10000
    unsigned int                      m_open_interest;                   // 合约持仓量，        实际值需要除以 10000
    price_quantity_unit               m_bid_unit[SZE_LEV2_DEPTH];        // 买方十档
    price_quantity_unit               m_ask_unit[SZE_LEV2_DEPTH];        // 卖方十档

    /// 当 m_bid_unit[0]=0 时，TotalAskQty 重置为 0；当 Bid_1_Qty=0 时，TotalBidQty 重置为 0。
};

// 深交所指数消息
struct sze_hpf_idx
{
    sze_hpf_idx()
    {
        memset(this, 0, sizeof(sze_hpf_idx));
    }

    sze_hpf_head                      m_head;                            // 包头
    unsigned long long                m_total_trade_num;                 // 成交笔数
    unsigned long long                m_total_quantity;                  // 总成交量，  实际值需要除以 100
    unsigned long long                m_total_value;                     // 总成交额，  实际值需要除以 1000000
    unsigned int                      m_last_price;                      // 最新价，    实际值需要除以 10000
    unsigned int                      m_pre_close_price;                 // 昨收价，    实际值需要除以 10000
    unsigned int                      m_open_price;                      // 开盘价，    实际值需要除以 10000
    unsigned int                      m_day_high_price;                  // 最高价，    实际值需要除以 10000
    unsigned int                      m_day_low_price;                   // 最低价，    实际值需要除以 10000
    unsigned int                      m_today_close_price;               // 收盘价，    实际值需要除以 10000
    char                              m_reserved[5];                     // 保留字段
};

// 深交所订单消息
struct sze_hpf_order
{
    sze_hpf_order()
    {
        memset(this, 0, sizeof(sze_hpf_order));
    }

    sze_hpf_head                      m_head;                            // 包头
    /// 对逐笔委托价格转换说明如下：
    //  ****************************************************************************************************************
    //  |订单类型 \ 买卖方向 |                    买（1）            |                           卖（2）                    |
    //  |     市价（1）      |                    直接透传           |                           直接透传                   |
    //  |    本方最优(U）    |                    直接透传           |                           直接透传                   |
    //  |     限价（2）      |   如果小于 3 的取为 3，否则取原值     |   如果大于 999999997，取为 999999997，否则取原值     |
    //  *****************************************************************************************************************
    unsigned int                      m_order_price;                     // 价格，  实际值需要除以 10000
    unsigned long long                m_order_quantity;                  // 数量，  实际值需要除以 100
    char                              m_side_flag;                       // 方向，‘1’=买 ‘2’=卖
    char                              m_order_type;                      // 订单类型
    //******************************************************************************************
    // '1'=市价  '2'=限价  'U'=本方最优
    //******************************************************************************************
    char                              m_reserved[15];                     // 保留字段
};

// 深交所成交消息
struct sze_hpf_exe
{
    sze_hpf_exe()
    {
        memset(this, 0, sizeof(sze_hpf_exe));
    }

    sze_hpf_head                      m_head;                           // 包头
    long long                         m_trade_buy_num;                  // 买方包序号
    long long                         m_trade_sell_num;                 // 卖方包序号
    unsigned int                      m_trade_price;                    // 价格，   实际值需要除以 10000
    long long                         m_trade_quantity;                 // 数量，   实际值需要除以 100
    char                              m_trade_type;                     // 成交类型
    //******************************************************************************************
    // '4'=撤销  'F'=成交
    //******************************************************************************************
};

// 深交所全新快照消息
struct sze_hpf_tree
{
    sze_hpf_tree()
    {
        memset(this, 0, sizeof(sze_hpf_tree));
    }

    sze_hpf_head                      m_head;                           // 包头
    long long                         m_total_trade_num;                // 成交笔数
    unsigned long long                m_total_quantity;                 // 总成交量，               实际值需要除以 100
    long long                         m_total_value;                    // 总成交额，               实际值需要除以 1000000
    unsigned int                      m_pre_close_price;                // 昨收价，                 实际值需要除以 10000
    unsigned int                      m_last_price;                     // 最新价，                 实际值需要除以 10000
    unsigned int                      m_open_price;                     // 开盘价，                 实际值需要除以 10000
    unsigned int                      m_day_high_price;                 // 最高价，                 实际值需要除以 10000
    unsigned int                      m_day_low_price;                  // 最低价，                 实际值需要除以 10000
    unsigned int                      m_today_close_price;              // 收盘价，                 实际值需要除以 10000
    unsigned int                      m_total_bid_weighted_avg_price;   // 买方挂单均价，           实际值需要除以 10000
    unsigned long long                m_total_bid_quantity;             // 买总量，                 实际值需要除以 100
    unsigned int                      m_total_ask_weighted_avg_price;   // 卖方挂单均价，           实际值需要除以 10000
    unsigned long long                m_total_ask_quantity;             // 卖总量，                 实际值需要除以 100
    unsigned int                      m_upper_limit_price;              // 涨停价，                 实际值需要除以 10000
    unsigned int                      m_low_limit_price;                // 跌停价，                 实际值需要除以 10000
    unsigned long long                m_market_open_total_bid;          // Bid 边市价单挂单总量，   实际值需要除以 100
    unsigned long long                m_market_open_total_ask;          // Ask 边市价单挂单总量，   实际值需要除以 100
    unsigned int                      m_bid_depth;                      // Bid 边总档位数
    unsigned int                      m_ask_depth;                      // Ask 边总档位数
    price_quantity_unit               m_bid_unit[SZE_LEV2_DEPTH];       // 买方十档
    price_quantity_unit               m_ask_unit[SZE_LEV2_DEPTH];       // 卖方十档
    char                              m_ch_gap_flag;                    // 0x01=此消息前逐笔消息有断号发生；0x00=逐笔消息序号连续，正常；
    char                              m_reserved[4];                    // 保留字段
};

// 深交所ibr建树快照消息
struct sze_hpf_ibr_tree
{
    sze_hpf_ibr_tree()
    {
        memset(this, 0, sizeof(sze_hpf_ibr_tree));
    }

    sze_hpf_head                      m_head;                           // 包头
    unsigned int                      m_total_bid_weighted_avg_price;   // 加权平均委托买价格，    实际值需要除以 10000
    unsigned long long                m_total_bid_quantity;             // 委托买入总量，          实际值需要除以 100
    unsigned int                      m_total_ask_weighted_avg_price;   // 加权平均委托卖价格，    实际值需要除以 10000
    unsigned long long                m_total_ask_quantity;             // 委托卖总量，            实际值需要除以 100
    unsigned int                      m_bid_depth;                      // bid边总档位数，据300191和300192计算
    unsigned int                      m_ask_depth;                      // ask边总档位数，据300191和300192计算
    /// 对 IBR 队列来说，买一档、卖一档指的是离当前盘口距离最近的档位，其它档位按照里盘口距离依次排列
    price_quantity_unit               m_bid_unit[SZE_IBR_DEPTH];        // bid各档位价格信息
    price_quantity_unit               m_ask_unit[SZE_IBR_DEPTH];        // ask各档位价格信息
    char                              m_reserved[5];                    // 保留字段
};

// 深交所成交量消息
struct sze_hpf_turnover
{
    sze_hpf_turnover()
    {
        memset(this, 0, sizeof(sze_hpf_turnover));
    }

    sze_hpf_head                      m_head;                           // 包头
    unsigned char                     m_trading_status;                 // 交易状态码
    /******************************************************************************************
     * | Bit 位    | 盛立值 | 交易所值 | 描述                                 |
     * | --------- | ------ | -------- | ------------------------------------ |
     * | Bid7-Bid4 | 0      | 'S'      | 表示启动(开市前)                     |
     * |           | 1      | 'O'      | 表示开盘集合竞价                     |
     * |           | 2      | 'T'      | 表示连续交易                         |
     * |           | 3      | 'B'      | 表示休市                             |
     * |           | 4      | 'C'      | 表示收盘集合竞价                     |
     * |           | 5      | 'E'      | 表示已闭市                           |
     * |           | 6      | 'H'      | 表示临时停牌                         |
     * |           | 7      | 'A'      | 表示盘后交易                         |
     * |           | 8      | 'V'      | 表示波动性中断                       |
     * |           | 15     | other    | 表示无意义                           |
     * | Bit3      | 0      | '0'      | 表示此正常状态                       |
     * |           | 1      | '1'      | 表示全天停牌                         |
     * |           | 0      | other    | 表示无意义，当成此产品可正常交易处理 |
     * | Bit2      | 0      |          | 预留位，值为 0                       |
     * | Bit1      | 0      |          | 预留位，值为 0                       |
     * | Bit0      | 0      |          | 预留位，值为 0                       |
    *******************************************************************************************/
    long long                         m_pre_close_price;                // 昨收价
    // 实盘数据中，字段 m_pre_close_price，所有成交量统计快照的此字段均为 0 值，因此透传即可。
    long long                         m_total_trade_num;                // 成交笔数
    unsigned long long                m_total_quantity;                 // 成交量，    实际值需要除以 100
    unsigned long long                m_total_value;                    // 成交金额，  实际值需要除以 1000000
    unsigned int                      m_stock_num;                      // 统计量指标样本个数
};

// 深交所盘后定价消息
struct sze_hpf_after_close
{
    sze_hpf_after_close()
    {
        memset(this, 0, sizeof(sze_hpf_after_close));
    }

    sze_hpf_head                      m_head;                           // 包头
    unsigned char                     m_trading_status;                 // 交易状态码
    /******************************************************************************************
     * | Bit 位    | 盛立值 | 交易所值 | 描述                                 |
     * | --------- | ------ | -------- | ------------------------------------ |
     * | Bid7-Bid4 | 0      | 'S'      | 表示启动(开市前)                     |
     * |           | 1      | 'O'      | 表示开盘集合竞价                     |
     * |           | 2      | 'T'      | 表示连续交易                         |
     * |           | 3      | 'B'      | 表示休市                             |
     * |           | 4      | 'C'      | 表示收盘集合竞价                     |
     * |           | 5      | 'E'      | 表示已闭市                           |
     * |           | 6      | 'H'      | 表示临时停牌                         |
     * |           | 7      | 'A'      | 表示盘后交易                         |
     * |           | 8      | 'V'      | 表示波动性中断                       |
     * |           | 15     | other    | 表示无意义                           |
     * | Bit3      | 0      | '0'      | 表示此正常状态                       |
     * |           | 1      | '1'      | 表示全天停牌                         |
     * |           | 0      | other    | 表示无意义，当成此产品可正常交易处理 |
     * | Bit2      | 0      |          | 预留位，值为 0                       |
     * | Bit1      | 0      |          | 预留位，值为 0                       |
     * | Bit0      | 0      |          | 预留位，值为 0                       |
    *******************************************************************************************/
    long long                         m_total_trade_num;                // 成交笔数
    unsigned long long                m_total_quantity;                 // 总成交量，实际值需要除以 100
    unsigned long long                m_total_value;                    // 总成交额，实际值需要除以 1000000
    unsigned int                      m_pre_close_price;                // 昨收价，  实际值需要除以 10000
    unsigned int                      m_trade_price;                    // 成交价，  实际值需要除以 10000
    unsigned long long                m_bid_quantity;                   // 买总量，  实际值需要除以 100
    unsigned long long                m_ask_quantity;                   // 卖总量，  实际值需要除以 100
    char                              m_reserved[4];                    // 保留字段
};


// 深交所全新快照消息
struct sze_hpf_bond_snap
{
    sze_hpf_bond_snap()
    {
        memset( this , 0 , sizeof( sze_hpf_bond_snap ) );
    }
    sze_hpf_head                      m_head;                            // 包头
    unsigned char                     m_trading_status;                  // 交易状态码
    /******************************************************************************************
     * | Bit 位    | 盛立值 | 交易所值 | 描述                                 |
     * | --------- | ------ | -------- | ------------------------------------ |
     * | Bid7-Bid4 | 0      | 'S'      | 表示启动(开市前)                     |
     * |           | 1      | 'O'      | 表示开盘集合竞价                     |
     * |           | 2      | 'T'      | 表示连续交易                         |
     * |           | 3      | 'B'      | 表示休市                             |
     * |           | 4      | 'C'      | 表示收盘集合竞价                     |
     * |           | 5      | 'E'      | 表示已闭市                           |
     * |           | 6      | 'H'      | 表示临时停牌                         |
     * |           | 7      | 'A'      | 表示盘后交易                         |
     * |           | 8      | 'V'      | 表示波动性中断                       |
     * |           | 15     | other    | 表示无意义                           |
     * | Bit3      | 0      | '0'      | 表示此正常状态                       |
     * |           | 1      | '1'      | 表示全天停牌                         |
     * |           | 0      | other    | 表示无意义，当成此产品可正常交易处理 |
     * | Bit2      | 0      |          | 预留位，值为 0                       |
     * | Bit1      | 0      |          | 预留位，值为 0                       |
     * | Bit0      | 0      |          | 预留位，值为 0                       |
    *******************************************************************************************/
    unsigned long long                m_total_trade_num;                 // 成交笔数
    unsigned long long                m_total_quantity;                  // 总成交量，          实际值需要除以 100
    unsigned long long                m_total_value;                     // 总成交额，          实际值需要除以 1000000
    unsigned int                      m_pre_close_price;                 // 昨收价，            实际值需要除以 10000
    unsigned int                      m_last_price;                      // 最新价，            实际值需要除以 10000
    unsigned int                      m_open_price;                      // 开盘价，            实际值需要除以 10000
    unsigned int                      m_day_high_price;                  // 最高价，            实际值需要除以 10000
    unsigned int                      m_day_low_price;                   // 最低价，            实际值需要除以 10000
    unsigned int                      m_today_close_price;               // 收盘价，            实际值需要除以 10000
    unsigned int                      m_total_trade_weighted_avg_price;  // 加权平均价，        实际值需要除以 10000
    int                               m_fluctuation_1;                   // 升跌 1,             实际值需要除以 10000
    int                               m_fluctuation_2;                   // 升跌 2,             实际值需要除以 10000
    unsigned int                      m_total_bid_weighted_avg_price;    // 买方挂单均价，      实际值需要除以 10000
    unsigned long long                m_total_bid_quantity;              // 买总量，            实际值需要除以 100
    unsigned int                      m_total_ask_weighted_avg_price;    // 卖方挂单均价，      实际值需要除以 10000
    unsigned long long                m_total_ask_quantity;              // 卖总量，            实际值需要除以 100

    int                               m_weighted_avg_price_BP;           // 加权平均涨跌 BP     实际值需要除以 10000
    unsigned int                      m_weighted_avg_pre_close_price;    // 昨收盘加权平均价    实际值需要除以 10000
    unsigned int                      m_auction_trade_last_price;        // 匹配成交最新价      实际值需要除以 10000
    unsigned char                     m_reserved[4];                     // 保留字段
    unsigned long long                m_auction_volume_trade;            // 匹配成交成交量      实际值需要除以 100
    unsigned long long                m_auction_value_trade;             // 匹配成交成交金额    实际值需要除以 1000000

    price_quantity_unit               m_bid_unit[SZE_LEV2_DEPTH];        // 买方十档
    price_quantity_unit               m_ask_unit[SZE_LEV2_DEPTH];        // 卖方十档
};

// 深交所债券逐笔委托
struct sze_hpf_bond_order
{
    sze_hpf_bond_order()
    {
        memset( this , 0 , sizeof( sze_hpf_bond_order ) );
    }

    sze_hpf_head                      m_head;                            // 包头
    /// 对逐笔委托价格转换说明如下：
    //  ****************************************************************************************************************
    //  |订单类型 \ 买卖方向 |                    买（1）            |                           卖（2）                    |
    //  |     市价（1）      |                    直接透传           |                           直接透传                   |
    //  |    本方最优(U）    |                    直接透传           |                           直接透传                   |
    //  |     限价（2）      |   如果小于 3 的取为 3，否则取原值     |   如果大于 999999997，取为 999999997，否则取原值     |
    //  *****************************************************************************************************************
    unsigned int                      m_price;                           // 价格，  实际值需要除以 10000
    unsigned long long                m_quantity;                        // 数量，  实际值需要除以 100
    char                              m_side;                            // 方向，‘1’=买 ‘2’=卖
    char                              m_order_type;                      // 订单类型
    //******************************************************************************************
    // '1'=市价  '2'=限价  'U'=本方最优
    //******************************************************************************************
    char                              m_reserved[15];                     // 保留字段
};

// 深交所债券逐笔成交
struct sze_hpf_bond_exe
{
    sze_hpf_bond_exe()
    {
        memset( this , 0 , sizeof( sze_hpf_bond_exe ) );
    }
    sze_hpf_head                      m_head;                           // 包头
    long long                         m_buy_num;                        // 买方包序号
    long long                         m_sell_num;                       // 卖方包序号
    unsigned int                      m_price;                          // 价格，   实际值需要除以 10000
    long long                         m_quantity;                       // 数量，   实际值需要除以 100
    char                              m_type;                           // 成交类型
    //******************************************************************************************
    // '4'=撤销  'F'=成交
    //******************************************************************************************
};

#pragma pack(pop)
