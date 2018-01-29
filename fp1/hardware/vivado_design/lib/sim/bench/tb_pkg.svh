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


`ifndef _TB_PKG_SVH_
`define _TB_PKG_SVH_

// ./test/tb_env.sv
`include "tb_env.sv"
// ./test/tb_test.sv
`include "tb_test.sv"

int     unsigned g_tb_regs[longint unsigned];
mailbox #(longint unsigned) g_tb_reg_wr_req = new();
mailbox #(longint unsigned) g_tb_reg_rd_req = new();
semaphore                   g_tb_reg_rd_rsp = new();

bit                         g_sim_end = 'd0;

export "DPI-C" task ul_reg_write;
export "DPI-C" task ul_reg_read;
// export "DPI-C" task finish_sim;
export "DPI-C" task tb_delay;
export "DPI-C" task tb_report;

export "DPI-C" task cfg_get_string;
export "DPI-C" task cfg_get_int;

task ul_reg_write(input  longint unsigned addr, 
                  input  int     unsigned data);
    g_tb_regs[addr] = data;
    g_tb_reg_wr_req.put(addr);
endtask : ul_reg_write

task ul_reg_read(input  longint unsigned addr, 
                 output int     unsigned data);
    g_tb_reg_rd_req.put(addr);
    // Wait response
    g_tb_reg_rd_rsp.get();
    data = g_tb_regs[addr];
endtask : ul_reg_read

// task finish_sim();
//     g_sim_end = 'd1;
// endtask : finish_sim

task tb_delay(input int x);
    repeat (x) #1us;
endtask : tb_delay

task tb_report(input int    level, 
               input string id,
               input string info);
    case (level)
        e_LOG_FATAL   : `tb_fatal(id,   info)
        e_LOG_ERROR   : `tb_error(id,   info)
        e_LOG_WARNING : `tb_warning(id, info)
        default       : `tb_info( id,   info)
    endcase
endtask : tb_report

task cfg_get_string(input  string name, 
                    output string value, 
                    input  string dflt);
    `tc_config_opt_get_string(name, value, dflt)
endtask : cfg_get_string

task cfg_get_int(input  string name, 
                 output int    value, 
                 input  int    dflt);
    `tc_config_opt_get_bits(name, value, dflt)
endtask : cfg_get_int

`endif // _TB_PKG_SVH_

