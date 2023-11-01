/**
 * @file         ama_test.java
 * @breif        TODO
 * @mail         guoguangkui@archforce.com.cn
 * @created time Mon 27 Nov 2017 02:33:53 PM CST
 */

package demo.ama;
import java.text.SimpleDateFormat;
import java.util.*;
import java.io.*;
import sun.misc.*;
import com.archforce.amd.ama.*;
import java.math.BigInteger;


public class AmaDemo {

    public static volatile boolean isRunning = true;

    static
    {
        System.loadLibrary("ama_java");
    }

    //日志打印
    static void log(int level, String title, String log) {

        String levelString = new String();
        if(level == LogLevel.kTrace) levelString = " Trace ";
        else if(level == LogLevel.kDebug) levelString = " Debug ";
        else if(level == LogLevel.kInfo) levelString = " Info ";
        else if(level == LogLevel.kWarn) levelString = " Warn ";
        else if(level == LogLevel.kError) levelString = " Error ";
        else if(level == LogLevel.kFatal) levelString = " Fatal ";

        SimpleDateFormat df = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSS");

        System.out.println(df.format(new Date()) + levelString + "| " + title + " | "+ log);
    }

    //代码表请求代码示例
    static public long getCodeTable() {
        CodeTableRecordList recordlist = new CodeTableRecordList();
        boolean result = IAMDApi.getCodeTableList(recordlist);
        if (result) {
            long listNums = recordlist.getListNums();
            CodeTableRecord records = recordlist.getRecords();
            for (int i = 0; i < listNums; ++i) {
                CodeTableRecord record = Tools.getDataByIndex(records, i);
                /* TODO */
                //获取到代码表信息后，可以根据需求对代码表信息进行处理
            }

            //records使用完后需要手动释放内存
            Tools.freeMemory(records);
            return listNums;
        }
        return 0;
    }

    //etf 代码表请求代码示例
    static public long getETFCodeTable() {
        ETFCodeTableRecordList recordlist = new ETFCodeTableRecordList();
        ETFItem etfItems = Tools.createETFItem(2);

        //查询深圳市场所有代码
        ETFItem etfItem = new ETFItem();
        etfItem.setMarket(MarketType.kSZSE);
        etfItem.setSecurityCode("");

        Tools.setETFItem(etfItems, 0, etfItem);

        //查询上海市场所有代码
        etfItem = new ETFItem();
        etfItem.setMarket(MarketType.kSSE);
        etfItem.setSecurityCode("");

        Tools.setETFItem(etfItems, 1, etfItem);
        
        boolean result = IAMDApi.getETFCodeTableList(recordlist,etfItems, 2);
        
        if (result) {
            long listNums = recordlist.getEtfListNums();
            ETFCodeTableRecord records = recordlist.getEtfRecords();
            for (int i = 0; i < listNums; ++i) {
                ETFCodeTableRecord record = Tools.getDataByIndex(records, i);
                /* TODO */
                //获取到ETF代码表信息后，可以根据需求对ETF代码表信息进行处理
            }

            //records 使用完后需要手动释放内存
            Tools.freeMemory(records);
            return listNums;
        }
        return 0;
    }

    //国际市场港股汇率数据获取示例
    static public long getIMCExchangeRateData() {
        IMCExchangeRateList list = new IMCExchangeRateList();

        boolean ret = IAMDApi.getIMCExchangeRate(list);
        if(ret){
            long list_num = list.getImcListNums();
            IMCExchangeRate rate_datas = list.getImcRateData();
            for (int i = 0; i < list_num; ++i) {
                IMCExchangeRate rate_data = Tools.getDataByIndex(rate_datas, i);
                /* TODO */
                //获取到国际市场港股汇率数据后，可以根据需求对数据进行处理
                System.out.println(Tools.serialize(rate_data));
            }

            Tools.freeMemory(rate_datas);
            return list_num;
        }

        return 0;
    }

    //RDI数据获取示例
    static public long getRDIData() {
        RDIQueryItem rdiItems = Tools.createRDIQueryItem(2);

        RDIQueryItem rdiItem = new RDIQueryItem();
        //设置查询010011这只代码，如果需要查询全部代码,security_code设置为空("")即可
        rdiItem.setSecurityCode("010011");
        Tools.setRDIQueryItem(rdiItems, 0, rdiItem);

        //设置查询010214这只代码，如果需要查询全部代码,security_code设置为空("")即可
        rdiItem = new RDIQueryItem();
        rdiItem.setSecurityCode("010214");
        Tools.setRDIQueryItem(rdiItems, 1, rdiItem);

        // 获取RDI信息
        /*
        获取债券信息(银行间) BondInfoInterbankList/getBondInfoInterbank
        获取资产支持证券信息(ABS) ABSInfoList/getABSInfo
        获取资产支持证券历史信息(ABS历史) ABSHistoryInfoList/getABSHistoryInfo
        获取资产支持证券信用评级数据 ABSCreditRatingsList/getABSCreditRatings
        获取预发行债券信息数据 PreIssuedBondInfoList/getPreIssuedBondInfo
        获取上市前债券信息(银行间)数据 PreIPOBondInfoList/getPreIPOBondInfo
        获取可交易债券信息数据 XBondTradeBondInfoList/getXBondTradeBondInfo
        获取质押券折算率(匿名点击)信息 PledgedConvertRateACInfoList/getPledgedConvertRateACInfo
        获取X-Repo 分层报价群组信息数据 XRepoHierQuoteGroupInfoList/getXRepoHierQuoteGroupInfo(没有代码信息，接口无item参数)
        获取X-Repo 合约信息数据 XRepoContractInfoList/getXRepoContractInfo
        获取利率互换(固浮)信息数据 SwapFixedFloatInfoList/getSwapFixedFloatInfo
        获取利率互换(固浮)期差基差合约信息数据 SwapFixedFloatBasisContractInfoList/getSwapFixedFloatBasisContractInfo
        */

        BondInfoInterbankList recordlist = new BondInfoInterbankList();
        boolean result = IAMDApi.getBondInfoInterbank(recordlist,rdiItems, 2);

        if (result) {
            long listNums = recordlist.getDataCnt();
            MDBondInfoInterbank records = recordlist.getData();
            for (long i = 0; i < listNums; ++i) {
                MDBondInfoInterbank record = Tools.getDataByIndex(records, i);
                
                /* TODO */
                //获取到RDI数据后，可以根据需求对数据进行处理
                System.out.println(Tools.serialize(record));
            }
            Tools.freeMemory(records);
            return listNums;
        }

        return 0;
    }

    //数据权限订阅示例代码
    static public void subscribeWithDataAuth() {
        /*
            按照权限订阅信息设置:
            1. 订阅信息分三个维度 Market:市场, flag:数据类型(比如现货快照, 逐笔成交, 指数快照等), 证券代码
            2. 订阅操作有三种:
                kSet 设置订阅, 新的订阅会覆盖之前的所有订阅信息
                kAdd 增加订阅, 在前一个基础上增加订阅信息
                kDel 删除订阅, 在前一个基础上删除订阅信息
                kCancelAll 取消所有订阅信息
        */

        //创建存储订阅代码信息的item 数组空间
        SubscribeItem subItems = Tools.createSubscribeItem(2);

        //订阅深圳市场所有代码的快照和逐笔委托
        SubscribeItem subItem = new SubscribeItem();
        subItem.setMarket(MarketType.kSZSE);
        int flag = SubscribeDataType.kSnapshot.intValue() | SubscribeDataType.kTickOrder.intValue();
        BigInteger Iflag = new BigInteger(String.valueOf(flag));
        subItem.setFlag(Iflag);
        subItem.setSecurityCode("");

        //设置item 数组空间的第0位的item值
        Tools.setSubscribeItem(subItems, 0, subItem);

        //订阅上交所600000单只证券的全部数据
        subItem = new SubscribeItem();
        subItem.setMarket(MarketType.kSSE);
        subItem.setFlag(SubscribeDataType.kNone);
        subItem.setSecurityCode("600000");

        //设置item 数组空间的第1位的item值
        Tools.setSubscribeItem(subItems, 1, subItem);

        // 发起订阅
        int ret = IAMDApi.subscribeData(SubscribeType.kSet, subItems, 2);
        if(ret == ErrorCode.kSuccess) {
            log(LogLevel.kInfo, "subscribeWithDataAuth", "subscribe with data_auth success");
        }
        else {
            log(LogLevel.kError, "subscribeWithDataAuth", "subscribe with data_auth failed");
        }

        //订阅调用完后释放item空间
        Tools.destroySubscribeItem(subItems);
    }

    //品种订阅示例代码
    static public void subscribeWithCategory() {
        /*
            按品种类型订阅信息设置:
            1. 订阅信息分三个维度 market:市场, data_type:证券数据类型(), category_type:品种类型, security_code:证券代码
            2. 订阅操作有三种:
                kSet 设置订阅, 新的订阅会覆盖之前的所有订阅信息
                kAdd 增加订阅, 在前一个基础上增加订阅信息
                kDel 删除订阅, 在前一个基础上删除订阅信息
                kCancelAll 取消所有订阅信息
        */

        //创建存储订阅代码信息的item 数组空间
        SubscribeCategoryItem subItems = Tools.createSubscribeCategoryItem(2);

        //增加订阅深圳市场所有代码的股票快照
        SubscribeCategoryItem subItem = new SubscribeCategoryItem();
        subItem.setMarket(MarketType.kSZSE);
        subItem.setDataType(SubscribeSecuDataType.kSnapshot);
        subItem.setCategoryType(SubscribeCategoryType.kStock);
        subItem.setSecurityCode("");

        //设置item 数组空间的第0位的item值
        Tools.setSubscribeCategoryItem(subItems, 0, subItem);

        //增加订阅上海所有指数品种的数据
        subItem = new SubscribeCategoryItem();
        subItem.setMarket(MarketType.kSSE);
        subItem.setDataType(SubscribeSecuDataType.kNone);
        subItem.setCategoryType(SubscribeCategoryType.kIndex);
        subItem.setSecurityCode("");

        //设置item 数组空间的第1位的item值
        Tools.setSubscribeCategoryItem(subItems, 1, subItem);

        //发起订阅
        int ret = IAMDApi.subscribeData(SubscribeType.kAdd, subItems, 2);
        if(ret == ErrorCode.kSuccess) {
            log(LogLevel.kInfo, "subscribeWithCategory", "subscribe with category success");
        }
        else {
            log(LogLevel.kError, "subscribeWithCategory", "subscribe with category failed");
        }

        //订阅调用完后释放item空间
        Tools.destroySubscribeCategoryItem(subItems);
    }

    // 委托簿数据订阅示例
    static public void subscribeOrderBook() {
        SubscribeOrderBookItem orderItems = Tools.createSubscribeOrderBookItem(2);

        //订阅上交所"600000"证券代码的委托簿数据
        SubscribeOrderBookItem orderItem = new SubscribeOrderBookItem();
        orderItem.setMarket(MarketType.kSSE);
        orderItem.setFlag(SubscribeOrderBookDataType.kOrderBook);
        orderItem.setSecurityCode("600000");
        Tools.setSubscribeOrderBookItem(orderItems, 0, orderItem);

        //订阅深交所"000001"证券的委托簿数据 & 委托簿快照数据 
        orderItem = new SubscribeOrderBookItem();
        orderItem.setMarket(MarketType.kSZSE);
        orderItem.setFlag(SubscribeOrderBookDataType.kNone);
        orderItem.setSecurityCode("000001");

        Tools.setSubscribeOrderBookItem(orderItems, 1, orderItem);

        /* 发起订阅 */
        int ret = IAMDApi.subscribeOrderBookData(SubscribeType.kSet, orderItems, 2);
        if(ret == ErrorCode.kSuccess) {
            log(LogLevel.kInfo, "subscribeOrderBook", "subscribe order_book success");
        }
        else {
            log(LogLevel.kError, "subscribeOrderBook", "subscribe order_book failed");
        }

        Tools.destroySubscribeOrderBookItem(orderItems);
    }

    // 行情衍生数据订阅示例
    static public void subscribeDerivedData() {
        
        //创建存储订阅代码信息的item 数组空间
        SubscribeDerivedDataItem derivedItems = Tools.createSubscribeDerivedDataItem(2);

        //订阅上交所"510020"证券代码
        SubscribeDerivedDataItem derivedItem = new SubscribeDerivedDataItem();
        derivedItem.setMarket(MarketType.kSSE);
        derivedItem.setSecurityCode("510020");
        
        //设置item 数组空间的第0位的item值
        Tools.setSubscribeDerivedDataItem(derivedItems, 0, derivedItem);

        //订阅深交所"159930"证券代码
        derivedItem = new SubscribeDerivedDataItem();
        derivedItem.setMarket(MarketType.kSZSE);
        derivedItem.setSecurityCode("159930");
        
        //设置item 数组空间的第1位的item值
        Tools.setSubscribeDerivedDataItem(derivedItems, 1, derivedItem);

        // 发起订阅 订阅IOPV快照数据类型
        int ret = IAMDApi.subscribeDerivedData(SubscribeType.kSet, SubscribeDerivedDataType.kIOPVSnapshot, derivedItems, 2);
        if(ret == ErrorCode.kSuccess) {
            log(LogLevel.kInfo, "subscribeDerivedData", "subscribe derived data success");
        }
        else {
            log(LogLevel.kError, "subscribeDerivedData", "subscribe derived data failed");
        }

        //订阅调用完后释放item空间
        Tools.destroySubscribeDerivedDataItem(derivedItems);
    }

    public static void main(String[] args) {

        SignalHandler handler = new SignalHandler() {
        
            public void handle(Signal sig) {
                isRunning = false; 
            }
        };

        Signal.handle(new Signal("INT"), handler);
        Signal.handle(new Signal("TERM"), handler);

        //设置初始化信息
        Cfg cfg = new Cfg();
        /*
            通道模式设置及各个通道说明:
            cfg.setChannelMode(ChannelMode.kTCP);    //TCP 方式计入上游行情系统
            cfg.setChannelMode(ChannelMode.kAMI);    //AMI 组播方式接入上游行情系统
            cfg.setChannelMode(ChannelMode.kRDMA);   //开启硬件加速RDMA通道,抓取网卡数据包数据
            cfg.setChannelMode(ChannelMode.kEXA);    //开启硬件加速EXA通道,抓取网卡数据包数据
            cfg.setChannelMode(ChannelMode.kPCAP);   //开启硬件加速PCAP通道,抓取网卡数据包数据
            cfg.setChannelMode(ChannelMode.kMDDP);   //直接接入交易所网关组播数据，现在只有深圳交易所开通了此服务
            cfg.setChannelMode(ChannelMode.kFPGA);   //FPGA接入数据
            
            int mode = ChannelMode.kTCP.intValue()|ChannelMode.kAMI.intValue();
            BigInteger IMode = new BigInteger(String.valueOf(mode));
            cfg.setChannelMode(IMode);   //同时通过TCP方式和AMI组播方式接入上游，通过cfg.ha_mode 设置对应的高可用设置模式
        */
        cfg.setChannelMode(ChannelMode.kTCP);

        cfg.setTcpCompressMode(0); //TCP传输数据方式: 0 不压缩 1 华锐自定义压缩 2 zstd压缩(仅TCP模式有效)

        /*
            通道高可用模式设置
            1. ChannelMode 为单通道时,建议设置值为kMasterSlaveA / kMasterSlaveB
            2. ChannelMode 混合开启多个通道时,根据需求设置不同的值
                1) 如果需要多个通道为多活模式接入,请设置kRegularDataFilter值
                2) 如果需要多个通道互为主备接入，请设置值为kMasterSlaveA / kMasterSlaveB,kMasterSlaveA / kMasterSlaveB 差别请参看注释说明
                通道优先级从高到低依次为 kRDMA/kEXA/kMDDP/kAMI/kTCP/kPCAP
        */
        cfg.setHaMode(HighAvailableMode.kMasterSlaveA);

        cfg.setMinLogLevel(LogLevel.kInfo); // 设置日志最小级别：Info级, AMA内部日志通过 OnLog 回调函数返回
        
        /*
            设置是否输出监控数据: true(是), false(否), 监控数据通过OnIndicator 回调函数返回
            监控数据格式为json, 主要监控数据包括订阅信息，数据接收数量统计
            数据量统计:包括接收数量和成功下发的数量统计,两者差值为过滤的数据量统计
            eg: "RecvSnapshot": "5926", "SuccessSnapshot": "5925",表示接收了快照数据5926个,成功下发5925个，过滤数据为 5926 - 5925 = 1 个
                过滤的数据有可能为重复数据或者非订阅数据
        */
        cfg.setIsOutputMonData(false);

        /*
            设置逐笔保序开关: true(开启保序功能) , false(关闭保序功能)
            主要校验逐笔成交数据和逐笔委托数据是否有丢失,如果丢失会有告警日志,缓存逐笔数据并等待KeepOrderTimeout(单位ms)时间等待上游数据重传,
            如果超过此时间,直接下发缓存数据,默认数据已经丢失,如果之后丢失数据过来也会丢弃。
            同时由于深圳和上海交易所都是通道内序号连续,如果打开了保序开关,必须订阅全部代码的逐笔数据,否则一部分序号会被订阅过滤,导致数据超时等待以及丢失告警。
        */
        cfg.setKeepOrder(false);
        cfg.setKeepOrderTimeoutMs(3000);

        cfg.setIsSubscribeFull(false); //设置默认订阅: true 代表默认全部订阅, false 代表默认全部不订阅

        /*
        委托簿千档行情参数设置(仅委托簿版本API有效，若非委托簿版本参数设置无效):
            1) 行情输出设置，包括档位数量以及每档对应的委托队列数量
            2）委托簿计算输出参数设置，包括线程数量以及递交间隔设置
        */
        cfg.setEnableOrderBook(OrderBookType.kNone);        //是否开启委托簿计算功能
        cfg.setEntrySize(10);                               //委托簿输出档位数量
        cfg.setThreadNum((short)3);                         //每个档位输出的委托队列揭示
        cfg.setOrderQueueSize((short)10);                   //计算委托簿的线程数量
        cfg.setOrderBookDeliverIntervalMicrosecond(0);      //递交的最小时间间隔(微妙)，默认10ms

        /*
            配置UMS信息:
            username/password 账户名/密码, 一个账户只能保持一个连接接入 （注意: 如果需要使用委托簿功能，注意账号需要有委托簿功能权限）
            ums地址配置:
                1) ums地址可以配置1-8个 建议值为2 互为主备, UmsServerCnt 为此次配置UMS地址的个数
                2) UMSItem 为UMS地址信息数据结构:
                    LocalIp 为本地地址,填0.0.0.0 或者本机ip
                    ServerIp 为ums服务端地址
                    ServerPort 为ums服务端端口
        */
        cfg.setUsername("amatest");
        cfg.setPassword("123456");

        UMSItem item1 = new UMSItem();
        item1.setLocalIp("0.0.0.0");
        item1.setServerIp("10.129.1.21");
        item1.setServerPort(8200);

        UMSItem item2 = new UMSItem();
        item2.setLocalIp("0.0.0.0");
        item2.setServerIp("192.168.102.102");
        item2.setServerPort(6001);

        cfg.setUmsServerCnt(2);
        Tools.setUMSServers(cfg.getUmsServers(), 0, item1);
        Tools.setUMSServers(cfg.getUmsServers(), 1, item2);

        /*
            业务数据回调接口(不包括 OnIndicator/OnLog等功能数据回调)的线程安全模式设置:
            true: 所有的业务数据接口为接口集线程安全
            false: 业务接口单接口为线程安全,接口集非线程安全
        */
        cfg.setIsThreadSafe(false);
        
        IAMDSpiApp spi = new IAMDSpiApp();  //初始化继承基类IAMDSpi的回放数据回调类
        /*
            初始化回调以及配置信息,此函数为异步函数, 如果通道初始化以及登陆出现问题会通过onLog / onEvent 返回初始化结果信息
        */
        final int ret = IAMDApi.init(spi, cfg);
        if(ret == ErrorCode.kSuccess) {
            log(LogLevel.kInfo, "AmaDemo", "!!!Init ama successfully!!!");
            /*TODO*/
            //初始化成功后可以发起订阅/请求代码表，可以根据需求调用和修改示例函数
            try {
                /* 保持主线程不退出 */
                while(isRunning) {
                    Thread.sleep(2000);
                }
            } catch (InterruptedException e) {
                e.printStackTrace(); 
            }

        } else {
            log(LogLevel.kInfo, "AmaDemo", "Init ama failed");
        }

        IAMDApi.release();
        IAMDSpiApp t = spi;
        return;
    }
}
