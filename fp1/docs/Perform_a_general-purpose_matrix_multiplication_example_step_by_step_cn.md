# 手把手操作通用型矩阵乘法示例


# 1 使用前准备

用户参考用户指南中的第三章节，完成开发套件的下载及镜像的安装配置。用户指南链接如下：

```bash
https://static.huaweicloud.com/upload/files/pdf/20170825/20170825094528_15473.pdf
```

# 2 矩阵乘法实例 HDK操作流程

##### 说明:

本文档基于`huaweicloud-fpga/fp1/`目录进行说明；华为提供的Xilinx软件License 仅限*root*账号使用。

## 2.1 修改`setup.cfg`文件

用户打开`huaweicloud-fpga/fp1/`路径下的`setup.cfg`文件，配置以下参数：

```bash
FPGA_DEVELOP_MODE="sdx"  
VIVADO_VER_REQ="2017.1" 
华北区：XILINX_LIC_SETUP="2100@100.125.1.240:2100@100.125.1.245"
华南区：XILINX_LIC_SETUP="2100@100.125.16.137:2100@100.125.16.138"
华东区：XILINX_LIC_SETUP="2100@100.125.17.108:2100@100.125.17.109"
```

##### 说明:

`XILINX_LIC_SETUP`参数配置需要根据用户申请的虚拟机所处位置确定。

## 2.2 使配置生效

执行`huaweicloud-fpga/fp1/setup.sh`脚本使配置的开发环境生效：

```bash
cd huaweicloud-fpga/fp1
export HW_FPGA_DIR=$(pwd)
source $HW_FPGA_DIR/setup.sh
```

## 2.3 编译矩阵乘法实例以生成xclbin文件

```bash
cd $HW_FPGA_DIR/hardware/sdaccel_design/examples/mmult_hls/scripts
sh compile.sh hw
```
##### 说明:

不需要仿真测试时，请跳过2.4、2.5章节。

## 2.4 进行矩阵乘法实例的CPU仿真

```bash
cd $HW_FPGA_DIR/hardware/sdaccel_design/examples/mmult_hls/scripts
sh run.sh emu ../prj/bin/mmult ../prj/bin/bin_mmult_cpu_emu.xclbin
```

## 2.5 进行矩阵乘法实例的硬件仿真

```bash
cd $HW_FPGA_DIR/hardware/sdaccel_design/examples/mmult_hls/scripts
sh run.sh emu ../prj/bin/mmult ../prj/bin/bin_mmult_hw_emu.xclbin
```


# 3 矩阵乘法实例 SDK操作流程


## 3.1 AEI注册

```bash
cd $HW_FPGA_DIR/hardware/sdaccel_design/examples/mmult_hls/scripts
sh AEI_Register.sh -n "mmult" -d "mmult-test"
```

## 3.2 编译host

```bash
cd $HW_FPGA_DIR/software/app/sdaccel_app/mmult_hls
make
```

## 3.3 硬件测试

```bash
sh run.sh mmult $HW_FPGA_DIR/hardware/sdaccel_design/examples/mmult_hls/prj/bin/bin_mmult_hw.xclbin
```

##### 说明:

`run.sh`脚本包含了文件的加载及硬件测试，因此不需要用户另外单独加载。