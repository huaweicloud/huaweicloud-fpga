# Xilinx Virtual Cable (XVC)功能使用指导 

[Switch to the English version](./Xilinx_Virtual_Cable_User_Guide.md)

## 使用前准备  

 *  进入SDx工具路径,查找xvc_pcie.zip文件,方法如下：  
    `cd /software/Xilinx/SDx_2017.4_op/SDK/2017.4.op/data/xicom/drivers/pcie`     
       如果无法进入上述路径，可以如下命令查找获得：  
    `find / -name "*xvc_pcie*"`   
 *  解压xvc_pcie.zip安装包,用户可以拷贝xvc_pcie.zip至任意目录解压。    
    `unzip xvc_pcie.zip`   
 *  获取xvc设备信息,查看/dev目录下的xvc设备信息，例如xvc0、xvc1。  
    `ls /dev`    
 *  部署xvcserver，具体方法参考解压后的文件xvcserver/README.txt，使用上述方法获取的xvc设备信息，例如/dev/xvc0替换掉README.txt中/dev/xil_xvc/cfg_ioc0_tree1信息。  
 *  部署驱动信息，用户不需要安装解压后的驱动，在执行fpga实例设置环境变量的时候驱动已经安装完成。
## 使用XVC  
  * 使用方法参见，如下链接文档的Connecting the Vivado Design Suite to the XVC-Server Application章节：  
          <https://www.xilinx.com/support/documentation/ip_documentation/pcie4_uscale_plus/v1_3/pg213-pcie4-ultrascale-plus.pdf> 

##  使用参考  
  * 更多关于虚拟JTAG的介绍,参见如下链接的Remote Debugging in Vivado部分：  
          <https://www.xilinx.com/support/documentation/sw_manuals/xilinx2018_2/ug908-vivado-programming-debugging.pdf>  

  * XVC的github路径如下：  
          <https://github.com/Xilinx/XilinxVirtualCable>

