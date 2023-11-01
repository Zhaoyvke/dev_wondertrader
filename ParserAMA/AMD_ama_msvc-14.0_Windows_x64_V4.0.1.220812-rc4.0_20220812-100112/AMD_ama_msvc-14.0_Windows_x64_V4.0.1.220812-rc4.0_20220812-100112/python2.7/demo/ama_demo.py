#!/usr/bin/env python
# coding=utf-8

import sys
sys.path.append('../')
import signal
import os
import ama
import argparse
import time,datetime
import json
from IAMDSpiApp import IAMDSpiApp

is_exit_flag = False
spi = IAMDSpiApp()

def Init():
    cfg = ama.Cfg()
    '''
    通道模式设置及各个通道说明:
    cfg.channel_mode = ama.ChannelMode.kTCP   #TCP 方式计入上游行情系统
    cfg.channel_mode = ama.ChannelMode.kAMI   #AMI 组播方式接入上游行情系统
    cfg.channel_mode = ama.ChannelMode.kRDMA  #开启硬件加速RDMA通道,抓取网卡数据包数据
    cfg.channel_mode = ama.ChannelMode.kEXA   #开启硬件加速EXA通道,抓取网卡数据包数据
    cfg.channel_mode = ama.ChannelMode.kPCAP  #开启硬件加速PCAP通道,抓取网卡数据包数据
    cfg.channel_mode = ama.ChannelMode.kMDDP  #直接接入交易所网关组播数据，现在只有深圳交易所开通了此服务
    cfg.channel_mode = ama.ChannelMode.kFPGA  #FPGA接入数据
    cfg.channel_mode = ama.ChannelMode.kTCP | ama.ChannelMode.kAMI #同时通过TCP方式和AMI组播方式接入上游，通过cfg.ha_mode 设置对应的高可用设置模式
    '''
    cfg.channel_mode = ama.ChannelMode.kTCP #TCP 方式计入上游行情系统

    cfg.tcp_compress_mode = 0               #TCP传输数据方式: 0 不压缩 1 华锐自定义压缩 2 zstd压缩(仅TCP模式有效)

    '''
        通道高可用模式设置
        1. cfg.channel_mode 为单通道时,建议设置值为kMasterSlaveA / kMasterSlaveB
        2. cfg.channel_mode 混合开启多个通道时,根据需求设置不同的值
            1) 如果需要多个通道为多活模式接入,请设置kRegularDataFilter值
            2) 如果需要多个通道互为主备接入，请设置值为kMasterSlaveA / kMasterSlaveB,kMasterSlaveA / kMasterSlaveB 差别请参看注释说明
               通道优先级从高到低依次为 kRDMA/kEXA/kMDDP/kAMI/kTCP/kPCAP
    '''
    cfg.ha_mode = ama.HighAvailableMode.kMasterSlaveA

    cfg.min_log_level = ama.LogLevel.kInfo #设置日志最小级别：Info级, AMA内部日志通过 OnLog 回调函数返回

    '''
    设置是否输出监控数据: true(是), false(否), 监控数据通过OnIndicator 回调函数返回
    监控数据格式为json, 主要监控数据包括订阅信息，数据接收数量统计
    数据量统计:包括接收数量和成功下发的数量统计,两者差值为过滤的数据量统计
    eg: "RecvSnapshot": "5926", "SuccessSnapshot": "5925",表示接收了快照数据5926个,成功下发5925个，过滤数据为 5926 - 5925 = 1 个
        过滤的数据有可能为重复数据或者非订阅数据
    '''
    cfg.is_output_mon_data = False

    '''
    设置逐笔保序开关: true(开启保序功能) , false(关闭保序功能)
    主要校验逐笔成交数据和逐笔委托数据是否有丢失,如果丢失会有告警日志,缓存逐笔数据并等待keep_order_timeout_ms(单位ms)时间等待上游数据重传,
    如果超过此时间,直接下发缓存数据,默认数据已经丢失,如果之后丢失数据过来也会丢弃。
    同时由于深圳和上海交易所都是通道内序号连续,如果打开了保序开关,必须订阅全部代码的逐笔数据,否则一部分序号会被订阅过滤,导致数据超时等待以及丢失告警。
    '''
    cfg.keep_order = False
    cfg.keep_order_timeout_ms = 3000

    cfg.is_subscribe_full = False #设置默认订阅: True 代表默认全部订阅, False 代表默认全部不订阅

    '''
    委托簿千档行情参数设置(仅委托簿版本API有效，若非委托簿版本参数设置无效):
        1) 行情输出设置，包括档位数量以及每档对应的委托队列数量
        2）委托簿计算输出参数设置，包括线程数量以及递交间隔设置
    '''
    cfg.enable_order_book = ama.OrderBookType.kNone         #是否开启委托簿计算功能
    cfg.entry_size = 10                                     #委托簿输出档位数量
    cfg.thread_num = 3                                      #每个档位输出的委托队列揭示
    cfg.order_queue_size = 10                               #计算委托簿的线程数量
    cfg.order_book_deliver_interval_microsecond = 0         #递交的最小时间间隔(微妙)，默认10ms

    '''
    配置UMS信息:
    username/password 账户名/密码, 一个账户只能保持一个连接接入 （注意: 如果需要使用委托簿功能，注意账号需要有委托簿功能权限）
    ums地址配置:
        1) ums地址可以配置1-8个 建议值为2 互为主备, ums_server_cnt 为此次配置UMS地址的个数
        2) ums_servers 为UMS地址信息数据结构:
            local_ip 为本地地址,填0.0.0.0 或者本机ip
            server_ip 为ums服务端地址
            server_port 为ums服务端端口
    '''
    cfg.username =  "amatest1"
    cfg.password = "amatest@1231"
    cfg.ums_server_cnt = 2
    item = ama.UMSItem()
    item.local_ip = "0.0.0.0"
    item.server_ip = "10.16.129.251"
    item.server_port = 8200
    ama.Tools.SetUMSServers(cfg.ums_servers, 0, item)
    item1 = ama.UMSItem()
    item1.local_ip = "0.0.0.0"
    item1.server_ip = "10.16.129.252"
    item1.server_port = 8200
    ama.Tools.SetUMSServers(cfg.ums_servers, 1, item1)

    '''
    业务数据回调接口(不包括 OnIndicator/OnLog等功能数据回调)的线程安全模式设置:
        true: 所有的业务数据接口为接口集线程安全
        false: 业务接口单接口为线程安全,接口集非线程安全
    '''
    cfg.is_thread_safe = False

    '''
    初始化回调以及配置信息,此函数为异步函数, 
    如果通道初始化以及登陆出现问题会通过onLog / onEvent 返回初始化结果信息
    '''
    return ama.IAMDApi.Init(spi, cfg)

def Release():
    return ama.IAMDApi.Release()

#数据权限订阅示例代码
def Subscription():
    '''
    按照权限订阅信息设置:
        1. 订阅信息分三个维度 market:市场, flag:数据类型(比如现货快照, 逐笔成交, 指数快照等), 证券代码
        2. 订阅操作有三种:
            kSet 设置订阅, 新的订阅会覆盖之前的所有订阅信息
            kAdd 增加订阅, 在前一个基础上增加订阅信息
            kDel 删除订阅, 在前一个基础上删除订阅信息
            kCancelAll 取消所有订阅信息
    '''

    item = ama.Tools.CreateSubscribeItem(1)                         #创建存储订阅代码信息的item空间
    #订阅深圳市场所有代码的快照
    item.market = ama.MarketType.kSZSE
    item.flag = ama.SubscribeDataType.kSnapshot
    item.security_code = ""
    ama.IAMDApi.SubscribeData(ama.SubscribeType.kSet, item, 1)      #发起订阅
    ama.Tools.DestroySubscribeItem(item)                            #释放item内存

    item = ama.Tools.CreateSubscribeItem(1)                         #创建存储订阅代码信息的item空间
    #订阅上海市场所有代码的逐笔委托
    item.market = ama.MarketType.kSSE
    item.flag = ama.SubscribeDataType.kTickOrder
    item.security_code = ""
    ama.IAMDApi.SubscribeData(ama.SubscribeType.kAdd, item, 1)      #在已有基础上增加订阅
    ama.Tools.DestroySubscribeItem(item)                            #释放item内存

#品种订阅示例代码
def SubscriptionCategory():
    '''
    按品种类型订阅信息设置:
        1. 订阅信息分三个维度 market:市场, data_type:证券数据类型(), category_type:品种类型, security_code:证券代码
        2. 订阅操作有三种:
            kSet 设置订阅, 新的订阅会覆盖之前的所有订阅信息
            kAdd 增加订阅, 在前一个基础上增加订阅信息
            kDel 删除订阅, 在前一个基础上删除订阅信息
            kCancelAll 取消所有订阅信息
    '''
    item = ama.Tools.CreateSubscribeCategoryItem(1)                 #创建存储订阅代码信息的item空间
    #订阅深圳市场所有代码的股票快照
    item.market = ama.MarketType.kSZSE
    item.data_type = ama.SubscribeSecuDataType.kSnapshot
    item.category_type = ama.SubscribeCategoryType.kStock
    item.security_code = "" 
    ama.IAMDApi.SubscribeData(ama.SubscribeType.kSet, item, 1)      #发起订阅
    ama.Tools.DestroySubscribeCategoryItem(item)                    #释放item内存

    item = ama.Tools.CreateSubscribeCategoryItem(1)                 #创建存储订阅代码信息的item空间
    #增加订阅上海所有指数品种的数据
    item.market = ama.MarketType.kSSE
    item.data_type = ama.SubscribeSecuDataType.kNone
    item.category_type = ama.SubscribeCategoryType.kIndex
    item.security_code = ""
    ama.IAMDApi.SubscribeData(ama.SubscribeType.kSet, item, 1)      #重新发起订阅
    ama.Tools.DestroySubscribeCategoryItem(item)                    #释放item内存

#委托簿数据订阅示例
def SubOrderBook():
    #订阅深交所"000001"证券代码的委托簿数据
    order_item = ama.Tools.CreateSubscribeOrderBookItem(1)
    order_item.market = ama.MarketType.kSZSE
    order_item.flag = ama.SubscribeOrderBookDataType.kOrderBook
    order_item.security_code = "000001"
    ama.IAMDApi.SubscribeOrderBookData(ama.SubscribeType.kAdd, order_item, 1)
    ama.Tools.DestroySubscribeOrderBookItem(order_item)
    
    # 订阅上交所600001证券的委托簿数据 & 委托簿快照数据 
    order_item = ama.Tools.CreateSubscribeOrderBookItem(1)
    order_item.market = ama.MarketType.kSSE
    order_item.flag = ama.SubscribeOrderBookDataType.kNone
    order_item.security_code = "600001"
    ama.IAMDApi.SubscribeOrderBookData(ama.SubscribeType.kAdd, order_item, 1)
    ama.Tools.DestroySubscribeOrderBookItem(order_item)

#行情衍生数据订阅示例
def SubDerivedData():
    derived_item = ama.Tools.CreateSubscribeDerivedDataItem(1)  #创建存储订阅代码信息的derived_item空间
    #订阅上交所"510020"证券代码的IOPV快照数据
    derived_item.market = ama.MarketType.kSSE
    derived_item.security_code = "510020"
    ama.IAMDApi.SubscribeDerivedData(ama.SubscribeType.kAdd, ama.SubscribeDerivedDataType.kIOPVSnapshot, derived_item, 1)
    ama.Tools.DestroySubscribeDerivedDataItem(derived_item)     #释放derived_item内存
    
    derived_item = ama.Tools.CreateSubscribeDerivedDataItem(1)  #创建存储订阅代码信息的derived_item空间
    # 订阅深交所159930证券的IOPV快照数据
    derived_item.market = ama.MarketType.kSZSE
    derived_item.security_code = "159930"
    ama.IAMDApi.SubscribeDerivedData(ama.SubscribeType.kAdd, ama.SubscribeDerivedDataType.kIOPVSnapshot, derived_item, 1)
    ama.Tools.DestroySubscribeDerivedDataItem(derived_item)     #释放derived_item内存

#代码表获取示例函数
def GetCodeList():
    code_items = ama.Tools.CreateSubCodeTableItem(2)        #创建存储获取代码表信息的数组空间
    #设置获取深圳市场的所有代码item信息
    code_item_szse = ama.SubCodeTableItem()      
    code_item_szse.market = ama.MarketType.kSZSE                      
    code_item_szse.security_code = ""   
    ama.Tools.SetSubCodeTableItem(code_items, 0, code_item_szse)  #设置code_items数组空间的第0位的值

    #设置获取上海市场的所有代码item信息
    code_item_sse = ama.SubCodeTableItem() 
    code_item_sse.market = ama.MarketType.kSSE                      
    code_item_sse.security_code = ""
    ama.Tools.SetSubCodeTableItem(code_items, 1, code_item_sse) #设置code_items数组空间的第1位的值

    #获取代码表信息
    list = ama.CodeTableRecordList()
    list.list_nums = 0
    if (ama.IAMDApi.GetCodeTableList(list, code_items, 2) == False):
        ama.Tools.DestroySubCodeTableItem(code_items)
        return
    #循环遍历输出代码表信息
    for i in range(list.list_nums): 
        print(ama.Tools.Serialize(ama.Tools.GetDataByIndex(list.records, i)))
    #释放etf代码结构数据内存
    if (list.list_nums > 0):
        ama.Tools.FreeMemory(list.records)
    #对应CreateSubCodeTableItem 释放item
    ama.Tools.DestroySubCodeTableItem(code_items)

#ETF代码表获取示例函数
def GetETFCodeList():
    etf_items = ama.Tools.CreateETFItem(2)                  #创建存储获取ETF代码表信息的数组空间

    etf_item_szse = ama.ETFItem()
    #订阅深圳市场的所有代码
    etf_item_szse.market = ama.MarketType.kSZSE                      
    etf_item_szse.security_code = ""   
    ama.Tools.SetETFItem(etf_items, 0, etf_item_szse)            #设置etf_items数组空间的第0位的etf_item值

    etf_item_sse = ama.ETFItem()
    #订阅上海市场的所有代码
    etf_item_sse.market = ama.MarketType.kSSE                      
    etf_item_sse.security_code = ""
    ama.Tools.SetETFItem(etf_items, 1, etf_item_sse)            #设置etf_items数组空间的第1位的etf_item值

    #获取etf代码表信息
    etf_list = ama.ETFCodeTableRecordList()
    etf_list.etf_list_nums = 0
    if (ama.IAMDApi.GetETFCodeTableList(etf_list ,etf_items, 2) == False):
        ama.Tools.DestroyETFItem(etf_items)
        return
    #循环遍历输出ETF代码表信息
    for i in range(etf_list.etf_list_nums): 
        print(ama.Tools.Serialize(ama.Tools.GetDataByIndex(etf_list.etf_records, i)))
    
    #释放etf代码结构数据内存
    if (etf_list.etf_list_nums > 0):
        ama.Tools.FreeMemory(etf_list.etf_records)
    #对应CreateETFItem 释放item
    ama.Tools.DestroyETFItem(etf_items)

#国际市场港股汇率数据获取示例函数
def GetIMCExchangeRateData():
    list = ama.IMCExchangeRateList()

    if(ama.IAMDApi.GetIMCExchangeRate(list) == False):
        return
    #循环遍历输出国际市场港股汇率数据
    for i in range(list.imc_list_nums): 
        print(ama.Tools.Serialize(ama.Tools.GetDataByIndex(list.imc_rate_data, i)))

    #释放港股汇率数据内存
    if (list.imc_list_nums > 0):
        ama.Tools.FreeMemory(list.imc_rate_data)


#RDI数据获取示例函数
def GetRDICodeList():
    rdi_items = ama.Tools.CreateRDIQueryItem(2)                  #创建存储查询信息的数组空间

    rdi_item = ama.RDIQueryItem()
    #设置查询010011这只代码，如果需要查询全部代码,security_code设置为空("")即可
    rdi_item.security_code = "010011"   
    ama.Tools.SetRDIQueryItem(rdi_items, 0, rdi_item)            #设置rdi_items数组空间的第0位的值

    #设置查询010214这只代码，如果需要查询全部代码,security_code设置为空("")即可
    rdi_item.security_code = "010214"   
    ama.Tools.SetRDIQueryItem(rdi_items, 1, rdi_item)            #设置rdi_items数组空间的第1位的值

    #获取RDI信息
    '''
    获取债券信息(银行间) BondInfoInterbankList/GetBondInfoInterbank
    获取资产支持证券信息(ABS) ABSInfoList/GetABSInfo
    获取资产支持证券历史信息(ABS历史) ABSHistoryInfoList/GetABSHistoryInfo
    获取资产支持证券信用评级数据 ABSCreditRatingsList/GetABSCreditRatings
    获取预发行债券信息数据 PreIssuedBondInfoList/GetPreIssuedBondInfo
    获取上市前债券信息(银行间)数据 PreIPOBondInfoList/GetPreIPOBondInfo
    获取可交易债券信息数据 XBondTradeBondInfoList/GetXBondTradeBondInfo
    获取质押券折算率(匿名点击)信息 PledgedConvertRateACInfoList/GetPledgedConvertRateACInfo
    获取X-Repo 分层报价群组信息数据 XRepoHierQuoteGroupInfoList/GetXRepoHierQuoteGroupInfo(没有代码信息，接口无item参数)
    获取X-Repo 合约信息数据 XRepoContractInfoList/GetXRepoContractInfo
    获取利率互换(固浮)信息数据 SwapFixedFloatInfoList/GetSwapFixedFloatInfo
    获取利率互换(固浮)期差基差合约信息数据 SwapFixedFloatBasisContractInfoList/GetSwapFixedFloatBasisContractInfo
    '''
    rdi_list = ama.BondInfoInterbankList()
    rdi_list.data_cnt = 0
    if (ama.IAMDApi.GetBondInfoInterbank(rdi_list ,rdi_items, 2) == False):
        ama.Tools.DestroyRDIQueryItem(rdi_items)
        return

    #循环遍历输出RDI代码表信息
    for i in range(rdi_list.data_cnt): 
        print(ama.Tools.Serialize(ama.Tools.GetDataByIndex(rdi_list.data, i)))
    #释放RDI获取数据内存
    if (rdi_list.data_cnt > 0):
        ama.Tools.FreeMemory(rdi_list.data)
    #对应CreateRDIQueryItem 释放item
    ama.Tools.DestroyRDIQueryItem(rdi_items)

#获取国际市场汇率数据操作函数示例
def GetIMCExchangeRateList():
    IMC_list = ama.IMCExchangeRateList()
    IMC_list.data_cnt = 0
    if (ama.IAMDApi.GetIMCExchangeRate(IMC_list) == False):
        return

    #循环遍历输出数据信息
    for i in range(IMC_list.data_cnt): 
        print(ama.Tools.Serialize(ama.Tools.GetDataByIndex(IMC_list.data, i)))
    #释放RDI获取数据内存
    if (IMC_list.data_cnt > 0):
        ama.Tools.FreeMemory(IMC_list.data)

def CtrlC(signum, frame):
    global is_exit_flag
    is_exit_flag = True

if(__name__ == "__main__"):
    signal.signal(signal.SIGINT, CtrlC)
    signal.signal(signal.SIGTERM, CtrlC)

    Init()
    Subscription()
    # SubscriptionCategory()
    # SubOrderBook()
    GetCodeList()
    GetETFCodeList()
    GetRDICodeList()
    # GetIMCExchangeRateData()
    # SubDerivedData()
    while(is_exit_flag == False):
        try:
            input()
        except:
            is_exit_flag = True
    Release()


