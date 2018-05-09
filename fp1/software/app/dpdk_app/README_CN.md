### *`该部分所有的命令必须在虚拟机上以根用户的方式运行`* ###

[Switch to the English version](./README.md)

*`目录列表`*

* **bin/**: 存储编译后的应用程序和用于启动/停止日志的脚本；  
* **example1/**: 存储用例一的源代码；  
* **example2/**: 存储用例二和用例三的源代码；  
* **execute_objs/**: 存储**example1/**和**example2/**目录下源代码编译后的目标文件；  
* **func/**: 存储用于主函数调用的源代码；  
* **func_objs/**: 存储**func/**目录下源代码编译后的目标文件；  
* **include/**: 存储头文件；  

我们提供了用于编译DPDK源代码和用例程序的自动化编译脚本，用户可以通过下面的命令来使用该脚本：

`source build_dpdk_app.sh`

如果使用了自动化编译脚本，可以直接从第5步开始；
如果没有，请从第一步开始。

# 1. 编译DPDK源代码
切换到DPDK源代码目录（默认为**fp1/software/userspace/dpdk_src**），然后参考**README.md**进行DPDK源代码编译。

# 2. 配置编译环境变量
将下面命令中的`VF_DPDK`替换成DPDK源代码的绝对路径（默认为**XXX/fp1/software/userspace/dpdk_src**， *XXX*是**fp1**的绝对路径），然后执行下面的命令：  
`export DPDK_OBJECT_HOME=$VF_DPDK/dpdk-16.04/x86_64-native-linuxapp-gcc`  
`export DPDK_INCLUDE_HOME=$DPDK_OBJECT_HOME/include`  
`export DPDK_LIB_HOME=$DPDK_OBJECT_HOME/lib`  
`export SECUREC_HOME=$VF_DPDK/securec`  
`export SECUREC_INCLUDE_HOME=$SECUREC_HOME/include`  
`export SECUREC_LIB_HOME=$SECUREC_HOME/lib`

# 3. 编译应用程序
切换到`dpdk_app`目录，运行下面的命令：

`make`

上述命令成功执行后，所有可执行二进制文件会生成到**bin/**目录。

# 4. 配置应用程序所需的运行环境变量

`export LD_LIBRARY_PATH=$DPDK_LIB_HOME:$SECUREC_LIB_HOME:$LD_LIBRARY_PATH`

# 5. 运行用例一

`cd bin/`

## 5.1 运行 `ul_get_version` ，打印IP的版本信息
`./ul_get_version`

## 5.2 运行反相器

`./ul_set_data_test -i 0xaa55`  
`./ul_get_data_test`

如果运行成功，将会打印0xffff55aa。

## 5.3 运行加法器

`./ul_set_data_add -i 0x11111111 -i 0x5a5a5a5a`  
`./ul_get_data_add_result`  

如果运行成功，将会打印0x6b6b6b6b。

## 5.4 打印DFX寄存器信息

`./dump_dfx_regs`  

如果运行成功，将会打印DFX寄存器的值。

# 6. 运行用例二
## 6.1 配置大页

`sysctl -w vm.nr_hugepages=8192`

## 6.2 运行`packet_process`
### 6.2.1 准备
第二步和第四步完成后，运行下面的命令：

`cd bin/`

### 6.2.2 开启/停止日志

运行下面的命令，将日志保存到**/var/log/fpga/dpdk.log**：

`sh start_dpdk_log.sh`

运行下面的命令，停止日志：

`sh shut_down_dpdk_log.sh`

### 6.2.3 收发包测试

`./packet_process -p 0 -q 0,1,2,7 -l 512 -n 102400099`  

选择`VF 0`设备的`0, 1, 2, 和 7`队列发送和接收`102400099`个长度为`512`的包，  
上述命令执行成功后，发包和收包线程分别会发送和接收`102400099`个包。

|    参数   |        描述                              |  
| --------- | ---------------------------------------- |  
| -d xxx    | xxx: 队列深度，数值必须是 **1024**, **2048**, **4096**, 或者 **8192**，默认为 **8192** |  
| -p xxx    | xxx: 端口号，逻辑只支持 **vf0** |  
| -q xxx    | xxx: 队列号，数值范围必须是[0, 7]，默认为 **0** |  
| -l xxx    | xxx: 收发包测试每个包的长度(取值范围为[64, 1048576]，默认为 **64**) |  
| -n xxx    | xxx: 收发包测试包的个数(默认为**128**，最大为**4294966271**) |  
| -x xxx    | xxx: 收发包测试的次数(取值范围为[1, 64511]，默认为 **1**) |  
| -f        | 该参数用于启动FMMU特性(默认关闭, 仅支持用例三) |  
| -h        | 该参数打印帮助信息  |  

## 6.3 运行 DDR Checker
该命令仅支持用例二，打印0x5a5a5a5a。

`./ul_write_ddr_data -n 0 -a 0x1000 -d 0x5a5a5a5a`  
`./ul_read_ddr_data -n 0 -a 0x1000`  

# 7. 运行用例三
## 7.1 配置大页

`sysctl -w vm.nr_hugepages=8192`  

## 7.2 运行`packet_process`
### 7.2.1 准备
第二步和第四步完成后，运行下面的命令： 

`cd bin/`  

### 7.2.2 开启/停止日志

运行下面的命令，将日志保存到**/var/log/fpga/dpdk.log**：

`sh start_dpdk_log.sh`

运行下面的命令，停止日志：

`sh shut_down_dpdk_log.sh`

### 7.2.3 收发包测试

`./packet_process -p 0 -q 0,1,2,7 -l 512 -n 102400099 -f`  

选择`VF 0`设备的`0, 1, 2, 和 7`队列发送和接收`102400099`个长度为`512`的包，  
上述命令执行成功后，发包和收包线程分别会发送和接收`102400099`个包。

|    参数   |        描述                              |  
| --------- | ---------------------------------------- |  
| -d xxx    | xxx: 队列深度，数值必须是 **1024**，**2048**，**4096**，或者 **8192**，默认为 **8192** |  
| -p xxx    | xxx: 端口号，逻辑只支持 **vf0** |  
| -q xxx    | xxx: 队列号，数值范围必须是 [0, 7]，默认为 **0** |  
| -l xxx    | xxx: 收发包测试每个包的长度(取值范围为 [64, 1048576]，默认为 **64**) |  
| -n xxx    | xxx: 收发包测试包的个数(默认为**128**，最大为**4294966271**) |  
| -x xxx    | xxx: 收发包测试的次数(取值范围为[1, 64511]，默认为 **1**) |  
| -f        | 该参数用于启动FMMU特性(默认关闭, 仅支持用例三) |  
| -h        | 该参数打印帮助信息  |  

