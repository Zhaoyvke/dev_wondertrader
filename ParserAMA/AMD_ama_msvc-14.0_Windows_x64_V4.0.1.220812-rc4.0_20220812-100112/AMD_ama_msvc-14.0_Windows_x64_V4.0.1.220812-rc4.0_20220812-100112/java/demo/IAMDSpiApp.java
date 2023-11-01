package demo.ama;
import java.util.*;
import java.io.*;
import sun.misc.*;
import com.archforce.amd.ama.*;
import java.math.BigInteger;

//继承IAMDSpi 基类，按照自己的需求重写回调接口
class IAMDSpiApp extends IAMDSpi {
    public IAMDSpiApp() {
        super();
    }

    //@Override 日志回调处理方法
    public void onLog(int level, String log, long len) {
        AmaDemo.log(level, "AMALog", log);
    }

    //@Override 监控回调处理方法
    public void onIndicator(String ind, long len) {
        AmaDemo.log(LogLevel.kInfo, "AMAIndicator", ind);
    }

    //@Override 事件回调处理方法
    public void onEvent(long level, long code, String eventMsg, long len) {
        AmaDemo.log(LogLevel.kInfo, "AMAEvent",
                "Level = " + Tools.getEventLevelString(level) + ", EventCode = " + Tools.getEventCodeString(code) + ", " + eventMsg);
    }

    //@Override 快照回调(股票/基金快照数据)
    public void onMDSnapshot(MDSnapshot snapshots, long cnt) {
        /* 数据取值以及使用范例 全部字段取值方法请参考开发指南对应数据字典说明
        for(long i=0; i<cnt; i++) {
            MDSnapshot snapshot = Tools.getDataByIndex(snapshots, i))); // 获取数组中的数据
            System.out.printf("%s,%d,%d,%d,%d,%d,%d,%d\n",snapshot.getSecurityCode(),
                            snapshot.getOrigTime(),snapshot.getTotalVolumeTrade(),
                            snapshot.getTotalValueTrade(),
                            //数组型档位数据获取参考下述方法,快照数据最多可以获取十档数据，调整档位标识0 ~ 9
                            Tools.getInt64DataByIndex(snapshot.getBidPrice(), 0),    //获取买一价格
                            Tools.getInt64DataByIndex(snapshot.getBidVolume(), 0),   //获取买一委托数量
                            Tools.getInt64DataByIndex(snapshot.getOfferPrice (), 0), //获取卖一价格
                            Tools.getInt64DataByIndex(snapshot.getOfferVolume(), 0)  //获取卖一委托数量
                            );
        }
        Tools.freeMemory(snapshots);
        */
        
        //调用serialize 可以将数据转换成字符串，快速输出
        for(long i=0; i<cnt; i++) {
            MDSnapshot snapshot = Tools.getDataByIndex(snapshots, i);
            System.out.println(Tools.serialize(snapshot));
        }

        /* 手动释放数据内存, 否则会造成内存泄露 */
        Tools.freeMemory(snapshots);
    }

    //@Override 指数快照回调
    public void onMDIndexSnapshot(MDIndexSnapshot snapshots, long cnt) {
        /* 数据取值以及使用范例 全部字段取值方法请参考开发指南对应数据字典说明
        for(long i=0; i<cnt; i++) {
            MDIndexSnapshot data = Tools.getDataByIndex(snapshots, i);                   //获取一个指数快照数据
            System.out.printf("%s,%d\n", data.getSecurityCode(), data.getMarketType());  //get方法取出对应的值
        }
        Tools.freeMemory(snapshots);
        */

        //调用serialize 可以将数据转换成字符串，快速输出
        for(long i=0; i<cnt; i++) {
            System.out.println(Tools.serialize(Tools.getDataByIndex(snapshots, i)));
        }
        /* 手动释放数据内存, 否则会造成内存泄露 */
        Tools.freeMemory(snapshots);
    }

    //@Override 逐笔委托回调
    public void onMDTickOrder(MDTickOrder ticks, long cnt) {
        /* 数据取值以及使用范例 全部字段取值方法请参考开发指南对应数据字典说明
        for(long i=0; i<cnt; i++) {
            MDTickOrder data = Tools.getDataByIndex(ticks, i);                             //获取一个逐笔委托数据
            System.out.printf("%s,%d\n", data.getSecurityCode(), data.getMarketType());    //get方法取出对应的值
        }
        Tools.freeMemory(ticks);
        */

        //调用serialize 可以将数据转换成字符串，快速输出
        for(long i=0; i<cnt; i++) {
            System.out.println(Tools.serialize(Tools.getDataByIndex(ticks, i)));
        }
        /* 手动释放数据内存, 否则会造成内存泄露 */
        Tools.freeMemory(ticks);
    }

    //@Override 逐笔成交回调
    public void onMDTickExecution(MDTickExecution ticks, long cnt) {
        /* 数据取值以及使用范例 全部字段取值方法请参考开发指南对应数据字典说明
        for(long i=0; i<cnt; i++) {
            MDTickExecution data = Tools.getDataByIndex(ticks, i);                          //获取一个逐笔成交数据
            System.out.printf("%s,%d\n", data.getSecurityCode(), data.getMarketType());     //get方法取出对应的值
                                }
            Tools.freeMemory(ticks);
        */

        //调用serialize 可以将数据转换成字符串，快速输出
        for(long i=0; i<cnt; i++) {
            System.out.println(Tools.serialize(Tools.getDataByIndex(ticks, i)));
        }
        /* 手动释放数据内存, 否则会造成内存泄露 */
        Tools.freeMemory(ticks);
    }

    //@Override 委托簿数据回调 无需手动释放内存
    public void onMDOrderBook(MDOrderBook orderBooks, long cnt) {
        /* 数据取值以及使用范例 全部字段取值方法请参考开发指南对应数据字典说明
        for (int i = 0; i < cnt; ++i) {
            MDOrderBook orderBook = Tools.getDataByIndex(orderBooks, i);
            //获取市场、证券代码、最新逐笔序列号\最新逐笔生成时间、最新快照生成时间
            String str = String.valueOf(orderBook.getMarketType()) + "," + String.valueOf(orderBook.getSecurityCode()) + "," +
                            String.valueOf(orderBook.getLastTickSeq()) + "," + String.valueOf(orderBook.getLastTickTime()) + "," +
                            String.valueOf(orderBook.getLastSnapshotTime()) + ",";

            //获取买委托簿，返回的结果是多个MDOrderBookItem组成的集合体，需要通过getDataByIndex获取单个
            MDOrderBookItem bids = orderBook.getBidOrderBook();
            int bid_size = orderBook.getBidOrderBookSize();          //获取买委托簿集合体数量
            for (int j = 0; j < bid_size; ++j) {
                //获取单个买委托簿
                MDOrderBookItem bid = Tools.getDataByIndex(bids, j);  //取出具体买item
            }

            //卖委托簿同上
            MDOrderBookItem offers = orderBook.getOfferOrderBook();
            int offer_size = orderBook.getOfferOrderBookSize();

            for (int j = 0; j < offer_size; ++j) {
                MDOrderBookItem offer =  Tools.getDataByIndex(offers, j);  //取出具体卖item
            }
        }
        */

        for (int i = 0; i < cnt; ++i) {
            MDOrderBook orderBook = Tools.getDataByIndex(orderBooks, i);
            
            //调用serialize 可以将数据转换成字符串，快速输出
            System.out.println(Tools.serialize(orderBook));
        }
        /*不用手动释放数据内存 */
    }

    /*
        TODO 
        可以根据需要重写基类 IAMDSpi 的实时数据回调函数，默认除了释放内存外不进行任何数据处理
    */
}