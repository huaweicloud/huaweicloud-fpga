
  //--------------------------------
  // Globals
  //--------------------------------
  input                           clk_200m                                 ,     
  input                           clk_a                                    ,     
  input                           clk_b                                    ,     
                                                                            
  input                           rst_200m                                 ,    
  input                           rst_a                                    ,    
  input                           rst_b                                    ,    

  //-------------------------------------------------------------------------------------------
  // AXI-S interface for DMA SLAVE. dmas2:AE to VE read command
  //-------------------------------------------------------------------------------------------
  output                          ul2sh_dmas2_tlast                        ,
  output   [255:0]                ul2sh_dmas2_tdata                        ,
  output   [31:0]                 ul2sh_dmas2_tkeep                        ,
  output                          ul2sh_dmas2_tvalid                       ,
  input                           sh2ul_dmas2_tready                       ,                               
  
  //-------------------------------------------------------------------------------------------
  // AXI-S interface for DMA SLAVE. dmas3:AE to VE PKT
  //-------------------------------------------------------------------------------------------
      
  output                          ul2sh_dmas3_tlast                        ,
  output   [511:0]                ul2sh_dmas3_tdata                        ,
  output   [63:0]                 ul2sh_dmas3_tkeep                        ,
  output                          ul2sh_dmas3_tvalid                       ,
  input                           sh2ul_dmas3_tready                       ,     
  
  //-------------------------------------------------------------------------------------------
  //  AXI-S interface for DMA MASTER. dmas0:VE to AE BD
  //-------------------------------------------------------------------------------------------
  input                           sh2ul_dmam0_tlast                        ,
  input   [255:0]                 sh2ul_dmam0_tdata                        ,
  input   [31:0]                  sh2ul_dmam0_tkeep                        ,
  input                           sh2ul_dmam0_tvalid                       ,
  output                          ul2sh_dmam0_tready                       ,                               
  
  //-------------------------------------------------------------------------------------------
  //  AXI-S interface for DMA MASTER. dmas1:VE to AE PKT
  //-------------------------------------------------------------------------------------------
  input                           sh2ul_dmam1_tlast                        ,
  input   [511:0]                 sh2ul_dmam1_tdata                        ,
  input   [63:0]                  sh2ul_dmam1_tkeep                        ,
  input                           sh2ul_dmam1_tvalid                       ,
  output                          ul2sh_dmam1_tready                       ,     

  
  //-----------------------------------------------------------------------------------
  // AXI4 Interface for DDR_C 
  // This is the DDR controller that is instantiated in the SH.  UL is the AXI-4
  // master and the DDR_C controller in the SH is the slave.
  //-----------------------------------------------------------------------------------
  //Write address 
  output  [3:0]                   ul2sh_ddr_awid                           ,  
  output  [63:0]                  ul2sh_ddr_awaddr                         ,
  output  [7:0]                   ul2sh_ddr_awlen                          ,
  output  [2:0]                   ul2sh_ddr_awsize                         ,
  output                          ul2sh_ddr_awvalid                        ,
  input                           sh2ul_ddr_awready                        ,
                                                                            
  //Write data                                                             
  output  [3:0]                   ul2sh_ddr_wid                            ,
  output  [511:0]                 ul2sh_ddr_wdata                          ,
  output  [63:0]                  ul2sh_ddr_wstrb                          ,
  output                          ul2sh_ddr_wlast                          ,
  output                          ul2sh_ddr_wvalid                         ,
  input                           sh2ul_ddr_wready                         ,
                                                                           
  //Write response 	                                                    
  input   [3:0]                   sh2ul_ddr_bid                            ,
  input   [1:0]                   sh2ul_ddr_bresp                          ,
  input                           sh2ul_ddr_bvalid                         ,
  output                          ul2sh_ddr_bready                         ,
     
  //Read address 
  output  [3:0]                   ul2sh_ddr_arid                           ,  
  output  [63:0]                  ul2sh_ddr_araddr                         ,
  output  [7:0]                   ul2sh_ddr_arlen                          ,
  output  [2:0]                   ul2sh_ddr_arsize                         ,
  output                          ul2sh_ddr_arvalid                        ,
  input                           sh2ul_ddr_arready                        ,
  
  //Read data/response                                                                                                       
  input   [3:0]                   sh2ul_ddr_rid                            ,
  input   [511:0]                 sh2ul_ddr_rdata                          ,
  input   [1:0]                   sh2ul_ddr_rresp                          ,
  input                           sh2ul_ddr_rlast                          ,
  input                           sh2ul_ddr_rvalid                         ,
  output                          ul2sh_ddr_rready                         ,
  //------------------------------------------------------------------------------------------
  // AXI4 Interface for BAR1
  //------------------------------------------------------------------------------------------
  //Write address 
  input                           sh2bar1_awvalid                          ,
  input  [ADDR_WIDTH-1:0]         sh2bar1_awaddr                           ,
  output                          bar12sh_awready                          ,
                                                                                                                               
  //Write data                                                                                                                     
  input                           sh2bar1_wvalid                           ,
  input  [DATA_WIDTH-1:0]         sh2bar1_wdata                            ,
  input  [DATA_BYTE_NUM-1:0]      sh2bar1_wstrb                            ,
  output                          bar12sh_wready                           ,
                                                                                                                             
  //Write response                                                                                                            
  output                          bar12sh_bvalid                           ,
  output [1:0]                    bar12sh_bresp                            ,
  input                           sh2bar1_bready                           ,
   
  //Read address                                                                                                             
  input                           sh2bar1_arvalid                          ,
  input  [ADDR_WIDTH-1:0]         sh2bar1_araddr                           ,
  output                          bar12sh_arready                          ,
                                                                                                                             
  //Read data/response                                                                                                       
  output                          bar12sh_rvalid                           ,
  output [DATA_WIDTH-1:0]         bar12sh_rdata                            ,
  output [1:0]                    bar12sh_rresp                            ,                                                                                                                               
  input                           sh2bar1_rready                           ,
                                                                                                                              
  //------------------------------------------------------------------------------------------
  // AXI4 Interface for BAR2
  //------------------------------------------------------------------------------------------
  //Write address 
  input                           sh2bar5_awvalid                          ,
  input  [ADDR_WIDTH-1:0]         sh2bar5_awaddr                           ,
  output                          bar52sh_awready                          ,
                                                                                                                                                           
  //Write data                                                                                                                                           
  input                           sh2bar5_wvalid                           ,
  input  [DATA_WIDTH-1:0]         sh2bar5_wdata                            ,
  input  [DATA_BYTE_NUM-1:0]      sh2bar5_wstrb                            ,
  output                          bar52sh_wready                           ,
                                                                                                                                                      
  //Write response                                                                                                                                      
  output                          bar52sh_bvalid                           ,
  output [1:0]                    bar52sh_bresp                            ,
  input                           sh2bar5_bready                           ,
                                  
  //Read address                                                                                                                           
  input                           sh2bar5_arvalid                          ,
  input  [ADDR_WIDTH-1:0]         sh2bar5_araddr                           ,
  output                          bar52sh_arready                          ,
                                                                                                                             
  //Read data/response                                                                                                       
  output                          bar52sh_rvalid                           ,
  output [31:0]                   bar52sh_rdata                            ,
  output [1:0]                    bar52sh_rresp                            ,                                                                                                                               
  input                           sh2bar5_rready                           ,
 
  //------------------------------------------------------------------------------------------
  // DDRA,DDRB,DDRD
  //------------------------------------------------------------------------------------------
  input                           ddra_100m_ref_clk_p                      ,
  input                           ddra_100m_ref_clk_n                      ,
  input                           ddrb_100m_ref_clk_p                      ,
  input                           ddrb_100m_ref_clk_n                      ,
  input                           ddrd_100m_ref_clk_p                      ,
  input                           ddrd_100m_ref_clk_n                      ,

  output                          ddra_72b_act_n                           ,
  output [16:0]                   ddra_72b_addr                            ,
  output [1:0]                    ddra_72b_ba                              ,
  output [1:0]                    ddra_72b_bg                              ,
  output [1:0]                    ddra_72b_cke                             ,
  output [1:0]                    ddra_72b_odt                             ,
  output [1:0]                    ddra_72b_cs_n                            ,
  output                          ddra_72b_ck_t                            ,
  output                          ddra_72b_ck_c                            ,
  output                          ddra_72b_rst_n                           ,
  output                          ddra_72b_par                             ,
  inout  [71:0]                   ddra_72b_dq                              ,
  inout  [8:0]                    ddra_72b_dqs_t                           ,
  inout  [8:0]                    ddra_72b_dqs_c                           ,
  inout  [8:0]                    ddra_72b_dm_dbi_n                        ,
  
  output                          ddrb_72b_act_n                           ,
  output [16:0]                   ddrb_72b_addr                            ,
  output [1:0]                    ddrb_72b_ba                              ,
  output [1:0]                    ddrb_72b_bg                              ,
  output [1:0]                    ddrb_72b_cke                             ,
  output [1:0]                    ddrb_72b_odt                             ,
  output [1:0]                    ddrb_72b_cs_n                            ,
  output                          ddrb_72b_ck_t                            ,
  output                          ddrb_72b_ck_c                            ,
  output                          ddrb_72b_rst_n                           ,
  output                          ddrb_72b_par                             ,
  inout  [71:0]                   ddrb_72b_dq                              ,
  inout  [8:0]                    ddrb_72b_dqs_t                           ,
  inout  [8:0]                    ddrb_72b_dqs_c                           ,
  inout  [8:0]                    ddrb_72b_dm_dbi_n                        ,
  
  output                          ddrd_72b_act_n                           ,
  output [16:0]                   ddrd_72b_addr                            ,
  output [1:0]                    ddrd_72b_ba                              ,
  output [1:0]                    ddrd_72b_bg                              ,
  output [1:0]                    ddrd_72b_cke                             ,
  output [1:0]                    ddrd_72b_odt                             ,
  output [1:0]                    ddrd_72b_cs_n                            ,
  output                          ddrd_72b_ck_t                            ,
  output                          ddrd_72b_ck_c                            ,
  output                          ddrd_72b_rst_n                           ,
  output                          ddrd_72b_par                             ,
  inout  [71:0]                   ddrd_72b_dq                              ,
  inout  [8:0]                    ddrd_72b_dqs_t                           ,
  inout  [8:0]                    ddrd_72b_dqs_c                           ,
  inout  [8:0]                    ddrd_72b_dm_dbi_n                        ,
  
  output [15:0]                   ul2sh_vled                               ,         
  //------------------------------------------------------------------------------------------
  // Debug bridge
  //------------------------------------------------------------------------------------------
(* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 S_BSCAN drck" *) (* DEBUG="true" *)
input S_BSCAN_drck,
(* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 S_BSCAN shift" *) (* DEBUG="true" *)
input S_BSCAN_shift,
(* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 S_BSCAN tdi" *) (* DEBUG="true" *)
input S_BSCAN_tdi,
(* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 S_BSCAN update" *) (* DEBUG="true" *)
input S_BSCAN_update,
(* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 S_BSCAN sel" *) (* DEBUG="true" *)
input S_BSCAN_sel,
(* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 S_BSCAN tdo" *) (* DEBUG="true" *)
output S_BSCAN_tdo,
(* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 S_BSCAN tms" *) (* DEBUG="true" *)
input S_BSCAN_tms,
(* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 S_BSCAN tck" *) (* DEBUG="true" *)
input S_BSCAN_tck,
(* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 S_BSCAN runtest" *) (* DEBUG="true" *)
input S_BSCAN_runtest,
(* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 S_BSCAN reset" *) (* DEBUG="true" *)
input S_BSCAN_reset,
(* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 S_BSCAN capture" *) (* DEBUG="true" *)
input S_BSCAN_capture,
(* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 S_BSCAN bscanid_en" *)
(* DEBUG="true" *) input S_BSCAN_bscanid_en              
     
