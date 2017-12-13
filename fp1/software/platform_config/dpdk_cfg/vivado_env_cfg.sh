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
#***************************************************
# Compiling the dpdk and app
#***************************************************
#Compiling the dpdk
igb_uio_path=$script_path/../../userspace/dpdk_src
sh $igb_uio_path/build_dpdk.sh

kernel_version=`uname -r`
echo "cp igb_uio.ko to /lib/modules/$kernel_version/extra/"
cp -rf $igb_uio_path/dpdk*/x86_64*/kmod/igb_uio.ko /lib/modules/`uname -r`/extra/
echo 'depmod -a'
depmod -a
echo 'cp dpdk_nic_bind.py to /usr/bin '
cp -rf $igb_uio_path/dpdk*/tools/dpdk_nic_bind.py /usr/bin/
echo 'cp fpga-server-guest to /etc/rc.d/init.d'
cp -rf $script_path/fpga-server-guest /etc/rc.d/init.d/

sh $script_path/fpga-server-guest

rc_local_file="/etc/rc.d/rc.local"
rc_append="sh /etc/rc.d/init.d/fpga-server-guest"
chmod +x $rc_local_file
while read Line
do
    if [ "$Line" = "$rc_append" ]; then
	echo "already append $rc_append in $rc_local_file, ignoring it"
	exit 0
    fi
done < $rc_local_file
chmod +w $rc_local_file
echo "$rc_append" >> $rc_local_file
chmod -w $rc_local_file
echo "Finish......"


