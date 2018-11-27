# Huawei Cloud FPGA - Frequently Asked Questions
[切换到中文版](./FAQs_cn.md)

# Contents
[Overview FAQs](#sec_1) 

[Application Market FAQs](#sec_2)

[Development Language FAQs](#sec_3)

[FPGA FAQs](#sec_4)

[FPGA Shell FAQs](#sec_5) 

[Troubleshooting](#sec_6)

[FpgaCmdEntry FAQs](#sec_7)

[High-Risk Operations FAQs](#sec_8)

<a name="sec_1"></a>
## Overview

**Q: What Is HDK?**

HDK is short for Hardware Development Kit. The HDK provides all designs and scripts for RTL (Verilog/VHDL) designing, verifying, and building.

**Q: What Is SDK?**

SDK is short for Software Development Kit. The SDK provides FPGA example running environment, drivers, tools, and applications.

<a name="sec_2"></a>
## Application Market
**Q: What is Huawei Cloud FPGA Marketplace?**

FPGA developers can share or sell their AEI files to other FPGA users on the Huawei Cloud FPGA Marketplace. 

<a name="sec_3"></a>
## Development Language
**Q: What development languages does Huawei Cloud FPGA support?**

Currently, two types of Huawei Cloud FPGA are available: general-purpose (OpenCL) and high-performance (DPDK). OpenCL is developed by using the C language, and the DPDK is developed by using the Verilog hardware language.

<a name="sec_4"></a>
## FPGA
**Q: What is the FPGA model used by Huawei Cloud FPGA?**

Currently, Huawei Cloud FPGA uses Xilinx UltraScale+ series xcvu9p-flgb2104-2-i cards.

<a name="sec_5"></a>
## FPGA Shell
**Q: What is Shell?**

Shell is the static logic provided by the HDK, including peripheral interfaces such as PCIe and DDR4.

**Q: Does Shell use AXI4 as its interface protocol?**

Not exactly. Currently, Shell uses AXI4, AXI4-Stream, and AXI4-Lite protocols to process different types of data.

<a name="sec_6"></a>
## Troubleshooting
**Q: "Current CONFIG_RTE_MAX_MEMSEG=256 is not enough" was displayed when I ran the packet_process packet sending program. What do I do?** 

**Causes:** 
The huge page allocation fails due to excessive memory fragments. As a result, the number of huge pages is too large, and the DPDK fails to initialize the huge pages. 

**Solutions:** 
- Run the `sysctl -w vm.nr_hugepages=8192` command to configure huge pages when the system is started and few memory fragments exist. Otherwise, the DPDK fails to initialize huge pages due to excessive memory fragments. 
- Modify the startup parameters of the Linux system on a VM. Add the startup parameters `default_hugepagesz=2M`, `hugepagesz=2M`, and `hugepages=8192` to the grub file. The procedure is as follows: 
  a.	Edit the `/etc/default/grub` file.  
  Set `GRUB_CMDLINE_LINUX="………"` to `GRUB_CMDLINE_LINUX ="……… default_hugepagesz=2M hugepagesz=2M hugepages=8192"`.  
  b.	Update the grub file of the VM.  
  `grub2-mkconfig > /boot/grub2/grub.cfg`  
  c.	Restart the VM.  

**Q: "Cannot init data mbuf pool for port", "Current CONFIG_RTE_MAX_MEMSEG=256 is not enough", or "Aborted(core dumped)" was displayed when I ran the packet_process packet sending program. What Do I Do?** 

**Symptoms**  
-  When the packet length is long and a large number of queues exist, the default 8192 huge pages are insufficient. As a result, the contiguous memory is insufficient. For example, if the packet length is 1 MB and the number of queues is 8, this problem occurs when you run packet_process.  
-  When the packet length is 10 MB, 24576 huge pages are configured, and the contiguous memory is sufficient, this problem occurs when you run packet_process using a single queue. 

**Causes:**   
- In the first case, the applied number of huge pages is 8192, and the contiguous physical memory is 16 GB. If the number of queues is too large and the packet length is too long, the total memory required will be greater than 16 GB. As a result, the contiguous memory is insufficient, and the system fails to apply for the mbuf pool.  
- In the second case, fragments are generated in the DPDK contiguous memory when the system is running. As a result, the system fails to obtain a sufficient number of 10 MB contiguous memory during initialization. 

**Solutions:** 
-  For the first case:  
  a. When the total physical memory is sufficient, apply for a larger number of huge pages. For example, the maximum load (8 queues and 1 MB packet length) needs 24576 huge pages, that is, 48 GB contiguous physical memory. The configuration command is as follows: `sysctl -w vm.nr_hugepages=24576`  
  b. Configure the packet length and queue quantity based on the continuous memory size. Use a long packet length with small queue quantity, or a short packet length with large queue quantity. For example: When the packet length is 1 MB, the memory size required by a queue equals to the memory occupied by BD messages (1 GB is sufficient) plus one of the following values: 

    Packet receiving queue: 1 MB x 2048 bytes. If the FMMU (FPGA memory manage unit) function is used, the value is (1 MB + 32) x 2048 bytes.  
    Packet sending queue: 1 MB x 2048 bytes. If the FMMU function is used, the value is (1 MB + 32) x 2048 bytes.  
  The size of each queue is about 2 GB. By default, 8192 huge pages are configured when the packet length is 1 MB. That is, a 16 GB contiguous physical memory supports only three queues at the same time.  
-  For the second case: 
  Restart the VM, run the `sysctl -w vm.nr_hugepages=24576` command to configure 24576 huge pages, and send packets again. 

**Q: The license cache files generated by Vivado have security risks. What Do I Do?**

Vivado is an FPGA design tool provided by Xilinx. The tool checks the license and generates cache files in the `/tmp/FLEXnet/`, `/usr/local/share/macrovision/storage/`, and `/usr/local/share/macrovision/storage/FLEXnet` directories.
 


 The cache files have the global write permission. Therefore, the data can be modified and damaged by any user in the system, which may damage the system.

**Solutions:** These files are used only when Vivado checks the license, and are generated each time they are used. You are advised to manually write scripts to delete all files in the preceding three directories each time you use Vivado. Deleting these files does not affect Vivado.

**Q: How can I design high-performance and high-quality code?**

For details about the FPGA development of the high-performance architecture, see the UltraFast Design Methodology Guide for the Vivado Design Suite (https://www.xilinx.com/support/documentation/sw_manuals/xilinx2018_1/ug949-vivado-design-methodology.pdf).

For details about the FPGA development of the general-purpose architecture, see the SDAccel Environment Profiling and Optimization Guide (https://www.xilinx.com/support/documentation/sw_manuals/xilinx2017_4/ug1207-sdaccel-optimization-guide.pdf).

<a name="sec_7"></a>
## FpgaCmdEntry
**Q: What is FpgaCmdEntry?**

FpgaCmdEntry is a tool used to operate the FPGA card on the VM. The tool allows users to load, clear, and query information on the FPGA card.

**Q: "Command busy" was displayed when I ran the FpgaCmdEntry command. What Do I Do?**

**Causes:** 
The FPGA card is performing other tasks. 

**Solutions:** 
Wait for 30 seconds to 5 minutes and then run the FpgaCmdEntry command again.

**Q: "invalid AEI ID" was displayed when I ran the FpgaCmdEntry loading command. What Do I Do?**

**Causes:** 
The AEI ID entered by the user is different from the AEI ID in the OBS bucket. 

**Solutions:** 
Check whether the AEI ID is correct and try again. If the problem persists, contact FusionSphere O&M personnel for support.

**Q: "internal error, please try FpgaCmdEntry IF -S <slot num> for details" was displayed when I ran the FpgaCmdEntry loading command. What Do I Do?**

**Causes:** 
The AEI file header or AEI file imported by the user is incorrect. 

**Solutions:** 
Verify that the AEI loading file and registration process are normal according to the README file on GitHub, and that the registration tool is the latest version. Register the AEI ID again and run the loading command. If the problem persists, contact FusionSphere O&M personnel for support.

<a name="sec_8"></a>
## High-Risk Operations

**Q: When I pressed Ctrl+C during the execution of xbsk test in OCL SDK, xbsk test failed to be executed again. What do I do?**

This operation is prohibited. If you have performed this operation, reinstall the XDMA driver to rectify the fault.

**Q: When I pressed Ctrl+C during the example execution in OCL SDK, the example failed to be executed again. What do I do?**

This operation is prohibited. If you have performed this operation, use FpgaCmdEntry to manually load the example again to rectify the fault.


