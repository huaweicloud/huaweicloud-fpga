# Directory Descriptions

[切换到中文版](./README_CN.md)

This directory stores HAL source code on which the compilation and running of the OpenCL development instance depend.

# Directory Structure
* driver

This directory stores HAL source code on which the OpenCL compilation depends.

* lib
  - runtime/platforms/hal

This directory stores the bottom-layer dynamic libraries on which the running of OpenCL instance depends. The structure of this directory cannot be changed without permissions.

Note

Before compiling the HAL, ensure that the XCPP tool in SDx software is available.

