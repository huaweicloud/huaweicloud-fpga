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

// ./common/tb_reg_cfg.svh
`include "tb_reg_cfg.svh"

// `ifdef EXAMPLE_ENABLE

class tb_reg_test extends tb_test;

    protected tb_reg_cfg m_cfg;

    // Register tb_reg_test into test_top

    `tb_register_test(tb_reg_test)

    function new(string name = "tb_reg_test");
        super.new(name);
    endfunction : new

    task start();
        super.start();
    endtask : start

    task run();
        bit    check;
        string info, err_info;
        REG_DATA_t ver_time, ver_type;
        REG_DATA_t oppos;
        REG_DATA_t wdata, rdata;
        wdata = 'h5a5aa5a5;
        m_cfg = new();
        // ----------------------------------------
        // STEP1: Check version
        // ----------------------------------------
        `tb_info(m_inst_name, {"\n----------------------------------------\n", 
                               " STEP1: Checking DUV Infomation\n", 
                               "----------------------------------------\n"})
        m_tb_env.m_reg_gen.read(g_reg_ver_time, ver_time);
        m_tb_env.m_reg_gen.read(g_reg_ver_type, ver_type);
        $sformat(info, {"+-------------------------------+\n", 
                        "|    DEMO version : %08x    |\n", 
                        "|    DEMO type    : %08x    |\n", 
                        "+-------------------------------+"}, ver_time, ver_type);
        `tb_info(m_inst_name, info)
        check = (ver_type == m_cfg.vertype);
        $sformat(info, {"+-------------------------------+\n", 
                        "|    Demo Check   : %s        |\n", 
                        "+-------------------------------+"}, check ? "PASS" : "FAIL");
        if (!check) begin
            $sformat(info, "%s\n\nDetail info: Type of Example1 should be 0x%x but get 0x%x!\n",
                     info, m_cfg.vertype, ver_type);
            `tb_error(m_inst_name, info)
            return;
        end else begin
            `tb_info(m_inst_name, info)
        end
        #10ns;

        // ----------------------------------------
        // STEP2: Test register
        // ----------------------------------------
        `tb_info(m_inst_name, {"\n----------------------------------------\n", 
                               " STEP2: Checking DUV Test Register\n", 
                               "----------------------------------------\n"})
        m_tb_env.m_reg_gen.write(g_reg_oppos_data, wdata);
        #10ns;
        m_tb_env.m_reg_gen.read(g_reg_oppos_data, oppos);
        check = (wdata == (~oppos));

        m_tb_env.m_reg_gen.read(g_reg_oppos_addr, rdata);
        $sformat(info, {"+------------------------------------+\n", 
                        "|    Test Register     : %s        |\n", 
                        "+------------------------------------+"}, check ? "PASS" : "FAIL");
        if (!check) begin
            $sformat(err_info, "\n\nDetail info: Write 0x%x but read 0x%x which should be 0x%x!\n",
                     wdata, rdata, ~wdata);
        end
        oppos = {~g_reg_oppos_data[17 : 2], ~g_reg_oppos_data[17 : 2]};
        check &= (oppos == rdata);
        begin
            string info_tmp;
            $sformat(info_tmp, {"%s\n|    Addr Test Register: %s        |\n", 
                                "+------------------------------------+"}, info, check ? "PASS" : "FAIL");
            info = info_tmp;
        end
        if (!check) begin
            info = {info, err_info};
            $sformat(info, "%s\n\nDetail info: Write addr 0x%x but read 0x%x which should be 0x%x!\n",
                     info, g_reg_oppos_data, rdata, oppos);
            `tb_error(m_inst_name, info)
            return;
        end else begin
            `tb_info(m_inst_name, info)
        end
        #10ns;

        // ----------------------------------------
        // STEP3: Test adder
        // ----------------------------------------
        `tb_info(m_inst_name, {"\n----------------------------------------\n", 
                               " STEP3: Checking DUV Adder\n", 
                               "----------------------------------------\n"})
        m_tb_env.m_reg_gen.write(g_reg_adder_cfg_wdata0, m_cfg.adder0);
        m_tb_env.m_reg_gen.write(g_reg_adder_cfg_wdata1, m_cfg.adder1);
        m_tb_env.m_reg_gen.read(g_reg_adder_sum_rdata,   rdata);
        check = (rdata == m_cfg.adder0 + m_cfg.adder1);
 
        $sformat(info, {"+-------------------------------+\n", 
                        "|    Test Adder   : %s        |\n", 
                        "+-------------------------------+"}, check ? "PASS" : "FAIL");
        if (!check) begin
            $sformat(info, "%s\n\nDetail info: Added value was not same to expect! Sum should be %d, but actual is %d!\n", 
                     info, m_cfg.adder0 + m_cfg.adder1, rdata);
            `tb_error(m_inst_name, info)
            return;
        end else begin
            `tb_info(m_inst_name, info)
        end
        $display("\nTestcase PASSED!\n");
    endtask : run

    task stop();
        super.stop();
    endtask : stop

endclass : tb_reg_test

// `endif // EXAMPLE_ENABLE

`endif // _TB_REG_TEST_SV_

