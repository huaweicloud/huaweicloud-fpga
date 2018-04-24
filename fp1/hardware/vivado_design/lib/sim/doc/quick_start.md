# Simulation Platform Quick Start Guide

[切换到中文版](./quick_start_cn.md)

This document is a quick start guide to the FACS simulation platform. The beginning part is a brief introduction to the platform. Then, the document shows how to run existing examples. Finally, users are instructed on how to compile their own simulation components and test cases for simulation.

<div id="table-of-contents">
<h2>Contents</h2>
<div id="text-table-of-contents">
<ul>
<li><a href="#sec-1">1. <b>Simulation Platform</b></a></li>
<li><a href="#sec-2">2. <b>Configuring the Simulation Environment</b></a></li>
<li><a href="#sec-3">3. <b>Simulating Examples</b></a>
<ul>
<li><a href="#sec-3-1">3.1. <b>Compiling Simulation Examples</b></a></li>
</ul>
<ul>
<li><a href="#sec-3-2">3.2. <b>Running Simulation Examples</b></a></li>
</ul>
<ul>
<li><a href="#sec-3-3">3.3. <b>Debugging Simulation Examples</b></a></li>
</ul>
<ul>
<li><a href="#sec-3-4">3.4. <b>Simulating Examples in One-Click Mode</b></a></li>
</ul>
<ul>
<li><a href="#sec-3-5">3.5. <b>Clearing Simulation Results</b></a></li>
</ul>
<ul>
<li><a href="#sec-3-6">3.6. <b>Viewing Simulation Logs</b></a></li>
</ul>
</li>
<li><a href="#sec-4">4. <b>User-Defined Simulation</b></a>
<ul>
<li><a href="#sec-4-1">4.1. <b>Compiling User Test Cases</b></a>
<ul>
<li><a href="#sec-4-1-1">4.1.1. Creating a User Project</a></li>
</ul>
<ul>
<li><a href="#sec-4-1-2">4.1.2. Creating User Test Cases</a></li>
</ul>
<ul>
<li><a href="#sec-4-1-3">4.1.3. Modifying Simulation Configuration</a></li>
</ul>
<ul>
<li><a href="#sec-4-1-4">4.1.4. Compiling Basic Test Cases</a></li>
</ul>
<ul>
<li><a href="#sec-4-1-5">4.1.5. Compiling User Test Configuration Files</a></li>
</ul>
</li>
<li><a href="#sec-4-2">4.2. <b>Simulating User Test Cases</b></a>
</ul>
</li>
<li><a href="#sec-5">5. <b>User-Defined Components</b></a>
<ul>
<li><a href="#sec-5-1">5.1. <b>Compiling User-Defined Incentives</b></a>
<ul>
<li><a href="#sec-5-1-1">5.1.1. Creating User Incentives</a></li>
</ul>
<ul>
<li><a href="#sec-5-1-2">5.1.2. Modifying User Incentives</a></li>
</ul>
<ul>
<li><a href="#sec-5-1-3">5.1.3. Associating User Incentives</a></li>
</ul>
<ul>
<li><a href="#sec-5-1-4">5.1.4. Starting User Incentives</a></li>
</ul>
</li>
<li><a href="#sec-5-2">5.2. <b>User-Defined Configuration</b></a>
<ul>
<li><a href="#sec-5-2-1">5.2.1. Configuring User-Defined Incentives</a></li>
</ul>
<ul>
<li><a href="#sec-5-2-2">5.2.2. Configuring User-Defined Platform</a></li>
</ul>
</li>
<li><a href="#sec-5-3">5.3. <b>Compiling User CPU Model Callback</b></a>
<ul>
<li><a href="#sec-5-3-1">5.3.1. Creating User CPU Model Callback</a></li>
</ul>
<ul>
<li><a href="#sec-5-3-2">5.3.2. Modifying User CPU Model Callback</a></li>
</ul>
<ul>
<li><a href="#sec-5-3-3">5.3.3. Associating User CPU Model Callback</a></li>
</ul>
</li>
<li><a href="#sec-5-4">5.4. <b>Compiling User Reference Models</b></a>
</li>
<li><a href="#sec-5-5">5.5. <b>Creating and Connecting User-Defined Components in Test Cases</b></a></li>
</ul>
</li>
<li><a href="#sec-6">6. <b>VIP Integration</b></a></li>
</div>
</div>

<a id="sec-1" name="sec-1"></a>

## **Simulation Platform Overview**

---

The FACS simulation platform implements collaborative simulation of C/SystemVerilog hybrid languages. The FACS simulation platform separates test cases from the simulation platform completely. You can implement simulation without modifying the simulation platform by designing test cases.

The FACS simulation platform is developed based on the SystemVerilog language compliant with IEEE-1800 (2012) and does not use any verification methods. This enables the simulation platform to be executed under a Vivado, VCS, or QuestaSim simulator.

The following figure shows the simulation platform structure.

<img src="./images/testbench.png" alt="Simulation platform structure">

The architecture:

- Allows you to complete test cases without modifying the simulation platform.

- Allows you to easily integrate VIPs into the simulation platform.

- Achieves multiple executions with one compilation.

- Improves the incentive compilation efficiency by supporting incentives in multiple languages.

- Provides higher speed simulation experience through pre-compiled libraries and optimized simulation parameters.

- Resolves the tool restriction issue through the excellent code compatibility of the simulation platform.

<a id="sec-2" name="sec-2"></a>

## **Configuring the Simulation Environment**

---

Set environment variables each time a terminal is started to run the FACS simulation platform. The configuration methods are as follows:

If you are using the platform for the first time, modify the configuration file to complete the license setting by changing the **XILINX_LIC_SETUP** field in the `setup.cfg` file. 
If there are multiple licenses, use colons (:) to separate them.

```bash
  $ cd /home/fpga_design
  $ vi ./setup.cfg
  # License server setup {{{
  # Xilinx Vivado License Setup
  # Floating, Fix license are all supportted
  # If there is more than one license setup, please use ':' as splitor, such as :
  # XILINX_LIC_SETUP="2100@100.123.456.789:2100@100.123.789.456"
  # Default is empty
  XILINX_LIC_SETUP=""
  # }}}
```

Environment variables are configured as follows:

```bash
  $ source ./setup.sh
  ---------------------------------------------------
  Checking software infomation.......
  ---------------------------------------------------
  Checking software license.
  ---------------------------------------------------
  ...
```

The setting may take some time. For details, see [fp1 Development Suite Description](../../../../../README.md).

After the setting, the root directory of the project is automatically configured in the environment variable. This root directory is the directory of the `fpga_design` folder. The environment variable is `WORK_DIR`.

Run the following command to view environment variables:

```bash
  $ echo $WORK_DIR
```

<a id="sec-3" name="sec-3"></a>

## **Simulating Examples**

---

The FACS simulation platform provides a host of examples to help users better understand how to perform simulation. The compilation, execution, and debugging of the simulation platform are implemented through the **Makefile**. To execute the simulation, use either of the following methods:

- Standard make method: Before compiling, executing, and debugging a simulation example, switch to the simulation root directory where the example is stored, and then run the `make` command. The following table describes the simulation root directories.

    | Example   | Directory                                |
    | --------- | ---------------------------------------- |
    | Example 1 | `$WORK_DIR/hardware/vivado_design/examples/example1/sim` |
    | Example 2 | `$WORK_DIR/hardware/vivado_design/examples/example2/sim` |

    You can run the following commands to switch to the simulation root directory:

    ```bash
      $ export EXAMPLE_DIR=$WORK_DIR/hardware/vivado_desgin/examples/examplex
      $ cd $EXAMPLE_DIR/sim
    ```
    `examplex` indicates the example name, which can be `example 1` or `example 2`.

- Designated directory method: While compiling, executing, and debugging a simulation example, you can run the `make -C` command to designate a **Makefile** directory.

    You can run the following commands to perform the `make` operation in the designated directory:

    ```bash
      $ export EXAMPLE_DIR=$WORK_DIR/hardware/vivado_desgin/examples/examplex
      $ make -C $EXAMPLE_DIR/sim XXX # XXX is the Makefile target.
    ```

For details about `Makefile` parameters and targets, see [Simulation Platform User Guide](./user_guide.md).

<a id="sec-3-1" name="sec-3-1"></a>

### **Compiling Simulation Examples**

---

Run the `make comp` command to compile an example. The following is the default command:

```bash
  $ make comp
```

By default, Vivado is used as the simulator. To use the VCS simulator or QuestaSim simulator, run the following commands:

```bash
  $ make comp TOOL=vcs    # Compile Using vcsmx
  $ make comp TOOL=questa # Compile Using questasim
  $ make comp TOOL=vivado # Compile Using vivado(Same as do not specify the simulation tools)
```

<a id="sec-3-2" name="sec-3-2"></a>

### **Running Simulation Examples**

The command for running example simulation is `make run`. You need to specify the name of the test case. The following is the command for executing the test case **sv_demo_001**. (**sv_demo_001** is the default name of the test case. You can omit this name for running this test case.)

```bash
  $ make run TC=sv_demo_001
```

The parameter after `TC` is the name of the test case to be executed. It must be the same as the folder name of the test case in the `$EXAMPLE_DIR/sim/tests/sv/` directory.

By default, Vivado is used as the simulator. To use the VCS simulator or QuestaSim simulator, run the following commands:

```bash
  $ make run TOOL=vcs TC=sv_demo_001 # Compile Using vcsmx
  $ make run TOOL=questa TC=sv_demo_001 # Compile Using questasim
  $ make run TOOL=vivado TC=sv_demo_001 # Compile Using vivado(Same as do not specify the simulation tools)
```

<a id="sec-3-3" name="sec-3-3"></a>

### **Debugging Simulation Examples**

Run the `make wave` command to debug an example. Parameters are similar to the parameters used for running a simulation example. You need to specify the name of the test case. The following command is used to debug the **sv_demo_001** test case:

```bash
  $ make wave TC=sv_demo_001
```

By default, Vivado is used for debugging. If you need to use DVE or QuestaSim, run the following commands:

```bash
  $ make wave TOOL=vcs TC=sv_demo_001    # Compile Using vcsmx
  $ make wave TOOL=questa TC=sv_demo_001 # Compile Using questasim
  $ make wave TOOL=vivado TC=sv_demo_001 # Compile Using vivado(Same as do not specify the simulation tools)
```

<a id="sec-3-4" name="sec-3-4"></a>

### **Simulating Examples in One-Click Mode**

Examples support one-click compilation and running. Run the following command (The value **all** can be omitted.):

```bash
  $ make all
```

VCS and QuestaSim also support one-click running. For details, see description in this section.

<a id="sec-3-5" name="sec-3-5"></a>

### **Clearing Simulation Results**

During the compilation or execution of test cases, some simulation intermediate files are generated in the `work` directory. You are advised to clear these files before each compilation by running the following command:

```bash
  $ make clean
```

If you need to clear **pre-compiled library files** when clearing simulation intermediate files, run the following command:

```bash
  $ make distclean
```

<a id="sec-3-6" name="sec-3-6"></a>

### **Viewing Simulation Logs**

If the compilation of the simulation fails, check the **log_comp.log** file in the `report` directory.

```bash
  $ vi ./report/log_comp.log
```

If the compilation is successful but an error is reported during the simulation, go to the corresponding test case directory and view **log_simulation.log** to locate the fault. *(test_xxx indicates the name of the test case to be viewed by the user.)*

```bash
  $ vi ./report/sv_test_xxx/log_simulation.log
```

<a id="sec-4" name="sec-4"></a>

## **User-Defined Simulation**

---

You can execute examples, or compile, run, and debug your own test cases.

<a id="sec-4-1" name="sec-4-1"></a>

### **Compiling User Test Cases**

---

To compile your own test cases, perform the following steps:

- 1 [Creating a User Project](#sec-4-1-1) *(Ignore this step if the project already exists.)*

- 2 [Creating User Test Cases](#sec-4-1-2)

- 3 [Modifying Simulation Configuration](#sec-4-1-3)

- 4 [Compiling Basic Test Cases](#sec-4-1-4)

- 5 [Compiling User Test Configuration Files](#sec-4-1-5)

<a id="sec-4-1-1" name="sec-4-1-1"></a>

#### Creating a User Project

To compile your own test cases, create a project by copying the **sim** folder in the `example` or `template` folder to the user directory. For example:

```bash
  $ export USER_DIR=$/WORK_DIR/hardware/vivado_desgin/user/user_xxx
  $ cd $USER_DIR
  $ cp -rf ../../lib/template/sim ./
  $ cd ./sim
```

You can also run the `creat_prj.sh` command in the user directory to create a user simulation folder. For example:

```bash
  $ cd $WORK_DIR/hardware/vivado_desgin/user
  $ sh ./create_prj.sh user_pri_name
  $ cd ./user_pri_name/sim
```

For details about the parameters of the `create_prj.sh` command, see [usr_template User Guide](../../template/README.md).

<a id="sec-4-1-2" name="sec-4-1-2"></a>

#### Creating User Test Cases

A project contains the simulation folder. The directory of the folder is as follows:

```bash
    sim/
    |-- common/                  # Common files of testbench
    |-- libs/                    # User lib files
    |-- tests/                   # User Testcases
        |-- sv/                  # Sv Testcases
            |--- base/           # Base Testcase
            |--- xxx_test/       # User Testcase xxx
        |-- c/                   # C Testcase
    |-- scripts/                 # User Scripts
    |-- work/                    # Sim Work Dir
    |-- report/                  # Log/Report
    |-- wave/                    # Wave
    |-- doc/                     # Document
    |-- Makefile
```

Create your own test cases. Ensure that the name of each test case is the same as that of the folder you created. You can copy the existing test cases in the example folder as your own test cases or create new ones.

```bash
  $ cd ./tests/sv
  $ mkdir xxx_test                  # Create Testcase Directory
  $ cp -r ./sv_demo_001/* xxx_test/ # Copy Example to Own Testcase
```

The user test cases are divided into two parts: **basic test cases** and **user test configurations**.
Basic test cases are compiled using the SystemVerilog language and are used to complete the main process of test cases.
Incentive and configuration parameters are obtained from these user test configuration files.

<a id="sec-4-1-3" name="sec-4-1-3"></a>

#### Modifying Simulation Configuration

After creating test case directories, modify the `project_settings.cfg` file in the `scripts` directory by designating user-defined simulation macros and library files.
**USER_LIBS** and **SIM_MACRO** are user-defined library files and simulation macros. The two parameters are not required if there are no files or macros.

```bash
  $ vi ./scripts/project_settings.cfg
  USER_LIBS="
  # '#' means comments
  # Example:
  # FILE_PATH1/FILE_NAME1
  # FILE_PATH2/FILE_NAME2
  "
  SIM_MACRO="
  # '#' means comments
  # Example:
  # MACRO1
  # MACRO2
  "
```

<a id="sec-4-1-4" name="sec-4-1-4"></a>

#### Compiling Basic Test Cases

Basic test cases are compiled using the SystemVerilog language, and are used to complete the main process of test cases and the instantiation and connection of user-defined components.

The compilation of basic test cases must comply with the following rules. For details, see [Simulation Platform User Guide](./user_guide.md.).

- Basic test cases must be inherited from `tb_test` or its subcategories.

- Debug and display the macro `tb_register_test` to register test cases.

- Debug and display the `new` method of the parent category for the new in basic test cases.

You are advised to place the main part of the test in the `run` task.

- To check results, use the `tb_error` macro to report the error (if any). Cases will fail if there are errors reported with this macro.

Compile basic test cases in the following way:

```verilog
    class tb_reg_test extends tb_test;
        // Register tb_reg_test into test_top
        `tb_register_test(tb_reg_test)

        function new(string name = "tb_reg_test");
            super.new(name);
            ...
        endfunction : new

        task run();
            ...
            // ----------------------------------------
            // STEP1: Check version
            // ----------------------------------------
            `tb_info(m_inst_name, {"\n----------------------------------------\n",
                                " STEP1: Checking DUV Infomation\n",
                                "----------------------------------------\n"})
            m_tb_env.m_reg_gen.read(g_reg_ver_time, ver_time);
            m_tb_env.m_reg_gen.read(g_reg_ver_type, ver_type);
            $sformat(info, {"+-------------------------------+\n",
                            "|    DEMO version : %08x    |\n",
                            "|    DEMO type    : %08x    |\n",
                            "+-------------------------------+"}, ver_time, ver_type);
            `tb_info(m_inst_name, info)
            check = (ver_type == 'h00d10001);
            $sformat(info, {"+-------------------------------+\n",
                            "|    Demo Check   : %s        |\n",
                            "+-------------------------------+"}, check ? "PASS" : "FAIL");
            if (!check) begin
                $sformat(info, "%s\n\nDetail info: Type of Demo1 should be 0x00d20001 but get 0x%x!\n",
                        info, ver_type);
                `tb_error(m_inst_name, info)
                return;
            end else begin
                `tb_info(m_inst_name, info)
            end
            #10ns;

            // ----------------------------------------
            // STEP2: Test register
            // ----------------------------------------
            ...
            $display("\nTestcase PASSED!\n");
        endtask : run

    endclass : tb_reg_test
```

<a id="sec-4-1-5" name="sec-4-1-5"></a>

#### **Compiling User Test Configuration Files**

User test configurations are used to determine the incentive and configuration data in the test cases, and are compiled in the same way as configuring files. The configuration file format is as follows:

```bash
  // Use '//' or '' '#' to for comments, which will not be sent to the Testbench.

  // The format of the parameter transmission is +xxx_name=yyyyy, where xxx_name is the parameter name, and yyyyy is the parameter content. (*Note: The two ends of '=' cannot have spaces.*)
  // The parameter content can be a decimal number (123, 456), a hexadecimal number ('hxxx), a string (abcd, "xxyyzz"), or a sequence.
  // If the sequence is a combination of multiple parameters, separate them with a comma ',' or 'semicolon'. (for example, 123,456,'h678, aaa)

  # TEST_NAME indicates the basic test corresponding to the test case.
  +TEST_NAME=tb_reg_test

  # DUMP_FSDB indicates whether the VERDI wave needs to be dumped.
  +DUM_FSDB=0

  ...
```

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

The configuration items in the configuration file are as follows:

```bash
  +NAME=TEST_NAME
  +ADDER0=123
  +ADDER1=456
```

<a id="sec-4-2" name="sec-4-2"></a>

### **Simulating User Test Cases**

The method of simulating user test cases is similar to that of simulating an example. You only need to modify the simulation root directory. For details, see [Simulating Examples](#sec-3-1).

<a id="sec-5" name="sec-5"></a>

## **User-Defined Components**

---

You need to customize complex incentive and reference models. The following components can be customized:

- [Incentives](#sec-5-1)
- [Configuration](#sec-5-2)
- [CPU Models](#sec-5-3)
- [Reference Models](#sec-5-4)

<a id="sec-5-1" name="sec-5-1"></a>

### **User-Defined Incentives**

---

User incentives are divided into three parts: **incentive generating method**, **incentive generator**, and **incentive configuration**, as shown in the following figure.	

<img src="./images/stim.png" alt="Incentive component structure">

The incentive generator does not include the method of generating incentives. Therefore, you do not need to modify it. If you need to customize incentives, modify the incentive generating method and configuration.

If you need to define the method for generating an incentive, perform the following three steps:

<a id="sec-5-1-1" name="sec-5-1-1"></a>

#### Creating User Incentives

User incentives can be stored in `common` of the `$USER_DIR/sim` folder or the `base` folder in the user `testcase` directory.

User incentives must be inherited from `axi_stims.sv` in the `$LIB_DIR/sim/bench/stim` folder. Therefore, you are advised to copy the file to the folder. For example:

```bash
  $ cp $LIB_DIR/sim/bench/stim/axi_stims.sv $USER_DIR/sim/common/user_stim.sv
```

or

```bash
  $ cp $LIB_DIR/sim/bench/stim/axi_stims.sv $USER_DIR/sim/tests/sv/base/user_stim.sv
```

<a id="sec-5-1-2" name="sec-5-1-2"></a>

#### Modifying User Incentives

Modify the `user_stim.sv` file as required.

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
        // If the Vivado is used as the simulator, code can be deleted.
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
            // I was no idea about why randomize can not be success when using vivado simulator, so I had to commont all randomize.
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

<a id="sec-5-1-3" name="sec-5-1-3"></a>

#### Associating User Incentives

Associate modified incentives with the incentive generator to generate incentives. This step must be performed in basic test cases. For example:

Users can edit `tb_test_user` (a basic test case of the user), and create and associate user incentives in `build` and `connect` methods. For details, see [Creating and Connecting User-Defined Components in Test Cases](#sec-5-5).

<a id="sec-5-1-4" name="sec-5-1-4"></a>

#### Starting User Incentives

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

<a id="sec-5-2" name="sec-5-2"></a>

### **User-Defined Configuration**

---

The user-defined configuration can be stored in `common` of the `$USER_DIR/sim` folder or the `base` folder in the user `testcase` directory.

The user-defined configuration includes **user-defined incentive configurations** and **user-defined platform configurations**.

<a id="sec-5-2-1" name="sec-5-2-1"></a>

#### User-Defined Incentives

Configurations must be inherited from `axi_stim_cfg.svh` in the `$LIB_DIR/sim/bench/stim` folder. You are advised to copy the file to the folder. For example:

```bash
  $ cp $LIB_DIR/sim/bench/stim/axi_stim_cfg.svh $USER_DIR/sim/common/user_stim_cfg.svh
```

or

```bash
  $ cp $LIB_DIR/sim/bench/stim/axi_stim_cfg.svh $USER_DIR/sim/tests/sv/base/user_stim_cfg.svh
```

Then, modify the `user_stim_cfg.svh` file as required. For example:

```verilog
    class user_stim_cfg extends axi_stim_cfg;
        //User-defined variables
        bit [63 : 0]      axi_addr_min  ;  // Address low range
        bit [63 : 0]      axi_addr_max  ;  // Address max range
        int               axi_data_len  ;  // Data length
        ...
        function new();
            // Obtain the content from the configuration file.
            axi_addr_min  = config_opt#(64)::get_bits("axi_addr_min"  );
            axi_addr_max  = config_opt#(64)::get_bits("axi_addr_max"  );
            axi_data_len  = config_opt#(32)::get_bits("axi_data_len"  );
            ...
        endfunction : new
    endclass : user_stim_cfg
```

Finally, associate incentives compiled by yourself to the corresponding incentive to generate incentives. This step must be performed in basic test cases.

Users can edit `tb_test_user` (a basic test case of the user), and create and associate user incentives in `build` and `connect` methods. For details, see [Creating and Connecting User-Defined Components in Test Cases](#sec-5-5).

<a id="sec-5-2-2" name="sec-5-2-2"></a>

#### Configuring the User-Defined Platform

There is no inheritance restriction for user-defined platform configurations. Create a new file in the following folder. For example:

```bash
  $ touch -f $USER_DIR/sim/common/user_tb_cfg.svh
```

Then, modify the `user_tb_cfg.svh` file as required. For example:

```verilog
    class user_tb_cfg;
        int user0;
        int user1;
        function new();
            user0 = config_opt#(32)::get_bits("USER0");
            user1 = config_opt#(32)::get_bits("USER1");
        endfunction : new
    endclass : user_tb_cfg
```

Finally, associate incentives compiled by yourself to the corresponding incentive to generate incentives. For details, see [Compiling User-Defined Incentive Configuration](#5-2-1).

<a id="sec-5-3" name="sec-5-3"></a>

### **Compiling Callback of the User CPU Model**

---

The CPU model is used to simulate behaviors of the CPU and `SHELL` and interact with the `UL` according to predefined rules. The CPU model consists of two parts: **CPU model ** and **CPU model callback**, as shown in the following figure.

<img src="./images/model.png" alt="CPU Model Components Diagram">

The CPU model does not include any interaction implementation and provides only interfaces to other components. Therefore, if you need to customize behaviors of the CPU model, customize its callback function.

To compile the CPU model callback, perform the following three steps:

<a id="sec-5-3-1" name="sec-5-3-1"></a>

#### **Creating Model Callback of the User CPU**

The model callback of the user CPU can be stored in `common` of the `$USER_DIR/sim` folder or the `base` folder in the user `testcase` directory.

The model callback of the user CPU must be inherited from `cpu_model_cb.svh` in the `$LIB_DIR/sim/bench/rm` folder. Therefore, you are advised to copy the file to the folder. For example:

```bash
  $ cp $LIB_DIR/sim/bench/rm/cpu_model_cb.svh $USER_DIR/sim/common/user_model_cb.svh
```

or

```bash
  $ cp $LIB_DIR/sim/bench/rm/cpu_model_cb.svh $USER_DIR/sim/tests/sv/base/user_model_cb.svh
```

<a id="sec-5-3-2" name="sec-5-3-2"></a>

#### **Modifying Model Callback of the User CPU**

The CPU model callback module provides three tasks for users to reload. The three tasks are as follows:

- request_process

    Processes incentives. After receiving the data sent by incentives, the task generates BDs according to the rules, stores data to the local virtual memory, and then sends data to the `RM`.

- user_process

    Returns requests. After receiving the read request sent by the `UL`, the task reads data from the local virtual memory according to the instructions in the BD, and then returns data to the `UL`.

- response_process

    Processes data sent by the `UL`. After receiving the write data and BDs from the `UL`, the task combines data with BDs, and then sends data to the `RM`.

You can reload three tasks in the `user_model_cb.svh` file as required.

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

<a id="sec-5-3-3" name="sec-5-3-3"></a>

#### **Associating Model Callback of the User CPU**

After modifying the model callback, associate the callback compiled by yourself with the CPU model. This step must be performed in basic test cases.

Users can edit `tb_test_user` (a basic test case of the user), and create and associate user incentives in `build` and `connect` methods. For details, see [Creating and Connecting User-Defined Components in Test Cases](#sec-5-5).

<a id="sec-5-4" name="sec-5-4"></a>

### **Compiling User Reference Models**

---

The user reference models are used to predict the user data and check the output user data.

To customize a reference model (RM), create a reference model first. The RM can be stored in `common` of the `$USER_DIR/sim` folder or the `base` folder in the `testcase` directory.

User reference models (RMs) must be inherited from `axi_rm.sv` in the `$LIB_DIR/sim/bench/rm` folder. Therefore, you are advised to copy the file to the folder. For example:

```bash
  $ cp $LIB_DIR/sim/bench/rm/axi_rm.sv $USER_DIR/sim/common/user_rm.sv
```

or

```bash
  $ cp  $LIB_DIR/sim/bench/rm/axi_rm.sv $USER_DIR/sim/tests/sv/base/user_rm.sv
```

The RM module provides two functions for users to reload. The two functions are as follows:

- insert:

    Completes the expectation of the incentive data.

- check

    Processes and compares returned data.

Modify the `user_rm.sv` file as required. As the `axi_rm.sv` has the scoreboard function, you are advised to modify components as follows:

- 1 The user-defined RM must be inherited from `axi_rm.sv`.
- 2 In the `insert` and `check` functions reloaded by users, it is recommended that the parent class be explicitly invoked in the last phase to complete the expectation and comparison of the scoreboard.

For example:

```verilog
    class user_rm extends axi_rm;
        ...
        // This method completes the expectation of the incentive part.
        function void insert(ref DATA data);
            ...
        endfunction : insert
        // This method checks responses.
        function void check(ref DATA data);
            ...
        endfunction : check
    endclass : user_rm
```

Finally, connect the RM compiled by yourself to other components. This step must be performed in basic test cases. For example:

Users can edit `tb_test_user` (a basic test case of the user), and create and associate the RM in `build` and `connect` methods. For details, see [Creating and Connecting User-Defined Components in Test Cases](#sec-5-5).

<a id="sec-5-5" name="sec-5-5"></a>

### **Creating and Connecting User-Defined Components in Test Cases**

---

If you have compiled a customized RM, incentives, or other callback, instantiating and connecting the callback before using it. The instantiation and connection methods are as follows:

```verilog
    ...
    // Instantiating user-defined components
    function void build();
        m_user_tb_cfg   = new();
        m_user_stim_cfg = new();
        m_user_cb       = new("m_user_cb"  ); // Instantiating user callback
        m_user_stim     = new("m_user_stim"); // Instantiating user incentives
        m_user_rm       = new("m_user_rm"  ); // Instantiating the RM
        super.build();
    endfunction : build
    // Connecting user-defined components
    function void connect();
        super.connect();
        // Associating incentive configurations with incentive methods.
        m_user_stim.set_cfg(m_user_stim_cfg);
        / / Connecting the RM
        m_user_cb.m_rm = m_user_rm;
        // Associating user incentives with incentive generators
        m_tb_env.m_axi_gen.reg_stims(m_user_stim);
        / / Adding user callback to components
        m_tb_env.m_cpu_model.append_callback(m_user_cb);
    endfunction : connect
```

<a id="sec-6" name="sec-6"></a>

## **Integrating VIPs **

---

**Verification Intellectual Property** (*VIP*) are verification components designed for users to verify complex protocols or functions.

Place VIPs in the `$LIB_DIR/sim/vip` directory or the `$USER_DIR/sim/lib` directory. The simulation platform automatically compiles code in either of the two directories. If the integrated VIPs have independent macro or simulation options, add them to the customized options in the `project_setting.cfg` file.

By default, the simulation platform uses the **DDR4 simulation model ** and **DDR4 RDIMM** simulation model of the Xilinx. By default, the two simulation models are not included in the VIP directory of the simulation platform. After you run the environment setting script `setup.sh`, the two VIP directories are automatically generated by invoking the interface of the Vivado.

Note: After the simulation platform automatically generates the two VIPs by invoking the Vivado interface, **a part of code is modified** by running the **setup.sh** script. **Do not modify** the two VIPs without permissions.
