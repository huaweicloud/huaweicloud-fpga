# Operation Instructions

[切换到中文版](./README_CN.md)

* Makefile in this directory is used to compile the host program and kernel program. Users can modify Makefile as required.
* If Makefile is used, manually modify the variables.

  `HOST_EXE=XXX`

Indicates the host target program name, for example, `HOST_EXE=vadd`.
	`KERNEL_NAME=XXX`

Indicates the kernel target file name, which must be the same as the kernel name.
For example, `KERNEL_NAME = krnl_vadd`.

