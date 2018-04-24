# Example 2 User Guide

---

[切换到中文版](./README_CN.md)

## About This Example

This document describes the components and usage of example 2 provided by Huawei FPGA Accelerated Cloud (FAC) services. This example implements the `DMA data loopback` function. From this example, you can learn about the development, simulation, and test processes on the cloud.

## Directory Structure

The example is stored in `$WORK_DIR/hardware/vivado_design/examples/`. The directory contains the following files and folders:

- **example2/**
  - prj
  - sim
  - src
  - src_encrypt
  - README.md  

## File and Folder Description

- prj

This directory stores Vivado project building information, including user-defined configuration files, building scripts, and the TAR package for generating AEIs after project building.

- sim

This directory stores the **simulation platform** corresponding to this example.

- src

This directory stores the **source code** of this example.

- src_encrypt

This directory stores the source code **encrypted** by Vivado and is used for project building.

- README.md

  This document describes other documents.

## Operation Instructions

### Example Building

Go to the **prj** directory, run the `build.sh` script, and wait until the building is complete.
For details, see [Example 2 Building Guide](./prj/README.md).

The building result is stored in the `/prj/build/checkpoints/to_facs` directory.

The building commands are as follows:

```bash
  $ cd $WORK_DIR/hardware/vivado_design/examples/example2/prj
  $ sh build.sh
```

### Example Simulation

For details, see [Example 2 Simulation Guide](./sim/README.md).

### Example Testing

1. Go to the `$WORK_DIR/software/app/dpdk_app/example2` directory and run the **make** command.
2. After the compilation is complete, perform example tests by referring to [Application Operation Instructions](../../../../software/app/dpdk_app/README.md).

