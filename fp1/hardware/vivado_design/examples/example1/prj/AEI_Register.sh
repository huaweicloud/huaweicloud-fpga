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
#  judge manifest
#####################################################################################################################
if [ ! -f "$script_path/build/checkpoints/to_facs/manifest.json" ];then
    echo "ERROR:The $script_path/build/checkpoints/to_facs/manifest.json is not found,Please check whether the build is successful"
    exit -1
fi

dcp_dir=`echo $script_path/build/checkpoints/to_facs`
dcpfile=`find $dcp_dir  -maxdepth 1 -name "*.dcp"`

if [ "x$dcpfile" == "x" ];then
    echo "ERROR:The $script_path/build/checkpoints/to_facs dcp file is not found,Please check whether the build is successful"
    exit -1
fi

######################################################################################################################
#tar dcp and json
######################################################################################################################
cd $script_path
cp $dcp_dir/*.dcp ./
cp $script_path/build/checkpoints/to_facs/manifest.json ./
tar cf dcp.tar ./*.dcp ./manifest.json
rm -fr ./*.dcp
rm -fr ./manifest.json

######################################################################################################################
#Upload
######################################################################################################################
echo "#############################################################"
echo "Register AEI"
echo "#############################################################"

id_info=`fis fpga-image-create --dcp-file "$script_path/dcp.tar" --dcp-obs-path "$obs_path" --log-obs-directory "$obs_dir" --name "$AEI_Name" --description "$AEI_Description"`
python -c "print '''$id_info'''"
#rm -rf $script_path/build/checkpoints/to_facs/${fileName%.*}*
