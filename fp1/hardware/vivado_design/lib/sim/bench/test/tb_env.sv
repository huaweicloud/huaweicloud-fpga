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


`ifndef _TB_ENV_SV_
`define _TB_ENV_SV_

// ./tb_log.svh
`include "tb_log.svh"

// ./config_db.svh
`include "config_db.svh"

// ./stimu/axi_stim_gen.sv
`include "axi_stim_gen.sv"

// ./stimu/reg_stim_gen.sv
`include "reg_stim_gen.sv"

// ./bfm/axi4l/axil_master_bfm.sv
`include "axil_master_bfm.sv"

// ./bfm/axi4l/axil_slave_bfm.sv
`include "axil_slave_bfm.sv"

// ./bfm/axi4s/axis_master_bfm.sv
`include "axis_master_bfm.sv"

// ./bfm/axi4s/axis_slave_bfm.sv
`include "axis_slave_bfm.sv"

// ./bfm/ddr/common_ddr.svh
`include "common_ddr.svh"

// ./rm/cpu_model.sv
`include "cpu_model.sv"

// ./rm/rm_wrapper.sv
`include "rm_wrapper.sv"

`ifdef USE_DDR_MODEL
// ./bfm/axi4/axi_master_bfm.sv
`include "axi_master_bfm.sv"

// ./bfm/axi4/axi_slave_bfm.sv
`include "axi_slave_bfm.sv"

// ./rm/ddr_model.sv
`include "ddr_model.sv"
`endif

// ./bfm/tb_vif_obj.sv
`include "tb_vif_obj.sv"

// AXI4-Stream Master Bfm for Command

typedef axis_master_bfm #(.REQ    (axi_data), 
                          .RSP    (axi_data),
                          .DWIDTH (`AXI4S_DATA_WIDTH), 
                          .KWIDTH (`AXI4S_KEEP_WIDTH), 
                          .UWIDTH (`AXI4S_USER_WIDTH)) axis_masterc_bfm_t;

// AXI4-Stream Master Bfm for Data

typedef axis_master_bfm #(.REQ    (axi_data), 
                          .RSP    (axi_data),
                          .DWIDTH (`AXI4_DATA_WIDTH ), 
                          .KWIDTH (`AXI4_STRB_WIDTH ), 
                          .UWIDTH (`AXI4S_USER_WIDTH)) axis_masterd_bfm_t;

// AXI4-Stream Slave Bfm for Command

typedef axis_slave_bfm #(.REQ    (axi_data), 
                         .RSP    (axi_data),
                         .DWIDTH (`AXI4S_DATA_WIDTH), 
                         .KWIDTH (`AXI4S_KEEP_WIDTH), 
                         .UWIDTH (`AXI4S_USER_WIDTH)) axis_slavec_bfm_t;

// AXI4-Stream Slave Bfm for Data

typedef axis_slave_bfm #(.REQ    (axi_data), 
                         .RSP    (axi_data),
                         .DWIDTH (`AXI4_DATA_WIDTH ), 
                         .KWIDTH (`AXI4_STRB_WIDTH ), 
                         .UWIDTH (`AXI4S_USER_WIDTH)) axis_slaved_bfm_t;

class tb_env;

    // AXI4-Lite Interface Bfm for register cfg and read

    axil_master_bfm          m_axilm_bfm;
    axil_slave_bfm           m_axils_bfm;

    // Register generator

    reg_stim_gen             m_reg_gen;

    // AXI4-Stream Interface Bfm a for DMA

    axis_masterc_bfm_t       m_axismc_bfm;
    axis_masterd_bfm_t       m_axismd_bfm;

    axis_slavec_bfm_t        m_axissc_bfm;
    axis_slaved_bfm_t        m_axissd_bfm;

    // AXI stim generator

    axi_stim_gen             m_axi_gen;

`ifdef USE_DDR_MODEL
    // AXI4 Interface Bfm b for DDR

    axi_slave_bfm            m_axisd_bfm;
`endif

    // CPU model

    cpu_model                m_cpu_model;

    // RM

    rm_wrapper               m_rm_wrapper;

`ifdef USE_DDR_MODEL
    // DDR model

    ddr_model                m_ddr_model;
`endif

    // Virtual interface
    
    virtual tb_interface     m_tb_vif   ;

    protected string         m_inst_name;

    extern function new(string name = "tb_env");

    extern function void build();
    extern function void connect();

endclass : tb_env

function tb_env::new(string name = "tb_env");
    m_inst_name = name;
endfunction : new

function void tb_env::build();
    m_axilm_bfm = new("m_axilm_bfm" );
    m_axils_bfm = new("m_axils_bfm" );
    m_axismc_bfm= new("m_axismc_bfm");
    m_axismd_bfm= new("m_axismd_bfm");
    m_axissc_bfm= new("m_axissc_bfm");
    m_axissd_bfm= new("m_axissd_bfm");

    m_reg_gen   = new("m_reg_gen"  );
    m_axi_gen   = new("m_axi_gen"  );

`ifdef USE_DDR_MODEL
    m_axisd_bfm = new("m_axisd_bfm");
`endif

    m_cpu_model = new("m_cpu_model");
`ifdef USE_DDR_MODEL
    m_ddr_model = new("m_ddr_model");
`endif
    m_rm_wrapper= new("m_rm_wrapper");
endfunction : build

function void tb_env::connect();
    //  Class can not be null when passthrough by using ref this handle to function/task by vivado simulator
    axi_stim_gen#()::REQMLBX_t axi_req_mlbx = new(1);
    axi_stim_gen#()::RSPMLBX_t axi_rsp_mlbx = new(1);
    reg_stim_gen#()::REQMLBX_t reg_req_mlbx = new(1);
    reg_stim_gen#()::RSPMLBX_t reg_rsp_mlbx = new(1);
`ifdef USE_DDR_MODEL
    ddr_model #()::REQMLBX_t   ddr_req_mlbx = new(1);
    ddr_model #()::RSPMLBX_t   ddr_rsp_mlbx = new(1);
`endif
    tb_vif_obj vif_obj;


    // Get virtual interface object
    bit val = config_db#(tb_vif_obj)::get("tb_vif", vif_obj);
    if (!val || vif_obj == null) begin
        `tb_fatal(m_inst_name, "Interface get fail or interface handle is null, please check!")
    end else begin
        // Get virtual interface handle
        m_tb_vif = vif_obj.m_tb_vif;
    end

    // Connect axi stim and cpu_model
    m_axi_gen.get_reqmlbx(axi_req_mlbx);
    m_axi_gen.get_rspmlbx(axi_rsp_mlbx);
    m_cpu_model.set_reqmlbx(axi_req_mlbx);
    m_cpu_model.set_rspmlbx(axi_rsp_mlbx);

    // Connect cpu_model and axi4-stream bfm
    m_axismc_bfm.set_reqmlbx(m_cpu_model.m_axismc_mlbx);
    m_axismd_bfm.set_reqmlbx(m_cpu_model.m_axismd_mlbx);
    m_axissc_bfm.set_rspmlbx(m_cpu_model.m_axissc_mlbx);
    m_axissd_bfm.set_rspmlbx(m_cpu_model.m_axissd_mlbx);

    // Connect cpu_model and rm_wrapper
    m_cpu_model.set_istmlbx(m_rm_wrapper.m_ist_mlbx);
    m_cpu_model.set_chkmlbx(m_rm_wrapper.m_chk_mlbx);

    // Connect reg stim and axil master bfm
    m_reg_gen.get_reqmlbx(reg_req_mlbx);
    m_reg_gen.get_rspmlbx(reg_rsp_mlbx);
    m_axilm_bfm.set_rspmlbx(reg_rsp_mlbx);
    m_axilm_bfm.set_reqmlbx(reg_req_mlbx);

`ifdef USE_DDR_MODEL
    // Connect axi bfm and ddr_model
    m_ddr_model.set_reqmlbx(ddr_req_mlbx);
    m_ddr_model.set_rspmlbx(ddr_rsp_mlbx);
    m_axisd_bfm.set_reqmlbx(ddr_rsp_mlbx);
    m_axisd_bfm.set_rspmlbx(ddr_req_mlbx);
`endif

    // Config virtual interface handle to bfm
    
    //----------------------------------
    // Connect AXI4-Lite Bfm and Vif
    //----------------------------------
    // {{{
    m_axilm_bfm.m_axil_vif  = vif_obj.m_axil_vif.m_axil_vif;
    // }}}
 
    //----------------------------------
    // Connect AXI4-Stream Bfm and Vif
    //----------------------------------
    // {{{
    m_axismc_bfm.m_axis_vif = vif_obj.m_axismc_vif.m_axis_vif;
    m_axismd_bfm.m_axis_vif = vif_obj.m_axismd_vif.m_axis_vif;
    m_axissc_bfm.m_axis_vif = vif_obj.m_axissc_vif.m_axis_vif;
    m_axissd_bfm.m_axis_vif = vif_obj.m_axissd_vif.m_axis_vif;
    // }}}

`ifdef USE_DDR_MODEL
    //----------------------------------
    // Connect AXI4 Bfm and Vif
    //----------------------------------
    // {{{
    m_axisd_bfm.m_axi_vif   = vif_obj.m_axisd_vif.m_axi_vif;
    // }}}
`endif
endfunction : connect

`endif // _TB_ENV_SV_

