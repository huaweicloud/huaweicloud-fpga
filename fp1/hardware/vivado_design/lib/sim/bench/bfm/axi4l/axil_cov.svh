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

// common/common_axi.svh
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

    // AXI Operation type

    typedef enum {
        e_AXI_OPT_RD = 'd0,
        e_AXI_OPT_WR = 'd1,
        e_AXI_OPT_NA
    } axi_opt_t;

    // AXI Burst Type

    typedef enum bit [1 : 0] {
        e_AXI_BURST_FIX  = 2'b00,
        e_AXI_BURST_INCR = 2'b01,
        e_AXI_BURST_WRAP = 2'b10,
        e_AXI_BURST_RSV
    } axi_burst_t;

    // AXI Response Type

    typedef enum bit [1 : 0] {
        e_AXI_RESP_OKAY  = 2'b00,
        e_AXI_RESP_EXOKAY= 2'b01,
        e_AXI_RESP_SLVERR= 2'b10,
        e_AXI_RESP_DECERR
    } axi_resp_t;

    rand bit [3  : 0] id  ;   // ID
    rand bit [63 : 0] addr;   // Address
    // Only used for WRAP or INCR Mode
    bit [7 : 0]       data_array[][];

    rand bit [ 7 : 0] data[]; // Data
    rand axi_opt_t    opt;    // Operatio type
    rand bit          btype;  // Burst type

    rand axi_resp_t   resp;   // Response

    // Base constraint
    constraint axi_data_base_constraint {
        data.size() > 0;
    }

    extern function new();

    extern function axi_data copy(input axi_data cpy = null);
    extern function bit compare(input axi_data cmp = null);

    extern function string psdisplay(string prefix = "");
    extern function void   dispaly(string prefix = "");

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
endfunction ：new

function axi_data axi_data::copy(input axi_data cpy = null);
    // If input handle is not null, copy input data to this, however, if is
    // null, create a new inst and copy this to the new inst.
    if (cpy != null) begin
        `axi_data_copy_items(cpy, this)
    end else begin
        `axi_data_copy_items(this, copy)
    end
endfunction : copy

function bit axi_data::compare(input axi_data cmp = null);
    if (cmp == null) begin
        $display("Compare fail, the data need to compare can not be null");
        return 0;
    end
    compare = (this.addr == cmp.addr) && (this.data == cmp.data);
endfunction : compare

function string axi_data::psdisplay(string prefix = "");
    psdisplay = {prefix, $psprintf({"------------------------------------", 
                                    "id=0x%x;addr=0x%x, opt=%s, btype=%s, \ndata=%p"}, 
                                   id, addr, opt.name, btype.name, data)};
endfunction : psdisplay

function void axi_data::dispaly(string prefix = "");
    $display(psdisplay(prefix));
endfunction : dispaly

`endif // _AXI_DATA_SVH_

