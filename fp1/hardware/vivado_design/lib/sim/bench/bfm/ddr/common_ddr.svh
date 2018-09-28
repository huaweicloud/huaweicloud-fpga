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


`ifndef _COMMON_DDR_SVH_
`define _COMMON_DDR_SVH_

`define DDRA_ADDR_WIDTH  'd17
`define DDRA_DATA_BYTES  'd9
`define DDRA_DATA_WIDTH  (`DDRA_DATA_BYTES << 3)
`define DDRA_RANK_WIDTH  'd2
`define DDRA_BANK_WIDTH  'd2
`define DDRA_BG_WIDTH    'd2
`define DDRA_TCK         'd952     //DDR4 interface clock period in ps

`define DDRB_ADDR_WIDTH  'd17
`define DDRB_DATA_BYTES  'd9
`define DDRB_DATA_WIDTH  (`DDRB_DATA_BYTES << 3)
`define DDRB_RANK_WIDTH  'd2
`define DDRB_BANK_WIDTH  'd2
`define DDRB_BG_WIDTH    'd2
`define DDRB_TCK         'd952     //DDR4 interface clock period in ps

`define DDRC_ADDR_WIDTH  'd17
`define DDRC_DATA_BYTES  'd9
`define DDRC_DATA_WIDTH  (`DDRC_DATA_BYTES << 3)
`define DDRC_RANK_WIDTH  'd2
`define DDRC_BANK_WIDTH  'd2
`define DDRC_BG_WIDTH    'd2
`define DDRC_TCK         'd952     //DDR4 interface clock period in ps

`define DDRD_ADDR_WIDTH  'd17
`define DDRD_DATA_BYTES  'd9
`define DDRD_DATA_WIDTH  (`DDRD_DATA_BYTES << 3)
`define DDRD_RANK_WIDTH  'd2
`define DDRD_BANK_WIDTH  'd2
`define DDRD_BG_WIDTH    'd2
`define DDRD_TCK         'd952     //DDR4 interface clock period in ps

// Connect DUT ddr interface to ddr4 rdimm
`define tb_ddr_dut_connect_rdimm(ID) \
    .ddr``ID``_100m_ref_clk_p( tb_if.clk_100m_p                  ), \
    .ddr``ID``_100m_ref_clk_n( tb_if.clk_100m_n                  ), \
    .ddr``ID``_72b_act_n     ( tb_if.u_ddr``ID``_if.ddr4_act_n   ), \
    .ddr``ID``_72b_addr      ( tb_if.u_ddr``ID``_if.ddr4_addr    ), \
    .ddr``ID``_72b_ba        ( tb_if.u_ddr``ID``_if.ddr4_ba      ), \
    .ddr``ID``_72b_bg        ( tb_if.u_ddr``ID``_if.ddr4_bg      ), \
    .ddr``ID``_72b_cke       ( tb_if.u_ddr``ID``_if.ddr4_cke     ), \
    .ddr``ID``_72b_odt       ( tb_if.u_ddr``ID``_if.ddr4_odt     ), \
    .ddr``ID``_72b_cs_n      ( tb_if.u_ddr``ID``_if.ddr4_cs_n    ), \
    .ddr``ID``_72b_ck_t      ( tb_if.u_ddr``ID``_if.ddr4_ck_t    ), \
    .ddr``ID``_72b_ck_c      ( tb_if.u_ddr``ID``_if.ddr4_ck_c    ), \
    .ddr``ID``_72b_rst_n     ( tb_if.u_ddr``ID``_if.ddr4_reset_n ), \
    .ddr``ID``_72b_par       ( tb_if.u_ddr``ID``_if.ddr4_par     ), \
    .ddr``ID``_72b_dq        ( tb_if.u_ddr``ID``_if.ddr4_dq      ), \
    .ddr``ID``_72b_dqs_t     ( tb_if.u_ddr``ID``_if.ddr4_dqs_t   ), \
    .ddr``ID``_72b_dqs_c     ( tb_if.u_ddr``ID``_if.ddr4_dqs_c   ), \
    .ddr``ID``_72b_dm_dbi_n  ( tb_if.u_ddr``ID``_if.ddr4_dm_dbi_n),

// Instance of ddr_rank
`define tb_ddr_rdimm_rank_even(ID, rank) \
    u_ddr4_rdimm_``ID``.rcd_enabled.NOLRDIMM.u_ddr4_dimm.rank_instances[rank].even_ranks.u_ddr4_rank

`define tb_ddr_rdimm_rank_odd(ID, rank) \
    u_ddr4_rdimm_``ID``.rcd_enabled.NOLRDIMM.u_ddr4_dimm.rank_instances[rank].mc_ca_mirroring_odd_rank.u_ddr4_rank

// Instance of ddr_chip
`define tb_ddr_rdimm_chip_even(ID, rank, chip) \
    `tb_ddr_rdimm_rank_even(ID, rank)``.Micron_model.instance_of_sdram_devices[chip].micron_mem_model.u_ddr4_model 

`define tb_ddr_rdimm_chip_odd(ID, rank, chip) \
    `tb_ddr_rdimm_rank_odd(ID, rank)``.Micron_model.instance_of_sdram_devices[chip].micron_mem_model.u_ddr4_model 

// Do not display any timing infomation for ddr4_model
`define tb_ddr_rdimm_chip_nodisp_even(ID, rank, chip) \
    `tb_ddr_rdimm_chip_even(ID, rank, chip)``.set_memory_warnings(0, 0); \

`define tb_ddr_rdimm_chip_nodisp_odd(ID, rank, chip) \
    `tb_ddr_rdimm_chip_odd(ID, rank, chip)``.set_memory_warnings(0, 0); \

// Instantiated DDR rdimm
`define tb_inst_ddr_rdimm(ID, IF, ADDR, DQ, BYTES, RANK, BANK, BG, TCK) \
    ddr4_rdimm_wrapper #( \
             .MC_DQ_WIDTH        (DQ       ), \
             .MC_DQS_BITS        (DQ >> 3  ), \
             .MC_DM_WIDTH        (DQ >> 3  ), \
             .MC_CKE_NUM         (RANK     ), \
             .MC_ODT_WIDTH       (RANK     ), \
             .MC_ABITS           (ADDR     ), \
             .MC_BANK_WIDTH      (BANK     ), \
             .MC_BANK_GROUP      (BG       ), \
             .MC_CS_NUM          (RANK     ), \
             .MC_RANKS_NUM       (RANK     ), \
             .NUM_PHYSICAL_PARTS (DQ / 8   ), \
             .CALIB_EN           ("NO"     ), \
             .tCK                (TCK      ), \
             .tPDM               (         ), \
             .MIN_TOTAL_R2R_DELAY(         ), \
             .MAX_TOTAL_R2R_DELAY(         ), \
             .TOTAL_FBT_DELAY    (         ), \
             .MEM_PART_WIDTH     ("x8"     ), \
             .MC_CA_MIRROR       ("ON"     ), \
           `ifdef SAMSUNG \
             .DDR_SIM_MODEL      ("SAMSUNG"), \
           `else \
             .DDR_SIM_MODEL      ("MICRON" ), \
           `endif \
             .DM_DBI             ("NONE"   ), \
             .MC_REG_CTRL        ("ON"     ), \
             .DIMM_MODEL         ("RDIMM"  ), \
             .RDIMM_SLOTS        (1        )  \
        ) u_ddr4_rdimm_``ID ( \
             .ddr4_act_n         ( IF.ddr4_act_n   ),  \
             .ddr4_addr          ( IF.ddr4_addr    ),  \
             .ddr4_ba            ( IF.ddr4_ba      ),  \
             .ddr4_bg            ( IF.ddr4_bg      ),  \
             .ddr4_par           ( IF.ddr4_par     ),  \
             .ddr4_cke           ( IF.ddr4_cke     ),  \
             .ddr4_odt           ( IF.ddr4_odt     ),  \
             .ddr4_cs_n          ( IF.ddr4_cs_n    ),  \
             .ddr4_ck_t          ( IF.ddr4_ck_t    ),  \
             .ddr4_ck_c          ( IF.ddr4_ck_c    ),  \
             .ddr4_reset_n       ( IF.ddr4_reset_n ),  \
             .ddr4_dm_dbi_n      ( IF.ddr4_dm_dbi_n),  \
             .ddr4_dq            ( IF.ddr4_dq      ),  \
             .ddr4_dqs_t         ( IF.ddr4_dqs_t   ),  \
             .ddr4_dqs_c         ( IF.ddr4_dqs_c   ),  \
             .ddr4_alert_n       ( IF.ddr4_alert_n ),  \
             .initDone           (                 ),  \
             .scl                (                 ),  \
             .sa0                (                 ),  \
             .sa1                (                 ),  \
             .sa2                (                 ),  \
             .sda                (                 ),  \
             .bfunc              (                 ),  \
             .vddspd             (                 )); \
    genvar rank``ID; \
    genvar chip``ID; \
    for (rank``ID = 'd0; rank``ID < RANK; rank``ID=rank``ID+'d1) begin : ddr4_rdimm``ID``_rank \
        for (chip``ID = 'd0; chip``ID < DQ / 8; chip``ID=chip``ID+'d1) begin : ddr4_rdimm``ID``_chip \
            if (rank``ID % 2 == 0) begin : ddr4_rdimm``ID``_even \
                initial begin \
                    `tb_ddr_rdimm_chip_nodisp_even(ID, rank``ID, chip``ID) \
                end \
            end else begin : ddr4_rdimm``ID``_odd \
                initial begin \
                    `tb_ddr_rdimm_chip_nodisp_odd(ID, rank``ID, chip``ID) \
                end \
            end \
        end \
    end

// Force DUT ddr initial_done to accelerate simulation
`define tb_ddr_dut_disable_init(ID) \
    begin \
        force $root.tb_top.u_dut.u_ddr``ID``_72b_top.bist_busy     = 'd0; \
        force $root.tb_top.u_dut.u_ddr``ID``_72b_top.ddr_init_done = 'd1; \
    end

`endif // _COMMON_DDR_SVH_

