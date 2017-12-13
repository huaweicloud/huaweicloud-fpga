#!/bin/sh
#
#-------------------------------------------------------------------------------
#      Copyright 2017 Huawei Technologies Co., Ltd. All Rights Reserved.
# 
#      This program is free software; you can redistribute it and/or modify
#      it under the terms of the Huawei Software License (the "License").
#      A copy of the License is located in the "LICENSE" file accompanying 
#      this file.
# 
#      This program is distributed in the hope that it will be useful,
#      but WITHOUT ANY WARRANTY; without even the implied warranty of
#      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#      Huawei Software License for more details. 
#-------------------------------------------------------------------------------

#######################################################################################################################
## get the script path
#######################################################################################################################
if [[ "$0" =~ ^\/.* ]]; then
	script=$0
else
	script=$(pwd)/$0
fi
script=$(readlink -f $script)
script_path=${script%/*}

#######################################################################################################################
## Function usage
#######################################################################################################################
template_path=$(readlink -f $script_path/../template/)
filename=usr_prj0
function usage
{
    echo "---------------------------------------------------------------------"
    echo "Usage: create_prj.sh [option]                                   "
    echo "Options:                                                        "
    echo "         -h |-H |-help |--help            Only for help               "
    echo "         [filename]                       Create [filename] directory "
    echo "---------------------------------------------------------------------"
    echo "Example: when you run this command 'sh create_prj.sh usr_prj0' ,"
    echo "         the directory will be build in '/fpga_design/hardware/vivado_design/usr/usr_prj0'" 
    echo "Note   : The [filename] must start with letters, digits, and underscores."
    echo " "
}
while [ "$1" != "" ]; do
    case $1 in
        -h | -H | -help | --help )       usage
                                         exit
                                         ;;
        [_a-zA-Z0-9]* )                  filename=$1
					 ;;
	* )
		echo "ERROR: '$1' invalid character. "
		echo "        The file name must start with letters, digits, and underscores."
		echo "Please input the '-h'，'-H'，'-help' or '--help' character to get help of create_prj.sh"
		echo
		exit
		;;		
    esac
    shift
done


#######################################################################################################################
## make files by uesr define
#######################################################################################################################


new_prj_path=$(readlink -f $script_path/$filename)
if [ -d "$new_prj_path" ]; then
	echo "ERROR:File exists"
	exit
else
	mkdir $new_prj_path
fi


#######################################################################################################################
## copy template to new project
#######################################################################################################################
cp -rf $script_path/../lib/template/*  $new_prj_path
# Modified the project name
sed -i "s/usr_template/$filename/g" $new_prj_path/sim/Makefile
sed -i "s/usr_template/$filename/g" $new_prj_path/sim/readme.md
sed -i "s/usr_template/$filename/g" $new_prj_path/prj/readme.md
sed -i "s/usr_template/$filename/g" $new_prj_path/readme.md
echo "INFO:It's successful to create the directory of $filename. "
