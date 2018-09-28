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


`ifndef _CONFIG_DB_SVH_
`define _CONFIG_DB_SVH_

// ./common/tb_log.svh
`include "tb_log.svh"

class config_db #(type T = int);

    //----------------------------------
    // Macro Define
    //----------------------------------

    //----------------------------------
    // Varible declaration
    //----------------------------------

    static local T  m_val[string];

    //----------------------------------
    // Task and function declaration
    //----------------------------------
    
    extern static function bit set(input string name,
                                   input T      value);

    extern static function bit get(input  string name, 
                                   output T      value);

endclass : config_db

//------------------------------------------------------------------------------
// IMPLEMENTATION
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// CLASS- config_db
//
//------------------------------------------------------------------------------

function bit config_db::set(input string name, 
                            input T      value);
    if (name != "") begin
        m_val[name] = value;
        set = 'd1;
    end else begin
        `tb_warning("config_db", "The name of value that you want to set can not be empty!")
        set = 'd0;
    end
endfunction : set

function bit config_db::get(input  string name,
                            output T      value);
    if (m_val.exists(name)) begin
        value = m_val[name];
        get   = 'd1;
    end else begin
        `tb_warning("config_db", "No inst found by specified name, please check the name!")
        get   = 'd0;
    end
endfunction : get

`endif // _CONFIG_DB_SVH_

