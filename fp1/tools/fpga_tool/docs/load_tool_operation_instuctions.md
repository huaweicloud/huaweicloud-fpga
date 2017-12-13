FpgaCmdEntry Operation Instructions
===================================

FpgaCmdEntry is an FPGA image querying and loading tool. You need to specify parameters and variables to implement corresponding functions.

The format of the command is **FpgaCmdEntry Operation code-Variable**.

The operation codes include DF (querying VM FPGA devices), LF (loading an image to an FPGA card), IF (querying the FPGA card image status), IL (querying the virtual LED status), IV (querying tool version information).

Querying VM FPGA Devices
----------------------------
###Function

This command is used to query FPGA card information of a VM, including slot number, DBDF, vendor ID, and device ID.

###Format

**FpgaCmdEntry DF -Variable**

**FpgaCmdEntry DF -D**

**FpgaCmdEntry DF -?**

**FpgaCmdEntry DF -h**

###Parameters

| Parameter | Description                       |
|-----------|-----------------------------------|
|   -D      | Displays FPGA device information. |
|   -?      | Displays help information.        |
|   -h      | Displays help information.        |

Loading an Image to an FPGA Card
--------------------------------
###Function

This command is used to load an FPGA image to an FPGA card in a specified slot. You need FPGA slot numbers and FPGA image IDs.

###Format

**FpgaCmdEntry LF -Variable**

**FpgaCmdEntry LF -S FPGA card slot number -I FPGA image ID**

**FpgaCmdEntry LF -?**

**FpgaCmdEntry LF -h**

###Parameters

| Parameter | Description                                                                                                                                  |
|-----------|--------------------------------------------------------------------------------------------------|
| -S      | Specifies the slot number of an FPGA card. The value ranges from 0 to 7. You can run the **FpgaCmdEntry DF -D** command to obtain the number. |
| -I      | Specifies the FPGA image ID.                                                                                                                 |
| -?      | Displays help information.                                                                                                                   |
| -h      | Displays help information.                                                                                                                   |

Querying the Status of an FPGA Card Image
-----------------------------------------
###Function

This command is used to query the loading status of an FPGA image. You need to
specify slot information.

###Format

**FpgaCmdEntry IF -S FPGA card slot number**

**FpgaCmdEntry IF -?**

**FpgaCmdEntry IF -h**

###Parameters

| Parameter | Description                                                                                                                                  |
|-----------|----------------------------------------------------------------------------------------------------------------------------------------------|
|  -S       | Specifies the slot number of an FPGA card. The value ranges from 0 to 7. You can run the **FpgaCmdEntry DF -D** command to obtain the number. |
|  -?       | Displays help information.                                                                                                                   |
|  -h       | Displays help information.                                                                                                                   |

Querying the Virtual LED Status
-------------------------------
###Function

This command is used to query the LED status of an FPGA card. You need to specify slot information.

###Format

**FpgaCmdEntry IL -S FPGA card slot number**

**FpgaCmdEntry IL -?**

**FpgaCmdEntry IL -h**

###Parameters

| Parameter | Description                                                                                                                                  |
|-----------|----------------------------------------------------------------------------------------------------------------------------------------------|
|   -S      | Specifies the slot number of an FPGA card. The value ranges from 0 to 7. You can run the **FpgaCmdEntry DF -D** command to obtain the number. |
|   -?      | Displays help information.                                                                                                                   |
|   -h      | Displays help information.                                                                                                                   |

Querying the Tool Version
-------------------------
###Function

This command is used to query the tool version.

###Format

**FpgaCmdEntry IV**

###Description

None



\----End