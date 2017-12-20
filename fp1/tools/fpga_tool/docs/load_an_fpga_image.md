加载FPGA镜像
=====================

FPGA镜像加载工具FpgaCmdEntry具备FPGA信息查询、镜像加载、镜像加载状态查询和虚拟点灯查询功能。


加载过程
---------------------

**步骤 1**：使用shell命令`ls /usr/local/bin/FpgaCmdEntry`，确认存在FPGA镜像加载工具。

    [root@fpga_01]# ls /usr/local/bin/FpgaCmdEntry 
	/usr/local/bin/FpgaCmdEntry

> 如果工具不存在，请按照[fpga_tool README](./../README.md)安装和编译章节进行工具的编译和安装。

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


**步骤 3**：使用shell命令 **FpgaCmdEntry IF -S** *Slot* 查询相应槽位FPGA设备是否已载FPGA镜像，LoadStatusName字段为NOT_PROGRAMMED表示镜像未载。

**Slot**：表示FPGA槽位号，为查询出来的设备槽位号。

	[root@fpga_01]# FpgaCmdEntry IF -S 0 
	-------------Image Information--------------------
	     Type			           Fpga Device
	     Slot			           0
	     VendorId			       0x19e5
	     DeviceId			       0xd503
	     DBDF			           0000:00:06.0
	     ImageId			
	     LoadStatusName		     NOT_PROGRAMMED
	     LoadStatusCode		     0
	     LoadErrName		        OK
	     LoadErrCode		        0
	     Shell ID			       01010021
	 --------------------------------------------------
	Command execution is complete. 
	
	[root@fpga_02]# FpgaCmdEntry IF -S 0 
	-------------Image Information--------------------
	     Type			           Fpga Device
	     Slot			           0
	     VendorId			       0x19e5
	     DeviceId			       0xd512
	     DBDF			           0000:00:06.0
	     ImageId			
	     LoadStatusName		     NOT_PROGRAMMED
	     LoadStatusCode		     0
	     LoadErrName		        OK
	     LoadErrCode		        0
	     Shell ID			       01210002
	 --------------------------------------------------
	Command execution is complete.


**步骤 4**：执行命令 **FpgaCmdEntry LF -S** *Slot* **-I** *ImageId* 加载FPGA镜像到FPGA设备。

**Slot**：表示FPGA槽位号，为查询出来的设备槽位号。

**ImageId**：AEI id 表示AEI编号，为用户编译生成的FPGA镜像ID。

	[root@fpga_02]# FpgaCmdEntry LF -S 0 -I ff8080825e9**********74851ee0023
	Command execution is complete.

**步骤 5**：执行shell命令 **FpgaCmdEntry IF -S** *Slot* 查询是否加载成功。如果LoadStutusName状态为LOADED，表示镜像加载成功。


	[root@fpga_01]# FpgaCmdEntry IF -S 0 
	 -------------Image Information-------------------- 
	     Type                       Fpga Device 
	     Slot                       0 
	     VendorId                   0x19e5 
	     DeviceId                   0xd503 
	     DBDF                       0000:00:06.0 
	     ImageId                    ff8080825e9**********177078f001e 
	     LoadStatusName             LOADED 
	     LoadStatusCode             1 
	     LoadErrName                OK 
	     LoadErrCode                0 
	     Shell ID			       01010021
	 -------------------------------------------------- 
	Command execution is complete. 
	
	[root@fpga_02]# FpgaCmdEntry IF -S 0 
	 -------------Image Information-------------------- 
	     Type                       Fpga Device 
	     Slot                       0 
	     VendorId                   0x19e5 
	     DeviceId                   0xd512 
	     DBDF                       0000:00:06.0 
	     ImageId                    ff8080825e9**********74851ee0023 
	     LoadStatusName             LOADED 
	     LoadStatusCode             1 
	     LoadErrName                OK 
	     LoadErrCode                0 
	     Shell ID			       01210002
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

\----End
