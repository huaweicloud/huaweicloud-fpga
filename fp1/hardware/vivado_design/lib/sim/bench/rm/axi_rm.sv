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


`ifndef _AXI_RM_SV_
`define _AXI_RM_SV_

// ./stim/axi_data.svh
`include "axi_data.svh"

class axi_rm #(type DATA = axi_data);

    // Data Strorage

    static    DATA           m_scb[int][int][$];
    protected DATA           m_data_exp[$];

    protected string         m_inst_name;

    mailbox #(DATA)          m_ist_mlbx;
    mailbox #(DATA)          m_chk_mlbx;

    protected bit            m_check_ok;
    protected int            m_inst_num;

    extern function new(string name = "axi_rm");

    extern virtual task run();

    // Write data to ddr

    extern virtual function void insert(ref DATA data);

    // Read data from ddr

    extern virtual function bit check(ref DATA data);

    extern virtual function void report();

    extern function bit get_check_status();

    // Process to request data(read response)

    extern virtual task insert_process();

    // Process to response data(read response)

    extern virtual task check_process();

endclass : axi_rm

function axi_rm::new(string name = "axi_rm");
    m_inst_name = name;
    m_scb.delete();
    m_check_ok  = 'd1;
    m_inst_num  = 'd0;
endfunction : new

task axi_rm::run();
endtask : run

function void axi_rm::insert(ref DATA data);
    int fsn    = data.fsn   ;
    int stream = data.stream;
    m_scb[stream][fsn].push_back(data);
endfunction : insert

function bit axi_rm::check(ref DATA data);
    int  fsn    = data.fsn   ;
    int  stream = data.stream;
    DATA exp;
    if (m_scb.exists(stream) && m_scb[stream].exists(fsn) && m_scb[stream][fsn].size()) begin
        exp = m_scb[stream][fsn].pop_front();
        check = exp.compare(data);
        if (check != 'd1) begin
            string info = "[Data Compare Error:]\n";
            $sformat(info, "%s------------------------------------\n", info);
            $sformat(info, "%s[Expect Data] is \n%p\n", info, exp);
            $sformat(info, "%s[Actual Data] is \n%p\n", info, data);
            $sformat(info, "%s------------------------------------\n", info);
            `tb_error(m_inst_name, info)
        end
        if (!m_scb[stream][fsn].size()) m_scb[stream].delete(fsn);
        if (!m_scb[stream].num()) m_scb.delete(stream);
    end else begin
        check = 'd0;
    end
endfunction : check

function void axi_rm::report();
    int fsn;
    int stream;
    if (m_scb.first(stream)) begin
        `tb_error(m_inst_name, "[Data Last Error]: Packets still in scoreboard!")
        do begin
            if (m_scb[stream].first(fsn)) begin
                do begin
                    foreach (m_scb[stream][fsn][idx]) begin
                        DATA data = m_scb[stream][fsn][idx];
                        data.display();
                    end
                end
                while (m_scb[stream].next(fsn));
            end
        end
        while (m_scb.next(stream));
    end
endfunction : report

function bit axi_rm::get_check_status();
    get_check_status = m_check_ok;
endfunction: get_check_status

task axi_rm::insert_process();
    DATA item;
    forever begin
        // Get request
        m_ist_mlbx.get(item);
        if (item == null) continue;
        // Insert Data
        insert(item);
    end
endtask : insert_process

task axi_rm::check_process();
    DATA   item;
    string info;
    forever begin
        bit check_ok;
        // Get response
        m_chk_mlbx.get(item);
        m_inst_num++;
        if (item != null) begin
            // Check data
            check_ok = check(item);
            $sformat(info, {"+------------------------------------+\n", 
                            "|  Compare Pkts   : %d, %s|\n", 
                            "+------------------------------------+"}, 
                            m_inst_num, check_ok ? "PASS" : "FAIL");
            if (check_ok == 'd1) begin
                // `tb_info("cpu_model_cb",  "Data Compare OK!")
                `tb_info(m_inst_name,  info)
            end else begin
                `tb_error(m_inst_name, info)
            end
            m_check_ok &= check_ok;
        end else begin
            $sformat(info, {"+------------------------------------+\n", 
                            "|  Compare Pkts   : %d, FAIL|\n", 
                            "+------------------------------------+"}, m_inst_num);
            `tb_error(m_inst_name, info)
            `tb_error(m_inst_name, "Data can not be null!")
            m_check_ok =  'd0;
        end
    end
endtask : check_process

`endif // _AXI_RM_SV_

