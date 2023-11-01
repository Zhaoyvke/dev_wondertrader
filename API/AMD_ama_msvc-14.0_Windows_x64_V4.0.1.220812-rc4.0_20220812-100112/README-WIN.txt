1、目录文件概览
.
├── c++											--- c++版本文件存放目录
│   ├── ama_demo.cpp							--- 样例程序
│   ├── bin                                     --- 已编译好可使用的ama_test，为演示程序，无代码，使用时可直接运行./run.sh，运行前需更改配置文件
│   ├── include									--- 库头文件。使用时可直接 #include<ama.h>
│   ├── lib										--- 库文件,包含库依赖文件
│   ├── project									--- Visual stutio 工程文件 
│   └── run.bat									--- 运行脚本文件
├── java										--- Java版本文件存放目录
│   ├── ama.jar									--- jar包文件	
│   ├── AmaDemo.class
│   ├── AmaDemo$1.class
│   ├── AmaDemo$IAMDSpiApp.class
│   ├── AmaDemo.java							--- java样例源码
│   ├── lib										--- 库文件,包含库依赖文件
│   └── run.bat									--- 运行脚本
├── python2.7									--- python2.7版本文件存放目录
│   ├── ama.py									--- ama的python包
│   ├── ama_demo.py								--- python版本的样例程序
│   ├── lib										--- 库文件,包含库依赖文件
│   └── run.bat									--- 运行脚本
├── python3.6									--- python3.6版本文件存放目录
│   ├── ama.py									--- ama的python包
│   ├── ama_demo.py								--- python版本的样例程序
│   ├── lib										--- 库文件,包含库依赖文件
│   └── run.bat									--- 运行脚本
├── README_WIN.txt									
└── revision									--- 版本信息

2、运行系统环境

Windows 7 及以上

3、 解压开发包
unzip AMD_ama_msvc-14.0_Windows_*.zip

4、C++ demo
ama_demo:该demo为AMA的使用样例程序，用户可以通过该样例程序了解如何使用AMA

编译步骤：
1) 进入到c++目录
cd AMD_ama_msvc-14.0_Windows_*/c++/
2) 建议visual studio工程

备注:以下参数为API的关键参数，使用前需根据实际情况填写，

    // 连接UMS用本地IP地址：不限
    strcpy(cfg.ums_servers[0].local_ip, "0.0.0.0");
	
    // 第一个UMS服务连接信息
    // UMS服务IP地址："192.168.1.101"
    strcpy(cfg.ums_servers[0].server_ip, "192.168.1.101");
    // UMS服务端口号：6001
    cfg.ums_servers[0].server_port = 6001;

    // 第二个UMS服务连接信息
    // 连接UMS用本地IP地址：不限
    strcpy(cfg.ums_servers[1].local_ip, "0.0.0.0");
    // UMS服务IP地址："192.168.1.102"
    strcpy(cfg.ums_servers[1].server_ip, "192.168.1.102");
    // UMS服务端口号：6001
    cfg.ums_servers[1].server_port = 6001;

    // UMS服务连接信息个数：2, UMS地址可以配置1个或者多个
    cfg.ums_server_cnt = 2;
    // 用户名：user1
    strcpy(cfg.username, "user1");
    // 用户密码：pass1234
    strcpy(cfg.password, "pass1234");


5、ama_test
该demo为AMA标准样例程序，支持参数配置，数据选择性输出等。
1) 修改参数

etc/ama.json关键参数更改
"LocalIp": "10.128.0.33",
"UMSInfo":
[
	{
		"ServerIp": "192.168.102.48",
		"ServerPort": 8201
	},
	{
		"ServerIp": "192.168.102.102",
		"ServerPort":6001
	}
],

"Username" : "ldd_udf",
"Password" : "udf123",

2) 运行
./run.bat

6、java版本
进入到java目录。
关键参数更改。同4.
运行 ./run.bat
如果基于ama接口开发程序，编译以及执行请参考run.bat

7、python版本
进入到python目录。
关键参数更改。同4.
运行 ./run.bat
