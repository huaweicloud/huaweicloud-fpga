# 使用说明

[Switch to the English version](./README.md)

* 本目录下Makefile，用于编译host主机程序和kernel程序，用户可以根据自己的需求进行修改。
*  如果使用本Makefile 用户需要自己手动修改其中一下变量

  `EXE=XXX`

  host目标程序名称，例如`EXE=host`

  `KERNEL_NAME=XXX`

  kernel目标文件名称，必须与kernel名称一致
  例如`KERNEL_NAME = krnl_vadd`


# hdl目录说明
本目录用于存放rtl文件
# *.tcl
根据rtl文件生成*.xo目标文件
# kernel.xml
此文件用于描述kernel信息。用户根据实际使用kernel进行编辑