# fp1开发套件说明

---
[Switch to the English version](./README.md)

##目录
-------
1. [fp1开发套件](#sec-1)
2. [目录结构](#sec-2)
3. [目录说明](#sec-3)
4. [HDK运行流程](#sec-4)
   * [4.1 配置License和配置环境变量的方法](#sec-4-1)
   * [4.2 设计和仿真](#sec-4-2)
   * [4.3 编写与调试用户应用](#sec-4-3)
   * [4.4 上传与注册](#sec-4-4)
   * [4.4 加载与运行](#sec-4-5)

</div>

<a id="sec-1" name="sec-1"></a>
## fp1开发套件

---

**fp1**是一款基于HWS的开源的云化的FPGA硬件与软件开发工具套件。该套件不仅能够帮助用户更完成设计、仿真、实现以及联合运行，而且还为用户提供专业的设计以及验证组件，帮助开发者更高效的实现FPGA的开发。

**fp1**主要由两部分组成，**硬件开发套件**（**HDK**）与**软件开发套件**（**SDK**）。其中HDK包括从RTL（Verilog/VHDL）设计、验证到构建的全部设计文件以及脚本。SDK则主要包含运行FPGA实例所需要的驱动、工具、运行环境以及应用程序。

<a id="sec-2" name="sec-2"></a>

## 目录结构

---

- **fp1/**
  - [hardware/](#sec-3-1)
  - [software/](#sec-3-2)
  - [tools/](#sec-3-3)
  - [docs/](#sec-3-4)
  - [release_notes.md](#sec-3-5)
  - [fpga_tool_setup.sh](#sec-3-6)
  - [fpga_tool_unistall.sh](#sec-3-7)
  - [setup.cfg](#sec-3-8)
  - [setup.sh](#sec-3-9)
  - README_CN.md

<a id="sec-3" name="sec-3"></a>

## 目录说明

---

<a id="sec-3-1" name="sec-3-1"></a>

- hardware

  用于存放fp1的硬件开发套件，包括vivado和SDAccel两种开发工具套件。可选择vivado作为开发工具，使用VHDL或Verilog硬件语言开发；也可以选择SDAccel作为开发工具，使用c、c++或openCL高级语言开发。

  详细说明见[hardware目录说明](./hardware/README_CN.md)。

<a id="sec-3-2" name="sec-3-2"></a>

- software

  用于存放fp1的软件开发套件，包括实例运行时所需要的运行环境、驱动、工具以及相关应用程序。

  详细说明见[software目录说明](./software/README_CN.md)。

<a id="sec-3-3" name="sec-3-3"></a>

- tools

  用于存放FP1平台下FPGA开发时所需的工具，目前仅包含FPGA镜像加载工具。

<a id="sec-3-4" name="sec-3-4"></a>

- docs

  用于存放FPGA开发套件指导文档，包括硬件开发流程以及示例的如何使用等。

<a id="sec-3-5" name="sec-3-5"></a>

- release_notes.md

  fp1的版本使用说明，包括工程构建、用户仿真、应用测试等说明。

<a id="sec-3-6" name="sec-3-6"></a>

- fpga_tool_setup.sh

  FPGA镜像加载工具设置及安装脚本，用户在使用FPGA镜像加载工具前需要执行。

<a id="sec-3-7" name="sec-3-7"></a>

- fpga_tool_unistall.sh

  FPGA镜像加载工具卸载脚本，用户在卸载FPGA镜像加载工具时需要执行。

<a id="sec-3-8" name="sec-3-8"></a>

- setup.cfg

  用户配置文件，包含license配置及vivado版本配置。

<a id="sec-3-9" name="sec-3-9"></a>

- setup.sh

  环境变量设置脚本,用户使用开发套件前需要执行。

<a id="sec-4" name="sec-4"></a>

## HDK运行流程

---

HDK的运行流程分为：基于vivado的流程，和基于sdaccel的流程。
这两种运行流程配置License和配置环境变量的方法是归一的，进行开发和仿真的路径和实例是不同的。

但是不论基于哪种开发模式，都需要经历以下几个步骤：

1.配置License与环境变量；
2.设计与仿真；
3.应用的设计与调试；
4.注册；
5.加载与运行。

<a id="sec-4-1" name="sec-4-1"></a>

###  配置License和配置环境变量的方法

- 进入fp1文件夹，打开setup.cfg文件:

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

- 配置环境变量：

```bash
  $ source setup.sh
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

<a id="sec-4-2" name="sec-4-2"></a>

### 设计和仿真的流程

HDK支持使用`vivado`以及`SDx`工具进行设计与仿真，详细步骤请见[hardware说明](./hardware/README_CN.md)中的**用户逻辑仿真**章节相关内容。

<a id="sec-4-3" name="sec-4-3"></a>

### 编写与调试用户应用

用户如果采用不同的开发方式，此步骤略有不同。

- vivado开发模式

Vivado开发模式采用DPDK的架构完成fpga与处理器的数据交互，详细编写以及调试用户应用的方法请参考[基于DPDK的用户应用开发说明](./software/app/dpdk_app/README_CN.md)。
如果用户需要修改驱动程序，请参考[基于DPDK的驱动开发说明](./software/userspace/dpdk_src/README_CN.md)完成。

- SDx开发模式

SDx开发模式采用Xilinx的SDAccel架构完成fpga与处理器的数据交互，详细编写以及调试用户应用的方法请参考[基于SDAccel的用户应用开发说明](./docs/Using an SDAccel-based Example_cn.md)。


<a id="sec-4-4" name="sec-4-4"></a>

### 上传与注册

用户可以依据fpga云服务提供的API以及上传和注册工具，完成bin文件的上传和注册ID的生成，详细注册方法请参考文档[usr_prj0构建指南](./hardware/vivado_design/user/usr_prj0/prj/README_CN.md)中的“AEI_Register.cfg文件配置说明”和“AEI_Register.sh命令的使用说明”章节。

<a id="sec-4-5" name="sec-4-5"></a>

### 加载与运行

用户可以依据fpga云服务提供的API以及上传时产生的注册ID，完成bin文件的加载，详细加载方式请参考相关文档。

运行方式与**编写与调试用户应用**的方法类似，请用户参考本文档[编写与调试用户应用](#sec-4-3)章节。
