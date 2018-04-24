### *`The Operations in this section must be performed on VMs by the root user`* ###

[切换到中文版](./README_CN.md)

*`Files list`*

* `dpdk-16.04.tar.bz2`: DPDK source code  
* `securec.tar.bz2`: security source code  
* `build_dpdk.sh`: DPDK build script 

# 1. Decompressing the Security Library

`tar -xjv -f securec.tar.bz2`  

# 2. Building the Security Library

`cd securec/`  
`sh ./securec_make.sh`  

# 3. Decompressing the DPDK That includes the Logical PMD

`cd ..`  
`tar -xjv -f dpdk-16.04.tar.bz2`

# 4. Building the DPDK

`cd dpdk-16.04`  
`make config T=x86_64-native-linuxapp-gcc`  
`make`  
`make install T=x86_64-native-linuxapp-gcc`  

### Notes:  
After step 4 is complete, "Installation cannot run with T defined and DESTDIR undefined" will be displayed.


After all steps are performed successfully, a `x86_64-native-linuxapp-gcc` directory will be generated in the current directory, containing the DPDK's **include** directory and **lib** directory.
