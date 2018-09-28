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


`ifndef _TEST_TOP_SV_
`define _TEST_TOP_SV_

// ./common/tb_log.svh
`include "tb_log.svh"

typedef class base_wrapper;
typedef class tb_test;

class test_top;

    static local test_top m_top;

    static local tb_log   m_log = new();

    // Test lib
 
    static protected base_wrapper m_test_lib[string];

    extern static function test_top register(string       name    = "",
                                             base_wrapper wrapper = null);

    extern static function string get_test();
    extern static function tb_test find_test(string name = "");

    extern static task run_test();

    extern static task exec_test(tb_test test = null);

endclass : test_top

function test_top test_top::register(string       name    = "",
                                     base_wrapper wrapper = null);
    $display("Register test case %s", name);
    if (m_test_lib.exists(name)) begin
        $display("Test name can not be same!");
    end else begin
        m_test_lib[name] = wrapper;
    end
    if (m_top == null) m_top = new();
    register = m_top;
endfunction : register

function string test_top::get_test();
    if (!$value$plusargs("TEST_NAME=%s", get_test)) begin
        $display("Get test name fail!");
    end
endfunction : get_test

function tb_test test_top::find_test(string name = "");
    if (!m_test_lib.exists(name)) begin
        $display("Test name has not been ever existed!");
        find_test = null;
    end else begin
        tb_test      test;
        base_wrapper wrapper = m_test_lib[name];
        if (wrapper != null) begin 
            test = m_test_lib[name].create(name);
        end
        find_test = test;
    end
endfunction : find_test

task test_top::run_test();
    string  test_name = get_test();
    tb_test test      = find_test(test_name);
    exec_test(test);
endtask :run_test

task test_top::exec_test(tb_test test = null);
    if (test == null) begin
        $display("Test can not be null!");
        $finish;
    end else begin
        test.build();
        test.connect();
        test.start_of_simulation();

        test.reset();
        test.configure();

        test.start();
        test.run();
        test.stop();

        test.end_of_simulation();
        test.report();
        $finish;
    end
endtask : exec_test

task run_test();
    test_top::run_test();
endtask : run_test

`endif // _TEST_TOP_SV_

