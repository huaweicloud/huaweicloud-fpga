# Example1用户指南

---

## 示例介绍

本文档主要介绍华为fpga云服务示例1的主要组成部分和使用方法；该示例主要实现用户逻辑的`版本号读取`，`输入数据取反`测试寄存器和`加法器`功能；在这里你可以了解到部分华为IP的使用方式，并利用该示例熟悉云上的开发、仿真和测试流程。

## 目录结构

示例放置在`$WORK_DIR/hardware/vivado_design/examples/`。目录包含如下文件和文件夹：

- **example1/**
  - prj
  - sim
  - src
  - src_encrypt  
  - README.md 

## 文件及文件夹说明

- prj  

该目录存放vivado工程构建的信息，包括用户自定义配置文件和执行构建脚本及构建工程后用于生成AEI的tar包等。

- sim  

该目录存放示例对应的**仿真平台**。

- src  

该目录存放示例的**源码**。

- src_encrypt  

该目录存放示例的源码经过vivado**加密**后的代码，该代码主要用于工程构建。

- README.md  

即本文档，用于介绍其他文档。

## 使用说明

### 示例构建说明

进入prj目录，执行构建脚本`build.sh`，等待构建完成。
执行构建的方法见[Example1构建指南](./prj/readme.md)。

构建结果存储在`/prj/build/checkpoints/to_facs`目录下。

构建命令如下：

```bash
  $ cd $WORK_DIR/hardware/vivado_design/examples/example1/prj
  $ sh build.sh
```

### 示例仿真说明

执行仿真的方法见[Example1仿真用户指南](./sim/readme.md)。

### 示例测试说明

1.进入`$WORK_DIR/software/app/dpdk_app/example1`目录,执行make。
2.编译完成后用户可以参见[应用程序使用说明](../../../../software/app/dpdk_app/readme.md)进行示例测试。
