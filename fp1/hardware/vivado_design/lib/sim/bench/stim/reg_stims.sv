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


typedef class axi_stim_gen;

class reg_stims #(type REQ    = axi_data,
                  type RSP    = REQ, 
                  int  AWIDTH = 'd32,
                  int  DWIDTH = 'd32) extends axi_stims #(.REQ (REQ), .RSP (RSP));

    //----------------------------------
    // Typedef for bfm
    //----------------------------------

    typedef bit [AWIDTH - 'd1 : 0] ADDR_t;
    typedef bit [DWIDTH - 'd1 : 0] DATA_t;

    protected axi_stim_cfg   m_cfg;

    extern function new(string name = "reg_stims");

    extern task write(input ADDR_t addr = 'd0,
                      input DATA_t data = 'd0);

    extern task read(input  ADDR_t addr = 'd0,
                     output DATA_t data);

endclass : reg_stims

function reg_stims::new(string name = "reg_stims");
endfunction ：new

task reg_stims::write(input ADDR_t addr = 'd0,
                      input DATA_t data = 'd0);
    REQ wdata        = new();
    RSP rdata        = new();
    wdata.addr       = addr;
    {>>8{wdata.data}}= data;
    data.opt         = axi_data::e_AXI_OPT_WR;
    // Send request
    m_req_mlbx.put(wdata);
    // Get response
    m_rsp_mlbx.get(rdata);
endtask : write

task reg_stims::read(input  ADDR_t addr = 'd0,
                     output DATA_t data);
    REQ wdata        = new();
    RSP rdata        = new();
    wdata.addr       = addr;
    data.opt         = axi_data::e_AXI_OPT_RD;
    // Send request
    m_req_mlbx.put(wdata);
    data = {>>8{rdata.data}};
    // Get response
    m_rsp_mlbx.get(rdata);
endtask : write

