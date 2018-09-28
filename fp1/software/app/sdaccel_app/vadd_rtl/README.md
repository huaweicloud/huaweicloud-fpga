Vector Addition Example 
============================================

[切换到中文版](./README_CN.md)

This is a simple example of vector addition. The kernel is used RTL
The prupose of this
 code is to introduce the user to application development

Files in the Example
----------------------
Application host code

- host.cpp
- xcl.cpp 

Compilation File
--------------------------------
* Makefile : Makefile for compiling application

Note
--------------------------------
* SDx must be installed to use the xcpp tool.

Compile host and Run the **run.sh** command to test hardware. The detailed procedure is as follows:

```
make
sh run.sh vadd ../../../../hardware/sdaccel_design/examples/vadd_rtl/prj/bin/vadd.hw.xilinx_huawei-vu9p-fp1_4ddr-xpr_4_1.xclbin 0

```

The 0 at the end of the above run.sh script indicates the slot number, which is obtained when the user requests the environment. 
For example, if a user applies for a virtual machine environment with 4 FPGA accelerator cards, the slot numbers are 0, 1, 2, and 3.

