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


`ifndef _AXI_DATA_SVH_
`define _AXI_DATA_SVH_

// ./common/common_axi.svh
`include "common_axi.svh"

class axi_data;

    `define axi_data_copy_items(from, to) \
        to.id         = from.id        ; \
        to.addr       = from.addr      ; \
        to.data_array = from.data_array; \
        to.data       = from.data      ; \
        to.opt        = from.opt       ; \
        to.btype      = from.btype     ; \
        to.resp       = from.resp      ;

    //----------------------------------
    // Varible declaration
    //----------------------------------
    
    rand bit [3  : 0] id  ;   // ID
    rand bit [63 : 0] addr;   // Address
    // Only used for WRAP or INCR Mode
    bit [7 : 0]       data_array[][];

`ifndef VIVADO
    rand bit [ 7 : 0] data[]; // Data
`else
    bit [ 7 : 0]      data[]; // Data
`endif
    rand axi_opt_t    opt;    // Operatio type
    rand axi_burst_t  btype;  // Burst type

    rand axi_resp_t   resp;   // Response

    int               stream; // Steam Id
    int               fsn   ; // Fsn Id


    //----------------------------------
    // Constraint declaration
    //----------------------------------

    // Base constraint
    constraint axi_data_base_constraint {
    `ifndef VIVADO
        data.size() > 0;
    `endif
    }

    extern function new();

    extern function axi_data copy(input axi_data cpy = null);
    extern function bit compare(input axi_data cmp = null);

    extern function string psdisplay(string prefix = "");
    extern function void   display(string prefix = "");

`ifdef VIVADO
    extern function void set_data(ref bit [7 : 0] data[]);
`endif

endclass : axi_data

//----------------------------------------------------------------------------------------------------------------------
// IMPLEMENTATION
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
//
// CLASS- axi_data
//
//----------------------------------------------------------------------------------------------------------------------

function axi_data::new();
    addr = 'd0;
    data_array.delete();
    data.delete();
endfunction : new

function axi_data axi_data::copy(input axi_data cpy = null);
    // If input handle is not null, copy input data to this, however, if is
    // null, create a new inst and copy this to the new inst.
    if (cpy != null) begin
        `axi_data_copy_items(cpy, this)
    end else begin
        copy = new();
        `axi_data_copy_items(this, copy)
    end
endfunction : copy

function bit axi_data::compare(input axi_data cmp = null);
    int    cmp_size  = 'd0;
    int    data_size = data.size();
    bit [7 : 0] data_array[];
    if (cmp == null) begin
        `tb_error("compare", "Compare fail, the data need to compare can not be null")
        return 0;
    end
    // Do not compare 2 data arrays because vivado warning
    data_array= cmp.data;
    cmp_size  = data_array.size();
    compare   = (this.addr == cmp.addr) && (cmp_size == data_size);
    cmp_size  = (data_size <= cmp_size) ? data_size : cmp_size ;
    for (int idx = 'd0; idx < cmp_size; idx++) begin
        bit    equal;
        // Vivado simulator do not support to compare to unit of data_array?
        // That is ridiculors
        bit [7 : 0] data_byte = data[idx];
        bit [7 : 0] cmp_byte  = cmp.data[idx];
        equal    =  (data_byte == cmp_byte);
        compare &=  equal;
    end
endfunction : compare

function string axi_data::psdisplay(string prefix = "");
// Aoid $psprintf when simulator is vivado because this system function was not suportted by vivado
`ifndef VIVADO
    psdisplay = {prefix, $psprintf({"------------------------------------", 
                                    "id=0x%x;addr=0x%x, opt=%s, btype=%s, \ndata=%p"}, 
                                   id, addr, opt.name, btype.name, data)};
`else
    $sformat(psdisplay, {prefix, "------------------------------------", 
                         "id=0x%x;addr=0x%x, opt=%s, btype=%s, \ndata=%p"}, 
                         id, addr, opt.name, btype.name, data);
`endif
endfunction : psdisplay

function void axi_data::display(string prefix = "");
    $display(psdisplay(prefix));
endfunction : display

`ifdef VIVADO
function void axi_data::set_data(ref bit [7 : 0] data[]);
    this.data = data;
endfunction : set_data
`endif

`endif // _AXI_DATA_SVH_

