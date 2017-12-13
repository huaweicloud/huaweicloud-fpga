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


`ifndef _EXT_EXP_CFG_SVH_
`define _EXT_EXP_CFG_SVH_

class ext_exp_cfg;

    // Address {{{
    int               exp_addr_start;  // Address Start Position(Byte)
    int               exp_addr_size ;  // Address Length(Bytes, can not longger than 8 octes)
    bit               exp_addr_user ;  // Address User-define
    // }}}

    // Length {{{
    int               exp_len_start ;
    int               exp_len_size  ;
    bit               exp_len_user  ;
    // }}}

    // Data {{{
    int               exp_data_start;  // Data Start position
    // }}}

    string            exp_user_cfg;    // Expect config file

    extern function new();

    extern function string psdisplay(string prefix = "");

endclass : ext_exp_cfg

function ext_exp_cfg::new();
  `ifndef VIVADO
    exp_addr_start= config_opt#(32)::get_bits("exp_addr_start");
    exp_addr_size = config_opt#(32)::get_bits("exp_addr_size" );
    exp_len_start = config_opt#(32)::get_bits("exp_len_start" );
    exp_len_size  = config_opt#(32)::get_bits("exp_len_size"  );
    exp_data_start= config_opt#(32)::get_bits("exp_data_start");
    exp_user_cfg  = config_opt#(32)::get_string("exp_user_cfg" );
  `else
    `tc_config_opt_get_bits(exp_addr_start, exp_addr_start, 'd0)
    `tc_config_opt_get_bits(exp_addr_size,  exp_addr_size , 'd0)
    `tc_config_opt_get_bits(exp_len_start,  exp_len_start , 'd0)
    `tc_config_opt_get_bits(exp_len_size,   exp_len_size  , 'd0)
    `tc_config_opt_get_bits(exp_data_start, exp_data_start, 'd0)
    `tc_config_opt_get_string(exp_user_cfg, exp_user_cfg,   "" )
  `endif
endfunction : new

function string ext_exp_cfg::psdisplay(string prefix = "");
// Aoid $psprintf when simulator is vivado because this system function was not suportted by vivado
`ifndef VIVADO
    psdisplay = {prefix, 
                 $psprintf({"------------------------------------", 
                            "addr_start=%1d, addr_size=%1d, len_start=%1d, len_size=%1d, data_start=%1d\nusercfg=%s"}, 
                            exp_addr_start, exp_addr_size, exp_len_start, exp_len_size, exp_data_start, exp_user_cfg)};
`else
    $sformat(psdisplay, {prefix, "------------------------------------", 
                         "addr_start=%1d, addr_size=%1d, len_start=%1d, len_size=%1d, data_start=%1d\nusercfg=%s"}, 
                         exp_addr_start, exp_addr_size, exp_len_start, exp_len_size, exp_data_start, exp_user_cfg);
`endif
endfunction : psdisplay

`endif // _EXT_EXP_CFG_SVH_