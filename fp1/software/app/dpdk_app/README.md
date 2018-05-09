### *`Operations in this section must be performed on VMs as the root user.`* ###

[切换到中文版](./README_CN.md)

*`Directory list`*

* **bin/**: stores compiled applications and scripts that start or stop logs.
* **example1/**: stores source code of example 1. 
* **example2/**: stores source code of example 2 and example 3.
* **execute_objs/**: stores OBJs compiled from source code in **example1/** and **example2/**.
* **func/**: stores source code for common main function.
* **func_objs/**: stores OBJs compiled from source code in **func/**.
* **include/**: stores header files.

We provide auto-compile scripts to build DPDK source code and applications. Run the following command to use the scripts:

`source build_dpdk_app.sh`

If you use auto-compile scripts, you can start from step 5 directly.
If not, start from step 1.

# 1. Building DPDK Source Code
Go to the DPDK source code directory (**fp1/software/userspace/dpdk_src** by default) and build DPDK source code by referring to **README.md**.

# 2. Configuring Compiling Environment 
Replace `VF_DPDK` with the full path of DPDK source code (**path:*XXX*/fp1/software/userspace/dpdk_src** by default. *XXX* is the path of **fp1**), and export the following variables:  
`export DPDK_OBJECT_HOME=$VF_DPDK/dpdk-16.04/x86_64-native-linuxapp-gcc`    
`export DPDK_INCLUDE_HOME=$DPDK_OBJECT_HOME/include`  
`export DPDK_LIB_HOME=$DPDK_OBJECT_HOME/lib`  
`export SECUREC_HOME=$VF_DPDK/securec`  
`export SECUREC_INCLUDE_HOME=$SECUREC_HOME/include`  
`export SECUREC_LIB_HOME=$SECUREC_HOME/lib`

# 3. Building the Application
Go to the `dpdk_app` directory, and run the following command:

`make`

After the command is executed successfully, all the executable binary files will be compiled and stored in **bin/**.

# 4. Configuring the Running Environment for dpdk_app

`export LD_LIBRARY_PATH=$DPDK_LIB_HOME:$SECUREC_LIB_HOME:$LD_LIBRARY_PATH`

# 5. Running Example 1

`cd bin/`

## 5.1 Running `ul_get_version` to Print the IP Version
`./ul_get_version`

## 5.2 Running the Negater

`./ul_set_data_test -i 0xaa55`  
`./ul_get_data_test`

If success, it will print 0xffff55aa

## 5.3 Running the Summator

`./ul_set_data_add -i 0x11111111 -i 0x5a5a5a5a`  
`./ul_get_data_add_result`  

If success, it will print 0x6b6b6b6b

## 5.4 Dumping DFX Registers

`./dump_dfx_regs`  

If success, it will print the value of DFX registers

# 6. Running Example 2
## 6.1 Configuring Huge Pages

`sysctl -w vm.nr_hugepages=8192`

## 6.2 Running `packet_process`
### 6.2.1 Preparation
After step 2 to step 4 are complete, run the following command:  

`cd bin/`

### 6.2.2 Starting or Stopping Logs

Run the following command to save logs to **/var/log/fpga/dpdk.log**:

`sh start_dpdk_log.sh`

Run the following command to stop logs:

`sh shut_down_dpdk_log.sh`

### 6.2.3 TX and RX Test

`./packet_process -p 0 -q 0,1,2,7 -l 512 -n 102400099`  

Select queue index `0, 1, 2, and 7` of `VF 0` to transmit and receive `102400099` packets with a packet length of `512`.
After the command is executed successfully, the TX/RX thread will send /recv 102400099 packets.

| Parameter | Description                              |
| --------- | ---------------------------------------- |
| -d xxx    | xxx: queue depth. The value should be **1024**, **2048**, **4096**, or **8192**. The default value is **8192**. |
| -p xxx    | xxx: port ID. Logic supports only **vf0**. |
| -q xxx    | xxx: queue ID. The value range should be [0, 7]. The default value is **0**. |
| -l xxx    | xxx: length for each packet to TX and RX. (The scope is [64, 1048576]. The default value is **64**.) |
| -n xxx    | xxx: number of packets to TX and RX. (**128** by default. The maximum value is **4294966271**.) |
| -x xxx    | xxx: loop time for a full TX/RX transaction. (The scope is [1, 64511]. The default value is **1**.) |
| -f        | This parameter enables the FMMU function. (FMMU is disabled by default, and supports only example 3.) |
| -h        | This parameter prints help information.  |

## 6.3 Running the DDR Checker
This command is only supported in example 2 and prints 0x5a5a5a5a.

`./ul_write_ddr_data -n 0 -a 0x1000 -d 0x5a5a5a5a`  
`./ul_read_ddr_data -n 0 -a 0x1000`  

# 7. Running Example 3
## 7.1 Configuring Huge Pages

`sysctl -w vm.nr_hugepages=8192`  

## 7.2 Running `packet_process`
### 7.2.1 Preparation
After step 2 to step 4 are complete, run the following command: 

`cd bin/`  

### 7.2.2 Starting or Stopping Logs

Run the following command to save logs to **/var/log/fpga/dpdk.log**:

`sh start_dpdk_log.sh`

Run the following command to stop logs:

`sh shut_down_dpdk_log.sh`

### 7.2.3 TX and RX Test

`./packet_process -p 0 -q 0,1,2,7 -l 512 -n 102400099 -f`  

Select queue index `0, 1, 2, and 7` of `VF 0` to transmit and receive `102400099` packets with a packet length of `512`.
After the command is executed successfully, the TX/RX thread will send/recv 102400099 packets.

| Parameter | Description                              |
| --------- | ---------------------------------------- |
| -d xxx    | xxx: queue depth. The value should be **1024**, **2048**, **4096**, or **8192**. The default value is **8192**. |
| -p xxx    | xxx: port ID. Logic supports only **vf0**. |
| -q xxx    | xxx: queue ID. The value range should be [0, 7]. The default value is **0**. |
| -l xxx    | xxx: length for each packet to TX and RX. (The scope is [64, 1048576]. The default value is **64**.) |
| -n xxx    | xxx: number of packets to TX and RX. (**128** by default. The maximum value is **4294966271**.) |
| -x xxx    | xxx: loop time for a full TX/RX transaction. (The scope is [1, 64511]. The default value is **1**.) |
| -f        | This parameter enables the FMMU function. (FMMU is disabled by default, and supports only example 3.) |
| -h        | This parameter prints help information.  |

