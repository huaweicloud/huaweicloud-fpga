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


`ifndef _AXI_STIM_GEN_SV_
`define _AXI_STIM_GEN_SV_

// ./stimu/axi_data.svh
`include "axi_data.svh"

// ./stimu/axi_stims.svh
`include "axi_stims.sv"

class axi_stim_gen #(type REQ = axi_data, 
                     type RSP = REQ);

    //----------------------------------
    // Typedef for bfm
    //----------------------------------

    typedef axi_stims #(REQ, RSP) STIMS_t;
    typedef mailbox   #(REQ)      REQMLBX_t;
    typedef mailbox   #(RSP)      RSPMLBX_t;


    //----------------------------------
    // Varible declaration
    //----------------------------------
    //
    protected REQMLBX_t      m_req_mlbx;
    protected RSPMLBX_t      m_rsp_mlbx;

    // Registered stims
    local     STIMS_t        m_reg_stims[$];
    local     int            m_reg_stims_num;

    local     bit            m_start;
    local     bit            m_stop;

    protected string         m_inst_name;

    //----------------------------------
    // Task and function declaration
    //----------------------------------

    extern function new(string    name    = "axi_stim_gen");

    extern local task run();

    extern virtual task start();
    extern virtual task stop();
    extern virtual task main();

    extern function void reg_stims(input STIMS_t stims = null);

    extern function void get_reqmlbx(ref mailbox #(REQ) req_mlbx);
    extern function void get_rspmlbx(ref mailbox #(RSP) rsp_mlbx);

    extern task send_request();
    extern task get_response();

endclass : axi_stim_gen

function axi_stim_gen::new(string name = "axi_stim_gen");
    m_inst_name= name ;
    m_start    = 'd0  ;
    m_stop     = 'd0  ;
    m_req_mlbx = new(1);
    m_rsp_mlbx = new(1);
endfunction : new

task axi_stim_gen::run();
    fork
        begin
            wait (m_start == 'd1);
            main();
        end
        begin
            wait (m_start == 'd1);
            wait (m_stop  == 'd1);
            m_start = 'd0;
            disable fork;
        end
    join_none
endtask : run

task axi_stim_gen::start();
    m_start = 'd1;
    // Fork the main task
    // Vivado simulator do not support fork/join_none in function
    // Move fork/join_none to start instead
    fork
        run();
    join_none
endtask: start

task axi_stim_gen::stop();
    m_stop = 'd1;
    wait (m_start == 'd0);
endtask: stop

task axi_stim_gen::main();
    fork
        send_request();
        get_response();
    join_none
endtask : main

function void axi_stim_gen::reg_stims(input STIMS_t stims = null);
    if (stims != null) begin
        m_reg_stims.push_back(stims);
        stims.reg_generator(this);
        m_reg_stims_num++;
    end else begin
        $display("[ERROR:] Register stims fail, stims need to be register can not be null");
    end
endfunction : reg_stims

function void axi_stim_gen::get_reqmlbx(ref mailbox #(REQ) req_mlbx);
    req_mlbx = m_req_mlbx;
endfunction : get_reqmlbx

function void axi_stim_gen::get_rspmlbx(ref mailbox #(REQ) rsp_mlbx);
    rsp_mlbx = m_rsp_mlbx;
endfunction : get_rspmlbx

task axi_stim_gen::send_request();
    REQ req;
    int invalid_size;
    STIMS_t stim;
    if (!m_reg_stims_num) return;
    forever begin
        // Only support RR-sch for more stims
        foreach (m_reg_stims[idx]) begin
            stim = m_reg_stims[idx];
            if (stim != null) begin
                m_reg_stims[idx].m_req_mlbx.get(req);
                m_req_mlbx.put(req);
            end else if (invalid_size++ >= m_reg_stims_num) begin
                break;
            end
        end
    end
endtask : send_request

task axi_stim_gen::get_response();
    RSP rsp;
    int invalid_size;
    STIMS_t stim;
    if (!m_reg_stims_num) return;
    forever begin
        // Only support RR-sch for more stims
        foreach (m_reg_stims[idx]) begin
            stim = m_reg_stims[idx];
            if (stim != null) begin
                m_rsp_mlbx.get(rsp);
                m_reg_stims[idx].m_rsp_mlbx.put(rsp);
            end else if (invalid_size++ >= m_reg_stims_num) begin
                break;
            end
        end
    end
endtask : get_response

`endif // _AXI_STIM_GEN_SV_

