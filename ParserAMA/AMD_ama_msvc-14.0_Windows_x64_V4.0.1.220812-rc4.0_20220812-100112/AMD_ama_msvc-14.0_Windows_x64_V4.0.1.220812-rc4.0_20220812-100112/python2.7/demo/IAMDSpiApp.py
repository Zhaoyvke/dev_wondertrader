#!/usr/bin/env python
# coding=utf-8

import sys
import signal
import os
import ama
import argparse
import time,datetime
import json

class IAMDSpiApp(ama.IAMDSpi):
    def OnLog(self, level, log, length):
        level_string = "None"
        if(level == ama.LogLevel.kTrace):
            level_string = "Trace"
        elif(level == ama.LogLevel.kDebug):
            level_string = "Debug"
        elif(level == ama.LogLevel.kInfo):
            level_string = "Info"
        elif(level == ama.LogLevel.kWarn):
            level_string = "Warn"
        elif(level == ama.LogLevel.kError):
            level_string = "Error"
        elif(level == ama.LogLevel.kFatal):
            level_string = "Fatal"

        print(datetime.datetime.strftime(datetime.datetime.now(), '%Y-%m-%d %H:%M:%S.%f')[:-3] + ' '+ level_string + ' AmaLog ' + log)

    def OnEvent(self, level, code, eventmsg, length):
        pass

    def OnIndicator(self, ind, length):
        pass

    def OnMDSnapshot(self, snapshots, cnt):
        '''
        #部分数据获取使用示例
        try:
            for i in range(cnt):
                data = ama.Tools.GetDataByIndex(snapshots, i)                  #取出第i个数据
                bidPrice = ama.Tools.GetInt64DataByIndex(data.bid_price, 0)   #取出委托档位买一档价格(最多十档)
                bidVolume = ama.Tools.GetInt64DataByIndex(data.bid_volume, 0) #取出委托档位买一档的委托量(最多十档)
                print("OnMDSnapshot======security_code = %s, orig_time = %d, bid_prce1 = %d" % (data.security_code, data.orig_time, bidPrice))
        except Exception as error:
            print("Get Data Error")
            pass
        '''
        #序列化输出数据
        for i in range(cnt):
            print(ama.Tools.Serialize(ama.Tools.GetDataByIndex(snapshots, i)))
        ama.Tools.FreeMemory(snapshots)

    def OnMDIndexSnapshot(self, snapshots, cnt):
        '''
        #部分数据获取使用示例
        try:
            for i in range(cnt):
                data = ama.Tools.GetDataByIndex(snapshots, i)    #取出第i个数据
                print("OnMDIndexSnapshot======security_code = %s, orig_time = %d, total_value_trade = %d" % (data.security_code, data.orig_time, data.total_value_trade))
        except Exception as error:
            print("Get Data Error")
            pass
        '''

        #序列化输出数据
        for i in range(cnt):
            print(ama.Tools.Serialize(ama.Tools.GetDataByIndex(snapshots, i)))
        ama.Tools.FreeMemory(snapshots)

    def OnMDTickOrder(self, ticks, cnt):
        '''
        #部分数据获取使用示例
        try:
            for i in range(cnt):
                data = ama.Tools.GetDataByIndex(ticks, i)        #取出第i个数据
                print("OnMDTickOrder======security_code = %s, order_time = %d, side = %c" % (data.security_code, data.order_time, data.side))
        except Exception as error:
            print("Get Data Error")
            pass
        '''
        #序列化输出数据
        for i in range(cnt):
            print(ama.Tools.Serialize(ama.Tools.GetDataByIndex(ticks, i)))
        ama.Tools.FreeMemory(ticks)

    def OnMDTickExecution(self, ticks, cnt):
        '''
        #部分数据获取使用示例
        try:
            for i in range(cnt):
                data = ama.Tools.GetDataByIndex(ticks, i)        #取出第i个数据
                print("OnMDTickExecution======security_code = %s, exec_time = %s, exec_price = %d" % (data.security_code, data.exec_time, data.exec_price))
        except Exception as error:
            print("Get Data Error")
            pass
        '''
        #序列化输出数据
        for i in range(cnt):
            print(ama.Tools.Serialize(ama.Tools.GetDataByIndex(ticks, i)))
        
        ama.Tools.FreeMemory(ticks)

    def OnMDOrderBook(self, order_book):
        '''
        #数据获取使用示例
        try:
            cnt = ama.Tools.GetDataSize(order_book)                     #获取委托簿size大小
            for i in range(cnt):
                data = ama.Tools.GetDataByIndex(order_book, i)          #获取第i个委托簿数据
                print("OnMDOrderBook======channel_no = %d, market_type = %s, security_code = %s, last_tick_time = %d, last_snapshot_time= %d, last_tick_seq= %d," 
                % (data.channel_no , data.market_type, data.security_code, data.last_tick_time, data.last_snapshot_time, data.last_tick_seq))
                bid_order_book = data.bid_order_book
                cnt_bid = ama.Tools.GetDataSize(bid_order_book)         #获取买档位数据size大小
                for j in range(cnt_bid):
                    bid_data = ama.Tools.GetDataByIndex(bid_order_book, j)
                    print("bid_order_book======price = %d, volume = %s, order_queue_size = %s" 
                    % (bid_data.price, bid_data.volume, bid_data.order_queue_size))
                    for index in range(50):
                        order_queue = ama.Tools.GetInt64DataByIndex(bid_data.order_queue, index)
                        print("order_queue======%d" % (order_queue))
                offer_order_book = data.offer_order_book                
                cnt_offer = ama.Tools.GetDataSize(bid_order_book)       #获取卖档位数据size大小
                for j in range(cnt_offer):
                    offer_data = ama.Tools.GetDataByIndex(offer_order_book, j)
                    print("offer_order_book======price = %d, volume = %s, order_queue_size = %s" 
                    % (offer_data.price, offer_data.volume, offer_data.order_queue_size))
                    for index in range(50):
                        order_queue = ama.Tools.GetInt64DataByIndex(offer_data.order_queue, index)
                        print("order_queue======%d" % (order_queue))
        except Exception as error:
            print("Get Data Error")
            pass
        '''
        #序列化输出数据
        cnt = ama.Tools.GetDataSize(order_book)
        for i in range(cnt):
            print(ama.Tools.Serialize(ama.Tools.GetDataByIndex(order_book, i)))

    # 港股股票快照数据回调
    def OnMDHKExSnapshot(self, snaps, cnt):
        '''
        #部分数据获取使用示例
        try:
            for i in range(cnt):
                data = ama.Tools.GetDataByIndex(snaps, i)        #取出第i个数据
                print("OnMDHKExSnapshot======security_code = %s" % (data.security_code))
        except Exception as error:
            print("Get Data Error")
            pass
        '''
        #序列化输出数据
        for i in range(cnt):
            print(ama.Tools.Serialize(ama.Tools.GetDataByIndex(snaps, i)))
        
        ama.Tools.FreeMemory(snaps)

    '''
        TODO 
        可以根据需要重写基类 IAMDSpi 的实时数据回调函数，默认除了释放内存外不进行任何数据处理
    '''