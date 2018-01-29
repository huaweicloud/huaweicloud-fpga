# 目录结构

[Switch to the English version](./README.md)

* [lib/chekpoints](#lib/chekpoints_dir)/  
  - SH_UL_BB_routed.dcp 
  - SH_UL_BB_routed.md5

# 目录说明
* SH_UL_BB_routed.dcp     
  该文件为SHELL静态逻辑实现网表；
* SH_UL_BB_routed.md5 SH_UL_BB_routed.dcp    
  该文件承载的是md5校验值；如果：
  - `md5sum SH_UL_BB_routed.dcp ==  SH_UL_BB_routed.md5，则表示SH_UL_BB_routed.dcp文件正常`；
  - `md5sum SH_UL_BB_routed.dcp !=  SH_UL_BB_routed.md5，则表示SH_UL_BB_routed.dcp文件异常`；

# 使用指导
* 当SH_UL_BB_routed.dcp文件`异常`时，则`不能用于工程编译`；
* 当SH_UL_BB_routed.dcp文件`异常`时，则`需要更新文件并重新校验`。