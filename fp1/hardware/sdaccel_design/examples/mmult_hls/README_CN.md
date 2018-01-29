# 示例介绍

[Switch to the English version](./README.md)

mmult_hls 矩阵乘法的c的标准化实现。

# 目录结构
[mmult_hls](#mmult_hls_dir)/

- prj
  - bin
  - log
- sim
- src
- scripts

# 文件及文件夹说明
- prj

  - prj/bin

  该目录主要存放编译生成的可执行文件，以及生成的xclbin文件等目标文件

  - prj/log

  该目录主要存放目标文件执行之后产生的日志信息文件
- sim

  用户仿真目录

- src

  存放host源码以及kernel源码的目录


- scripts

  主要包含编译以及执行脚本

  compile.sh

  编译脚本，具体见 `sh compile.sh -h` 或 `sh compile.sh --help`

  run.sh

  执行脚本，具体见 `sh run.sh -h` 或 `sh run.sh --help`

