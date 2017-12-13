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


`ifndef _EXT_STIM_CFG_SVH_
`define _EXT_STIM_CFG_SVH_

// /stimu/axi_stim_cfg.svh
`include "axi_stim_cfg.svh"

class ext_stim_cfg extends axi_stim_cfg;

    // Address {{{
    int               axi_addr_start;  // Address Start Position(Byte)
    int               axi_addr_size ;  // Address Length(Bytes, can not longger than 8 octes)
    bit               axi_addr_user ;  // Address User-define
    // }}}

    // Length {{{
    int               axi_len_start ;
    int               axi_len_size  ;
    bit               axi_len_user  ;
    // }}}

    // Data {{{
    int               axi_data_start;  // Data Start position
    // }}}

    // Bandwidth {{{
    longint           axi_bandwidth;   // PPS(Packet Per Second)
    // }}}

    string            stim_user_cfg;   // Stimulate config file

    extern function new();

    extern function string psdisplay(string prefix = "");

endclass : ext_stim_cfg

function ext_stim_cfg::new();
    super.new();
  `ifndef VIVADO
    axi_addr_start= config_opt#(32)::get_bits("axi_addr_start");
    axi_addr_size = config_opt#(32)::get_bits("axi_addr_size" );
    axi_len_start = config_opt#(32)::get_bits("axi_len_start" );
    axi_len_size  = config_opt#(32)::get_bits("axi_len_size"  );
    axi_data_start= config_opt#(32)::get_bits("axi_data_start");
    stim_user_cfg = config_opt#(32)::get_string("stim_user_cfg");
  `else
    `tc_config_opt_get_bits(axi_addr_start, axi_addr_start, 'd0)
    `tc_config_opt_get_bits(axi_addr_size,  axi_addr_size , 'd0)
    `tc_config_opt_get_bits(axi_len_start,  axi_len_start , 'd0)
    `tc_config_opt_get_bits(axi_len_size,   axi_len_size  , 'd0)
    `tc_config_opt_get_bits(axi_data_start, axi_data_start, 'd0)
    `tc_config_opt_get_string(stim_user_cfg, stim_user_cfg, "" )
  `endif
endfunction : new

function string ext_stim_cfg::psdisplay(string prefix = "");
// Aoid $psprintf when simulator is vivado because this system function was not suportted by vivado
`ifndef VIVADO
    psdisplay = {prefix, super.psdisplay(), 
                 $psprintf({"------------------------------------", 
                            "addr_start=%1d, addr_size=%1d, len_start=%1d, len_size=%1d, data_start=%1d\nusercfg=%s"}, 
                            axi_addr_start, axi_addr_size, axi_len_start, axi_len_size, axi_data_start, stim_user_cfg)};
`else
    $sformat(psdisplay, {prefix, super.psdisplay(), "------------------------------------", 
                         "addr_start=%1d, addr_size=%1d, len_start=%1d, len_size=%1d, data_start=%1d\nusercfg=%s"}, 
                         axi_addr_start, axi_addr_size, axi_len_start, axi_len_size, axi_data_start, stim_user_cfg);
`endif
endfunction : psdisplay

`endif // _EXT_STIM_CFG_SVH_