# 文件说明
*  vivado_design/lib/common 存放华为公司提供的通用基础模块（Common Building Block，简称CBB）；
*  vivado_design/lib/common 中的所有CBB属于公用库文件，`fpga_desgin/hardware/vivado_design`路径下的工程均可直接调用；
* 华为公司提供的CBB按文件名称区分，具体说明参见目录结构说明；

# 目录说明
* [vivado_design/lib/common](#vivado_design/lib/common_dir)/  
  平台可调用的所有CBB及功能见下表:  

  |CBB名称                      |CBB功能                                 |  
  |:----------------------------|:------------------------------------- |  
  |asyn_frm_fifo_288x512_sa     |异步帧级FIFO，位宽为288bit，升读为512;   |  
  | axi_time_out  				      |axi协议的valid和ready超时检测模块；      |  
  | axi4           				      |hpi接口转axi4接口模块；                 |  
  | axil2hpis_adp  				      |PCIe bar0/bar5 AXI-L接口适配模块；      |  
  | buft32  					          |32位三态缓冲器；                        |  
  | cmd_reg32_inst				      |命令寄存器适配模块；                    |  
  | cnt32_reg_inst				      |32位计数器访问模块；                    |  
  | count32						          |32位计数器模块；                        |  
  | ddr_ctrl					          |ddra/ddrb/ddrd网表；                    |  
  | err_wc_reg_inst				      |告警寄存器适配模块；                     |  
  | hpi2axi4lm_adp				      |PCIe用户HPI接口适配模块；                |  
  | if_cbb						          |fifo库cbb；                             |  
  | ram_def  					          |ram参数定义模块；                        |  
  | raxi_rc256_fifo.v  			    |dma接收通道AXI接口适配模块(256位)；       |  
  | raxi_rc512_fifo  			      |dma接收通道AXI接口适配模块(512位)；       |  
  | raxi_rq256_fifo				      |dma发送通道AXI接口适配模块(256位)；       |  
  | raxi_rq512_fifo				      |dma发送通道AXI接口适配模块(512位)；       |  
  | ro_reg_inst					        |只读寄存器适配模块；                      |  
  | rw_reg_inst					        |读写寄存器适配模块；                      |  
  | sdpramb_dclk				        |双时钟简单双端口ram模块；                 |  
  | sdpramb_sclk				        |单时钟简单双端口ram模块；                 |  
  | syn_frm_fifo_540x512b    	  |同步帧级FIFO，位宽为540bit，深度为512;    |  
  | ts_addr_reg_inst			      |地址取反寄存器适配模块；                  |  
  | ts_reg_inst					        |数据取反寄存器适配模块；                  |  
  | wc_reg_inst					        |写清寄存器适配模块；                     |  
  | xilinx_sdpramb_dclk			    |xilinx双时钟简单双端口ram模块；          |    
  | xilinx_sdpramb_sclk			    |xilinx单时钟简单双端口ram模块；          |  	