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


`ifndef _CPU_MODEL_SV_
`define _CPU_MODEL_SV_

// ./stim/axi_data.svh
`include "axi_data.svh"

// ./stim/cpu_data.svh
`include "cpu_data.svh"

// ./rm/cpu_model_cb.svh
`include "cpu_model_cb.svh"

class cpu_model #(type REQ = axi_data, 
                  type RSP = REQ);

    //----------------------------------
    // Usertype define
    //----------------------------------
    
    typedef cpu_model_cb #(REQ, RSP) cb_t;

    //----------------------------------
    // Varible declaration
    //----------------------------------
    
    protected string         m_inst_name;

    // Connect to stim
    protected mailbox #(REQ) m_req_mlbx;
    protected mailbox #(RSP) m_rsp_mlbx;

    // Connect to bfm
    mailbox #(REQ)           m_axismc_mlbx;
    mailbox #(REQ)           m_axismd_mlbx;

    mailbox #(RSP)           m_axissc_mlbx;
    mailbox #(RSP)           m_axissd_mlbx;

    // Connect to rm
    protected mailbox #(REQ) m_ist_mlbx;
    protected mailbox #(REQ) m_chk_mlbx;

    local     bit            m_start;
    local     bit            m_stop;

    protected cb_t           m_cpu_cb[$];

    //----------------------------------
    // Task and function declaration
    //----------------------------------
    
    extern function new(string name = "cpu_model");

    extern local task run();

    extern virtual task start();
    extern virtual task stop();
    extern virtual task main();

    extern function void append_callback(input cb_t cb = null);

    // 
    // Set the handle of the m_req_mlbx in order to bind mailbox to generator
    //
    
    extern function void set_reqmlbx(ref mailbox #(REQ) req_mlbx);

    // 
    // Set the handle of the m_rsp_mlbx in order to bind mailbox to generator
    //
    
    extern function void set_rspmlbx(ref mailbox #(REQ) rsp_mlbx);

    // 
    // Set the handle of the m_ist_mlbx in order to bind mailbox to generator
    //
    
    extern function void set_istmlbx(ref mailbox #(REQ) ist_mlbx);

    // 
    // Set the handle of the m_chk_mlbx in order to bind mailbox to generator
    //
    
    extern function void set_chkmlbx(ref mailbox #(REQ) chk_mlbx);

    // Process to request data(read response)

    extern task request_process();

    // Process to response data(read response)

    extern task response_process();

    // User Process

    extern task user_process();

endclass : cpu_model

function cpu_model::new(string name = "cpu_model");
    m_inst_name   = name;
    m_axismc_mlbx = new(1);
    m_axismd_mlbx = new(1);
    m_axissc_mlbx = new(1);
    m_axissd_mlbx = new(1);
endfunction : new

task cpu_model::run();
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

task cpu_model::start();
    m_start = 'd1;
    // Fork the main task
    fork
        run();
    join_none
endtask: start

task cpu_model::stop();
    m_stop = 'd1;
    wait (m_start == 'd0);
endtask: stop

task cpu_model::main();
    fork
        request_process();
        response_process();
        user_process();
    join_none
endtask : main

function void cpu_model::append_callback(input cb_t cb = null);
    // Bind req/rsp mailbox of model and cb
    cb.set_reqmlbx(this.m_req_mlbx);
    cb.set_rspmlbx(this.m_rsp_mlbx);
    // Bind bfm mailbox of model and cb
    cb.m_axismc_mlbx = this.m_axismc_mlbx;
    cb.m_axismd_mlbx = this.m_axismd_mlbx;
    cb.m_axissc_mlbx = this.m_axissc_mlbx;
    cb.m_axissd_mlbx = this.m_axissd_mlbx;
    // Bind req/rsp mailbox of model and cb
    cb.set_istmlbx(this.m_ist_mlbx);
    cb.set_chkmlbx(this.m_chk_mlbx);
    // Add callback to pool
    m_cpu_cb.push_back(cb);
endfunction : append_callback

function void cpu_model::set_reqmlbx(ref mailbox #(REQ) req_mlbx);
    m_req_mlbx = req_mlbx;
endfunction: set_reqmlbx

function void cpu_model::set_rspmlbx(ref mailbox #(REQ) rsp_mlbx);
    m_rsp_mlbx = rsp_mlbx;
endfunction: set_rspmlbx

function void cpu_model::set_istmlbx(ref mailbox #(REQ) ist_mlbx);
    m_ist_mlbx = ist_mlbx;
endfunction: set_istmlbx

function void cpu_model::set_chkmlbx(ref mailbox #(REQ) chk_mlbx);
    m_chk_mlbx = chk_mlbx;
endfunction: set_chkmlbx

task cpu_model::request_process();
    // Vivado do not support multi-callbacks
    foreach (m_cpu_cb[idx]) begin
        automatic int cb_id = idx;
        // fork
            begin
                cb_t cpu_cb = m_cpu_cb[cb_id];
                if (cpu_cb == null) 
                    m_cpu_cb[cb_id] = new();
                m_cpu_cb[cb_id].request_process();
            end
        // join_none
    end
endtask : request_process

task cpu_model::response_process();
    // Vivado do not support multi-callbacks
    foreach (m_cpu_cb[idx]) begin
        automatic int cb_id = idx;
        // fork
            begin
                cb_t cpu_cb = m_cpu_cb[cb_id];
                if (cpu_cb == null) 
                    m_cpu_cb[cb_id] = new();

                m_cpu_cb[cb_id].response_process();
            end
        // join_none
    end
endtask : response_process

task cpu_model::user_process();
    // Vivado do not support multi-callbacks
    foreach (m_cpu_cb[idx]) begin
        automatic int cb_id = idx;
        // fork
            begin
                cb_t cpu_cb = m_cpu_cb[cb_id];
                if (cpu_cb == null) 
                    m_cpu_cb[cb_id] = new();

                m_cpu_cb[cb_id].user_process();
            end
        // join_none
    end
endtask : user_process

`endif // _CPU_MODEL_SV_

