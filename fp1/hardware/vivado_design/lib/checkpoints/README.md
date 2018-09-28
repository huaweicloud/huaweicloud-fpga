# Directory Structure

[切换到中文版](./README_CN.md)

* [lib/chekpoints](#lib/chekpoints_dir)/  
  - SH_UL_BB_routed.dcp 
  - SH_UL_BB_routed.sha256

# Directory Description
* SH_UL_BB_routed.dcp
  This is a netlist file for shell static logic.
* SH_UL_BB_routed.sha256
  The file records the sha256 check value. If:
  - `sha256sum SH_UL_BB_routed.dcp ==  SH_UL_BB_routed.sha256, this indicates that the SH_UL_BB_routed.dcp file is in the normal state`;
  - `sha256sum SH_UL_BB_routed.dcp !=  SH_UL_BB_routed.sha256, this indicates that the SH_UL_BB_routed.dcp file is in the abnormal state`.

# Operation Instructions
* If the **SH_UL_BB_routed.dcp** file is in the `abnormal` state, it `cannot be used for project compilation`.
* If the **SH_UL_BB_routed.dcp** file is in the `abnormal` state, it `needs to be updated and re-verified`.


