Matrix Multiplication Example Using C Kernel
================================

[切换到中文版](./README_CN.md)

This is an implementation of performing matrix multiplication of two 16 x 16 matrices and getting the result back in a 16 x 16 matrix. 
The main algorithm characteristics of this application are:

Files in the Example
---------------------
Application host code

* test-cl.cpp

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
sh run.sh mmult bin_dir/bin_mmult_hw.xclbin

```