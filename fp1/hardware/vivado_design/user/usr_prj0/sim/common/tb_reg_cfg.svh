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


`ifndef _TB_REG_CFG_SV_
`define _TB_REG_CFG_SV_

// ./tb_pkg.svh
`include "tb_pkg.svh"

class tb_reg_cfg;

    int adder0;
    int adder1;

    int unsigned verdate;
    int unsigned vertype;

    function new();
    `ifndef VIVADO
        adder0  = config_opt#(32)::get_bits("ADDER0",      'd0       );
        adder1  = config_opt#(32)::get_bits("ADDER1",      'd0       );
        verdate = config_opt#(32)::get_bits("DUT_VER_DATE",'h20170823);
        vertype = config_opt#(32)::get_bits("DUT_VER_TYPE",'h00d10001);
    `else
        `tc_config_opt_get_bits(ADDER0,       adder0, 'd0       )
        `tc_config_opt_get_bits(ADDER1,       adder1, 'd0       )
        `tc_config_opt_get_bits(DUT_VER_DATE, verdate,'h20170823)
        `tc_config_opt_get_bits(DUT_VER_TYPE, vertype,'h00d10001)
    `endif
    endfunction : new

endclass : tb_reg_cfg

`endif // _TB_REG_CFG_SV_
