# Directory Structure

[切换到中文版](./README_CN.md)

The **Documents** folder contains the following documents:

* [documents](#documents_dir)/
  - **example1.jpg**: shows the logic structure of example 1.
  - **example2.jpg**: shows the logic structure of example 2.
  - **example3.jpg**: shows the logic structure of example 3.
  - **interface_signal.md**: describes the signals of interfaces between static logic and dynamic logic.  
  - **Pcie_Memory_Map.md**: describes PCIe storage space partition.  
  - **requirements for tools and license.md**: describes the requirement for tools and license.  
  - **SH_UL_interface.jpg**: shows the interfaces between static logic and dynamic logic.  
  - **README.md** (this document) 

# Contents Description

* example1.jpg 
  - In the **example1.jpg**, **UL_VER** instantiates CBB **ro_reg_inst**. The version number obtained by the `application` is the release time of example 1.
  - **UL_TYPE** instantiates CBB **ro_reg_inst** and obtains the version information of this example, for example, `32'h00d10001`.
  - **DATA_TEST** instantiates CBB **ts_reg_inst**, which implements the `input data inversion` function.
  - **ADDR_TEST** instantiates CBB **ts_addr_reg_inst**, which implements the `inversion of the last operation address`.
  - **ADDER** instantiates `two CBB rw_reg_inst blocks` as the addend and augend, and then instantiates one `CBB ro_reg_inst` to read the addition result.
  - VLED is accessed under `PF` and static logic provides pins for dynamic logic. Users read and write the VLED and ensure that `UL` works properly.VLED instantiates a group of CBB **rw_reg_inst** blocks, which link the output result to VLED.
  - Input signals of DDR user interfaces are instantiated in `unused_ddr_a_b_d_inst.h` and `unused_ddr_c_inst.h `, and the value of the signals is **0**.
  - **DEBUG_BRIDGE** and **ILA0** are instantiated for debugging. `Eight` ILA debugging signals are available.

* example2.jpg 
  - In the **example2.jpg**, **UL_VER** instantiates CBB **ro_reg_inst**. The version number obtained by the `application` is the release time of example 2.
  - **UL_TYPE** instantiates CBB **ro_reg_inst** and obtains the information of this example, for example, `32'h00d20001`.
  - **DATA_TEST** instantiates CBB **ts_reg_inst**, which implements the `input data inversion` function.
  - **ADDR_TEST** instantiates CBB **ts_addr_reg_inst**, which implements the `inversion of the last operation address`.
  - **DMA_UL** sends a read packet request to the host, and then sends the received packets back to the host. In this way, the `DMA data loopback` of the `x86- > host- > user logic- > host- > x86` path is implemented.
  - **DDR_WR_RD** implements read/write access to the data channels of `four DDRs`.
  - VLED is accessed under `PF` and static logic provides pins for dynamic logic. Users read and write the VLED and ensure that `UL` works properly.
  - VLED instantiates a group of CBB **rw_reg_inst** blocks, which link the output result to VLED.
  - **ADDER** is an adder which sums up the input data.
  - **DEBUG_BRIDGE** and **ILA0** are instantiated for debugging. `Eight` ILA debugging signals are available.

* example3.jpg 
  - In the **example3.jpg**, **UL_VER** instantiates CBB **ro_reg_inst**. The version number obtained by the `application` is the release time of example 3.
  - **UL_TYPE** instantiates CBB **ro_reg_inst** and obtains the information of this example, for example, `32'h00d30001`.
  - **DATA_TEST** instantiates CBB **ts_reg_inst**, which implements the `input data inversion` function.
  - **ADDR_TEST** instantiates CBB **ts_addr_reg_inst**, which implements the `inversion of the last operation address`.
  - **MMU_UL** sends a read data request to the host, writes the received data to DDRs, reads the processed write data in DDRs, and sends the processed data to the host.
  - **KERNEL_UL** reads the data written by **MMU_UL** in DDRs, processes the data, and writes the data to DDRs. Then **MMU_UL** obtains the data.
  - VLED is accessed under `PF` and static logic provides pins for dynamic logic. Users read and write the VLED and ensure that `UL` works properly.
  - VLED instantiates a group of CBB **rw_reg_inst** blocks, which link the output result to VLED.
  - **ADDER** is an adder which sums up the input data.
  - **DEBUG_BRIDGE** and **ILA0** are instantiated for debugging. `Eight` ILA debugging signals are available.

* interface_signal.md
  - This document describes all signals of the interfaces between user logic and static logic. 

* requirements for tools and license.md  
  - This document describes the tools for running fpga_design and `license` requirements.

* pcie_memory_map.md  
  - This document describes the storage space partition of `PCIe`.

* sh_ul_interface.jpg   
  - This image describes available types and bit widths of the interfaces between static logic and dynamic logic designed by users.
  - SH is the static part of logic interfaces. You only need to know about the composition of this part.
  - UL is the dynamic part of logic interfaces. You can customize this part.

* README.md
  - This document describes other documents.

