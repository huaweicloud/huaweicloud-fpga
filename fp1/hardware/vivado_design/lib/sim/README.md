# Descriptions on Simulation Common Folders

[切换到中文版](./README_CN.md)

<div id="table-of-contents">
<h2>Contents</h2>
<div id="text-table-of-contents">
<ul>
<li><a href="#sec-1">1. <b>Introduction</b></a></li>
<li><a href="#sec-2">2. <b>Directory Structure</b></a><ul>
<li><a href="#sec-2-1">2.1. <b>sim Directory</b></a></li>
<li><a href="#sec-2-2">2.2. <b>bench Directory</b></a></li>
<li><a href="#sec-2-3">2.3. <b>common Directory</b></a></li>
<li><a href="#sec-2-4">2.4. <b>stim Directory</b></a></li>
<li><a href="#sec-2-5">2.5. <b>bfm Directory</b></a></li>
<li><a href="#sec-2-6">2.6. <b>rm Directory</b></a></li>
<li><a href="#sec-2-7">2.7. <b>test Directory</b></a></li>
<li><a href="#sec-2-8">2.8. <b>vip Directory</b></a></li>
<li><a href="#sec-2-9">2.9. <b>xxx_vip Directory</b></a></li>
<li><a href="#sec-2-10">2.10. <b>precompiled Directory</b></a></li>
<li><a href="#sec-2-11">2.11. <b>vcs_lib Directory</b></a></li>
<li><a href="#sec-2-12">2.12. <b>questa_lib Directory</b></a></li>
<li><a href="#sec-2-13">2.13. <b>scripts Directory</b></a></li>
<li><a href="#sec-2-14">2.14. <b>doc Directory</b></a></li>
</ul>
</li>
<li><a href="#sec-3">3. <b>Simulation Descriptions</b></a></li>
</ul>
</div>
</div>

<a id="sec-1" name="sec-1"></a>

## Introduction

This folder stores simulation common files, including common simulation library files, common parts of the simulation platform, verification IPs, and common scripts.

<a id="sec-2" name="sec-2"></a>

## Directory Structure

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

## Directory Descriptions

---

<a id="sec-2-1" name="sec-2-1"></a>

### sim Directory

The simulation common directory includes simulation platform code, scripts, precoding library, and verification IPs.

<a id="sec-2-2" name="sec-2-2"></a>

### bench Directory

Indicates the root directory of the simulation platform.

<a id="sec-2-3" name="sec-2-3"></a>

### common Directory

Indicates the common file directory of the simulation platform (including common header files and port definitions).

<a id="sec-2-4" name="sec-2-4"></a>

### stim Directory

Indicates the incentive directory of the simulation platform (including incentive data and incentive generation components).

<a id="sec-2-5" name="sec-2-5"></a>

### bfm Directory

Indicates the BFM directory of the simulation platform (including BFM directories of the AXI Master, AXI Slave, and AXI-Lite interfaces).

<a id="sec-2-6" name="sec-2-6"></a>

### rm Directory

Indicates the RM directory of the simulation platform (including RM and scorecard).

<a id="sec-2-7" name="sec-2-7"></a>

### test Directory

Indicates the directory of the simulation platform Env and basic test cases.

<a id="sec-2-8" name="sec-2-8"></a>

### vip Directory

Verifies IPs, including all the verification IPs of the platform.

<a id="sec-2-9" name="sec-2-9"></a>

### xxx_vip Directory

Verification IP directories are stored separately based on verificaiton IPs.

<a id="sec-2-10" name="sec-2-10"></a>

### precompiled Directory

Indicates the pre-compiled library path and includes the pre-compiled Xilinx general simulation model (used to improve the compilation speed).

<a id="sec-2-11" name="sec-2-11"></a>

### vcs_lib Directory

Indicates the pre-compiled library of the VCS.

<a id="sec-2-12" name="sec-2-12"></a>

### questa_lib Directory

Indicates the pre-compiled library of the QuestaSim.

<a id="sec-2-13" name="sec-2-13"></a>

### scripts Directory

Indicates the simulation script directory and contains simulation scripts.

<a id="sec-2-14" name="sec-2-14"></a>

### doc Directory

Indicates the simulation document folder and contains simulation platform descriptions and design documents.

---

<a id="sec-3" name="sec-3"></a>

## Simulation Descriptions

For details about the simulation platform, see the [Simulation Platform Quick Start Guide](./doc/quick_start.md) and the [Simulation Platform User Guide](./doc/user_guide.md).

