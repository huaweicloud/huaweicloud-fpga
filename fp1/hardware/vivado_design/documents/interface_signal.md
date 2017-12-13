# 综述
* 加速卡FPGA通过PCIe3.0 X16接口与CPU对接，外挂4片DDR4存储器，支持高速器件互联接口和100G以太网口。每片逻辑由`SHELL和Custom Logic`组成：
  - SHELL(SH)：FACS 提供的静态逻辑部分，包括PCIe、DDR4等外围接口设计；
  - Custom Logic(CL)：用户开发的动态逻辑部分；
    
  最终，用户需要用SH和CL一起编译FPGA镜像`AEI文件`；      
  这里主要描述SH和CL之间的硬件接口和功能。  

# SHELL功能
* SHELL提供PCIe bar1/bar5访问功能； 
* SHELL提供PCIe dma通道访问功能；
* SHELL提供1片DDR4映射空间访问功能；
* SHELL提供调试、状态、控制功能；

# SHELL接口
SHELL功能使用AXI-Lite或AXI-4接口和用户逻辑对接。
* bar1/bar5空间访问使用32bit AXI-Lite接口；
* dma通道访问使用512bit AXI-4接口；
* 1片DDR4映射空间访问使用512bit AXI-4接口；
		
# 时钟和复位信号
* clk_100m dma通道访问的AXI-4接口时钟，rst_100m为同步复位；
* clk_200m DDR4映射空间访问的AXI-4接口时钟，rst_200m为同步复位；
* clk_hpi bar1/bar5空间访问的AXI-Lite的接口时钟，rst_hpi为同步复位；
* 三个时钟源共用同一个PLL/VCO输出，具有严格的相位关系，clk_hpi时钟频率为200M；
	
# dma通道访问接口
* SEHLL提供的AXI-4从端信号；
  - cl2sh_dmas3_tlast
  - cl2sh_dmas3_tdata
  - cl2sh_dmas3_tkeep
  - cl2sh_dmas3_tvalid
  - sh2cl_dmas3_tready

* SEHLL提供的AXI-4主端信号；
  - sh2cl_dmam1_tlast
  - sh2cl_dmam1_tdata[511:0]
  - sh2cl_dmam1_tkeep[63:0]
  - sh2cl_dmam1_tvalid
  - cl2sh_dmam1_tready

# DDR4映射空间访问接口
* SHELL提供的AXI-4接口写地址信号；
  - cl2sh_ddr_awid[3:0]
  - cl2sh_ddr_awaddr[63:0]
  - cl2sh_ddr_awlen[6:0]
  - cl2sh_ddr_awsize[6:0]
  - cl2sh_ddr_awvalid
		sh2cl_ddr_awready
	
* SHELL提供的AXI-4接口写数据信号； 
  - cl2sh_ddr_wid[3:0]
  - cl2sh_ddr_wdata[511:0]
  - cl2sh_ddr_wstrb[63:0]
  - cl2sh_ddr_wlast
  - cl2sh_ddr_wvalid
  - sh2cl_ddr_wready
		
* SHELL提供的AXI-4接口写响应信号； 
  - sh2cl_ddr_bid[3:0]
  - sh2cl_ddr_bresp[1:0]
  - sh2cl_ddr_bvalid
  - cl2sh_ddr_bready
		
* SHELL提供的AXI-4接口读地址信号； 
  - cl2sh_ddr_arid[3:0]
  - cl2sh_ddr_araddr[63:0]
  - cl2sh_ddr_arlen[6:0]
  - cl2sh_ddr_arsize[6:0]
  - cl2sh_ddr_arvalid
  - sh2cl_ddr_arready
		
* SHELL提供的AXI-4接口读数据/响应信号； 
  - sh2cl_ddr_rid[3:0]
  - sh2cl_ddr_rdata[511:0]
  - sh2cl_ddr_rresp[1:0]
  - sh2cl_ddr_rlast
  - sh2cl_ddr_rvalid
  - cl2sh_ddr_rready

# PCIe bar1访问接口
* SHELL提供的bar1 AXI-L接口写地址信号；
  - sh2bar1_awvalid
  - sh2bar1_awaddr[31:0]
  - bar12sh_awready

* SHELL提供的bar1 AXI-L接口写数据信号；
  - sh2bar1_wvalid    
  - sh2bar1_wdata[31:0]     
  - sh2bar1_wstrb     
  - bar12sh_wready    
           
* SHELL提供的bar1 AXI-L接口写响应信号；
  - bar12sh_bvalid
  - bar12sh_bresp 
  - sh2bar1_bready
    
* SHELL提供的bar1 AXI-L接口读地址信号；
  - sh2bar1_arvalid
  - sh2bar1_araddr[31:0] 
  - bar12sh_arready
    
* SHELL提供的bar1 AXI-L接口读数据/响应信号；
  - bar12sh_rvalid 	
  - bar12sh_rdata[31:0]  
  - bar12sh_rresp  
  - sh2bar1_rready
        
# PCIe bar5访问接口
* SHELL提供的bar5 AXI-L接口写地址信号；
  - sh2bar5_awvalid
  - sh2bar5_awaddr[31:0]
  - bar52sh_awready

* SHELL提供的bar5 AXI-L接口写数据信号；
  - sh2bar5_wvalid    
  - sh2bar5_wdata[31:0]     
  - sh2bar5_wstrb     
  - bar52sh_wready    
           
* SHELL提供的bar5 AXI-L接口写响应信号；
  - bar52sh_bvalid
  - bar52sh_bresp 
  - sh2bar5_bready
    
* SHELL提供的bar5 AXI-L接口读地址信号；
  - sh2bar5_arvalid
  - sh2bar5_araddr[31:0] 
  - bar52sh_arready
    
* SHELL提供的bar5 AXI-L接口读数据/响应信号；
  - bar52sh_rvalid 	
  - bar52sh_rdata[31:0]  
  - bar52sh_rresp  
  - sh2bar5_rready   