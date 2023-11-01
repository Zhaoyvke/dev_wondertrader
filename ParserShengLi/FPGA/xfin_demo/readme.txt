1. 编译
在dmeo目录下，执行命令：make
编译完成，将自动创建bin文件夹，可执行程序生成在bin文件夹，同时创建lib文件夹中对应so文件的软连接

执行 make clean 则清除编译生成的文件

2. 执行demo程序
运行demo目录中对应的start脚本：
启动单独接收上交所行情：bash start_sh_demo.sh
启动单独接收深交所行情：bash start_sz_demo.sh
启动同时接收上交所及深交所行情：bash start_sh_sz_demo.sh

3. 配置文件
配置文件位于demo目录下etc文件夹中，具体配置祥见config-sdk.conf

4. demo代码
示例代码位于demo目录下src文件夹中：
单独接收上交所行情示例代码：sh_demo.cpp
单独接收深交所行情示例代码：sz_demo.cpp
同时接收上交所及深交所行情示例代码：sh_sz_demo.cpp

5. 目录结构要求
对于plugins目录有特殊要求：
程序运行时，将在"../plugins"目录中动态加载库文件，
因此，可执行程序所在文件夹应与plugins文件夹同级，
例如可执行文件位于bin文件夹，则bin与plugins应同级，
否则将无法使用加速栈。
对其他目录无特殊要求。