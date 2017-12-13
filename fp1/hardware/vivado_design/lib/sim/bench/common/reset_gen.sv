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


`ifndef _RESET_GEN_SV_
`define _RESET_GEN_SV_

`timescale 1ns/1ps

module reset_gen #(parameter TIME       = 100,     // Reset time(ns) 
                   parameter VALUE      = 0,       // Reset value(0 - 0, other - 1)
                   parameter OFFSET_MIN = 0,       // Offset Low Range(ns)
                   parameter OFFSET_MAX = 100,     // Offset High Range(ns)
                   parameter SYNC_RLS   = 0,       // Sync release reset (0 -disable, 1 - enable)
                   parameter DEFAULT_VAL= 0        // Default value of reset(0 - 0, 1 - 1, 2 - z, 3 -x)
                   ) (
                   input  logic clk  ,             // Only userd for sync mode
                   output logic reset,
                   output logic done
                   );

// Reset generate process
initial begin
    // Generate random offset
    static int  offset = $urandom_range(OFFSET_MAX, OFFSET_MIN);
    // Assert the clk at specfic value
    case (DEFAULT_VAL)
        0, 1:    reset = DEFAULT_VAL;
        2:       reset = 'hz;
        default: reset = 'hx;
    endcase
    done = 'd0;
    // Delay sometime
    #(offset * 1ns);
    // Assert reset signal
    reset = (VALUE != 0);
    #(TIME * 1ns);
    // Deassert reset signal
    if (SYNC_RLS) begin
        @ (posedge clk);
        reset <= ~reset;
    end else begin
        reset = ~reset;
    end
    done  = 'd1;
end

endmodule

`endif // _RESET_GEN_SV_

