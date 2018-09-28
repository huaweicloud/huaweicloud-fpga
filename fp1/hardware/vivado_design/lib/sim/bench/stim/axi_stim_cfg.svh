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


`ifndef _AXI_STIM_CFG_SVH_
`define _AXI_STIM_CFG_SVH_

// common/common_axi.svh
`include "common_axi.svh"

// common/config_opt.svh
`include "config_opt.svh"

class axi_stim_cfg;

    bit [63 : 0]      axi_addr_min  ;  // Address low range
    bit [63 : 0]      axi_addr_max  ;  // Address max range

    int               axi_data_len  ;  // Data length
    int               axi_opt       ;  // Operatio type
    int               axi_burst_type;  // Burst type

    int               axi_resp      ;  // Response

    int               axi_inst_num  ;  // Inst name

    extern function new();

    extern virtual function string psdisplay(string prefix = "");
    extern virtual function void   display(string prefix = "");

endclass : axi_stim_cfg

function axi_stim_cfg::new();
  `ifndef VIVADO
    axi_addr_min  = config_opt#(64)::get_bits("axi_addr_min"  );
    axi_addr_max  = config_opt#(64)::get_bits("axi_addr_max"  );
    axi_data_len  = config_opt#(32)::get_bits("axi_data_len"  );
    axi_opt       = config_opt#(32)::get_bits("axi_opt"       );
    axi_burst_type= config_opt#(32)::get_bits("axi_burst_type");
    axi_resp      = config_opt#(32)::get_bits("axi_resp"      );
    axi_inst_num  = config_opt#(32)::get_bits("axi_inst_num"  );
  `else
    `tc_config_opt_get_bits(axi_addr_min,   axi_addr_min,  'd0)
    `tc_config_opt_get_bits(axi_addr_max,   axi_addr_max,  'd0)
    `tc_config_opt_get_bits(axi_opt,        axi_opt,       'd0)
    `tc_config_opt_get_bits(axi_burst_type, axi_burst_type,'d0)
    `tc_config_opt_get_bits(axi_data_len,   axi_data_len,  'd0)
    `tc_config_opt_get_bits(axi_resp,       axi_resp,      'd0)
    `tc_config_opt_get_bits(axi_inst_num,   axi_inst_num,  'd0)
  `endif
endfunction : new

function string axi_stim_cfg::psdisplay(string prefix = "");
// Aoid $psprintf when simulator is vivado because this system function was not suportted by vivado
`ifndef VIVADO
    psdisplay = {prefix, $psprintf({"------------------------------------", 
                                    "addr=['h%x:'h%x], data_len=%1d, opt=%1d, btype=%1d, resp=%1d,inst_name=%1d"}, 
                                   axi_addr_max, axi_addr_min, axi_data_len, axi_opt, axi_burst_type, axi_resp, axi_inst_num)};
`else
    $sformat(psdisplay, {prefix, "------------------------------------", 
                                 "addr=['h%x:'h%x], data_len=%1d, opt=%s, btype=%s, resp=%1d,inst_name=%1d"}, 
                         axi_addr_max, axi_addr_min, axi_data_len, axi_opt, axi_burst_type, axi_resp, axi_inst_num);
`endif
endfunction : psdisplay

function void axi_stim_cfg::display(string prefix = "");
    $display(psdisplay(prefix));
endfunction : display

`endif // _AXI_STIM_CFG_SVH_

