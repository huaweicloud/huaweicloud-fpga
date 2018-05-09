Vector Addition Example 
============================================

[Switch to the English version](./README.md)

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
* SDx must be already installed and then xcpp tool can be available 

Compile host and Run the **run.sh** command to test hardware. The detailed procedure is as follows:
--------------------------------

```
make
sh run.sh vadd ./vadd.hw.xilinx_huawei-vu9p-fp1_4ddr-xpr_4_1.xclbin

```