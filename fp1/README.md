# fp1 Development Suite Description

[切换到中文版](./README_CN.md)

---

## Contents
-------
1. [fp1 Development Suite](#sec-1)
2. [Directory Structure](#sec-2)
3. [Contents Description](#sec-3)
4. [HDK Running Process](#sec-4)
   * [4.1 License and Environment Variable Configuration](#sec-4-1)
   * [4.2 Designing and Simulation](#sec-4-2)
   * [4.3 User Application Compilation and Debugging](#sec-4-3)
   * [4.4 Uploading and Registration](#sec-4-4)
   * [4.4 Loading and Running](#sec-4-5)

</div>

<a id="sec-1" name="sec-1"></a>
## fp1 Development Suite

---

**fp1** is an open-source and cloud-based FPGA hardware/software development tool suite based on FACS. This suite helps users to design, simulate, implement, and jointly run FPGA, and provides professional design and verification components to help users to efficiently implement FPGA development.

**fp1** provides tools for **hardware** (**HDK**) and **software** (**SDK**) development respectively. The HDK provides all designs and scripts for RTL (Verilog/VHDL) designing, verifying, and building. The SDK provides FPGA example running environment, drivers, tools, and applications.

<a id="sec-2" name="sec-2"></a>

## Directory Structure

---

- **fp1/**
  - [hardware/](#sec-3-1)
  - [software/](#sec-3-2)
  - [tools/](#sec-3-3)
  - [docs/](#sec-3-4)
  - [release_notes.md](#sec-3-5)
  - [fpga_tool_setup.sh](#sec-3-6)
  - [fpga_tool_unistall.sh](#sec-3-7)
  - [setup.cfg](#sec-3-8)
  - [setup.sh](#sec-3-9)
  - README.md

<a id="sec-3" name="sec-3"></a>

## Contents Description

---

<a id="sec-3-1" name="sec-3-1"></a>

- hardware

  This directory stores fp1 hardware development suite, including Vivado and SDAccel tools. Vivado supports VHDL and Verilog for development, while SDAccel supports C, C++, and OpenCL for development.

  For details, see [hardware README](./fp1/hardware/README.md).

<a id="sec-3-2" name="sec-3-2"></a>

- software

  This directory stores fp1 software development suite, including example running environment, drivers, tools, and applications.

  For details, see [software README](./fp1/software/README.md).

<a id="sec-3-3" name="sec-3-3"></a>

- tools

  This directory stores tools for fp1 FPGA development. Currently, only the FPGA image loading tool is available.

<a id="sec-3-4" name="sec-3-4"></a>

- docs

  This directory stores FPGA development suite guides, including hardware development process and example operation instructions.

<a id="sec-3-5" name="sec-3-5"></a>

- release_notes.md

  This document provides fp1 operation instructions, including project building, user simulation, and application tests.

<a id="sec-3-6" name="sec-3-6"></a>

- fpga_tool_setup.sh

  This script is used to configure and install the FPGA image loading tool. Run this script before using the FPGA image loading tool.

<a id="sec-3-7" name="sec-3-7"></a>

- fpga_tool_unistall.sh

  This script is used to uninstall the FPGA image loading tool.

<a id="sec-3-8" name="sec-3-8"></a>

- setup.cfg

  This user configuration file stores license configurations and Vivado version configurations.

<a id="sec-3-9" name="sec-3-9"></a>

- setup.sh

  This script is used to configure environment variables. Run this script before using the development suite.

<a id="sec-4" name="sec-4"></a>

## HDK Running Process

---

The HDK running process is classified into Vivado-based process and SDAccel-based process.
Vivado and SDAccel tools share the same method to configure the license and environment variables, but use different paths and examples for development and simulation.

The following are common steps for Vivado and SDAccel development:

1. Configuring the license and environmental variables.
2. Designing and simulating.
3. Designing and debugging applications.
4. Loading and running.

<a id="sec-4-1" name="sec-4-1"></a>

### License and Environment Variable Configuration

- Open the **setup.cfg** file:

```bash
  $ vim setup.cfg
```

- Configure **FPGA_DEVELOP_MODE**:

  If SDAccel is used, set **FPGA_DEVELOP_MODE="sdx"**.
  If Vivado is used, set **FPGA_DEVELOP_MODE="vivado"**.
  **FPGA_DEVELOP_MODE="vivado"** is the default configuration.

- Configure the software license:

  Obtain Xilinx License from the Huawei official website. The following is a configuration example:

```bash
  "XILINX_LIC_SETUP="2100@100.xxx.yyy.zzz:2100@100.xxx.yyy.zzz" (100.xxx.yyy.zzz indicates the IP address of the license.)
```

- Configure **VIVADO_VER_REQ**:

  If SDAccel is used, set **VIVADO_VER_REQ="2017.1"**.
  If Vivado is used, set **VIVADO_VER_REQ="2017.2"**.
  **VIVADO_VER_REQ="2017.2"** is the default configuration.

- Configure environment variables:

```bash
  $ source setup.sh
```

**Note**

The fislcint tool must be installed before executing the `setup.sh` script。

Each time the <kbd>source setup.sh</kbd> command is executed, the HDK executes the following steps:

1. Check whether the license files of all tools are configured and whether the tools are installed. (By default, the tools and license are not installed.)
2. Notify users whether the tools are installed.
3. Print version information about all installed tools.

**Note**

If the project is installed for the first time or the version is upgraded, in addition to the preceding three steps, the HDK executes the following steps:

1. Precompile the VCSMX simulation library (if the VCSMX tool exists).
2. Precompile the QuestaSim simulation library (if the QuestaSim tool exists).
3. Use the Vivado tool to generate an IP and a DDR simulation model，OpenCL calls SDX tools and DSA and other compressed packages.
4. Download the .dcp file and compressed package from the OBS bucket.OpenCL downloads DSA files and archives in OBS buckets.This process takes about three to five minutes.

<a id="sec-4-2" name="sec-4-2"></a>

### Designing and Simulation

The HDK supports designing and simulation using `Vivado` and `SDx` tools. For details, see **Simulating User Logic** in the [hardware Description](./hardware/README.md).

<a id="sec-4-3" name="sec-4-3"></a>

### User Application Compilation and Debugging

If different development modes are used, this step is slightly different.

- Vivado Development Mode

Vivado development mode uses the DPDK architecture to exchange data between the FPGA and processors. For details about how to compile and debug user applications, see [DPDK-based User Applications Development Description](./software/app/dpdk_app/README.md).
If you need to modify the driver, see [DPDK-based Driver Development Description](./software/userspace/dpdk_src/README.md).

- SDx Development Mode

SDx development mode uses the Xilinx SDAccel architecture to exchange data between the FPGA and processors. For details about how to compile and debug user applications, see [Using an SDAccel-based Example](./docs/Using_an_SDAccel_based_Example.md).

<a id="sec-4-4" name="sec-4-4"></a>

### Uploading and Registration

You can upload a .bin file and generate the registration ID based on the APIs and the uploading and registration tools provided by the FAC services. For details, see sections "AEI_Register.sh Operation Instructions" in [usr_prj0 Compilation Guide](./hardware/vivado_design/user/usr_prj0/prj/README.md).

<a id="sec-4-5" name="sec-4-5"></a>

### Loading and Running

You can load the .bin file based on the APIs provided by the FAC services and the registration ID generated during the .bin file upload. For details, see related documents.

Running the .bin file is similar to **compiling and debugging user applications**. For details, see section [Compiling and Debugging User Applications](#sec-4-3) in this document.

