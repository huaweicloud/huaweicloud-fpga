# Release 1.0.2
- 文档优化；
- opencl代码解决安全漏洞；
- example3 src代码优化；
- dpdk代码优化；

# Release 1.0.1
- 文档优化，合入各类文档体验问题100余个；
- setup.sh脚本优化，解决文件覆盖异常提示；
- pmd代码缺陷解决；
- 修改perf.cpp，datamover.h，shim.cpp适配代码扫描；
- dpdk编译脚本优化：修改build_dpdk.sh与build_dpdk_app.sh脚本，执行错误打印相应的错误说明及返回相应的错误码；执行成功后，打印成功信息；

# Release 1.0.0
- 这是Huawei Cloud FPGA设计实例的首个公开版本。有关该版本功能的详细信息，可以在以下的**FPGA 设计实例特性概述**以及**FPGA 设计实例特性详述**章节中找到。

---
# FPGA 设计实例特性概述

* 每个FPGA用户可以使用的接口如下：
  - `1个pcie gen3 x16 `接口    
  - `4个ddr4` RDIMM接口

* PCIE支持的特性：
  - `1个 PF `(physical function)
  - `1个 VF` (Virtual function)
  - 每个VF支持`8`个队列  

* 用户逻辑和静态逻辑之间支持的接口特性：
  - 静态逻辑到用户逻辑之间的DMA数据通道是`512 bit`位宽的AXI4-Stream接口
  - 用户逻辑到静态逻辑之间的DMA数据通道是`512 bit`位宽的AXI4-Stream接口
  - 静态逻辑到用户逻辑之间的DMA BD（Buffer Description）通道是`256 bit`位宽的AXI4-Stream接口
  - 用户逻辑到静态逻辑之间的DMA BD（Buffer Description）通道是`256 bit`位宽的AXI4-Stream接口
  - 寄存器访问和bar空间映射使用的是AXI4-Lite接口
  - DDR的接口使用的是`512 bit`位宽的AXI4接口

* DDR接口划分：
  - 1个DDR控制器放置在静态逻辑部分
  - 3个DDR控制器放置在用户逻辑部分
  - 支持用户最多使用`4个DDR控制器`


---
# FPGA 设计实例特性详述

# 目录

## 1. [工程构建](#工程构建)
## 2. [用户仿真](#用户仿真)
## 3. [应用测试](#应用测试)
## 4. [工具环境](#工具环境)
## 5. [license要求](#license要求)
## 6. [即将支持特性](#即将支持特性)
***
<a name="工程构建"></a>
# 工程构建

## 概述
执行工程构建之前必须要`确认vivado工具及license安装完成`；工程构建旨在用户通过最小的改动实现符合时序要求的工程设计。

## 特性列表

* 支持vivado设计和sdaccel高级语言设计

* 支持用户使用`VHDL和Verilog`编码

* 支持用户使用`opencL、c和c++`编码

* 支持VHDL、Verilog、opencL、c、c++和SystemVerilog代码`自动扫描加密`

* 支持用户配置和执行命令解耦，用户只需要`定义自己的工程名字和路径`等变量，即可完成工程构建

* 支持所有的Vivado综合策略用户可灵活配置选择，支持的综合策略:
  - DEFAULT
  - AreaOptimized_high
  - AreaOptimized_medium
  - AreaMultThresholdDSP
  - AlternateRoutability
  - PerfOptimized_high
  - PerfThresholdCarry
  - RuntimeOptimized

* 支持所有的Vivado实现策略用户可灵活配置选择，支持的综合策略:
  - DEFAULT
  - Explore
  - ExplorePostRoutePhysOpt
  - WLBlockPlacement
  - WLBlockPlacementFanoutOpt
  - NetDelay_high
  - NetDelay_low
  - Retiming
  - ExtraTimingOpt
  - RefinePlacement
  - SpreadSLLs
  - BalanceSLLs
  - SpreadLogic_high
  - SpreadLogic_medium
  - SpreadLogic_low
  - SpreadLogic_Explore
  - SSI_SpreadLogic_high
  - SSI_SpreadLogic_low
  - SSI_SpreadLogic_Explore
  - Area_Explore
  - Area_ExploreSequential
  - Area_ExploreWithRemap
  - Power_DefaultOpt
  - Power_ExploreArea
  - Flow_RunPhysOpt
  - Flow_RunPostRoutePhysOpt
  - Flow_RuntimeOptimized
  - Flow_Quick

* 支持用户工程构建完成后，自动检查时序报告，并打印检查结果

* 支持用户使用Vivado IP catalog自定义IP

* 支持PR校验

* 支持综合、实现、pr校验、目标文件生成操作`分步执行`

* 支持`定时执行`工程

* 支持用户代码、华为IP和Xilinx IP自动扫描机制

* 支持`一键式`创建用户目录

* SHELL的md5校验


## 待优化特性
 * 无


## 已知问题
 * 无

---

<a name="用户仿真"></a>

# 用户仿真

## 概述
执行工程构建之前必须要`确认vivado工具及license安装完成`；用户仿真旨在通过验证平台证实已有的设计是否符合设计预期要求。

## 特性列表

* Testbench采用`systemverilog-2012标准语法`编写

* 支持代码覆盖率的收集以及报告的生成
  - 收集代码覆盖率的文件可由用户自定义

* 支持axi4/axi4-lite部分验证特性
  - 支持AXI4接口的burstlen`从1到255`
  - 支持AXI4接口AW以及AR通道的outstanding特性
  - 支持burst长度与实际长度的一致性检查
  - 支持基于AXI4/AXI4-lite标准协议的部分覆盖率收集
  - 支持基于AXI4/AXI4-lite标准协议的部分断言检查

* 支持用户自定义激励
  - 支持用户通过配置文件自定义激励
  - 支持用户自己实现激励产生以及发送部分

* 支持用户自定义callback方法
  - 支持用户自定义callback task/function，为用户提供了实现自定义功能而无需修改testbench的方法

* 支持testbench与testcase分离
  - 支持testbench与testcase分离，为用户提供了`自行设计、实现testcase而无需修改testbench`的方法

* 提供简易Scoreboard
  - 支持基本的报文比对，报文比对基于stream_id以及fsn

* 支持功能覆盖率的收集以及报告的生成
  - 支持基于AXI4/AXI4-Lite的部分功能覆盖率
  - 支持的功能覆盖率包括`burst_len, burst_size, burst_mode, strobe`等

* 支持接口的断言
  - 支持基于`AXI4/AXI4-Lite接口`的部分断言
  - 断言主要覆盖X/Z状态的检查

* 支持调试工具
  - 支持使用`Verdi/DVE/questasim/vivado`进行调试

* 支持预编译Xilinx仿真库
  - 支持预编译Xilinx的仿真库（包含unisims、unimacro以及secureip等）以提高仿真编译的速度


## 待优化特性
* C/C++语言支持

## 已知问题
* 无

---


<a name="应用测试"></a>

# 应用测试

## 概述

在`fpga_design/software/`下，有一个app 工程子目录，用户可以通过一些脚本代码(应用程序)编译应用工程，对工程进行特性或功能测试。详细测试方法，用户可以参考该目录下面的readme进行测试。

## 待优化特性

* 无

## 已知问题
* 无

---

<a name="工具环境"></a>

# 工具环境

* 支持的工具和环境如下：
  - Linux `centos 7.3`
  - Xilinx `Vivado 2017.2` 

---

<a name="license要求"></a>

# license要求
* 需要的license如下      
  - Xilinx Vivado 2017.2   
  - encryption tool  Version 2  

---
<a name="即将支持特性"></a>

# 即将支持特性
* peer to peer
