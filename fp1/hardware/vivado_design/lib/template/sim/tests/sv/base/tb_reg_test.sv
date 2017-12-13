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


`ifndef _TB_REG_TEST_SV_
`define _TB_REG_TEST_SV_

// ./tb_pkg.svh
`include "tb_pkg.svh"

// ./common/common_reg.svh
`include "common_reg.svh"

// `ifdef EXAMPLE_ENABLE

class tb_reg_cfg;

    int adder0;
    int adder1;

    function new();
        bit val;
        val = $value$plusargs("ADDER0='h%x", adder0);
        val = $value$plusargs("ADDER1='h%x", adder1);
    endfunction : new

endclass : tb_reg_cfg

class tb_reg_test extends tb_test;

    protected tb_reg_cfg m_cfg;

    // Register tb_reg_test into test_top

    `tb_register_test(tb_reg_test)

    function new(string name = "tb_reg_test");
        super.new(name);
        m_cfg = new();
    endfunction : new

    task start();
        super.start();
    endtask : start

    task run();
        string info;
        REG_DATA_t wdata, rdata;
        wdata = 'h5a5aa5a5;
        // ----------------------------------------
        // STEP1: Check version
        // ----------------------------------------
        m_tb_env.m_reg_gen.read(g_reg_demo_version, rdata);
        $sformat(info, {"+------------------------------+\n", 
                        "|    DEMO version: %08x    |\n", 
                        "+------------------------------+"}, rdata);
        `tb_info(m_inst_name, info)
        #10ns;
        // ----------------------------------------
        // STEP2: Test register
        // ----------------------------------------
        m_tb_env.m_reg_gen.write(g_reg_oppos_data, wdata);
        #10ns;
        m_tb_env.m_reg_gen.read(g_reg_oppos_data, rdata);
        if (wdata != (~rdata)) begin
            `tb_error(m_inst_name, "Test oppos register Fail!")
            return;
        end
        #10ns;
        // ----------------------------------------
        // STEP3: Test adder
        // ----------------------------------------
        m_tb_env.m_reg_gen.write(g_reg_adder_cfg_wdata0, m_cfg.adder0);
        m_tb_env.m_reg_gen.write(g_reg_adder_cfg_wdata1, m_cfg.adder1);
        m_tb_env.m_reg_gen.read(g_reg_adder_sum_rdata,   rdata);
        if (rdata != m_cfg.adder0 + m_cfg.adder1) begin
            $sformat(info, "m_cfg=%p, sum should be %d, but actual is %d", 
                            m_cfg, m_cfg.adder0 + m_cfg.adder1, rdata);
            `tb_error(m_inst_name, "Test Fail! Added value was not same to expect!")
            `tb_info(m_inst_name, info)
            return;
        end
        $display("Testcase PASSED!");
    endtask : run

    task stop();
        super.stop();
    endtask : stop

endclass : tb_reg_test

// `endif // EXAMPLE_ENABLE

`endif // _TB_REG_TEST_SV_

