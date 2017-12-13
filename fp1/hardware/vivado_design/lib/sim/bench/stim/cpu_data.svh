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


`ifndef _CPU_DATA_SVH_
`define _CPU_DATA_SVH_

// common/common_axi.svh
`include "common_axi.svh"

class cpu_data;

    //----------------------------------
    // Usertype define
    //----------------------------------
 
    // CPU Data type

    typedef enum {
        e_CPU_WR_BD   = 'd0,
        e_CPU_WR_REQ,
        e_CPU_WR_DATA,
        e_CPU_RD_BD  ,
        e_CPU_RD_DATA
    } cpu_head_t;

    // Write BD type
    typedef struct packed {
        bit [7  : 0] prty       ; // [255:248]
        bit [7  : 0] bdcode     ; // [247:240]
        bit [23 : 0] rsv        ; // [239:216]
        bit [7  : 0] acc_type   ; // [215:208]
        bit [47 : 0] ve_info    ; // [207:160]
        bit [31 : 0] data_len   ; // [159:128]
        bit [63 : 0] dest_addr  ; // [127: 64]
        bit [63 : 0] src_addr   ; // [63 :  0]
    } cpu_bd_t;

    // DMA response
    typedef struct packed {
        bit [31 : 0] ae_info    ; // [255:224]
        bit [7  : 0] rsv1       ; // [223:216]
        bit [7  : 0] acc_type   ; // [215:208]
        bit [47 : 0] ve_info    ; // [207:160]
        bit [31 : 0] pkt_len    ; // [159:128]
        bit [63:  0] rsv0       ; // [127: 64]
        bit [63 : 0] dma_addr   ; // [ 63:  0]
    } cpu_dmareq_t;

    typedef struct packed {
        bit [31 : 0] ae_info    ; // [255:224]
        bit [ 6 : 0] rsv2       ; // [223:217]
        bit          done_flag  ; // 216
        bit [7  : 0] acc_type   ; // [215:208]
        bit [47 : 0] ve_info    ; // [207:160]
        bit [31 : 0] rsv1       ; // [159:128]
        bit [63 : 0] dest_addr  ; // [127: 64]
        bit [63 : 0] rsv0       ; // [ 63:  0]
    } cpu_dmadata_t;

    //----------------------------------
    // Varible declaration
    //----------------------------------
    
    cpu_head_t          cpu_type ; // Cpu data type

    local cpu_bd_t      bd_head  ;
    local cpu_dmareq_t  dma_req  ;
    local cpu_dmadata_t dma_data ;

    rand  bit [47 : 0]  ve_info  ;   // ID
    rand  bit [31 : 0]  ae_info  ;   // AE Private Info
    rand  bit [11 : 0]  data_len ;
    rand  bit [63 : 0]  addr     ;   // 
    bit [63: 0]         dest_addr;   // 
    bit [7 : 0]         acc_type ;
    bit                 done_flag;
    bit                 acc_len  ;

    // Base constraint
    constraint cpu_data_base_constraint {
    }

    extern function new();

    extern function string psdisplay(string prefix = "");
    extern function void   dispaly(string prefix = "");

    extern function int pack_bytes(ref bit [7 : 0] bytes[]);
    extern function int unpack_bytes(const ref bit [7 : 0] bytes[]);

endclass : cpu_data

//----------------------------------------------------------------------------------------------------------------------
// IMPLEMENTATION
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
//
// CLASS- cpu_data
//
//----------------------------------------------------------------------------------------------------------------------

function cpu_data::new();
endfunction : new

function string cpu_data::psdisplay(string prefix = "");
    psdisplay = prefix;
endfunction : psdisplay

function void cpu_data::dispaly(string prefix = "");
    $display(psdisplay(prefix));
endfunction : dispaly

function int cpu_data::pack_bytes(ref bit [7 : 0] bytes[]);
    case (cpu_type)
        e_CPU_WR_BD, e_CPU_RD_BD : begin
            cpu_bd_t head  ;
            bd_head = 'd0;
            bd_head.acc_type  = acc_type ;
            bd_head.ve_info   = ve_info  ;
            bd_head.data_len  = data_len ;
            bd_head.src_addr  = addr     ;
            bd_head.dest_addr = dest_addr;
            head              = {<<8{bd_head}};
            {>>8{bytes}} = head;
        end
        e_CPU_WR_REQ : begin
            cpu_dmareq_t  req;
            dma_req = 'd0;
            dma_req.ae_info   = ae_info  ;
            dma_req.acc_type  = acc_type ;
            dma_req.ve_info   = ve_info  ;
            dma_req.dma_addr  = addr     ;
            dma_req.pkt_len   = data_len ;
            req               = {<<8{dma_req}};
            {>>8{bytes}} = req;
        end
        e_CPU_WR_DATA, e_CPU_RD_DATA : begin
            cpu_dmadata_t data ;
            dma_data = 'd0;
            dma_data.ae_info  = ae_info  ;
            dma_data.done_flag= done_flag;
            dma_data.acc_type = acc_type ;
            dma_data.ve_info  = ve_info  ;
            dma_data.dest_addr= addr     ;
            data              = {<<8{dma_data}};
            {>>8{bytes}} = data;
        end
    endcase
    pack_bytes = bytes.size();
endfunction : pack_bytes

function int cpu_data::unpack_bytes(const ref bit [7 : 0] bytes[]);
    case (cpu_type)
        e_CPU_WR_BD, e_CPU_RD_BD : begin
            cpu_bd_t head  ;
            head     = {>>8{bytes}};
            bd_head  = {<<8{head}};
            acc_type = bd_head.acc_type  ;
            ve_info  = bd_head.ve_info   ;
            data_len = bd_head.data_len  ;
            addr     = bd_head.src_addr  ;
            dest_addr= bd_head.dest_addr ;
        end
        e_CPU_WR_REQ : begin
            cpu_dmareq_t  req;
            req      = {>>8{bytes}};
            dma_req  = {<<8{req}};
            ae_info  = dma_req.ae_info   ;
            acc_type = dma_req.acc_type  ;
            ve_info  = dma_req.ve_info   ;
            addr     = dma_req.dma_addr  ;
            data_len = dma_req.pkt_len   ;
        end
        e_CPU_WR_DATA, e_CPU_RD_DATA : begin
            cpu_dmadata_t data ;
            data     = {>>8{bytes}};
            dma_data = {<<8{data}};
            ae_info  = dma_data.ae_info  ;
            done_flag= dma_data.done_flag;
            acc_type = dma_data.acc_type ;
            ve_info  = dma_data.ve_info  ;
            addr     = dma_data.dest_addr;
        end
    endcase
    unpack_bytes = bytes.size();
endfunction : unpack_bytes

`endif // _CPU_DATA_SVH_

