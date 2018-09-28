FpgaCmdEntry Operation Instructions
===================================
[切换到中文版](load_tool_operation_instuctions_cn.md)


FpgaCmdEntry is a linux system command-line tool. Users need to specify parameters and variables to implement corresponding functions.

The format of the command is **FpgaCmdEntry Operation code -Parameter**.

The operation codes include DF (querying VM FPGA devices), LF (loading an image to an FPGA card), IF (querying the FPGA card image status), CF (clearing the FPGA card image), IL (querying the virtual LED status), and IV (querying tool version information).

Querying VM FPGA Devices
----------------------------
###Function

This command is used to query FPGA card information of a VM, including slot number, DBDF, vendor ID, and device ID.

###Format

**FpgaCmdEntry DF -Parameter**

**FpgaCmdEntry DF -D**

**FpgaCmdEntry DF -?**

**FpgaCmdEntry DF -h**

###Parameters

| Parameter | Description                       |
| --------- | --------------------------------- |
| -D        | Displays FPGA device information. |
| -?        | Displays help information.        |
| -h        | Displays help information.        |

Loading an Image to an FPGA Card
--------------------------------
###Function

This command is used to load an FPGA image to an FPGA card in a specified slot. You need FPGA slot numbers and FPGA image IDs.

###Format

**FpgaCmdEntry LF -Parameter**

**FpgaCmdEntry LF -S** *Slot* **-I** *ImageId*

**FpgaCmdEntry LF -?**

**FpgaCmdEntry LF -h**

###Parameters

| Parameter | Description                              |
| --------- | ---------------------------------------- |
| -S        | Specifies the slot number of an FPGA card. The value ranges from 0 to 7. You can run the **FpgaCmdEntry DF -D** command to obtain the number. |
| -I        | Specifies the FPGA image ID.             |
| -?        | Displays help information.               |
| -h        | Displays help information.               |

Querying the Status of an FPGA Card Image
-----------------------------------------
###Function

This command is used to query the loading status of an FPGA image. You need to specify slot information.

###Format

**FpgaCmdEntry IF -S** *Slot*

**FpgaCmdEntry IF -?**

**FpgaCmdEntry IF -h**

###Parameters

| Parameter | Description                              |
| --------- | ---------------------------------------- |
| -S        | Specifies the slot number of an FPGA card. The value ranges from 0 to 7. You can run the **FpgaCmdEntry DF -D** command to obtain the number. |
| -?        | Displays help information.               |
| -h        | Displays help information.               |

Clearing the FPGA Card Image
--------------------------------
###Function

This command is used to clear the FPGA card image in a specified slot. You need to specify slot information.

###Format

**FpgaCmdEntry CF -Parameter**

**FpgaCmdEntry CF -S** *Slot*

**FpgaCmdEntry CF -?**

**FpgaCmdEntry CF -h**

###Parameters

| Parameter | Description                              |
| --------- | ---------------------------------------- |
| -S        | Specifies the slot number of an FPGA card. The value ranges from 0 to 7. You can run the **FpgaCmdEntry DF -D** command to obtain the number. |
| -?        | Displays help information.               |
| -h        | Displays help information.               |

Querying the Virtual LED Status
-------------------------------
###Function

This command is used to query the LED status of an FPGA card. You need to specify slot information.

###Format

**FpgaCmdEntry IL -S** *Slot*

**FpgaCmdEntry IL -?**

**FpgaCmdEntry IL -h**

###Parameters

| Parameter | Description                              |
| --------- | ---------------------------------------- |
| -S        | Specifies the slot number of an FPGA card. The value ranges from 0 to 7. You can run the **FpgaCmdEntry DF -D** command to obtain the number. |
| -?        | Displays help information.               |
| -h        | Displays help information.               |

Querying the Tool Version
-------------------------
###Function

This command is used to query the tool version.

###Format

**FpgaCmdEntry IV**

###Description

None



\----End