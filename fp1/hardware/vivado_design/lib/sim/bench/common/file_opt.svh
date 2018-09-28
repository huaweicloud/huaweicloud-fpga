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


`ifndef _FILE_OPT_SVH_
`define _FILE_OPT_SVH_

// ./common/tb_log.svh
`include "tb_log.svh"

class file_opt;

    //----------------------------------
    // Macro declaration
    //----------------------------------
    
    // Open File Macro(Used for vivado)

    `define tb_file_opt_open_file(FILE, OPT, RSLT) \
      begin \
          int id = file_opt::get_id(FILE); \
          if (id == 0) begin \
              id = $fopen(FILE, OPT); \
              file_opt::m_id_pool[FILE] = id; \
          end else begin \
              `tb_warning("file_opt", {"File ", FILE, " has been opened! Ignore reopen!"}) \
          end \
          RSLT = id; \
      end

    //----------------------------------
    // Varible declaration
    //----------------------------------

    static int        m_id_pool[string]; // File Id pool, storage existed file id

    //----------------------------------
    // Task and function declaration
    //----------------------------------

    extern static function int get_id(input string file_name = "");

    extern static function int open(input string file_name = "", 
                                    input string file_opts = "r");
    
    extern static task close(input string file_name = "");

    extern static task flush(input string file_name = "");

    extern static function int getc(input string file_name = "");

    extern static function int gets(input string file_name = "", 
                                    ref   string file_str);
    
    extern static function int write(input string file_name = "", 
                                     ref   string file_str);

endclass : file_opt

//------------------------------------------------------------------------------
// IMPLEMENTATION
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// CLASS- file_opt
//
//------------------------------------------------------------------------------

function int file_opt::get_id(input string file_name = "");
    get_id = (file_name != "") ? m_id_pool.exists(file_name) ? m_id_pool[file_name] : 0 : 0;
endfunction : get_id

function int file_opt::open(input string file_name = "", 
                            input string file_opts = "r");
    int id = get_id(file_name);
    if (id == 0) begin
        id = $fopen(file_name, file_opts);
        m_id_pool[file_name] = id;
    end else begin
        `tb_warning("file_opt", {"File ", file_name, " has been opened! Ignore reopen!"})
    end
    open = id;
endfunction : open

task file_opt::close(input string file_name = "");
    int id = get_id(file_name);
    if (id == 0) begin
        `tb_warning("file_opt", {"File ", file_name, " has not been opened! Ignore close!"})
    end else begin
        $fclose(id);
        m_id_pool.delete(file_name);
    end
endtask : close

task file_opt::flush(input string file_name = "");
    int id = get_id(file_name);
    if (id == 0) begin
        `tb_warning("file_opt", {"File ", file_name, " has not been opened! Ignore flush!"})
    end else begin
        $fflush(id);
    end
endtask : flush

function int file_opt::getc(input string file_name = "");
    int id = get_id(file_name);
    if (id == 0) begin
        `tb_warning("file_opt", {"File ", file_name, " has not been opened! Please open first!"})
        getc = 0;
    end else begin
        getc = $fgetc(id);
    end
endfunction : getc

function int file_opt::gets(input string file_name = "", 
                            ref   string file_str);
    int id = get_id(file_name);
    if (id == 0) begin
        `tb_warning("file_opt", {"File ", file_name, " has not been opened! Please open first!"})
        gets = 0;
    end else begin
        gets = $fgets(file_str, id);
    end
endfunction : gets

function int file_opt::write(input string file_name = "", 
                             ref   string file_str);
    int id = get_id(file_name);
    if (id == 0) begin
        `tb_warning("file_opt", {"File ", file_name, " has not been opened! Please open first!"})
        write = 0;
    end else begin
        $fwrite(id, "%s\n", file_str);
        write = 0;
    end
endfunction : write

`endif // _FILE_OPT_SVH_
