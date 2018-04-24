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
s3cmd_info=`which s3cmd 2> /dev/null`
if [ "x$s3cmd_info" == "x" ];then
    echo -e "ERROR:s3cmd tool is not detected, please refer to the FACS User Guide, go to \033[0;35;1mhttp://s3tools.org/download\033[0m to download and intall s3cmd."
    exit
fi
fis_info=`which fis 2> /dev/null`
if [ "x$fis_info" == "x" ];then
    echo -e "ERROR:fis tool is not detected, please refer to the FACS User Guide, go to intall fis."
    exit
fi
fischeck_info=`which fischeck 2> /dev/null`
if [ "x$fischeck_info" == "x" ];then
    echo -e "ERROR:fischeck tool is not detected, please refer to the FACS User Guide, go to intall fischeck."
    exit
fi

#####################################################################################################################
#get the config from the AEI_Register.cfg
#####################################################################################################################
source "$script_path/AEI_Register.cfg"
bucketName=${OBS_BUCKETNAME}
mode=${MODE}
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
########################################################
#judge the .s3cfg file
########################################################
if [ -f ~/.s3cfg ];then
    host_base=`cat  ~/.s3cfg| grep "host_base"`
    if [ -z "$host_base" ];then
        echo "ERROR:the ~/.s3cfg file's configuration is wrong "
        exit -1
    fi
    bucket_location=`cat  ~/.s3cfg| grep "bucket_location"`
    if [ -z "$bucket_location" ];then
        echo "ERROR:the ~/.s3cfg file's configuration is wrong "
        exit -1
    fi
else
    echo "ERROR:the ~/.s3cfg does not exist"
    exit -1
fi


#####################################################################################################################
#verifying the fis register arguments
#####################################################################################################################
if [ "$bucketName" == "" ];then
    echo "ERROR: bucketName is invalid"
    exit -1
else

    fischeck --args-only --location "$bucketName:$fileName" --name "$AEI_Name" --metadata "$metadata" --description "$AEI_Description"
    if [ $? != 0 ];then
        echo -e "\nverifying the fis register arguments failed"
        exit -1
    fi
fi
#####################################################################################################################
#verifying the bucketName,access_key,secret_key
#####################################################################################################################

i_times=3
while [ $i_times -gt 0 ];do
    read -p "Input access_key:" -s ACCESS_KEY
    echo
    read -p "Input secret_key:" -s SECRET_KEY
    echo
    i_times=$(($i_times-1))
    bucket_error=`s3cmd --ssl --no-check-certificate --access_key="$ACCESS_KEY" --secret_key="$SECRET_KEY" --host-bucket="" du "s3://$bucketName" 2>&1`
    err_code=$?
    if [ $err_code == 0 ];then
        echo -e "\nVerifying the access_key,secret_key successfully"
        break
    elif [ $err_code == 12 ];then
        echo -e "\nThe bucket which named $bucketName is not exist!"
        exit -1
    elif [[ $bucket_error =~ .*(AccessDenied).* ]];then
        echo $bucket_error
        exit -1
    elif [ $i_times -ne 0  ];then
        echo $bucket_error
        echo
        continue
    else
        echo -e "\nERROR:verifying the secret_key,secret_key fails,please try again."
        exit -1
    fi
done

#####################################################################################################################
#Used for authenticating the password and /etc/cfg.file
#####################################################################################################################
i_times=3
while [ $i_times -gt 0 ];do
    read -p "Input passwd:" -s Passwd
    i_times=$(($i_times-1))
    fischeck --password "$Passwd"
    if [ $? == 0 ];then
        echo -e "\nverifying the password and /etc/cfg.file successfully"
        break
    elif [ $i_times -ne 0  ];then
        echo
        continue
    else
        echo -e "\nERROR:verifying the password and /etc/cfg.file fails, please try again."
        exit -1
    fi
done
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
    #packager aei.bin
    python $script_path/../../../lib/scripts/AEI_packager.py "$script_path/build/reports/metadata.json" "$script_path/build/checkpoints/to_facs/$fileName"

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
    #packager aei.bin
    python $script_path/../../../lib/scripts/AEI_packager.py "$script_path/../prj/log/metadata.json" "$script_path/../prj/bin/${fileName%.*}.bin"
    
    xclbin_name=$dcp_Name
    dcp_Name="$script_path/../prj/bin/${fileName%.*}_aei.bin"
    metadata=`cat $script_path/../prj/log/metadata.json`
    fileName=${fileName%.*}_aei.bin

fi

######################################################################################################################
#Upload
######################################################################################################################
if [ "$mode" == "DPDK" ];then
    s3cmd --ssl --no-check-certificate --access_key=$ACCESS_KEY --secret_key=$SECRET_KEY --host-bucket="" put "$script_path/build/checkpoints/to_facs/$fileName" "s3://$bucketName"
    rm  $script_path/build/checkpoints/to_facs/${fileName%.*}*
 
elif [ "$mode" == "OCL" ];then
    s3cmd --ssl --no-check-certificate --access_key=$ACCESS_KEY --secret_key=$SECRET_KEY --host-bucket="" put "$dcp_Name" "s3://$bucketName"
    rm $script_path/../prj/bin/*.bin
fi

######################################################################################################################
#Register
######################################################################################################################
echo "#############################################################"
echo "Register AEI"
echo "#############################################################"
if [ "$AEI_Description" == "" ];then
    id_info=`fis --password "$Passwd" fpga-image-register --location "$bucketName:$fileName" --name "$AEI_Name" --metadata "$metadata"`
else
    id_info=`fis --password "$Passwd" fpga-image-register --location "$bucketName:$fileName" --name "$AEI_Name" --metadata "$metadata" --description "$AEI_Description"`
fi
echo $id_info
if [ -n "`echo $id_info|grep "Success:"`" -a $mode == "OCL" ];then
    shell_id=`echo $id_info|sed 's/^.*id:.*\([a-f0-9]\{32\}\).*/\1/g'`
    chmod +x $script_path/../../../lib/scripts/xclbinaddaei
    $script_path/../../../lib/scripts/xclbinaddaei $xclbin_name $shell_id >/dev/null 2>&1 
fi
