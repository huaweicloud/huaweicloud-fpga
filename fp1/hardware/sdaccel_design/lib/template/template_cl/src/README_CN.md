# 使用说明

[Switch to the English version](./README.md)

* 本目录下Makefile，用于编译host主机程序和kernel程序，用户可以根据自己的需求进行修改。
*  如果使用本Makefile 用户需要自己手动修改其中一下变量

  `HOST_EXE=XXX`

  host目标程序名称，例如`HOST_EXE=vadd`

  `KERNEL_NAME=XXX`

  kernel目标文件名称，必须与kernel名称一致
  例如`KERNEL_NAME = krnl_vadd`
