# 手把手操作高性能型DMA环回实例


# 1 使用前准备

用户参考用户指南中的第三章节，完成开发套件的下载及镜像的安装配置。用户指南链接如下：

```bash
https://static.huaweicloud.com/upload/files/pdf/20170825/20170825094528_15473.pdf
```

# 2 DMA环回实例 HDK操作流程

##### 说明:

本文档基于`huaweicloud-fpga/fp1/`目录进行说明；华为提供的Xilinx软件License 仅限*root*账号使用。

## 2.1 修改`setup.cfg`文件

用户打开`huaweicloud-fpga/fp1/`路径下的`setup.cfg`文件，配置以下参数：

```bash
FPGA_DEVELOP_MODE="vivado"  
VIVADO_VER_REQ="2017.2" 
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

## 2.3 编译DMA环回实例以生成dcp文件

```bash
cd $HW_FPGA_DIR/hardware/vivado_design/examples/example2/prj
sh ./build.sh
```

##### 说明:

不需要仿真测试时，请跳过2.4章节。

## 2.4 DMA环回实例仿真测试

```bash
cd $HW_FPGA_DIR/hardware/vivado_design/examples/example2/sim
make TC=sv_demo_001
```

# 3 DMA环回实例 SDK操作流程

## 3.1 AEI注册

```bash
cd $HW_FPGA_DIR/hardware/vivado_design/examples/example2/prj
sh AEI_Register.sh -p "vu9p/abc.tar" -o "vu9p" -n "ocl-test" -d "ocl-desc"
```

## 3.2 加载FPGA镜像

参考下面文档中步骤1至步骤5对镜像进行加载：

```bash
cd $HW_FPGA_DIR/tools/fpga_tool/docs/
vim load_an_fpga_image_cn.md
```

## 3.3 编译DMA环回实例的SDK部分

```bash
cd $HW_FPGA_DIR/software/app/dpdk_app/
chmod +x build_dpdk_app.sh
source build_dpdk_app.sh
```

## 3.4 设置环境变量

```bash
cd huaweicloud-fpga/fp1
export HW_FPGA_DIR=$(pwd)
export LD_LIBRARY_PATH=$HW_FPGA_DIR/software/userspace/dpdk_src/dpdk-16.04/x86_64-native-linuxapp-gcc/lib:$HW_FPGA_DIR/software/userspace/dpdk_src/securec/lib:$LD_LIBRARY_PATH
```

## 3.5 进入APP可执行文件目录设置hugepage个数

```bash
cd $HW_FPGA_DIR/software/app/dpdk_app/bin
sysctl -w vm.nr_hugepages=8192
```

## 3.6 DMA环回实例测试

```bash
./packet_process -d 8192 -q 0 -l 512 -n 102400099
```

**输出打印结果示例**  

	[root@CentOS7 bin]# ./packet_process -d 8192 -q 0 -l 512 -n 102400099
	available cpu number: 24, cpu mask parameter: -cffffff
	...
	----------------TEST TIME 0 for port 0----------------
	Not find mempool sec_mp_bd_0_0, create it
	mempool sec_mp_bd_0_0 has 2048 available entries
	Not find mempool sec_mp_data_0_0, create it
	mempool sec_mp_data_0_0 has 4096 available entries
	---------------- test for port 0, queue 0,  ----------------
	...
	port 0, queue 0 run_business_rx_thread_route finish, time 7627967(us)
	----------------port 0, queue 0 rx_packet_len 512, packet_num: 102400099, performance = 54 gbps----------------
	port 0, queue 0 TX/RX all success, all process 102400099 packets
	...