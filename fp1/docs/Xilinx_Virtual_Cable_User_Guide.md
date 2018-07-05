# Xilinx Virtual Cable User Guide  

[切换到中文版](./Xilinx_Virtual_Cable_User_Guide_cn.md)

## Preparations  

 *  Go to the path of the SDx tool and search for the xvc_pcie.zip file.  
    `cd /software/Xilinx/SDx_2017.4_op/SDK/2017.4.op/data/xicom/drivers/pcie`  
    If you cannot access the above path, run the following command:  
    `find / -name "*xvc_pcie*"`  
 *  Copy the xvc_pcie.zip installation package to any directory and decompress it.    
    `unzip xvc_pcie.zip`   
 *  Go to the /dev directory and obtain the Xilinx virtual cable (XVC) device information, such as xvc0 and xvc1.  
    `ls /dev`   
 *  Deploy xvcserver. Replace /dev/xil_xvc/cfg_ioc0_tree1 in README.txt with the XVC information (for example, /dev/xvc0) obtained in the preceding step. For details about how to deploy xvcserver, see xvcserver/README.txt extracted from the package.  
 *  You do not need to install the decompressed driver. The driver has been installed when you run an FPGA instance and set environment variables.
##  Using XVC  
  * For details, see section "Connecting the Vivado Design Suite to the XVC-Server Application" in:
         <https://www.xilinx.com/support/documentation/ip_documentation/pcie4_uscale_plus/v1_3/pg213-pcie4-ultrascale-plus.pdf> 

##  Reference  
  * For more information about the virtual JTAG, see section "Remote Debugging in Vivado" in: 
         <https://www.xilinx.com/support/documentation/sw_manuals/xilinx2018_2/ug908-vivado-programming-debugging.pdf>  

  * For more information about XVC, visit: 
         <https://github.com/Xilinx/XilinxVirtualCable>

