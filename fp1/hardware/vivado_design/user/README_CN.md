# 用户目录使用说明

[Switch to the English version](./README.md)

## 目录结构

- **user/**
  - create_prj.sh
  - usr_prj0
  - README_CN.md

## 目录说明

- create_prj.sh
  - 该文件承载的是用户创建工程的执行代码，`create_prj.sh`是用户创建工程时，执行命令的重要组成部分;
  - 执行如下命令，实现一键式将`工程目录模板`复制到`$WORK_DIR/hardware/vivado_design/usr/`：

    ```bash
    # usr_prjxx为用户工程的名称
    $ sh create_prj.sh usr_prjxx
    ```

  - 执行如下命令：

    ```bash
    $ sh create_prj.sh -h
    ```

    命令详细参数请参考命令的的帮助：

    ```bash
    ---------------------------------------------------------------------
    Usage: create_prj.sh [option]
    Options:
         -h |-H |-help                    Only for help
         [filename]                       Create [filename] directory
    ---------------------------------------------------------------------
    Example: when you run this command 'sh create_prj.sh usr_prj0' ,
         the directory will be build in '/fp1/hardware/vivado_design/usr/usr_prj0'
    Note   : The [filename] must start with letters, digits, and underscores.
    ```

- usr_prj0

  该文件夹是执行命令 `sh create_prj.sh usr_prj0`产生一个用户工程 `usr_prj0`的示例。

- README.md

  即本文档，用于介绍其他文档。

## 用户工程创建方法

用户工程创建方法有两种：

- 复制实例工程，通过更改参数，添加功能模块实现用户需求；
- 自己创建工程，上传用户自己设计的代码，添加在编译环境中，实现用户需求。

前者可快速实现一个用户工程，后者需要用户自己编写代码创建工程。

### 快速实现一个用户工程

#### 配置License及工具信息

- 打开setup.cfg文件:

```bash
  $ vim setup.cfg
```

- 配置FPGA_DEVELOP_MODE：

  如果使用SDAccel开发的话，请配置成：FPGA_DEVELOP_MODE="sdx"。
  如果使用vivado开发的话，请配置成：FPGA_DEVELOP_MODE="vivado"。
  默认配置为vivado。

- 配置软件License：

  从华为官网获取XILINX License；配置示例如：

```bash
  "XILINX_LIC_SETUP="2100@100.xxx.yyy.zzz:2100@100.xxx.yyy.zzz"(100.xxx.yyy.zzz表示license的ip地址).
```

- 配置VIVADO_VER_REQ：

  如果使用SDAccel开发的话，请配置成：VIVADO_VER_REQ="2017.1"。
  如果使用vivado开发的话，请配置成：VIVADO_VER_REQ="2017.2"。
  默认配置为2017.2。

---

#### 配置环境变量

  ```bash
  $ source $WORK_DIR/setup.sh
  ```

每次执行<kbd>source setup.sh</kbd>命令时，HDK会执行以下三个步骤的检测：

1. 逐一检测所有工具的License是否已配置以及工具是否已安装（工程的初始状态是未安装的）；
2. 逐一告知工具是否已安装成功；
3. 打印出所有已安装的工具版本信息。

**注意**：如果是第一次安装本工程或者是完成版本升级，首次设置环境变量，HDK除了进行以上三步检测外还会执行以下步骤：

1. 预编译VCSMX仿真库（如果存在VCSMX工具）；
2. 预编译Questasim仿真库（如果存在Questasim工具）；
3. 调用Vivado工具生成IP以及DDR仿真模型；
4. 下载OBS桶中的dcp文件和压缩包，该过程大约需要3~5分钟，请耐心等待。

---

#### 复制工程目录模板

  在`$WORK_DIR/hardware/vivado_design/user`下输入：

  ````bash
  # usr_prjxx为用户工程的名称
  $ sh create_prj.sh usr_prjxx
  ````

  实现一键式复制template工程文件夹及文件至`$WORK_DIR/hardware/vivado_design/usr/usr_prjxx`;

---

#### 配置usr_prj_cfg

- 该文件主要用于配置用户工程的自定义信息。
- 在`$WORK_DIR/hardware/vivado_design/user/usr_prjxx/prj/`打开`usr_prj_cfg`文件，命令如下:

```bash
  $ vim $WORK_DIR/hardware/vivado_design/user/usr_prjxx/prj/usr_prj_cfg
```

详细配置信息请参考`$WORK_DIR/hardware/vivado_design/user/usr_prjxx/README_CN.md`。

#### 添加模块

- 打开 `vivado_design/lib/common/`，该路径下存放了fifo 和 ram 的通用 CBB，点击打开可以看到代码，查找RAM相关的或fifo相关的设计文件;
- 平台提供的CBB的相关信息可以参阅`vivado_design/lib/common/README_CN.md`，查看模块功能；
- 选择所需模块，复制并添加到自己的工程`vivado_design/user/usr_prjxx/src`下，修改参数以满足需求；

#### 通过脚本build.sh完成构建

该命令用于编译一键执行快速创建的工程，**实现综合**、**布局布线**、**pr校验**、和**bit文件生成**，完成RTL构建的完整流程。
如用户工程进行过一键执行RTL构建，该命令还可用于单步执行某一项编译任务。

详细配置信息请参考[usr_prj0构建指南](./usr_prj0/README_CN.md)。

#### bit文件上传

RTL构建结束，生成的二进制文件存放在目录`$WORK_DIR/hardware/vivado_design/user/usr_prjxx/prj/build/checkpoints/to_facs`。文件夹中包含以下文件：

- usr_prjxx_partial.bin
- usr_prjxx_partial.bit
- usr_prjxx_routed.dcp
- manifest.json

最终通过调用系统命令`AEI_Register.sh`完成上传，系统会将需要的文件`usr_prjxx_routed.dcp 和 manifest.json` 上传到存储桶并返回AEI，由系统完成编译。
命令详细使用方法见[usr_prj0构建指南](./usr_prj0/README_CN.md)。
