Using an SDAccel-based Example
========================

[切换到中文版](./Using an SDAccel-based Example_cn.md)

Functions
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

This SDAccel example implements SDAccel-based simulation and hardware instance execution functions. This section describes the operations of the mmult_hls example. For the operations of vadd_cl and vadd_rtl examples, see this section.  

SDAccel HDK Operation Instructions
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

The SDAccel HDK platform serves to complete the compilation and simulation of the development process, and needs to run in the compilation environment. After applying for an SDAccel development environment, log in to a VM. Then, obtain the complete directory of the development suite fp1 from github. Users are advised to store the directory in the huaweicloud-fpga directory. This document is based on the huaweicloud-fpga/fp1/ directory.

### To use the HDK, perform the following steps:

1. Go to the compilation environment for SDAccel development.

##### Note

  The compilation environment must include:  
  fp1 development directory (The configuration part and hardware are mandatory, and other options are optional.)  
  SDx development tool

2.  Configure the license file of EDA.

  Open the `setup.cfg` file in `huaweicloud-fpga/fp1/` and set XILINX_LIC_SETUP to the IP address "2100@100.125.1.240:2100@100.125.1.251" of the license server. Set FPGA_DEVELOP_MODE="sdx"; VIVADO_VER_REQ="2017.1".

  	FPGA_DEVELOP_MODE="sdx"  
  	VIVADO_VER_REQ="2017.1"  
  XILINX_LIC_SETUP="2100@100.125.1.240:2100@100.125.1.251".

##### Note
  **Only user root** has the right to use the Xilinx license file provided by Huawei.

3. Configure the development environment.

  Run the **huaweicloud-fpga/fp1/setup.sh** script to configure the hardware development environment.

  	cd huaweicloud-fpga/fp1
  	export HW_FPGA_DIR=$(pwd)
  	source $HW_FPGA_DIR/setup.sh
##### Note
  For details, see the configuration environment part of the README file in the huaweicloud-fpga/fp1/ directory.

4. Configure the example.
   ```
   cd $HW_FPGA_DIR/hardware/sdaccel_design/examples/mmult_hls/scripts
   sh compile.sh compile_mode
   ```

##### Notes
   You can use the – h parameter to obtain help information in the following steps.  
   *compile_mode* contains three compilation modes:  
   cpu_em: cpu simulation mode; bin_mmult_cpu_emu.xclbin is generated after the compilation is complete.  
   hw_em: hardware simulation mode; bin_mmult_hw_emu.xclbin is generated after the compilation is complete.   
   hw: hardware compilation mode; bin_mmult_hw.xclbin is generated after the compilation is complete.   
   Users can select a compilation mode as required.  
   For details, see: $HW\_FPGA_DIR/hardware/sdaccel_design/examples/mmult/README.md.

5. Simulate the example.
   ```
    cd $HW_FPGA_DIR/hardware/sdaccel_design/examples/mmult_hls/scripts
    sh run.sh emu ../prj/bin/host ../prj/bin/xclbin
   ```
   
##### Notes

   *host* is the host program generated during compilation (The hardware compilation mode does not support simulation.):  
   mmult_hls example: The host program is mmult.  
   vadd_cl example: The host program is vadd.  
   vadd_rtl example: The host program is host.

   *.xclbin* is the .xclbin file generated during compilation. Different compilation modes correspond to different .xclbin files.  
   If the *compile_mode* is cpu_em, select bin_mmult_cpu_emu.xclbin.  
   If the *compile_mode* is hw_em, select bin_mmult_hw_emu.xclbin.  

   Skip this step if the simulation test is not required.

6. Perform the hardware test.

  Select the hw mode as shown in step 4 to compile and generate an.xclbin file, register an image, load the image, and perform the hardware test on the running environment according to the SDAccel SDK process in the following sections.
  For details about how to register an image, see [Registering an FPGA Image](./Registering an FPGA Image.md). For details about how to load the image, see [Loading an FPGA Image](../tools/fpga_tool/docs/load_an_fpga_image.md).

----End

SDAccel SDK Operation Instructions
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

The SDAccel SDK platform is used to test hardware. It needs to compile and run the host program in the execution environment. This section uses mmult_hls as an example to describe the operations.

1.  Go to the execution environment for SDAccel development.

##### Note

  The execution environment must include:  
  fp1 development directory (The configuration part and hardware are mandatory, and other options are optional.)  
  SDx development tool  
  SDAccel hardware

2. Configure the execution environment.

  Run the **huaweicloud-fpga/fp1/setup.sh** script to configure the hardware development environment.

  	cd huaweicloud-fpga/fp1
  	export SW_FPGA_DIR=$(pwd)
  	source $SW_FPGA_DIR/setup.sh
##### Note

  For details, see the configuration environment part of the README file in the huaweicloud-fpga/fp1/ directory.

3.  Go to the directory where the host application is located.

    cd $SW_FPGA_DIR/software/app/sdaccel_app

4. Compile host.

  Go to the directory where the example host is located and run the make command to generate the executable file mmult.

  	cd $SW_FPGA_DIR/software/app/sdaccel_app/mmult_hls
  	make

##### Note

  Host programs for different examples compiled:  
  mmult_hls example: The host program is mmult.  
  vadd_cl example: The host program is vadd.  
  vadd_rtl example: The host program is vadd.

5. Perform the hardware test.

  Run the run.sh command to test the hardware. The detailed procedure is as follows:

  	cd $SW_FPGA_DIR/software/app/sdaccel_app/mmult_hls
  	sh run.sh mmult bin_dir/bin_mmult_hw.xclbin

##### Note

  For details about how to use run.sh, run the sh run.sh -h command.  
  *bin_dir* is the directory storing the .xclbin file generated during the compilation of the SDAccel HDK process in **hw mode**.

----End

