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

proj_setting="$USER_DIR/sim/scripts/project_settings.cfg"

simv_opt=" -reportstats 
           +vcs+lic_wait 
           -parallel
           +fsdb_parallel
           "
vsim_opt=" -batch
           -64
           -quiet
           -onfinish stop
           -suppress 3003,3691,3829,8233,8607,3839
           +notimingchecks
           +fsdb_parallel
           -printsimstats
           +autofindloop
           -lib $USER_DIR/sim/work
           -L $USER_DIR/sim/work
           -wlfcompress
           -debugdb=$TC.dbg
           top
           "
xsim_opt=" tb_top
           "

log_sim="$USER_DIR/sim/report/$TC/log_simulation.log"

sim_dir="$USER_DIR/sim/work"

tc_cfg_file="$USER_DIR/sim/tests/sv/$TC/test.cfg"

fin_tc_cfg_file="$sim_dir/$TC/test.cfg"

wave_dir="$USER_DIR/sim/wave"

# Load base.sh
source $common

# Load project settings
source $proj_setting

# Get simulator
simulator=`get_simulator $@`

if [ ! -d "$USER_DIR/sim/report/$TC" ] ; then
    mkdir -p $USER_DIR/sim/report/$TC
fi

if [ ! -d "$wave_dir/$TC" ] ; then
    mkdir -p $wave_dir/$TC
fi

if [ ! -d "$sim_dir/$TC" ] ; then
    mkdir -p $sim_dir/$TC
fi

# Check whether wave dir existed
if [ ! -d $USER_DIR/sim/wave ] ; then
    mkdir -p $USER_DIR/sim/wave
fi

# Copy all files from testcase dir to work dir
cp -rf $USER_DIR/sim/tests/sv/$TC/* $sim_dir/$TC/

# Check whether tc config file exists
if [ -f "$fin_tc_cfg_file" ] ; then
    rm -f $fin_tc_cfg_file
fi
# Create empty file
touch -f $fin_tc_cfg_file

if [ -f "$tc_cfg_file" ] ; then
    declare -a tc_cfg_file_lines=(`cat $tc_cfg_file`)
    for cfg_line in ${tc_cfg_file_lines[@]}; do
        # Delete empty lines or comment lines
        if [ `check_valid_cfg $cfg_line` ] ; then
            continue
        fi
        if [ $simulator == "vivado" ] ; then
            cfg_line_new=${cfg_line##*+}
            cfg_line_new="--testplusarg $cfg_line_new"
        elif [ $simulator == "questa" ] ; then
            # Avoid the questa warnings, Questasim do not recognize "'h" directly
            # Need add ""
            cfg_line_opt=${cfg_line%%=*}
            cfg_line_val=${cfg_line##*=}
            cfg_line_first=${cfg_line_val:0:1}
            if [ "x$cfg_line_first" == "x'" ] ; then
                cfg_line_new="$cfg_line_opt=\"$cfg_line_val\""
            else
                cfg_line_new=$cfg_line
            fi
        else
            cfg_line_new=$cfg_line
        fi
        echo $cfg_line_new >> $fin_tc_cfg_file
    done
    echo "" >> $fin_tc_cfg_file
fi

cd $sim_dir/$TC

dump_fsdb=`cat $tc_cfg_file | grep +DUMP_FSDB | grep "=1"`
dump_fsdb="x$dump_fsdb"

# Run Simulation
if [ $simulator == "vcs" ] ; then
    # Coverage option
    if [ "x$COV_OPT" != x ] ; then
        for cov_opt in $COV_OPT ; do
            simv_opt+=" -cm $cov_opt "
        done
        simv_opt+=" -cm_name $TC "
    fi
    $sim_dir/simv $simv_opt +WAVE_DIR="$wave_dir/$TC" -l $log_sim -f $fin_tc_cfg_file
elif [ $simulator == "questa" ] ; then
    if [ "x" == $dump_fsdb ] ; then
        do_command="log -r /*;run -all;"
    else
        do_command="run -all;"
    fi
    # Coverage option
    if [ "x$COV_OPT" != x ] ; then
        vsim_opt+=" -coverage "
        do_command+="coverage save -onexit $sim_dir/$TC.ucdb; exit -f;"
    else
        do_command+="exit -f;"
    fi
    # Check verdi
    novas_home=`echo $NOVAS_HOME`
    novas_homex="x$novas_home"
    if [ $novas_homex != "x" ]; then
        vsim_opt="$vsim_opt -pli novas_fli.so "
    fi
    # Run Vsim
    vsim $vsim_opt +WAVE_DIR="$wave_dir/$TC" -l $log_sim -f $fin_tc_cfg_file -wlf $wave_dir/$TC/tb_top.wlf -do "$do_command"
else
    ln -sf $sim_dir/xsim.dir ./
    batch_file="$sim_dir/sim.tcl"
    # Delete batch file
    if [ -f "$batch_file" ] ; then
        /bin/rm -f $batch_file
    fi
    echo "add_wave -r /" >> $batch_file
    echo "run all" >> $batch_file
    echo "quit" >> $batch_file
    # Run Xsim
    xsim $xsim_opt --tclbatch $batch_file --testplusarg WAVE_DIR="$wave_dir/$TC" -log $log_sim -f $fin_tc_cfg_file -wdb $wave_dir/$TC/tb_top.wdb --stats
fi

