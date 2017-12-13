#!/bin/bash

#
#   BSD LICENSE
#
#   Copyright(c)  2017 Huawei Technologies Co., Ltd. All rights reserved.
#   All rights reserved.
#
#   Redistribution and use in source and binary forms, with or without
#   modification, are permitted provided that the following conditions
#   are met:
#
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in
#       the documentation and/or other materials provided with the
#       distribution.
#     * Neither the name of Huawei Technologies Co., Ltd  nor the names of its
#       contributors may be used to endorse or promote products derived
#       from this software without specific prior written permission.
#
#   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
#   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
#   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
#   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
#   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
#   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
#   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
#   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
#   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
#   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
#   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#


export FPGA_TOOL_DIR=${FPGA_TOOL_DIR:=$(pwd)}

echo "FPGA_TOOL SETUP MESSAGE:Done setting environment variables."

if [ -z "$FPGA_TOOL_DIR" ]; 
then
    echo "FPGA_TOOL SETUP ERROR: FPGA_TOOL_DIR environment variable is not set.  Please use 'source fpga_tool_setup.sh' from the fpga directory."
    exit 1
fi

#check gcc
gcc --version &> /dev/null
RET=$?
if [ $RET != 0 ]; 
then
    echo "FPGA_TOOL SETUP ERROR: gcc is not installed."
    exit $RET 
fi

sudo -V &> /dev/null
RET=$?
if [ $RET != 0 ]; 
then
    echo "FPGA_TOOL SETUP ERROR: sudo is not in path or not installed. Driver installations will fail "
    echo "To install drivers please add sudo to your path or install sudo by running \"yum install sudo\" as root "
    exit $RET 
fi

FPGA_TOOL_BUILD_DIR=$FPGA_TOOL_DIR/tools/fpga_tool/build

#make fpga_tool
(cd $FPGA_TOOL_BUILD_DIR && bash fpga_tool_make.sh)
RET=$?
if [ $RET != 0 ]; 
then
    echo "FPGA_TOOL SETUP ERROR: Make fpga tool failed."
    exit $RET
fi
echo "FPGA_TOOL SETUP MESSAGE: Build completed."

#install fpga_tool
bash $FPGA_TOOL_BUILD_DIR/fpga_tool_install.sh
RET=$?
if [ $RET != 0 ]; 
then
    echo "FPGA_TOOL SETUP ERROR: Install fpga tool failed."
    exit $RET
fi

echo "FPGA_TOOL SETUP MESSAGE: Setup fpga_tool success."

