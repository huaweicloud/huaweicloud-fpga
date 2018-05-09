# Example3仿真用户指南

[Switch to the English version](./README.md)

<div id="table-of-contents">
<h2>目录</h2>
<div id="text-table-of-contents">
<ul>
<li><a href="#sec-1">1. <b>执行Example3的编译与仿真</b></a>
<ul>
<li><a href="#sec-1-1">1.1. <b>编译</b></a></li>
</ul>
<ul>
<li><a href="#sec-1-2">1.2. <b>执行仿真</b></a></li>
</ul>
<ul>
<li><a href="#sec-1-3">1.3. <b>调试</b></a></li>
</ul>
<ul>
<li><a href="#sec-1-4">1.4. <b>一键式执行</b></a></li>
</ul>
<ul>
<li><a href="#sec-1-5">1.5. <b>清理</b></a></li>
</ul>
<ul>
<li><a href="#sec-1-6">1.6. <b>查看日志</b></a></li>
</ul>
<ul>
<li><a href="#sec-1-7">1.7. <b>测试用例说明</b></a>
<ul>
<li><a href="#sec-1-7-1">1.7.1. 测试用例sv_demo_001说明</a></li>
</ul>
<ul>
<li><a href="#sec-1-7-2">1.7.2. 测试用例sv_demo_002说明</a></li>
</ul>
<ul>
<li><a href="#sec-1-7-3">1.7.3. 测试用例sv_demo_003说明</a></li>
</ul>
</li>
</ul>
</li>
<li><a href="#sec-2">2. <b>用户自定义测试</b></a>
<ul>
<li><a href="#sec-2-1">2.1. <b>编写用户测试用例</b></a>
<ul>
<li><a href="#sec-2-1-1">2.1.1. 编写基础测试用例</a></li>
</ul>
<ul>
<li><a href="#sec-2-1-2">2.1.2. 编写用户测试配置</a></li>
</ul>
</li>
<li><a href="#sec-2-2">2.2. <b>执行用户测试用例</b></a>
</ul>
</li>
</div>
</div>

<a id="sec-1" name="sec-1"></a>

## **执行Example3的编译与仿真**

Example3的编译、运行以及调试均通过Makefile实现。在编译、仿真以及调试测试用例前请先切换至**仿真根目录**。可通过以下命令切换至仿真根目录（如无特殊说明后续所有操作请在**仿真根目录**执行）：

```bash
    $ cd $WORK_DIR/hardware/vivado_desgin/examples/example3/sim
```

<a id="sec-1-1" name="sec-1-1"></a>

### **编译**

编译Example的命令为`make comp`，以下为编译example3的命令：

```bash
    $ make comp
```

默认采用vivado作为仿真器，如果用户需要使用vcs仿真器或questasim仿真器，可使用如下命令：[1][1]

```bash
    $ make comp TOOL=vcs # Compile Using vcsmx
    $ make comp TOOL=questa # Compile Using questasim
    $ make comp TOOL=vivado # Compile Using vivado(Same as do not specify the simulation tools)
```

**make的详细参数请参考**[user_guide](../../../lib/sim/doc/user_guide_cn.md)。

<a id="sec-1-2" name="sec-1-2"></a>

### **执行仿真**

执行Example仿真的命令为`make run`，需要指定测试用例名，以下为执行example3的**sv_demo_001**测试用例的命令：

```bash
    $ make run TC=sv_demo_001
```

默认采用vivado作为仿真器，如果用户需要使用vcs仿真器或questasim仿真器，可使用如下命令：

```bash
    $ make run TOOL=vcs TC=sv_demo_001 # Compile Using vcsmx
    $ make run TOOL=questa TC=sv_demo_001 # Compile Using questasim
    $ make run TOOL=vivado TC=sv_demo_001 # Compile Using vivado(Same as do not specify the simulation tools)
```

<a id="sec-1-3" name="sec-1-3"></a>

### **调试**

调试Example的命令为`make wave`，需要指定测试用例名，以下为调试example3的**sv_demo_001**测试用例的命令：

```bash
    $ make wave TC=sv_demo_001
```

默认采用vivado进行调试，如果用户需要使用dve或questasim进行调试，可使用如下命令：

```bash
    $ make wave TOOL=vcs TC=sv_demo_001 # Compile Using vcsmx
    $ make wave TOOL=questa TC=sv_demo_001 # Compile Using questasim
    $ make wave TOOL=vivado TC=sv_demo_001 # Compile Using vivado(Same as do not specify the simulation tools)
```

<a id="sec-1-4" name="sec-1-4"></a>

### **一键式执行**

Example支持一键式运行，即一键式自动完成编译以及运行，可使用如下命令：

```bash
    $ make TOOL=vcs TC=sv_demo_001
```

如果用户使用Vivado仿真器执行测试用例`sv_demo_001`，则可省略`make`命令后的参数，例如：

```bash
    $ make
```

一键式执行也支持vcs与questasim仿真器，详细使用方式请参考以上章节。

<a id="sec-1-5" name="sec-1-5"></a>

### **清理**

重新执行测试用例时，用户可将之前编译或者仿真的结果删除，清除操作命令如下：

```bash
    $ make clean
```

<a id="sec-1-6" name="sec-1-6"></a>

### **查看日志**

如果仿真编译出现错误，可查看report目录下的**log_comp.log**文件。编译中出现的错误会在日志中以`ERROR`关键字进行标注，命令如下：

```bash
    $ vi ./report/log_comp.log
```

如果编译成功而执行报错，可进入到相应的测试用例目录下，通过**log_simulation.log**可查看仿真运行的日志。仿真过程中的错误会以`[ERROR]:`关键字进行标注，命令如下：

```bash
    $ vi ./report/sv_demo_001/log_simulation.log
```

<a id="sec-1-7" name="sec-1-7"></a>

### **测试用例说明**

Example3中包含了三个测试用例`sv_demo_001`，`sv_demo_002`，`sv_demo_003`。

三个测试用例均包括如下功能：

1.读取`版本号`UL寄存器；
2.对UL的`输入数据取反`测试寄存器进行检测；

寄存器的定义在文件`./common/common_reg.svh`中。

<a id="sec-1-7-1" name="sec-1-7-1"></a>

#### 测试用例sv_demo_001说明

测试用例`sv_demo_001`除了完成版本寄存器读取与测试寄存器的检测外还对UL外挂的三个DDR接口进行了**读写测试**。

    详细过程如下：
    sv_demo_001会按照顺序对三个UL外挂的DDR接口依次下发写入以及读取操作。
    然后会判断读出的内容与写入是否相等。
    如果相等则会打印PASS，否则打印FAIL并终止仿真。

<a id="sec-1-7-2" name="sec-1-7-2"></a>

#### 测试用例sv_demo_002说明

测试用例`sv_demo_002`除了完成版本寄存器读取与测试寄存器的检测外还对UL进行了**DMA测试**。

    详细过程如下：
    sv_demo_002会通过仿真平台构造报文与BD并通过与UL相连的AXI4-Stream接口发送给UL。
    UL处理后会将报文原封不动的返回仿真平台并产生BD。
    仿真平台比对发送与接收的报文，如果相等则会打印PASS，否则打印FAIL并终止仿真。

<a id="sec-1-7-3" name="sec-1-7-3"></a>

#### 测试用例sv_demo_003说明

测试用例`sv_demo_003`完成的功能和`sv_demo_002`一样，区别在于`sv_demo_003`的激励和预期的数据是通过文件形式在`./tests/sv/sv_demo_003/test.cfg`中配置。

    详细过程如下：
    sv_demo_003通过`test.cfg`中配置的文件解析并构造报文与BD并通过与UL相连的AXI4-Stream接口发送给UL。
    UL处理后会将报文原封不动的返回仿真平台并产生BD。
    仿真平台比对发送与接收的报文，如果相等则会打印PASS，否则打印FAIL并终止仿真。

<a id="sec-2" name="sec-2"></a>

## **用户自定义测试**

<a id="sec-2-1" name="sec-2-1"></a>
### **修改仿真配置**
`scripts`目录中的`project_settings.cfg`文件，主要需要指定用户自定义仿真宏以及库文件。其中**USER_LIBS**与**SIM_MACRO**分别对应自定义库文件以及仿真宏。（如果没有可不填）
 example3 **SIM_MACRO** 仿真宏中ACC_LEN_CFG值不容许修改。

```bash
  $ vi ./scripts/project_settings.cfg
  
  SIM_MACRO=" USE_DDR_MODEL 
              ACC_LEN_CFG=1
  # '#' means comments
  # Example:
  # MACRO1
  # MACRO2
  "
  USER_LIBS="
  # '#' means comments
  # Example:
  # FILE_PATH1/FILE_NAME1
  # FILE_PATH2/FILE_NAME2
  "
```
### **编写用户测试用例**

整个example仿真文件夹目录如下：

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

首先用户需要建立用户自己的测试用例，测试用例的名称与用户建立的文件夹相同。用户可复制example中的已有测试用例，也可以自行创建。

```bash
    $ mkdir ./tests/sv/xxx_test                          # Create Testcase Directory
    $ touch ./tests/sv/base/xx_test.sv                   # Create Base Testcase
    $ cp -r ./tests/sv/sv_demo_001/* ./tests/sv/xxx_test # Copy Example to Own Testcase
```

其中用户测试用例可分为两部分，即**基础测试用例**以及**用户测试配置**。基础测试用例采用systemverilog语言编写，主要完成测试用例的主体流程；而用户测试配置是用户的配置文件，主要决定测试用例中需要的激励、配置等数据。

<a id="sec-2-1-1" name="sec-2-1-1"></a>

#### **编写基础测试用例**

如用户无需修改测试流程，仅需要修改激励的内容，可略过此章。
基础测试用例建议放在`./tests/sv/base`目录中并且命名为`xxx_test.sv`。基础测试用例可参考以下方式编写：

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

#### **编写用户测试配置**

用户测试配置主要用于确定测试用例中的激励、配置的数据的内容，采用配置文件的方法编写。用户配置文件建议放在`./tests/sv/xxx_test`目录中并且命名为`test.cfg`。配置文件语法格式如下：

```bash
    // 可使用'//'或者'#'作为注释，注释不会被传入Testbench

    // 参数传递语法格式为+xxx_name=yyyyy，其中xxx_name为参数的名字，yyyyy为参数内容（*注意：'='两端都不能有空格*）
    // 参数内容可以为10进制数字（123、456）、16进制数字（'hxxx）、字符串（abcd、"xxyyzz"）以及序列
    // 序列为多个参数的组合，中间使用','或者'；'进行分割，例如123,456,'h678,aaa

    # TEST_NAME表示测试用例对应的基础test
    +TEST_NAME=tb_reg_test

    # DUMP_FSDB表示是否DUMP VERDI波形
    +DUM_FSDB=0

    ...
```

配置文件中包含诸多配置项，其中每个配置项的名称定义在用户cfg中，例如：

```verilog
    class tb_reg_cfg;

        int adder0;
        int adder1;
        int name;

        function new();
            // get_string第一个参数为参数在配置文件中的名称，第二个参数为默认值
            name   = config_opt::get_string("NAME","noname");
            adder0 = config_opt::get_int("ADDER0", 'd0     );
            adder1 = config_opt::get_int("ADDER1", 'd0     );
        endfunction : new

    endclass : tb_reg_cfg
```

配置文件中对应的配置项如下：

```bash
    +NAME=TEST_NAME
    +ADDER0=123
    +ADDER1=456
```

**如果要修改激励组件以及CPU模型，请参考**[quick_start](../../../lib/sim/doc/quick_start_cn.md)。

<a id="sec-2-2" name="sec-2-2"></a>

### **执行用户测试用例**

如果需要编译、执行测试用例`xxx_test`，可通过如下命令执行：

```bash
    $ make TC=xxx_test              # Run testcase xxx_test，Compile Using vivado
    $ make TC=xxx_test TOOL=vcs     # Run testcase xxx_test，Compile Using vcsmx
    $ make TC=xxx_test TOOL=questa  # Run testcase xxx_test，Compile Using questasim
```

[1]:&amp;quot;VCS以及Questasim工具用户需自行安装&amp;quot;
