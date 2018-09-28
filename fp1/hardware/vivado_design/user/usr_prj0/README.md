# usr_prj0 User Guide

---

[切换到中文版](./README_CN.md)


## Directory Structure

The directory is `$WORK_DIR/hardware/vivado_design/user/usr_template/` and contains the following files and folders:

- **usr_prj0/**
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

Go to the prj directory, run the `build.sh` script, and wait until the building is completed.
For building details, see [usr_prj0 Building Guide](./prj/README.md).

The building result is stored in the `/prj/build/checkpoints/to_facs` directory.

The building commands are as follows:

```bash
  $ cd $WORK_DIR/hardware/vivado_design/user/usr_prj0/prj
  $ sh build.sh
```

### Example Simulation

For simulation details, see [usr_prj0 Simulation User Guide](./sim/README.md).

