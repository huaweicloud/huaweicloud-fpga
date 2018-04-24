#
#-------------------------------------------------------------------------------
#      Copyright (c) 2017 Huawei Technologies Co., Ltd. All Rights Reserved.
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
#!/bin/bash
LOGFILE=/var/log/fpga/dpdk.log
LOGPATH=${LOGFILE%/*}
CONFIGFILE=/etc/rsyslog.d/dpdk.conf

#1. determine if relevant directory exist.
if [ -d $LOGPATH ];then
    echo "this path has exist, no need to create it!"
else
    mkdir -p $LOGPATH
fi

#2. copy dpdk.conf to system dir.
echo "if (\$programname == 'packet_process') then {" > $CONFIGFILE
echo "action(type=\"omfile\" fileCreateMode=\"0600\" file=\"/var/log/fpga/dpdk.log\")" >> $CONFIGFILE
echo "stop" >> $CONFIGFILE
echo "}" >> $CONFIGFILE

#3. restart rsyslog service.
service rsyslog restart
