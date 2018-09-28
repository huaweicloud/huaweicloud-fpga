
# Directory Structure

[切换到中文版](./README_CN.md)

* [sdaccel_design/user](#sdaccel_design/user)/  
  - create_prj.sh
  - README.md 

# Contents Description  
* create_prj.sh  
  - This is the execution code for creating a project. `create_prj.sh` is an important part for command execution when users create a project.
  - To copy the `project directory templates` to `/fpga_design/hardware/sdaccel_design/user/` with one click, run the following command:

    ````bash
    # usr_prjxx indicates the name of the user project.
    # mode indicates the kernel type. Select either temp_cl, temp_c, or temp_rtl.
    $ sh create_prj.sh usr_prjxx temp_cl   
    ````

  - Run the following command:  

    ````bash
    $ sh create_prj.sh -h
    ````

    By running this command, you can obtain help information about **create_prj.sh**.


# Creating a User Project
  There are two methods for creating a user project:
  * Copy an example project. Modify parameters and add functional modules as required.
  * Create a project and add user-developed code to the compilation environment as required.
      The first method allows users to quickly implement a project, and the second requires users to compile code.

