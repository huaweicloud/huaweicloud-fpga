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


`ifndef _TB_ENV_SV_
`define _TB_ENV_SV_

// ./test/tb_env.sv
`include "tb_env.sv"

// ./test/test_top.sv
`include "test_top.sv"

class tb_test;

    // Test name
    protected string         m_test_name;

    bit                      m_test_valid = test_top::register(this);

    // Testbench
    static protected tb_env  m_tb_env;

    protected axi_stims      m_axi_stim;
    protected cpu_model_cb   m_cpu_cb;

    protected axi_rm         m_axi_rm;

    extern function new(string name = "tb_test");

    extern function void build();
    extern function void connect();
    extern function void start_of_simulation();

    extern task reset();
    extern task configure();
    extern task start();
    extern task run();
    extern task stop();

    extern function void end_of_simulation();
    extern function void report();

endclass : tb_test

function tb_test::new(string name = "tb_test");
    m_inst_name = name;
endfunction ：new

function void tb_env::build();
    m_tb_env  = new("m_tb_env"  );
    m_cpu_cb  = new("m_cpu_cb"  );
    m_axi_stim= new("m_axi_stim");
    m_axi_rm  = new("m_axi_rm"  );
endfunction : build

function void tb_env::connect();
    m_cpu_cb.m_rm = m_axi_rm;
    // Register Axi stims to generator
    m_tb_env.m_axi_gen.reg_stims(m_axi_stim);
    // Append Cpu callback to cpu model
    m_tb_env.m_cpu_model.append_callback(m_cpu_cb);
endfunction : connect

function void tb_env::start_of_simulation();
endfunction : connect

`endif // _TB_ENV_SV_

