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


`ifndef _CLOCK_GEN_SV_
`define _CLOCK_GEN_SV_

`timescale 1ns/1ps

//------------------------------------------------------------------------------
//
// MODULE: clock_gen
//
// The clock_gen module is the clock generation module for testbench. This 
// generator 
// a driver and monitor. It is suitable for AXI4 Proctol(Do not support AXI3).
// No full feature of AXI4 will be supported, such as out of order, narrow
// mode, unalign mode, back to back transport an so on. If more verification
// features are required, please contact us for power pack or AXI4 VIP.
//
//------------------------------------------------------------------------------

module clock_gen #(parameter FREQ       = 100000,  // Frequency(KHz) 
                   parameter PERIOD     = 5,       // Time period(ns)
                   parameter DUTY       = 50,      // Duty cycle(%)
                   parameter OFFSET_MIN = 0,       // Offset Low Range(ns)
                   parameter OFFSET_MAX = 100,     // Offset High Range(ns)
                   parameter DEFAULT_VAL= 0        // Default value of clk(0 - 0, 1 - 1, 2 - z, 3 -x)
                   ) (
                   output logic clk_p,
                   output logic clk_n
                   );

//----------------------------------
// Parameter Define
//----------------------------------

parameter PERIOD_H = (FREQ <= 0) ? (PERIOD * DUTY * 1.0) / 100 : ((1000000.0 / FREQ) * DUTY) / 100;
parameter PERIOD_L = (FREQ <= 0) ? (PERIOD * 1.0) - PERIOD_H   : (1000000.0 / FREQ) - PERIOD_H;

//----------------------------------
// Clock generation process declaration and implementation
//----------------------------------

assign clk_n = ~clk_p;

// Clock generate process
initial begin
    static real period_h;
    static real period_l;
    // Generate random offset
    static int  offset = $urandom_range(OFFSET_MAX, OFFSET_MIN);
    // Assert the clk at specfic value
    case (DEFAULT_VAL)
        0, 1:    clk_p = DEFAULT_VAL;
        2:       clk_p = 'hz;
        default: clk_p = 'hx;
    endcase
    // Delay sometime
    #(offset * 1ns);
    // Caculate time period
    period_h = PERIOD_H * 1ns;
    period_l = PERIOD_L * 1ns;

    // If period is zero, set to default value(10ns).
    if (period_h <= 0) period_h = 5.0;
    if (period_l <= 0) period_l = 5.0;
    // Generate clock
    forever begin
        clk_p = 'd1;
        #(period_h * 1ns);
        clk_p = 'd0;
        #(period_l * 1ns);
    end
end

endmodule

`endif // _CLOCK_GEN_SV_

