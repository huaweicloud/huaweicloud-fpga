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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// Vivado does not support svGetScopeFromName
#ifndef VIVADO
    #include "svdpi.h"
#endif

#include "dpi_common.h"

#include "test_common.h"

void tb_c_test_main(uint32_t *exit_code) {

    // Vivado does not support svGetScopeFromName
#ifndef VIVADO
    svScope scope;
#endif

    uint32_t check;
    // uint32_t rdata;
    uint32_t ver_time, ver_type;

    uint32_t ver_type_exp;

    char m_inst_name[20] = "tb_c_test_main";
    char info[200];

    // Vivado does not support svGetScopeFromName
#ifndef VIVADO
    scope = svGetScopeFromName("tb");
    svSetScope(scope);
#endif

    // Get param does not works if using vivado as simulator
    cfg_get_int("DUT_VER_TYPE", (int *)&ver_type_exp, 0x00d10008);

    // ----------------------------------------
    // STEP1: Check version
    // ----------------------------------------
    tb_report(LOG_INFO, m_inst_name, "\n----------------------------------------\n STEP1: Checking DUV Infomation\n----------------------------------------\n");
    ul_reg_read(g_reg_ver_time, &ver_time);
    ul_reg_read(g_reg_ver_type, &ver_type);
    sprintf(info, "+-------------------------------+\n|    DEMO version : %08x    |\n|    DEMO type    : %08x    |\n+-------------------------------+", ver_time, ver_type);
    tb_report(LOG_INFO, m_inst_name, info);
    check = (ver_type == ver_type_exp);
    sprintf(info, "+-------------------------------+\n|    Demo Check   : %s        |\n+-------------------------------+", check ? "PASS" : "FAIL");
    if (!check) {
        sprintf(info, "%s\n\nDetail info: Type of Example1 should be 0x%x but get 0x%x!\n",
                      info, ver_type_exp, ver_type);
        tb_report(LOG_ERROR, m_inst_name, info);
        *exit_code = 1;
        return;
    } else {
        tb_report(LOG_INFO, m_inst_name, info);
    }

    *exit_code = 0;
}
