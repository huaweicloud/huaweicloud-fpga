Kernel基于opencl c语言实现的矢量相加实例
===============================
[Switch to the English version](./README.md)

这是一个基于opencl的矢量相加实例，其kernel部分基于opencl c语言实现. 

这个示例主要包含:
---------------------
应用层host代码

- vadd.cpp
- vadd.h


编译脚本
--------------------------------
* Makefile : 编译应用层（host代码）的编译脚本

** 说明 **
--------------------------------
* 必须已经安装SDx，然后才能使用xcpp工具 


先编译host代码然后执行 **run.sh** 命令来测试该用例，具体的操作命令如下：

```
make
sh run.sh vadd ../../../../hardware/sdaccel_design/examples/vadd_cl/prj/bin/bin_vadd_hw.xclbin 0

```

上面run.sh脚本末尾的0表示slot号，该号在用户申请环境时得到。比如用户申请了一个带4张FPGA加速卡的虚拟机环境，则slot号为0、1、2、3。