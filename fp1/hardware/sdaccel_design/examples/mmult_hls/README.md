

# About This Example

[切换到中文版](./README_CN.md)

This example implements the C standardization of **mmult_hls** matrix multiplication.



# Directory Structure

[mmult_hls](#mmult_hls_dir)/



- prj

  - bin

  - log

- sim

- src

- scripts



# File and Folder Descriptions

- prj

  - prj/bin

 This directory stores the executable files and target files, such as xclbin, generated after compilation.

  - prj/log

  This directory stores logs generated after the execution of target files
- sim

  This is the user simulation directory.

- src

  This directory stores the host source code and kernel source code.


- scripts

  This directory stores compilation and execution scripts.

  compile.sh
  This is the compilation script. For details, see sh `compile.sh -h` or `sh compile.sh --help`

  run.sh

  This is the execution script. For details, see `sh run.sh -h` or `sh run.sh --help`