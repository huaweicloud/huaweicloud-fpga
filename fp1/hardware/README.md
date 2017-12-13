# hardware说明

<div id="table-of-contents">
<h2>目录</h2>
<div id="text-table-of-contents">
<ul>
<li><a href="#sec-1">1. <b>子目录结构</b></a></li>
<li><a href="#sec-2">2. <b>子目录说明</b></a></li>
<li><a href="#sec-3">3. <b>华为fpga云服务硬件开发说明</b></a>
<ul>
<li><a href="#sec-3-1">3.1. <b>申请实例</b></a></li>
</ul>
<ul>
<li><a href="#sec-3-2">3.2. <b>初始化环境变量</b></a></li>
</ul>
<ul>
<li><a href="#sec-3-3">3.3. <b>构建example</b></a></li>
</ul>
<ul>
<li><a href="#sec-3-4">3.4. <b>自定义设计</b></a></li>
</ul>
<ul>
<li><a href="#sec-3-5">3.5. <b>用户逻辑仿真</b></a></li>
</ul>
<ul>
<li><a href="#sec-3-6">3.6. <b>构建并提交用户逻辑生成AEI</b></a></li>
</ul>
</div>
</div>

<a id="sec-1" name="sec-1"></a>

## 1. 子目录结构

- **hardware/**

  - vivado_design/
  - sdaccel_design/
  - LICENSE.txt
  - README.md
  - version_note_dpdk.txt
  - version_note_sdx.txt

<a id="sec-2" name="sec-2"></a>

## 2. 子目录说明

- vivado_design

  本文件承载基于vivado开发工具套件的所有设计文件，包括库文件、源码、执行脚本等。

- sdaccel_design

  本文件承载基于sdaccel开发工具套件的所有设计文件，包括库文件、源码、执行脚本等。

- LICENSE.txt

  本文件承载的是HDK的License，用户不可以对该文件内容做任何更改。

- README.md

  本文档，用于说明其他文件。  

- version_note_dpdk.txt  

  本文档承载的是基于vivado工具开发的静态逻辑的发布日期及版本号，用户不可以对该文件内容做任何更改。

- version_note_sdx.txt  

  本文档承载的是基于sdx工具开发的静态逻辑的**发布日期**及**版本号**，用户不可以对该文件内容做任何更改。  

<a id="sec-3" name="sec-3"></a>

## 3. 华为fpga云服务硬件开发说明

华为FPGA云服务提供了基于vivado rtl的开发模式和基于SDAccel的c/c++/opencl等语言开发模式。用户可以依据自身的情况灵活选择开发模式，并在对应的目录结构下完成自己的加速设计。

- Vivado rtl的开发模式

1.该开发模式适合有一定FPGA开发经验的用户使用。
2.支持用户使用verilog或者VHDL语言开发、system_verilog或者c语言仿真和测试。
3.对应的目录结构为**vivado_design**。

- SDAccel c/c++/opencl语言的开发模式

1.该开发模式适合有软件开发经验的用户使用。
2.支持SDAccel开发流程，用户灵活选择使用c/c++/opencl语言进行开发、仿真和测试。
3.对应的目录结构为sdaccel_design。

说明：华为fpga云服务硬件开发只能用于**centos7.3**系统，并提供了所有的设计文件和脚本，用户可以在fpga云服务器上使用预安装的工具进行**开发、仿真和[构建AEI](../docs/注册FPGA镜像.md)**。

<a id="sec-3-1" name="sec-3-1"></a>

### 3.1  申请实例

用户使用fpga云服务之前需要**申请一个带有Vivado工具和相应license的实例（高性能型）或者带有SDA工具和相应license的实例（通用型）**，申请参考参考云服务网站：http://support.huaweicloud.com/usermanual-fpga/zh-cn_topic_0069154765.html

<a id="sec-3-2" name="sec-3-2"></a>

### 3.2 初始化环境变量

说明：环境变量只需要配置一次，如果已经配置过则可以跳过此步骤。

1.切换到工程根目录，命令如下：*(默认工程路径位置，用户可将工程复制或者移动到其他文件夹)*

```bash
  $ cd /home/fp1
```

2.配置**setup.cfg**文件中的用户自定义信息，详细说明与执行步骤参见[fp1开发套件说明](../README.md)中的**配置License和配置环境变量的方法**章节相关内容；

3.执行**setup.sh**设置环境变量以及工程相关依赖，命令如下：

```bash
  $ source setup.sh
```

每次执行`source setup.sh`命令时，HDK会执行以下三个步骤的检测：

1. 逐一检测所有工具的License是否已配置以及工具是否已安装（工程的初始状态是未安装的）；
2. 逐一告知工具是否已安装成功；
3. 打印出所有已安装的工具版本信息。

**注意**：如果是第一次安装本工程或者是完成版本升级，首次设置环境变量，HDK除了进行以上三步检测外还会执行以下步骤：

1. 预编译VCSMX仿真库（如果存在VCSMX工具）；
2. 预编译Questasim仿真库（如果存在Questasim工具）；
3. 调用Vivado工具生成IP以及DDR仿真模型；
4. 下载OBS桶中的dcp文件和压缩包（shell逻辑部分），该过程大约需要3~5分钟，请耐心等待。

<a id="sec-3-3" name="sec-3-3"></a>

#### 3.3 构建example

用户可按照example目录下面的README.md中的指导完成工程的开发、仿真与构建。

说明：example1、example2、example3为高性能型架构的样例，功能介绍见[DPDK example简介](./vivado_design/documents/README.md)；
mmult_hls、vadd_cl、vadd_rtl为通用型架构的样例，功能介绍见[OCL example简介](./sdaccel_design/examples/README.md)。

- example1的工程构建和仿真详细步骤与说明请参见[Example1用户指南](./vivado_design/examples/example1/README.md)；

- example2的工程构建和仿真详细步骤与说明请参见[Example2用户指南](./vivado_design/examples/example2/README.md)；

- example3的工程构建和仿真详细步骤与说明请参见[Example3用户指南](./vivado_design/examples/example3/README.md)；

- mmult_hls的工程构建和仿真(暂不支持)详细步骤与说明请参见[mmult_hls用户指南](./sdaccel_design/examples/mmult_hls/README.md)；

- vadd_cl的工程构建和仿真(暂不支持)详细步骤与说明请参见[vadd_cl用户指南](./sdaccel_design/examples/vadd_cl/README.md)；

- vadd_rtl的工程构建仿真(暂不支持)详细步骤与说明请参见[vadd_rtl用户指南](./sdaccel_design/examples/vadd_rtl/README.md)；

<a id="sec-3-4" name="sec-3-4"></a>

### 3.4 自定义设计

如果用户使用vivado进行开发，需要先配置环境变量，然后请使用以下命令：

```bash
  $ cd $WORK_DIR/hardware/vivado_design/user
  $ sh create_prj.sh
```

如果用户使用SDAccel进行开发，请在配置完成环境变量后执行以下命令：

```bash
  $ cd $WORK_DIR/hardware/sdaccel_design/user
  $ sh create_prj.sh
```

用户可以一键式的创建自定义的工程，支持两种方式：拷贝工程模板（需要拷贝到指定目录下）和直接创建目录结构的方式，使用户的开发更便捷。
详细操作步骤见[用户目录使用说明](./vivado_design/user/README.md)。

<a id="sec-3-5" name="sec-3-5"></a>

### 3.5 用户逻辑仿真

- 用户可以使用华为fpga云服务自带的Vivado XSIM仿真器进行仿真，用户也可以依据自己的习惯选择购买和安装自己的仿真器。
- 华为fpga云服务提供的仿真平台支持Synopsys公司的VCS, Mentor的Questasim以及Vivado仿真器。
- 用户也可以使用华为fpga云服务提供的sdaccel工具套件配套的仿真器进行仿真，用户也可以依据自己的习惯选择购买和安装自己的仿真器。
- 用户仿真的详细步骤可查看[仿真平台快速指南](./vivado_design/lib/sim/doc/quick_start.md)；
- 用户也可通过查看[仿真平台用户指导](./vivado_design/lib/sim/doc/user_guide.md)获得详细的仿真平台接口说明以及详细方案。

<a id="sec-3-6" name="sec-3-6"></a>

### 3.6 构建并提交用户逻辑生成AEI

用户可以依据fpga云服务提供的命令完成用户逻辑的上传及AEI的生成，详细使用方法请参考[用户目录使用说明](./vivado_design/user/README.md)。
