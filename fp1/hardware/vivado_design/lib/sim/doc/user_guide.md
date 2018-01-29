# Simulation Platform User Guide

[切换到中文版](./user_guide_cn.md)

<div id="table-of-contents">
<h2>Contents</h2>
<div id="text-table-of-contents">
<ul>
<li><a href="#sec-1">1 <b>About This Document</b></a></li>
<li><a href="#sec-2">2 <b>Simulation Platform Overview</b></a>
<ul>
<li><a href="#sec-2-1">2.1 <b>FACS Simulation Platform</b></a></li>
<li><a href="#sec-2-2">2.2 <b>Directory Structure of the Simulation Platform</b></a>
<ul>
<li><a href="#sec-2-2-1">2.2.1 <b>Simulation Directory and File Description</b></a></li>
</ul>
</li>
<li><a href="#sec-2-3">2.3 <b>Verification Features</b></a></li>
<li><a href="#sec-2-4">2.4 <b>Verification Components</b></a></li>
<li><a href="#sec-2-5">2.5 <b>Data Structure</b></a></li>
<li><a href="#sec-2-6">2.6 <b>Bus Functional Model</b></a>
<ul>
<li><a href="#sec-2-6-1">2.6.1 <b>Function</b></a></li>
<li><a href="#sec-2-6-2">2.6.2 <b>Design</b></a></li>
</ul>
</li>
<li><a href="#sec-2-7">2.7 <b>Incentive Components</b></a>
<ul>
<li><a href="#sec-2-7-1">2.7.1 <b>Function</b></a></li>
<li><a href="#sec-2-7-2">2.7.2 <b>Design</b></a></li>
</ul>
</li>
<li><a href="#sec-2-8">2.8 <b>Checker</b></a>
<ul>
<li><a href="#sec-2-8-1">2.8.1 <b>Function</b></a></li>
<li><a href="#sec-2-8-2">2.8.2 <b>Design</b></a></li>
</ul>
</li>
<li><a href="#sec-2-9">2.9 <b>Test Environment</b></a></li>
</ul>
</li>
<li><a href="#sec-3">3 <b>Simulation Platform Concept</b></a>
<ul>
<li><a href="#sec-3-1">3.1 <b>Layer</b></a></li>
</ul>
</li>
<li><a href="#sec-4">4 <b>Simulation Platform Application</b></a>
<ul>
<li><a href="#sec-4-1">4.1 <b>Usage Process of the Simulation Platform</b></a>
</li>
<li><a href="#sec-4-2">4.2 <b>Script</b></a>
<ul>
<li><a href="#sec-4-2-1">4.2.1 <b>Makefile Description</b></a></li>
<li><a href="#sec-4-2-2">4.2.2 <b>Makefile Usage</b></a></li>
</ul>
</li>
<li><a href="#sec-4-3">4.3 <b>Simulation Platform Interfaces<b></a>
<ul>
<li><a href="#sec-4-3-1">4.3.1. <b>axi_stims.sv<b></a></li>
<li><a href="#sec-4-3-2">4.3.2. <b>cpu_model_cb<b></a></li>
</ul>
</li>
<li><a href="#sec-4-4">4.4 <b>Simulation Platform Configurations<b></a>
<ul>
<li><a href="#sec-4-4-1">4.4.1 <b>Configuration File Format</b></a></li>
<li><a href="#sec-4-4-2">4.4.2 <b>Adding Configuration Items</b></a></li>
</ul>
</li>
</ul>
</li>
<li><a href="#sec-5">5 <b>Examples</b></a>
<ul>
<li><a href="#sec-5-1">5.1 <b>Example 1</b></a>
<ul>
<li><a href="#sec-5-1-1">5.1.1 <b>Example 1 Overview</b></a></li>
<li><a href="#sec-5-1-2">5.1.2 <b>Example 1 Description</b></a></li>
<li><a href="#sec-5-1-3">5.1.3 <b>Example 1 Operation Instructions</b></a></li>
</ul>
</li>
<li><a href="#sec-5-1">5.2 <b>Example 2</b></a>
<ul>
<li><a href="#sec-5-1-1">5.2.1 <b>Example 2 Overview</b></a></li>
<li><a href="#sec-5-1-2">5.2.2 <b>Example 2 Description</b></a></li>
<li><a href="#sec-5-1-3">5.2.3 <b>Example 2 Operation Instructions</b></a></li>
</ul>
</li>
</ul>
</li>
<li><a href="#sec-6">6 <b>Appendix</b></a></li>
</ul>
</div>
</div>

<a id="sec-1" name="sec-1"></a>

## **About This Document**

---

This document describes how to verify the logic by using the **FPGA Accelerated Cloud Server (FACS) simulation platform**. This document covers:

- Description of the Testbench
  - Features of the Testbench
  - Usage of the Testbench
- Testbench concept
- The method of adding components to the Testbench
- The method of creating test cases
- Testbench Usage through examples

<a id="sec-2" name="sec-2"></a>

## **Simulation Platform Overview**

---

This section introduces and describes the features and basic architecture of the **FACS simulation platform**(**Testbench**, **TB**, or the **simulation platform** for short).

<a id="sec-2-1" name="sec-2-1"></a>

### **FACS Simulation Platform**

---

The Testbench is a simulation and verification environment built to verify the DUV. The FACS simulation platform is a Testbench that allows users to perform verification in the FACS environment.
The FACS Testbench consists of multiple verification components. Each component is an encapsulated collection of an interface protocol, functions, or verification environment.
All verification components have a unified architecture, but work differently. These components combine together to form the entire Testbench.

The Testbench is the main body for DUT simulation. Therefore, a complete Testbench should support:

- Generation and sending of user-defined incentives
- Simulation of the DUT interface timing
- Check of the DUT result

The FACS simulation platform is a general-purpose simulation platform, which contains all features of a common Testbench, supports verification throughout the process, and provides a decoupled architecture and a host of verification IP library for users. The following figure shows the simulation platform structure.

<img src="./images/testbench.png" alt="Simulation platform structure">

<a id="sec-2-2" name="sec-2-2"></a>

### **Directory Structure of the Simulation Platform**

---

- [hardware/](#sec-2-2-1-1)
  - [vivado_design/](#sec-2-2-1-2)
    - [lib/](#sec-2-2-1-3)
      - [sim/](#sec-2-2-1-4)
        - [bench/](#sec-2-2-1-5)
          - [common/](#sec-2-2-1-6)
          - [stim/](#sec-2-2-1-7)
          - [bfm/](#sec-2-2-1-8)
          - [rm/](#sec-2-2-1-9)
          - [test/](#sec-2-2-1-10)
          - [vip/](#sec-2-2-1-11)
            - [xxx_vip/](#sec-2-2-1-12)
          - [precompiled/](#sec-2-2-1-13)
            - [vcs_lib/](#sec-2-2-1-14)
            - [questa_lib/](#sec-2-2-1-15)
          - [scripts/](#sec-2-2-1-16)
          - [doc/](#sec-2-2-1-17)
    - [user/](#sec-2-2-1-18)
      - [user_xxx/](#sec-2-2-1-19)
        - [sim/](#sec-2-2-1-4)
          - [common/](#sec-2-2-1-6)
          - [libs/](#sec-2-2-1-20)
          - [tests/](#sec-2-2-1-21)
            - [sv/](#sec-2-2-1-22)
              - [base/](#sec-2-2-1-23)
              - [xxx_test/](#sec-2-2-1-24)
            - [c/](#sec-2-2-1-25)
          - [scripts/](#sec-2-2-1-16)
          - [work/](#sec-2-2-1-26)
          - [report/](#sec-2-2-1-27)
          - [wave/](#sec-2-2-1-28)
          - [doc/](#sec-2-2-1-32)
          - [Makefile](#sec-2-2-1-17)
    - [examples/](#sec-2-2-1-30)
      ...

<a id="sec-2-2-1" name="sec-2-2-1"></a>

#### **Simulation Directory and File Description**

---

<a id="sec-2-2-1-1" name="sec-2-2-1-1"></a>

##### hardware

This is the hardware directory, which stores all FPGA designs, verification documents, code, and project files.

<a id="sec-2-2-1-2" name="sec-2-2-1-2"></a>

##### vivado_design

This is the directory for Vivado development, simulation, and implementation. This directory stores the Vivado-based FPGA designs, verification documents, code, and project files.

<a id="sec-2-2-1-3" name="sec-2-2-1-3"></a>

##### lib

This is the common library directory. You do not need to modify the contents in this directory.

<a id="sec-2-2-1-4" name="sec-2-2-1-4"></a>

##### sim

This is the simulation directory, which stores simulation platform code, scripts, precompiled libraries, and VIPs.

<a id="sec-2-2-1-5" name="sec-2-2-1-5"></a>

##### bench

This is the Testbench directory.

<a id="sec-2-2-1-6" name="sec-2-2-1-6"></a>

##### common

This is the common file directory, which stores common header files and interface definitions.

<a id="sec-2-2-1-7" name="sec-2-2-1-7"></a>

##### stim

This is the Testbench incentive directory, which stores incentive data and incentive-generating components.

<a id="sec-2-2-1-8" name="sec-2-2-1-8"></a>

##### bfm

This is the Testbench BFM directory, which stores BFM directories of the AXI Master, AXI Slave, and AXI-Lite interfaces.

<a id="sec-2-2-1-9" name="sec-2-2-1-9"></a>

##### rm

This is the Testbench RM directory, which stores the RM and scoreboard.

<a id="sec-2-2-1-10" name="sec-2-2-1-10"></a>

##### test

This is the directory for Testbench environment and basic tests.

<a id="sec-2-2-1-11" name="sec-2-2-1-11"></a>

##### vip

This is the verification IP directory, which stores all verification IPs of the platform.

<a id="sec-2-2-1-12" name="sec-2-2-1-12"></a>

##### xxx_vip

These are VIP directories, which are stored separately based on VIPs.

<a id="sec-2-2-1-13" name="sec-2-2-1-13"></a>

##### precompiled

This is the precompiled library directory, which stores the precompiled general-purpose simulation model of Xilinx. This model improves the compilation speed.

<a id="sec-2-2-1-14" name="sec-2-2-1-14"></a>

##### vcs_lib

This is the precompiled library of the VCS.

<a id="sec-2-2-1-15" name="sec-2-2-1-15"></a>

##### questa_lib

This is the precompiled library of the QuestaSim.

<a id="sec-2-2-1-16" name="sec-2-2-1-16"></a>

##### scripts

This is the simulation script directory, which stores simulation scripts.

<a id="sec-2-2-1-17" name="sec-2-2-1-17"></a>

##### doc

This is the simulation document folder, which stores simulation platform description and designs.

<a id="sec-2-2-1-18" name="sec-2-2-1-18"></a>

##### user

This is the user folder, which stores all user changes and intermediate files.

<a id="sec-2-2-1-19" name="sec-2-2-1-19"></a>

##### user_xxx

These are user directories, which are stored separately based on projects.

<a id="sec-2-2-1-20" name="sec-2-2-1-20"></a>

##### libs

This is the user simulation library directory, which stores the library files required by users or IPs compiled by users.

<a id="sec-2-2-1-21" name="sec-2-2-1-21"></a>

##### tests

This is the test case folder, which stores all C and SV test cases.

<a id="sec-2-2-1-22" name="sec-2-2-1-22"></a>

##### sv

This is the SystemVerilog-based test case directory.

<a id="sec-2-2-1-23" name="sec-2-2-1-23"></a>

##### test_common

This is the directory of test case common files. This directory stores basic test cases or their common files.

<a id="sec-2-2-1-24" name="sec-2-2-1-24"></a>

##### xxx_test

This is the user test case directory, which stores scripts, configuration files, and .sv files.

<a id="sec-2-2-1-25" name="sec-2-2-1-25"></a>

##### c

This is the C-based test case directory.

<a id="sec-2-2-1-26" name="sec-2-2-1-26"></a>

##### work

This is the user work directory, which stores the compilation results of the user simulation.

<a id="sec-2-2-1-27" name="sec-2-2-1-27"></a>

##### report

This is the user log directory, which stores all log files generated during user compilation and simulation.

<a id="sec-2-2-1-28" name="sec-2-2-1-28"></a>

##### wave

This is the user wave directory, which stores all wave files generated during user simulation.

<a id="sec-2-2-1-29" name="sec-2-2-1-29"></a>

##### Makefile

This is the user Makefile directory.

<a id="sec-2-2-1-30" name="sec-2-2-1-30"></a>

##### example

This is the user example directory, which stores all user example code, scripts, and documents.

<a id="sec-2-3" name="sec-2-3"></a>

### **Verification Features**

---

- Standard SystemVerilog 2012 syntax
- Code coverage collection and report generation
  - Customizable code coverage collection files
- AXI4 and AXI4-Lite verification features
  - AXI4 interface **burstlen** from **1** to **255**
  - **outstanding** feature of the AXI4 AW and AR channels
  - Consistency check of the **burst** length and the actual length
  - Coverage collection based on the AXI4 and AXI4-Lite protocols
  - Assertion check based on the AXI4 and AXI4-Lite protocols
- Customizable incentives based on configuration files
  - Customizable incentives
  - Generating and sending incentives by users
- Customizable callback methods
  - Customizable callback task/function (Modifying Testbench is not needed.)
- Separated Testbench and test cases
  - Designing and implementing test cases without modifying the Testbench
- Simple scoreboard
  - Basic packet comparison based on **stream_id** and **fsn**
- Function coverage collection and report generation
  - Function coverage based on the AXI4 and AXI4-Lite interfaces
  - **burst_len**, **burst_size**, **burst_mode**, and **strobe** function coverage
- Interface assertion
  - Assertion based on the AXI4 and AXI4-Lite interfaces
  - Assertion covering the X/Z status check
- Debugging tools
  - Verdi and DVE
  - QuestaSim
  - Vivado
- Precompiled simulation libraries of Xilinx
  - Precompiled **unisims**, **unimacro**, and **secureip** (This improves the compilation speed of the user simulation.)
- Recommended environment and operating systems
  - Linux CentOS 7.3
  - Xilinx 2017.2 Vivado
  - Synopsys VCSMX 2017-03-SP1
  - Mentor QuestaSim 10.6b

<a id="sec-2-4" name="sec-2-4"></a>

### **Verification Components**

---

To implement the verification features of the simulation platform, the verification components are classified into the following types: (The following acronyms and abbreviations correspond to the **data structure**, **bus functional model**, **incentive**, **checker**, and **simulation environment** respectively.)

- [Data](##sec-2-5)
- [BFM](##sec-2-6)
- [Stim](##sec-2-7)
- [Checker](##sec-2-8)
- [Environment](##sec-2-9)

<a id="sec-2-5" name="sec-2-5"></a>

### Data Structure

---

The incentive basic data format is the encapsulation and abstraction of the data format. In this way, the transmitted data in the Testbench is transformed from signals to transactions.

<a id="sec-2-6" name="sec-2-6"></a>

### Bus Functional Model

---

The bus functional model (BFM) transmits incentive data to the DUV. In addition, the BFM receives response data from the DUV to check the correctness of the result.

<a id="sec-2-6-1" name="sec-2-6-1"></a>

#### Function

---

The BFM functions as a bridge between the Testbench and DUV to implement Testbench layering. The BFM converts the upper-layer data into the interface code stream with timing and sends the code stream to the DUV.
The BFM also converts the timing sequence from the DUV into the data that can be processed by the upper layer, and transmits the data to the TestBench.
The BFM allows the Testbench to use non-timing designs so that upper-layer transactions and bottom-layer timing are layered.

<a id="sec-2-6-2" name="sec-2-6-2"></a>

#### Design

---

The BFM focuses on the conversion between timings and non-timings and does not process transactions. The BFM needs to proactively initiate interface timings to check whether the DUV supports various operation timing sequences.
In addition, the BFM needs to proactively initiate timing sequences of various interface abnormal operations to check the error tolerance capability of the DUV for those sequences.

The internal channel of the FACS is PCIe. Therefore, the RC behavior needs to be simulated to interact with the DUT. The BFM is not only a timing model but also a CPU model.

The CPU model simulates the behavior of the CPU and `Shell` and interacts with the `UL` based on predefined rules. The CPU model consists of the **CPU model** and **CPU model callback**. The following figure shows the CPU model structure.

<img src="./images/model.png" alt="CPU model structure">

The CPU model does not contain any interaction implementations and only provides interfaces to connect to other components. The interaction methods are implemented by the CPU model callback. This callback can be customized by users to implement the user-defined interaction model.

<a id="sec-2-7" name="sec-2-7"></a>

### Incentive Components

---

The incentive components generate incentives based on user definitions.

<a id="sec-2-7-1" name="sec-2-7-1"></a>

#### Function

---

Incentives are the source of user data. The data generated by incentives is sent to the DUV through the BFM. In addition, incentives are significant for Testbench layering.

<a id="sec-2-7-2" name="sec-2-7-2"></a>

#### Design

---

Incentives usually consist of data and generators. You can customize and generate incentives in three methods.

1. Implement a generator to generate incentives.
2. When data is transmitted by configuration files, you do not need to modify a generator.
3. Separate incentive generation from the generator and implement your own incentive generation method without modifying a generator.

The FACS simulation platform combines **method 2** and **method 3** to implement incentives. User incentives are divided into three components: **incentive generation method**, **incentive generator**, and **incentive configuration**. The following figure shows the incentive components structure.

<img src="./images/model.png" alt="Incentive components structure">

The **incentive generator** only provides an interaction interface between incentives and the BFM and does not contain any incentive generation methods. The generation and sending methods of incentives are defined in the **incentive generation method**. After being enabled, the incentive generator automatically invokes the interface of its incentive generation method to generate and send incentives. The methods for generating and sending incentives in the incentive generator are subject to the **incentive configuration**.

To define an incentive generation method, modify the **incentive generation method** and **incentive configuration**.

<a id="sec-2-8" name="sec-2-8"></a>

### Checker

---

A checker checks the correctness of the DUV and can be implemented in either of the following methods:

1. Check based on rules
2. Check based on reference models

<a id="sec-2-8-1" name="sec-2-8-1"></a>

#### Function

A checker is located at the ending point of the Testbench and checks the timing, functions, performance, reliability, and testability of the DUV.

- Check based on rules

After rules are configured by users, the checker reads and parses the rules, and then compares the output data based on the rules.

- Check based on reference models

The checker simulates a model with the same functions as the DUT, expects the output data, and compares it with the actual output data.

The simulation platform supports the preceding comparison methods.

<a id="sec-2-8-2" name="sec-2-8-2"></a>

#### Design

A checker checks the functions and testability of the DUV based on rules, and checks the performance and timing of the DUV based on reference models.
A checker consists of three components: reference models, a scoreboard, and other checkers.

<a id="sec-2-9" name="sec-2-9"></a>

### Test Environment

---

The test environment lies at the top layer of the Testbench, and instantiates and connects Testbench components, effectively reusing and managing the Testbench. On the simulation platform, the test environment consists of test cases and test environments.

A test environment is an encapsulation and combination of Testbench components. Each test environment corresponds to one or more test scenarios. A test case is a collection of all elements for users to perform a test.

The FACS simulation platform divides test cases to a top layer and test cases, initiates the test cases at the top layer, and then executes a selected test case. In this way, the test cases and simulation platform are layered. The following figure shows the detailed result.

<img src="./images/test.png" alt="Test Environment">

Test cases are instantiated at the top layer, which executes test cases specified by users.

<a id="sec-3" name="sec-3"></a>

## **Simulation Platform Principles**

---

<a id="sec-3-1" name="sec-3-1"></a>

### **Layer**

The Testbench simulation platform is divided into the Testbench layer and test case layer based on functions.
The Testbench layer functions as the simulation platform, which contains all simulation components. You do not need to modify this part.
The test case layer is a collection for users to cover their test points. You need to design and compile this part.
Therefore, the Testbench is designed by separating TC from TB.
The incentive generator and BFM components belong to the TB layer and do not need to be modified. Test cases belong to the TC layer and can be modified.

Modifying Incentives
To generate incentives, separate the incentive generator from the incentive generation method, configure the incentive generation method in a test case, and then associate the method with the incentive generator in the Testbench.

Customizing the RM
Compile and instantiate the RM in a test case, and then connect the RM to components in the Testbench through a predefined interface.

Customizing the Functions of Other Components in the Testbench
By using the callback function, you can modify TB component functions without modifying the Testbench.

<a id="sec-4" name="sec-4"></a>

## **Simulation Platform Application**

---

<a id="sec-4-1" name="sec-4-1"></a>

### **Usage Process of the Simulation Platform**

1. Create a project directory.
2. Copy all the files and folders in the `sim` directory of the `example/user` folder to the user directory (If the files and folders already exist, skip this step.).
3. Modify the `project_settings.cfg` file in the `sim/scripts` directory, and configure your library files and path, simulation macro, and parameters.
4. Go to the `sim/common` directory and compile user-defined incentives or the callback.
5. Go to the `test/sv` or `test/c` directory and compile basic test cases.
6. Compile configuration files based on test cases, including the configurations of basic test case names, registers, and incentives.
7. Perform compilation, simulation, and debugging by using the Makefile in the **sim** directory.

<a id="sec-4-2" name="sec-4-2"></a>

### **Scripts**

---

The simulation platform scripts consist of **common** and **user** parts. The common scripts are basic files on the simulation platform and are irrelevant to user projects. You do not need to modify this part. The user scripts are used for user projects or project implementation. You need to modify this part as required.

The simulation platform scripts are as follows:

- Common scripts: compilation, execution, and wave viewing scripts.
- User scripts: **project_setting.cfg**, [Makefile](4-2-1), and user-defined scripts.

<a id="sec-4-2-1" name="sec-4-2-1"></a>

#### Makefile Parameters

The Makefile is a rule file for users to perform the <kbd>make</kbd> command. By using this file, you can perform compilation, simulation, and debugging operations. The following table lists the detailed parameters.

| Parameter     | Description                              |
| ------------- | ---------------------------------------- |
| **clean**     | Deletes all files (including wave files and logs) generated during compilation and simulation. |
| **distclean** | Cleans up the environment. This parameter deletes all files generated during the compilation and simulation and the simulation library. |
| **comp**      | Compiles the Testbench and DUT, and executes elaboration. A parameter *simulation tool* is required after this parameter. |
| **run**       | Executes the simulation. Parameters *test case name* (must be the same as the name of the test case folder) and *simulation tool* are required after this parameter. |
| **wave**      | Debugs and views waves. Parameters *test case name* and *simulation tool* are required after this parameter. |
| **cov**       | Generates a coverage rate report (not supported by the Vivado). The coverage collection is configurable. Parameters *test case name* and *simulation tool* are required after this parameter. |
| **lib**       | Precompiles all simulation libraries. This parameter needs to be executed before compilation and can be automatically executed during environment variables configuration. A parameter *simulation tool* is required after this parameter. |
| **list**      | Lists all available test cases.          |
| **help**      | Lists help information.                  |

* By default, the Vivado is used for compilation and simulation.
  *You can specify other simulation tools such as QuestaSim or VSC.*

<a id="sec-4-2-2" name="sec-4-2-2"></a>

#### Makefile Usage

- **Compilation**

    | Command                          | Description                              |
    | -------------------------------- | ---------------------------------------- |
    | <kbd>make comp</kbd>             | Uses the default simulation tool (Vivado) to compile the simulation platform. |
    | <kbd>make comp TOOL=vcs</kbd>    | Uses VCSMX to compile the simulation platform. |
    | <kbd>make comp TOOL=questa</kbd> | Uses QuestaSim to compile the simulation platform. |
    | <kbd>make comp TOOL=vivado</kbd> | Uses Vivado to compile the simulation platform. |

    *If the simulation platform fails to be compiled, the simulation will not continue.*

- **Execution**

    | Command                                | Description                              |
    | -------------------------------------- | ---------------------------------------- |
    | <kbd>make run</kbd>                    | Uses the default simulation tool (Vivado) to execute the default test case (sv_demo_001). |
    | <kbd>make run TC=xxx</kbd>             | Uses the default simulation tool (Vivado) to execute the test case xxx. |
    | <kbd>make run TC=xxx TOOL=vcs</kbd>    | Uses VCSMX to execute the test case xxx. |
    | <kbd>make run TC=xxx TOOL=questa</kbd> | Uses QuestaSim to execute the test case xxx. |
    | <kbd>make run TC=xxx TOOL=vivado</kbd> | Uses Vivado to execute the test case xxx. |

- **Wave**

    | Command                                 | Description                              |
    | --------------------------------------- | ---------------------------------------- |
    | <kbd>make wave</kbd>                    | Uses the default simulation tool (Vivado) to view the wave of the default test case (sv_demo_001). |
    | <kbd>make wave TC=xxx</kbd>             | Uses the default simulation tool (Vivado) to view the wave of the test case xxx. |
    | <kbd>make wave TC=xxx TOOL=vcs</kbd>    | Uses VCSMX to view the wave of the test case xxx. |
    | <kbd>make wave TC=xxx TOOL=questa</kbd> | Uses QuestaSim to view the wave of the test case xxx. |
    | <kbd>make wave TC=xxx TOOL=vivado</kbd> | Uses Vivado to view the wave of the test case xxx. |

- **One-click compilation and execution**

    | Command                            | Description                              |
    | ---------------------------------- | ---------------------------------------- |
    | <kbd>make</kbd>                    | Uses the default simulation tool (vivado) to compile and execute the default test case (sv_demo_001). |
    | <kbd>make TC=xxx</kbd>             | Uses the default simulation tool (vivado) to compile and execute the test case xxx. |
    | <kbd>make TC=xxx TOOL=vcs</kbd>    | Uses VCSMX to compile and execute the test case xxx. |
    | <kbd>make TC=xxx TOOL=questa</kbd> | Uses QuestaSim to compile and execute the test case xxx. |
    | <kbd>make TC=xxx TOOL=vivado</kbd> | Uses Vivado to compile and execute the test case xxx. |

- **Coverage report generation**

    | Command                         | Description                              |
    | ------------------------------- | ---------------------------------------- |
    | <kbd>make cov TOOL=vcs</kbd>    | Uses VCSMX to generate coverages and combine them to generate a report. |
    | <kbd>make cov TOOL=questa</kbd> | Uses QuestaSim to generate coverages and combine them to generate a report. |

<a id="sec-4-3" name="sec-4-3"></a>

### **Simulation Platform Interfaces**

The simulation platform provides interfaces for users to reload. You need to customize complex incentives and reference models. The simulation platform provides interfaces for users to customize following components:

- [axi_stims.sv](#sec-4-3-1)
- [CPU Model](#sec-4-3-2)
- [Configuration](#sec-5-3)
- [Reference Models](#sec-5-4)

---

<a id="sec-4-3-1" name="sec-4-3-1"></a>

#### axi_stims

---

This component is the basic AXI incentive component responsible for generating incentives based on users' configurations and constraints. User-defined incentives must be inherited from this component. For details about the structure, see [Incentive Components](#sec-2-7).

<a id="sec-4-3-1-1" name="sec-4-3-1-1"></a>

##### axi_stims Interface Description

- **Variable definition**

    | Parameter  | Description                              |
    | ---------- | ---------------------------------------- |
    | m_req_mlbx | Incentive request mailbox. The requests generated are stored in this mailbox and automatically sent to lower-level modules. This mailbox supports the read and write requests of the AXI, AXI-S, and AXI-Lite interfaces. |
    | m_rsp_mlbx | Incentive response mailbox. The responses of lower-level modules are stored in this mailbox. This mailbox supports the read responses of the AXI, AXI-S, and AXI-Lite interfaces. |

- <kbd>new</kbd>

    ** Function:** construction function. This interface is used to create a component and initialize internal variables. Before using the component, invoke this interface to construct the component. For example, to customize an incentive component, you need to explicitly use super.new() in the component to invoke the new method of the parent class.

    **Return value:** None

    | Parameter | Description                              |
    | --------- | ---------------------------------------- |
    | name      | Name of the incentive component (only for printing). |

- <kbd>reg_generator</kbd>

    ** Function:** incentive generator registration. This interface is used to associate incentives with incentive generators. It is a non-virtual method, and users do not need to reload it. Users do not need to invoke this method. This method is automatically invoked by reg_stims of the generator.

    **Type:** Function

    **Return value:** None

    | Parameter | Description                 |
    | --------- | --------------------------- |
    | generator | Incentive generator handle. |

- <kbd>body</kbd>

    ** Function:** incentive body. This interface is used to generate and send user-defined incentives.

    **Parameter:** None

    **Type:** Task

    **Return value:** None

- <kbd>gen_pkt</kbd>

    **Function:** method of generating incentives. This interface is used to generate user-defined incentives. To modify the method of generating incentives, reload this function.

    **Parameter:** None

    **Type:** Task

    **Return value:** None

- <kbd>send_pkt</kbd>

    **Function:** method of sending incentives. This interface is used to generate user-defined incentives. To modify the method of generating incentives, reload this function.

    **Parameter:** None

    **Type:** Task

    **Return value:** None

- <kbd>start</kbd>

    **Function:** method of starting incentives. No parameter is required, and users do not need to reload it.

    **Parameter:** None

    **Type:** Task

    **Return value:** None

- <kbd>stop</kbd>

    **Function:** method of stopping incentives. No parameter is required, and users do not need to reload it. Generally, an incentive will be automatically stopped when it is sent. To stop the incentive before it is sent, invoke this interface.

    **Parameter:** None

    **Type:** Task

    **Return value:** None

- <kbd>wait_done</kbd>

    **Function:** method of waiting for incentives to be sent. No parameter is required, and users do not need to reload it. The method will be blocked until all incentives are sent.

    **Parameter:** None

    **Type:** Task

    **Return value:** None


##### axi_stims Usage

To customize an incentive method, you can modify the `user_stim.sv` file as required.

Suggestions or requirements for the modification:

- `user_stim.sv` must be inherited from `axi_stims`.

- To customize the method of generating incentives, reload the task `gen_pkt`.

- To customize the method of sending incentives, reload the task `send_pkt`.

For example:

```verilog
    class user_stims extends axi_stims;
        ...
        // Stim constraint
        constraint axi_data_user_constraint {
        // If the Vivado is not used as the simulator, compile your own incentive generating mode.
        // If the Vivado is used as the simulator, the code can be deleted.
        // Vivado simulator does not support constraint.
        `ifndef VIVADO
            m_item.id    == 'd0;
            m_item.addr inside {[m_cfg.axi_addr_min : m_cfg.axi_addr_max]};
            m_item.data.size() == m_cfg.axi_data_len;
            m_item.opt   == m_cfg.axi_opt;
            m_item.btype == m_cfg.axi_burst_type;
            m_item.resp  == m_cfg.axi_resp;
        `endif
        }
        ...
        task gen_packet();
            ...
            // Generate data
        `ifndef VIVADO
            assert(randomize()) begin
                `tb_debug(m_inst_name, "Randomize success!")
            end else begin
                `tb_fatal(m_inst_name, "Randomize fail!")
            end
        `else
            // If using vivado simulator, use std::randomize instead to avoid the
            // core dump
            // I was no idea about why randomize cannot be success when using vivado simulator, so I had to comment all randomize.
            id     = 'd0;
            result = 'd1;
            addr  += 'h1000;
            assert(result) begin
                `tb_debug(m_inst_name, "Randomize success!")
                m_item.id    = id   ;
                // Align addr 32bit
                m_item.addr  = addr << 2;
                m_item.opt   = opt  ;
                m_item.btype = btype;
                m_item.resp  = resp ;
                data = new[m_cfg.axi_data_len];
                foreach (data[idx]) begin
                    data[idx] = data_byte++;
                end
                m_item.data  = data ;
            end else begin
                `tb_fatal(m_inst_name, "Randomize fail!")
            end
        `endif
        endtask : gen_packet
        task axi_stims::send_packet();
            ...
            // Copy data to req
            req = m_item.copy();
            // Send request
            m_req_mlbx.put(req);
            ...
            // There is no delay for stim send. You can add time delay here if you need.
        endtask : send_packet
    endclass : user_stims
```

After the modification, you need to instantiate user-defined incentives in test cases and bind the user-defined incentives to the incentive generator, as shown in the following code:

```verilog
    ...
    // Instantiating user-defined components
    function void build();
        ...
        m_user_stim     = new("m_user_stim"); // Instantiating user incentives
        ...
        super.build();
    endfunction : build
    // Connecting user-defined components
    function void connect();
        super.connect();
        ...
        // Associating user incentives with incentive generators
        m_tb_env.m_axi_gen.reg_stims(m_user_stim);
        ...
    endfunction : connect
```

After completing the incentive instantiation and association, you can enable the incentive sending by using the incentive component method <kbd>start</kbd>, manually stop the incentive sending by using <kbd>stop</kbd>, or wait until the incentives are sent and stopped automatically by using <kbd>wait_done</kbd>.

For example:

```verilog
    task run();
        ...
        // Start sending stimulate
        m_user_stim.start();
        // Wait stimulate sending over
        m_user_stim.wait_done();
        ...
    endtask : run
```

For details, see [axi_stims Interface Description](#sec-4-3-1-1).

<a id="sec-4-3-2" name="sec-4-3-2"></a>

#### cpu_model_cb

---

<a id="sec-4-3-2-1" name="sec-4-3-2-1"></a>

##### cpu_model_cb Interface Description

- **Variable definition**

    | Parameter     | Description                              |
    | ------------- | ---------------------------------------- |
    | m_req_mlbx    | Incentive request mailbox. The request generated will be placed into the mailbox and automatically sent to lower-level modules. This mailbox supports the read and write requests of the AXI, AXI-S, and AXI-Lite interfaces. |
    | m_rsp_mlbx    | Incentive response mailbox. The responses of lower-level modules are stored in this mailbox. This mailbox supports the read responses of the AXI, AXI-S, and AXI-Lite interfaces. |
    | m_data        | Data cache, which is used to cache the data of sent incentives. |
    | m_bd          | BD queue, which is used to cache the generated buffer descriptors (BDs). |
    | m_axismc_mlbx | AXI4-Stream Master command interface mailbox, which is used to cache the BDs sent from the CPU model to the DUT. |
    | m_axismd_mlbx | AXI4-Stream Master data interface mailbox, which is used to cache the read data returned by the DUT to the CPU model. |
    | m_axissc_mlbx | AXI4-Stream Slave command interface mailbox, which is used to cache the read requests sent from the CPU model to the DUT. |
    | m_axissc_mlbx | AXI4-Stream Slave data interface mailbox, which is used to cache the read data sent from the CPU model to the DUT. |

- <kbd>new</kbd>

    **Function:** construction function. This interface is used to create a component and initialize internal variables. Before using the component, invoke this interface to construct the component. For example, to customize an incentive component, you need to explicitly use **super.new()** in the component to invoke the **new** method of the parent class.

    **Return value:** None

    | Parameter | Description                              |
    | --------- | ---------------------------------------- |
    | name      | Name of the incentive component (only for printing). |

- <kbd>set_reqmlbx</kbd>

    **Function:** binds the request port to associate incentives with CPU models. It is a non-virtual method, and users do not need to reload it. Users need to bind the request port before sending incentives.

    **Type:** Function

    **Return value:** None

    | Parameter | Description                    |
    | --------- | ------------------------------ |
    | req_mlbx  | Incentive request port handle. |

- <kbd>request_process</kbd>

    **Function:** processes incentives. After receiving the data sent by incentives, the task generates BDs according to the rules, stores data to the local virtual memory, sends data to the `RM`, and generates the expected data processing result.

    **Parameter:** None

    **Type:** Task

    **Return value:** None

- <kbd>response_process</kbd>

    **Function:** returns requests. After receiving the read request sent by the `UL`, the task reads data from the local virtual memory according to the instructions in the BD, and then returns data to the `UL`.

    **Parameter:** None

    **Type:** Task

    **Return value:** None

- <kbd>user_process</kbd>

    **Function:** processes data sent by the `UL`. After receiving the write data and BDs from the `UL`, the task combines data with BDs, and then sends data to the `RM`.

    **Parameter:** None

    **Type:** Task

    **Return value:** None

##### cpu_model_cb Usage

First, you can reload three tasks in the `user_model_cb.svh` file as required to implement the functions.

For example:

```verilog
    class user_model_cb extends cpu_model_cb;
        ...
        / / This method processes incentives.
        task request_process();
            ...
        endtask : request_process
        / / This method returns requests.
        task cpu_model_cb::response_process();
            ...
        endtask : response_process
        / / This method processes other tasks.
        task cpu_model_cb::user_process();
            ...
        endtask : user_process
    endclass : user_model_cb
```

After modifying the model callback, associate the callback compiled by yourself with the CPU model. This step needs to be performed in basic test cases. For example:

```verilog
    ...
    // Instantiating user-defined components
    function void build();
        ...
        m_user_cb       = new("m_user_cb"  ); // Instantiating user callback
        ...
        super.build();
    endfunction : build
    // Connecting user-defined components
    function void connect();
        super.connect();
        ...
        / / Adding user callback to components
        m_tb_env.m_cpu_model.append_callback(m_user_cb);
    endfunction : connect
```

<a id="sec-4-4" name="sec-4-4"></a>

### **Simulation Platform Configurations**

---

<a id="sec-4-4-1" name="sec-4-4-1"></a>

#### Configuration File Format

Users transmit data to the Testbench through the configuration file. The format of the file is as follows:

```bash
    // Use '//' or '#' for comments, which will not be sent to Testbench.

    // The format of the parameter transmission is +xxx_name=yyyyy, where xxx_name is the parameter name, and yyyyy is the parameter content. (*Note: The two ends of '=' cannot have spaces.*)
    // The parameter content can be a decimal number (123, 456), a hexadecimal number ('hxxx), a string (abcd, "xxyyzz"), or a sequence.
    // If the sequence is a combination of multiple parameters, separate them with a comma ',' or 'semicolon'. (for example, 123,456,'h678,aaa)

    # TEST_NAME indicates the basic test corresponding to the test case.
    +TEST_NAME=tb_reg_test

    # DUMP_FSDB indicates whether the VERDI wave needs to be dumped.
    +DUM_FSDB=0

    ...
```

<a id="sec-4-4-2" name="sec-4-4-2"></a>

#### Adding Configuration Items

A configuration file contains many configuration items. The name of each item is defined in the user cfg. For example:

```verilog
    class tb_reg_cfg;
        int adder0;
        int adder1;
        int name;
        function new();
            // The first parameter of get_string is the name of the parameter in the configuration file, and the second parameter is the default value.
            name   = config_opt::get_string("NAME","noname");
            adder0 = config_opt::get_int("ADDER0", 'd0     );
            adder1 = config_opt::get_int("ADDER1", 'd0     );
        endfunction : new
    endclass : tb_reg_cfg
```

The configuration in the file is as follows:

```bash
    +NAME=TEST_NAME
    +ADDER0=123
    +ADDER1=456
```

<a id="sec-5" name="sec-5"></a>

## **Examples**

---

<a id="sec-5-1" name="sec-5-1"></a>

### Example 1

---

<a id="sec-5-1-1" name="sec-5-1-1"></a>

#### Example 1 Overview

This example implements user logic version reading, data inversion, and addition functions. You can refer to existing Huawei design components and use this example to learn about the development, simulation, verification, and test processes on the cloud.

<a id="sec-5-1-2" name="sec-5-1-2"></a>

#### Example 1 Architecture

<a id="sec-5-1-3" name="sec-5-1-3"></a>

#### Example 1 Operation Instructions

##### 1. Running Simulation in One-Click

You can run the **make** command in the **sim** directory and use the Vivado to compile and run the sv_demo_001 test case. For details, see [Compiling and Executing in One-Click](#4.2.2.4).
For example:

``` bash
  $ cd xxxx/sim
  $ make
  $ make TC=sv_demo_001
```

##### 2. Compiling Test Cases

You can create a folder in the sv or c subdirectory of **xxx/sim/tests** to create a test case. Store the test case in the c subdirectory only when you need to create a C-based test case.
Each test case requires a configuration file. For details about the format of the file, see [Configuration File Format](#sec-4-4-1).
In each test case, display the name of the basic test case for platform execution.
The default name of the basic test case is tb_reg_test. This test case is used to read and write a register.

##### 3. Functions of the Default Test Case

The default test case has the following functions:

- Read the logic version number and type.
- Check whether the version and type are correct.
- Check data and the address inversion register.
- Read two values from the configuration file and configure them for the logic.
- Check the addition result.

##### 4. Viewing Logs

The FACS stores all the logs generated during the compilation and simulation processes of the simulation platform. The detailed functions of the logs are as follows:

| Directory      | Log Name           | Description                              |
| -------------- | ------------------ | ---------------------------------------- |
| sim/report     | log_comp.log       | Pre-compilation logs of the simulation platform |
| sim/report     | log_elab.log       | Link logs of the simulation platform     |
| sim/report/xxx | log_simulation.log | Operation logs of test case xxx          |

*If a compilation error occurs on the simulation platform, check the pre-compilation logs or link logs. If the compilation is successful but the running fails, check the log_simulation.log.* **If the test case is executed successfully, PASS will be displayed at the end of the log. If not successful, FAIL will be displayed.**

For details, see [Simulation Platform Quick Start Guide](./quick_start.md).

<a id="sec-5-2" name="sec-5-2"></a>

### Example 2

---

<a id="sec-5-2-1" name="sec-5-2-1"></a>

#### Example 2 Overview

This example implements user logic version reading, data inversion, and addition functions. You can refer to existing Huawei design components and use this example to learn about the development, simulation, verification, and test processes on the cloud.

Example 2 consists of two test cases: `sv_demo_001` and `sv_demo_002`. The test case `sv_demo_001` covers the read and write access of user DDRs, while the `sv_demo_002` processes the user DMA data stream.

- `sv_demo_001`: Read and write the three external DDRs of the `ULs` (write before read). The write data is a `32bit` random number, and the write address is random (covering the rank 0 and rank 1 of DDRs). If the read data is inconsistent with the write data, an error will be reported. If consistent, **PASS** will be displayed.

- `sv_demo_002`: The incentive sends BDs to `UL` through the CPU model. After receiving BDs, the `UL` initiates a read data request to the CPU model, and then the CPU model returns the data to the `UL` after receiving the request. After receiving packets, the `UL` writes the packets into the CPU model. Then, the CPU model compares the received data with the expected result. If the result is inconsistent, an error will be reported. If consistent, **PASS** will be displayed.

<a id="sec-5-2-2" name="sec-5-2-2"></a>

#### Example 2 Architecture Overview

<a id="sec-5-2-3" name="sec-5-2-3"></a>

#### Example 2 Operation Instructions

The usage method of example 2 is the same as that of example 1. For details, see [Example 1 Operation Instructions](sec-5-1-3).

<a id="sec-6" name="sec-6"></a>

N/A

## **Appendix**


