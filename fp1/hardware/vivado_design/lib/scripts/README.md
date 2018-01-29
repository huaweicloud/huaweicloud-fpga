# Directory Structure

[切换到中文版](./README_CN.md)

This folder stores encrypted KeyFile and FAC services build scripts not concerned by users. The structure of this folder is as follows:
* [lib/scripts](#lib/scripts_dir)/
  - build_facs.tcl
  - fpga_design_run.sh
  - init_ip.sh
  - init_ip.tcl
  - keyfile_ver.txt
  - keyfile_vhd.txt
  - README.md

# Directory Descriptions
* build_facs.tcl  
  This file bears `FAC service building code` of the FPGA.
* fpga_design_run.sh  
  This file bears `project execution code` of the fpga_design project.
* init_ip.sh  
  This file bears `main script code for generating IPs`. It works with i**nit_ip.tcl** to generate IPs.
* init_ip.tcl  
  This file bears `sub-script code for generating IP addresses`. It works with **init_ip.sh** to generate IPs.
* keyfile_ver.txt  
  This file bears encryption methods and is used for encrypting  `Verilog HDL language` code.
* keyfile_vhd.txt  
  This file carries encryption methods and is used for encrypting `VHDL language` code.
* README.md  
  This document describes other documents.



​			

