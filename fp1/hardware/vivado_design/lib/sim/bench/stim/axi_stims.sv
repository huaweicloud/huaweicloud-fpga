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


`ifndef _AXI_STIMS_SV_
`define _AXI_STIMS_SV_

// ./common/tb_log.svh
`include "tb_log.svh"

// ./stimu/axi_data.svh
`include "axi_data.svh"

// ./stimu/axi_stim_cfg.svh
`include "axi_stim_cfg.svh"

// If using vivado simulator, "typedef class" may cause core dump
`ifdef VIVADO
// ./stimu/axi_stim_gen.sv
`include "axi_stim_gen.sv"
`else
typedef class axi_stim_gen;
`endif

class axi_stims #(type REQ = axi_data,
                  type RSP = REQ);

    //----------------------------------
    // Usertype define
    //----------------------------------
    
    typedef axi_stim_gen #(.REQ (REQ), .RSP (RSP)) GEN;

    //----------------------------------
    // Varible declaration
    //----------------------------------
    
    local     GEN            m_generator;

    mailbox #(REQ)           m_req_mlbx;
    mailbox #(RSP)           m_rsp_mlbx;

    REQ                      req;
    RSP                      rsp;

`ifdef VIVADO
    rand bit [3  : 0]        id  ;   // ID
    rand bit [63 : 0]        addr;   // Address

    bit  [7 : 0]             data[]; // Data

    rand axi_opt_t           opt;    // Operatio type
    rand axi_burst_t         btype;  // Burst type

    rand axi_resp_t          resp;   // Response
`endif

    protected bit            m_stop;
    protected bit            m_end;
    protected int            m_inst_num;

`ifdef VIVADO
    protected REQ            m_item;
`else
    rand      REQ            m_item;
`endif
    protected axi_stim_cfg   m_cfg;

    // Inst name
    protected string         m_inst_name;

    //----------------------------------
    // Constraint declaration
    //----------------------------------

    // Stim constraint
    constraint axi_data_user_constraint {
    `ifndef VIVADO
        m_item.id    == 'd0;
        m_item.addr inside {[m_cfg.axi_addr_min : m_cfg.axi_addr_max]}; 
        m_item.data.size() == m_cfg.axi_data_len;
        m_item.opt   == m_cfg.axi_opt; 
        m_item.btype == m_cfg.axi_burst_type; 
        m_item.resp  == m_cfg.axi_resp;
    `endif
    }

    //----------------------------------
    // Task and function declaration
    //----------------------------------
    
    extern function new(string name = "axi_stims");

    extern function void reg_generator(input GEN generator = null);

    extern function void set_cfg(input axi_stim_cfg cfg = null);

    extern virtual task body();

    extern virtual task gen_packet();
    extern virtual task send_packet();

    extern task start();
    extern task stop();
    extern task wait_done();

endclass : axi_stims

//------------------------------------------------------------------------------
// IMPLEMENTATION
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// CLASS- axi_stims
//
//------------------------------------------------------------------------------

function axi_stims::new(string name = "axi_stims");
    m_inst_name= name;
    m_cfg      = new();
    m_item     = new();
    m_stop     = 'd0;
    m_end      = 'd0;
    m_inst_num = 'd0;
    m_req_mlbx = new(1);
    m_rsp_mlbx = new(1);
    m_cfg.display("AXI Stim Cfg:\n");
endfunction : new

function void axi_stims::reg_generator(input GEN generator = null);
    if (generator != null) begin
        // Establish the relation between generator and stims
        m_generator = generator;
        // m_generator.get_reqmlbx(m_req_mlbx);
        // m_generator.get_rspmlbx(m_rsp_mlbx);
    end
endfunction : reg_generator

function void axi_stims::set_cfg(input axi_stim_cfg cfg = null);
    if (cfg != null) begin
        // Bind stim cfg and stims
        m_cfg = cfg;
    end
endfunction : set_cfg

task axi_stims::gen_packet();
    int         result;
    bit [7 : 0] data_byte = 'd0;
    // Generate data
`ifndef VIVADO
    assert(randomize()) begin
        `tb_debug(m_inst_name, "Randomize success!")
    end else begin
        `tb_fatal(m_inst_name, "Randomize fail!")
    end
`else
    // If using vivado simulator, use std::randomize instead to avoid the
    // core dump
    // I was no idea about why randomize can not be success when using vivado simulator, so I had to commont all randomize.
    id     = 'd0;
    result = 'd1;
    addr  += 'h1000;
    assert(result) begin
        `tb_debug(m_inst_name, "Randomize success!")
        m_item.id    = id   ;
        // Align addr 32bit
        m_item.addr  = addr << 2;
        m_item.opt   = opt  ;
        m_item.btype = btype;
        m_item.resp  = resp ;
        data = new[m_cfg.axi_data_len];
        foreach (data[idx]) begin
            data[idx] = data_byte++;
        end
        m_item.data  = data ;
    end else begin
        `tb_fatal(m_inst_name, "Randomize fail!")
    end
`endif
endtask : gen_packet

task axi_stims::send_packet();
    string      info;
    // Do not need to new req
    // req  = new();
    // Copy data to req
    req = m_item.copy();
    // Send request
    m_req_mlbx.put(req);
    // Get response
    // m_rsp_mlbx.get(rsp);
    // Show pkts num which have been sent
    $sformat(info, {"+------------------------------------+\n", 
                    "| Generate Pkts   : %d       |\n", 
                    "+------------------------------------+"}, m_inst_num + 'd1);
    `tb_info("axi_stims", info)
    // There is no delay for stim send. You can add time delay here if you need.
endtask : send_packet

task axi_stims::body();
    `ifdef VIVADO
        // Random start address
        addr = $urandom_range(m_cfg.axi_addr_max, m_cfg.axi_addr_min);
    `endif
    forever begin
        if (m_stop == 'd1) break;
        // Generate data
        gen_packet();
        // Send data
        send_packet();
        if (m_cfg.axi_inst_num <= ++m_inst_num) break;
    end
    m_end = 'd1;
endtask : body

task axi_stims::start();
    fork
        body();
    join_none
endtask : start

task axi_stims::stop();
    m_stop = 'd1;
    wait_done();
endtask : stop

task axi_stims::wait_done();
    wait(m_end == 'd1);
endtask : wait_done

`endif // _AXI_STIMS_SV_

