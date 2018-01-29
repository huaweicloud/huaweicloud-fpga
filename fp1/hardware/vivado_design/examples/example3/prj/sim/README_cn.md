# Example 3 Simulation User Guide

<div id="table-of-contents">
<h2>Contents</h2>
<div id="text-table-of-contents">
<ul>
<li><a href="#sec-1">1. <b>Executing Compilation and Simulation of Example 3</b></a>
<ul>
<li><a href="#sec-1-1">1.1. <b>Compiling</b></a></li>
</ul>
<ul>
<li><a href="#sec-1-2">1.2. <b>Executing Simulation</b></a></li>
</ul>
<ul>
<li><a href="#sec-1-3">1.3. <b>Debugging</b></a></li>
</ul>
<ul>
<li><a href="#sec-1-4">1.4. <b>One-Click Execution</b></a></li>
</ul>
<ul>
<li><a href="#sec-1-5">1.5. <b>Clearing</b></a></li>
</ul>
<ul>
<li><a href="#sec-1-6">1.6. <b>Viewing Logs</b></a></li>
</ul>
<ul>
<li><a href="#sec-1-7">1.7. <b>Test Case Descriptions</b></a>
<ul>
<li><a href="#sec-1-7-1">1.7.1. Test Case sv_demo_001 Descriptions</a></li>
</ul>
<ul>
<li><a href="#sec-1-7-2">1.7.2. Test Case sv_demo_002 Descriptions</a></li>
</ul>
</li>
</ul>
</li>
<li><a href="#sec-2">2. <b>User-defined Tests</b></a>
<ul>
<li><a href="#sec-2-1">2.1. <b>Compiling User Test Cases</b></a>
<ul>
<li><a href="#sec-2-1-1">2.1.1. Compiling Basic Test Cases</a></li>
</ul>
<ul>
<li><a href="#sec-2-1-2">2.1.2. Compiling User Test Configurations</a></li>
</ul>
</li>
<li><a href="#sec-2-2">2.2. <b>Executing User Test Cases</b></a>
</ul>
</li>
</div>
</div>

<a id="sec-1" name="sec-1"></a>

## **Executing Compilation and Simulation of Example 3**

The compilation, running, and debugging of Example 3 are implemented through Makefile. Before compiling, simulating, and debugging test cases, switch to the **simulation root directory**.Run the following command to switch to the simulation root directory. (Unless otherwise specified, perform the following operations in the **simulation root directory**.)

```bash
    $ cd $WORK_DIR/hardware/vivado_desgin/examples/example3/sim
```

<a id="sec-1-1" name="sec-1-1"></a>

### **Compiling**

Run the `make comp` command to compile an example. The following is the command for compiling example 3:

```bash
    $ make comp
```

By default, Vivado is used as the simulator. To use the VCS simulator or QuestaSim simulator, run the following command: [1][1]

```bash
    $ make comp TOOL=vcs # Compile Using vcsmx
    $ make comp TOOL=questa # Compile Using questasim
    $ make comp TOOL=vivado # Compile Using vivado(Same as do not specify the simulation tools)
```

**For details about make parameters**, see [user_guide] (../../../lib/sim/doc/user_guide.md).

<a id="sec-1-2" name="sec-1-2"></a>

### **Executing Simulation**

Run the `make run` command to perform the example simulation. Specify the name of the test case. The following command is used to execute the **sv_demo_001** test case of example 3:

```bash
    $ make run TC=sv_demo_001
```

By default, Vivado is used as the simulator. To use the VCS simulator or QuestaSim simulator, run the following command:

```bash
    $ make run TOOL=vcs TC=sv_demo_001 # Compile Using vcsmx
    $ make run TOOL=questa TC=sv_demo_001 # Compile Using questasim
    $ make run TOOL=vivado TC=sv_demo_001 # Compile Using vivado(Same as do not specify the simulation tools)
```

<a id="sec-1-3" name="sec-1-3"></a>

### **Debugging**

Run the `make wave` command to debug an example. Specify the name of the test case. The following command is used to debug the **sv_demo_001** test case of example 3:

```bash
    $ make wave TC=sv_demo_001
```

By default, Vivado is used for debugging. If you need to use DVE or QuestaSim, run the following command:

```bash
    $ make wave TOOL=vcs TC=sv_demo_001 # Compile Using vcsmx
    $ make wave TOOL=questa TC=sv_demo_001 # Compile Using questasim
    $ make wave TOOL=vivado TC=sv_demo_001 # Compile Using vivado(Same as do not specify the simulation tools)
```

<a id="sec-1-4" name="sec-1-4"></a>

### **One-Click Execution**

Examples support one-click compilation and running. Run the following command:

```bash
    $ make TOOL=vcs TC=sv_demo_001
```

If users use the Vivado simulator to execute the test case `sv_demo_001`, the parameters after the `make` command can be omitted. For example:

```bash
    $ make
```

The one-click execution also supports VCS and QuestaSim simulators. For details, see the preceding sections.

<a id="sec-1-5" name="sec-1-5"></a>

### **Clearing**

When executing test cases again, users can delete the previous compilation or simulation results. The command is as follows:

```bash
    $ make clean
```

<a id="sec-1-6" name="sec-1-6"></a>

### **Viewing Logs**

If errors occur during the simulation compilation, you can view the **log_comp.log** file in the report directory. Errors occurred during the compilation are marked with the keyword `ERROR` in the log. The command is as follows:

```bash
    $ vi ./report/log_comp.log
```

If the compilation is successful but execution errors occur, you can enter the corresponding test case directory and run the **log_simulation.log** command to view the simulation running logs. Errors occurred during the simulation process are marked with the keyword `[ERROR]:`. The command is as follows:

```bash
    $ vi ./report/sv_demo_001/log_simulation.log
```

<a id="sec-1-7" name="sec-1-7"></a>

### **Test Case Descriptions**

Example 3 contains two test cases: `sv_demo_001` and `sv_demo_002`.

The two test cases have the following functions:

1. Read the `version` of the UL register.
2. Check the `data inversion` of the UL register.

The definition of the register is stored in the `./common/common_reg.svh` file.

<a id="sec-1-7-1" name="sec-1-7-1"></a>

#### **Test Case sv_demo_001 Descriptions**

The test case `sv_demo_001` reads the version register, checks the test register, and ** reads/writes the three external DDR interfaces of the UL.

    Details are as follows:
    sv_demo_001 delivers read/write operations to the three external DDR interfaces of the UL in sequence.
    Then, it is determined whether the read content is equal to the written content.
    If they are the same, PASS is displayed. If not, FAIL is displayed and the simulation ends.

<a id="sec-1-7-2" name="sec-1-7-2"></a>

### **Test Case sv_demo_002 Descriptions**

The test case `sv_demo_002` reads the version register, checks the test register, and performs the **DMA test** for the UL.

    Details are as follows:
    The test case sv_demo_002 builds packets and BDs through the simulation platform and sends them to the UL through the AXI4-Stream interface connected to the UL.
    After the UL processes packets, the packets are returned to the simulation platform without any changes and BDs are generated.
    The simulation platform compares packets sent and received. If they are the same, PASS is displayed. If not, FAIL is displayed and the simulation ends.

<a id="sec-2" name="sec-2"></a>

## **User-defined Tests**

<a id="sec-2-1" name="sec-2-1"></a>

### **Compiling User Test Cases**

The directory of the example simulation folder is as follows:

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
    |-- doc/
    |-- Makefile
```

Users need to create a test case for themselves. The name of the test case must be the same as the name of the folder created by the user. Users can copy the existing test cases in the example or create new ones.

```bash
    $ mkdir ./tests/sv/xxx_test                          # Create Testcase Directory
    $ touch ./tests/sv/base/xx_test.sv                   # Create Base Testcase
    $ cp -r ./tests/sv/sv_demo_001/* ./tests/sv/xxx_test # Copy Example to Own Testcase
```

The user test cases are divided into two parts: ** basic test cases ** and ** user test configuration **. The former is compiled using the SystemVerilog language and are used to complete the main process of test cases, and the latter is the configuration file of the user, which determines the data such as the incentive and configuration required in the test case.

<a id="sec-2-1-1" name="sec-2-1-1"></a>

#### **Compiling Basic Test Cases**

Skip this section if you need to modify only the incentive content without modifying the test process.
It is recommended that basic test cases be stored in the `./tests/sv/base` directory and named `xxx_test.sv`. Basic test cases can be compiled in the following way:

```verilog
    class xxx_test extends tb_test;
        // Register xxx_test into test_top
        `tb_register_test(xxx_test)

        function new(string name = "xxx_test");
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

    endclass : xxx_test
```

<a id="sec-2-1-2" name="sec-2-1-2"></a>

#### **Compiling User Test Configurations**

User test configurations are used to determine the incentive and configuration data in the test cases, and are compiled by using the parameters in configuration files. It is recommended that user configuration files be stored in the `./tests/sv/xxx_test` directory and named `test.cfg`. The format is as follows:

```bash
    // Use '//' or '#' to for comment, which will not be sent to Testbench.

    // The format of the parameter transfer is +xxx_name=yyyyy, where xxx_name is the parameter name, and yyyyy is the parameter content (*Note: There are no spaces at either end of ‘=’.*)
    // The parameter content can be a decimal number (123, 456), a hexadecimal number ('hxxx), a string (abcd, "xxyyzz"), and a sequence.
    // If the sequence is a combination of multiple parameters, separate them with a comma or 'semicolon'. (For example, 123,456,'h678,aaa)

    # TEST_NAME indicates the basic test corresponding to the basic test cases.
    +TEST_NAME=tb_reg_test

    # DUMP_FSDB indicates whether the DUMP VERDI waveform is used.
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

**For details about how to modify the incentive components and CPU model, see **[quick_start](../../../lib/sim/doc/quick_start.md).

<a id="sec-2-2" name="sec-2-2"></a>

### **Executing User Test Cases**

To compile and execute the test case `xxx_test`, run the following command:

```bash
    $ make TC=xxx_test              # Run testcase xxx_test，Compile Using vivado
    $ make TC=xxx_test TOOL=vcs     # Run testcase xxx_test，Compile Using vcsmx
    $ make TC=xxx_test TOOL=questa  # Run testcase xxx_test，Compile Using questasim
```

[1]: "Users need to install VCS and QuestaSim tools by themselves."

