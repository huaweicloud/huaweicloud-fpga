//
//------------------------------------------------------------------------------
//     Copyright (c) 2017 Huawei Technologies Co., Ltd. All Rights Reserved.
//
//     This program is free software; you can redistribute it and/or modify
//     it under the terms of the Huawei Software License (the "License").
//     A copy of the License is located in the "License" file accompanying 
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

    task reset();
        // Force DUT ddr initial_done to accelerate simulation
        `tb_ddr_dut_disable_init(a)
        `tb_ddr_dut_disable_init(b)
        `tb_ddr_dut_disable_init(d)
        super.reset();
    endtask : reset

    task run();
        bit    check;
        string info, err_info;
        REG_ADDR_t addr;
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
            $sformat(info, "%s\n\nDetail info: Type of Example3 should be 0x%x but get 0x%x!\n",
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

        $sformat(info, {"+-------------------------------+\n", 
                        "|    Test Register: %s        |\n", 
                        "+-------------------------------+"}, check ? "PASS" : "FAIL");
        if (!check) begin
            $sformat(err_info, "\n\nDetail info: Write 0x%x but read 0x%x which should be 0x%x!\n",
                     wdata, rdata, ~wdata);
        end
        oppos = {~g_reg_oppos_data[17 : 2], ~g_reg_oppos_data[17 : 2]};
        check &= (oppos == rdata);
        begin
            string info_tmp;
            $sformat(info_tmp, {"%s\n|    Addr Test Register: %s   |\n", 
                                "+-------------------------------+"}, info, check ? "PASS" : "FAIL");
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
    `ifdef CHECK_DDR_FMMU
        #25us;

        // ----------------------------------------
        // STEP4: Test DDRA
        // ----------------------------------------
        `tb_info(m_inst_name, {"\n----------------------------------------\n", 
                               " STEP4: Checking DDRA\n", 
                               "----------------------------------------\n"})
        // Check DDR rank0
        addr  = $urandom_range('h3fff00f, 'h3fff00e);
        wdata = $urandom_range('h7fffffff, 'd0);
        // Wirte DDR
        `tb_ddr_data_write(a, (addr << 2), wdata)
        #10ns;
        `tb_ddr_data_read(a, (addr << 2), rdata)
        check = (rdata == wdata);
        // Check DDR rank1
        addr  = $urandom_range('h3ffffff, 'h3fffffe);
        wdata = $urandom_range('h7fffffff, 'd0);
        // Wirte DDR
        `tb_ddr_data_write(a, (addr << 2), wdata)
        #10ns;
        `tb_ddr_data_read(a, (addr << 2), rdata)
        check &= (rdata == wdata);
 
        $sformat(info, {"+-------------------------------+\n", 
                        "|    Test DDRA    : %s        |\n", 
                        "+-------------------------------+"}, check ? "PASS" : "FAIL");
        if (!check) begin
            $sformat(info, "%s\n\nDetail info: Read data from ddra was not same to write! Rdata should be 0x%x, but actual is 0x%x!\n", 
                     info, wdata, rdata);
            `tb_error(m_inst_name, info)
            #10ns;
            return;
        end else begin
            `tb_info(m_inst_name, info)
        end
        #10ns;
        // ----------------------------------------
        // STEP4: Test DDRB
        // ----------------------------------------
        `tb_info(m_inst_name, {"\n----------------------------------------\n", 
                               " STEP5: Checking DDRB\n", 
                               "----------------------------------------\n"})
        addr  = $urandom_range('h3fff00f, 'h3fff00e);
        wdata = $urandom_range('h7fffffff, 'd0);
        // Wirte DDR
        `tb_ddr_data_write(b, (addr << 2), wdata)
        #10ns;
        `tb_ddr_data_read(b, (addr << 2), rdata)
        check = (rdata == wdata);
        // Check DDR rank1
        addr  = $urandom_range('h3ffffff, 'h3fffffe);
        wdata = $urandom_range('h7fffffff, 'd0);
        // Wirte DDR
        `tb_ddr_data_write(b, (addr << 2), wdata)
        #10ns;
        `tb_ddr_data_read(b, (addr << 2), rdata)
        check &= (rdata == wdata);
 
        $sformat(info, {"+-------------------------------+\n", 
                        "|    Test DDRB    : %s        |\n", 
                        "+-------------------------------+"}, check ? "PASS" : "FAIL");
        if (!check) begin
            $sformat(info, "%s\n\nDetail info: Read data from ddrb was not same to write! Rdata should be 0x%x, but actual is 0x%x!\n", 
                     info, wdata, rdata);
            `tb_error(m_inst_name, info)
            #10ns;
            return;
        end else begin
            `tb_info(m_inst_name, info)
        end
        #10ns;
        // ----------------------------------------
        // STEP4: Test DDRD
        // ----------------------------------------
        `tb_info(m_inst_name, {"\n----------------------------------------\n", 
                               " STEP6: Checking DDRD\n", 
                               "----------------------------------------\n"})
        addr  = $urandom_range('h3fff00f, 'h3fff00e);
        wdata = $urandom_range('h7fffffff, 'd0);
        // Wirte DDR
        `tb_ddr_data_write(d, (addr << 2), wdata)
        #10ns;
        `tb_ddr_data_read(d, (addr << 2), rdata)
        check = (rdata == wdata);
        // Check DDR rank1
        addr  = $urandom_range('h3ffffff, 'h3fffffe);
        wdata = $urandom_range('h7fffffff, 'd0);
        // Wirte DDR
        `tb_ddr_data_write(d, (addr << 2), wdata)
        #10ns;
        `tb_ddr_data_read(d, (addr << 2), rdata)
        check &= (rdata == wdata);
 
        $sformat(info, {"+-------------------------------+\n", 
                        "|    Test DDRD    : %s        |\n", 
                        "+-------------------------------+"}, check ? "PASS" : "FAIL");
        if (!check) begin
            $sformat(info, "%s\n\nDetail info: Read data from ddrd was not same to write! Rdata should be 0x%x, but actual is 0x%x!\n", 
                     info, wdata, rdata);
            `tb_error(m_inst_name, info)
            #10ns;
            return;
        end else begin
            `tb_info(m_inst_name, info)
        end
    `endif // CHECK_DDR_FMMU
        $display("\nTestcase PASSED!\n");
    endtask : run

    task stop();
        super.stop();
        #5us;
    endtask : stop

endclass : tb_reg_test

// `endif // EXAMPLE_ENABLE

`endif // _TB_REG_TEST_SV_

