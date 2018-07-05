#!/bin/bash
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

if [[ "$0" =~ ^\/.* ]]; then
    script=$0
else
    script=$(pwd)/$0
fi
script=$(readlink -f $script)
script_path=${script%/*}

function usage
{
    echo "Usage:sh AEI_Register.sh -n [AEI_name] -d [AEI_Description]"
}

#####################################################################################################################
# get parameter 
#####################################################################################################################
AEI_Name=""
AEI_Description=""
while [ "$1" != "" ];do
    case $1 in
        -h | -H | -help | --help )
            usage
            exit
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


fis_info=`which fis 2> /dev/null`
if [ "x$fis_info" == "x" ];then
    echo -e "ERROR:fis tool is not detected, please refer to the FACS User Guide, go to intall fisclient."
    exit
fi
fischeck_info=`which fischeck 2> /dev/null`
if [ "x$fischeck_info" == "x" ];then
    echo -e "ERROR:fischeck tool is not detected, please refer to the FACS User Guide, go to intall fisclient."
    exit
fi

#####################################################################################################################
#####################################################################################################################
if [ "$(echo "$script_path"|grep "hardware/sdaccel_design/")" != "" ];then
    mode="OCL"
elif [ "$(echo "$script_path"|grep "hardware/vivado_design/")" != "" ];then
    mode="DPDK"
fi

#####################################################################################################################
#get the metadata from the manifest.txt
#####################################################################################################################
if [ "$mode" == "DPDK" ];then
    if [ -f "$script_path/build/reports/manifest.txt" ];then
        source $script_path/build/reports/manifest.txt
    else
        echo "ERROR:The $script_path/build/reports/manifest.txt is not found,Please check whether the build is successful"
    fi
    dcp_Name=${dcp_file_name}
    metadata="{\
            \"manifest_format_version\":\"$manifest_format_version\",\
            \"pci_vendor_id\":\"$pci_vendor_id\",\
            \"pci_device_id\":\"$pci_device_id\",\
            \"pci_subsystem_id\":\"$pci_subsystem_id\",\
            \"pci_subsystem_vendor_id\":\"$pci_subsystem_vendor_id\",\
            \"dcp_hash\":\"$dcp_hash\",\
            \"shell_type\":\"$shell_type\",\
            \"shell_version\":\"$shell_version\",\
            \"dcp_file_name\":\"${dcp_Name##*/}\",\
            \"hdk_version\":\"$hdk_version\",\
            \"date\":\"$date\"\
            }"
    Date=`date +%Y%m%d%H%M%S`
    fileName="pr_ul_$Date.bin"

elif [ "$mode" == "OCL" ];then
    dcp_Name=`echo $script_path/../prj/bin/*.xclbin`
    if [ -f "$script_path/../prj/bin/manifest.txt" ];then
        source $script_path/../prj/bin/manifest.txt
    else
        echo "ERROR:The $script_path/../prj/bin/manifest.txt is not found,Please check whether the build is successful"
    fi
    source $script_path/../prj/bin/manifest.txt
    metadata="{\
            \"manifest_format_version\":\"$manifest_format_version\",\
            \"pci_vendor_id\":\"$pci_vendor_id\",\
            \"pci_device_id\":\"$pci_device_id\",\
            \"pci_subsystem_id\":\"$pci_subsystem_id\",\
            \"pci_subsystem_vendor_id\":\"$pci_subsystem_vendor_id\",\
            \"shell_type\":\"$shell_type\",\
            \"shell_version\":\"$shell_version\",\
            \"hdk_version\":\"$hdk_version\",\
            \"date\":\"$date\"\
            }"
    fileName=${dcp_Name##*/}
else
    echo "Unknown Mode:$mode"
    exit -1
fi

#####################################################################################################################
#verifying the fis register arguments
#####################################################################################################################
fischeck --file-name "$fileName" --name "$AEI_Name" --metadata "$metadata" --description "$AEI_Description"
if [ $? != 0 ];then
        echo -e "\nverifying the fis register arguments failed"
        exit -1
fi

#####################################################################################################################
#Build
#####################################################################################################################
if [ "$mode" == "DPDK" ];then
    echo "INFO: DPDK Running"
    vivado -mode batch -nojournal -notrace -nolog -source $script_path/../../../lib/scripts/pr_verify_bitgen.tcl -tclargs $dcp_Name $Date
    #create metadata.json
    if [ ! -f $script_path/build/checkpoints/to_facs/$fileName ];then
         echo "ERROR:the vivado bin is not found"
         exit -1
    fi
    #create metadata.json
    python $script_path/../../../lib/scripts/create_dpdk_metadata.py "$script_path/build/checkpoints/to_facs/$fileName"
    if [ $? -ne 0 ];then
         exit -1
    fi
    
    #packager aei.bin
    python $script_path/../../../lib/scripts/AEI_packager.py "$script_path/build/reports/metadata.json" "$script_path/build/checkpoints/to_facs/$fileName"
    if [ $? -ne 0 ];then
         exit -1
    fi
    
    fileName=${fileName%.*}_aei.bin
    metadata=`cat $script_path/build/reports/metadata.json`
    
elif [ "$mode" == "OCL" ];then
    echo "INFO: OCL Running"
    if [ ! -f $dcp_Name ];then
         echo "ERROR:xclbin file is not found in this prj"
         exit -1     
    fi
    #create metadata.json
    python $script_path/../../../lib/scripts/create_sdaccel_metadata.py $dcp_Name
    if [ $? -ne 0 ];then
         exit -1
    fi
    
    #packager aei.bin
    python $script_path/../../../lib/scripts/AEI_packager.py "$script_path/../prj/log/metadata.json" "$script_path/../prj/bin/${fileName%.*}.bin"
    if [ $? -ne 0 ];then
         exit -1
    fi
    
    xclbin_name=$dcp_Name
    dcp_Name="$script_path/../prj/bin/${fileName%.*}_aei.bin"
    metadata=`cat $script_path/../prj/log/metadata.json`
    fileName=${fileName%.*}_aei.bin

fi

######################################################################################################################
#Upload
######################################################################################################################
echo "#############################################################"
echo "Register AEI"
echo "#############################################################"
if [ "$mode" == "DPDK" ];then
    id_info=`fis fpga-image-register --fpga-image-file "$script_path/build/checkpoints/to_facs/$fileName" --name "$AEI_Name" --metadata "$metadata" --description "$AEI_Description"`
    rm -rf $script_path/build/checkpoints/to_facs/${fileName%.*}*

elif [ "$mode" == "OCL" ];then
    id_info=`fis fpga-image-register --fpga-image-file "$dcp_Name" --name "$AEI_Name" --metadata "$metadata" --description "$AEI_Description"`
    rm $script_path/../prj/bin/*.bin
fi

python -c "print '''$id_info'''"
if [ -n "`echo $id_info|grep "Success:"`" -a $mode == "OCL" ];then
    shell_id=`echo $id_info|sed 's/^.*id:.*\([a-f0-9]\{32\}\).*/\1/g'`
    chmod +x $script_path/../../../lib/scripts/xclbinaddaei.py
    python $script_path/../../../lib/scripts/xclbinaddaei.py $xclbin_name $shell_id >/dev/null 2>&1
fi
