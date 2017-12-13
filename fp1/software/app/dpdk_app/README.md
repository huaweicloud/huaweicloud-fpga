### *`These actions must be performed on VM as root privilege`* ###
*`Directory list`*

* bin/: APPs compiled out.
* example1/: Source code for example1.  
* example2/: Source code for example2 and example3.
* execute_objs/: OBJs compiled out of example1/ and example2/ source code.
* func/: Source code for common main function.
* func_objs/: OBJs compiled out of func/ source code.
* include/: Header files.

# 1. Configure compiling environment 
Please use real path to take place of `VF_DPDK`, export following variables	
`export DPDK_OBJECT_HOME=$VF_DPDK/dpdk-16.04/x86_64-native-linuxapp-gcc`    
`export DPDK_INCLUDE_HOME=$DPDK_OBJECT_HOME/include`  
`export DPDK_LIB_HOME=$DPDK_OBJECT_HOME/lib`  
`export SECUREC_HOME=$VF_DPDK/securec`
`export SECUREC_INCLUDE_HOME=$SECUREC_HOME/include`  
` export SECUREC_LIB_HOME=$SECUREC_HOME/lib`

# 2. Build the app
Under the `dpdk_app` directory  

`make`

If success, all the binary executed files will be compiled out to bin/.

# 3. configure the dpdk_app's running environment

`export LD_LIBRARY_PATH=$DPDK_LIB_HOME:$SECUREC_LIB_HOME:$LD_LIBRARY_PATH`

# 4. Running example1

`cd bin/`

## 4.1 Running `ul_get_version`, it will print the IP version
`./ul_get_version`

## 4.2 Running negater

`./ul_set_data_test -i 0xaa55`  
`./ul_get_data_test`

If success, it will print 0xffff55aa

## 4.3 Running summator

`./ul_set_data_add -i 0x11111111 -i 0x5a5a5a5a`  
`./ul_get_data_add_result`  

If success, it will print 0x6b6b6b6b

## 4.4 Dump DFX registers

`./dump_dfx_regs`  

If success, it will print the value of DFX registers

# 5. Running example2
## 5.1 Configure Hugepage

`sysctl -w vm.nr_hugepages=8192`

## 5.2 Running `packet_process`
### 5.2.1 Prepare
Please make sure that we have done step 1 and step 3, then  

`cd bin/`  

### 5.2.2 TX and RX test

`./packet_process -p 0 -q 0,1,2,7 -l 512 -n 102400099`  

This command means that: user select queue index `0, 1, 2, and 7` of `VF 0` to transmit and receive `102400099` packets with packet length `512`  
If success, the TX/RX thread will send/recv 102400099 packets  

|parameter|description|
|-------|-------|
|-d xxx |xxx: queue depth, should be 1024 or 2048 or 4096 or 8192, 8192 as default|
|-p xxx |xxx: port id, logic only support vf0|
|-q xxx |xxx: queue idx, should be [0, 7], 0 as default|
|-l xxx |xxx: length for each packet to tx and rx (length's scope is [64, 1048576], 64 as default);|
|-n xxx |xxx: number of packet to tx and rx; (128 as default, max=(4294966271))|
|-x xxx |xxx: the loop time for a full TX/RX business(loop's scope is [1, 64511], 1 as default)|
|-f     |enable FMMU function(disable as default, this is for example3, not supported in example2)|
|-h     |print help|

## 5.3 Running DDR checker
This command is only supported in example2.

`./ul_write_ddr_data -n 0 -a 0x1000 -d 0x5a5a5a5a`  
`./ul_read_ddr_data -n 0 -a 0x1000`  
  
If success, it will print 0x5a5a5a5a

# 6. Running example3
## 6.1 Configure Hugepage

`sysctl -w vm.nr_hugepages=8192`  

## 6.2 Running `packet_process`
### 6.2.1 Prepare
Please make sure that we have done step 1 and step 3, then  

`cd bin/`  

### 6.2.2 TX and RX test

`./packet_process -p 0 -q 0,1,2,7 -l 512 -n 102400099 -f`  
  
This command means that: user select queue index `0, 1, 2, and 7` of `VF 0` to transmit and receive `102400099` packets with packet length `512`  
If success, the TX/RX thread will send/recv 102400099 packets  

|Parameter|Description|
|-------|-------|
|-d xxx |xxx: queue depth, should be 1024 or 2048 or 4096 or 8192, 8192 as default|
|-p xxx |xxx: port id, logic only support vf0|
|-q xxx |xxx: queue idx, should be [0, 7], 0 as default|
|-l xxx |xxx: length for each packet to tx and rx (length's scope is [64, 1048576], 64 as default);|
|-n xxx |xxx: number of packet to tx and rx; (128 as default, max=(4294966271))|
|-x xxx |xxx: the loop time for a full TX/RX business(loop's scope is [1, 64511], 1 as default)|
|-f     |enable FMMU function(disable as default, should add this parameter for example3 test)|
|-h     |print help|