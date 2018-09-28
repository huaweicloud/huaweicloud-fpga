#!/bin/sh
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


proj_setting="$USER_DIR/sim/scripts/project_settings.cfg"
# Load project settings
. $proj_setting

tc_name=$TC
tc_type="sv"
base_test="tb_reg_test"
dump_fsdb=0
dump_wave=0

# Show usage info
function usage () {
    echo  -e "\e[0;35m Usage: sh create.sh -c / -sv / -f / -w / -n tc_name / -b base_test \e[0m"
    echo  -e "\e[0;35m Create new testcase. \e[0m"
    echo  -e "\e[0;35m example: sh create.sh -sv -n sv_demo_004 \e[0m"
    echo  -e "\e[0;35m example: sh create.sh -sv -n sv_test_001 -b tb_dma_test -f \e[0m"
    echo  -e "\e[0;35m \e[0m"
    echo  -e "\e[0;35m Parameter: \e[0m"
    echo  -e "\e[0;35m         -h / --help          Print usage info \e[0m"
    echo  -e "\e[0;35m         -c                   Create C testcase \e[0m"
    echo  -e "\e[0;35m         -sv                  Create Sv testcase \e[0m"
    echo  -e "\e[0;35m         -b / --base          Basetest name of testcase \e[0m"
    echo  -e "\e[0;35m         -f / --fsdb          Enable verdi debuging \e[0m"
    echo  -e "\e[0;35m         -w / --wave          Enable dumping wave(This option will not exist with --fsdb) \e[0m"
}

# Get param
if [ $# -lt 1 -a "$tc_name" = "$TC" ] ; then
    echo "Using $tc_name as testcase name and $tc_type as testcase type ."
else
    while [ "$1" != "" ]; do
        case $1 in
            -h | -help | --help ) 
                usage
                exit
                ;;
            -c )
                tc_type="c"
                ;;
            -sv )
                tc_type="sv"
                ;;
            -b | -base | --base )
                shift
                base_test=$1
                ;;
            -f | -fsdb | --fsdb )
                dump_fsdb=1
                dump_wave=0
                ;;
            -w | -wave | --wave )
                dump_wave=1
                dump_fsdb=0
                ;;
            *)
                echo "Input param error! Please using -h/--help for suppotted param."
                ;;
        esac
        shift
    done
fi

declare -a exist_tests=$(find "$USER_DIR/sim/tests" -type d -name "$tc_name")
test_dir=${exist_tests[0]}

if [ ${#exist_tests[@]} -ge 1 -a "x$test_dir" != "x" -a "-d $test_dir" ] ; then
    echo "Testcase error! Test $tc_name has exists, please input another testcase name."
    echo ${#exist_tests[@]}
    exit
fi

mkdir -p "$USER_DIR/sim/tests/$tc_type/$tc_name"

test_cfg_name="$USER_DIR/sim/tests/$tc_type/$tc_name/test.cfg"

touch -f "$test_cfg_name"

echo "+TEST_NAME=$base_test" >> $test_cfg_name
echo "+DUMP_FSDB=$dump_fsdb" >> $test_cfg_name
echo "+DUMP_VPD=$dump_wave"  >> $test_cfg_name
