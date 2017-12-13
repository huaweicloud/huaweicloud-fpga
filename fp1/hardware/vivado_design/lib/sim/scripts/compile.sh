#!/bin/sh
#
#-------------------------------------------------------------------------------
#      Copyright (c) 2017 Huawei Technologies Co., Ltd. All Rights Reserved.
# 
#      This program is free software; you can redistribute it and/or modify
#      it under the terms of the Huawei Software License (the "License").
#      A copy of the License is located in the "License" file accompanying 
#      this file.
# 
#      This program is distributed in the hope that it will be useful,
#      but WITHOUT ANY WARRANTY; without even the implied warranty of
#      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#      Huawei Software License for more details. 
#-------------------------------------------------------------------------------


common="$LIB_DIR/sim/scripts/base.sh"

proj_setting="$USER_DIR/sim/scripts/project_settings.cfg"

common_opt_vcs=" -full64 -q +warn=noISALS,noENBL_OPTS "
common_opt_vivado=" -m64 --initfile=$XILINX_VIVADO/data/xsim/ip/xsim_ip.ini --incr "
common_opt_questa=" -64 -quiet +acc=mnbprtu -mfcu -suppress 2181,2897,4308 -O5 -incr "

tb_opt_vcs=" -sverilog
             +incdir+$LIB_DIR/sim/bench
             +incdir+$LIB_DIR/sim/bench/bfm
             +incdir+$LIB_DIR/sim/bench/common
             +incdir+$LIB_DIR/sim/bench/rm
             +incdir+$LIB_DIR/sim/bench/stim
             +incdir+$LIB_DIR/sim/bench/test
             +incdir+$LIB_DIR/sim/bench/bfm/axi4
             +incdir+$LIB_DIR/sim/bench/bfm/axi4l
             +incdir+$LIB_DIR/sim/bench/bfm/axi4s
             +incdir+$LIB_DIR/sim/bench/bfm/ddr
             +incdir+$LIB_DIR/sim/vip/ddr4_model
             $LIB_DIR/sim/bench/tb_top.sv
             $LIB_DIR/sim/bench/tb_pkg.svh
             $LIB_DIR/sim/vip/ddr4_model/ddr4_sdram_model_wrapper.sv
             -y $LIB_DIR/sim/vip/ddr4_model
             -y $LIB_DIR/sim/vip/ddr4_rdimm_wrapper
             -unit_timescale=1ns/1ps
             "
tb_opt_questa=" -sv
                -sv12compat
                +incdir+$LIB_DIR/sim/bench
                +incdir+$LIB_DIR/sim/bench/bfm
                +incdir+$LIB_DIR/sim/bench/common
                +incdir+$LIB_DIR/sim/bench/rm
                +incdir+$LIB_DIR/sim/bench/stim
                +incdir+$LIB_DIR/sim/bench/test
                +incdir+$LIB_DIR/sim/bench/bfm/axi4
                +incdir+$LIB_DIR/sim/bench/bfm/axi4l
                +incdir+$LIB_DIR/sim/bench/bfm/axi4s
                +incdir+$LIB_DIR/sim/bench/bfm/ddr
                +incdir+$LIB_DIR/sim/vip/ddr4_model
                $LIB_DIR/sim/bench/tb_top.sv
                $LIB_DIR/sim/bench/tb_pkg.svh
                $LIB_DIR/sim/vip/ddr4_model/ddr4_sdram_model_wrapper.sv
                -y $LIB_DIR/sim/vip/ddr4_model
                -y $LIB_DIR/sim/vip/ddr4_rdimm_wrapper
                -timescale 1ns/1ps
                "
tb_opt_vivado=" -include $LIB_DIR/sim/bench
                -include $LIB_DIR/sim/bench/bfm
                -include $LIB_DIR/sim/bench/common
                -include $LIB_DIR/sim/bench/rm
                -include $LIB_DIR/sim/bench/stim
                -include $LIB_DIR/sim/bench/test
                -include $LIB_DIR/sim/bench/bfm/axi4
                -include $LIB_DIR/sim/bench/bfm/axi4l
                -include $LIB_DIR/sim/bench/bfm/axi4s
                -include $LIB_DIR/sim/bench/bfm/ddr
                -include $LIB_DIR/sim/vip/ddr4_model
                $LIB_DIR/sim/bench/tb_top.sv
                $LIB_DIR/sim/bench/tb_pkg.svh
                $LIB_DIR/sim/vip/ddr4_model/ddr4_sdram_model_wrapper.sv
                -sourcelibdir $LIB_DIR/sim/vip/ddr4_model
                -sourcelibdir $LIB_DIR/sim/vip/ddr4_rdimm_wrapper
                -define VIVADO
                "

verilog_ana_opt_vcs=" -v2005
                      +notimingchecks
                      +nospecify
                      +libext+extension+.v
                      +libext+extension+.vh
                      +libext+extension+.sv
                      +libext+extension+.svh
                      +define+SIMULATION
                      "
verilog_ana_opt_questa=" +notimingchecks     
                         +nospecify
                         +libext+.v
                         +libext+.vh
                         +libext+.sv
                         +libext+.svh
                         +define+SIMULATION
                         "
verilog_ana_opt_vivado=" --sv                 
                         --relax 
                         -sourcelibext .v
                         -sourcelibext .sv
                         -sourcelibext .svh
                         --define SIMULATION
                         "

vcs_opt=" +rad
          -j8
          -debug_acc+pp+r
          -lca
          +vcsd
          +memcbk
          +vcs+loopdetect
          -reportstats
          "
questa_opt=" -j 8
             -debugdb
             -O5
             +notimingchecks
             +nospecify
             -permissive
             -memopt=4
             -timescale 1ns/1ps
             "
xelab_opt=" --timescale=1ns/1ps
            --mt 8
            --debug typical
            --notimingchecks
            --nospecify
            --O3
            --stats
            --snapshot tb_top
            "

rtl_opt_vcs=" +incdir+$LIB_DIR/interfaces
              +incdir+$LIB_DIR/common/if_cbb
              +incdir+$USER_DIR/src
              -y $LIB_DIR/common
              "
rtl_opt_questa=" +incdir+$LIB_DIR/interfaces
                 +incdir+$LIB_DIR/common/if_cbb
                 +incdir+$USER_DIR/src
                 -y $LIB_DIR/common
                 "
rtl_opt_vivado=" -include $LIB_DIR/interfaces
                 -include $LIB_DIR/common/if_cbb
                 -include $USER_DIR/src
                 -sourcelibdir $LIB_DIR/common
                 "

comp_opt_verdi=" -2012
                 +v2k
                 +systemverilogext+.sv+.svh +verilog2001ext+.v+.vp
                 +incdir+$LIB_DIR/sim/bench
                 +incdir+$LIB_DIR/sim/bench/bfm
                 +incdir+$LIB_DIR/sim/bench/common
                 +incdir+$LIB_DIR/sim/bench/rm
                 +incdir+$LIB_DIR/sim/bench/stim
                 +incdir+$LIB_DIR/sim/bench/test
                 +incdir+$LIB_DIR/sim/bench/bfm/axi4
                 +incdir+$LIB_DIR/sim/bench/bfm/axi4l
                 +incdir+$LIB_DIR/sim/bench/bfm/axi4s
                 +incdir+$LIB_DIR/sim/bench/bfm/ddr
                 +incdir+$LIB_DIR/sim/vip/ddr4_model
                 $LIB_DIR/sim/bench/tb_top.sv
                 $LIB_DIR/sim/bench/tb_pkg.svh
                 $LIB_DIR/sim/vip/ddr4_model/ddr4_sdram_model_wrapper.sv
                 -y $LIB_DIR/sim/vip/ddr4_model
                 -y $LIB_DIR/sim/vip/ddr4_rdimm_wrapper
                 -timescale=1ns/1ps
                 +define+SIMULATION
                 +incdir+$LIB_DIR/interfaces
                 +incdir+$LIB_DIR/common/if_cbb
                 +incdir+$USER_DIR/src
                 -y $LIB_DIR/common
                 -ssv -ssy -ssz
                 "

compilelib_dir="$LIB_DIR/sim/precompiled"

ddr4_ip_file_list="$LIB_DIR/sim/scripts/ddr4_ip_file_list.f"

ip_file_list="$USER_DIR/sim/work/ip_file_list.f"

tc_file_list="$USER_DIR/sim/work/tc_file_list.f"

user_file_list="$USER_DIR/sim/work/user_file_list.f"

rtl_file_list="$USER_DIR/sim/work/rtl_file_list.f"

other_src_file_list="$USER_DIR/sim/work/other_src_file_list.f"

user_macro_list="$USER_DIR/sim/work/user_macro_list.f"

cov_file_list="$USER_DIR/sim/work/cov_file_list.f"

sim_setup="$USER_DIR/sim/work/synopsys_sim.setup"

log_comp="$USER_DIR/sim/report/log_comp.log"

log_elab="$USER_DIR/sim/report/log_elab.log"

log_verdi="$USER_DIR/sim/report/log_verdi.log"

# Check whether work dir existed
if [ ! -d $USER_DIR/sim/work ] ; then
    mkdir -p $USER_DIR/sim/work
fi

# Check whether report dir existed
if [ ! -d $USER_DIR/sim/report ] ; then
    mkdir -p $USER_DIR/sim/report
fi

cd $USER_DIR/sim/work

# Load base.sh
source $common

# Load project settings
source $proj_setting

# Check simulator
check_simulator $@

# Get simulator
simulator=`get_simulator $@`

# Eval env config
# exec $proj_setting

# Generate synopsys_sim.setup(If using synopsys vcs/vcsmx)
if [ $simulator == "vcs" ] ; then
    compilelib_dir="$compilelib_dir/vcs_lib"
    echo "OTHERS = $compilelib_dir/synopsys_sim.setup" > $sim_setup
    echo "WORK >DEFAULT 
    DEFAULT : ./" >> $sim_setup
elif [ $simulator == "questa" ] ; then
    work="$USER_DIR/sim/work"
    touch -f ./_info
    compilelib_dir="$compilelib_dir/questa_lib"
    compiled_work="-L $compilelib_dir/unisims_ver -L $compilelib_dir/unimacro_ver -L $compilelib_dir/secureip "
else
    work="xil_defultlib"
    compiled_work="-L unisims_ver -L unimacro_ver -L secureip "
fi

# Generate tc_file_list
if [ -f "$tc_file_list" ] ; then
    /bin/rm -f $tc_file_list
fi

find $USER_DIR/sim/tests -regex ".+\.sv.?" >> $tc_file_list 

# Generate user_file_list 
if [ -f "$user_file_list" ] ; then
    /bin/rm -f $user_file_list
fi

find $USER_DIR/sim/common -regex ".+\.sv.?" >> $user_file_list 

# Generate rtl_file_list
if [ -f "$rtl_file_list" ] ; then
    /bin/rm -f $rtl_file_list
fi

find $USER_DIR/src -regex ".+\.v.?" >> $rtl_file_list 

# Generate other other_src_file_list
if [ -f "$other_src_file_list" ] ; then
    /bin/rm -f $other_src_file_list
fi

declare -a other_list=(`find $LIB_DIR/common -type f -name "*.v*"`)
for item in ${other_list[@]}; do
    if [ $simulator == "vivado" ] ; then
        # Vivado syntax libfile
        echo "--sourcelibfile $item" >> $other_src_file_list
    else
        echo "-v $item" >> $other_src_file_list
    fi
done

declare -a lib_list=(`find $LIB_DIR/sim/libs -type f -name "*.v*"`)
for item in ${lib_list[@]}; do
    if [ $simulator == "vivado" ] ; then
        # Vivado syntax libfile
        echo "--sourcelibfile $item" >> $other_src_file_list
    else
        echo "-v $item" >> $other_src_file_list
    fi
done

declare -a user_ip_list=(`find $USER_DIR/sim/libs -type f -name "*.v*"`)
for item in ${user_ip_list[@]}; do
    if [ $simulator == "vivado" ] ; then
        # Vivado syntax libfile
        echo "--sourcelibfile $item" >> $other_src_file_list
    else
        echo "-v $item" >> $other_src_file_list
    fi
done

# Find out all dir
other_inc_dir=""
declare -a dir_list=(`find $LIB_DIR/common -name "*.*h"`)
for item in ${dir_list[@]}; do
    filename=`echo ${item##*/}`
    dirname=`echo ${item%%/$filename}`
    if [ $simulator == "vivado" ] ; then
        other_inc_dir="$other_inc_dir -include $dirname"
    else
        other_inc_dir="$other_inc_dir +incdir+$dirname"
    fi
done

# Generate ip file list
if [ -f "$ip_file_list" ] ; then
    /bin/rm -f $ip_file_list
fi

# Generate macro file list
if [ -f "$user_macro_list" ] ; then
    /bin/rm -f $user_macro_list
fi

if [ $simulator == "vcs" ] ; then
    echo "-sverilog" >> $ip_file_list
elif [ $simulator == "questa" ] ; then
    echo "-sv" >> $ip_file_list
fi

declare -a ip_list=(`cat $ddr4_ip_file_list`)
for item in ${ip_list[@]}; do
    if [ $simulator == "vivado" ] ; then
        incdirkey=${item:0:7}
        libkey=${item:0:2}
        if [ "x$incdirkey" == "x+incdir" ] ; then
            dirname=`echo ${item##*+}`
            item="-include $dirname"
        elif [ "x$libkey" == "x-v" ] ; then
            libname=`echo ${item#*v }`
            item="-sourcelibfile $dirname"
        fi
    fi
    echo "$item" >> $ip_file_list
done

if [ $simulator == "vivado" ] ; then
    echo "-include $LIB_DIR/ip/debug_bridge_0/bd_0/ip/ip_0/hdl/verilog" >> $ip_file_list
else
    echo "+incdir+$LIB_DIR/ip/debug_bridge_0/bd_0/ip/ip_0/hdl/verilog" >> $ip_file_list
fi
echo "$LIB_DIR/ip/ila_0/sim/ila_0.v" >> $ip_file_list
echo "$LIB_DIR/ip/debug_bridge_0/sim/debug_bridge_0.v" >> $ip_file_list
echo "$LIB_DIR/ip/debug_bridge_0/bd_0/hdl/bd_54be.v" >> $ip_file_list
echo "$LIB_DIR/ip/debug_bridge_0/bd_0/ip/ip_0/sim/bd_54be_xsdbm_0.v" >> $ip_file_list
echo "$LIB_DIR/ip/debug_bridge_0/bd_0/ip/ip_0/hdl/xsdbm_v3_0_vl_rfs.v" >> $ip_file_list
echo "$LIB_DIR/ip/debug_bridge_0/bd_0/ip/ip_0/hdl/ltlib_v1_0_vl_rfs.v" >> $ip_file_list
echo "$LIB_DIR/ip/debug_bridge_0/bd_0/ip/ip_1/sim/bd_54be_lut_buffer_0.v" >> $ip_file_list
echo "$LIB_DIR/ip/debug_bridge_0/bd_0/ip/ip_1/hdl/lut_buffer_v2_0_vl_rfs.v" >> $ip_file_list
echo "$LIB_DIR/ip/debug_bridge_0/bd_0/hdl/bd_54be_wrapper.v" >> $ip_file_list


echo "$XILINX_VIVADO/data/verilog/src/glbl.v" >> $ip_file_list

touch $user_macro_list
declare -a user_sim_macro=($SIM_MACRO)
for item in ${user_sim_macro[@]}; do
    if [ $simulator == "vivado" ] ; then
        # Vivado syntax libfile
        echo "--define $item" >> $user_macro_list
    else
        echo "+define+$item" >> $user_macro_list
    fi
done

echo
echo "---------------------------------------------------"
echo

# Start Compiling
starttime=`date +'%Y-%m-%d %H:%M:%S'`
echo "Start Compiling...  @$starttime"
echo
echo "---------------------------------------------------"
echo

comp_verdi=0

echo "---------------------------------" >  $log_comp
echo "Compile and analysis Testbench..." >> $log_comp
echo "---------------------------------" >> $log_comp

if [ $simulator == "vcs" ] ; then
    # Generate opt
    common_opt=$common_opt_vcs
    verilog_ana_opt=$verilog_ana_opt_vcs
    # Check verdi
    novas_home=`echo $NOVAS_HOME`
    novas_homex="x$novas_home"
    if [ $novas_homex != "x" ]; then
        comp_verdi=1
        tb_opt_vcs="$tb_opt_vcs +define+VERDI "
    fi
    # Coverage option
    echo "+filelist $rtl_file_list" > $cov_file_list
    if [ "x$COV_OPT" != x ] ; then
        for cov_opt in $COV_OPT ; do
            vcs_opt+=" -cm $cov_opt "
        done
        vcs_opt+=" -cm_hier $cov_file_list "
    fi

    # Compile testbench
    vlogan $common_opt $verilog_ana_opt $tb_opt_vcs -f $tc_file_list -f $user_file_list +incdir+$USER_DIR/sim/common -f $user_macro_list | tee -a $log_comp

    echo "---------------------------------" >> $log_comp
    echo "Compile and analysis RTL..."       >> $log_comp
    echo "---------------------------------" >> $log_comp

    # Compile dut
    vlogan $common_opt $verilog_ana_opt $rtl_opt_vcs -f $rtl_file_list -f $other_src_file_list $other_inc_dir | tee -a $log_comp

    echo "---------------------------------" >> $log_comp
    echo "Compile and analysis IP..."        >> $log_comp
    echo "---------------------------------" >> $log_comp

    # Compile lib
    vlogan $common_opt $verilog_ana_opt $rtl_opt_vcs -f $ip_file_list | tee -a $log_comp

    # Errors detected when compiling skip elab
    has_error=`cat $log_comp | grep "Error-"`
    if [ "x$has_error" != "x" ] ; then
        echo -e "\e[0;34m Error: Error occurs during compiling with vcs. Please check the log \"$log_comp\" for detail. \e[0m"
        exit -1
    fi

    # Elebrate
    vcs $common_opt $vcs_opt -top tb_top -top glbl -l $log_elab
elif [ $simulator == "questa" ] ; then
    # Generate opt
    common_opt=$common_opt_questa
    verilog_ana_opt=$verilog_ana_opt_questa
    # Check verdi
    novas_home=`echo $NOVAS_HOME`
    novas_homex="x$novas_home"
    if [ $novas_homex != "x" ]; then
        comp_verdi=1
        tb_opt_questa="$tb_opt_questa +define+VERDI"
    fi
    # Coverage option
    questa_cov=""
    if [ "x$COV_OPT" != x ] ; then
        questa_cov=" +cover="
        for cov_opt in $COV_OPT ; do
            case $cov_opt in
                line )
                    questa_cov+="s"
                    ;;
                cond )
                    questa_cov+="ce"
                    ;;
                branch )
                    questa_cov+="b"
                    ;;
                tgl )
                    questa_cov+="t"
                    ;;
                fsm )
                    questa_cov+="f"
                    ;;
            esac
        done
        questa_cov+=" -coverudp "
    fi
    # Compile testbench
    vlog $common_opt $verilog_ana_opt $tb_opt_questa -f $tc_file_list -f $user_file_list +incdir+$USER_DIR/sim/common -f $user_macro_list -work $work | tee -a $log_comp

    echo "---------------------------------" >> $log_comp
    echo "Compile and analysis RTL..."       >> $log_comp
    echo "---------------------------------" >> $log_comp

    # Compile dut
    vlog $common_opt $verilog_ana_opt $rtl_opt_questa $questa_cov -f $rtl_file_list -f $other_src_file_list $other_inc_dir -work $work | tee -a $log_comp

    echo "---------------------------------" >> $log_comp
    echo "Compile and analysis IP..."        >> $log_comp
    echo "---------------------------------" >> $log_comp

    # Compile lib
    vlog $common_opt $verilog_ana_opt $rtl_opt_questa -f $ip_file_list -work $work | tee -a $log_comp

    # Errors detected when compiling skip elab
    has_error=`cat $log_comp | grep "Error:"`
    if [ "x$has_error" != "x" ] ; then
        echo -e "\e[0;34m Error: Error occurs during compiling with questasim. Please check the log \"$log_comp\" for detail. \e[0m"
        exit -1
    fi

    # Elebrate
    vopt $common_opt $questa_opt -L $work $compiled_work -work $work tb_top glbl -o top -l $log_elab
else
    if [ -f "$USER_DIR/sim/work/xsim.dir/xil_defultlib/xil_defultlib.rlx" ] ; then
        /bin/rm -f "$USER_DIR/sim/work/xsim.dir/xil_defultlib/xil_defultlib.rlx"
    fi
    # Generate opt
    common_opt=$common_opt_vivado
    verilog_ana_opt=$verilog_ana_opt_vivado
    # Compile testbench
    xvlog $common_opt $verilog_ana_opt $tb_opt_vivado -f $tc_file_list -f $user_file_list -include $USER_DIR/sim/common -f $user_macro_list --work $work | tee -a $log_comp

    echo "---------------------------------" >> $log_comp
    echo "Compile and analysis RTL..."       >> $log_comp
    echo "---------------------------------" >> $log_comp

    # Compile dut
    xvlog $common_opt $verilog_ana_opt $rtl_opt_vivado -f $rtl_file_list -f $other_src_file_list -f $ip_file_list $other_inc_dir --work $work | tee -a $log_comp

    echo "---------------------------------" >> $log_comp
    echo "Compile and analysis IP..."        >> $log_comp
    echo "---------------------------------" >> $log_comp

    # Compile lib
    xvlog $common_opt $verilog_ana_opt $rtl_opt_vivado -f $ip_file_list --work $work | tee -a $log_comp

    # Errors detected when compiling skip elab
    has_error=`cat $log_comp | grep "ERROR:" | grep "VRFC"`
    if [ "x$has_error" != "x" ] ; then
        echo -e "\e[0;34m Error: Error occurs during compiling with vivado. Please check the log \"$log_comp\" for detail. \e[0m"
        exit -1
    fi

    # Elebrate
    xelab $common_opt $xelab_opt -L $work $compiled_work xil_defultlib.tb_top xil_defultlib.glbl -log $log_elab
fi

if [ $comp_verdi -eq 1 ] ; then
    # Compile Verdi
    vericom $comp_opt_verdi -f $tc_file_list -f $user_file_list +incdir+$USER_DIR/sim/common -f $user_macro_list -f $rtl_file_list -f $other_src_file_list -f $ip_file_list $other_inc_dir > $log_verdi&
fi

echo
echo "---------------------------------------------------"
echo

# Start Compiling
endtime=`date +'%Y-%m-%d %H:%M:%S'`
start_seconds=$(date --date="$starttime" +%s)
end_seconds=$(date --date="$endtime" +%s)
echo "Finish Compiling  @$endtime"
echo "Compile elapsed time : "$((end_seconds-start_seconds))"s"
echo
echo "---------------------------------------------------"
echo
