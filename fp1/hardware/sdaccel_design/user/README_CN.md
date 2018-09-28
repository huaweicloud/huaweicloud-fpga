# 目录结构

[Switch to the English version](./README.md)

* [sdaccel_design/user](#sdaccel_design/user)/  
  - create_prj.sh
  - README_CN.md 

# 目录说明  
* create_prj.sh  
  - 该文件承载的是用户创建工程的执行代码，`create_prj.sh`是用户创建工程时，执行命令的重要组成部分;
  - 执行如下命令，实现一键式将`工程目录模板`复制到`/fpga_design/hardware/sdaccel_design/user/`：

    ````bash
    # usr_prjxx为用户工程的名称
    # mode为kernel类型，可选择temp_cl/temp_c/temp_rtl
    $ sh create_prj.sh usr_prjxx temp_cl   
    ````

  - 执行如下命令可获取命令相关的帮助：  

    ````bash
    $ sh create_prj.sh -h
    ````

# 用户工程创建方法
  用户工程创建方法有两种：
  
  * 复制实例工程，通过更改参数，添加功能模块实现用户需求；
  
  * 自己创建工程，上传用户自己设计的代码，添加在编译环境中，实现用户需求；

  * 前者可快速实现一个用户工程，后者需要用户自己编写代码创建工程。

