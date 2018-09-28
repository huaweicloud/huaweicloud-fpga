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

tc_cfg_file="$USER_DIR/sim/tests/sv/$TC/test.cfg"

dump_fsdb=`cat $tc_cfg_file | grep +DUMP_FSDB | grep "=1"`
dump_vpd=`cat $tc_cfg_file | grep +DUMP_VPD | grep "=1"`

dump_fsdb="x$dump_fsdb"
dump_vpd="x$dump_vpd"

verdi_opt=" -nologo 
            -lib
            "

dve_opt=" -full64
          "

wlf_opt=" -64
          -quiet
          -gui
          "

wdb_opt=" -mode gui
          "

simv_dir="$USER_DIR/sim/work"

wave_dir="$USER_DIR/sim/wave/$TC"

# Load base.sh
source $common

# Load project settings
# source $proj_setting

# Get simulator
simulator=`get_simulator $@`

cd $simv_dir

if [ ! -d "$wave_dir" ] ; then
    echo "No wave files for testcase:$TC exist! Open wave fail!"
    exit
fi

if [ $simulator == "vcs" ] ; then
    if [ "x" != $dump_fsdb ] ; then
        verdi $verdi_opt $sim_dir/work -top tb_top -ssy -ssv -ssz -ssf $wave_dir/tb_top.vf &
    elif [ "x" != $dump_vpd ] ; then
        dve $dve_opt -vpd $wave_dir/tb_top.vpd &
    fi
elif [ $simulator == "questa" ] ; then
    if [ "x" != $dump_fsdb ] ; then
        verdi $verdi_opt $sim_dir/work -top tb_top -ssy -ssv -ssz -ssf $wave_dir/tb_top.vf &
    else
        if [ -f $USER_DIR/sim/work/$TC.dbg ] ; then
            mv $USER_DIR/sim/work/$TC.dbg $wave_dir/tb_top.dbg
        fi
        vsim $wlf_opt -view $wave_dir/tb_top.wlf &
    fi
else
    echo "load_feature simulator" > $simv_dir/$TC/wave.tcl
    echo "open_wave_database {$wave_dir/tb_top.wdb}" > $simv_dir/$TC/wave.tcl
    vivado $wdb_opt -source $simv_dir/$TC/wave.tcl &
fi
