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


`ifndef _RM_WARPPER_SV_
`define _RM_WARPPER_SV_

// ./rm/axi_rm.sv
`include "axi_rm.sv"

class rm_wrapper #(type DATA = axi_data);

    typedef axi_rm #(DATA) rm_t;

    protected string         m_inst_name;

    mailbox #(DATA)          m_ist_mlbx;
    mailbox #(DATA)          m_chk_mlbx;

    local     bit            m_start;
    local     bit            m_stop;

    protected rm_t           m_rm;

    protected bit            m_check_ok;
    protected int            m_inst_num;

    extern function new(string name = "rm_wrapper");

    extern task run();

    extern task start();
    extern task stop();
    extern task main();

    extern function void report();

    extern function void bind_rm(input rm_t rm = null);

    // Process to request data(read response)

    extern task insert_process();

    // Process to response data(read response)

    extern task check_process();

endclass : rm_wrapper

function rm_wrapper::new(string name = "rm_wrapper");
    m_inst_name = name;
    m_ist_mlbx  = new(1);
    m_chk_mlbx  = new(1);
endfunction : new

task rm_wrapper::run();
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

task rm_wrapper::start();
    m_start = 'd1;
    // Fork the main task
    fork
        run();
    join_none
endtask: start

task rm_wrapper::stop();
    m_stop = 'd1;
    wait (m_start == 'd0);
endtask: stop

task rm_wrapper::main();
    fork
        insert_process();
        check_process();
        begin
            if (m_rm != null) m_rm.run();
        end
    join_none
endtask : main

function void rm_wrapper::report();
    if (m_rm != null) begin
        m_rm.report();
    end
endfunction : report

function void rm_wrapper::bind_rm(input rm_t rm = null);
    // Add callback to pool
    m_rm = rm;
    // Bind req/rsp mailbox of model and cb
    m_rm.m_ist_mlbx = m_ist_mlbx;
    m_rm.m_chk_mlbx = m_chk_mlbx;
endfunction : bind_rm

task rm_wrapper::insert_process();
    if (m_rm != null) m_rm.insert_process();
endtask : insert_process

task rm_wrapper::check_process();
    if (m_rm != null) m_rm.check_process();
endtask : check_process

`endif // _RM_WARPPER_SV_