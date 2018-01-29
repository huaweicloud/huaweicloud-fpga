Using a Vivado-based Example
=======================
[切换到中文版](./Using a Vivado-based Example_cn.md)

[Example 1 Operation Instructions](#Example 1 Operation Instructions)

[Example 2 Operation Instructions](#Example 2 Operation Instructions)

[Example 3 Operation Instructions](#Example 3 Operation Instructions)

##### Note 
For users who need to understand the functions of the three examples, see [Example Application Guide](../hardware/vivado_design/examples/README.md).

Example 1 Operation Instructions
-------

### Functions

This example implements user logic version reading, data inversion, addition, and DFX printing functions.

### Vivado HDK Operation Instructions

After applying for an FPGA image, log in to a VM. The HDK is stored in the `huaweicloud-fpga/fp1` directory. To use the HDK, perform the following steps:

#### Step 1 Configure the license file of EDA.

Open the `setup.cfg` file in `huaweicloud-fpga/fp1/` and set `XILINX_LIC_SETUP` to the IP address `2100@100.125.1.240:2100@100.125.1.251` of the license server.

`XILINX_LIC_SETUP="2100@100.125.1.240:2100@100.125.1.251"`


##### Note
Only user root has the right to use the Xilinx license file provided by Huawei.

#### Step 2 Configure the development environment.

`cd huaweicloud-fpga/fp1`    
`export HW_FPGA_DIR=$(pwd)`  
`source $HW_FPGA_DIR/setup.sh`  

#### Step 3 Go to the directory of example 1.

`cd $HW_FPGA_DIR/hardware/vivado_design/examples/example1`    
`export UL1_DIR=$(pwd)`  

#### Step 4 Compile example 1.

`cd $UL1_DIR/prj`  
`sh ./build.sh`

#### Step 5 Simulate example 1.

`cd $UL1_DIR/sim`    
`make TC=sv_demo_001`  

### Vivado SDK Operation Instructions
##### Note
Generate a .dcp file, register and load the image, (For details, see "Registering an FPGA image" in the root directory), [Loading an Image](../tools/fpga_tool/docs/load_an_fpga_image.md), and compile the SDK (For details, see "Compiling the SDK" in the root directory) before using the SDK.

#### Step 1 Compile the SDK of example 1.

`cd huaweicloud-fpga/fp1/software/app/dpdk_app/`  
`chmod +x build_dpdk_app.sh`  
`sh build_dpdk_app.sh`  

After the compilation is successful, an executable binary file is generated in the `bin/` directory.

#### Step 2 Set runtime environment variables.

`cd huaweicloud-fpga/fp1`  
`export SW_FPGA_DIR=$(pwd)`  
`export LD_LIBRARY_PATH=$SW_FPGA_DIR/software/userspace/dpdk_src/dpdk-16.04/x86_64-native-linuxapp-gcc/lib:$SW_FPGA_DIR/software/userspace/dpdk_src/securec/lib:$LD_LIBRARY_PATH`

#### Step 3 Go to the directory that stores executable files.

`cd $SW_FPGA_DIR/software/app/dpdk_app/bin`

#### Step 4 Run the Test command.

##### Note
You can use the – h parameter, such as `./ul_get_version -h`, to obtain help information in the following steps.

##### Example 1 Print the logic version number.

`./ul_get_version`

**Information similar to the following is displayed:**

	[root@CentOS7 bin]# ./ul_get_version 
	version: 0x20171108

##### Example 2 Test the inverter.

**1. Set the input register of the inverter by entering a string up to 32 bits.**

Run the ./ul_set_data_test -i *num* command.

For example, `./ul_set_data_test -i 0xaa55`.  

**2. Read the inverter register, and check that the value is the bitwise inversion of the string previously entered.**

`./ul_get_data_test`  

**Information similar to the following is displayed:** 

	[root@CentOS7 bin]# ./ul_set_data_test -i 0xaa55  
	[root@CentOS7 bin]# ./ul_get_data_test  
	oppos: 0xffff55aa

##### Example 3 Test the adder.

**1. Set the adder register.** 

Run the ./ul_set_data_add -i *augend* -i *addend* command.

For example, `./ul_set_data_add -i 0x11111111 -i 0x5a5a5a5a`.  

**2. Read the adder register.**

`./ul_get_data_add_result`  

**Information similar to the following is displayed:**  

	[root@CentOS7 bin]# ./ul_set_data_add -i 0x11111111 -i 0x5a5a5a5a
	Set [0x11111111]:[0x5a5a5a5a] to REG_PF_DEMO1_ADDER_CFG_WDATA0:REG_PF_DEMO1_ADDER_CFG_WDATA1 
	[root@CentOS7 bin]# ./ul_get_data_add_result
	add result: 0x6b6b6b6b

##### Example 4 Test the DFX.

Run the command to print DFX status.

`./dump_dfx_regs`  

**Information similar to the following is displayed:**

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

Example 2 Operation Instructions
------------

### Functions

This example implements user logic DMA loopback channels and DDR read functions.

### Vivado HDK Operation Instructions

After applying for an FPGA image, log in to a VM. The HDK is stored in the `huaweicloud-fpga/fp1` directory by default. To use the HDK, perform the following steps:

#### Step 1 Configure the license file of EDA.

Open the `setup.cfg` file in `huaweicloud-fpga/fp1/` and set `XILINX_LIC_SETUP` to the IP address `2100@100.125.1.240:2100@100.125.1.251` of the license server.

`XILINX_LIC_SETUP="2100@100.125.1.240:2100@100.125.1.251"`

##### Note
Only user root has the right to use the Xilinx license file provided by Huawei.

#### Step 2 Configure the development environment.

`cd huaweicloud-fpga/fp1`  
`export HW_FPGA_DIR=$(pwd)`  
`source $HW_FPGA_DIR/setup.sh`

#### Step 3 Go to the directory of example 2.

`cd $HW_FPGA_DIR/hardware/vivado_design/examples/example2`  
`export UL2_DIR=$(pwd)`

#### Step 4 Compile example 2.

`cd $UL2_DIR/prj`  
`sh ./build.sh`

#### Step 5 Simulate example 2.

`cd $UL2_DIR/sim`  
`make TC=sv_demo_001`

### Vivado SDK Operation Instructions

#### SDK Test Environment Configuration of Example 2

#### Step 1 Compile example 2 of the SDK.

`cd huaweicloud-fpga/fp1/software/app/dpdk_app/`  
`chmod +x build_dpdk_app.sh`  
`sh build_dpdk_app.sh`  

After the compilation is successful, an executable binary file is generated in the `bin/` directory.

#### Step 2 Set runtime environment variables.

`cd huaweicloud-fpga/fp1`  
`export SW_FPGA_DIR=$(pwd)`  
`export LD_LIBRARY_PATH=$SW_FPGA_DIR/software/userspace/dpdk_src/dpdk-16.04/x86_64-native-linuxapp-gcc/lib:$SW_FPGA_DIR/software/userspace/dpdk_src/securec/lib:$LD_LIBRARY_PATH`

#### Step 3 Go to the directory that stores executable application files.

`cd $SW_FPGA_DIR/software/app/dpdk_app/bin`  

##### Step 4 Set the number of huge pages. The default size of a huge page is 2 MB and the current number is 8192, which means that the total size of huge pages is 16 GB. Ensure that the VM memory capacity is at least 16 GB.

`sysctl -w vm.nr_hugepages=8192`  


#### SDK DMA Loop Test

##### Note
Users can use the – h parameter to obtain help information in the following steps.

##### Step 1 Run the packet_process process.

`./packet_process -d 8192 -q 0 -l 512 -n 102400099`  

**Command Parameters**  

| Parameter | Description                              |
| --------- | ---------------------------------------- |
| **-d**    | Indicates the queue depth. The value can be 1024, 2048, 4096, and 8192. The default value is 8192. |
| **-p**    | Indicates the VF device that is used. The default value is 0. |
| **-q**    | Indicates the queues to be sent. The value range is [0,7]. The default value is 0. You can select multiple queues and use commas to separate them, for example, -q 0,1,5. |
| **-l**    | Indicates the length of a single packet in the packets to be sent. The value range is [64,1048576]. The default value is 64. |
| **-n**    | Indicates the number of packets to be sent. The value range is [1,4294966271]. The default value is 128. |
| **-x**    | Indicates the number of cycles used for stress test. The value range is [1,64511]. The default value is 1. |
| **-h**    | Displays the help information.           |

**Information similar to the following is displayed:**  

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
##### Note
As shown in preceding logs, if the operation is successful, **TX/RX all success** is displayed.


#### SDK DMR Read/Write Test

Currently, four 16GB 2RX8 DDRs are provided for users. The DDR read/write test is performed through configuring the BAR space register.

##### Note
Users can use the – h parameter to obtain help information in the following steps.

##### Step 1 Set the DDR value.

Run the ./ul_write_ddr_data -n 0 -a *addr* -d *data* command.

For example, `./ul_write_ddr_data -n 0 -a 0x1000 -d 0x12345678`.  

##### Step 2 Set the DDR value.
Run the ./ul_read_ddr_data -n 0 -a *addr* command.

For example, `./ul_read_ddr_data -n 0 -a 0x1000`.  

**Information similar to the following is displayed:**

	[root@CentOS7 bin]# ./ul_write_ddr_data -n 0 -a 0x1000 -d 0x12345678
	[root@CentOS7 bin]# ./ul_read_ddr_data -n 0 -a 0x1000
	Value: 0x12345678


Example 3 Operation Instructions
------------

### Functions

This example implements user logic FPGA memory manage unit (FMMU) function.

### Vivado HDK Operation Instructions

After applying for an FPGA image, log in to a VM. The HDK is stored in the `huaweicloud-fpga/fp1` directory. To use the HDK, perform the following steps:

#### Step 1 Configure the license file of EDA.

Open the `setup.cfg` file in `huaweicloud-fpga/fp1/` and set XILINX_LIC_SETUP to the IP address `2100@100.125.1.240:2100@100.125.1.251` of the license server.

`XILINX_LIC_SETUP="2100@100.125.1.240:2100@100.125.1.251"`

##### Note
Only user root has the right to use the Xilinx license file provided by Huawei.

#### Step 2 Configure the development environment.

Run the following commands to configure the hardware development environment.

`cd huaweicloud-fpga/fp1`  
`export HW_FPGA_DIR=$(pwd)`  
`source $HW_FPGA_DIR/setup.sh`

#### Step 3 Go to the directory of example 3.

`cd $HW_FPGA_DIR/hardware/vivado_design/examples/example3`  
`export UL2_DIR=$(pwd)`

#### Step 4 Compile example 3.

`cd $UL2_DIR/prj`  
`sh ./build.sh`

#### Step 5 Simulate example 3.

`cd $UL2_DIR/sim`  
`make TC=sv_demo_001`

### Vivado SDK Operation Instructions

#### Configuration of Example 3 SDK Test Environment

#### Step 1 Compile the SDK of example 3.

`cd huaweicloud-fpga/fp1/software/app/dpdk_app/`  
`chmod +x build_dpdk_app.sh`  
`sh build_dpdk_app.sh`  

After the compilation is successful, an executable binary file is generated in the `bin/` directory.

#### Step 2 Set runtime environment variables.

`cd huaweicloud-fpga/fp1`  
`export SW_FPGA_DIR=$(pwd)`  
`export LD_LIBRARY_PATH=$SW_FPGA_DIR/software/userspace/dpdk_src/dpdk-16.04/x86_64-native-linuxapp-gcc/lib:$SW_FPGA_DIR/software/userspace/dpdk_src/securec/lib:$LD_LIBRARY_PATH`

#### Step 3 Go to the directory that stores executable application files.

`cd $SW_FPGA_DIR/software/app/dpdk_app/bin`


##### Step 2 Set the number of huge pages. The default size of a huge page is 2 MB and the current number is 8192, which means that the total size of huge pages is 16 GB. Ensure that the VM memory capacity is at least 16 GB.

`sysctl -w vm.nr_hugepages=8192`


#### Testing the FMMU.

##### Note
Users can use the – h parameter to obtain help information in the following steps.

##### Step 1 Run the packet_process process.

`./packet_process -d 8192 -q 0 -l 512 -n 102400099 -f`  

**Command Parameters**  

| Parameter | Description                              |
| --------- | ---------------------------------------- |
| **-d**    | Indicates the queue depth. The value can be 1024, 2048, 4096, and 8192. The default value is 8192. |
| **-p**    | Indicates the VF device that is used. The default value is 0. |
| **-q**    | Indicates the queues to be sent. The value range is [0,7]. The default value is 0. You can select multiple queues and use commas to separate them, for example, -q 0,1,5. |
| **-l**    | Indicates the length of a single packet in the packets to be sent. The value range is [64,1048576]. The default value is 64. |
| **-n**    | Indicates the number of packets to be sent. The value range is [1,4294966271]. The default value is 128. |
| **-f**    | Enables the FMMU function, which must be enabled for FMMU logic function test. If this parameter is not specified, this function is disabled by default. |
| **-x**    | Indicates the number of cycles used for stress test. The value range is [1,64511]. The default value is 1. |
| **-h**    | Displays the help information.           |

**Information similar to the following is displayed:** 

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
##### Note
As shown in preceding logs, if the operation is successful, **TX/RX all success** is displayed.

