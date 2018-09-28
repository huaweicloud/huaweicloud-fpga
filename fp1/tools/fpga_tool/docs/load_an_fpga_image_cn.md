加载FPGA镜像
=====================
[Switch to the English version](./load_an_fpga_image.md)


FPGA镜像加载工具FpgaCmdEntry具备FPGA信息查询、镜像加载、镜像加载状态查询、虚拟点灯查询和镜像清除功能。

注：对于不同类型的实例，FPGA设备数量不同，这里以只有一个FPGA设备为例。

加载过程
---------------------

**步骤 1**：使用shell命令`ls /usr/local/bin/FpgaCmdEntry`，确认存在FPGA镜像加载工具。

    [root@fpga_01]# ls /usr/local/bin/FpgaCmdEntry 
	/usr/local/bin/FpgaCmdEntry

> 如果工具不存在，请按照[fpga_tool README_CN](./../README_CN.md)安装和编译章节进行工具的编译和安装。

**步骤 2**：使用shell命令`FpgaCmdEntry DF -D`，确认虚拟机上有FPGA设备。DeviceId字段为0xd503代表当前实例中的设备为高性能型，DeviceId字段为0xd512代表当前实例中的设备为通用型。

	[root@fpga_01]# FpgaCmdEntry DF -D 
	----------------FPGA Information------------------
	     Type			           Fpga Device
	     Slot			           0
	     VendorId			       0x19e5
	     DeviceId			       0xd503
	     DBDF			           0000:00:06.0
	 --------------------------------------------------
	Command execution is complete. 
	
	[root@fpga_02]# FpgaCmdEntry DF -D 
	----------------FPGA Information------------------
	     Type			           Fpga Device
	     Slot			           0
	     VendorId			       0x19e5
	     DeviceId			       0xd512
	     DBDF			           0000:00:06.0
	 -------------------------------------------------- 
	Command execution is complete.


**步骤 3**：使用shell命令 **FpgaCmdEntry IF -S** *Slot* 查询相应槽位FPGA设备是否已载FPGA镜像，FPGA PR status字段为NOT_PROGRAMMED表示镜像未载。

**Slot**：表示FPGA槽位号，为查询出来的设备槽位号。

	[root@fpga_01]# FpgaCmdEntry IF -S 0 
	-------------Image Information--------------------
	     Type			           Fpga Device
	     Slot			           0
	     VendorId			       0x19e5
	     DeviceId			       0xd503
         DBDF                       0000:00:06.0
         AEI ID
		 Shell ID			       01010021
         FPGA PR status             NOT_PROGRAMMED
	     Load/ClearOpsStatus        INITIALIZED       
	 --------------------------------------------------
	Command execution is complete. 
	
	[root@fpga_02]# FpgaCmdEntry IF -S 0 
	-------------Image Information--------------------
	     Type			           Fpga Device
	     Slot			           0
	     VendorId			       0x19e5
	     DeviceId			       0xd512
         DBDF                       0000:00:06.0
         AEI ID
		 Shell ID			       01210002
         FPGA PR status             NOT_PROGRAMMED
	     Load/ClearOpsStatus        INITIALIZED
	 --------------------------------------------------
	Command execution is complete.


**步骤 4**：执行命令 **FpgaCmdEntry LF -S** *Slot* **-I** *AEI ID* 加载FPGA镜像到FPGA设备。

**Slot**：表示FPGA槽位号，为查询出来的设备槽位号。

**AEI ID**：AEI id 表示AEI编号，为用户编译生成的FPGA镜像ID。

	[root@fpga_02]# FpgaCmdEntry LF -S 0 -I ff8080825e9**********74851ee0023
	Command execution is complete.

**步骤 5**：执行shell命令 **FpgaCmdEntry IF -S** *Slot* 查询是否加载成功。如果Load/ClearOpsStatus状态为SUCCESS，表示上一次镜像加载命令执行成功。如果FPGA PR status状态为PROGRAMMED，且AEI ID与上一次镜像加载命令中输入AEI ID相同，则表示镜像加载成功。


	[root@fpga_01]# FpgaCmdEntry IF -S 0 
	 -------------Image Information-------------------- 
	     Type			           Fpga Device
	     Slot			           0
	     VendorId			       0x19e5
	     DeviceId			       0xd503
         DBDF                       0000:00:06.0
         AEI ID                     ff8080825e9**********177078f001e
		 Shell ID			       01010021
         FPGA PR status             PROGRAMMED
	     Load/ClearOpsStatus        SUCCESS
	 -------------------------------------------------- 
	Command execution is complete. 
	
	[root@fpga_02]# FpgaCmdEntry IF -S 0 
	 -------------Image Information-------------------- 
	     Type			           Fpga Device
	     Slot			           0
	     VendorId			       0x19e5
	     DeviceId			       0xd512
         DBDF                       0000:00:06.0
         AEI ID                     ff8080825e9**********74851ee0023
		 Shell ID			       01210002
         FPGA PR status             PROGRAMMED
	     Load/ClearOpsStatus        SUCCESS
	 -------------------------------------------------- 
	Command execution is complete.

**步骤 6 （非必须）**：执行shell命令 **FpgaCmdEntry IL -S** *Slot* 可以查询相应槽位的点灯状态，点灯状态值是用户加载PR时自行设置的。点灯状态查询可以用于确认加载的镜像功能是否正常，当查询结果与设置一致时，则说明加载的镜像功能正常。需要注意的是通用型设备不支持点灯状态查询。

	[root@fpga_01]# FpgaCmdEntry IL -S 0 
	LED Status(H): 0x0 
	LED Status(H): 0000|0000|0000|0000|0000|0000|0000|0000 
	Command execution is complete. 
	
	[root@fpga_02]# FpgaCmdEntry IL -S 0 
	General purpose architecture device doesn't support user LED.
	Command execution is complete.

> 关于如何设置VLED寄存器请联系华为技术支持。

清除过程
---------------------

**步骤 1**：执行命令 **FpgaCmdEntry CF -S** *Slot* 清除FPGA镜像。

**Slot**：表示FPGA槽位号，为查询出来的设备槽位号。

	[root@fpga_02]# FpgaCmdEntry CF -S 0
	Command execution is complete.

**步骤 2**：执行shell命令 **FpgaCmdEntry IF -S** *Slot* 查询是否清除成功。如果Load/ClearOpsStatus状态为SUCCESS，表示上一次镜像清除命令执行成功。如果FPGA PR status状态为NOT_PROGRAMMED，且AEI ID为空，则表示镜像清除成功。


	[root@fpga_01]# FpgaCmdEntry IF -S 0 
	 -------------Image Information-------------------- 
	     Type			           Fpga Device
	     Slot			           0
	     VendorId			       0x19e5
	     DeviceId			       0xd503
         DBDF                       0000:00:06.0
         AEI ID                    
		 Shell ID			       01010021
         FPGA PR status             NOT_PROGRAMMED
	     Load/ClearOpsStatus        SUCCESS
	 -------------------------------------------------- 
	Command execution is complete. 
	
	[root@fpga_02]# FpgaCmdEntry IF -S 0 
	 -------------Image Information-------------------- 
	     Type			           Fpga Device
	     Slot			           0
	     VendorId			       0x19e5
	     DeviceId			       0xd512
         DBDF                       0000:00:06.0
         AEI ID                    
		 Shell ID			       01210002
         FPGA PR status             NOT_PROGRAMMED
	     Load/ClearOpsStatus        SUCCESS
	 -------------------------------------------------- 
	Command execution is complete.

\----End
