# 目录结构

Documents文件夹下，总共包含如下文档：

* [documents](#documents_dir)/
  - example1.jpg 示例1的逻辑结构框图；
  - example2.jpg 示例2的逻辑结构框图；
  - example3.jpg 示例3的逻辑结构框图；
  - interface_signal.md 静态逻辑和动态逻辑之间接口信号说明；  
  - Pcie_Memory_Map.md Pcie 存储空间划分说明；  
  - requirements for tools and license.md 工具及license要求说明；  
  - SH_UL_interface.jpg 静态逻辑和动态逻辑之间接口示图；  
  - README.md（本文档） 

# 目录说明

* example1

  ![example1.jpg ](./example1.jpg)

  - example1的框图中，UL_VER的功能是例化ro_reg_inst CBB，通过`app`读取到的版本号是example1发布的时间信息。
  - UL_TYPE的功能是例化ro_reg_inst CBB，通过读该寄存器可获得当前example的信息`32'h00d10001`。
  - DATA_TEST的功能是例化ts_reg_inst CBB，实现对`写入数据取反`功能。
  - ADDR_TEST的功能是例化ts_addr_reg_inst CBB，实现对最近一次操作`地址的取反`功能。
  - ADDER例化`两个rw_reg_inst CBB`分别为加法器的加数和被加数，再例化`一个ro_reg_inst CBB`实现加法器的结果读取。
  - VLED在`PF`下面访问，由静态提供管脚给动态用户使用，用户可以读写VLED，确保`UL`部分工作正常。VLED例化一组rw_reg_inst CBB将输出结果链接到VLED。
  - 所有的DDR部分用户接口输入信号全部例化`unused_ddr_a_b_d_inst.h`和`unused_ddr_c_inst.h `连接成0。
  - 例化DEBUG_BRIDGE和ILA0，供用户调试使用，ILA的调试信号开放`八根`。

* example2

  ![example2.jpg ](./example2.jpg)

  - example2的框图中，UL_VER的功能是例化ro_reg_inst CBB，版本号是通过`app`读取到的example2发布的时间信息。
  - UL_TYPE的功能是例化ro_reg_inst CBB，通过读该寄存器可获得当前example的信息是`32'h00d20001`。
  - DATA_TEST的功能是例化ts_reg_inst CBB，实现对`写入数据取反`功能。
  - ADDR_TEST的功能是例化ts_addr_reg_inst CBB，实现对最近一次操作`地址的取反`功能。
  - DMA_UL的功能是向host发起读报文请求再将收到的报文送回到host，实现数据`x86->host->用户逻辑->host->x86`路径的`dma数据环回`。
  - DDR_WR_RD实现对`4组`DDR的数据通道的读写访问功能。
  - VLED在`PF`下面访问，由静态提供管脚给动态用户使用，用户可以读写VLED，确保`UL`部分工作正常。
  - VLED例化一组rw_reg_instCBB将输出结果连接到VLED。
  - ADDER是加法器，实现对输入数据求和。
  - 例化DEBUG_BRIDGE和ILA0，供用户调试使用，ILA的调试信号开放`八根`。

* example3

  ![example3.jpg ](./example3.jpg)

  - example3的框图中，UL_VER的功能是例化ro_reg_inst CBB，版本号是通过`app`读取到的example3发布的时间信息。
  - UL_TYPE的功能是例化ro_reg_inst CBB，通过读该寄存器可获得当前example的信息是`32'h00d30001`。
  - DATA_TEST的功能是例化ts_reg_inst CBB，实现对`写入数据取反`功能。
  - ADDR_TEST的功能是例化ts_addr_reg_inst CBB，实现对最近一次操作`地址的取反`功能。
  - MMU_UL的功能有两个，其一是向host发起读数据请求再将收到的数据写入DDR，其二是读取用户写入DDR处理后的数据并送往host。
  - KERNEL_UL的功能是读取MMU_UL写入DDR的数据，处理完成后再写入DDR，等待MMU_UL取走这些数据。
  - VLED在`PF`下面访问，由静态提供管脚给动态用户使用，用户可以读写VLED，确保`UL`部分工作正常。
  - VLED例化一组rw_reg_instCBB将输出结果连接到VLED。
  - ADDER是加法器，实现对输入数据求和。
  - 例化DEBUG_BRIDGE和ILA0，供用户调试使用，ILA的调试信号开放`八根`。
* interface_signal.md
  - 描述了用户逻辑和静态逻辑之间的所有接口信号。 
* requirements for tools and license.md  
  - 描述了fpga_design正常运行的工具及`license`要求说明；
* pcie_memory_map.md  
  - 主要描述`pcie`的存储空间划分。
* sh_ul_interface.jpg   
  - 用结构框图的方式，主要描述静态逻辑和用户设计的动态逻辑之间可用的`接口类型和接口位宽`信息;
  - SH部分为逻辑接口的静态部分，用户只需要知道其组成即可;
  - UL部分为逻辑接口的动态部分，用户可以在这部分实现自己的个性化需求。
* README.md
  - 即本文档，用于介绍其他文档。
