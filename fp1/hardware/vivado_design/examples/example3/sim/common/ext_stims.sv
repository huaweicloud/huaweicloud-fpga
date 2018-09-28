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

`ifndef _EXT_STIMS_SV_
`define _EXT_STIMS_SV_

// ./common/file_opt.svh
`include "file_opt.svh"

// ./stimu/axi_stims.sv
`include "axi_stims.sv"

// /stimu/ext_stim_cfg.svh
`include "ext_stim_cfg.svh"

class ext_stims extends axi_stims;

    protected bit [3  : 0]   id  ;         // ID
    protected bit [63 : 0]   addr;         // Address

    protected bit  [7 : 0]   data[];       // Data

    protected axi_opt_t      opt;          // Operatio type
    protected axi_burst_t    btype;        // Burst type

    protected axi_resp_t     resp;         // Response

    local     bit            m_file_open;
    local     bit            m_file_end ;  // End of file
    local     int            m_file_id;

    protected ext_stim_cfg   m_cfg;

    //----------------------------------
    // Task and function declaration
    //----------------------------------
    
    extern function new(string name = "extend_stim");

    extern virtual task body();

    extern virtual task gen_packet();
    extern virtual task send_packet();

endclass : ext_stims

//------------------------------------------------------------------------------
// IMPLEMENTATION
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// CLASS- ext_stims
//
//------------------------------------------------------------------------------

function ext_stims::new(string name = "extend_stim");
    super.new(name);
    m_file_id = 'd0;
endfunction : new

task ext_stims::body();
    m_cfg = new();
    if (m_cfg.stim_user_cfg != "") begin
        // Open Specified Files
    `ifdef VIVADO
        m_file_id = $fopen(m_cfg.stim_user_cfg, "r");
        // `tb_file_opt_open_file(m_cfg.stim_user_cfg, "r", m_file_id)
    `else
        m_file_id = file_opt::open(m_cfg.stim_user_cfg, "r");
    `endif
        m_file_open = (m_file_id != 0);
        // Find Files Successfull
        if (m_file_open != 0) begin
            `tb_info(m_inst_name, {"User-define stim from file ", m_cfg.stim_user_cfg, " !"})
        end else if ('d1 == config_opt#()::numeral_check(m_cfg.stim_user_cfg)) begin
            // Check whether stim_user_cfg is valid(Do not contain invalid char)
            `tb_info(m_inst_name, "User-define stim from cfg!")
        end else begin
            `tb_warning(m_inst_name, "No stim specified!")
            m_end = 'd1;
            return;
        end
    end else begin
        `tb_warning(m_inst_name, "No stim specified!")
        m_end = 'd1;
        return;
    end
    
    forever begin
        if (m_stop == 'd1) break;
        // Generate data
        gen_packet();
        // If end of file, break loop
        if (m_file_end == 'd1) break;
        // Send data
        send_packet();
        ++m_inst_num;
        // Check when to stop stim
        if (m_cfg.axi_inst_num && m_cfg.axi_inst_num <= m_inst_num) break;
    end
    // Close Specified Files
`ifdef VIVADO
    $fclose(m_file_id);
`else
    file_opt::close(m_cfg.stim_user_cfg);
`endif
    m_end = 'd1;
endtask : body

task ext_stims::gen_packet();
    int         result;
    int         length;
    bit [7 : 0] data_byte = 'd0;
    string      addr_str  = "";
    string      len_str   = "";
    string      data_str  = "";
    // Generate data
`ifdef VIVADO
    if (m_file_open) result = $fgets(data_str, m_file_id);
`else
    if (m_file_open) result = file_opt::gets(m_cfg.stim_user_cfg, data_str);
`endif
    if (result == 0) begin
        m_file_end = 'd1;
    end else begin
        int addr_start = m_cfg.axi_addr_start << 1;
        int addr_end   = addr_start + (m_cfg.axi_addr_size << 1) - 'd1;
        int len_start  = m_cfg.axi_len_start << 1;
        int len_end    = len_start + (m_cfg.axi_len_size << 1) - 'd1;
        int data_size  = (data_str.len() >> 1) - (m_cfg.axi_data_start << 1);
        // Extract Addr by rules
        addr_str = data_str.substr(addr_start, addr_end);
        addr     = config_opt#(64)::string2bits({"'h", addr_str}, 'd0);
        // Extract Len by rules
        len_str  = data_str.substr(len_start, len_end);
        length   = config_opt#(32)::string2bits({"'h", len_str}, 'd0);
        data     = new[length];
        // Extract Data by rules
        for (int idx = 'd0; idx < length; idx++) begin
            int    data_start = (m_cfg.axi_data_start + idx) << 1;
            string data_oct   = data_str.substr(data_start, data_start + 'd1);
            data_byte = config_opt#(8)::string2bits({"'h", data_oct}, 'd0);
            data[idx] = data_byte;
            if (idx + 'd1 > data_size) break;
        end
        m_item.addr  = addr ;
        m_item.id    = id   ;
        m_item.opt   = opt  ;
        m_item.btype = btype;
        m_item.resp  = resp ;
        m_item.data  = data ;
    end
endtask : gen_packet

task ext_stims::send_packet();
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
    `tb_info("ext_stims", info)
    // There is no delay for stim send. You can add time delay here if you need.
endtask : send_packet

`endif // _EXT_STIMS_SV_
