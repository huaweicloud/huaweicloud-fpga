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

if [ -z "$FPGA_TOOL_DIR" ]; 
then
	echo "FPGA_TOOL INSTALL ERROR: FPGA_TOOL_DIR environment variable is not set.  Please 'source fpga_tool_setup.sh' from the fpga directory first."
	exit 1
fi

if [ $EUID != 0 ]; 
then
	echo ""
	echo "Root privileges are required to install. You may be asked for your password..."
	sudo -E "$0" "$@"
	exit $?
else
	echo "FPGA_TOOL INSTALL MESSAGE: Executing as root..."
fi

BASE_PATH=/usr/local

echo $PATH | grep "$BASE_PATH" &> /dev/null
ret=$?
if [ $ret -ne "0" ]; 
then
	BASE_PATH=/usr
fi

FPGA_TOOL_DIST_DIR=$FPGA_TOOL_DIR/tools/fpga_tool/dist
FPGA_TOOL_DST_DIR=$BASE_PATH/bin

if [ ! -d "$FPGA_TOOL_DST_DIR" ];
then
	mkdir -p $FPGA_TOOL_DST_DIR
fi

#Copy libfpgamgmt.so to /usr/lib64
cp -f $FPGA_TOOL_DIST_DIR/libfpgamgmt.so /usr/lib64
RET=$?
if [ $RET -ne 0 ]; 
then
    echo "FPGA_TOOL INSTALL ERROR: Copy libfpgamgmt.so to /usr/lib64 failed."
    exit 1
fi
echo "FPGA_TOOL INSTALL MESSAGE: Copy libfpgamgmt.so to /usr/lib64 success "

#Set libfpgamgmt.so privilege
chmod 600 /usr/lib64/libfpgamgmt.so
RET=$?
if [ $RET -ne 0 ]; 
then
    echo "FPGA_TOOL INSTALL ERROR: Set the privilege of /usr/lib64/libfpgamgmt.so failed."
    exit 1
fi
echo "FPGA_TOOL INSTALL MESSAGE: Set the privilege of /usr/lib64/libfpgamgmt.so success"

#Copy fpga tool to /usr/local/bin or /usr/bin
cp -f $FPGA_TOOL_DIST_DIR/FpgaCmdEntry $FPGA_TOOL_DST_DIR
RET=$?
if [ $RET -ne 0 ]; 
then
    echo "FPGA_TOOL INSTALL ERROR:Copy FpgaCmdEntry to $FPGA_TOOL_DST_DIR failed."
    exit 1
fi
echo "FPGA_TOOL INSTALL MESSAGE: Copy FpgaCmdEntry to $FPGA_TOOL_DST_DIR success "

#Set fpga tool privilege
chmod 700 $FPGA_TOOL_DST_DIR/FpgaCmdEntry
RET=$?
if [ $RET -ne 0 ]; 
then
    echo "FPGA_TOOL INSTALL ERROR:Set the privilege of FpgaCmdEntry failed."
    exit 1
fi

echo "FPGA_TOOL INSTALL MESSAGE: Set the privilege of $FPGA_TOOL_DST_DIR/FpgaCmdEntry success"
