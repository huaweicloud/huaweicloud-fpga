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

#ifndef _DPI_COMMON_H_
#define _DPI_COMMON_H_

#include <stdarg.h>

// Log level

typedef enum {
    LOG_FATAL  = 1,
    LOG_ERROR  = 2,
    LOG_WARNING= 4,
    LOG_INFO   = 8,
    LOG_DEBUG  = 10
} tb_rep_lev_t;

extern void tb_report(int  level,
                      char *id,
                      char *info);

extern void tb_delay(uint32_t x);

extern void ul_reg_read(uint64_t addr, uint32_t *data);
extern void ul_reg_write(uint64_t addr, uint32_t  data);

extern void cfg_get_string(char *name,
                           char **value,
                           char *dflt);

extern void cfg_get_int(char *name,
                        int  *value,
                        int  dflt);

#endif // _DPI_COMMON_H_
