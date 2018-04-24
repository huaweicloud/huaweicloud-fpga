注册
----

[Switch to the English version](./Register_an_FPGA_image_for_a_DPDK_project.md)

用户使用AEI_Register.sh工具向FPGA镜像管理模块注册FPGA镜像。完成注册后，用户会获得一个FPGA镜像ID，可用于查询FPGA镜像的注册操作是否成功，以及后续的FPGA镜像加载、删除、关联等操作。

### 准备操作

在执行注册操作前，用户需要完成准备操作。

#### 切换到工程的脚本目录。

需要切换到工程中的“prj”目录。

例如，对于example工程，该目录为“`huaweicloud-fpga/fp1/hardware/vivado_design/examples/example1/prj`”。


#### 构建工程（若已完成工程的构建，则不需要再重复构建）。

执行`sh build.sh`命令构建工程。

编辑工程脚本目录下的`AEI_Register.cfg`文件。将文件中的**OBS_BUCKETNAME**选项的内容配置为在配置章节中创建的OBS桶名，**MODE**选项使用默认值，无需重新配置。

**说明：**配置章节可参考根目录下面README.md中1.2.2 修改配置文件和配置镜像章节。

配置后的信息如下回显所示。

    MODE=DPDK  
    OBS_BUCKETNAME=obs-fpga

\----结束

#### 执行注册脚本

AEI_Register.sh脚本的命令格式如下所示。

Usage:sh AEI_Register.sh *-n* [AEI_name] *-d* [AEI_Description]

-   *-n*选项用于指定待注册FPGA镜像的AEI名称（AEI_name）。AEI_name是由英文大小写字母、数字、下划线、中划线组成的字符串，长度为1到64位。

-   *-d*选项用于指定待注册FPGA镜像的AEI描述信息（AEI_Description）。AEI_Description由中文汉字、中文句号逗号、英文大小写字母、数字、中划线、下划线、英文句号逗号、空格组成的字符串，长度为0到255位。

在AEI_Register.sh脚本执行过程中，用户需要根据提示信息输入AK，SK和密码。

-  在出现**Input access_key:**信息时，输入在配置章节中 获取OBS配置参数获取的AK。

-  在出现**Input secret_key:**信息时，输入在配置章节中 获取OBS配置参数获取的SK。

-  在出现**Input passwd:**信息时，输入华为云账户的密码。

在AEI_Register.sh脚本执行成功后，会产生如下的回显信息。

\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#  
Register AEI  
\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#  
Success: 200 OK  
id: 0000\*\*\*\*\*\*\*\*5568015e3c87835c0326  
status: saving

-   “Success: 200
    OK”信息表示AEI_Register.sh脚本执行成功。AEI_Register.sh脚本执行成功并不代表FPGA镜像注册成功。用户需要执行fisclient程序的查询列表子命令，并使用注册回显信息中FPGA镜像ID来查询相应的FPGA镜像信息。如果FPGA镜像的状态是“active”，则表示FPGA镜像注册成功。确认注册成功后用户才能进行FPGA镜像的加载操作。

-   “id:
    0000\*\*\*\*\*\*\*\*5568015e3c87835c0326”信息表示FPGA镜像管理为待注册FPGA镜像分配的ID为0000\*\*\*\*\*\*\*\*5568015e3c87835c0326，可用于查询FPGA镜像的注册操作是否成功以及后续的加载等操作。

-   “status: saving”信息表示用户注册的FPGA镜像当前正处于保存状态。


**说明：**AEI_Register.sh脚本在注册过程中会将生成的FPGA逻辑文件传输到用户的OBS桶中，用于注册相应的FPGA镜像。在确认注册成功后，用户可以将OBS桶中的逻辑文件手动删除，以便消除不必要的OBS服务计费。

例如，用户可以执行如下命令来注册一个DPDK镜像，并将AEI_name设置为“DPDK-test”，将AEI_Description设置为“DPDK-desc”。

**说明：**执行以下命令运行约20分钟左右。
[root\@ scripts]\# sh AEI_Register.sh -n "DPDK-test" -d "DPDK-desc"  
fischeck arguments are OK  
**Input access_key:**  
**Input secret_key:**  
45837484 1 objects s3://obs-fpga/  
verifying the access_key,secret_key successfully  
**Input passwd:**fischeck password and config file are OK  
verifying the password and /etc/cfg.file successfully  
INFO: DPDK Running

... ...

write_bitstream completed successfully
write_bitstream: Time (s): cpu = 01:12:43 ; elapsed = 00:22:32 . Memory (MB): peak = 10133.406 ; gain = 3079.641 ; free physical = 2149 ; free virtual = 10317
INFO: [Common 17-206] Exiting Vivado at Sun Apr  8 17:15:03 2018...
upload: '/home/fp1/hardware/vivado_design/examples/example3/prj/build/checkpoints/to_facs/pr_ul_20180408163826_aei.bin' -> 's3://obs-yx-fpga/pr_ul_20180408163826_aei.bin'  [part 1 of 4, 15MB] [1 of 1]
 15728640 of 15728640   100% in    0s    21.30 MB/s  done
upload: '/home/fp1/hardware/vivado_design/examples/example3/prj/build/checkpoints/to_facs/pr_ul_20180408163826_aei.bin' -> 's3://obs-yx-fpga/pr_ul_20180408163826_aei.bin'  [part 2 of 4, 15MB] [1 of 1]
 15728640 of 15728640   100% in    0s    21.04 MB/s  done
upload: '/home/fp1/hardware/vivado_design/examples/example3/prj/build/checkpoints/to_facs/pr_ul_20180408163826_aei.bin' -> 's3://obs-yx-fpga/pr_ul_20180408163826_aei.bin'  [part 3 of 4, 15MB] [1 of 1]
 15728640 of 15728640   100% in    0s    21.44 MB/s  done
upload: '/home/fp1/hardware/vivado_design/examples/example3/prj/build/checkpoints/to_facs/pr_ul_20180408163826_aei.bin' -> 's3://obs-yx-fpga/pr_ul_20180408163826_aei.bin'  [part 4 of 4, 3MB] [1 of 1]
 3432084 of 3432084   100% in    0s    11.57 MB/s  done 
\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#  
Register AEI  
\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#  
**Success: 200 OK id: ff808082628ffc7a0162a48d452a760c status: saving**
