# Operation Instructions

[切换到中文版](./README_CN.md)

* Makefile in this directory is used to compile the host program and kernel program. Users can modify Makefile as required.
* If Makefile is used, manually modify the variables.

  `EXE=XXX`

Indicates the host target program name, for example, `EXE=host`.
	`KERNEL_NAME=XXX`

Indicates the kernel target file name, which must be the same as the kernel name.
For example, `KERNEL_NAME = krnl_vadd`.


# hdl Directory Description
This directory stores .rtl files.
# *.tcl
This file generates the *.xo target files based on .rtl files.
# kernel.xml
This file describes kernel information. Users can edit the file as required by using the kernel.

