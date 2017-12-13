# 目录结构说明
用于存放用户使用CoreGen生成的Xilinx IP Core,其结构如下：
* [lib/ip](#lib/ip_dir)/ 
  - debug_bridge_0 				
  - ila_0  						
  - rdimma_x8_16GB_2133Mbps  		
  - rdimmb_x8_16GB_2133Mbps	
  - rdimmc_x8_16GB_2133Mbps		
  - README.md

# 文件说明
* debug_bridge_0  
  是一个供用户使用Xilinx IP Core，主要是调试时使用，该文件在执行`setup.sh`设置环境变量后可见；  

* ila_0    
  是一个供用户使用Xilinx IP Core，主要是调试时使用，用于ILA的调试信，该文件在执行`setup.sh`设置环境变量后可见； 

* rdimma_x8_16GB_2133Mbps  
  该文件夹中包含Xilinx提供的DDR4控制器的物理层IP（针对ddra），它的功能是配合用户自己的控制器或本工程提供的控制器完成对DDR颗粒的读写访问。该文件夹在用户执行`setup.sh`设置环境变量后自动生成；  

* rdimmb_x8_16GB_2133Mbps  
  该文件夹中包含Xilinx提供的DDR4控制器的物理层IP（针对ddrb），它的功能是配合用户自己的控制器或本工程提供的控制器完成对DDR颗粒的读写访问。该文件夹在用户执行`setup.sh`设置环境变量后自动生成；

* rdimmc_x8_16GB_2133Mbps  
   该文件夹中包含Xilinx提供的DDR4控制器的物理层IP（针对ddrd），它的功能是配合用户自己的控制器或本工程提供的控制器完成对DDR颗粒的读写访问。该文件夹在用户执行`setup.sh`设置环境变量后自动生成；

* README.md   
  即本文档，用于介绍其他文档。 

# 注意  			
该文件夹中的Xilinx IP Core属于`公用库文件`，..\..\vivado_design路径下的工程`均可直接调用`；  
Xilinx IP Core按文件名称区分，用户在生成和调用IP Core时`应注意文件名称`，确保IP Core可读易用。