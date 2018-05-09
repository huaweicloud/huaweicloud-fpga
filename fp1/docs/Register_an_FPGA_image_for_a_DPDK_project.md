Registration
----

[切换到中文版](./Register_an_FPGA_image_for_a_DPDK_project_cn.md)

Use AEI_Regsiter.sh to register an FPGA image with the image management module. After the registration, an ID is assigned to the FPGA image. The ID can be used to query the registration status, and load, delete, and associate the image.

### Preparations

Make the following preparations before the registration:

#### Switch to the project directory where the scripts are stored.

switch to the **prj** directory.

For example, for an example project, switch to the `huaweicloud-fpga/fp1/hardware/vivado_design/examples/example1/prj` directory.


#### Build a project. (If a project has been built, skip this step.)

Run the `sh build.sh` command.

\----End

#### Running the Registration Script

The format of the AEI_Register.sh script is as follows:

Usage:sh AEI_Register.sh *-n* [AEI_name] *-d* [AEI_Description]

-   *-n* specifies the AEI name (**AEI_name**) of the FPGA image to be registered. An AEI_name is a string of 1 to 64 characters consisting of letters, digits, underscores (_), and hyphens (-).

-   *-d* specifies the AEI description (**AEI_Description**) of the FPGA image to be registered.  AEI_Description consists of 0 to 255 characters, including uppercase and lowercase letters, digits, hyphens (-), underscores (_), periods (.), commas (,), and spaces.

The **AEI_Register.sh** script is executed successfully if the following output is displayed:

\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#  
Register AEI  
\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#  
Uploading FPGA image to OBS
Upload 46696040 bytes using 2.00751 seconds
Registering FPGA image to FIS
Success: 200 OK  
id: 0000\*\*\*\*\*\*\*\*5568015e3c87835c0326  
status: saving

-   "Success: 200 OK" indicates that the **AEI_Register.sh** script is executed successfully. The execution of the **AEI_Register.sh** script does not necessarily mean that the FPGA image is successfully registered. Users need to run query subcommands of fisclient and use the FPGA image ID in the registration command output to check the FPGA image information. If the state of the FPGA image is active, the image is successfully registered. An FPGA image can be loaded only after it is successfully registered.

-   "id: 00005568015e3c87835c0326" indicates that the FPGA image management module assigns the FPGA image to be registered the image ID 00005568015e3c87835c0326, which can be used to query the FPGA image registration and loading status.

-   "status: saving" indicates that the FPGA image is being saved.


**Note:** The **AEI_Register.sh** script will transfer the FPGA logic files generated during the registration to the OBS bucket for image registration. After confirming that the registration is successful, you can manually delete the logical files from the OBS bucket to prevent unnecessary OBS charging.

For example, to register an OCL image, run the following command:

[root\@ scripts]\# sh AEI_Register.sh -n "DPDK-test" -d "DPDK-desc"  
fis argument(s) and config file are OK
INFO: OCL Running
#############################################################
Register AEI
#############################################################
Uploading FPGA image to OBS
Upload 46696040 bytes using 2.00751 seconds
Registering FPGA image to FIS
Success: 200 OK
id: ff80808262f26be40162f70699287c18
status: saving

