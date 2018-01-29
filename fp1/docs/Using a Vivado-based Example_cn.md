使用基于vivado的Example
=======================
[Switch to the English version](./Using a Vivado-based Example.md)

[使用example1](#a)

[使用example2](#b)

[使用example3](#c)

**说明：**需要了解这三个example功能等信息的用户可查阅[示例应用指南](../hardware/vivado_design/examples/README_CN.md)。

<a name="a"></a>
使用example1
-------

### 功能描述

Example1示例主要实现用户逻辑的版本号读取、数据取反、加法器和打印DFX信息功能。

### 使用Vivado HDK

用户申请FPGA镜像后，登录VM，HDK默认存放在huaweicloud-fpga/fp1目录下，HDK的使用步骤如下。

#### 步骤1 设置Vivado工具License。

用户打开huaweicloud-fpga/fp1/路径下的`setup.cfg`文件，将文件中`XILINX_LIC_SETUP`的值配置为License服务器的IP地址`2100@100.125.1.240:2100@100.125.1.251`。

`XILINX_LIC_SETUP="2100@100.125.1.240:2100@100.125.1.251"`


**说明：** 华为提供的Xilinx软件License仅限root账号使用。

#### 步骤2 配置开发环境。

`cd huaweicloud-fpga/fp1`    
`export HW_FPGA_DIR=$(pwd)`  
`source $HW_FPGA_DIR/setup.sh`  

#### 步骤3 进入example1目录。

`cd $HW_FPGA_DIR/hardware/vivado_design/examples/example1`    
`export UL1_DIR=$(pwd)`  

#### 步骤4 编译example1。

`cd $UL1_DIR/prj`  
`sh ./build.sh`

#### 步骤5 仿真example1。

`cd $UL1_DIR/sim`    
`make TC=sv_demo_001`  

### 使用Vivado SDK
**说明：** 在生成dcp文件后，需要先完成环境配置及SDK的编译（具体可参考根目录下README 2.1.2章节），才能使用SDK。

#### 步骤1 编译example1的SDK。

`cd huaweicloud-fpga/fp1/software/app/dpdk_app/`  
`chmod +x build_dpdk_app.sh`  
`sh build_dpdk_app.sh`  

编译成功后，在当前目录下的`bin/`下生成二进制的可执行文件。

#### 步骤2 设置运行时环境变量。

`cd huaweicloud-fpga/fp1`  
`export SW_FPGA_DIR=$(pwd)`  
`export LD_LIBRARY_PATH=$SW_FPGA_DIR/software/userspace/dpdk_src/dpdk-16.04/x86_64-native-linuxapp-gcc/lib:$SW_FPGA_DIR/software/userspace/dpdk_src/securec/lib:$LD_LIBRARY_PATH`

#### 步骤3 进入APP可执行文件目录。

`cd $SW_FPGA_DIR/software/app/dpdk_app/bin`

#### 步骤4 运行测试命令。

**说明：** 以下步骤的命令都可以通过–h 参数获取帮助信息，例如命令`./ul_get_version -h`。

##### 例子1 打印逻辑的版本号。

`./ul_get_version`

**输出打印结果示例**  （版本号以实际查询出的结果为准）。

	[root@CentOS7 bin]# ./ul_get_version 
	version: 0x20171108

##### 例子2 测试取反寄存器

**1. 设置取反器输入寄存器，输入一个不超过32bit的数据**

运行命令为./ul_set_data_test -i *num*

例如`./ul_set_data_test -i 0xaa55`  

**2. 读取取反器结果寄存器，读取的数值为上一步中输入数值的取反值。**

`./ul_get_data_test`  

**输出打印结果示例** 

	[root@CentOS7 bin]# ./ul_set_data_test -i 0xaa55  
	[root@CentOS7 bin]# ./ul_get_data_test  
	oppos: 0xffff55aa

##### 例子3 测试加法器。

**1. 设置加法器加数寄存器** 

运行命令为./ul_set_data_add -i *augend* -i *addend*

例如`./ul_set_data_add -i 0x11111111 -i 0x5a5a5a5a`  

**2. 读取加法器结果寄存器。**

`./ul_get_data_add_result`  

**输出打印结果示例**  

	[root@CentOS7 bin]# ./ul_set_data_add -i 0x11111111 -i 0x5a5a5a5a
	Set [0x11111111]:[0x5a5a5a5a] to REG_PF_DEMO1_ADDER_CFG_WDATA0:REG_PF_DEMO1_ADDER_CFG_WDATA1 
	[root@CentOS7 bin]# ./ul_get_data_add_result
	add result: 0x6b6b6b6b

##### 例子4 测试打印DFX寄存器。

运行打印DFX寄存器命令。

`./dump_dfx_regs`  

**输出打印结果示例**  （实际结果值以逻辑当前寄存器状态为准）

	[root@CentOS7 bin]# ./dump_dfx_regs 
	 -------- Dump logic regs begin -------- 
		Reg addr      Value         Description
		[0x00018200]: 0x00000000  - txqm: reg_bdqm_err
		[0x00018204]: 0x00000000  - txqm: reg_mulqm_err0
		[0x00018440]: 0x00000010  - txqm: reg_bdqm_sta
		[0x00018608]: 0x00000000  - txqm: reg_r540_w288_c_cnt_en
		[0x0001c204]: 0x00000000  - txm: reg_txm_err
		[0x0001c400]: 0x00000005  - txm: reg_txm_status
		[0x0001c600]: 0x00000000  - txm: reg_ae2txm_req_rgt_cnt
		[0x0001c604]: 0x00000000  - txm: reg_ae2txm_req_err_cnt
		[0x0001c608]: 0x00000000  - txm: reg_txm2ae_tx_cnt
		[0x00024200]: 0x00000000  - rxm: reg_parrlt_err
		[0x00024408]: 0x00000001  - rxm: reg_axi_fifo_sta
		[0x00024628]: 0x00000000  - rxm: reg_axi_dis_cnt
		[0x0002462c]: 0x00000000  - rxm: reg_axi_rc_cnt
	 -------- Dump logic regs end -------- 

<a name="b"></b>
使用example2
------------

### 功能描述

Example2主要实现用户逻辑DMA环回通道和DDR读取功能。

### 使用Vivado HDK

用户申请FPGA镜像后，登录VM，HDK默认存放在huaweicloud-fpga/fp1目录下,HDK的使用步骤如下。

#### 步骤1 设置Vivado工具License。

用户打开huaweicloud-fpga/fp1/路径下的`setup.cfg`文件，将文件中`XILINX_LIC_SETUP`的值配置为License服务器的IP地址`2100@100.125.1.240:2100@100.125.1.251`。

`XILINX_LIC_SETUP="2100@100.125.1.240:2100@100.125.1.251"`

**说明：** 华为提供的Xilinx软件License仅限root账号使用。

#### 步骤2 配置开发环境。

`cd huaweicloud-fpga/fp1`  
`export HW_FPGA_DIR=$(pwd)`  
`source $HW_FPGA_DIR/setup.sh`

#### 步骤3 进入example2目录。

`cd $HW_FPGA_DIR/hardware/vivado_design/examples/example2`  
`export UL2_DIR=$(pwd)`

#### 步骤4 编译example2。

`cd $UL2_DIR/prj`  
`sh ./build.sh`

#### 步骤5 仿真example2。

`cd $UL2_DIR/sim`  
`make TC=sv_demo_001`

### 使用Vivado SDK
**说明：** 在生成dcp文件后，需要先完成环境配置及SDK的编译（具体可参考根目录下README 2.1.2章节），才能使用SDK。

#### example2 SDK测试环境配置

##### 步骤1 编译example2的SDK。

`cd huaweicloud-fpga/fp1/software/app/dpdk_app/`  
`chmod +x build_dpdk_app.sh`  
`sh build_dpdk_app.sh`  

编译成功后，在当前目录下的`bin/`下生成二进制的可执行文件。

##### 步骤2 设置环境变量。

`cd huaweicloud-fpga/fp1`  
`export SW_FPGA_DIR=$(pwd)`  
`export LD_LIBRARY_PATH=$SW_FPGA_DIR/software/userspace/dpdk_src/dpdk-16.04/x86_64-native-linuxapp-gcc/lib:$SW_FPGA_DIR/software/userspace/dpdk_src/securec/lib:$LD_LIBRARY_PATH`

##### 步骤3 进入APP可执行文件目录。

`cd $SW_FPGA_DIR/software/app/dpdk_app/bin`  

##### 步骤4 设置hugepage个数，默认的hugepage大小为2MB，当前配置个数为8192，即hugepage总大小为16GB，请确保VM的大页内存不小于16G。

`sysctl -w vm.nr_hugepages=8192`  


#### SDK DMA环路测试

**说明：** 以下步骤的命令都可以通过附带–h 参数获取帮助信息。

##### 步骤1 执行packet_process进程。

`./packet_process -d 8192 -q 0 -l 512 -n 102400099`  

**本例可用命令参数说明**  

| 参数     | 说明                                       |
| ------ | ---------------------------------------- |
| **-d** | 设置队列深度，有效值是1024、2048、4096、8192，默认值是8192。 |
| **-p** | 指定VF设备号，默认为0。                            |
| **-q** | 指定队列发送：有效值为[0，7]，默认为0，可以通过逗号进行多选（如 -q 0,1,5）。 |
| **-l** | 表示发送包的单包数据长度：有效值为[64，1048576]，默认值为64。    |
| **-n** | 表示发送包的个数：有效值为[1，4294966271]，默认为128。      |
| **-x** | 表示要进行几次循环，用于压力测试用，有效值为[1，64511]，默认为1。    |
| **-h** | 打印帮助信息。                                  |

**输出打印结果示例**  

	[root@CentOS7 bin]# ./packet_process -d 8192 -q 0 -l 512 -n 102400099
	available cpu number: 24, cpu mask parameter: -cffffff
	...
	----------------TEST TIME 0 for port 0----------------
	Not find mempool sec_mp_bd_0_0, create it
	mempool sec_mp_bd_0_0 has 2048 available entries
	Not find mempool sec_mp_data_0_0, create it
	mempool sec_mp_data_0_0 has 4096 available entries
	---------------- test for port 0, queue 0,  ----------------
	PMD: acc_dev_rx_burst()-418: port id: 0, queue id: 0: g_s_rx_total=10000220, g_s_rx_time=718061(us)
	PMD: acc_dev_rx_burst()-418: port id: 0, queue id: 0: g_s_rx_total=10000046, g_s_rx_time=717936(us)
	PMD: acc_dev_rx_burst()-418: port id: 0, queue id: 0: g_s_rx_total=10000532, g_s_rx_time=718003(us)
	PMD: acc_dev_rx_burst()-418: port id: 0, queue id: 0: g_s_rx_total=10000738, g_s_rx_time=718167(us)
	PMD: acc_dev_rx_burst()-418: port id: 0, queue id: 0: g_s_rx_total=10000384, g_s_rx_time=718047(us)
	PMD: acc_dev_rx_burst()-418: port id: 0, queue id: 0: g_s_rx_total=10000011, g_s_rx_time=717839(us)
	PMD: acc_dev_rx_burst()-418: port id: 0, queue id: 0: g_s_rx_total=10000623, g_s_rx_time=762135(us)
	PMD: acc_dev_rx_burst()-418: port id: 0, queue id: 0: g_s_rx_total=10000518, g_s_rx_time=789353(us)
	PMD: acc_dev_rx_burst()-418: port id: 0, queue id: 0: g_s_rx_total=10000384, g_s_rx_time=789451(us)
	PMD: acc_dev_rx_burst()-418: port id: 0, queue id: 0: g_s_rx_total=10000384, g_s_rx_time=789311(us)
	port 0, queue 0 run_business_tx_thread_route finish, time 7627130(us)
	port 0, queue 0 run_business_rx_thread_route finish, time 7627967(us)
	----------------port 0, queue 0 rx_packet_len 512, packet_num: 102400099, performance = 54 gbps----------------
	port 0, queue 0 TX/RX all success, all process 102400099 packets
	...
**说明：** 如上面日志所示，运行成功则打印日志**TX/RX all success**


#### SDK DDR读写测试

当前提供给用户4个16GB 2RX8 DDR，DDR读写测试是通过配置BAR空间寄存器对DDR进行间接读写。

**说明：** 以下步骤的命令都可以通过–h 参数获取帮助信息。

##### 步骤1 设置DDR值。

运行命令为./ul_write_ddr_data -n 0 -a *addr* -d *data*

例如`./ul_write_ddr_data -n 0 -a 0x1000 -d 0x12345678`  

##### 步骤2 读取DDR值。
运行命令为./ul_read_ddr_data -n 0 -a *addr*

例如`./ul_read_ddr_data -n 0 -a 0x1000`  

**输出打印结果示例**  （实际结果值以逻辑当前寄存器状态为准）

	[root@CentOS7 bin]# ./ul_write_ddr_data -n 0 -a 0x1000 -d 0x12345678
	[root@CentOS7 bin]# ./ul_read_ddr_data -n 0 -a 0x1000
	Value: 0x12345678

<a name="c"></c>
使用example3
------------

### 功能描述

Example3主要实现用户逻辑FMMU（Fpga Mermory Manage Unit）功能。

### 使用Vivado HDK

用户申请FPGA镜像后，登录VM，HDK默认存放在`huaweicloud-fpga/fp1`目录下，HDK的使用步骤如下。

#### 步骤1 设置Vivado工具License。

用户打开huaweicloud-fpga/fp1/路径下的`setup.cfg`文件，将文件中XILINX_LIC_SETUP的值配置为License服务器的IP地址`2100@100.125.1.240:2100@100.125.1.251`。

`XILINX_LIC_SETUP="2100@100.125.1.240:2100@100.125.1.251"`

**说明：** 华为提供的Xilinx软件License仅限root账号使用。

#### 步骤2 配置开发环境。

执行以下指令完成硬件开发环境的配置。

`cd huaweicloud-fpga/fp1`  
`export HW_FPGA_DIR=$(pwd)`  
`source $HW_FPGA_DIR/setup.sh`

#### 步骤3 进入example3目录。

`cd $HW_FPGA_DIR/hardware/vivado_design/examples/example3`  
`export UL2_DIR=$(pwd)`

#### 步骤4 编译example3。

`cd $UL2_DIR/prj`  
`sh ./build.sh`

#### 步骤5 仿真example3。

`cd $UL2_DIR/sim`  
`make TC=sv_demo_001`

### 使用Vivado SDK

#### Example3 SDK测试环境配置

**说明：** 在生成dcp文件后，需要先完成环境配置及SDK的编译（具体可参考根目录下README 2.1.2章节），才能使用SDK。

##### 步骤1 编译example3的SDK。

`cd huaweicloud-fpga/fp1/software/app/dpdk_app/`  
`chmod +x build_dpdk_app.sh`  
`sh build_dpdk_app.sh`  

编译成功后，在当前目录下的`bin/`下生成二进制的可执行文件。

##### 步骤2 设置环境变量。

`cd huaweicloud-fpga/fp1`  
`export SW_FPGA_DIR=$(pwd)`  
`export LD_LIBRARY_PATH=$SW_FPGA_DIR/software/userspace/dpdk_src/dpdk-16.04/x86_64-native-linuxapp-gcc/lib:$SW_FPGA_DIR/software/userspace/dpdk_src/securec/lib:$LD_LIBRARY_PATH`

##### 步骤3 进入APP可执行文件目录。

`cd $SW_FPGA_DIR/software/app/dpdk_app/bin`


##### 步骤4 设置hugepage个数，默认的hugepage大小为2MB，当前配置个数为8192，即hugepage总大小为16GB，请确保VM的大页内存不小于16GB。

`sysctl -w vm.nr_hugepages=8192`


#### FMMU测试

**说明：** 以下步骤的命令都可以通过–h 参数获取帮助信息。

##### 步骤1 执行packet_process进程。

`./packet_process -d 8192 -q 0 -l 512 -n 102400099 -f`  

**本例可用命令参数说明**  

| 参数     | 说明                                       |
| ------ | ---------------------------------------- |
| **-d** | 设置队列深度，有效值是1024、2048、4096、8192，默认值是8192。 |
| **-p** | 指定VF设备号，默认为0。                            |
| **-q** | 指定队列发送：有效值为[0，7]，默认为0，可以通过逗号进行多选（如 -q 0,1,5）。 |
| **-l** | 表示发送包的单包数据长度：有效值为[64，1048576]，默认值为64。    |
| **-n** | 表示发送包的个数：有效值为[1，4294966271]，默认为128。      |
| **-f** | 打开FMMU功能，测试FMMU逻辑功能时必须打开此开关，不加该参数则默认为关闭。 |
| **-x** | 表示要进行几次循环，用于压力测试用，有效值为[1，64511]，默认为1。    |
| **-h** | 打印帮助信息。                                  |

**输出打印结果示例** 

	[root@CentOS7 bin]# ./packet_process -d 8192 -q 0 -l 512 -n 102400099 -f
	available cpu number: 24, cpu mask parameter: -cffffff
	...
	----------------TEST TIME 0 for port 0----------------
	Not find mempool sec_mp_bd_0_0, create it
	mempool sec_mp_bd_0_0 has 2048 available entries
	Not find mempool sec_mp_data_0_0, create it
	mempool sec_mp_data_0_0 has 4096 available entries
	---------------- test for port 0, queue 0, [FMMU]  ----------------
	PMD: acc_dev_rx_burst()-418: port id: 0, queue id: 0: g_s_rx_total=10000384, g_s_rx_time=733990(us)
	PMD: acc_dev_rx_burst()-418: port id: 0, queue id: 0: g_s_rx_total=10000384, g_s_rx_time=733935(us)
	PMD: acc_dev_rx_burst()-418: port id: 0, queue id: 0: g_s_rx_total=10000384, g_s_rx_time=740312(us)
	PMD: acc_dev_rx_burst()-418: port id: 0, queue id: 0: g_s_rx_total=10000279, g_s_rx_time=764110(us)
	PMD: acc_dev_rx_burst()-418: port id: 0, queue id: 0: g_s_rx_total=10000053, g_s_rx_time=764071(us)
	PMD: acc_dev_rx_burst()-418: port id: 0, queue id: 0: g_s_rx_total=10000129, g_s_rx_time=764130(us)
	PMD: acc_dev_rx_burst()-418: port id: 0, queue id: 0: g_s_rx_total=10000489, g_s_rx_time=764277(us)
	PMD: acc_dev_rx_burst()-418: port id: 0, queue id: 0: g_s_rx_total=10000970, g_s_rx_time=764283(us)
	PMD: acc_dev_rx_burst()-418: port id: 0, queue id: 0: g_s_rx_total=10000384, g_s_rx_time=764199(us)
	PMD: acc_dev_rx_burst()-418: port id: 0, queue id: 0: g_s_rx_total=10000031, g_s_rx_time=764029(us)
	port 0, queue 0 run_business_tx_thread_route finish, time 7740224(us)
	port 0, queue 0 run_business_rx_thread_route finish, time 7740980(us)
	----------------port 0, queue 0 rx_packet_len 544, packet_num: 102400099, performance = 57 gbps----------------
	port 0, queue 0 TX/RX all success, all process 102400099 packets
	...
=======
使用基于vivado的Example
=======================
[Switch to the English version](./Using a Vivado-based Example.md)

[使用example1](#a)

[使用example2](#b)

[使用example3](#c)

**说明：**需要了解这三个example功能等信息的用户可查阅[示例应用指南](../hardware/vivado_design/examples/README_CN.md)。

<a name="a"></a>
使用example1
-------

### 功能描述

Example1示例主要实现用户逻辑的版本号读取、数据取反、加法器和打印DFX信息功能。

### 使用Vivado HDK

用户申请FPGA镜像后，登录VM，HDK默认存放在huaweicloud-fpga/fp1目录下，HDK的使用步骤如下。

#### 步骤1 设置Vivado工具License。

用户打开huaweicloud-fpga/fp1/路径下的`setup.cfg`文件，将文件中`XILINX_LIC_SETUP`的值配置为License服务器的IP地址`2100@100.125.1.240:2100@100.125.1.251`。

`XILINX_LIC_SETUP="2100@100.125.1.240:2100@100.125.1.251"`


**说明：** 华为提供的Xilinx软件License仅限root账号使用。

#### 步骤2 配置开发环境。

`cd huaweicloud-fpga/fp1`    
`export HW_FPGA_DIR=$(pwd)`  
`source $HW_FPGA_DIR/setup.sh`  

#### 步骤3 进入example1目录。

`cd $HW_FPGA_DIR/hardware/vivado_design/examples/example1`    
`export UL1_DIR=$(pwd)`  

#### 步骤4 编译example1。

`cd $UL1_DIR/prj`  
`sh ./build.sh`

#### 步骤5 仿真example1。

`cd $UL1_DIR/sim`    
`make TC=sv_demo_001`  

### 使用Vivado SDK
**说明：** 在生成dcp文件后，需要先完成环境配置及SDK的编译（具体可参考根目录下README 2.1.2章节），才能使用SDK。

#### 步骤1 编译example1的SDK。

`cd huaweicloud-fpga/fp1/software/app/dpdk_app/`  
`chmod +x build_dpdk_app.sh`  
`sh build_dpdk_app.sh`  

编译成功后，在当前目录下的`bin/`下生成二进制的可执行文件。

#### 步骤2 设置运行时环境变量。

`cd huaweicloud-fpga/fp1`  
`export SW_FPGA_DIR=$(pwd)`  
`export LD_LIBRARY_PATH=$SW_FPGA_DIR/software/userspace/dpdk_src/dpdk-16.04/x86_64-native-linuxapp-gcc/lib:$SW_FPGA_DIR/software/userspace/dpdk_src/securec/lib:$LD_LIBRARY_PATH`

#### 步骤3 进入APP可执行文件目录。

`cd $SW_FPGA_DIR/software/app/dpdk_app/bin`

#### 步骤4 运行测试命令。

**说明：** 以下步骤的命令都可以通过–h 参数获取帮助信息，例如命令`./ul_get_version -h`。

##### 例子1 打印逻辑的版本号。

`./ul_get_version`

**输出打印结果示例**  （版本号以实际查询出的结果为准）。

	[root@CentOS7 bin]# ./ul_get_version 
	version: 0x20171108

##### 例子2 测试取反寄存器

**1. 设置取反器输入寄存器，输入一个不超过32bit的数据**

运行命令为./ul_set_data_test -i *num*

例如`./ul_set_data_test -i 0xaa55`  

**2. 读取取反器结果寄存器，读取的数值为上一步中输入数值的取反值。**

`./ul_get_data_test`  

**输出打印结果示例** 

	[root@CentOS7 bin]# ./ul_set_data_test -i 0xaa55  
	[root@CentOS7 bin]# ./ul_get_data_test  
	oppos: 0xffff55aa

##### 例子3 测试加法器。

**1. 设置加法器加数寄存器** 

运行命令为./ul_set_data_add -i *augend* -i *addend*

例如`./ul_set_data_add -i 0x11111111 -i 0x5a5a5a5a`  

**2. 读取加法器结果寄存器。**

`./ul_get_data_add_result`  

**输出打印结果示例**  

	[root@CentOS7 bin]# ./ul_set_data_add -i 0x11111111 -i 0x5a5a5a5a
	Set [0x11111111]:[0x5a5a5a5a] to REG_PF_DEMO1_ADDER_CFG_WDATA0:REG_PF_DEMO1_ADDER_CFG_WDATA1 
	[root@CentOS7 bin]# ./ul_get_data_add_result
	add result: 0x6b6b6b6b

##### 例子4 测试打印DFX寄存器。

运行打印DFX寄存器命令。

`./dump_dfx_regs`  

**输出打印结果示例**  （实际结果值以逻辑当前寄存器状态为准）

	[root@CentOS7 bin]# ./dump_dfx_regs 
	 -------- Dump logic regs begin -------- 
		Reg addr      Value         Description
		[0x00018200]: 0x00000000  - txqm: reg_bdqm_err
		[0x00018204]: 0x00000000  - txqm: reg_mulqm_err0
		[0x00018440]: 0x00000010  - txqm: reg_bdqm_sta
		[0x00018608]: 0x00000000  - txqm: reg_r540_w288_c_cnt_en
		[0x0001c204]: 0x00000000  - txm: reg_txm_err
		[0x0001c400]: 0x00000005  - txm: reg_txm_status
		[0x0001c600]: 0x00000000  - txm: reg_ae2txm_req_rgt_cnt
		[0x0001c604]: 0x00000000  - txm: reg_ae2txm_req_err_cnt
		[0x0001c608]: 0x00000000  - txm: reg_txm2ae_tx_cnt
		[0x00024200]: 0x00000000  - rxm: reg_parrlt_err
		[0x00024408]: 0x00000001  - rxm: reg_axi_fifo_sta
		[0x00024628]: 0x00000000  - rxm: reg_axi_dis_cnt
		[0x0002462c]: 0x00000000  - rxm: reg_axi_rc_cnt
	 -------- Dump logic regs end -------- 

<a name="b"></b>
使用example2
------------

### 功能描述

Example2主要实现用户逻辑DMA环回通道和DDR读取功能。

### 使用Vivado HDK

用户申请FPGA镜像后，登录VM，HDK默认存放在huaweicloud-fpga/fp1目录下,HDK的使用步骤如下。

#### 步骤1 设置Vivado工具License。

用户打开huaweicloud-fpga/fp1/路径下的`setup.cfg`文件，将文件中`XILINX_LIC_SETUP`的值配置为License服务器的IP地址`2100@100.125.1.240:2100@100.125.1.251`。

`XILINX_LIC_SETUP="2100@100.125.1.240:2100@100.125.1.251"`

**说明：** 华为提供的Xilinx软件License仅限root账号使用。

#### 步骤2 配置开发环境。

`cd huaweicloud-fpga/fp1`  
`export HW_FPGA_DIR=$(pwd)`  
`source $HW_FPGA_DIR/setup.sh`

#### 步骤3 进入example2目录。

`cd $HW_FPGA_DIR/hardware/vivado_design/examples/example2`  
`export UL2_DIR=$(pwd)`

#### 步骤4 编译example2。

`cd $UL2_DIR/prj`  
`sh ./build.sh`

#### 步骤5 仿真example2。

`cd $UL2_DIR/sim`  
`make TC=sv_demo_001`

### 使用Vivado SDK
**说明：** 在生成dcp文件后，需要先完成环境配置及SDK的编译（具体可参考根目录下README 2.1.2章节），才能使用SDK。

#### example2 SDK测试环境配置

##### 步骤1 编译example2的SDK。

`cd huaweicloud-fpga/fp1/software/app/dpdk_app/`  
`chmod +x build_dpdk_app.sh`  
`sh build_dpdk_app.sh`  

编译成功后，在当前目录下的`bin/`下生成二进制的可执行文件。

##### 步骤2 设置环境变量。

`cd huaweicloud-fpga/fp1`  
`export SW_FPGA_DIR=$(pwd)`  
`export LD_LIBRARY_PATH=$SW_FPGA_DIR/software/userspace/dpdk_src/dpdk-16.04/x86_64-native-linuxapp-gcc/lib:$SW_FPGA_DIR/software/userspace/dpdk_src/securec/lib:$LD_LIBRARY_PATH`

##### 步骤3 进入APP可执行文件目录。

`cd $SW_FPGA_DIR/software/app/dpdk_app/bin`  

##### 步骤4 设置hugepage个数，默认的hugepage大小为2MB，当前配置个数为8192，即hugepage总大小为16GB，请确保VM的大页内存不小于16G。

`sysctl -w vm.nr_hugepages=8192`  


#### SDK DMA环路测试

**说明：** 以下步骤的命令都可以通过附带–h 参数获取帮助信息。

##### 步骤1 执行packet_process进程。

`./packet_process -d 8192 -q 0 -l 512 -n 102400099`  

**本例可用命令参数说明**  

| 参数     | 说明                                       |
| ------ | ---------------------------------------- |
| **-d** | 设置队列深度，有效值是1024、2048、4096、8192，默认值是8192。 |
| **-p** | 指定VF设备号，默认为0。                            |
| **-q** | 指定队列发送：有效值为[0，7]，默认为0，可以通过逗号进行多选（如 -q 0,1,5）。 |
| **-l** | 表示发送包的单包数据长度：有效值为[64，1048576]，默认值为64。    |
| **-n** | 表示发送包的个数：有效值为[1，4294966271]，默认为128。      |
| **-x** | 表示要进行几次循环，用于压力测试用，有效值为[1，64511]，默认为1。    |
| **-h** | 打印帮助信息。                                  |

**输出打印结果示例**  

	[root@CentOS7 bin]# ./packet_process -d 8192 -q 0 -l 512 -n 102400099
	available cpu number: 24, cpu mask parameter: -cffffff
	...
	----------------TEST TIME 0 for port 0----------------
	Not find mempool sec_mp_bd_0_0, create it
	mempool sec_mp_bd_0_0 has 2048 available entries
	Not find mempool sec_mp_data_0_0, create it
	mempool sec_mp_data_0_0 has 4096 available entries
	---------------- test for port 0, queue 0,  ----------------
	PMD: acc_dev_rx_burst()-418: port id: 0, queue id: 0: g_s_rx_total=10000220, g_s_rx_time=718061(us)
	PMD: acc_dev_rx_burst()-418: port id: 0, queue id: 0: g_s_rx_total=10000046, g_s_rx_time=717936(us)
	PMD: acc_dev_rx_burst()-418: port id: 0, queue id: 0: g_s_rx_total=10000532, g_s_rx_time=718003(us)
	PMD: acc_dev_rx_burst()-418: port id: 0, queue id: 0: g_s_rx_total=10000738, g_s_rx_time=718167(us)
	PMD: acc_dev_rx_burst()-418: port id: 0, queue id: 0: g_s_rx_total=10000384, g_s_rx_time=718047(us)
	PMD: acc_dev_rx_burst()-418: port id: 0, queue id: 0: g_s_rx_total=10000011, g_s_rx_time=717839(us)
	PMD: acc_dev_rx_burst()-418: port id: 0, queue id: 0: g_s_rx_total=10000623, g_s_rx_time=762135(us)
	PMD: acc_dev_rx_burst()-418: port id: 0, queue id: 0: g_s_rx_total=10000518, g_s_rx_time=789353(us)
	PMD: acc_dev_rx_burst()-418: port id: 0, queue id: 0: g_s_rx_total=10000384, g_s_rx_time=789451(us)
	PMD: acc_dev_rx_burst()-418: port id: 0, queue id: 0: g_s_rx_total=10000384, g_s_rx_time=789311(us)
	port 0, queue 0 run_business_tx_thread_route finish, time 7627130(us)
	port 0, queue 0 run_business_rx_thread_route finish, time 7627967(us)
	----------------port 0, queue 0 rx_packet_len 512, packet_num: 102400099, performance = 54 gbps----------------
	port 0, queue 0 TX/RX all success, all process 102400099 packets
	...
**说明：** 如上面日志所示，运行成功则打印日志**TX/RX all success**


#### SDK DDR读写测试

当前提供给用户4个16GB 2RX8 DDR，DDR读写测试是通过配置BAR空间寄存器对DDR进行间接读写。

**说明：** 以下步骤的命令都可以通过–h 参数获取帮助信息。

##### 步骤1 设置DDR值。

运行命令为./ul_write_ddr_data -n 0 -a *addr* -d *data*

例如`./ul_write_ddr_data -n 0 -a 0x1000 -d 0x12345678`  

##### 步骤2 读取DDR值。
运行命令为./ul_read_ddr_data -n 0 -a *addr*

例如`./ul_read_ddr_data -n 0 -a 0x1000`  

**输出打印结果示例**  （实际结果值以逻辑当前寄存器状态为准）

	[root@CentOS7 bin]# ./ul_write_ddr_data -n 0 -a 0x1000 -d 0x12345678
	[root@CentOS7 bin]# ./ul_read_ddr_data -n 0 -a 0x1000
	Value: 0x12345678

<a name="c"></c>
使用example3
------------

### 功能描述

Example3主要实现用户逻辑FMMU（Fpga Memory Manage Unit）功能。

### 使用Vivado HDK

用户申请FPGA镜像后，登录VM，HDK默认存放在`huaweicloud-fpga/fp1`目录下，HDK的使用步骤如下。

#### 步骤1 设置Vivado工具License。

用户打开huaweicloud-fpga/fp1/路径下的`setup.cfg`文件，将文件中XILINX_LIC_SETUP的值配置为License服务器的IP地址`2100@100.125.1.240:2100@100.125.1.251`。

`XILINX_LIC_SETUP="2100@100.125.1.240:2100@100.125.1.251"`

**说明：** 华为提供的Xilinx软件License仅限root账号使用。

#### 步骤2 配置开发环境。

执行以下指令完成硬件开发环境的配置。

`cd huaweicloud-fpga/fp1`  
`export HW_FPGA_DIR=$(pwd)`  
`source $HW_FPGA_DIR/setup.sh`

#### 步骤3 进入example3目录。

`cd $HW_FPGA_DIR/hardware/vivado_design/examples/example3`  
`export UL2_DIR=$(pwd)`

#### 步骤4 编译example3。

`cd $UL2_DIR/prj`  
`sh ./build.sh`

#### 步骤5 仿真example3。

`cd $UL2_DIR/sim`  
`make TC=sv_demo_001`

### 使用Vivado SDK

#### Example3 SDK测试环境配置

**说明：** 在生成dcp文件后，需要先完成环境配置及SDK的编译（具体可参考根目录下README 2.1.2章节），才能使用SDK。

##### 步骤1 编译example3的SDK。

`cd huaweicloud-fpga/fp1/software/app/dpdk_app/`  
`chmod +x build_dpdk_app.sh`  
`sh build_dpdk_app.sh`  

编译成功后，在当前目录下的`bin/`下生成二进制的可执行文件。

##### 步骤2 设置环境变量。

`cd huaweicloud-fpga/fp1`  
`export SW_FPGA_DIR=$(pwd)`  
`export LD_LIBRARY_PATH=$SW_FPGA_DIR/software/userspace/dpdk_src/dpdk-16.04/x86_64-native-linuxapp-gcc/lib:$SW_FPGA_DIR/software/userspace/dpdk_src/securec/lib:$LD_LIBRARY_PATH`

##### 步骤3 进入APP可执行文件目录。

`cd $SW_FPGA_DIR/software/app/dpdk_app/bin`


##### 步骤4 设置hugepage个数，默认的hugepage大小为2MB，当前配置个数为8192，即hugepage总大小为16GB，请确保VM的大页内存不小于16GB。

`sysctl -w vm.nr_hugepages=8192`


#### FMMU测试

**说明：** 以下步骤的命令都可以通过–h 参数获取帮助信息。

##### 步骤1 执行packet_process进程。

`./packet_process -d 8192 -q 0 -l 512 -n 102400099 -f`  

**本例可用命令参数说明**  

| 参数     | 说明                                       |
| ------ | ---------------------------------------- |
| **-d** | 设置队列深度，有效值是1024、2048、4096、8192，默认值是8192。 |
| **-p** | 指定VF设备号，默认为0。                            |
| **-q** | 指定队列发送：有效值为[0，7]，默认为0，可以通过逗号进行多选（如 -q 0,1,5）。 |
| **-l** | 表示发送包的单包数据长度：有效值为[64，1048576]，默认值为64。    |
| **-n** | 表示发送包的个数：有效值为[1，4294966271]，默认为128。      |
| **-f** | 打开FMMU功能，测试FMMU逻辑功能时必须打开此开关，不加该参数则默认为关闭。 |
| **-x** | 表示要进行几次循环，用于压力测试用，有效值为[1，64511]，默认为1。    |
| **-h** | 打印帮助信息。                                  |

**输出打印结果示例** 

	[root@CentOS7 bin]# ./packet_process -d 8192 -q 0 -l 512 -n 102400099 -f
	available cpu number: 24, cpu mask parameter: -cffffff
	...
	----------------TEST TIME 0 for port 0----------------
	Not find mempool sec_mp_bd_0_0, create it
	mempool sec_mp_bd_0_0 has 2048 available entries
	Not find mempool sec_mp_data_0_0, create it
	mempool sec_mp_data_0_0 has 4096 available entries
	---------------- test for port 0, queue 0, [FMMU]  ----------------
	PMD: acc_dev_rx_burst()-418: port id: 0, queue id: 0: g_s_rx_total=10000384, g_s_rx_time=733990(us)
	PMD: acc_dev_rx_burst()-418: port id: 0, queue id: 0: g_s_rx_total=10000384, g_s_rx_time=733935(us)
	PMD: acc_dev_rx_burst()-418: port id: 0, queue id: 0: g_s_rx_total=10000384, g_s_rx_time=740312(us)
	PMD: acc_dev_rx_burst()-418: port id: 0, queue id: 0: g_s_rx_total=10000279, g_s_rx_time=764110(us)
	PMD: acc_dev_rx_burst()-418: port id: 0, queue id: 0: g_s_rx_total=10000053, g_s_rx_time=764071(us)
	PMD: acc_dev_rx_burst()-418: port id: 0, queue id: 0: g_s_rx_total=10000129, g_s_rx_time=764130(us)
	PMD: acc_dev_rx_burst()-418: port id: 0, queue id: 0: g_s_rx_total=10000489, g_s_rx_time=764277(us)
	PMD: acc_dev_rx_burst()-418: port id: 0, queue id: 0: g_s_rx_total=10000970, g_s_rx_time=764283(us)
	PMD: acc_dev_rx_burst()-418: port id: 0, queue id: 0: g_s_rx_total=10000384, g_s_rx_time=764199(us)
	PMD: acc_dev_rx_burst()-418: port id: 0, queue id: 0: g_s_rx_total=10000031, g_s_rx_time=764029(us)
	port 0, queue 0 run_business_tx_thread_route finish, time 7740224(us)
	port 0, queue 0 run_business_rx_thread_route finish, time 7740980(us)
	----------------port 0, queue 0 rx_packet_len 544, packet_num: 102400099, performance = 57 gbps----------------
	port 0, queue 0 TX/RX all success, all process 102400099 packets
	...
**说明：** 如上面日志所示，运行成功则打印日志**TX/RX all success**