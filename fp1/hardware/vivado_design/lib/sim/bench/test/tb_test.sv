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


`ifndef _TB_TEST_SV_
`define _TB_TEST_SV_

// ./common/tb_log.svh
`include "tb_log.svh"

// ./test/tb_env.sv
`include "tb_env.sv"

// ./test/test_top.sv
`include "test_top.sv"

typedef class tb_test;

virtual class base_wrapper;

    virtual function tb_test create(string name);
        return null;
    endfunction : create

endclass : base_wrapper

class tb_test_wrapper #(type T = tb_test) extends base_wrapper;

    typedef tb_test_wrapper #(T) this_type_t;

    local static this_type_t m_self;

    static function this_type_t get();
        if (m_self == null) begin
            m_self = new();
        end
        get = m_self;
    endfunction : get

    function tb_test create(string name);
        T test = new(name);
        return test;
    endfunction : create

endclass : tb_test_wrapper

virtual class tb_test;

`define tb_register_test(NAME) \
    typedef tb_test_wrapper #(NAME) wrapper; \
    static test_top m_test_top = test_top::register(`"NAME`", wrapper::get());

    // Test name
    protected string         m_inst_name;

    // bit                      m_test_valid = test_top::register(this);

    // Testbench
    protected static tb_env  m_tb_env;

    extern function new(string name = "tb_test");

    extern virtual function void build();
    extern virtual function void connect();
    extern virtual function void start_of_simulation();

    extern virtual task reset();
    extern virtual task configure();
    extern virtual task start();
    extern virtual task run();
    extern virtual task stop();

    extern virtual function void end_of_simulation();
    extern virtual function void report();

endclass : tb_test

function tb_test::new(string name = "tb_test");
    m_inst_name = name;
endfunction : new

function void tb_test::build();
    m_tb_env  = new("m_tb_env"  );
    m_tb_env.build();
endfunction : build

function void tb_test::connect();
    m_tb_env.connect();
endfunction : connect

function void tb_test::start_of_simulation();
    string info;
    $sformat(info, {"\n+---------------------------------------------+",
                    "\n|    RUNING TESTCASE :  %20s  |", 
                    "\n+---------------------------------------------+\n"}, 
                    m_inst_name);
    `tb_info(m_inst_name, info)
endfunction : start_of_simulation

task tb_test::reset();
    wait (m_tb_env.m_tb_vif.rst_200m_done);
    wait (m_tb_env.m_tb_vif.rst_100m_done);
    #10ns;
    `tb_info(m_inst_name, "\n-------------System reset done-----------------\n")
endtask : reset

task tb_test::configure();
endtask : configure

task tb_test::start();
    m_tb_env.m_axilm_bfm.start();
`ifdef USE_DDR_MODEL
    m_tb_env.m_axisd_bfm.start();
    m_tb_env.m_ddr_model.start();
`endif
endtask : start

task tb_test::run();
endtask : run

task tb_test::stop();
    m_tb_env.m_axilm_bfm.stop();
`ifdef USE_DDR_MODEL
    m_tb_env.m_axisd_bfm.stop();
    m_tb_env.m_ddr_model.stop();
`endif
endtask : stop

function void tb_test::end_of_simulation();
endfunction : end_of_simulation

function void tb_test::report();
endfunction : report

`endif // _TB_TEST_SV_

