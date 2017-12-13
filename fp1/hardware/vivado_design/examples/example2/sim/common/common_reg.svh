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


`ifndef _COMMON_REG_SVH_
`define _COMMON_REG_SVH_

// ./tb_pkg.svh
`include "tb_pkg.svh"

`define tb_ddr_reg_define(ID, ADDR) \
    const REG_ADDR_t g_reg_ddr``ID``_base  = ADDR; \
    const REG_ADDR_t g_reg_ddr``ID``_cmd   = g_reg_ddr``ID``_base + 'h0; \
    const REG_ADDR_t g_reg_ddr``ID``_addr  = g_reg_ddr``ID``_base + 'h4; \
    const REG_ADDR_t g_reg_ddr``ID``_wdata = g_reg_ddr``ID``_base + 'h8; \
    const REG_ADDR_t g_reg_ddr``ID``_rdata = g_reg_ddr``ID``_base + 'hc;

// Write data to ddr
`define tb_ddr_data_write(ID, ADDR, DATA) \
    begin \
        REG_DATA_t cmd; \
        int        timeout; \
        m_tb_env.m_reg_gen.write(g_reg_ddr``ID``_wdata, DATA); \
        m_tb_env.m_reg_gen.write(g_reg_ddr``ID``_addr,  ADDR); \
        m_tb_env.m_reg_gen.write(g_reg_ddr``ID``_cmd  ,  'd3); \
        do begin \
            m_tb_env.m_reg_gen.read(g_reg_ddr``ID``_cmd, cmd); \
            #10ns; \
            if (timeout++ >= 'd2000) begin \
                `tb_error(m_inst_name, {"Write DDR", `"ID`", "timeout!"}) \
                break; \
            end \
        end \
        while (cmd != 'd0); \
    end

// Read data to ddr
`define tb_ddr_data_read(ID, ADDR, DATA) \
    begin \
        REG_DATA_t cmd; \
        int        timeout; \
        m_tb_env.m_reg_gen.write(g_reg_ddr``ID``_addr,  ADDR); \
        m_tb_env.m_reg_gen.write(g_reg_ddr``ID``_cmd  ,  'd2); \
        do begin \
            m_tb_env.m_reg_gen.read(g_reg_ddr``ID``_cmd, cmd); \
            #10ns; \
            if (timeout++ >= 'd2000) begin \
                `tb_error(m_inst_name, {"Read DDR", `"ID`", "timeout!"}) \
                break; \
            end \
        end \
        while (cmd != 'd0); \
        m_tb_env.m_reg_gen.read(g_reg_ddr``ID``_rdata, DATA); \
    end

typedef bit [`AXI4L_DATA_WIDTH - 1 : 0] REG_DATA_t;
typedef bit [`AXI4L_ADDR_WIDTH - 1 : 0] REG_ADDR_t;

// Demo Regs
const REG_ADDR_t g_reg_ver_time        = ('h0 << 2);
const REG_ADDR_t g_reg_ver_type        = ('h1 << 2);
const REG_ADDR_t g_reg_adder_cfg_wdata0= ('h2 << 2);
const REG_ADDR_t g_reg_adder_cfg_wdata1= ('h3 << 2);
const REG_ADDR_t g_reg_adder_sum_rdata = ('h4 << 2);
const REG_ADDR_t g_reg_oppos_data      = ('h5 << 2);
const REG_ADDR_t g_reg_vled_data       = ('h6 << 2);
const REG_ADDR_t g_reg_oppos_addr      = ('h7 << 2);

// DDRA Regs
`tb_ddr_reg_define(a, ('h2000 << 2))

// DDRB Regs
`tb_ddr_reg_define(b, ('h3000 << 2))

// DDRD Regs
`tb_ddr_reg_define(d, ('h5000 << 2))

`endif // _COMMON_REG_SVH_

