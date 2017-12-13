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
if [[ "$0" =~ ^\/.* ]]; then
	script=$0
else
	script=$(pwd)/$0
fi
script=$(readlink -f $script)
CUR_PATH=${script%/*}
DPDK_DIR=${CUR_PATH}/../../userspace/dpdk_src

# 1. build dpdk-16.04 and securec
cd ${DPDK_DIR}/
chmod +x build_dpdk.sh
./build_dpdk.sh
if [ $? -ne 0 ]; then
        echo "build dpdk failed!"
        exit 1
fi
# 2.  set env path
export DPDK_INCLUDE_HOME=${DPDK_DIR}/dpdk-16.04/x86_64-native-linuxapp-gcc/include
export DPDK_LIB_HOME=${DPDK_DIR}/dpdk-16.04/x86_64-native-linuxapp-gcc/lib
export SECUREC_LIB_HOME=${DPDK_DIR}/securec/lib
export SECUREC_INCLUDE_HOME=${DPDK_DIR}/securec/include

# 3. build dpdk_app
cd ${CUR_PATH}/
make
if [ $? -ne 0 ]; then
        echo "build dpdk app failed!"
        exit 2
fi
echo "==================build dpdk app success============="