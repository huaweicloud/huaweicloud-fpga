### *`Operations in this section must be performed on VMs as the root user.`* ###

[切换到中文版](./README_CN.md)

*`Directory list`*

* **bin/**: stores compiled applications and scripts that start or stop logs.
* **example1/**: stores source code of example 1. 
* **example2/**: stores source code of example 2.
* **example3/**: stores source code of example 3.
* **execute_objs/**: stores OBJs compiled from source code in **example1/** and **example2/**.
* **func/**: stores source code for common main function.
* **func_objs/**: stores OBJs compiled from source code in **func/**.
* **include/**: stores header files.
* **lib/**: stores dynamic link library files.
* **tools/**: stores source code of tools (read or write register).

# 1. Building DPDK Source Code and Application  
We provide auto-compile scripts to build DPDK source code and applications. Run the following command to use the scripts:

`source build_dpdk_app.sh`

# 2. Running Example 1

`cd bin/`

XXX below is the slot id of device which you want to run the test, 0 as default.

## 2.1 Running `ul_get_version` to Print the IP Version
`./ul_get_version -s XXX`

## 2.2 Running the Negater

`./ul_set_data_test -s XXX -i 0xaa55`  
`./ul_get_data_test -s XXX`

## 2.3 Running the Summator

`./ul_set_data_add -s XXX -i 0x11111111 -i 0x5a5a5a5a`  
`./ul_get_data_add_result -s XXX`  

If success, it will print 0x6b6b6b6b

## 2.4 Dumping DFX Registers

`./dump_dfx_regs -s XXX`  

If success, it will print the value of DFX registers

## 2.5 Read UL logic register

`./ul_read_bar2_data -s XXX -a AAA`  

AAA is the address which you want to read.

## 2.6 Write UL logic register

`./ul_write_bar2_data -s XXX -a AAA -d DDD`  

AAA is the address to be written, and DDD is the actual data to be written.

# 3. Running Example 2
## 3.1 Configuring Huge Pages

`sysctl -w vm.nr_hugepages=8192`

## 3.2 Running `packet_process`
### 3.2.1 Preparation

`cd bin/`

### 3.2.2 Starting or Stopping Logs

Run the following command to save logs to **/var/log/fpga/dpdk.log**:

`sh start_dpdk_log.sh`

Run the following command to stop logs:

`sh shut_down_dpdk_log.sh`

### 3.2.3 TX and RX Test

`./packet_process -s XXX -q 0,1,2,7 -l 512 -n 102400099`  

Select queue index `0, 1, 2, and 7` of `slot XXX` to transmit and receive `102400099` packets with a packet length of `512`.
After the command is executed successfully, the TX/RX thread will send /recv 102400099 packets.

| Parameter | Description                              |  
| --------- | ---------------------------------------- |  
| -d xxx    | xxx: queue depth. The value should be **1024**, **2048**, **4096**, or **8192**. The default value is **8192**. |  
| -s xxx    | xxx: slot ID. (The scope is [0, 7]. The default value is **0**.) |  
| -q xxx    | xxx: queue ID. The value range should be [0, 7]. The default value is **0**. |  
| -l xxx    | xxx: length for each packet to TX and RX. (The scope is [64, 1048576]. The default value is **64**.) |  
| -n xxx    | xxx: number of packets to TX and RX. (**128** by default. The maximum value is **4294966271**.) |  
| -x xxx    | xxx: loop time for a full TX/RX transaction. (The scope is [1, 64511]. The default value is **1**.) |  
| -h        | This parameter prints help information.  |  

## 3.3 Running the DDR Checker
This command is only supported in example 2 and prints 0x5a5a5a5a.

`./ul_write_ddr_data -s XXX -n 0 -a 0x1000 -d 0x5a5a5a5a`  
`./ul_read_ddr_data -s XXX -n 0 -a 0x1000`  

# 4. Running Example 3
## 4.1 Configuring Huge Pages

`sysctl -w vm.nr_hugepages=8192`  

## 4.2 Running `fpga_ddr_rw`
### 4.2.1 Preparation

`cd bin/`  

### 4.2.2 Read or Write FPGA DDR Test

`./fpga_ddr_rw -s 0 -t 1 -p 1000 -m 1 -l 131072 -r 0`  

Launch one thread to read 1000 packages from fpga ddr of slot 0, package len is 131072 byte and fpga ddr read addr is 0.

`./fpga_ddr_rw -s 0 -t 1 -p 1000 -m 2 -l 131072 -w 0`  

Launch one thread to write 1000 packages to fpga ddr of slot 0, package len is 131072 byte and fpga ddr write addr is 0.

`./fpga_ddr_rw -s 0 -t 1 -p 1000 -m 0 -l 131072 -w 0 -r 0`  

Launch one thread to loopback 1000 packages with fpga ddr of slot 0, package len is 131072 byte, fpga ddr read addr and fpga ddr write addr is both 0.

| Parameter | Description                              |  
| --------- | ---------------------------------------- |  
| -t xxx    | xxx: thread num. The value should be [1, 10]. The default value is **1**. |  
| -s xxx    | xxx: slot id. The default value is **0**. |  
| -p xxx    | xxx: package num. The default value is **1000**. |  
| -m xxx    | xxx: mode. The value range should be [0(loopback), 1(read), 2(write)]. The default value is **0**. |  
| -l xxx    | xxx: package len. The default value is **64**. The value should be [1, 4*1024*1024]. |  
| -r xxx    | xxx: fpga ddr read addr. The value should be [0, 64*1024*1024*1024). The maximum value is **64*1024*1024*1024**. |   
| -w xxx    | xxx: fpga ddr write addr. The value should be [0, 64*1024*1024*1024). The maximum value is **64*1024*1024*1024**. |   
| -h        | This parameter prints help information.  |  

