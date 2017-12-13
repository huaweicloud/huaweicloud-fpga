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

# proj_setting="$USER_DIR/sim/scripts/project_settings.cfg"

compilelib_dir="$LIB_DIR/sim/precompiled"

# Load base.sh
source $common

# Load project settings
# source $proj_setting

# Get simulator
simulator=`get_simulator $@`

# Check simulator
if [ $simulator == "vivado" ] ; then
    # Do not precompile simlib when simulator is xsim
    echo "No need to precompile simulation libs for vivado xsim."
    exit
elif [ $simulator == "vcs" ] ; then
    compilelib_dir="$compilelib_dir/vcs_lib"
elif [ $simulator == "questa" ] ; then
    compilelib_dir="$compilelib_dir/questa_lib"
fi

# Check compilelib_dir
if [ ! -d "$compilelib_dir" ] ; then
    /bin/mkdir -p $compilelib_dir
fi

cd $compilelib_dir

# Check vivado version
vivado_ver=`vivado -version | grep Vivado | awk {'print $2'}`
vivado_verx="x$vivado_ver"
if [ $vivado_verx == "x" ]; then
    echo "Vivado can not find, make sure vivado has been installed."
    exit
fi

# Check vcs version
if [ $simulator == "vcs" ] ; then
    vcs_home=`echo $VCS_HOME`
    vcs_homex="x$vcs_home"
    if [ $vcs_homex == "x" ]; then
        echo "Vcs can not find, make sure vcs has been installed."
        exit
    fi
elif [ $simulator == "questa" ] ; then
    questa_dir=`which vsim`
    questa_dir=${questa_dir%%vsim*}
    if [ "x$questa_dir" == "x" ]; then
        echo "Questasim can not find, make sure questasim has been installed."
        exit
    fi
fi

# Check whether sim_lib exists
if [ -d "$compile_simlib/unisim" ] ; then
    /bin/rm -fr $compile_simlib/*
fi

# Generate lib compiling tcl
if [ $simulator == "vcs" ] ; then
    echo "compile_simlib -language all -dir {$compilelib_dir} -simulator vcs -simulator_exec_path {$vcs_home/bin} -library unisim -family  virtexuplus -no_ip_compile" > simlib.tcl
elif [ $simulator == "questa" ] ; then
    echo "compile_simlib -language all -dir {$compilelib_dir} -simulator questa -simulator_exec_path {$questa_dir/linux_x86_64} -library unisim -family  virtexuplus -no_ip_compile" > simlib.tcl
fi

vivado -mode batch -source $compilelib_dir/simlib.tcl

