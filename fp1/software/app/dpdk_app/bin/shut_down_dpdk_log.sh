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

#1. delete relevant directory.
LOGFILE=/var/log/fpga/dpdk.log
LOGPATH=${LOGFILE%/*}
if [ -d $LOGPATH ];then
    echo "this path has exist, delete it!"
    rm -rf $LOGPATH
fi

#2. delete dpdk.conf.
rm -rf /etc/rsyslog.d/dpdk.conf

#3. restart rsyslog service.
service rsyslog restart
