Vector Addition Example
===============================

[切换到中文版](./README_CN.md)

This is an example of vector addition.

Files in the Example
---------------------
Application host code

- vadd.cpp
- vadd.h


Compilation File
--------------------------------
* Makefile : used for compiling application

Note
--------------------------------
* SDx must be installed to use the xcpp tool.

Compile host and Run the **run.sh** command to test hardware. The detailed procedure is as follows:

```
make
sh run.sh vadd ../../../../hardware/sdaccel_design/examples/vadd_cl/prj/bin/bin_vadd_hw.xclbin 0

```

The 0 at the end of the above run.sh script indicates the slot number, which is obtained when the user requests the environment. 
For example, if a user applies for a virtual machine environment with 4 FPGA accelerator cards, the slot numbers are 0, 1, 2, and 3.