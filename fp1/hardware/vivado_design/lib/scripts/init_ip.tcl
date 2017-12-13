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

# Generate ip and examples
set_msg_config -severity INFO -suppress
set_msg_config -severity STATUS -suppress
set_msg_config -severity WARNING -suppress
set_msg_config -string {exportsim} -suppress
set_msg_config -string {IP_Flow} -suppress

# Get tmp_prj dir
set project_dir /tmp/$::env(CURRENT_USER)/$::env(WORK_DIR)/tmp_prj
set export_dir  $::env(LIB_DIR)/ip

# Generate DDR4 phy ip
proc generate_ddr_phy_ip { ip_name project_dir export_dir } {
    create_ip -name ddr4 -vendor xilinx.com -library ip -version 2.2 -module_name $ip_name -dir $export_dir
    set_property -dict [list CONFIG.Phy_Only {Phy_Only_Single} CONFIG.C0.DDR4_TimePeriod {938} CONFIG.C0.DDR4_InputClockPeriod {10005} CONFIG.C0.DDR4_MemoryType {RDIMMs} CONFIG.C0.DDR4_MemoryPart {MTA18ASF2G72PDZ-2G3} CONFIG.C0.DDR4_CLKOUT0_DIVIDE {6} CONFIG.C0.DDR4_DataWidth {72} CONFIG.C0.DDR4_CasLatency {15} CONFIG.C0.CKE_WIDTH {2} CONFIG.C0.CS_WIDTH {2} CONFIG.C0.ODT_WIDTH {2}] [get_ips $ip_name]
    generate_target {instantiation_template} [get_files $export_dir/$ip_name/$ip_name.xci]
    set_property generate_synth_checkpoint false [get_files $export_dir/$ip_name/$ip_name.xci]
    # set_property top $ip_name [get_filesets sim_1]
    generate_target all [get_files $export_dir/$ip_name/$ip_name.xci]
}

# Create project in /tmp
create_project -force tmp_ip $project_dir -part xcvu9p-flgb2104-2-i

# Create ddra ip
generate_ddr_phy_ip rdimma_x8_16GB_2133Mbps $project_dir $export_dir
# Generate ddr sim files
open_example_project -force -dir $project_dir/tmp_ip_ex [get_ips rdimma_x8_16GB_2133Mbps]

# Create ddrb ip
generate_ddr_phy_ip rdimmb_x8_16GB_2133Mbps $project_dir $export_dir

# Create ddrd ip
generate_ddr_phy_ip rdimmd_x8_16GB_2133Mbps $project_dir $export_dir

# Create debug_bridge ip
create_ip -name debug_bridge -vendor xilinx.com -library ip -version 2.0 -module_name debug_bridge_0 -dir $export_dir
set_property -dict [list CONFIG.C_EN_BSCANID_VEC {false}] [get_ips debug_bridge_0]
generate_target {instantiation_template} [get_files $export_dir/debug_bridge_0/debug_bridge_0.xci]
set_property generate_synth_checkpoint false [get_files  $export_dir/debug_bridge_0/debug_bridge_0.xci]
generate_target all [get_files  $export_dir/debug_bridge_0/debug_bridge_0.xci]

# Create ila ip
create_ip -name ila -vendor xilinx.com -library ip -version 6.2 -module_name ila_0 -dir $export_dir
set_property -dict [list CONFIG.C_NUM_OF_PROBES {6} CONFIG.C_INPUT_PIPE_STAGES {4}] [get_ips ila_0]
generate_target {instantiation_template} [get_files $export_dir/ila_0/ila_0.xci]
set_property generate_synth_checkpoint false [get_files  $export_dir/ila_0/ila_0.xci]
generate_target all [get_files  $export_dir/ila_0/ila_0.xci]

exit
