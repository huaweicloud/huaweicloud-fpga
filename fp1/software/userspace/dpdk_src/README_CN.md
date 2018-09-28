### *`这些操作必须在虚拟机以根用户的方式运行`* ###

[Switch to the English version](./README.md)

*`文件列表`*

* `dpdk-16.04.tar.bz2`: DPDK源代码
* `securec.tar.bz2`: 安全函数库源代码
* `build_dpdk.sh`: DPDK编译脚本

# 1. 解压安全函数库源码包

`tar -xjv -f securec.tar.bz2`  

# 2. 编译安全函数库

`cd securec/`  
`sh ./securec_make.sh`  

# 3. 解压DPDK源码包

`cd ..`  
`tar -xjv -f dpdk-16.04.tar.bz2`

# 4. 编译DPDK

`cd dpdk-16.04`  
`make config T=x86_64-native-linuxapp-gcc`  
`make`  
`make install T=x86_64-native-linuxapp-gcc`  

### 提示:  
第四步完成后会打印"Installation cannot run with T defined and DESTDIR undefined"，该提示并非错误。

成功执行后，在当前目录会产生一个`x86_64-native-linuxapp-gcc`文件夹，其中包括DPDK的头文件目录和库目录。