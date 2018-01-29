# Directory Structure

[切换到中文版](./README_CN.md)

* [lib/chekpoints](#lib/chekpoints_dir)/  
  - SH_UL_BB_routed.dcp 
  - SH_UL_BB_routed.md5

# Directory Descriptions
* SH_UL_BB_routed.dcp     
  This is a netlist file for shell static logic.
* SH_UL_BB_routed.md5 SH_UL_BB_routed.dcp    
  The file records the MD5 check value. If:
  - `md5sum SH_UL_BB_routed.dcp ==  SH_UL_BB_routed.md5, this indicates that the SH_UL_BB_routed.dcp file is in the normal state`;
  - `md5sum SH_UL_BB_routed.dcp !=  SH_UL_BB_routed.md5, this indicates that the SH_UL_BB_routed.dcp file is in the abnormal state`.

# Operation Instructions
* If the **SH_UL_BB_routed.dcp** file is in the `abnormal` state, it `cannot be used for project compilation`.
* If the **SH_UL_BB_routed.dcp** file is in the `abnormal` state, it `needs to be updated and re-verified`.


