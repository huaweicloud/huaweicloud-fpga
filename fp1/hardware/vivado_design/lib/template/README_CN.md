# usr_template用户指南

[Switch to the English version](./README.md)

## 目录结构

目录位置为`$WORK_DIR/hardware/vivado_design/user/usr_template/`。目录包含如下文件和文件夹：

- **usr_template/**
  - prj
  - README_CN.md
  - sim
  - src
  - src_encrypt

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
执行构建的方法见[usr_template构建指南](./prj/README_CN.md)。

构建结果存储在`/prj/build/checkpoints/to_facs`目录下。

构建命令如下：

```bash
  $ cd $WORK_DIR/hardware/vivado_design/user/usr_template/prj
  $ sh build.sh
```

### 示例仿真说明

执行仿真的方法见[usr_template仿真用户指南](./sim/README_CN.md)。
