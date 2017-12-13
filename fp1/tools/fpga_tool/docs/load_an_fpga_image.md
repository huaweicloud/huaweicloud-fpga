Loading an FPGA Image
=====================

FpgaCmdEntry, an FPGA image loading tool in the SDK, supports VM FPGA information query, image loading and loading status query, and virtual LED status query.


Procedure
---------------------

**Step 1**:  Run the `ls /usr/local/bin/FpgaCmdEntry` command to check that the FPGA image loading tool exists.

    [root@fpga_01]# ls /usr/local/bin/FpgaCmdEntry 
	/usr/local/bin/FpgaCmdEntry


**Step 2**: Run the `FpgaCmdEntry DF -D` command to check that there are FPGA devices on the VM. If the value of DeviceId is 0xd503, the device in the current example is a high-perform device. If the value of DeviceId is 0xd512, the device in the current example is a general-purpose device.

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


**Step 3**: Run the **FpgaCmdEntry IF -S** *slot_fpga* command to check whether an FPGA image is loaded to the FPGA device in each slot. If the value of **LoadStatusName** is **NOT_PROGRAMMED**, no image is loaded.

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


**Step 4**: Run the **FpgaCmdEntry LF -S** *slot_fpga* **-I** *fi_id* command to load an image to an FPGA device.

**slot_fpga** indicates the slot number of an FPGA device.

**fi_id** indicates an FPGA image ID.

	[root@fpga_02]# FpgaCmdEntry LF -S 0 -I ff8080825e9**********74851ee0023
	Command execution is complete.

**Step 5**: Run the **FpgaCmdEntry IF -S** *slot_fpga* command to check whether the image is loaded successfully.

If the value of **LoadStutusName** is **LOADED**, the image is loaded successfully.

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


**Step 6 (not necessary)**: Query the virtual LED status to verify that the image loaded is functional. If you have completed VLED data register settings, run the **FpgaCmdEntry IL -S** *slot_fpga* command to query the LED status of a slot. If the queried value is the same as the set value, the loaded image is functional. A general-purpose architecture device does not support the query of the LED status.

	[root@fpga_01]# FpgaCmdEntry IL -S 0 
	LED Status(H): 0x0 
	LED Status(H): 0000|0000|0000|0000|0000|0000|0000|0000 
	Command execution is complete. 
	
	[root@fpga_02]# FpgaCmdEntry IL -S 0 
	General purpose architecture device doesn't support user LED.
	Command execution is complete.

**Note**: How to set the VLED data register please refer the development guide.

\----End
