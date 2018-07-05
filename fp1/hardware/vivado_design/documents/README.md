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
  - **UL_TYPE** instantiates CBB **ro_reg_inst** and obtains the version information of example 1.
  - **DATA_TEST** instantiates CBB **ts_reg_inst**, which implements the `input data inversion` function.
  - **ADDR_TEST** instantiates CBB **ts_addr_reg_inst**, which implements the `inversion of the last operation address`.
  - **ADDER** instantiates `two CBB rw_reg_inst blocks` as the addend and augend, and then instantiates one `CBB ro_reg_inst` to read the addition result.
  - VLED is accessed under `PF` and static logic provides pins for dynamic logic. Users read and write the VLED and ensure that `UL` works properly. VLED instantiates a group of CBB **rw_reg_inst** blocks, which link the output result to VLED.
  - Input signals of DDR user interfaces are instantiated in `unused_ddr_a_b_d_inst.h` and `unused_ddr_c_inst.h `, and the value of the signals is **0**.
  - **DEBUG_BRIDGE** and **ILA0** are instantiated for debugging. `Eight` ILA debugging signals are available.

* example2.jpg 
  - In the **example2.jpg**, **UL_VER** instantiates CBB **ro_reg_inst**. The version number obtained by the `application` is the release time of example 2.
  - **UL_TYPE** instantiates CBB **ro_reg_inst** and obtains the version information of example 2.
  - **DATA_TEST** instantiates CBB **ts_reg_inst**, which implements the `input data inversion` function.
  - **ADDR_TEST** instantiates CBB **ts_addr_reg_inst**, which implements the `inversion of the last operation address`.
  - **DMA_UL** sends a read packet request to the host, and then sends the received packets back to the host. In this way, the `DMA data loopback` of the `x86- > host- > user logic- > host- > x86` path is implemented.
  - **DDR_WR_RD** implements read/write access to the data channels of `four DDRs`.
  - VLED is accessed under `PF` and static logic provides pins for dynamic logic. Users read and write the VLED and ensure that `UL` works properly.
  - VLED instantiates a group of CBB **rw_reg_inst** blocks, which link the output result to VLED.
  - **ADDER** is an adder which sums up the input data.
  - **DEBUG_BRIDGE** and **ILA0** are instantiated for debugging. `Eight` ILA debugging signals are available.

* example3.jpg

  - ![new_example3](./example3.jpg)
  - 1 Functions
  - Example 3 supports DMA read-only mode, DMA write-only mode, and packet PROCESS mode. In PROCESS mode, packets are written to DDRs, processed by the kernel, and then read from the DDRs and sent back to the CPU. The packet processing mode depends on the user requirements and is specified by the **opcode** field in the shell-to-UL BDs. The first 32 bytes of the **payload** storage area of **src_addr** in the CPU mbuf packets indicate the **hardacc** field. The **hardacc** field contains information such as the address for writing packets to DDRs and the address for reading packets from DDRs. The **length** field in the shell-to-UL BDs specifies the DDR read/write length. For details, see the User_Development_Guide_for_an_FACS.
  - ​
  - 2 Data Processing Process
  - 2.1 DMA Write-Only Mode
  - 1) Applications issue the DMA write command to MMU_UL through the DPDK driver to write packets to DDR BDs.
  - 2) MMU_UL sends a read command to shell to read **src_addr** in the **hardacc** field to obtain the DDR write address.
  - 3) MMU_UL issues a read packet command to read **payload**, and writes the packet to the specified DDR address.
  - 4) MMU_UL constructs a tail packet and sends it to shell to respond to the CPU. The DMA write operation is complete.
  - 2.2 DMA Read-Only Mode
  - 1) Applications issue the DMA read command to MMU_UL through the DPDK driver to read packets from DDR BDs.
  - 2) MMU_UL sends a read command to shell to read **src_addr** in the **hardacc** field to obtain the DDR read address.
  - 3) MMU_UL reads the packet from the specified DDR address and sends the packet through the UL-to-shell data interface.
  - 4) MMU_UL constructs a tail packet and sends it to shell to respond to the CPU. The DMA read operation is complete.
  - 2.3 Packet PROCESS Mode
  - 1) Applications issue the DMA write and read commands to MMU_UL through the DPDK driver to write packets to and then read packets from DDR BDs.
  - 2) MMU_UL sends a read command to shell to read **src_addr** in the **hardacc** field to obtain the DDR read and write addresses.
  - 3) MMU_UL issues a read packet command to read **payload**, writes the packet to the specified DDR write address, and notifies the KERNEL_UL module. Then, KERNEL_UL reads the packet from the DDR write address (obtained from **hardacc**), accelerates the packet, writes the packet into the DDR read address (obtained from **hardacc**), and notifies the MMU_UL module.
  - 4) MMU_UL reads the packet from the specified DDR read address and sends the packet through the UL-to-shell data interface.
  - 5) MMU_UL constructs a tail packet and sends it to shell to respond to the CPU. The PROCESS operation is complete.
  - ​
  - 3 Internal Modules, Functions, and Interfaces
  - In the **example3.jpg**, UL_VER instantiates CBB **ro_reg_inst**. The version number obtained by the `application` is the release time of example 3.
  - UL_TYPE instantiates CBB **ro_reg_inst** and obtains the version information of example 3.
  - DATA_TEST instantiates CBB **ts_reg_inst**, which implements the input data inversion function.
  - ADDR_TEST instantiates CBB **ts_addr_reg_inst**, which implements the inversion of the last operation address.
  - MMU_UL provides different functions depending on the packet processing mode. In PROCESS mode, MMU_UL sends read data requests to the host and writes the received data to DDRs, and reads the data processed by DDRs and sends the data to the host. In DMA write-only mode, MMU_UL sends read data requests to the host, writes the received data to DDRs, constructs a tail packet carrying **hardacc**, and sends the packet to the host. In DMA read-only mode, MMU_UL reads data stored in DDRs and sends the data to the host.
  - KERNEL_UL reads the data written by MMU_UL in DDRs, processes the data, and writes the data to DDRs. Then MMU_UL obtains the data.
  - The interfaces between MMU_UL and KERNEL_UL, and between MMU_TX_UL and MMU_RX_UL in MMU_UL are used to transfer information such as **src_addr**, **dst_addr**, **opcode**, **length**, and DDR read and write addresses.
  - SMT_CON converges the read and write requests to the DDR controllers of MMU_UL and KERNEL_UL, arbitrates the requests, and then connects the read and write DDR controller signals to the four groups of DDR controller modules. The interface of SMT_CON connects to the request interface signals of the DDR controllers of MMU_UL and KERNEL_UL at one end, connects to the four groups of DDR controllers at the other end.
  - DDRX_72B_TOP is a DDR controller. The UL part has three groups of DDR controllers, and the shell part has the rest one group of DDR controller, which is accessible by UL through the AXI4 interface. The interface of DDRX_72B_TOP connects to SMT_CON at one end, and connects to DDR PHY at the other end. DDRX_72B_TOP supports CPU configuration and debugging register reading of the HPI interface. In addition, DDRX_72B_TOP implements DDR RAM read/write control.
  - VLED is accessed under `PF` and static logic provides pins for dynamic logic. You can read and write VLED to ensure that `UL` works properly.
  - VLED instantiates a group of CBB **rw_reg_inst** blocks, which lead the output result to VLED.
  - ADDER is an adder which sums up the input data.
  - DEBUG_BRIDGE and ILA0 are instantiated for debugging. `Eight` ILA debugging signals are available.
  - ​
  - 4 Transplantation and Design Constraints
  - You can refer to the example 3 design to complete the DMA write-only function, DMA read-only function, and packet PROCESS function.
  - 4.1 DMA Write-Only or Read-Only Function
  - You can directly invoke Huawei's designs in most cases. When invoking the application-related interface functions (for example, on the usage mode and number of DDRs, or which DDR is used), you only need to slightly modify the functions to match the application, and the logic part can be directly used in most cases.
  - 4.2 PROCESS Function
  - You need to adapt interfaces related to KERNEL_UL to the applications, including:
  - 1) The AXI4 information interface is used between KERNEL_UL and MMU_UL to transmit packet BD information. For details, see the FACS User Guide. 2) The DDR read/write request interface is used between KERNEL_UL and SMT_CON_UL. You can send read and write DDR controller requests to SMT_CON_UL based on the actual read/write requests from KERNEL_UL to DDR controllers. It is recommended that the number of read/write requests be the same as that in this reference design. In this way, the SMT_CON_UL module can be reused directly.
  - 3) In addition, you need to add a CPU access interface to KERNEL_UL so that the CPU can read and write the register/entry configuration and the debugging register of the KERNEL_UL module.
  - 4.3 Design Constraints
  - 1) This reference design supports only the read and write operations of a single packet in a single 16 GB DDR, which is executed by the DPDK driver. This reference design does not support storage of a single packet in two or more DDRs, which is restricted by the application mode of DDR controllers.
  - 2) In DMA write-only and DMA read-only modes, you need to ensure normal read/write sequence for applications. That is, a packet must be written into a DDR address before it can be read from the address area.
  - 3) In either of the three modes, a tail packet needs to be constructed in the UL receive direction to respond to the CPU, so that the driver can complete packet processing.

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

