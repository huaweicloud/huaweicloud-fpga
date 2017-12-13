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


`ifndef _REG_STIM_GEN_SV_
`define _REG_STIM_GEN_SV_

// ./stimu/axi_data.svh
`include "axi_data.svh"

class reg_stim_gen #(type REQ    = axi_data, 
                     type RSP    = REQ,
                     int  AWIDTH = 'd32,
                     int  DWIDTH = 'd32);

    //----------------------------------
    // Typedef for generator
    //----------------------------------

    typedef bit [AWIDTH - 'd1 : 0] ADDR_t;
    typedef bit [DWIDTH - 'd1 : 0] DATA_t;
    typedef mailbox #(REQ)         REQMLBX_t;
    typedef mailbox #(RSP)         RSPMLBX_t;

    //----------------------------------
    // Varible declaration
    //---------------------------------- 

    protected mailbox #(REQ) m_req_mlbx;
    protected mailbox #(RSP) m_rsp_mlbx;

    protected REQ            m_wr_req;
    protected RSP            m_wr_rsp;
    protected REQ            m_rd_req;
    protected RSP            m_rd_rsp;

    protected string         m_inst_name;

    //----------------------------------
    // Task and function declaration
    //----------------------------------

    extern function new(string name = "reg_stim_gen");

    extern function void get_reqmlbx(ref mailbox #(REQ) req_mlbx);
    extern function void get_rspmlbx(ref mailbox #(REQ) rsp_mlbx);

    extern task write(input ADDR_t addr = 'd0,
                      input DATA_t data = 'd0);

    extern task read(input  ADDR_t addr = 'd0,
                     output DATA_t data);

endclass : reg_stim_gen

//----------------------------------------------------------------------------------------------------------------------
// IMPLEMENTATION
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
//
// CLASS- reg_stim_gen
//
//----------------------------------------------------------------------------------------------------------------------

function reg_stim_gen::new(string name = "reg_stim_gen");
    m_inst_name = name;
    m_req_mlbx  = new(1);
    m_rsp_mlbx  = new(1);
    m_wr_req    = new();
    m_wr_rsp    = new();
    m_rd_req    = new();
    m_rd_rsp    = new();
    $display("New reg_stim_gen");
endfunction : new

function void reg_stim_gen::get_reqmlbx(ref mailbox #(REQ) req_mlbx);
    req_mlbx = m_req_mlbx;
endfunction : get_reqmlbx

function void reg_stim_gen::get_rspmlbx(ref mailbox #(REQ) rsp_mlbx);
    rsp_mlbx = m_rsp_mlbx;
endfunction : get_rspmlbx

task reg_stim_gen::write(input ADDR_t addr = 'd0,
                         input DATA_t data = 'd0);
    bit [7 : 0] byte_queue[$];
    m_wr_req.addr    = addr;
    {>>8{byte_queue}}= data;
    m_wr_req.data    = byte_queue;
    m_wr_req.opt     = e_AXI_OPT_WR;
    // Send request
    m_req_mlbx.put(m_wr_req);
    // Get response
    m_rsp_mlbx.get(m_wr_rsp);
endtask : write

task reg_stim_gen::read(input  ADDR_t addr = 'd0,
                        output DATA_t data);
    bit [7 : 0] byte_queue[$];
    m_rd_req.addr       = addr;
    m_rd_req.opt        = e_AXI_OPT_RD;
    // Send request
    m_req_mlbx.put(m_rd_req);
    // data = {>>8{rdata.data}};
    // Get response
    m_rsp_mlbx.get(m_rd_rsp);
    byte_queue = m_rd_rsp.data;
    data = {>>8{byte_queue}};
endtask : read

`endif // _REG_STIM_GEN_SV_

