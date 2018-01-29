# 仿真公共文件夹说明

[Switch to the English version](./README.md)

<div id="table-of-contents">
<h2>目录</h2>
<div id="text-table-of-contents">
<ul>
<li><a href="#sec-1">1. <b>简介</b></a></li>
<li><a href="#sec-2">2. <b>目录结构</b></a><ul>
<li><a href="#sec-2-1">2.1. <b>sim目录</b></a></li>
<li><a href="#sec-2-2">2.2. <b>bench目录</b></a></li>
<li><a href="#sec-2-3">2.3. <b>common目录</b></a></li>
<li><a href="#sec-2-4">2.4. <b>stim目录</b></a></li>
<li><a href="#sec-2-5">2.5. <b>bfm目录</b></a></li>
<li><a href="#sec-2-6">2.6. <b>rm目录</b></a></li>
<li><a href="#sec-2-7">2.7. <b>test目录</b></a></li>
<li><a href="#sec-2-8">2.8. <b>vip目录</b></a></li>
<li><a href="#sec-2-9">2.9. <b>xxx_vip目录</b></a></li>
<li><a href="#sec-2-10">2.10. <b>precompiled目录</b></a></li>
<li><a href="#sec-2-11">2.11. <b>vcs_lib目录</b></a></li>
<li><a href="#sec-2-12">2.12. <b>questa_lib目录</b></a></li>
<li><a href="#sec-2-13">2.13. <b>scripts目录</b></a></li>
<li><a href="#sec-2-14">2.14. <b>doc目录</b></a></li>
</ul>
</li>
<li><a href="#sec-3">3. <b>仿真说明</b></a></li>
</ul>
</div>
</div>

<a id="sec-1" name="sec-1"></a>

## 简介

该文件夹主要存储仿真**公共文件**，包含公用仿真库文件、仿真平台公共部分、用户VIP、公用脚本等文件。

<a id="sec-2" name="sec-2"></a>

## 目录结构

- [sim/](#sec-2-1)
  - [bench/](#sec-2-2)
        -   [common/](#sec-2-3)
        -   [stim/](#sec-2-4)
        -   [bfm/](#sec-2-5)
        -   [rm/](#sec-2-6)
        -   [test/](#sec-2-7)
  - [vip/](#sec-2-8)
        -   [xxx_vip/](#sec-2-9)
  - [precompiled/](#sec-2-10)
        -   [vcs_lib/](#sec-2-11)
        -   [questa_lib/](#sec-2-12)
  - [scripts/](#sec-2-13)
  - [doc/](#sec-2-14)

## 目录说明

---

<a id="sec-2-1" name="sec-2-1"></a>

### sim目录

仿真公共目录，包含仿真平台代码、脚本、预编译库以及VIP。

<a id="sec-2-2" name="sec-2-2"></a>

### bench目录

仿真平台根目录。

<a id="sec-2-3" name="sec-2-3"></a>

### common目录

仿真平台公用文件目录（主要包含公用头文件，端口定义等等）。

<a id="sec-2-4" name="sec-2-4"></a>

### stim目录

仿真平台激励目录（包含激励数据以及激励产生组件）。

<a id="sec-2-5" name="sec-2-5"></a>

### bfm目录

仿真平台 BFM目录（包含AXI Master、AXI Slave以及AXI-Lite接口的BFM）。

<a id="sec-2-6" name="sec-2-6"></a>

### rm目录

仿真平台 RM目录（包含RM以及记分牌）。

<a id="sec-2-7" name="sec-2-7"></a>

### test目录

仿真平台 Env以及基础测试用例目录。

<a id="sec-2-8" name="sec-2-8"></a>

### vip目录

验证IP，包含平台的所有验证IP。

<a id="sec-2-9" name="sec-2-9"></a>

### xxx_vip目录

VIP目录，按照VIP分开存放。

<a id="sec-2-10" name="sec-2-10"></a>

### precompiled目录

预编译库路径，包括预编译的Xilinx通用仿真模型（主要用于提高编译速度）。

<a id="sec-2-11" name="sec-2-11"></a>

### vcs_lib目录

VCS预编译库。

<a id="sec-2-12" name="sec-2-12"></a>

### questa_lib目录

Questasim预编译库。

<a id="sec-2-13" name="sec-2-13"></a>

### scripts目录

仿真脚本目录，包含仿真脚本。

<a id="sec-2-14" name="sec-2-14"></a>

### doc目录

仿真文档文件夹，包含仿真平台的说明以及设计文档。

---

<a id="sec-3" name="sec-3"></a>

## 仿真说明

仿真平台详细说明请参考[仿真平台快速指南](./doc/quick_start_cn.md)以及[仿真平台用户指导](./doc/user_guide_cn.md)。
