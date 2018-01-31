Configuring the Development Environment
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
[切换到中文版](./SDAccel_based_SDK_Configuration_and_Compilation_cn.md)

Compile and run the host program in the execution environment.
```
  cd huaweicloud-fpga/fp1
  source setup.sh
  export SW_FPGA_DIR=$(pwd)
```

Compiling the Host Program
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

  Create a user directory by referring to the examples mmult_hls, vadd_cl, or vadd_rtl, and modify and compile Makefile according to the user-developed host code and compilation options.

```
  cd $SW_FPGA_DIR/software/app/sdaccel_app/usr_host
  make
```

  After the compilation is complete, the executable host file is generated in this directory.

##### Note

  *usr_host* indicates the user directory, which is created by users. Source files, Makefile, or scripts compiled by users must be stored in this directory.
  You can also copy and rename the example directory and use it as the *usr_host* directory.


----End

