//
//------------------------------------------------------------------------------
//     Copyright (c) 2017 Huawei Technologies Co., Ltd. All Rights Reserved.
//
//     This program is free software; you can redistribute it and/or modify
//     it under the terms of the Huawei Software License (the "License").
//     A copy of the License is located in the "LICENSE" file accompanying 
//     this file.
//
//     This program is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//     Huawei Software License for more details. 
//------------------------------------------------------------------------------

`ifndef _TB_TOP_SV_
`define _TB_TOP_SV_

`timescale 1ns/1ps

// Include files {{{

// ./tb_pkg.svh
`include "tb_pkg.svh"

// ./config_db.svh
`include "config_db.svh"

// ./bfm/tb_interface.sv
`include "tb_interface.sv"

// ./bfm/tb_vif_obj.sv
`include "tb_vif_obj.sv"

// ./common/clock_gen.sv
`include "clock_gen.sv"

// ./common/reset_gen.sv
`include "reset_gen.sv"

// }}}

// --------------------------------------------------------------------------------------------------------------------
//     +------------------------------------------------------------+
//     |  +-------------+  +-----------+         +---------------+  |
//     |  | u_clk_200m  |  |           |         |               |  |
//     |  +-------------+  |           |         |               |  |
//     |  +-------------+  |           |         |               |  |
//     |  | u_clk_100m  |  |           |         |               |  |
//     |  +-------------+  |           |         |               |  |
//     |                   |   tb_if   | tb_top  |     u_dut     |  |
//     |  +-------------+  |           |         |               |  |
//     |  | u_rst_200m  |  |           |         |               |  |
//     |  +-------------+  |           |         |               |  |
//     |  +-------------+  |           |         |               |  |
//     |  | u_rst_100m  |  |           |         |               |  |
//     |  +-------------+  +-----------+         +---------------+  |
//     +------------------------------------------------------------+
// --------------------------------------------------------------------------------------------------------------------

module tb_top();

// Testbench interface top

tb_interface tb_if();

// Generate 200m clock

clock_gen #(.FREQ       (200000), // 200M frequency
            .PERIOD     (      ),
            .DUTY       (      ),
            .OFFSET_MIN (1000  ),
            .OFFSET_MAX (10000 ),
            .DEFAULT_VAL(      )) u_clk_200m(.clk_p ( tb_if.clk_200m     ),
                                             .clk_n (                    ));

// Generate 100m clock

clock_gen #(.FREQ       (100000),
            .PERIOD     (      ),
            .DUTY       (      ),
            .OFFSET_MIN (1000  ), 
            .OFFSET_MAX (5000  ),
            .DEFAULT_VAL(      )) u_clk_100m(.clk_p ( tb_if.clk_100m_p   ),
                                             .clk_n ( tb_if.clk_100m_n   ));

// Generate 200m reset

reset_gen #(.TIME       (10000 ), 
            .VALUE      (1     ),
            .OFFSET_MIN (1000  ), 
            .OFFSET_MAX (10000 ),
            .SYNC_RLS   (1     ),
            .DEFAULT_VAL(0     )) u_rst_200m(.clk   ( tb_if.clk_200m     ),
                                             .reset ( tb_if.rst_200m     ),
                                             .done  ( tb_if.rst_200m_done));

// Generate 100m reset

reset_gen #(.TIME       (15000 ), 
            .VALUE      (1     ),
            .OFFSET_MIN (1000  ), 
            .OFFSET_MAX (5000  ),
            .SYNC_RLS   (1     ),
            .DEFAULT_VAL(0     )) u_rst_100m(.clk   ( tb_if.clk_100m_p   ),
                                             .reset ( tb_if.rst_100m     ),
                                             .done  ( tb_if.rst_100m_done));

// If DISABLE_DUT defined, do not instantiate USER_TOP {{{
`ifndef DISABLE_DUT

// Userdefine top name, if not define, using cl_pr_top as default
`ifndef USER_TOP
  `define USER_TOP ul_pr_top
`endif

`USER_TOP #(
    .ADDR_WIDTH   ( `AXI4L_ADDR_WIDTH ),
    .DATA_WIDTH   ( `AXI4L_DATA_WIDTH )
) u_dut (
    //--------------------------------
    // Globals
    //--------------------------------
    .clk_200m           ( tb_if.clk_200m               ),
    .rst_200m           ( tb_if.rst_200m               ),
    .clk_a              ( tb_if.clk_100m_p             ),
    .rst_a              ( tb_if.rst_100m               ),
    .clk_b              ( tb_if.clk_100m_p             ),
    .rst_b              ( tb_if.rst_100m               ),

    //-------------------------------------------------------------------------------------------
    // AXI-4 interface for DMA SLAVE
    //-------------------------------------------------------------------------------------------
    // Command Interface
    .ul2sh_dmas2_tdata  ( tb_if.u_axissc_if.ddata      ),
    .ul2sh_dmas2_tkeep  ( tb_if.u_axissc_if.dkeep      ),
    .ul2sh_dmas2_tlast  ( tb_if.u_axissc_if.dlast      ),
    .ul2sh_dmas2_tvalid ( tb_if.u_axissc_if.dvalid     ),
    .sh2ul_dmas2_tready ( tb_if.u_axissc_if.dready     ),
    // Data Interface
    .ul2sh_dmas3_tdata  ( tb_if.u_axissd_if.ddata      ),
    .ul2sh_dmas3_tkeep  ( tb_if.u_axissd_if.dkeep      ),
    .ul2sh_dmas3_tlast  ( tb_if.u_axissd_if.dlast      ),
    .ul2sh_dmas3_tvalid ( tb_if.u_axissd_if.dvalid     ),
    .sh2ul_dmas3_tready ( tb_if.u_axissd_if.dready     ),

    //-------------------------------------------------------------------------------------------
    //  AXI-4 interface for DMA MASTER
    //-------------------------------------------------------------------------------------------
    // Command Interface
    .sh2ul_dmam0_tdata  ( tb_if.u_axismc_if.ddata      ),
    .sh2ul_dmam0_tkeep  ( tb_if.u_axismc_if.dkeep      ),
    .sh2ul_dmam0_tlast  ( tb_if.u_axismc_if.dlast      ),
    .sh2ul_dmam0_tvalid ( tb_if.u_axismc_if.dvalid     ),
    .ul2sh_dmam0_tready ( tb_if.u_axismc_if.dready     ),
    // Data Interface
    .sh2ul_dmam1_tdata  ( tb_if.u_axismd_if.ddata      ),
    .sh2ul_dmam1_tkeep  ( tb_if.u_axismd_if.dkeep      ),
    .sh2ul_dmam1_tlast  ( tb_if.u_axismd_if.dlast      ),
    .sh2ul_dmam1_tvalid ( tb_if.u_axismd_if.dvalid     ),
    .ul2sh_dmam1_tready ( tb_if.u_axismd_if.dready     ),

    //-----------------------------------------------------------------------------------
    // AXI4 Interface for DDR_C 
    // This is the DDR controller that is instantiated in the SH.  CL is the AXI-4
    // master and the DDR_C controller in the SH is the slave.
    //-----------------------------------------------------------------------------------
`ifdef USE_DDR_MODEL
    // Write address
    .ul2sh_ddr_awid     ( tb_if.u_axisd_if.awid        ),
    .ul2sh_ddr_awaddr   ( tb_if.u_axisd_if.awaddr      ),
    .ul2sh_ddr_awlen    ( tb_if.u_axisd_if.awlen       ),
    .ul2sh_ddr_awsize   ( tb_if.u_axisd_if.awsize      ),
    .ul2sh_ddr_awvalid  ( tb_if.u_axisd_if.awvalid     ),
    .sh2ul_ddr_awready  ( tb_if.u_axisd_if.awready     ),
    // Write data
    .ul2sh_ddr_wid      ( tb_if.u_axisd_if.wid         ),
    .ul2sh_ddr_wdata    ( tb_if.u_axisd_if.wdata       ),
    .ul2sh_ddr_wstrb    ( tb_if.u_axisd_if.wstrb       ),
    .ul2sh_ddr_wlast    ( tb_if.u_axisd_if.wlast       ),
    .ul2sh_ddr_wvalid   ( tb_if.u_axisd_if.wvalid      ),
    .sh2ul_ddr_wready   ( tb_if.u_axisd_if.wready      ),
    // Write response
    .sh2ul_ddr_bid      ( tb_if.u_axisd_if.bid         ),
    .sh2ul_ddr_bresp    ( tb_if.u_axisd_if.bresp       ),
    .sh2ul_ddr_bvalid   ( tb_if.u_axisd_if.bvalid      ),
    .ul2sh_ddr_bready   ( tb_if.u_axisd_if.bready      ),
    // Read address
    .ul2sh_ddr_arid     ( tb_if.u_axisd_if.arid        ),
    .ul2sh_ddr_araddr   ( tb_if.u_axisd_if.araddr      ),
    .ul2sh_ddr_arlen    ( tb_if.u_axisd_if.arlen       ),
    .ul2sh_ddr_arsize   ( tb_if.u_axisd_if.arsize      ),
    .ul2sh_ddr_arvalid  ( tb_if.u_axisd_if.arvalid     ),
    .sh2ul_ddr_arready  ( tb_if.u_axisd_if.arready     ),
    // Read data
    .sh2ul_ddr_rid      ( tb_if.u_axisd_if.rid         ),
    .sh2ul_ddr_rdata    ( tb_if.u_axisd_if.rdata       ),
    .sh2ul_ddr_rresp    ( tb_if.u_axisd_if.rresp       ),
    .sh2ul_ddr_rlast    ( tb_if.u_axisd_if.rlast       ),
    .sh2ul_ddr_rvalid   ( tb_if.u_axisd_if.rvalid      ),
    .ul2sh_ddr_rready   ( tb_if.u_axisd_if.rready      ),
`else
    //Write address 
    .ul2sh_ddr_awid     (                              ),
    .ul2sh_ddr_awaddr   (                              ),
    .ul2sh_ddr_awlen    (                              ),
    .ul2sh_ddr_awsize   (                              ),
    .ul2sh_ddr_awvalid  (                              ),
    .sh2ul_ddr_awready  (                              ),
    //Write data
    .ul2sh_ddr_wid      (                              ),
    .ul2sh_ddr_wdata    (                              ),
    .ul2sh_ddr_wstrb    (                              ),
    .ul2sh_ddr_wlast    (                              ),
    .ul2sh_ddr_wvalid   (                              ),
    .sh2ul_ddr_wready   (                              ),
    //Write response
    .sh2ul_ddr_bid      (                              ),
    .sh2ul_ddr_bresp    (                              ),
    .sh2ul_ddr_bvalid   (                              ),
    .ul2sh_ddr_bready   (                              ),
    // Read address
    .ul2sh_ddr_arid     (                              ),
    .ul2sh_ddr_araddr   (                              ),
    .ul2sh_ddr_arlen    (                              ),
    .ul2sh_ddr_arsize   (                              ),
    .ul2sh_ddr_arvalid  (                              ),
    .sh2ul_ddr_arready  (                              ),
    // Read data
    .sh2ul_ddr_rid      (                              ),
    .sh2ul_ddr_rdata    (                              ),
    .sh2ul_ddr_rresp    (                              ),
    .sh2ul_ddr_rlast    (                              ),
    .sh2ul_ddr_rvalid   (                              ),
    .ul2sh_ddr_rready   (                              ),
`endif

    //------------------------------------------------------------------------------------------
    // AXI4 Interface for BAR1
    //------------------------------------------------------------------------------------------
    //Write address
    .sh2bar1_awvalid    ( tb_if.u_axil1_if.awvalid     ),
    .sh2bar1_awaddr     ( tb_if.u_axil1_if.awaddr      ),
    .bar12sh_awready    ( tb_if.u_axil1_if.awready     ),
    //Write data
    .sh2bar1_wvalid     ( tb_if.u_axil1_if.wvalid      ),
    .sh2bar1_wdata      ( tb_if.u_axil1_if.wdata       ),
    .sh2bar1_wstrb      ( tb_if.u_axil1_if.wstrb       ),
    .bar12sh_wready     ( tb_if.u_axil1_if.wready      ),
    //Write response
    .bar12sh_bvalid     ( tb_if.u_axil1_if.bvalid      ),
    .bar12sh_bresp      ( tb_if.u_axil1_if.bresp       ),
    .sh2bar1_bready     ( tb_if.u_axil1_if.bready      ),
    //Read address
    .sh2bar1_arvalid    ( tb_if.u_axil1_if.arvalid     ),
    .sh2bar1_araddr     ( tb_if.u_axil1_if.araddr      ),
    .bar12sh_arready    ( tb_if.u_axil1_if.arready     ),
    //Read data/response
    .bar12sh_rvalid     ( tb_if.u_axil1_if.rvalid      ),
    .bar12sh_rdata      ( tb_if.u_axil1_if.rdata       ),
    .bar12sh_rresp      ( tb_if.u_axil1_if.rresp       ),
    .sh2bar1_rready     ( tb_if.u_axil1_if.rready      ),

    //------------------------------------------------------------------------------------------
    // AXI4 Interface for BAR2
    //------------------------------------------------------------------------------------------
    //Write address
    .sh2bar5_awvalid    ( tb_if.u_axil2_if.awvalid     ),
    .sh2bar5_awaddr     ( tb_if.u_axil2_if.awaddr      ),
    .bar52sh_awready    ( tb_if.u_axil2_if.awready     ),
    //Write data
    .sh2bar5_wvalid     ( tb_if.u_axil2_if.wvalid      ),
    .sh2bar5_wdata      ( tb_if.u_axil2_if.wdata       ),
    .sh2bar5_wstrb      ( tb_if.u_axil2_if.wstrb       ),
    .bar52sh_wready     ( tb_if.u_axil2_if.wready      ),
    //Write response
    .bar52sh_bvalid     ( tb_if.u_axil2_if.bvalid      ),
    .bar52sh_bresp      ( tb_if.u_axil2_if.bresp       ),
    .sh2bar5_bready     ( tb_if.u_axil2_if.bready      ),
    //Read address
    .sh2bar5_arvalid    ( tb_if.u_axil2_if.arvalid     ),
    .sh2bar5_araddr     ( tb_if.u_axil2_if.araddr      ),
    .bar52sh_arready    ( tb_if.u_axil2_if.arready     ),
    //Read data/response
    .bar52sh_rvalid     ( tb_if.u_axil2_if.rvalid      ),
    .bar52sh_rdata      ( tb_if.u_axil2_if.rdata       ),
    .bar52sh_rresp      ( tb_if.u_axil2_if.rresp       ),
    .sh2bar5_rready     ( tb_if.u_axil2_if.rready      ),

    //------------------------------------------------------------------------------------------
    // DDRA,DDRB,DDRD
    //------------------------------------------------------------------------------------------
    // Connect dut and ddr rdimma
    `tb_ddr_dut_connect_rdimm(a)
    // Connect dut and ddr rdimmb
    `tb_ddr_dut_connect_rdimm(b)
    // Connect dut and ddr rdimmd
    `tb_ddr_dut_connect_rdimm(d)
    //------------------------------------------------------------------------------------------
    // Vled
    //------------------------------------------------------------------------------------------
    .ul2sh_vled         (),

    //------------------------------------------------------------------------------------------
    // Debug bridge
    //------------------------------------------------------------------------------------------
    .S_BSCAN_drck       (),
    .S_BSCAN_shift      (),
    .S_BSCAN_tdi        (),
    .S_BSCAN_update     (),
    .S_BSCAN_sel        (),
    .S_BSCAN_tdo        (),
    .S_BSCAN_tms        (),
    .S_BSCAN_tck        (),
    .S_BSCAN_runtest    (),
    .S_BSCAN_reset      (),
    .S_BSCAN_capture    (),
    .S_BSCAN_bscanid_en ()
);

`ifndef NO_DDR_INST
    // Instantiated DDRA rdimm
    `tb_inst_ddr_rdimm(a, tb_if.u_ddra_if, 
                       `DDRA_ADDR_WIDTH, 
                       `DDRA_DATA_WIDTH, 
                       `DDRA_DATA_BYTES, 
                       `DDRA_RANK_WIDTH, 
                       `DDRA_BANK_WIDTH, 
                       `DDRA_BG_WIDTH, 
                       `DDRA_TCK)
    // Instantiated DDRB rdimm
    `tb_inst_ddr_rdimm(b, tb_if.u_ddrb_if, 
                       `DDRB_ADDR_WIDTH, 
                       `DDRB_DATA_WIDTH, 
                       `DDRB_DATA_BYTES, 
                       `DDRB_RANK_WIDTH, 
                       `DDRB_BANK_WIDTH, 
                       `DDRB_BG_WIDTH, 
                       `DDRB_TCK)
    // Instantiated DDRD rdimm
    `tb_inst_ddr_rdimm(d, tb_if.u_ddrd_if, 
                       `DDRD_ADDR_WIDTH, 
                       `DDRD_DATA_WIDTH, 
                       `DDRD_DATA_BYTES, 
                       `DDRD_RANK_WIDTH, 
                       `DDRD_BANK_WIDTH, 
                       `DDRD_BG_WIDTH, 
                       `DDRD_TCK)
`endif

`ifndef USE_DDR_MODEL

`else

assign tb_if.u_axisd_if.rstrb = {`AXI4_STRB_WIDTH{1'd1}};
`endif
`endif // DISABLE_DUT }}}

// Set virtual interface to all components of tb and start testbench
// {{{
initial begin
    bit          val;
    tb_vif_obj   tb_vif;
    axil_vif_obj axil_vif;

    axisc_vif_t  axismc_vif;
    axisd_vif_t  axismd_vif;
    axisc_vif_t  axissc_vif;
    axisd_vif_t  axissd_vif;

`ifdef USE_DDR_MODEL
    axid_vif_t   axisd_vif;
`endif

    // Bind virtual interface and interface {{{
    // Export axi4-lite interfaces to one level to avoid the error of vivado
    axil_vif  = new(tb_if.u_axil1_if);
    // Export axi4-stream interfaces to one level to avoid the error of vivado
    axismc_vif= new(tb_if.u_axismc_if);
    tb_if.u_axismc_if.initial_bus_master();
    // Export axi4-stream interfaces to one level to avoid the error of vivado
    axismd_vif= new(tb_if.u_axismd_if);
    tb_if.u_axismd_if.initial_bus_master();
    // Export axi4-stream interfaces to one level to avoid the error of vivado
    axissc_vif= new(tb_if.u_axissc_if);
    tb_if.u_axissc_if.initial_bus_slave();
    // Export axi4-stream interfaces to one level to avoid the error of vivado
    axissd_vif= new(tb_if.u_axissd_if);
    tb_if.u_axissd_if.initial_bus_slave();
`ifdef USE_DDR_MODEL
    // Export axi4 interfaces to one level to avoid the error of vivado
    axisd_vif= new(tb_if.u_axisd_if);
    tb_if.u_axisd_if.initial_bus_slave();
`endif
    // Export all interfaces to one level to avoid the error of vivado
    tb_vif    = new(.tb_vif     ( tb_if      ), 
                    .axil_vif   ( axil_vif   ), 
                    .axismc_vif ( axismc_vif ), 
                    .axismd_vif ( axismd_vif ), 
                    .axissc_vif ( axissc_vif ), 
                    .axissd_vif ( axissd_vif )
                `ifdef USE_DDR_MODEL
                   ,.axisd_vif  ( axisd_vif  )
                `endif
                   );
    // }}}
 
    // Set to config_db
    val = config_db#(tb_vif_obj)::set("tb_vif", tb_vif);
    // Run Testbench
    run_test();
end
// }}}

// Dump waveform
// {{{
initial begin
    bit dump_fsdb;
    bit dump_vpd;
    bit dump_mem;
    string wave_dir;
    // Get waveform dumping param
    if (!$value$plusargs("DUMP_FSDB=%d", dump_fsdb)) dump_fsdb = 'd0;
    if (!$value$plusargs("DUMP_VPD=%d",  dump_vpd))  dump_vpd  = 'd0;
    if (!$value$plusargs("DUMP_MEM=%d",  dump_mem))  dump_mem  = 'd0;
    if (!$value$plusargs("WAVE_DIR=%s",  wave_dir))  wave_dir  = "./";
`ifdef VCS
    if (dump_fsdb == 'd1) begin
    `ifdef VERDI
        $fsdbAutoSwitchDumpfile (2048, {wave_dir, "/tb_top.fsdb"}, 20);
        $fsdbDumpvars(9, tb_top);
        //$fsdbDumpSVA(7,  u_dut_top);
        //$fsdbDumpvarsToFile("uuu.lst");  Location: 08_sim_work/uuu.lst
        $fsdbDumpflush;
        if (dump_mem == 'd1) $fsdbDumpMDA;
    `endif
    end else if (dump_vpd == 'd1) begin
        $vcdplusfile({wave_dir, "/tb_top.vpd"});
        $vcdpluson(0, tb_top); 
        if (dump_mem == 'd1) $vcdplusmemon;
    end
`elsif QUESTA
    if (dump_fsdb == 'd1) begin
    `ifdef VERDI
        $fsdbAutoSwitchDumpfile (2048, {wave_dir, "/tb_top.fsdb"}, 20);
        $fsdbDumpvars(9, tb_top);
        //$fsdbDumpSVA(7,  u_dut_top);
        //$fsdbDumpvarsToFile("uuu.lst");  Location: 08_sim_work/uuu.lst
        $fsdbDumpflush;
        if (dump_mem == 'd1) $fsdbDumpMDA;
    `endif
    end
`elsif VIVADO
`endif
end
// }}}

endmodule : tb_top

`ifdef XILINX_SIMULATOR
    module short(in1, in1);
        inout in1;
    endmodule
`endif

`endif // _TB_TOP_SV_

