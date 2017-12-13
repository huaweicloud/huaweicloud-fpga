# 目录结构
本文件夹用于存放加密的keyfile和fpga云服务构建脚本库（用户不关注），其结构如下：
* [lib/scripts](#lib/scripts_dir)/
  - build_facs.tcl
  - fpga_design_run.sh
  - init_ip.sh
  - init_ip.tcl
  - keyfile_ver.txt
  - keyfile_vhd.txt
  - README.md

# 目录说明
* build_facs.tcl  
  该文件承载fpga`云服务构建代码`；
* fpga_design_run.sh  
  该文件承载fpga_design`工程执行代码`；
* init_ip.sh  
  该文件承载`生成IP的主脚本代码`，与init_ip.tcl一起实现生成IP的功能；
* init_ip.tcl  
  该文件承载`生成IP的子脚本代码`，与init_ip.sh一起实现生成IP的功能；
* keyfile_ver.txt  
  该文件承载加密方法，用于`加密verilog HDL语言`的代码；
* keyfile_vhd.txt  
  本文件承载加密方法，用于`加密VHDL语言编写的代码`；
* README.md  
  即本文档，用于介绍其他文档。



			
