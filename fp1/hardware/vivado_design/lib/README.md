# 公共目录使用说明

## 目录结构

本文档主要存放平台提供的各种库文件，用户可以不关注。

- **lib/**
  - checkpoints/
  - common/
  - constraints/
  - interfaces/
  - ip/
  - scripts/
  - sim/
  - template/
  - README.md

## 目录说明

- checkpoints
  该目录用于存放静态逻辑网表文件shell_routed.dcp，以及该文件对应的`md5校验值`;

- common
  该目录用于存放华为公司提供的`通用基础模块`（Common Building Block，简称CBB）;

- constraints
  该目录用于存放用户逻辑ddra/ddrb/ddrd的`管脚约束信息`;

- interfaces
  该目录用于存放静态逻辑和用户逻辑之间的接口文件及未使用的`接口库文件`;

- ip
  该目录用于存放用户使用CoreGen生成的`Xilinx IP Core`;

- scripts
  该目录用于存放加密的keyfile和fpga云服务`构建脚本库`;

- sim
  该目录用于存放用户逻辑的`仿真公共文件`，详细说明请参考[仿真公共文件夹说明](./sim/readme.md);

- template
  该目录用于存放用户逻辑构建`工程目录模板`;

- README.md
  即本文档，用于介绍其他文件。
