#!/bin/bash
#
#-------------------------------------------------------------------------------
#      Copyright 2018 Huawei Technologies Co., Ltd. All Rights Reserved.
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
if [[ "$0" =~ ^\/.* ]]; then
    script=$0
else
    script=$(pwd)/$0
fi
script=$(readlink -f $script)
script_path=${script%/*}

function usage
{
    echo "Usage:sh AEI_Register.sh -p [obs_path] -o [obs_dir] -n [AEI_name] -d [AEI_Description]"
}
#####################################################################################################################
# get parameter 
#####################################################################################################################
obs_path=""
obs_dir=""
AEI_Name=""
AEI_Description=""
while [ "$1" != "" ];do
    case $1 in
        -h | -H | -help | --help )
            usage
            exit
            ;;
        -p )
            shift
            if [[ "$1" =~ ^-.* ]];then
                echo "Invalid arguments:$1"
                exit
            fi
            obs_path=$1
            ;;
        -o )
            shift
            if [[ "$1" =~ ^-.* ]];then
                echo "Invalid arguments:$1"
                exit
            fi
            obs_dir=$1
            ;;
        -n )
            shift
            if [[ "$1" =~ ^-.* ]];then
                echo "Invalid arguments:$1"
                exit
            fi
            AEI_Name=$1
            ;;
        -d )
            shift
            if [[ "$1" =~ ^-.* ]];then
                echo "Invalid arguments:$1"
                exit
            fi
            AEI_Description=$1
            ;;
        *)
            echo "Unknown Argument: $1"
            usage
            exit -1
    esac
    shift
done
if [ "x$AEI_Name" == "x" ];then
    usage
    exit
fi

#####################################################################################################################
#  judge fisclient
#####################################################################################################################

fis_info=`which fis 2> /dev/null`
if [ "x$fis_info" == "x" ];then
    echo -e "ERROR:fis tool is not detected, please refer to huaweicloud-fpga/README.md 2.1.2, go to intall fisclient."
    exit
fi
fischeck_info=`which fischeck 2> /dev/null`
if [ "x$fischeck_info" == "x" ];then
    echo -e "ERROR:fischeck tool is not detected, please refer to huaweicloud-fpga/README.md 2.1.2, go to intall fisclient."
    exit
fi

#####################################################################################################################
if [ "$(echo "$script_path"|grep "hardware/sdaccel_design/")" != "" ];then
    mode="OCL"
else
    echo "Unknown Mode:$mode"
    exit -1
fi
#####################################################################################################################
#Build
#####################################################################################################################
if [ "$mode" == "OCL" ];then
    echo "INFO: OCL Running"
    xclbin_Name=`echo $script_path/../prj/bin/*.xclbin`
    if [ ! -f $xclbin_Name ];then
         echo "ERROR:ocl xclbin file is not found in this prj"
         exit -1
    fi
    #create manifest.json

    cd $script_path/../prj/bin
    /software/Xilinx/SDx_2017.4_op/SDx/2017.4.op/runtime/bin/xclbinsplit $xclbin_Name
    mv split-primary.bit split-primary.dcp
    cd $script_path

    dcp_Name=`echo $script_path/../prj/bin/split-primary.dcp`
    fileName=${dcp_Name##*/}
    if [ ! -f $dcp_Name ];then
         echo "ERROR:$script_path/../prj/bin/ ocl dcp file is not found"
         exit -1
    fi


    #generate manifest.json
    python $script_path/../../../lib/scripts/create_sdaccel_metadata.py $xclbin_Name $dcp_Name
    if [ $? -ne 0 ];then
        exit -1
    fi

    if [ -f "$script_path/../prj/bin/manifest.json" ];then
        cd $script_path
        cp $script_path/../prj/bin/*.dcp ./
        cp $script_path/../prj/bin/manifest.json ./
        #tar cf dcp.tar $dcp_Name $script_path/../prj/bin/manifest.json
        tar cf dcp.tar ./*.dcp ./manifest.json
        if [ $? -ne 0 ];then
            exit -1
        fi
        rm -fr ./*.dcp
        rm -fr ./manifest.json
    else
        echo "ERROR:The $script_path/../prj/bin/manifest.json is not found,Please check whether the build is successful"
        exit -1
    fi
fi
######################################################################################################################
#Upload
######################################################################################################################
echo "#############################################################"
echo "Register AEI"
echo "#############################################################"
if [ "$mode" == "OCL" ];then
    id_info=`fis fpga-image-create --dcp-file "$script_path/dcp.tar" --dcp-obs-path "$obs_path" --log-obs-directory "$obs_dir" --name "$AEI_Name" --description "$AEI_Description"`
    #rm $script_path/../prj/bin/*.bin
fi

python -c "print '''$id_info'''"
if [ -n "`echo $id_info|grep "Success:"`" -a $mode == "OCL" ];then
    shell_id=`echo $id_info|sed 's/^.*id:.*\([a-f0-9]\{32\}\).*/\1/g'`
	xclbin_name=`echo $script_path/../prj/bin/*.xclbin`
    chmod +x $script_path/../../../lib/scripts/xclbinaddaei.py
    python $script_path/../../../lib/scripts/xclbinaddaei.py $xclbin_name $shell_id >/dev/null 2>&1
fi
