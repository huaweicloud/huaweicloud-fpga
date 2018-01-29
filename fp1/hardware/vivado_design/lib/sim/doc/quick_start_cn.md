# 仿真平台快速指南

[Switch to the English version](./quick_start.md)

这是一个FACS仿真平台的快速入门教程。在此教程中我们首先将对仿真平台进行简单介绍，然后说明如何运行现有示例，最后告诉用户如何编写自己的仿真组件、测试用例完成仿真。

<div id="table-of-contents">
<h2>目录</h2>
<div id="text-table-of-contents">
<ul>
<li><a href="#sec-1">1. <b>仿真平台简介</b></a></li>
<li><a href="#sec-2">2. <b>设置仿真环境</b></a></li>
<li><a href="#sec-3">3. <b>对示例进行仿真</b></a>
<ul>
<li><a href="#sec-3-1">3.1. <b>编译仿真示例</b></a></li>
</ul>
<ul>
<li><a href="#sec-3-2">3.2. <b>运行仿真示例</b></a></li>
</ul>
<ul>
<li><a href="#sec-3-3">3.3. <b>调试仿真示例</b></a></li>
</ul>
<ul>
<li><a href="#sec-3-4">3.4. <b>一键式仿真示例</b></a></li>
</ul>
<ul>
<li><a href="#sec-3-5">3.5. <b>清除仿真结果</b></a></li>
</ul>
<ul>
<li><a href="#sec-3-6">3.6. <b>查看仿真日志</b></a></li>
</ul>
</li>
<li><a href="#sec-4">4. <b>用户自定义仿真</b></a>
<ul>
<li><a href="#sec-4-1">4.1. <b>编写用户测试用例</b></a>
<ul>
<li><a href="#sec-4-1-1">4.1.1. 创建用户工程</a></li>
</ul>
<ul>
<li><a href="#sec-4-1-2">4.1.2. 创建用户测试用例</a></li>
</ul>
<ul>
<li><a href="#sec-4-1-3">4.1.3. 修改仿真配置</a></li>
</ul>
<ul>
<li><a href="#sec-4-1-4">4.1.4. 编写基础测试用例</a></li>
</ul>
<ul>
<li><a href="#sec-4-1-5">4.1.5. 编写用户测试配置</a></li>
</ul>
</li>
<li><a href="#sec-4-2">4.2. <b>对用户测试用例进行仿真</b></a>
</ul>
</li>
<li><a href="#sec-5">5. <b>用户自定义组件</b></a>
<ul>
<li><a href="#sec-5-1">5.1. <b>编写用户自定义激励</b></a>
<ul>
<li><a href="#sec-5-1-1">5.1.1. 创建用户激励</a></li>
</ul>
<ul>
<li><a href="#sec-5-1-2">5.1.2. 修改用户激励</a></li>
</ul>
<ul>
<li><a href="#sec-5-1-3">5.1.3. 绑定用户激励</a></li>
</ul>
<ul>
<li><a href="#sec-5-1-4">5.1.4. 启动用户激励</a></li>
</ul>
</li>
<li><a href="#sec-5-2">5.2. <b>编写用户自定义配置</b></a>
<ul>
<li><a href="#sec-5-2-1">5.2.1. 编写自定义激励配置</a></li>
</ul>
<ul>
<li><a href="#sec-5-2-2">5.2.2. 编写自定义平台配置</a></li>
</ul>
</li>
<li><a href="#sec-5-3">5.3. <b>编写用户CPU模型回调</b></a>
<ul>
<li><a href="#sec-5-3-1">5.3.1. 创建用户CPU模型回调</a></li>
</ul>
<ul>
<li><a href="#sec-5-3-2">5.3.2. 修改用户CPU模型回调</a></li>
</ul>
<ul>
<li><a href="#sec-5-3-3">5.3.3. 绑定用户CPU模型回调</a></li>
</ul>
</li>
<li><a href="#sec-5-4">5.4. <b>编写用户参考模型</b></a>
</li>
<li><a href="#sec-5-5">5.5. <b>在测试用例中创建连接用户自定义组件</b></a></li>
</ul>
</li>
<li><a href="#sec-6">6. <b>VIP集成</b></a></li>
</div>
</div>

<a id="sec-1" name="sec-1"></a>

## **仿真平台简介**

---

FACS仿真平台可以实现C/Systemverilog混合语言的协同仿真。它提完整的分离的仿真平台与测试用例，用户可方便的通过对测试用例的设计实现仿真而无需修改仿真平台。

FACS仿真平台基于符合IEEE-1800(2012)规范的systemverilog语言开发，不使用任何验证方法学，使得仿真平台在Vivado、VCS以及Questasim仿真器下均可执行。

仿真平台结构如下图所示:

<img src="./images/testbench.png" alt="仿真平台组件框图">

其中。本架构具有如下有点：

- 用户可方便的完成自己的测试用例而无需关注、修改仿真平台

- 用户可方便的将VIP集成到仿真平台中

- 一次编译多次执行，提高仿真的效率

- 多语言激励支持将在极大限度上提高用户编写激励的效率

- 预编译库以及优化的仿真参数将为用户提供更高速的仿真体验

- 仿真平台优秀的代码兼容性使得仿真不再受工具限制

<a id="sec-2" name="sec-2"></a>

## **设置仿真环境**

---

每次开启新的终端运行FACS仿真平台时都需要设置环境变量，设置方法分为以下几步。

如果是第一次使用，需要先修改配置文件完成license的设置，需要修改`setup.cfg`中**XILINX_LIC_SETUP**字段的内容。
如有多个License，请使用":"进行分隔。

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

环境变量采用如下方式进行设置：

```bash
  $ source ./setup.sh
  ---------------------------------------------------
  Checking software infomation.......
  ---------------------------------------------------
  Checking software license.
  ---------------------------------------------------
  ...
```

环境变量的设置可能需要一段时间，请耐心等待。关于环境变量设置的详细操作与步骤请参考[fp1开发套件说明](../../../../../README_cn.md)。

仿真环境设置完成后，会自动在环境变量中配置好工程的根目录，即`fpga_design`文件夹的目录，环境变量为`WORK_DIR`。

用户可通过如下命令查看环境变量：

```bash
  $ echo $WORK_DIR
```

<a id="sec-3" name="sec-3"></a>

## **对示例进行仿真**

---

FACS仿真平台提供了丰富的示例以帮助用户更好的理解如何仿真。仿真平台的编译、执行以及调试操作均通过Makefile的目标完成。如需执行仿真有两种方式:

- 标准make方法：即编译、执行以及调试仿真示例前用户首先需要将当前目录切换到示例所在的仿真根目录，再执行`make`命令完成操作。仿真根目录如下表所示：

    | 示例名      | 目录                                       |
    | -------- | ---------------------------------------- |
    | example1 | `$WORK_DIR/hardware/vivado_design/example/example1/sim` |
    | example2 | `$WORK_DIR/hardware/vivado_design/example/example2/sim` |

    用户可采用如下命令切换到示例对应的仿真根目录：

    ```bash
      $ export EXAMPLE_DIR=$WORK_DIR/hardware/vivado_desgin/examples/examplex
      $ cd $EXAMPLE_DIR/sim
    ```
    其中`examplex`表示示例名，可以为`example1`或`example2`。

- 指定目录make方法：即编译、执行以及调试仿真示例时用户直接通过`make -C`命令指定Makefile的目录完成操作。

    用户可采用如下命令完成制定目录的`make`操作：

    ```bash
      $ export EXAMPLE_DIR=$WORK_DIR/hardware/vivado_desgin/examples/examplex
      $ make -C $EXAMPLE_DIR/sim XXX # XXX为Makefile目标
    ```

其他详细的`Makefile`相关参数以及目标请参考[仿真平台用户指导](./user_guide_cn.md)。

<a id="sec-3-1" name="sec-3-1"></a>

### **编译仿真示例**

---

编译Example的命令为`make comp`，缺省编译命令如下：

```bash
  $ make comp
```

默认采用vivado作为仿真器，如果用户需要使用vcs仿真器或questasim仿真器，可使用如下命令：

```bash
  $ make comp TOOL=vcs    # Compile Using vcsmx
  $ make comp TOOL=questa # Compile Using questasim
  $ make comp TOOL=vivado # Compile Using vivado(Same as do not specify the simulation tools)
```

<a id="sec-3-2" name="sec-3-2"></a>

### **运行仿真示例**

执行Example仿真的命令为`make run`，需要指定测试用例名，以下为执行**sv_demo_001**测试用例的命令（由于sv_demo_001为缺省测试用例名，所以如果执行该用例可省略用例名）：

```bash
  $ make run TC=sv_demo_001
```

`TC`后参数为需要执行的测试用例名称，需要和`$EXAMPLE_DIR/sim/tests/sv/`目录中的测试用例的文件夹名称一致。

默认采用vivado作为仿真器，如果用户需要使用vcs仿真器或questasim仿真器，可使用如下命令：

```bash
  $ make run TOOL=vcs TC=sv_demo_001 # Compile Using vcsmx
  $ make run TOOL=questa TC=sv_demo_001 # Compile Using questasim
  $ make run TOOL=vivado TC=sv_demo_001 # Compile Using vivado(Same as do not specify the simulation tools)
```

<a id="sec-3-3" name="sec-3-3"></a>

### **调试仿真示例**

调试Example的命令为`make wave`，参数与执行类似，需要指定测试用例名，以下为调试**sv_demo_001**测试用例的命令：

```bash
  $ make wave TC=sv_demo_001
```

默认采用vivado进行调试，如果用户需要使用dve或questasim进行调试，可使用如下命令：

```bash
  $ make wave TOOL=vcs TC=sv_demo_001    # Compile Using vcsmx
  $ make wave TOOL=questa TC=sv_demo_001 # Compile Using questasim
  $ make wave TOOL=vivado TC=sv_demo_001 # Compile Using vivado(Same as do not specify the simulation tools)
```

<a id="sec-3-4" name="sec-3-4"></a>

### **一键式仿真示例**

Example支持一键式运行，即一键式自动完成编译以及运行，可使用如下命令：（all为缺省目标，可以省略）

```bash
  $ make all
```

一键式运行也支持vcs与questasim，详细使用方式请参考以上章节。

<a id="sec-3-5" name="sec-3-5"></a>

### **清除仿真结果**

测试用例编译或者执行时，会在`work`目录中产生一些仿真中间文件，建议每次编译前先将这些文件清空，可采用如下命令进行清除：

```bash
  $ make clean
```

用户如果在清除仿真中间文件时还需需要清除**预编译库文件**，可执行以下命令：

```bash
  $ make distclean
```

<a id="sec-3-6" name="sec-3-6"></a>

### **查看仿真日志**

如果仿真编译失败，可查看`report`目录下的编译的log文件**log_comp.log**：

```bash
  $ vi ./report/log_comp.log
```

如果编译成功而执行仿真时报错，可进入到相应的测试用例目录下，通过查看仿真运行的log文件**log_simulation.log**定位：*（test_xxx表示用户需要查看的测试用例名）*

```bash
  $ vi ./report/test_xxx/log_simulation.log
```

<a id="sec-4" name="sec-4"></a>

## **用户自定义仿真**

---

用户不仅可以执行示例，也可以自行编写、编译、运行以及调试自己的测试用例。

<a id="sec-4-1" name="sec-4-1"></a>

### **编写用户测试用例**

---

用户如果需要编写自己的测试用例需要以下几步：

- 1 [创建用户工程](#sec-4-1-1)*（如果已有工程请忽略这一步）*

- 2 [创建用户测试用例](#sec-4-1-2)

- 3 [修改仿真配置](#sec-4-1-3)

- 4 [编写基础测试用例](#sec-4-1-4)

- 5 [编写用户测试配置](#sec-4-1-5)

<a id="sec-4-1-1" name="sec-4-1-1"></a>

#### 创建用户工程

用户如需编写自己的测试用例，首先需要创建工程，可将`example`或者`template`文件夹中的sim文件夹复制到用户目录，例如：

```bash
  $ export USER_DIR=$/WORK_DIR/hardware/vivado_desgin/user/user_xxx
  $ cd $USER_DIR
  $ cp -rf ../../lib/template/sim ./
  $ cd ./sim
```

还可以在用户目录下，使用`creat_prj.sh`帮助用户完成用户仿真文件夹的创建，例如：

```bash
  $ cd $WORK_DIR/hardware/vivado_desgin/user
  $ sh ./create_prj.sh user_pri_name
  $ cd ./user_pri_name/sim
```

详细`create_prj.sh`命令的参数请参考[usr_template用户指南](../../template/readme_cn.md)。

<a id="sec-4-1-2" name="sec-4-1-2"></a>

#### 创建用户测试用例

用户创建工程后，工程中会包含仿真文件夹，整个用户仿真文件夹目录如下：

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

用户需要建立用户自己的Testcase，testcase的名称与用户建立的文件夹名称相同。用户可将example文件夹中的已有测试用例复制为自己的testcase，也可以自行创建。

```bash
  $ cd ./tests/sv
  $ mkdir xxx_test                  # Create Testcase Directory
  $ cp -r ./sv_demo_001/* xxx_test/ # Copy Example to Own Testcase
```

其中测试用例可分为两部分，即**基础测试用例**以及**用户测试配置**。
基础测试用例采用systemverilog语言编写，主要完成测试用例的主体流程；
用户测试配置是用户的配置文件，主要决定测试用例中需要的激励、配置等参数。

<a id="sec-4-1-3" name="sec-4-1-3"></a>

#### 修改仿真配置

创建了测试用例目录后，用户还需要修改`scripts`目录中的`project_settings.cfg`文件，主要需要指定用户自定义仿真宏以及库文件。
其中**USER_LIBS**与**SIM_MACRO**分别对应自定义库文件以及仿真宏。（如果没有可不填）

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

#### 编写基础测试用例

基础测试用例采用systemverilog语言编写，主要完成测试用例的主体流程以及用户自定义组件的实例化与连接。

基础测试用例的编写应遵循以下规则（详细用法请参考[仿真平台用户指导](./user_guide_cn.md)）：

- 基础测试用例必须继承自`tb_test`类或者其子类；

- 基础测试用例中需要显式调用宏`tb_register_test`来完成测试用例的注册；

- 基础测试用例的new需要显式调用父类的`new`方法；

- 建议用户将测试的主体部分放到任务`run`中；

- 如果用户需要进行结果检测，错误请使用`tb_error`宏上报，经过该宏上报的错误最终会使得用例失败。

基础测试用例可参考以下方式编写：

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

#### **编写用户测试配置**

用户测试配置主要用于确定测试用例中的激励、配置的数据的内容，采用配置文件的方法编写。配置文件语法格式如下：

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

<a id="sec-4-2" name="sec-4-2"></a>

### **对用户测试用例进行仿真**

执行用户测试用例的方法与仿真示例的方法类似，仅需要修改仿真根目录即可，详细过程请参考章节[对示例进行仿真](#sec-3-1)。

<a id="sec-5" name="sec-5"></a>

## **用户自定义组件**

---

如果有复杂激励、参考模型的需求，用户需要自定义这些组件。目前可支持用户自定义的组件如下：

- [激励](#sec-5-1)
- [配置](#sec-5-2)
- [CPU模型](#sec-5-3)
- [参考模型](#sec-5-4)

<a id="sec-5-1" name="sec-5-1"></a>

### **用户自定义激励**

---

用户激励分为三部分，即**激励产生方法**、**激励生成器**以及**激励配置**。如下图所示：

<img src="./images/stim.png" alt="激励组件框图">

其中激励生产器本身比不包含激励的产生方法，故用户无需修改；如需自定义激励仅需要修改激励产生方法以及配置。

如果需要自己定义激励的产生方法，可采用以下三个步骤实现：

<a id="sec-5-1-1" name="sec-5-1-1"></a>

#### 创建用户激励

用户激励可放在`$USER_DIR/sim`文件夹下的`common`或用户`testcase`目录下的`base`文件夹。

用户激励需继承自`$LIB_DIR/sim/bench/stim`文件夹中的`axi_stims.sv`，所以建议用户直接copy该文件到上述文件夹下，例如：

```bash
  $ cp $LIB_DIR/sim/bench/stim/axi_stims.sv $USER_DIR/sim/common/user_stim.sv
```

或

```bash
  $ cp $LIB_DIR/sim/bench/stim/axi_stims.sv $USER_DIR/sim/testcase/sv/base/user_stim.sv
```

<a id="sec-5-1-2" name="sec-5-1-2"></a>

#### 修改用户激励

用户可以按照自己的需求修改`user_stim.sv`文件。

修改user_stim.sv有以下几个建议或要求：

- `user_stim.sv`必须继承自`axi_stims`类；

- 如果需要自定义激励的产生方法，请重载任务`gen_pkt`；

- 如果需要自定义激励的发送方法，请重载任务`send_pkt`；

例如：

```verilog
    class user_stims extends axi_stims;
        ...
        // Stim constraint
        constraint axi_data_user_constraint {
        // 如果不使用VIVADO作为仿真器，在下面编写用户自己的的激励产生方式
        // 如果使用VIVADO作为仿真器，则此段代码可删除
        // Vivado仿真器不支持constraint
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

#### 绑定用户激励

修改用户激励完成后，还需要将自己编写的激励绑定到激励生成器上，实现最终的激励产生。这部分实现需要在基础测试用例中实现。例如：

用户可编辑`tb_test_user`(用户基础testcase)，在`build`以及`connect`方法中实现用户激励的创建与绑定。详细方法请参考章节[在测试用例中创建连接用户自定义组件](#sec-5-5)。

<a id="sec-5-1-4" name="sec-5-1-4"></a>

#### 启动用户激励

用户可在完成激励实例化与绑定后通过激励组件方法<kbd>start</kbd>启动激励发送，并通过<kbd>stop</kbd>方法手动停止激励发送或通过<kbd>wait_done</kbd>方法等待激励发送完成后自动停止。

例如：

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

### **编写用户自定义配置**

---

用户自定义配置可放在`$USER_DIR/sim`文件夹下的`common`或用户`testcase`目录下的`base`文件夹。

用户自定义配置包含**自定义激励配置**与**自定义平台配置**。

<a id="sec-5-2-1" name="sec-5-2-1"></a>

#### 编写自定义激励配置

如果是自定义激励配置，配置需继承自`$LIB_DIR/sim/bench/stim`文件夹中的`axi_stim_cfg.svh`，所以建议用户直接copy该文件到上述文件夹下，例如：

```bash
  $ cp $LIB_DIR/sim/bench/stim/axi_stim_cfg.svh $USER_DIR/sim/common/user_stim_cfg.svh
```

或

```bash
  $ cp $LIB_DIR/sim/bench/stim/axi_stim_cfg.svh $USER_DIR/sim/testcase/sv/base/user_stim_cfg.svh
```

然后再按照用户自己的需求修改`user_stim_cfg.svh`文件，例如：

```verilog
    class user_stim_cfg extends axi_stim_cfg;
        // 用户自行定义配置变量
        bit [63 : 0]      axi_addr_min  ;  // Address low range
        bit [63 : 0]      axi_addr_max  ;  // Address max range
        int               axi_data_len  ;  // Data length
        ...
        function new();
            // 获得配置文件中的内容
            axi_addr_min  = config_opt#(64)::get_bits("axi_addr_min"  );
            axi_addr_max  = config_opt#(64)::get_bits("axi_addr_max"  );
            axi_data_len  = config_opt#(32)::get_bits("axi_data_len"  );
            ...
        endfunction : new
    endclass : user_stim_cfg
```

最后，用户需要将自己编写的激励配置绑定到对应的激励上，实现最终的激励产生。这部分实现需要在基础Testcase中实现。

用户可编辑tb_test_user(用户基础testcase)，在`build`以及`connect`方法中实现用户激励的创建与绑定。详细方法请参考章节[在测试用例中创建连接用户自定义组件](#sec-5-5)。

<a id="sec-5-2-2" name="sec-5-2-2"></a>

#### 编写自定义平台配置

如果是自定义平台配置，则无继承关系约束，用户可直接新建该文件到上述文件夹下，例如：

```bash
  $ touch -f $USER_DIR/sim/common/user_tb_cfg.svh
```

然后再按照用户自己的需求修改`user_tb_cfg.svh`文件，例如：

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

最后，用户需要将自己编写的激励配置绑定到对应的激励上，实现最终的激励产生，详细步骤见[编写自定义激励配置](#5-2-1)。

<a id="sec-5-3" name="sec-5-3"></a>

### **编写用户CPU模型回调**

---

CPU模型主要用于模拟CPU与`SHELL`的行为，与`UL`按照预定义规则完成交互。CPU模型可分为两部分：**CPU模型**以及**CPU模型回调**。如下图所示：

<img src="./images/model.png" alt="CPU模型组件框图">

其中CPU模型中不包含任何交互相关的实现，仅提供接口与其他组件连接，所以如果用户需要自己定义CPU模型的行为，只需自定义CPU模型回调。

编写CPU模型回调可分为以下三个步骤：

<a id="sec-5-3-1" name="sec-5-3-1"></a>

#### 创建用户CPU模型回调

用户CPU模型回调可放在`$USER_DIR/sim`文件夹下的`common`或用户`testcase`目录下的`base`文件夹。

用户CPU模型回调需继承自`$LIB_DIR/sim/bench/rm`文件夹中的`cpu_model_cb.svh`，所以建议用户直接copy该文件到上述文件夹下，例如：

```bash
  $ cp $LIB_DIR/sim/bench/rm/cpu_model_cb.svh $USER_DIR/sim/common/user_model_cb.svh
```

或

```bash
  $ cp $LIB_DIR/sim/bench/rm/cpu_model_cb.svh $USER_DIR/sim/testcase/sv/base/user_model_cb.svh
```

<a id="sec-5-3-2" name="sec-5-3-2"></a>

#### 修改用户CPU模型回调

CPU模型回调模块提供了三个任务可供用户重载，这三个任务分别为：

- request_process：

    主要负责CPU模型对激励的处理，即收到激励发送的数据后，按照规则产生BD、将数据存入本地虚拟memory中，再将数据发送给`RM`，完成预期。

- response_process

    主要负责完成CPU模型对请求的相应返回，即收到`UL`发送的读请求后，按照BD中的指示从本地虚拟memory中读取数据，再将数据发送给`UL`。

- user_process

    主要负责完成CPU模型对`UL`发送数据的处理，即收到`UL`发送的写数据和BD后，将数据与BD拼接在一起，再将数据发送给`RM`，完成预期。

用户可以按照自己的需求对`user_model_cb.svh`文件中的三个任务进行重载。

例如：

```verilog
    class user_model_cb extends cpu_model_cb;
        ...
        // 该方法主要完成CPU模型对激励的处理
        task request_process();
            ...
        endtask : request_process
        // 该方法主要完成CPU模型根据请求返回响应
        task cpu_model_cb::response_process();
            ...
        endtask : response_process
        // 该方法主要完成CPU模型的其他处理
        task cpu_model_cb::user_process();
            ...
        endtask : user_process
    endclass : user_model_cb
```

<a id="sec-5-3-3" name="sec-5-3-3"></a>

#### 绑定用户CPU模型回调

修改用户CPU模型回调完成后，还需要将自己编写的CPU模型回调绑定到CPU模型上，实现最终的CPU模型自定义。这部分实现需要在基础测试用例中实现。

用户可编辑`tb_test_user`(用户基础testcase)，在`build`以及`connect`方法中实现用户CPU模型回调的创建与绑定。详细方法请参考章节[在测试用例中创建连接用户自定义组件](#sec-5-5)。

<a id="sec-5-4" name="sec-5-4"></a>

### **编写用户参考模型**

---

用户参考模型主要用于用户数据的预期以及对输出相应的核查。

如果用户需要自己定义参考模型(RM)，首先需创建参考模型，RM可放在`$USER_DIR/sim`文件夹下的`common`或用户`testcase`目录下的`base`文件夹。

其次，用户CPU模型回调需继承自`$LIB_DIR/sim/bench/rm`文件夹中的`axi_rm.sv`，所以建议用户直接copy该文件到上述文件夹下，例如：

```bash
  $ cp $LIB_DIR/sim/bench/rm/axi_rm.sv $USER_DIR/sim/common/user_rm.sv
```

或

```bash
  $ cp  $LIB_DIR/sim/bench/rm/axi_rm.sv $USER_DIR/sim/testcase/sv/base/user_rm.sv
```

用户RM模块提供了两个函数可供用户重载，这两个函数分别为：

- insert：

    主要完成激励数据的预期。

- check

    主要完成对返回数据的处理与比对。

用户可按照自己的需求修改`user_rm.sv`文件。由于`axi_rm.sv`中实现了记分牌的功能，所以建议用户以如下方式修改该组件：

- 1 用户自定义RM需要继承自`axi_rm.sv`；

- 2 用户重载的函数`insert`和`check`中建议最后阶段显式调用父类的方法完成记分牌的预期与比对；

例如：

```verilog
    class user_rm extends axi_rm;
        ...
        // 该方法主要完成激励部分的预期
        function void insert(ref DATA data);
            ...
        endfunction : insert
        // 该方法主要完成响应的核查
        function void check(ref DATA data);
            ...
        endfunction : check
    endclass : user_rm
```

最后，用户需要把自己编写的RM和其他组件连接起来。这部分实现需要在基础Testcase中实现。例如：

用户可编辑tb_test_user(用户基础testcase)，在`build`以及`connect`方法中实现RM的创建与连接。详细方法请参考章节[在测试用例中创建连接用户自定义组件](#sec-5-5)。

<a id="sec-5-5" name="sec-5-5"></a>

### **在测试用例中创建连接用户自定义组件**

---

如果用户编写了自定义的RM、激励或者其他callback，还需要实例化并且连接后才可使用。实例化与连接的方法如下：

```verilog
    ...
    // 实例化用户自定义组件
    function void build();
        m_user_tb_cfg   = new();
        m_user_stim_cfg = new();
        m_user_cb       = new("m_user_cb"  ); // 实例化用户回调
        m_user_stim     = new("m_user_stim"); // 实例化用户激励
        m_user_rm       = new("m_user_rm"  ); // 实例化RM
        super.build();
    endfunction : build
    // 连接用户自定义组件
    function void connect();
        super.connect();
        // 绑定激励配置到激励方法
        m_user_stim.set_cfg(m_user_stim_cfg);
        // 连接RM
        m_user_cb.m_rm = m_user_rm;
        // 绑定用户激励与激励生成器
        m_tb_env.m_axi_gen.reg_stims(m_user_stim);
        // 添加用户回调到组件
        m_tb_env.m_cpu_model.append_callback(m_user_cb);
    endfunction : connect
```

<a id="sec-6" name="sec-6"></a>

## **VIP集成**

---

**VIP**(*Verification Intellectual Property*)即**验证IP**，指的是某些设计好的验证组件，用于方便用户对复杂协议或功能进行验证。

用户可将VIP放到`$LIB_DIR/sim/vip`目录，也可将VIP放置在用户`$USER_DIR/sim/lib`目录中，只要放在该目录中的代码，仿真平台会自动编译。如果用户集成的VIP有独立的宏或者仿真选项，可在`project_setting.cfg`文件中的制定选项中进行添加。

仿真平台默认情况下需要使用Xilinx的**DDR4仿真模型**以及**DDR4 RDIMM**仿真模型。这两个仿真模型默认情况下不会包含在仿真平台的VIP目录中，用户执行环境设置脚本`setup.sh`后会调用Vivado的接口自动生成这两个VIP。

注意：仿真平台在调用Vivado的接口自动生成这两个VIP后会通过脚本对VIP的**部分代码进行修改**，所以请**不要随意修改**这两个VIP的内容。
