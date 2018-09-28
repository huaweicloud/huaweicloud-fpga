# 说明

[Switch to the English version](./README.md)

本目录存放`工程目录模板`，旨在规范化，简单化构建工程，其中包含项目工程必须的基本文件，此模板亦可作为用户构建自己工程的参考。

# 目录结构
[template_XX](#template_XX_dir)/  

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

  用户仿真用

- src

  存放host源码以及kernel源码的目录，目录中含有基本makefile 用户可根据需求自己修改，详细见src README_CN.md

- scripts

  主要包含编译以及执行脚本

    compile.sh

  编译脚本，具体见 `sh compile.sh -h` 或 `sh compile.sh --help`

  run.sh

  执行脚本，具体见 `sh run.sh -h` 或 `sh run.sh --help`

