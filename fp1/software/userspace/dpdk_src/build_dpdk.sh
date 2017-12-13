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
CUR_PATH=${script%/*}
cd $CUR_PATH
if [ -d $CUR_PATH/securec ];then
	rm -rf $CUR_PATH/securec
	rm -rf $CUR_PATH/dpdk-16.04
fi
# 1. uncompress securec.tar.bz2
tar -xjv -f $CUR_PATH/securec.tar.bz2 
if [ $? -ne 0 ]; then
        echo "uncompress securec.tar.bz2 failed!"
        exit 1
fi
# 2. build securec
cd $CUR_PATH/securec
chmod +x  securec_make.sh
./securec_make.sh
if [ $? -ne 0 ]; then
        echo "build securec failed!"
        exit 2
fi
# 3. uncompress dpdk-16.04
cd $CUR_PATH/
tar -xjv -f dpdk-16.04.tar.bz2
if [ $? -ne 0 ]; then
        echo "uncompress dpdk-16.04.tar.bz2 failed!"
        exit 3
fi
# 4. build dpdk-16.04
cd $CUR_PATH/dpdk-16.04
make config T=x86_64-native-linuxapp-gcc
if [ $? -ne 0 ]; then
        echo "config dpdk-16.04 failed!"
        exit 4
fi
make
if [ $? -ne 0 ]; then
        echo "build dpdk-16.04 failed!"
        exit 5
fi
make install T=x86_64-native-linuxapp-gcc
if [ $? -ne 0 ]; then
        echo "install dpdk-16.04 failed!"
        exit 6
fi
echo "==================build dpdk success============="