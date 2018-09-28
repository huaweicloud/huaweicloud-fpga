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


`ifndef _CONFIG_OPT_SVH_
`define _CONFIG_OPT_SVH_

// ./common/tb_log.svh
`include "tb_log.svh"

class config_opt #(int WIDTH = 'd32);

    //----------------------------------
    // Macro Define
    //----------------------------------

    // Get config string

    `define tc_config_opt_get_string(NAME, ARG, DFLT) \
      begin \
          automatic bit val = $value$plusargs({`"NAME`", "=%s"}, ARG); \
          if (!val) ARG = DFLT; \
      end

    // Get config value

    `define tc_config_opt_get_bits(NAME, ARG, DFLT) \
      begin \
          automatic string dflt_str; \
          automatic string get_str ; \
          $sformat(dflt_str, "%d", DFLT); \
          `tc_config_opt_get_string(NAME, get_str, dflt_str) \
          if (get_str == "") begin \
              ARG = DFLT; \
          end else begin \
              ARG = config_opt#($bits(ARG))::string2bits(get_str); \
          end \
      end

    //----------------------------------
    // User Type Define
    //----------------------------------

    typedef bit [WIDTH - 'd1 : 'd0] DATA_t;

    typedef enum {
        e_STR_DEC = 'd0,
        e_STR_HEX,
        e_STR_OCT,
        e_STR_BIN,
        e_STR_NULL
    } str_radix_t;

    //----------------------------------
    // Varible declaration
    //----------------------------------

    //----------------------------------
    // Task and function declaration
    //----------------------------------

    // Check whether strin is numeral

    extern static function bit numeral_check(input string strin);

    extern static function DATA_t string2bits(input string strin, 
                                              input DATA_t dflt = 'd0);
    
    // Please do not use this method if using vivado simulator, using macro "tc_config_opt_get_string" instead

    extern static function string get_string(input string name,
                                             input string dflt = "");
    
    // Please do not use this method if using vivado simulator, using macro "tc_config_opt_get_int" instead

    extern static function int get_int(input string name, 
                                       input int    dflt = 'd0);

    // Please do not use this method if using vivado simulator, using macro "tc_config_opt_get_int" instead

    extern static function DATA_t get_bits(input string name, 
                                           input DATA_t dflt = 'd0);

endclass : config_opt

//------------------------------------------------------------------------------
// IMPLEMENTATION
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// CLASS- config_opt
//
//------------------------------------------------------------------------------

function bit config_opt::numeral_check(input string strin);
    int         strlen = strin.len();
    str_radix_t radix  = e_STR_DEC;
    if (strlen < 'd1) begin
        return 'd0;
    end else begin
        int start_idx  = 'd0;
        numeral_check  = 'd1;
        // Trimleft
        while (strin.getc(start_idx) == " ") begin
            if (++start_idx >= strlen) begin
                return 'd0;
            end
        end
        if (strin.getc(start_idx) == "'") begin
            bit [7 : 0] radix_str = strlen > 2 ? strin.getc(1) : " ";
            case (radix_str)
                "h" : radix = e_STR_HEX;
                "d" : radix = e_STR_DEC;
                "o" : radix = e_STR_OCT;
                "b" : radix = e_STR_BIN;
                default : return 'd0;
            endcase
            start_idx = 'd2;
        end else if (strin.getc(start_idx) < "0" || strin.getc(start_idx) > "9") begin
            return 'd0;
        end
        for (int idx = start_idx; idx < strlen; idx++) begin
            bit [7 : 0] curchar = strin.getc(idx);
            case (radix)
                e_STR_DEC : begin
                    if (curchar < "0" || curchar > "9") begin
                        return 'd0;
                    end
                end
                e_STR_HEX : begin
                    if (curchar < "0" || (curchar > "9" && curchar < "A") || 
                       (curchar > "F" && curchar < "a") || curchar > "f") begin
                        return 'd0;
                    end
                end
                e_STR_OCT : begin
                    if (curchar < "0" || curchar > "7") begin
                        return 'd0;
                    end
                end
                e_STR_BIN : begin
                    if (curchar != "0" && curchar != "1") begin
                        return 'd0;
                    end
                end
                default : return 'd0;
            endcase
        end
    end
endfunction : numeral_check

function config_opt::DATA_t config_opt::string2bits(input string strin,
                                                    input DATA_t dflt = 'd0);
    int         strlen = strin.len();
    str_radix_t radix  = e_STR_DEC;
    if (strlen < 'd1) begin
        return dflt;
    end else begin
        int start_idx  = 'd0;
        string2bits    = 'd0;
        // Trimleft
        while (strin.getc(start_idx) == " ") begin
            if (++start_idx >= strlen) begin
                return dflt;
            end
        end
        if (strin.getc(start_idx) == "'") begin
            bit [7 : 0] radix_str = strlen > 2 ? strin.getc(1) : " ";
            case (radix_str)
                "h" : radix = e_STR_HEX;
                "d" : radix = e_STR_DEC;
                "o" : radix = e_STR_OCT;
                "b" : radix = e_STR_BIN;
                default : begin
                    `tb_warning("config_opt", "Unknow radix!")
                    return dflt;
                end
            endcase
            start_idx += 'd2;
        end else if (strin.getc(start_idx) < "0" || strin.getc(start_idx) > "9") begin
            `tb_warning("config_opt", "Unknow char!")
        end
        for (int idx = start_idx; idx < strlen; idx++) begin
            bit [7 : 0] curchar = strin.getc(idx);
            case (radix)
                e_STR_DEC : begin
                    if (curchar >= "0" && curchar <= "9") begin
                        string2bits = (string2bits * 10) + (curchar - 'h30);
                    end else begin
                        `tb_warning("config_opt", "Unknow char for DEC radix, only 0~9 can be recognize!")
                        return dflt;
                    end
                end
                e_STR_HEX : begin
                    if (curchar >= "0" && curchar <= "9") begin
                        string2bits = (string2bits * 16) + (curchar - 'h30);
                    end else if (curchar >= "A" && curchar <= "F") begin
                        string2bits = (string2bits * 16) + (curchar - 'h41 + 'd10);
                    end else if (curchar >= "a" && curchar <= "f") begin
                        string2bits = (string2bits * 16) + (curchar - 'h61 + 'd10);
                    end else begin
                        `tb_warning("config_opt", "Unknow char for HEX radix, only 0~f can be recognize!")
                        return dflt;
                    end
                end
                e_STR_OCT : begin
                    if (curchar >= "0" && curchar <= "7") begin
                        string2bits = (string2bits * 8) + (curchar - 'h30);
                    end else begin
                        `tb_warning("config_opt", "Unknow char for OCT radix, only 0~7 can be recognize!")
                        return dflt;
                    end
                end
                e_STR_BIN : begin
                    if (curchar == "0" || curchar == "1") begin
                        string2bits = (string2bits * 2) + (curchar - 'h30);
                    end else begin
                        `tb_warning("config_opt", "Unknow char for BIN radix, only 0 and 1 can be recognize!")
                        return dflt;
                    end
                end
                default : return dflt;
            endcase
        end
    end
endfunction : string2bits

function string config_opt::get_string(input string name, 
                                       input string dflt = "");
    bit val = $value$plusargs({name, "=%s"}, get_string);
    if (!val) get_string = dflt;
endfunction : get_string

function int config_opt::get_int(input string name, 
                                 input int    dflt = 'd0);
    string dflt_str;
    string get_str ;
    $sformat(dflt_str, "%d", dflt);
    get_str = get_string(name, dflt_str);
    if (get_str == "") return dflt;
    get_int = string2bits(get_str);
endfunction : get_int

function config_opt::DATA_t config_opt::get_bits(input string name, 
                                                 input DATA_t dflt = 'd0);
    string dflt_str;
    string get_str ;
    $sformat(dflt_str, "%d", dflt);
    get_str = get_string(name, dflt_str);
    if (get_str == "") return dflt;
    get_bits = string2bits(get_str);
endfunction : get_bits

`endif // _CONFIG_OPT_SVH_

