# Example2用户指南

---

[Switch to the English version](./README.md)

## 示例介绍

本文档主要介绍华为fpga云服务示例2的主要组成部分和使用方法；该示例主要实现`dma数据环回`的功能，并利用该示例熟悉云上的开发、仿真和测试流程。

## 目录结构

示例放置在`$WORK_DIR/hardware/vivado_design/examples/`。目录包含如下文件和文件夹：

- **example2/**
  - prj
  - sim
  - src
  - src_encrypt
  - README_CN.md  

## 文件及文件夹说明

- prj

该目录存放vivado工程构建的信息，包括用户自定义配置文件和执行构建脚本及构建工程后用于生成AEI的tar包等。

- sim

该目录存放示例对应的**仿真平台**。

- src

该目录存放示例的**源码**。

- src_encrypt

该目录存放示例的源码经过vivado**加密**后的代码，该代码主要用于工程构建。

- README_CN.md
  即本文档，用于介绍其他文档。

## 使用说明

### 示例构建说明

进入prj目录，执行构建脚本`build.sh`，等待构建完成。
执行构建的方法见[Example2构建指南](./prj/README_cn.md)。

构建结果存储在`/prj/build/checkpoints/to_facs`目录下。

构建命令如下：

```bash
  $ cd $WORK_DIR/hardware/vivado_design/examples/example2/prj
  $ sh build.sh
```

### 示例仿真说明

执行仿真的方法见[Example2仿真用户指南](./sim/README_cn.md)。

### 示例测试说明

1.进入`$WORK_DIR/software/app/dpdk_app/example2`目录,执行make。
2.编译完成后用户可以参见[应用程序使用说明](../../../../software/app/dpdk_app/README_cn.md)进行示例测试。
