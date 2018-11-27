# Huawei Cloud FPGA - Frequently Asked Questions
[Switch to the English version](./FAQs.md)

# 目录

[概况 FAQs](#sec_1) 

[应用市场 FAQs](#sec_2)

[开发语言 FAQs](#sec_3)

[FPGA  FAQs](#sec_4)

[FPGA Shell FAQs](#sec_5) 

[疑难解答](#sec_6)

[FpgaCmdEntry FAQs](#sec_7)

[高危操作](#sec_8)

<a name="sec_1"></a>
## 概况

**Q: 什么是HDK?**

HDK是Hardware Development Kit的缩写，即硬件开发套件。HDK主要包括RTL（Verilog/VHDL）设计、验证到构建的全部设计文件以及脚本。

**Q: 什么是SDK?**

SDK是Software Development Kit的缩写，即软件开发套件。SDK主要包含运行FPGA实例所需要的驱动、工具、运行环境以及应用程序。

<a name="sec_2"></a>
## 应用市场
**Q: Huawei Cloud FPGA Marketplace是什么？**

FPGA开发人员可以使用Marketplace将他们的AEI文件共享或出售给其他Huawei Cloud FPGA用户。 

<a name="sec_3"></a>
## 开发语言
**Q: Huawei Cloud FPGA支持哪些开发语言?**

目前Huawei Cloud FPGA有通用型（OpenCL）和高性能型（DPDK）两种，OpenCL主要采用C语言开发，而DPDK主要使用Verilog硬件语言开发。

<a name="sec_4"></a>
## FPGA
**Q: Huawei Cloud FPGA使用的FPGA型号是什么?**

目前Huawei Cloud FPGA使用的FGPA是Xilinx公司的Ultra Scale+系列xcvu9p-flgb2104-2-i板卡。

<a name="sec_5"></a>
## FPGA Shell
**Q: 什么是Shell？**

Shell是HDK提供的静态逻辑部分，包括PCIe、DDR4等外围接口设计。

**Q: Shell的接口协议是axi4吗?**

不完全是。目前Shell接口使用的协议有axi4，axi4-s，axi4-l三种协议，主要和不同接口处理的数据类型不同有关。

<a name="sec_6"></a>
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

**Q: 为什么运行发包程序packet_process出现错误"Cannot init data mbuf pool for port "或"Current CONFIG_RTE_MAX_MEMSEG=256 is not enough"或"Aborted(core dumped)" ?** 

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

**Q：如何设计出高性能高质量的代码？**

高性能架构FPGA开发请参考https://www.xilinx.com/support/documentation/sw_manuals/xilinx2018_1/ug949-vivado-design-methodology.pdf

通用型架构FPGA开发请参考https://www.xilinx.com/support/documentation/sw_manuals/xilinx2017_4/ug1207-sdaccel-optimization-guide.pdf

<a name="sec_7"></a>
## FpgaCmdEntry
**Q: 什么是FpgaCmdEntry？**

FpgaCmdEntry是用户侧在虚拟机上对分配FPGA卡进行操作的工具，该工具支持用户对FPGA卡进行加载、清除、查询等操作。

**Q: 为什么运行FpgaCmdEntry时会出现错误"Commond busy"或者"Commond busy"?**

**现象原因：** 
该现象是由于当前FPGA卡正在执行其它任务导致。 

**解决方法：** 
等待30s~5min之后再执行FpgaCmdEntry命令。

**Q: 为什么运行FpgaCmdEntry加载命令时会出现错误"invalid AEI ID"?**

**现象原因：** 
该现象是由于用户输入的AEI ID与OBS桶中的AEI ID不一致导致。 

**解决方法：** 
检查输入的AEI ID是否正确，修改之后进行重试。如若继续提示"invalid AEI ID"，联系FS运维人员进行恢复。

**Q: 为什么运行FpgaCmdEntry加载命令时会出现错误"internal error, please try FpgaCmdEntry IF -S <slot num> for details"?**

**现象原因：** 
该现象是由于用户输入的AEI文件头或者AEI文件存在问题导致。 

**解决方法：** 
根据github上面的readme核对AEI加载文件、注册流程是否有问题，注册工具是否是最新的版本，重新注册AEI ID后进行加载。如若失败现象保持不变，联系FS运维人员进行恢复。

<a name="sec_8"></a>
## 高危操作

**Q：在OCL SDK操作中执行xbsk test过程中按ctrl+c按键，再次执行xbsk test时会失败**

禁止执行此操作。如果不小心已执行了，则需要重新安装xdma驱动才能恢复。

**Q：在OCL SDK操作中执行example的过程中按ctrl+c按键，再次执行example时会失败**

禁止执行此操作。如果不小心已执行了，则用FpgaCmdEntry工具手动执行加载一次才能恢复。


