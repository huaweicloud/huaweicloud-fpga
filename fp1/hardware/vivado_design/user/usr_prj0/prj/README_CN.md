# usr_prj0构建指南

---

[Switch to the English version](./README.md)

<div id="table-of-contents">
<h2>目录</h2>
<div id="text-table-of-contents">
<ul>
<li><a href="#sec-1">1. <b>目录结构</b></a></li>
<li><a href="#sec-2">2. <b>文件及文件夹说明</b></a></li>
<li><a href="#sec-3">3. <b>构建说明</b></a>
<ul>
<li><a href="#sec-3-1">3.1. <b>usr_prj_cfg 配置说明</b></a></li>
</ul>
<ul>
<li><a href="#sec-3-2">3.2. <b>build.sh使用说明</b></a></li>
</ul>
<ul>
<li><a href="#sec-3-3">3.3. <b>schedule_task.sh使用说明</b></a></li>
</ul>
<ul>
<li><a href="#sec-3-5">3.4. <b>AEI_Register.sh文件配置说明</b></a></li>
</ul>
</li>
<li><a href="#sec-4">4. <b>其他</b></a></li>
</ul>
</li>
</div>
</div>

<a id="sec-1" name="sec-1"></a>

## 目录结构

usr_prj0的prj文件夹层级结构如下：

- **prj/**
  - [build/](#sec-2-1)
  - [constraints/](#sec-2-2)
  - [build.sh](#sec-2-3)
  - README_cn.md （本文档）
  - [schedule_task.sh](#sec-2-4)
  - [usr_prj_cfg](#sec-2-5)
  - [AEI_Register.sh](#sec-2-7)

<a id="sec-2" name="sec-2"></a>

## 文件及文件夹说明

<a id="sec-2-1" name="sec-2-1"></a>

- build  
  该目录主要用于存放示例构建信息。

<a id="sec-2-2" name="sec-2-2"></a>

- constraints  
  该目录主要用于存放usr_prj0的自定义约束信息，来源于`usr_prj_cfg` 中的 USR_CONSTRAINTS。

<a id="sec-2-3" name="sec-2-3"></a>

- build.sh  
  该文件主要用于构建工程，用户`修改完usr_prj_cfg`后，执行该文件即可实现工程构建，详细使用说明请见[build.sh使用说明](#sec-3-2)。

<a id="sec-2-4" name="sec-2-4"></a>

- schedule_task.sh  
  该文件主要用于完成工程的延时以及定时构建，详细使用说明请见[schedule_task.sh使用说明](#sec-3-3)。

<a id="sec-2-5" name="sec-2-5"></a>

- usr_prj_cfg  
  该文件主要用于配置usr_prj0工程的自定义信息，详细使用说明请见[usr_prj_cfg使用说明](#sec-3-1)。

<a id="sec-2-7" name="sec-2-7"></a>

- AEI_Register.sh  
  该命令主要用于完成dcp文件打包上传等步骤，执行该文件即可实现将dcp文件打包上传至OBS桶，并获取注册ID，详细使用说明请见[AEI_Register.sh使用说明](#sec-3-5)。

<a id="sec-3" name="sec-3"></a>

## 构建说明

<a id="sec-3-1" name="sec-3-1"></a>

### usr_prj_cfg 配置说明

- 该文件主要用于配置用户工程的自定义信息。

- usr_prj_cfg 文件在`$WORK_DIR/hardware/vivado_design/user/usr_prj0/prj/`目录（后续所有操作均在此目录执行）。

如需编辑usr_prj_cfg文件，请使用以下命令：

```bash
  $ cd $WORK_DIR/hardware/vivado_design/user/usr_prj0/prj/
  $ vim ./usr_prj_cfg
```

`usr_prj_cfg`内容介绍：

| 内容        | 关键字               | 示例                                       |
| :-------- | :---------------- | :--------------------------------------- |
| 用户创建工程的名字 | USR_PRJ_NAME      | USR_PRJ_NAME=ul_pr_top                   |
| 工程顶层的名字   | USR_TOP           | USR_TOP=ul_pr_top                        |
| 用户指定的综合策略 | USR_SYN_STRATEGY  | USR_SYN_STRATEGY=AreaOptimized_high      |
| 用户指定的实现策略 | USR_IMPL_STRATEGY | USR_IMPL_STRATEGY=Explore                |
| 用户的自定义约束  | USR_CONSTRAINTS   | USR_CONSTRAINTS="set_multicycle_path -setup -from [get_pins cpu_data_out*/D] 3" |

---

用户需根据自己的设计按照上表中`示例`一栏对自定义工程的usr_prj_cfg进行配置：

- USR_PRJ_NAME=`此处为用户创建工程工程的名字`，如 ul_pr_top 或 usr_prjxx_top；
- USR_TOP=`此处为工程顶层的名字` ，如 ul_pr_top 或 usr_prjxx_top；
- USR_SYN_STRATEGY=`此处为综合策略` ，如 AreaOptimized_high等 ，用户自己指定，一般默认为DEFAULT；
- USR_IMPL_STRATEGY=`此处为实现策略` ，如 Explore 等 ，用户自己指定，一般默认为DEFAULT；
- USR_CONSTRAINTS=`此处为自定义约束`，一般为默认状态，不更改。

<a id="sec-3-2" name="sec-3-2"></a>

### build.sh使用说明

`build.sh`脚本主要完成工程构建。不仅支持一键式构建也支持分部构建，更多工程构建详细的说明可使用`-h`命令获得，命令如下：

```bash
  $ sh build.sh -h
```

| 参数                         | 说明           |
| :------------------------- | :----------- |
| [-s] or [-S] or [-synth]   | 单步执行综合       |
| [-i] or [-I] or [-impl]    | 单步执行实现       |
| [-p] or [-P] or [-pr]      | 单步执行pr校验     |
| [-b] or [-B] or [-bit]     | 单步执行目标文件生成   |
| [-e] or [-E] or [-encrypt] | 综合选择加密，默认不加密 |
| [-h] or [-H] or [-help]    | build.sh帮助说明 |
| [-s_strategy_help]         | 综合策略帮助说明     |
| [-i_strategy_help]         | 实现策略帮助说明     |

- 如果需要使用一键式工程构建，请使用如下用命令:

```bash
  $ sh build.sh
```

  该命令将一键式完成**综合**、**布局布线**2个步骤，`所有步骤都successfully`，整个工程执行PASS；  
  `注：  **pr校验**和**bit文件生成**这2个步骤也可以通过单步执行的方式实现（参见[build.sh使用说明](#sec-3-2)单步执行的说明）。

---

`build.sh`还可用于单步执行某一项编译任务。

- 单步执行**综合**参考命令如下：

```bash
  $ sh build.sh -s
```

单步执行综合打印提示出现`“ synth_design completed successfully.”`表示综合成功；

---

- 单步执行**实现**参考命令如下：

```bash
  $ sh build.sh -i
```

单步执行布局布线打印提示出现`“route_design completed successfully”`表示布局布线成功；

---

- 单步执行**pr校验**参考命令如下：

```bash
  $ sh build.sh -pr
```

单步执行pr校验打印提示出现`“PR_VERIFY: check points /home/.../usr_prjxx/prj/build/checkpoints/to_facs/usr_prjxx_routed.dcp and /home/.../lib/checkpoints/SH_UL_BB_routed.dcp are compatible”`表示PR校验成功；

---

- 单步执行**bit文件生成**参考命令如下：

```bash
  $ sh build.sh -b
```

单步执行bit文件生成打印提示出现`“Bitgen Completed Successfully.”`表示bit文件生成成功。

---

用户查看执行过程打印，能读到vivado工具执行每一步的结果是否seccussfully，也能看到最终执行结果是否PASS。

---

<a id="sec-3-3" name="sec-3-3"></a>

### schedule_task.sh使用说明

`schedule_task.sh`脚本主要完成可配置时间的定时构建，更多详细的说明可使用`-h`命令获得，命令如下：

```bash
  $ sh schedule_task.sh -h
```

`schedule_task.sh`有两个参数，分别为：`hour和minutes`；
同时，`schedule_task.sh`支持两种执行方式：

- 方式1：延时执行

  使用`schedule_task.sh *h`或`schedule_task.sh *m`命令表示在若干小时或若干分钟后执行工程编译，`*`表示用户设定的数值，也可以使用`schedule_task.sh *h *m`的方式执行，表示在若干小时若干分钟后执行；

  如果不写单位默认单位为 s（秒），m 表示分钟, h 表示小时，d表示天，详细使用方式例如：

```bash
  $ sh ./schedule_task.sh 1m      # after 1 minute run
  $ sh ./schedule_task.sh 1h      # after 1 hour run
```

---

- 方式2：定时执行

  使用`schedule_task.sh hour:minute`命令，直接跟随执行时刻，表示在用户指定的时间点执行工程构建。

   如果不写时间默认为立即执行，详细使用方式例如：

```bash
  $ sh ./schedule_task.sh 11:50   # run project at 11:50
  $ sh ./schedule_task.sh 23:00   # run project at 23:00
```

<a id="sec-3-5" name="sec-3-5"></a>

### AEI_Register.sh命令的使用说明

该命令主要完成dcp文件打包上传等步骤，执行该文件即可实现将dcp文件上传至OBS桶，并获取注册ID。
执行AEI_Register.sh脚本的命令格式如下：

```bash
  $ sh AEI_Register.sh -p [dcp_obs_path] -o [log_obs_dir] -n [AEI_name] -d [AEI_Description]

  # -p DCP文件存储在OBS桶中的文件路径。dcp_obs_path 不能以“/”开头，必须以“.tar”结尾，不能为空，不能以“.”开头或结尾。dcp_obs_path由英文大、小写字母，数字，中划线，下划线，斜杠，英文句号组成。长度4到128个字符。
  # -o （可选）后台编译所产生的给用户查看的LOG文件所在的OBS桶中的文件目录。当log_obs_dir参数未指定或为空时，默认与DCP文件位于同一级目录下。   
  # -n 选项指定待注册的FPGA镜像（AEI）名称。AEI_name是由英文大小写字母、数字、下划线、中划线组成的字符串，长度为1到64位，用户自行设计即可。
  # -d 选项指定待注册的FPGA镜像（AEI）描述信息。AEI_Description由中文汉字、中文句号逗号、英文大小写字母、数字、中划线、下划线、英文句号逗号、空格组成的字符串，长度为0到255位，用户自行设计即可。
  # 参数之间需要分别用引号括起来，例如sh AEI_Register.sh -p "vu9p/abc.tar" -o "vu9p" -n "ocl-test" -d "ocl-desc"
```

**重要说明**:

- 执行AEI_Register.sh命令完成`dcp文件打包上传`和`注册ID生成`3个步骤，因此该步骤耗时稍长。

- 在AEI_Register.sh脚本执行成功后，会产生如下的回显信息。

```bash
#############################################################
Register AEI
#############################################################
Success: 200 OK
id: 0000********5568015e3c87835c0326
status: saving
```

- 出现`Success: 200 OK`信息表示AEI_Register.sh脚本执行成功，bit文件已上传至OBS桶。
- `status: saving`信息表示用户注册的二进制文件当前正处于保存状态。

<a id="sec-4" name="sec-4"></a>

## 其他

完成构建后，会产生的`vivado.log`、`"$USR_TOP"_terminal_run.log`等文件。如果构建失败，用户可根据这些日志定位构建失败的原因。
