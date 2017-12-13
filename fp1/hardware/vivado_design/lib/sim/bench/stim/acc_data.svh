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

`ifndef _ACC_DATA_SVH_
`define _ACC_DATA_SVH_

class acc_data;

    //----------------------------------
    // Macro define
    //----------------------------------

    `define ACC_HEAD_LEN  ($bits(acc_head_t) >> 3)

    //----------------------------------
    // Usertype define
    //----------------------------------

    // Write BD type
    typedef struct packed {
        bit [127: 0] rsv        ; // [255:128]
        bit [63 : 0] dest_addr  ; // [127: 64]
        bit [63 : 0] src_addr   ; // [63 :  0]
    } acc_head_t;

    //----------------------------------
    // Varible declaration
    //----------------------------------
    
    local acc_head_t    acc_head ;

    rand  bit [63 : 0]  addr     ;   // 
    bit [63: 0]         dest_addr;   // 

    // Base constraint
    constraint acc_data_base_constraint {
    }

    extern function new();

    extern function bit compare(input acc_data cmp = null);

    extern function string psdisplay(string prefix = "");
    extern function void   dispaly(string prefix = "");

    extern function int pack_bytes(ref bit [7 : 0] bytes[]);
    extern function int unpack_bytes(const ref bit [7 : 0] bytes[]);

endclass : acc_data

//----------------------------------------------------------------------------------------------------------------------
// IMPLEMENTATION
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
//
// CLASS- acc_data
//
//----------------------------------------------------------------------------------------------------------------------

function acc_data::new();
endfunction : new

function bit acc_data::compare(input acc_data cmp = null);
    bit [63 : 0]  laddr_s;
    bit [63 : 0]  laddr_d;
    if (cmp == null) begin
        `tb_error("compare", "Compare fail, the data need to compare can not be null")
        return 0;
    end
    laddr_s = cmp.addr;
    laddr_d = cmp.dest_addr;
    compare = (addr == laddr_s) && (dest_addr == laddr_d);
endfunction : compare

function string acc_data::psdisplay(string prefix = "");
    $sformat(psdisplay, {prefix, "------------------------------------", 
                         "source_addr=0x%x, dest_addr=0x%x"}, addr, dest_addr);
endfunction : psdisplay

function void acc_data::dispaly(string prefix = "");
    $display(psdisplay(prefix));
endfunction : dispaly

function int acc_data::pack_bytes(ref bit [7 : 0] bytes[]);
    acc_head = 'd0;
    acc_head.src_addr  = addr     ;
    acc_head.dest_addr = dest_addr;
    acc_head = {<<8{acc_head}};
    {>>8{bytes}}       = acc_head;
    pack_bytes         = bytes.size();
endfunction : pack_bytes

function int acc_data::unpack_bytes(const ref bit [7 : 0] bytes[]);
    acc_head = {>>8{bytes}};
    acc_head = {<<8{acc_head}};
    addr     = acc_head.src_addr  ;
    dest_addr= acc_head.dest_addr ;
    unpack_bytes = bytes.size();
endfunction : unpack_bytes

`endif // _ACC_DATA_SVH_
