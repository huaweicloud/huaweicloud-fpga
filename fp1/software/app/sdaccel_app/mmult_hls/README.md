Matrix Multiplication Example with C Kernel
================================

[切换到中文版](./README_CN.md)

This is an implementation of performing matrix multiplication of two 16x16 matrices and getting the result back in 16x16 matrix. 
The main algorithm characteristics of this application are:

Files in the Example
---------------------
Application host code

* test-cl.cpp

Compilation File
--------------------------------
* Makefile: Makefile for compiling application

Note
--------------------------------
* SDx must be already installed and then xcpp tool can be available 

Compile host and Run the **run.sh** command to test hardware. The detailed procedure is as follows:

```
make
sh run.sh mmult ../../../../hardware/sdaccel_design/examples/mmult_hls/prj/bin/bin_mmult_hw.xclbin 0

```

The 0 at the end of the above run.sh script indicates the slot number, which is obtained when the user requests the environment. 
For example, if a user applies for a virtual machine environment with 4 FPGA accelerator cards, the slot numbers are 0, 1, 2, and 3.