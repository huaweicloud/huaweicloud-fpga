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


`ifndef _EXT_RM_SV_
`define _EXT_RM_SV_

// ./rm/axi_rm.sv
`include "axi_rm.sv"

// /stimu/ext_exp_cfg.svh
`include "ext_exp_cfg.svh"

class ext_rm extends axi_rm #(axi_data);

    protected ext_exp_cfg    m_cfg;

    protected bit            m_user_exp;
    local     int            m_file_id;

    //----------------------------------
    // Task and function declaration
    //----------------------------------

    extern function new(string name = "ext_rm");

    // Write data to ddr

    extern function void insert(ref DATA data);

    // Read data from ddr

    extern function bit check(ref DATA data);

    extern function void report();

    // Process to request data(read response)

    extern task insert_process();

    // Process to response data(read response)

    extern task check_process();

endclass : ext_rm

//------------------------------------------------------------------------------
// IMPLEMENTATION
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// CLASS- ext_rm
//
//------------------------------------------------------------------------------

function ext_rm::new(string name = "ext_rm");
    super.new(name);
endfunction : new

function void ext_rm::insert(ref DATA data);
    super.insert(data);
endfunction : insert

function bit ext_rm::check(ref DATA data);
    DATA exp;
    if (m_data_exp.size()) begin
        exp   = m_data_exp.pop_front();
        if (exp != null) begin
            check = exp.compare(data);
            if (check != 'd1) begin
                string info = "[Data Compare Error:]\n";
                $sformat(info, "%s------------------------------------\n", info);
                $sformat(info, "%s[Expect Data] is \n%p\n", info, exp);
                $sformat(info, "%s[Actual Data] is \n%p\n", info, data);
                $sformat(info, "%s------------------------------------\n", info);
                `tb_error(m_inst_name, info)
            end
        end else begin
            `tb_error(m_inst_name, "Expect data is null!")
            check = 'd0;
        end
    end else begin
        `tb_error(m_inst_name, "No expect data detected!")
        check = 'd0;
    end
endfunction : check

function void ext_rm::report();
    super.report();
endfunction : report

task ext_rm::insert_process();
    DATA item;
    bit  file_open;
    int  file_id;
    m_cfg = new();
    if (m_cfg.exp_user_cfg != "") begin
        // Open Specified Files
    `ifdef VIVADO
        // `tb_file_opt_open_file(m_cfg.exp_user_cfg, "r", m_file_id)
        m_file_id = $fopen(m_cfg.exp_user_cfg, "r");
    `else
        m_file_id = file_opt::open(m_cfg.exp_user_cfg, "r");
    `endif
        file_open = (m_file_id != 0);
        // Find Files Successfull
        if (file_open != 0) begin
            `tb_info(m_inst_name, {"User-define expect from file ", m_cfg.exp_user_cfg, " !"})
            m_user_exp = 'd1;
        end else if ('d1 == config_opt#()::numeral_check(m_cfg.exp_user_cfg)) begin
            // Check whether stim_user_cfg is valid(Do not contain invalid char)
            `tb_info(m_inst_name, "User-define expect from cfg!")
            m_user_exp = 'd1;
        end else begin
            `tb_warning(m_inst_name, "No user expect specified!")
            return;
        end
    end else begin
        `tb_warning(m_inst_name, "No user expect specified!")
        return;
    end
    forever begin
        int         result;
        int         length;
        bit [7 : 0] data_array[];
        bit [7 : 0] data_byte = 'd0;
        string      addr_str  = "";
        string      len_str   = "";
        string      data_str  = "";
        DATA        data      = new();
    `ifdef VIVADO
        if (m_user_exp) result = $fgets(data_str, m_file_id);
    `else
        if (m_user_exp) result = file_opt::gets(m_cfg.exp_user_cfg, data_str);
    `endif
        if (result != 0) begin
            int addr_start = m_cfg.exp_addr_start << 1;
            int addr_end   = addr_start + (m_cfg.exp_addr_size << 1) - 'd1;
            int len_start  = m_cfg.exp_len_start << 1;
            int len_end    = len_start + (m_cfg.exp_len_size << 1) - 'd1;
            int data_size  = (data_str.len() >> 1) - (m_cfg.exp_data_start << 1);
            // Extract Addr by rules
            addr_str  = data_str.substr(addr_start, addr_end);
            // data.addr = config_opt#(64)::string2bits({"'h", addr_str}, 'd0);
            // Extract Len by rules
            len_str   = data_str.substr(len_start, len_end);
            length    = config_opt#(32)::string2bits({"'h", len_str}, 'd0);
            data_array= new[length];
            // Extract Data by rules
            for (int idx = 'd0; idx < length; idx++) begin
                int    data_start = (m_cfg.exp_data_start + idx) << 1;
                string data_oct   = data_str.substr(data_start, data_start + 'd1);
                data_byte = config_opt#(8)::string2bits({"'h", data_oct}, 'd0);
                data_array[idx] = data_byte;
                if (idx + 'd1 > data_size) break;
            end
            data.data  = data_array ;
            m_data_exp.push_back(data);
        end else begin
            break;
        end
    end
    // Close Specified Files
`ifdef VIVADO
    $fclose(m_file_id);
`else
    file_opt::close(m_cfg.exp_user_cfg);
`endif
    forever begin
        // Get request
        m_ist_mlbx.get(item);
        if (item == null) continue;
        // Insert Data
        insert(item);
    end
endtask : insert_process

task ext_rm::check_process();
    super.check_process();
endtask : check_process

`endif // _EXT_RM_SV_
