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

TOP=`pwd` 
BUILD_DIR="../src"
DIST_DIR="../dist"

cd $TOP/$BUILD_DIR 
echo "Entering $TOP/$BUILD_DIR"
RET=$?
if [ $RET -ne 0 ]; 
then
	echo "FPGA_TOOL CLEAN ERROR:Can not cd to $TOP/$BUILD_DIR"
	exit 1
fi

make clean -f Makefile_lib
RET=$?
if [ $RET -ne 0 ]; 
then
    echo "FPGA_TOOL CLEAN ERROR: make clean -f Makefile_lib failed"
    exit 1
fi

make clean -f Makefile_cli
RET=$?
if [ $RET -ne 0 ]; 
then
    echo "FPGA_TOOL CLEAN ERROR: make clean -f Makefile_cli failed"
    exit 1
fi

#Remove dist dir
rm -rf $TOP/$DIST_DIR 

#Remove fpga_tool
if [ -d "/usr/local/bin" ];
then
	rm /usr/local/bin/FpgaCmdEntry
	RET=$?
	if [ $RET -ne 0 ]; 
	then
		echo "FPGA_TOOL CLEAN ERROR:Remove tool failed"
		exit 1
	fi
else
	rm /usr/bin/FpgaCmdEntry
	RET=$?
	if [ $RET -ne 0 ]; 
	then
		echo "FPGA_TOOL CLEAN ERROR:Remove tool failed"
		exit 1
	fi
fi

#Remove libfpgamgmt.so
rm /usr/lib64/libfpgamgmt.so
RET=$?
if [ $RET -ne 0 ]; 
then
	echo "FPGA_TOOL CLEAN ERROR: Remove /usr/lib64/libfpgamgmt.so failed"
	exit 1
fi

cd $TOP

echo "FPGA_TOOL CLEAN MESSAGE:Clean success"


