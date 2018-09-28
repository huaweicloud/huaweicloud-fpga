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


`ifndef _TB_LOG_SVH_
`define _TB_LOG_SVH_

// Log level

typedef enum {
    e_LOG_FATAL  = 'd1,
    e_LOG_ERROR  = 'd2,
    e_LOG_WARNING= 'd4,
    e_LOG_INFO   = 'd8,
    e_LOG_DEBUG  = 'd10
} tb_log_lev_t;

`ifdef VIVADO
// Vivaod alwasy report fatal error if using static associate-array
int          g_log_stat[tb_log_lev_t];
`endif

class tb_log;

    //----------------------------------
    // Macro Define
    //----------------------------------

`define tb_debug(ID, MSG) \
  begin \
    tb_log::print_message(MSG, ID, e_LOG_DEBUG, `__FILE__, `__LINE__); \
  end

`define tb_info(ID, MSG) \
  begin \
    tb_log::print_message(MSG, ID, e_LOG_INFO, `__FILE__, `__LINE__); \
  end

`define tb_warning(ID, MSG) \
  begin \
    tb_log::print_message(MSG, ID, e_LOG_WARNING, `__FILE__, `__LINE__); \
  end

`define tb_error(ID, MSG) \
  begin \
    tb_log::print_message(MSG, ID, e_LOG_ERROR, `__FILE__, `__LINE__); \
  end

`define tb_fatal(ID, MSG) \
  begin \
    tb_log::print_message(MSG, ID, e_LOG_FATAL, `__FILE__, `__LINE__); \
  end

    //----------------------------------
    // Varible declaration
    //----------------------------------

    protected static tb_log_lev_t m_level;
    protected static int          m_maxerr;
`ifndef VIVADO
    protected static int          m_stat[tb_log_lev_t];
`endif

    //----------------------------------
    // Task and function declaration
    //----------------------------------
    
    extern function new();

    extern static function void set_log_level(input tb_log_lev_t level = e_LOG_INFO);

    extern static function tb_log_lev_t get_log_level();

    extern static function int get_log_num(input tb_log_lev_t level = e_LOG_INFO);

    extern static function void print_message(input string       message = "", 
                                              input string       name    = "",
                                              input tb_log_lev_t level   = e_LOG_INFO, 
                                              input string       filename= "",
                                              input int          linenum = 'd0);

endclass : tb_log

//------------------------------------------------------------------------------
// IMPLEMENTATION
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// CLASS- tb_log
//
//------------------------------------------------------------------------------

function tb_log::new();
    string log_level;
    if (!$value$plusargs("LOG_LEVEL=%s", log_level)) begin
        m_level  = e_LOG_INFO;
    end else begin
        case (log_level)
            "FATAL"  : m_level = e_LOG_FATAL;
            "ERROR"  : m_level = e_LOG_ERROR;
            "WARNING": m_level = e_LOG_WARNING;
            "INFO"   : m_level = e_LOG_INFO;
            "DEBUG"  : m_level = e_LOG_DEBUG;
            default  : m_level = e_LOG_INFO;
        endcase
    end
    if (!$value$plusargs("MAX_ERRORS=%s", m_maxerr)) begin
        m_maxerr = 'd0;
    end
endfunction : new

function void tb_log::set_log_level(input tb_log_lev_t level = e_LOG_INFO);
    m_level = level;
endfunction : set_log_level

function tb_log_lev_t tb_log::get_log_level();
    get_log_level = m_level;
endfunction : get_log_level

function int tb_log::get_log_num(input tb_log_lev_t level = e_LOG_INFO);
`ifndef VIVADO
    if (!m_stat.exists(level)) begin
        get_log_num = 'd0;
    end else begin
        get_log_num = m_stat[level];
    end
`else
    if (!g_log_stat.exists(level)) begin
        get_log_num = 'd0;
    end else begin
        get_log_num = g_log_stat[level];
    end
`endif
endfunction : get_log_num

function void tb_log::print_message(input string       message = "", 
                                    input string       name    = "",
                                    input tb_log_lev_t level   = e_LOG_INFO,
                                    input string       filename= "",
                                    input int          linenum = 'd0);
    string info;
    time   curtime = $time;
    // If log level is higher than config log level, do not print anything
    if (level > m_level) return;
    case (level)
        e_LOG_FATAL  : $sformat(info, {"Log [TB_FATAL]   on ", name, 
                                       " at %20t from %s, line %1d :\n", message}, curtime, filename, linenum);
        e_LOG_ERROR  : $sformat(info, {"Log [TB_ERROR]   on ", name,
                                       " at %20t from %s, line %1d :\n", message}, curtime, filename, linenum);
        e_LOG_WARNING: $sformat(info, {"Log [TB_WARNING] on ", name,
                                       " at %20t from %s, line %1d :\n", message}, curtime, filename, linenum);
        e_LOG_INFO   : $sformat(info, {"Log [TB_INFO]    on ", name,
                                       " at %20t from %s, line %1d :\n", message}, curtime, filename, linenum);
        default      : $sformat(info, {"Log [TB_DEFAULT] on ", name, 
                                       " at %20t from %s, line %1d :\n", message}, curtime, filename, linenum); 
    endcase
    $display(info);
`ifndef VIVADO
    if (m_stat.exists(level)) begin
        m_stat[level] = 'd1;
    end else begin
        ++m_stat[level];
    end
`else
    if (g_log_stat.exists(level)) begin
        g_log_stat[level] = 'd1;
    end else begin
        ++g_log_stat[level];
    end
`endif
    // If fatal error detected, stop simulation.
    if (e_LOG_FATAL == level) begin
        $display("Simulation halted because of fatal error.");
        $finish;
    end
    // If error numbers are more than m_maxerr, stop simulation.
`ifndef VIVADO
    if (e_LOG_ERROR == level && m_maxerr && m_stat[level] >= m_maxerr) begin
`else
    if (e_LOG_ERROR == level && m_maxerr && g_log_stat[level] >= m_maxerr) begin
`endif
        $display("Simulation halted because of too many errors.");
        $finish;
    end
endfunction : print_message

`endif // _TB_LOG_SVH_

