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

`ifndef _DDR_MODEL_SV_
`define _DDR_MODEL_SV_

// ./stim/axi_data.svh
`include "axi_data.svh"

class ddr_model #(type REQ  = axi_data,
                  type RSP  = REQ,
                  int  DNUM = 'd1);

    //----------------------------------
    // Usertype define
    //----------------------------------

    typedef bit [63 : 0] ADDR_t;
    typedef bit [7  : 0] DATA_t;

    typedef enum {
        e_DDR_MSB = 'd0,
        e_DDR_LSB = 'd1
    } ddr_oeder_t;

    typedef mailbox   #(REQ) REQMLBX_t;
    typedef mailbox   #(RSP) RSPMLBX_t;

    //----------------------------------
    // Varible declaration
    //----------------------------------
 
    // DDR Strorage

    static    DATA_t         m_data[DNUM][ADDR_t];

    protected mailbox #(REQ) m_req_mlbx;
    protected mailbox #(RSP) m_rsp_mlbx;

    local     mailbox #(REQ) m_rd_mlbx[DNUM];

    protected string         m_inst_name;

    local     bit            m_start;
    local     bit            m_stop;

    //----------------------------------
    // Task and function declaration
    //----------------------------------

    extern function new(string name = "ddr_model");

    extern local task run();

    extern virtual task start();
    extern virtual task stop();
    extern virtual task main();

    // 
    // Set the handle of the m_req_mlbx in order to bind mailbox to generator
    //
    
    extern function void set_reqmlbx(ref mailbox #(REQ) req_mlbx);

    // 
    // Set the handle of the m_rsp_mlbx in order to bind mailbox to generator
    //
    
    extern function void set_rspmlbx(ref mailbox #(REQ) rsp_mlbx);

    // Write data to ddr

    extern static task write(input     int         id = 'd0,
                             input     int         len= 'd1,
                             input     ADDR_t      addr, 
                             const ref DATA_t      data[]);

    // Read data from ddr

    extern static task read(input     int         id = 'd0,
                            input     int         len= 'd1,
                            input     ADDR_t      addr, 
                            ref       DATA_t      data[]);
    
    // Get read response

    extern task get_response(input     int  ddr_id = 'd0,
                             const ref REQ  req, 
                             ref       RSP  rsp);

    // Process to request data(read response)

    extern task request_process();

    // Process to response data(read response)

    extern task response_process();

endclass : ddr_model

function ddr_model::new(string name = "ddr_model");
    m_inst_name = name;
    foreach (m_rd_mlbx[idx]) m_rd_mlbx[idx] = new();
endfunction : new

task ddr_model::run();
    fork
        begin
            wait (m_start == 'd1);
            main();
        end
        begin
            wait (m_start == 'd1);
            wait (m_stop  == 'd1);
            m_start = 'd0;
            disable fork;
        end
    join_none
endtask : run

task ddr_model::start();
    m_start = 'd1;
    // Fork the main task
    fork
        run();
    join_none
endtask: start

task ddr_model::stop();
    m_stop = 'd1;
    wait (m_start == 'd0);
endtask: stop

task ddr_model::main();
    fork
        request_process();
        response_process();
        // user_process();
    join_none
endtask : main

function void ddr_model::set_reqmlbx(ref mailbox #(REQ) req_mlbx);
    m_req_mlbx = req_mlbx;
endfunction: set_reqmlbx

function void ddr_model::set_rspmlbx(ref mailbox #(REQ) rsp_mlbx);
    m_rsp_mlbx = rsp_mlbx;
endfunction: set_rspmlbx

task ddr_model::write(input     int         id = 'd0,
                      input     int         len= 'd1,
                      input     ADDR_t      addr, 
                      const ref DATA_t      data[]);
    if (id < DNUM) begin
        if (len <= 'd0) len = 'd1;
        for (int idx = 'd0; idx < len; idx++) begin
            m_data[id][addr + idx] = data[idx];
        end
    end
endtask : write

task ddr_model::read(input     int         id = 'd0,
                     input     int         len= 'd1,
                     input     ADDR_t      addr, 
                     ref       DATA_t      data[]);
    if (id < DNUM) begin
        if (len <= 'd0) len = 'd1;
        for (int idx = 'd0; idx < len; idx++) begin
            data[idx] = m_data[id][addr + idx];
        end
    end
endtask : read

task ddr_model::get_response(input     int  ddr_id = 'd0,
                             const ref REQ  req, 
                             ref       RSP  rsp);
    int data_size = req.data.size();
    rsp = new req;
    if (data_size) begin
    `ifndef VIVADO
        read(ddr_id, data_size, rsp.addr, rsp.data);
    `else
        int    id     = rsp.id;
        ADDR_t addr   = rsp.addr;
        DATA_t data[] = new[data_size];
        read(ddr_id, data_size, addr, data);
        rsp.set_data(data);
    `endif
    end else begin
        `tb_error(m_inst_name, "Read data length can not be 0!")
    end
endtask : get_response

task ddr_model::request_process();
`ifndef VIVADO
    for (int idx = 'd0; idx < DNUM; idx++) begin
        automatic int ddr_id = idx;
`endif
        fork : ddr_model_req_proc
            forever begin
                REQ req;
                REQ data;
                // Get request
                m_req_mlbx.get(req);
                // Judge Data
                if (req == null) begin
                    `tb_error(m_inst_name, "Request can not be null!")
                    continue;
                end else if (req.opt == e_AXI_OPT_WR) begin
                    // Write DDR
                    if (req.data.size()) begin
                    `ifndef VIVADO
                        write(ddr_id, req.data.size(), req.addr, req.data);
                    `else
                        write('d0, req.data.size(), req.addr, req.data);
                    `endif
                    end else begin
                        `tb_error(m_inst_name, "Write data can not be empty!")
                    end
                end else if (req.opt == e_AXI_OPT_RD) begin
                    // Read DDR
                    data = new req;
                `ifndef VIVADO
                    m_rd_mlbx[ddr_id].put(data);
                `else
                    m_rd_mlbx['d0].put(data);
                `endif
                end else begin
                    `tb_error(m_inst_name, "Ilegal ddr operation!")
                    continue;
                end
            end
        join_none
`ifndef VIVADO
    end
`endif
endtask : request_process

task ddr_model::response_process();
`ifndef VIVADO
    for (int idx = 'd0; idx < DNUM; idx++) begin
        automatic int ddr_id = idx;
        fork : ddr_model_rsp_proc
`endif
            forever begin
                REQ data;
                RSP item;
                // Get request
            `ifndef VIVADO
                m_rd_mlbx[ddr_id].get(data);
            `else
                m_rd_mlbx['d0].get(data);
            `endif
                if (data == null) begin
                    continue;
                end else begin
                `ifndef VIVADO
                    get_response(ddr_id, data, item);
                `else
                    get_response('d0, data, item);
                `endif
                end
            `ifndef VIVADO
                fork : ddr_model_send_rsp
                    begin
                        RSP rsp = new item;
                        // Delays 200ns to response
                        #200ns;
                        m_rsp_mlbx.put(rsp);
                    end
                join_none
                #0;
            `else
                m_rsp_mlbx.put(item);
            `endif
            end
`ifndef VIVADO
        join_none
    end
`endif
endtask : response_process

`endif // _DDR_MODEL_SV_

