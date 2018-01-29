Vivado-based SDK Configuration and Compilation
===========================

[切换到中文版](./Vivado-based SDK Configuration and Compilation_cn.md)

[Configuring the Environment](#a)

[Compiling the DPDK](#b)

[Compiling an Example](#c)

<a name="a"></a>
Configuring the Environment
------------
```
cd huaweicloud-fpga/fp1/
source setup.sh
```

<a name="b"></a>
Compiling the DPDK
------------
### Note
This step is only for compiling the DPDK. The compiled PMD driver is provided for users.

Compilation Method
```
cd huaweicloud-fpga/fp1/software/userspace/dpdk_src  
chmod +x build_dpdk.sh  
sh build_dpdk.sh 
```

<a name="c"></a>
Compiling an Example
------------
### Note
The compilation of the example application depends on the DPDK. Therefore, the DPDK is automatically compiled.

Compilation Method

```
cd huaweicloud-fpga/fp1/software/app/dpdk_app/
chmod +x build_dpdk_app.sh 
sh build_dpdk_app.sh
```

After the compilation is successful, an executable binary file is generated in the `bin/` directory.

