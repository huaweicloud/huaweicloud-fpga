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


common="$LIB_DIR/sim/scripts/base.sh"

urg_opt="-full64
         -lca
         -format both
         -dir
        "
cov_opt="-64
         "

sim_dir="$USER_DIR/sim/work"
cov_dir="$USER_DIR/sim/report/coverage"

# Load base.sh
source $common

# Get simulator
simulator=`get_simulator $@`

if [ $simulator == "vivado" ] ; then
    echo "FATAL: Vivado simulator does not support coverage. Please use vcs/questa instead!"
    exit -1
elif [ ! -d $cov_dir ] ; then
    mkdir -p $cov_dir
fi

echo "Generating Coverage Report..."

# Merge and report coverage
if [ $simulator == "vcs" ] ; then
    urg $urg_opt $sim_dir/simv.vdb -report $cov_dir
elif [ $simulator == "questa" ] ; then
    if [ -f $sim_dir/all.ucdb ] ; then
        rm -f $sim_dir/all.ucdb
    fi
    vcover $cov_opt merge $sim_dir/all.ucdb $sim_dir/*.ucdb
    vcover $cov_opt report -html -htmldir $cov_dir $sim_dir/all.ucdb
fi
echo "Coverage Report has been Generated Successfully."

