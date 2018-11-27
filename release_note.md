# FPGA Accelerated Cloud Server Instance Feature Overview
[切换到中文版](./release_note_cn.md)


* Users can flexibly select high-performance instances (DPDK) or general-purpose instances (opencl) based on their acceleration types.  

* Each user can apply for one, two, four, or eight VU9P FPGA acceleration cards.

* Interfaces available for FPGA acceleration card users are as follows:
  - `One PCIe Gen3 x16` interface    
  - `Four DDR4` RDIMM interfaces

* PCIe features are as follows:
  * High-performance instances:
    - `One physical function (PF)`
    - `One virtual function (VF)`
    - `Eight` queues for each VF   
  * Common instances:
    - `Two PFs`

* Interface features between user logic and static logic are as follows:
  * High-performance instances:
    - The DMA data channel from static logic to user logic uses the AXI4-Stream interface with a bit width of `512 bits`.
    - The DMA data channel from user logic to static logic uses the AXI4-Stream interface with a bit width of `512 bits`.
    - The DMA buffer description (BD) channel from static logic to user logic uses the AXI4-Stream interface with a bit width of `256 bits`.
    - The DMA BD channel from user logic to static logic uses the AXI4-Stream interface with a bit width of `256 bits`.
    - The register access and BAR space mapping use the AXI4-Lite interface a bit width of `32 bits`.
    - DDRs use the AXI4 interface with a bit width of `512 bits`.
  * Common instances:
    - The data channel from user logic to static logic uses the AXI4-MM interface with a bit width of `512 bits`.
    - The control channel from user logic to static logic uses the AXI4-Lite interface with a bit width of `512 bits`.


* The DDR interface partition is as follows:
  - One DDR controller is placed in the static logic partition.
  - Three DDR controllers are placed in the user logic partition.
  - A maximum of four DDR controllers can be used.

# Release 1.2.1

- Added the PR check function to DPDK and OCL.
- Optimized the OCL script.
- Optimized the document.

# Release 1.2.0

- Supported the 1:N multi-card scenario.
- Supported the SDAccel 2017.4 development suite
- Supported coexistence of multiple shell versions.
- Supported user APP single-write, single-read, and loopback access to the DDR connected to an FPGA in the DPDK scenario.
- Optimized simulation in the DPDK scenario to support single-write, single-read, and loopback emulation modes.
- Optimized the document.
- Added the XVC feature.

# Release 1.1.2

- Supported the 1:N multi-card scenario.
- Optimized the document.

# Release 1.1.1

- Optimized the document.
- Optimized the AEI registration mode.

# Release 1.1.0

- Optimize the -vivado simulation platform.
- Optimized the xdma driver of SDAccel.
- Optimized the document and added the User_Development_Guide_for_an_FACS document.
- Resolved security vulnerabilities in the opencl code.
- Optimized the example3 src code.
- Optimized the dpdk src code.

# Release 1.0.1
- Optimized the document with more than 100 experience problems of various documents incorporated.
- Optimized the setup.sh script to resolve the problem that the file coverage is abnormal.
- Fixed the bugs in the pmd code.
-Modified perf.cpp, datamover.h, and shim.cpp for code scanning.
- Optimized the dpdk compilation script: The build_dpdk.sh and build_dpdk_app.sh scripts are modified. When an error occurs, the error description is displayed and the error code is returned. After the command is executed successfully, the system displays a message indicating that the operation is successful.

# Release 1.0.0
- This is the first public version of the Huawei Cloud FPGA design instance. For details about its functions, see **FPGA Design Instance Feature Overview** and **FPGA Design Instance Feature Description**.

---
# FPGA Instance Feature Description

# Contents

## 1 [Project Building](#Project Building)
## 2 [User Simulation](#User Simulation)
## 3 [Application Tests](#Application Tests)
## 4 [Tools and Environment](#Tools and Environment)
## 5 [License Requirements](#License Requirements)
## 6 [Features To Be Supported](#Features To Be Supported)
***
<a name="Project Building"></a>
# Project Building

## Overview
Before building a project, `ensure that Vivado and license are installed`. The project building aims to design a project that meets the timing requirements through the minimum modification.

## Features

* Vivado and SDAccel designs

* `VHDL and Verilog` coding

* `OpenCL, C, and C++` coding

* `Automatic scanning and encryption` of VHDL, Verilog, OpenCL, C, C++, and SystemVerilog code

* User configuration and command execution decoupling. To build a project, you only need to `define the project name and path`.

* Flexible configuration and selection of all Vivado synthesis policies. Available synthesis policies are as follows:
  - DEFAULT
  - AreaOptimized_high
  - AreaOptimized_medium
  - AreaMultThresholdDSP
  - AlternateRoutability
  - PerfOptimized_high
  - PerfThresholdCarry
  - RuntimeOptimized

* Flexible configuration of all Vivado implementation policies. Available implementation policies are as follows:
  - DEFAULT
  - Explore
  - ExplorePostRoutePhysOpt
  - WLBlockPlacement
  - WLBlockPlacementFanoutOpt
  - NetDelay_high
  - NetDelay_low
  - Retiming
  - ExtraTimingOpt
  - RefinePlacement
  - SpreadSLLs
  - BalanceSLLs
  - SpreadLogic_high
  - SpreadLogic_medium
  - SpreadLogic_low
  - SpreadLogic_Explore
  - SSI_SpreadLogic_high
  - SSI_SpreadLogic_low
  - SSI_SpreadLogic_Explore
  - Area_Explore
  - Area_ExploreSequential
  - Area_ExploreWithRemap
  - Power_DefaultOpt
  - Power_ExploreArea
  - Flow_RunPhysOpt
  - Flow_RunPostRoutePhysOpt
  - Flow_RuntimeOptimized
  - Flow_Quick

* Automatic timing report checking and check result printing after project building

* IP customizing by using Vivado IP catalog

* PR verification

* `Execution in steps` for synthesis, implementation, PR verification, and target file generation

* `Scheduled execution` of projects

* Automatic scanning of user code, Huawei IP, and Xilinx IP

* `One-click` user directory creation

* SHELL SHA256 check


---

<a name="User Simulation"></a>

# User Simulation

## Overview
Before executing a project, ensure that `Vivado and license are installed`. User simulation aims to verify whether the existing designs are as required through the verification platform.

## Features

* `Standard SystemVerilog 2012 syntax` supported in Testbench

* Code coverage collection and report generation
  - Customizable code coverage collection files

* AXI4 and AXI4-Lite verification features
  - `burstlen` of the AXI4 interface supported `from 1 to 255`
  - `outstanding` feature of the AXI4 AW and AR channels
  - Consistency check of the `burst` length and the actual length
  - Coverage collection based on the AXI4 and AXI4-Lite protocols
  - Assertion check based on the AXI4 and AXI4-Lite protocols

* Customizable incentives
  - Customizable incentives based on configuration files
  - Incentive generating and sending by users

* Customizable callback methods
  - Customizable callback task/function (Modifying Testbench is not needed.)

* Decoupling of Testbench and Testcase
  - `Testcase design and implementation without modifying Testbench`

* Simple Scoreboard
  - Basic packet comparison based on stream_id and fsn

* Function coverage collection and report generation
  - Function coverage based on the AXI4 and AXI4-Lite interfaces
  - A function coverage of `burst_len, burst_size, burst_mode, and strobe`

* Interface assertion
  - Assertion based on the `AXI4 and AXI4-Lite interfaces`
  - Assertion covering the X/Z status check

* Debugging tools
  - `Verdi, DVE, QuestaSim, and Vivado` for debugging

* Precompiling Xilinx simulation libraries
  - unisims, unimacro, and secureip provided to improve the speed of simulation compilation

---


<a name="Application Tests"></a>

# Application Tests

## Overview

In the `fp1/software/` directory, there is an application project subdirectory. You can compile an application project by using script code (applications) to test features or functions of the project. For details, see readme in this directory.

---

<a name="Tools and Environment"></a>

# Tools and Environment

* The supported tools and environment are as follows:
  - Linux `centos 7.3`  
  - Xilinx `Vivado 2017.2` 
  - Xilinx `SDAccel 2017.1` 

---

<a name="License Requirements"></a>

# License Requirements
* The required licenses are as follows:      
  - SysGen  
  - PartialReconfiguration  
  - Simulationt  
  - Analyzer  
  - HLS  
  - ap_opencl  
  - XCVU9P  
  - EncryptedWriter_v2  
  - xcvu9p_bitgen  
---
<a name="Features To Be Supported"></a>

# Features To Be Supported
* peer to peer
