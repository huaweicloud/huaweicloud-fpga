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

`ifndef _EXT_CPU_MODEL_CB_SVH_
`define _EXT_CPU_MODEL_CB_SVH_

// ./rm/cpu_model_cb.svh
`include "cpu_model_cb.svh"

class ext_cpu_model_cb extends cpu_model_cb;

    //----------------------------------
    // Task and function declaration
    //----------------------------------
    
    extern function new(string name = "ext_cpu_model_cb");

endclass : ext_cpu_model_cb

function ext_cpu_model_cb::new(string name = "ext_cpu_model_cb");
    super.new(name);
endfunction : new

`endif // _EXT_CPU_MODEL_CB_SVH_
