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

`include "tb_pkg.svh"

class tb_reg_test extends tb_test;

    `tb_register_test(tb_reg_test)

    function new(string name = "tb_reg_test");
        super.new(name);
    endfunction : new

    task start();
        $display("start pahse start...................................");
        super.start();
        $display("start pahse end...................................");
    endtask : start

    task run();
        bit [`AXI4L_DATA_WIDTH : 0] wdata, rdata;
        wdata = 'h5a5aa5a5;
        rdata = 'ha5a55a5a;
        $display("run pahse start...................................");
        m_tb_env.m_reg_gen.write('h000c, wdata);
        #10ns;
        m_tb_env.m_reg_gen.read('h000c, rdata);
        if (wdata != rdata) begin
            $display("Test Fail!");
        end
        #10ns;
    endtask : run

    task stop();
        super.stop();
    endtask : stop

endclass : tb_reg_test

`endif // _TB_REG_TEST_SV_

