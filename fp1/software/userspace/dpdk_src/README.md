### *`This action must performed on VM as root privilege`* ###

[切换到中文版](./README_CN.md)

*`Files list`*

* `dpdk-16.04.tar.bz2`: DPDK source code  
* `securec.tar.bz2`: Security source code  
* `build_dpdk.sh`: DPDK build script  

# 1. Uncompress the security library

`tar -xjv -f securec.tar.bz2`  

# 2. Build the security library

`cd securec/`  
`sh ./securec_make.sh`  

# 3. Uncompress the DPDK which including the logical PMD

`cd ..`  
`tar -xjv -f dpdk-16.04.tar.bz2`

# 4. Build the DPDK

`cd dpdk-16.04`  
`make config T=x86_64-native-linuxapp-gcc`  
`make`  
`make install T=x86_64-native-linuxapp-gcc`  

### Notes:  
It will print "Installation cannot run with T defined and DESTDIR undefined" after finish step 4, please feel fine, it is not a problem.


If success, under the current directory, it will generate a `x86_64-native-linuxapp-gcc` directory, in which contains the dpdk's include directory and lib directory.
