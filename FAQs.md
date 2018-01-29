# Huawei Cloud FPGA - Frequently Asked Questions

[概况 FAQs](#概况) 

[应用市场 FAQs](#应用市场)

[开发语言 FAQs](#开发语言)

[FPGA  FAQs](#fpga)

[FPGA Shell FAQs](#fpga shell) 

[疑难解答](#疑难解答)


## 概况

**Q: 什么是HDK?**

HDK是Hardware Development Kit的缩写，即硬件开发套件。HDK主要包括RTL（Verilog/VHDL）设计、验证到构建的全部设计文件以及脚本。

**Q: 什么是SDK?**

SDK是Software Development Kit的缩写，即软件开发套件。SDK主要包含运行FPGA实例所需要的驱动、工具、运行环境以及应用程序。


## 应用市场
**Q: Huawei Cloud FPGA Marketplace是什么？**

FPGA开发人员可以使用Marketplace将他们的AEI文件共享或出售给其他Huawei Cloud FPGA用户。 


## 开发语言
**Q: Huawei Cloud FPGA支持哪些开发语言?**

目前Huawei Cloud FPGA有通用型（OpenCL）和高性能型（DPDK）两种，OpenCL主要采用C语言开发，而DPDK主要使用Verilog硬件语言开发。


## FPGA
**Q: Huawei Cloud FPGA使用的FPGA型号是什么?**

目前Huawei Cloud FPGA使用的FGPA是Xilinx公司的Ultra Scale+系列xcvu9p-flgb2104-2-i板卡。


## FPGA Shell
**Q: 什么是Shell？**

Shell是HDK提供的静态逻辑部分，包括PCIe、DDR4等外围接口设计。

**Q: Shell的接口协议是axi4吗?**

不完全是。目前Shell接口使用的协议有axi4，axi4-s，axi4-l三种协议，主要和不同接口处理的数据类型不同有关。

## 疑难解答
**Q: 为什么运行发包程序packet_process出现错误"Current CONFIG_RTE_MAX_MEMSEG=256 is not enough"?** 

**现象原因：** 
该现象是大页分配失败，原因是内存碎片过多，造成分散的hugepage过多，导致DPDK对hugepage初始化失败。 

**解决方法：** 
- 尽量在系统启动时就配置hugepage(通过`sysctl -w vm.nr_hugepages=8192`命令)，这个时候系统的内存碎片较少，在这个时候配置好了hugepage，dpdk初始化就不会因为内存碎片过多而失败。 
- 修改虚拟机的Linux系统的启动参数，在grub文件中添加启动参数`default_hugepagesz=2M hugepagesz=2M hugepages=8192`，具体步骤如下。 
  a.	编辑`/etc/default/grub`文件。  
  `GRUB_CMDLINE_LINUX=“………”`中添加配置，改为`GRUB_CMDLINE_LINUX =“……… default_hugepagesz=2M hugepagesz=2M hugepages=8192”`  
  b.	更新虚拟机grub。  
  `grub2-mkconfig > /boot/grub2/grub.cfg`  
  c.	重启虚拟机。  

**Q: 为什么运行发包程序packet_process出现错误"Cannot init data mbuf pool for port "?** 

**产生该现象的两种情况：**  
-  包长较长，队列较多时默认配置的8192个hugepage数量不够，导致连续内存不够用，例如使用包长1M，8队列运行packet_process例程时会出现该问题。  
-  使用包长10M，配置24576个huage之后，连续内存足够的情况下，单队列运行pakcet_process例程时报错该问题。 

**原因：**   
- 对于第一种情况，由于申请的hugepage为8192，可使用连续物理内存为16G，如果发包时设置队列数量过多，包长过长时，导致总内存需求大于16G，就会导致连续内存不足，申请mbuf pool失败。  
- 对于第二种情况，由于系统运行过程中DPDK连续内存产生了碎片，导致初始化时无法申请到足够数量的10M大小连续内存。 

**解决方法：** 
-  对于第一种情况：  
  a. 在设备总物理内存充足的条件下，配置申请更大的hugepage，例如，需支持最大负荷(8队列，1M包长)时需要配置hugepage数量为24576(即申请连续物理内存48G)，配置命令: `sysctl -w vm.nr_hugepages=24576`。  
  b. 根据连续内存总量合理分配包长与队列数量，包长较长时同时使用少量队列发包，或多队列发包时不使用过长包，如: 包长1M时一条队列需要内存大小为BD消息所占内存(预留1G即可)再加上： 

    收包队列：1M * 2048 Bytes，若FMMU，则为(1M + 32) * 2048 Bytes  
    发包队列：1M * 2048 Bytes，若FMMU，则为(1M + 32) * 2048 Bytes  
  每条队列约2G，1M大小时默认配置的8192个huagepage，即16G连续物理内存仅支持同时3条队列运行。  
-  对于第二种情况： 
  重启虚拟机并在启动之后配置24576个hugepage，命令`sysctl -w vm.nr_hugepages=24576`，再次尝试发包即可。 

**Q：使用Vivado软件工具后产生的license缓存文件有安全风险，应该如何处理？**

Xilinx的FPGA设计工具Vivado，使用时会检查license，并在
`/tmp/FLEXnet/` 
`/usr/local/share/macrovision/storage/`
`/usr/local/share/macrovision/storage/FLEXnet`
三个目录下产生缓存文件。由于这些缓存文件具有全局可写权限，所以其数据可以被系统中的任何用户修改和破坏，可能对系统造成危害。

**处理方式**：这些文件仅在Vivado软件检查license时使用，并且每次使用都会重新生成，所以建议用户每次使用Vivado工具后，手动或编写脚本删除这3个目录下的所有文件，删除这些文件不会对下一次使用Vivado软件造成影响。
