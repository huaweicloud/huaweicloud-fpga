注册
----

[Switch to the English version](./Register_an_FPGA_image_for_an_OpenCL_project.md)

用户使用AEI_Register.sh工具向FPGA镜像管理模块注册FPGA镜像。完成注册后，用户会获得一个FPGA镜像ID，可用于查询FPGA镜像的注册操作是否成功，以及后续的FPGA镜像加载、删除、关联等操作。

### 准备操作

在执行注册操作前，用户需要完成准备操作。

#### 切换到工程的脚本目录。

需要切换到工程中的“scripts”目录。

例如，对于example工程，该目录为“`huaweicloud-fpga/fp1/hardware/sdaccel_design/examples/mmult_hls/scripts`”。

#### 构建工程（若已完成工程的构建，则不需要再重复构建）。

执行`sh compile.sh hw`命令构建工程。

#### 执行注册脚本

AEI_Register.sh脚本的命令格式如下所示。

Usage:sh AEI_Register.sh *-n* [AEI_name] *-d* [AEI_Description]

-   *-n*选项用于指定待注册FPGA镜像的AEI名称（AEI_name）。AEI_name是由英文大小写字母、数字、下划线、中划线组成的字符串，长度为1到64位。

-   *-d*选项用于指定待注册FPGA镜像的AEI描述信息（AEI_Description）。AEI_Description由中文汉字、中文句号逗号、英文大小写字母、数字、中划线、下划线、英文句号逗号、空格组成的字符串，长度为0到255位。

-   AEI_name和AEI_Description参数需要分别用引号括起来，例如  sh AEI_Register.sh -n "ocl-test" -d "ocl-desc"
  
在AEI_Register.sh脚本执行成功后，会产生如下的回显信息。

\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#  
Register AEI  
\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#  
Uploading FPGA image to OBS
Upload 46696040 bytes using 2.038394 seconds
Registering FPGA image to FIS
Success: 200 OK  
id: 0000\*\*\*\*\*\*\*\*5568015e3c87835c0326  
status: saving

-   “Success: 200
    OK”信息表示AEI_Register.sh脚本执行成功。AEI_Register.sh脚本执行成功并不代表FPGA镜像注册成功。用户需要执行fisclient程序的查询列表子命令，并使用注册回显信息中FPGA镜像ID来查询相应的FPGA镜像信息。如果FPGA镜像的状态是“active”，则表示FPGA镜像注册成功。确认注册成功后用户才能进行FPGA镜像的加载操作。

-   “id:
    0000\*\*\*\*\*\*\*\*5568015e3c87835c0326”信息表示FPGA镜像管理为待注册FPGA镜像分配的ID为0000\*\*\*\*\*\*\*\*5568015e3c87835c0326，可用于查询FPGA镜像的注册操作是否成功以及后续的加载等操作。

-   “status: saving”信息表示用户注册的FPGA镜像当前正处于保存状态。


**说明：**AEI_Register.sh脚本在注册过程中会将生成的FPGA逻辑文件传输到用户的OBS桶中，用于注册相应的FPGA镜像。在确认注册成功后，用户可以将OBS桶中的逻辑文件手动删除，以便消除不必要的OBS服务计费。

例如，用户可以执行如下命令来注册一个OCL镜像，并将AEI_name设置为“ocl-test”，将AEI_Description设置为“ocl-desc”。

[root\@ scripts]\# sh AEI_Register.sh -n "ocl-test" -d "ocl-desc"  
fis argument(s) and config file are OK
INFO: OCL Running
#############################################################
Register AEI
#############################################################
Uploading FPGA image to OBS
Upload 46696040 bytes using 2.038394 seconds
Registering FPGA image to FIS
Success: 200 OK
id: ff80808262f26e170162f6dc18d26cc0
status: saving
