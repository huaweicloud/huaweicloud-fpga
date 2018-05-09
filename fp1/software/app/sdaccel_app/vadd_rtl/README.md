Vector Addition Example 
============================================

[切换到中文版](./README_CN.md)

This is an example of vector addition. The kernel uses RTL.
This code introduces application development to users.

Files in the Example
----------------------
Application host code

- host.cpp
- xcl.cpp 

Compilation File
--------------------------------
* Makefile: used for compiling application

Note
--------------------------------
* SDx must be installed to use the xcpp tool.

Compile host and Run the **run.sh** command to test hardware. The detailed procedure is as follows:
--------------------------------

```
make
sh run.sh vadd ./vadd.hw.xilinx_huawei-vu9p-fp1_4ddr-xpr_4_1.xclbin

```