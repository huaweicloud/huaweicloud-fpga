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


`ifndef _TB_C_TEST_SV_
`define _TB_C_TEST_SV_

// ./tb_pkg.svh
`include "tb_pkg.svh"

// ./common/common_reg.svh
`include "common_reg.svh"

import "DPI-C" context task tb_c_test_main(output int unsigned exit_code);

class tb_c_test extends tb_test;

    // Register tb_reg_test into test_top

    `tb_register_test(tb_c_test)

    function new(string name = "tb_c_test");
        super.new(name);
    endfunction : new

    task start();
        super.start();
    endtask : start

    task write_req();
        forever begin
            longint unsigned addr;
            int unsigned     data;
            g_tb_reg_wr_req.get(addr);
            if (g_tb_regs.exists(addr)) begin
                data = g_tb_regs[addr];
            end else begin
                data = 'd0;
            end
            m_tb_env.m_reg_gen.write(addr, data);
        end
    endtask : write_req

    task read_req();
        forever begin
            longint unsigned addr;
            int unsigned     data;
            g_tb_reg_rd_req.get(addr);
            m_tb_env.m_reg_gen.read(addr, data);
            g_tb_regs[addr] = data;
            g_tb_reg_rd_rsp.put();
        end
    endtask : read_req

    task run();
        int unsigned exit_code;
        fork
            tb_c_test_main(exit_code);
            write_req();
            read_req();
        join_any
        disable fork;
        if (exit_code) begin
            $display("\nTestcase FAILED!\n");
        end else begin
            $display("\nTestcase PASSED!\n");
        end
    endtask : run

    task stop();
        super.stop();
    endtask : stop

endclass : tb_c_test

`endif // _TB_C_TEST_SV_
