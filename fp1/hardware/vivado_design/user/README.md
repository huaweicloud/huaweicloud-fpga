# User Directory Operation Instructions

[切换到中文版](./README_CN.md)

## Directory Structure

- **user/**
  - create_prj.sh
  - usr_prj0
  - README.md

## Directory Description

- create_prj.sh
  - The file carries the execution code for creating a project. `create_prj.sh` is an important part of the command executed when a user creates a project.
  - Run the following command to copy the `project directory templates` to `$WORK_DIR/hardware/vivado_design/usr/` in one click.

    ```bash
    # usr_prjxx is the name of the user project.
    $ sh create_prj.sh usr_prjxx
    ```

  - Run the following command:

    ```bash
    $ sh create_prj.sh -h
    ```

    For details about the command parameters, see the help information of the command.

    ```bash
    ---------------------------------------------------------------------
    Usage: create_prj.sh [option]
    Options:
         -h |-H |-help                    Only for help
         [filename]                       Create [filename] directory
    ---------------------------------------------------------------------
    Example: when you run this command 'sh create_prj.sh usr_prj0' ,
         the directory will be build in '/fp1/hardware/vivado_design/usr/usr_prj0'
    Note   : The [filename] must start with letters, digits, and underscores.
    ```

- usr_prj0

  This folder is an instance of generating user project `usr_prj0` by running the `sh create_prj.sh usr_prj0` command.

- README.md

  This document describes other documents.

## Creating a User Project

There are two methods for creating a user project:

- Copy an example project, modify parameters, and add functional modules to meet user requirements.
- Create a new project and add user-developed code to the compiling environment to meet user requirements.

The first method allows a user project to be quickly implemented, and the second method requires users to compile code.

### Implementing a User Project Quickly

#### Configuring License and Tool Information

- Open the **setup.cfg** file:

```bash
  $ vim setup.cfg
```

- Configure **FPGA_DEVELOP_MODE**:

  If SDAccel is used, configure it to **FPGA_DEVELOP_MODE="sdx".**
  If Vivado is used, configure it to **FPGA_DEVELOP_MODE="vivado"**.
  **FPGA_DEVELOP_MODE="vivado"** is the default configuration.

- Configure the software license:

  Obtain the XILINX License from Huawei official website. The following is a configuration example:

```bash
  "XILINX_LIC_SETUP="2100@100.xxx.yyy.zzz:2100@100.xxx.yyy.zzz"(100.xxx.yyy.zzz is the IP address of the license.)
```

- Configure **VIVADO_VER_REQ**:

  If SDAccel is used, configure it to **VIVADO_VER_REQ="2017.1"**.
  If Vivado is used, configure it to **VIVADO_VER_REQ="2017.2"**.
  **VIVADO_VER_REQ="2017.2"** is the default configuration.

---

#### Configuring Environment Variables

  ```bash
  $ source $WORK_DIR/setup.sh
  ```

Each time the <kbd>source setup.sh</kbd> command is executed, the HDK takes the following steps:

1. Check whether the license files of all tools are configured and whether the tools are installed. (By default, the tools and license are not installed.)
2. Notify users whether the tools are installed.
3. Print the version information about all installed tools.

**Note** 

If the project is installed for the first time or the version is upgraded, in addition to the preceding three steps, the HDK takes the following steps:

1. Pre-compile the VCSMX simulation library (if the VCSMX tool exists).
2. Pre-compile the QuestaSim simulation library (if the QuestaSim tool exists).
3. Use the Vivado tool to generate an IP and a DDR simulation model.
4. Download the .dcp file and compressed package from the OBS bucket. This process takes about three to five minutes.

---

#### Copying a Project Directory Template

  Enter the following command in `$WORK_DIR/hardware/vivado_design/user`:

  ````bash
  # usr_prjxx is the name of the user project.
  $ sh create_prj.sh usr_prjxx
  ````

 The template project folder and files can be copied to `$WORK_DIR/hardware/vivado_design/usr/usr_prjxx` in one-click mode.

---

#### Configuring usr_prj_cfg

The file is used to configure user-defined information of a user project.
Run the following command to open the `usr_prj_cfg` file in `$WORK_DIR/hardware/vivado_design/user/usr_prjxx/prj/`:

```bash
  $ vim $WORK_DIR/hardware/vivado_design/user/usr_prjxx/prj/usr_prj_cfg
```

For details, see `$WORK_DIR/hardware/vivado_design/user/usr_prjxx/README.md`.

#### Adding a Block

- Open `vivado_design/lib/common/`. The common CBBs of FIFO and RAM are available in the path. You can open the CBBs to view the code and search for RAM files or FIFO design files.
  For details about CBBs provided by the platform and their functions, see `vivado_design/lib/common/README.md`.
- Select the required block, copy and add it to `vivado_design/user/usr_prjxx/src`, and modify parameters as required.

#### Completing Building Through the build.sh Script

This command is used to compile a project created in one-click mode, implementing synthesis, placing and routing, PR verification, and bit file generation , and RTL building.
If the RTL building in one-click mode has been done for a user project, this command can also be used for single-step execution of a compilation task.

For details, see [usr_prj0 Building Guide](./usr_prj0/README.md).

#### Uploading a .bit File

After the RTL building, the generated binary files are stored in the `$WORK_DIR/hardware/vivado_design/user/usr_prjxx/prj/build/checkpoints/to_facs` directory. The folder contains the following files:

- usr_prjxx_partial.bin
- usr_prjxx_partial.bit
- usr_prjxx_routed.dcp

Finally, run the `AEI_Register.sh` command to upload files. The system uploads the required loading files `usr_prjxx_routed.dcp and usr_prjxx_partial.bin` to the storage bucket, returns the AEI, and then loads the files.
For details, see [usr_prj0 Building Guide](./usr_prj0/README.md).

