#
#-------------------------------------------------------------------------------
#      Copyright 2017 Huawei Technologies Co., Ltd. All Rights Reserved.
# 
#      This program is free software; you can redistribute it and/or modify
#      it under the terms of the Huawei Software License (the "License").
#      A copy of the License is located in the "LICENSE" file accompanying 
#      this file.
# 
#      This program is distributed in the hope that it will be useful,
#      but WITHOUT ANY WARRANTY; without even the implied warranty of
#      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#      Huawei Software License for more details. 
#-------------------------------------------------------------------------------

set dcp_file            [lindex $argv  0]
set prj_name            [lindex $argv  1]
set dcp_dir             [file dirname $dcp_file]
set shell_dir           [file join ${dcp_dir} "../../../../../../lib/checkpoints"]

puts "INFO: DCP_DIR   |$dcp_file"
puts "INFO: SHELL_DIR |$shell_dir"

#******************************************************************************
#         pr_verify
#******************************************************************************
pr_verify -full_check $dcp_file ${shell_dir}/SH_UL_BB_routed.dcp

#******************************************************************************
#         open_checkpoint
#******************************************************************************
open_checkpoint $dcp_file
    
set_property BITSTREAM.GENERAL.COMPRESS TRUE [current_design ]
set_param bitstream.enablePR 4123
write_bitstream -force -bin_file $dcp_dir/$prj_name
#******************************************************************************
#         rename_bin
#******************************************************************************
if {[file exists ${dcp_dir}/${prj_name}.bin]} {
   file delete ${dcp_dir}/${prj_name}.bin
   file delete ${dcp_dir}/${prj_name}.bit
   file rename ${dcp_dir}/${prj_name}_pblock_u_ul_pr_top_partial.bin ${dcp_dir}/pr_ul_${prj_name}.bin
   file rename ${dcp_dir}/${prj_name}_pblock_u_ul_pr_top_partial.bit ${dcp_dir}/pr_ul_${prj_name}.bit
}

