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

# Print animation '.' for processing
function dots_process () {
    seconds=1
    while :
    do
        sleep $seconds
        echo -n '.'
        if [ -f $WORK_DIR/.find_tmp ] ; then
            exit 0
        fi
    done
}

# Print animation '-, \, |, /' for processing
function rotate_process () {
    INTERVAL=0.5
    RCOUNT="0"
    while : ; do
        ((RCOUNT = RCOUNT + 1))
        case $RCOUNT in
            1)  echo -e '-\b\c'
                sleep $INTERVAL
                ;;
            2)  echo -e '\\\b\c'
                sleep $INTERVAL
                ;;
            3)  echo -e '|\b\c'
                sleep $INTERVAL
                ;;
            4)  echo -e '/\b\c'
                sleep $INTERVAL
                ;;
            *) RCOUNT=0
                ;;
        esac
        if [ -f $precomp_lib_dir/$1/lib_fin ] ; then
            break
        fi
    done
}

function stop_process () {
    if [ ! -f $WORK_DIR/.find_tmp ] ; then
        touch -f $WORK_DIR/.find_tmp
    fi
    # Check whether or not the vcs precompiled lib exists
    if [ ! -d $precomp_lib_dir/vcs_lib/ ] ; then
        mkdir -p $precomp_lib_dir/vcs_lib/
    fi
    if [ ! -f $precomp_lib_dir/vcs_lib/lib_fin ] ; then
        touch -f $precomp_lib_dir/vcs_lib/lib_fin
    fi
    # Check whether or not the questatsim precompiled lib exists
    if [ ! -d $precomp_lib_dir/questa_lib/ ] ; then
        mkdir -p $precomp_lib_dir/questa_lib/
    fi
    if [ ! -f $precomp_lib_dir/questa_lib/lib_fin ] ; then
        touch -f $precomp_lib_dir/questa_lib/lib_fin
    fi
}

# Check and split software dir
function check_soft_dir () {
    if [ $# -lt 1 ] ; then
        echo ""
    elif [ -d $1 ] ; then
        echo $1
    else
        soft_dir_tmp=(${1//:/ })
        if [ ${#soft_dir_tmp[@]} -lt 1 ] ; then
            echo ""
        else
            echo ${soft_dir_tmp[@]}
        fi
    fi
}

# Check whether more than one software detected
# If more than one software detected, return first result.
function check_more_soft () {
    if [ $# -lt 1 ] ; then
        echo ""
    elif [ ! -f $1 ]; then
        echo $1 | awk {'print $1'}
    else
        echo $1
    fi
}

# Show usage info
function usage () {
    echo  -e "\e[0;35m Usage: source setup.sh [software directory] / -c / --clean / -u / -l / -xr \e[0m"
    echo  -e "\e[0;35m Setup hardware develop eviranment. \e[0m"
    echo  -e "\e[0;35m example: source setup.sh \e[0m"
    echo  -e "\e[0;35m example: source setup.sh /opt \e[0m"
    echo  -e "\e[0;35m \e[0m"
    echo  -e "\e[0;35m Parameter: \e[0m"
    echo  -e "\e[0;35m         -h / --help          Print usage info \e[0m"
    echo  -e "\e[0;35m         -c / --clean         Clean all tmp files and simulation library if exists \e[0m"
    echo  -e "\e[0;35m         -u / --usercfg       Specify usercfg file(Using setup.cfg as default) \e[0m"
    echo  -e "\e[0;35m         -s / --su            Specify permitted user(Using root as default) \e[0m"
    echo  -e "\e[0;35m         -l / --license       Specify user license cfg for software \e[0m"
    echo  -e "\e[0;35m         -xr/ --xilinxreq     Specify xilinx vivado version \e[0m"
    echo  -e "\e[0;35m         -x / --usesdx        Using SDAccel develop mode \e[0m"
    echo  -e "\e[0;35m         -v / --verbose       Enable Verbose mode(More infomation will be print) \e[0m"
    echo  -e "\e[0;35m         [software directory] Software install directory(If not define using /software as default) \e[0m"
}

# Touch empty file to stop subprocess
trap "stop_process" 1 2 3 15

# Set env varible
export WORK_DIR=$(cd $(dirname ${BASH_SOURCE[0]});pwd )
export LIB_DIR="$WORK_DIR/hardware/vivado_design/lib"

precomp_lib_sh="$LIB_DIR/sim/scripts/simlib.sh"
precomp_lib_dir="$LIB_DIR/sim/precompiled"

default_cfg="$WORK_DIR/setup.cfg"
setup_cfg=$default_cfg

default_soft_dir="/software"
soft_dir=$default_soft_dir
soft_dir_userdef=0

vivado_ver_req="2017.2"

script_name=${BASH_SOURCE[0]}

# Judge which method that scripts run
if [ $script_name == $0 ] ; then
    # using 'sh ./setup.sh' or './setup.sh' 
    script_exec=1
else
    # using 'source ./setup.sh' or '. ./setup.sh' 
    script_exec=0
fi

quit_script=0
user_permit=""
usercfg_file=""
userlic_cfg=""
xilinxuser_req=""
fpga_dev_mode=0
verbose_mode=0

function info_show() {
    if [ $verbose_mode -ne 0 ] ; then
        echo "$1"
    fi
}

function info_show_n() {
    if [ $verbose_mode -ne 0 ] ; then
        echo -n "$1"
    fi
}

function info_show_e() {
    if [ $verbose_mode -ne 0 ] ; then
        echo -e "$1"
    fi
}

# Check whether or not user specified the software installation directory
if [ $# -lt 1 ] ; then
    soft_dir=$default_soft_dir
else

    while [ "$1" != "" ]; do
        case $1 in
            -h | -H | -help | --help ) 
                usage
                quit_script=1
                break
                ;;
            -c | -clean | --clean )
                rm -fr $precomp_lib_dir/*
                quit_script=1
                break
                ;;
            -u | -usercfg | --usercfg )
                shift
                usercfg_file=$1
                ;;
            -s | -su | --su )
                shift
                user_permit=$1
                ;;
            -l | -license | --license )
                shift
                userlic_cfg=$1
                ;;
            -xr | -xilinxreq | --xilinxreq )
                shift
                xilinxuser_req=$1
                ;;
            -x | -usesdx | --usesdx )
                fpga_dev_mode=1
                ;;
            -v | -verbose | --verbose )
                verbose_mode=1
                ;;
            *)
                soft_dir=`check_soft_dir $1`
                if [ "x$soft_dir" == "x" ] ; then
                    echo -e "\e[0;33m Info: Software directory does not ever exist, using $default_soft_dir as default. \e[0m"
                else
                    soft_dir_userdef=1
                fi
                ;;
        esac
        shift
    done
fi

# If using source xxxxx, error will not cause quit of shell.
if [ $quit_script == 1 -a $script_exec == 0 ] ; then
    return
elif [ $quit_script == 1 ] ; then
    exit
fi

# Load user cfg file
if [ "x$usercfg_file" != "x" ] ; then
    setup_cfg=$userlic_cfg
fi

source $setup_cfg
# Check user permission
if [ "x$user_permit" == "x" -a "x$USER_PERMISSION" == "x" ] ; then
    user_permit="root"
elif [ "x$user_permit" == "x" ] ; then
    user_permit=$USER_PERMISSION
fi

id $user_permit 2>&1 > /dev/null
user_exists=`echo $?`
cur_user=`whoami`

if [ $user_exists -eq 1 ] ; then
    echo -e "\e[0;34m Error: User '$cur_user' was not existed. Please check and select a valid user. \e[0m"
    quit_script=1
elif [ $user_permit != $cur_user -a $cur_user != "root" ] ; then
    # If user is root, give user permission no matter what user_permit is.
    echo -e "\e[0;34m Error: User '$cur_user' was not permitted. Only '$user_permit' have permision to run this scripts. \e[0m"
    quit_script=1
fi

# Check user write permission
if [ $quit_script -eq 0 -a -f $WORK_DIR/.test_permit ] ; then
    rm -f $WORK_DIR/.test_permit
    if [ -f $WORK_DIR/.test_permit ] ; then
        echo -e "\e[0;34m Error: Current user '$cur_user' do not have write permission for this  directory. Please add write permiision. \e[0m"
        quit_script=1
    fi
fi

if [ $quit_script -eq 0 ] ; then
    touch -f $WORK_DIR/.test_permit
    if [ -f $WORK_DIR/.test_permit ] ; then
        # User have write permission
        rm -f $WORK_DIR/.test_permit
    else
        echo -e "\e[0;34m Error: Current user '$cur_user' do not have write permission for this  directory. Please add write permiision. \e[0m"
        quit_script=1
    fi
fi

# If using source xxxxx, error will not cause quit of shell.
if [ $quit_script == 1 -a $script_exec == 0 ] ; then
    return
elif [ $quit_script == 1 ] ; then
    exit
fi

# Load user license cfg
if [ "x$userlic_cfg" != "x" ] ; then
    vivado_lic=$userlic_cfg
elif [ "x$XILINX_LIC_SETUP" != x ] ; then
    vivado_lic=$XILINX_LIC_SETUP
else
    echo -e "\e[0;34m Error: No user-define license find,  vivado can not start, please check the license setup. \e[0m"
    quit_script=1
fi

# Load soft_dir
if [ $soft_dir_userdef -eq 1 ] ; then
    # Do nothing
    soft_dir=$soft_dir
elif [ "x$SOFT_INSTALL_DIR" != "x" ] ; then
    if [ -d $SOFT_INSTALL_DIR ] ; then
        soft_dir=$SOFT_INSTALL_DIR
    fi
fi

# Check whether software directory exists
if [ ! -d $soft_dir ] ; then
    echo -e "\e[0;34m Error: No software directory $soft_dir detected, please check the directory. \e[0m"
    quit_script=1
fi

# Load Xilin User Version Req
if [ "x$xilinxuser_req" != "x" ] ; then
    vivado_ver_req=$xilinxuser_req
elif [ "x$VIVADO_VER_REQ" != "x" ] ; then
    vivado_ver_req=$VIVADO_VER_REQ
fi

# If using source xxxxx, error will not cause quit of shell.
if [ $quit_script == 1 -a $script_exec == 0 ] ; then
    return
elif [ $quit_script == 1 ] ; then
    exit
fi

# Show Fpga Develop Mode
if [ $fpga_dev_mode == 1 -o $FPGA_DEVELOP_MODE != "vivado" ] ; then
    dev_mode_name="SDAccel"
else
    dev_mode_name="Vivado"
fi

echo
echo "---------------------------------------------------"
echo
echo "Fpga develop mode is $dev_mode_name."

# Get software info
echo
echo "---------------------------------------------------"
echo

echo -n "Checking software infomation."

if [ -f $WORK_DIR/.find_tmp ] ; then
    rm -f $WORK_DIR/.find_tmp
fi

# Using subprocess to aviod usless info
(dots_process &)

# Autoscan can be bypassed by user to avoid searching the directory.
# It is recommonded that user should enable 'SOFT_AUTO_SCAN' to find the software install infoamtion if not too many softwares installed
if [ "x$SOFT_AUTO_SCAN" == "xyes" ] ; then
    find $soft_dir/ -type f -name "vsim" -o -type f -name "vivado" -o -type f -name "vcs" -o -type l -name "verdi" > $WORK_DIR/.tmp
else
    # Redirect all software install directory into '.tmp' file
    echo "$VIVADO_INS_DIR" >  $WORK_DIR/.tmp
    echo "$VERDI_INS_DIR"  >> $WORK_DIR/.tmp
    echo "$VCS_INS_DIR"    >> $WORK_DIR/.tmp
    echo "$QUESTA_INS_DIR" >> $WORK_DIR/.tmp
fi

mv -f $WORK_DIR/.tmp $WORK_DIR/.find_tmp
sleep 1
echo
echo
echo "---------------------------------------------------"
echo

echo -n "Checking software license."
# Get license info
lic_info=(${LM_LICENSE_FILE//:/ })
echo "."
echo
echo "---------------------------------------------------"
echo

# Get vivado directory
echo "Checking vivado/sdx infomation..."
echo

echo "---------------------------------------------------"
echo

info_show_n "Checking whether vivado env has been set : "
if [ $fpga_dev_mode -eq 1 -o $FPGA_DEVELOP_MODE != "vivado" ] ; then
    vivado_info=`which vivado 2> /dev/null | grep SDx`
    vivado_name="vivado_sdx"
else
    vivado_info=`which vivado 2> /dev/null | grep -v SDx`
    vivado_name="vivado"
fi

if [ "x$vivado_info" == "x" ] ; then
    if [ $fpga_dev_mode -eq 0 -a $FPGA_DEVELOP_MODE == "vivado" ] ; then
        vivado_info=`cat $WORK_DIR/.find_tmp | grep vivado | grep $vivado_ver_req | grep -v unwrapped | sort -r`
    else
        vivado_info=`cat $WORK_DIR/.find_tmp | grep vivado | grep $vivado_ver_req | grep -v unwrapped | grep SDx | sort -r`
    fi
    info_show_e "\e[0;33m have not been set \e[0m"
    info_show_e "\e[0;32mStart setup vivado env \e[0m"

    # Check vivado info and add vivado to path
    info_show_n "Checking whether vivado install correctly : "
    if [ "x$vivado_info" != "x" ] ; then
        info_show_e "\e[0;32m ok \e[0m"
        # Check whether more than one vivado software detected
        vivado_info=`check_more_soft $vivado_info`
        vivado_dir=${vivado_info%%/bin*}
        if [ $fpga_dev_mode -eq 1 -o $FPGA_DEVELOP_MODE != "vivado" ] ; then
            vivado_dir=${vivado_dir%%/Vivado}
        fi
        info_show_n "Setup vivado env : "
        # Config vivado env
        source $vivado_dir/settings64.sh $vivado_dir > /dev/null
        info_show "vivado installed in $vivado_dir"
        info_show_e "\e[0;32mFinish setup vivado env \e[0m"
        echo "Setup $vivado_name env successful"
    else
        info_show_e "\e[0;34m fail \e[0m"
        echo "Error: No $vivado_name elf file find, please make sure the $vivado_name has install correctly!"
        echo "Setup $vivado_name env fail"
        quit_script=1
    fi
else
    info_show_e "\e[0;32m have been set  \e[0m"
    info_show "Vivado env has been set correctly, skip"
    echo "Setup $vivado_name env successful"
fi

# If using source xxxxx, error will not cause quit of shell.
if [ $quit_script == 1 -a $script_exec == 0 ] ; then
    return
elif [ $quit_script == 1 ] ; then
    exit
fi

# Config vivado license sever
vivado_lic_exist=0
info_show_n "Check whether vivado license has been set : "
for lic in ${lic_info[@]} ; do
    if [ $lic == $vivado_lic ] ; then
        vivado_lic_exist=1
        info_show_e "\e[0;32m has been set \e[0m"
        break
    fi
done

if [ $vivado_lic_exist != 1 ] ; then
    export LM_LICENSE_FILE=$vivado_lic:$LM_LICENSE_FILE
    info_show_e "\e[0;33m no \e[0m"
    info_show "Set vivado license successful."
fi

# Check vivado version
vivado_ver=`vivado -version | grep Vivado | awk {'print $2'}`
if [ $fpga_dev_mode -eq 1 -o $FPGA_DEVELOP_MODE != "vivado" ] ; then
    vivado_ver_req="$vivado_ver_req"_sdx
fi
if [ x$vivado_ver != "xv$vivado_ver_req" ] ; then
    echo "Error: Vivado version not matched, only support vivado$vivado_ver_req!"
    quit_script=1
fi

# If using source xxxxx, error will not cause quit of shell.
if [ $quit_script == 1 -a $script_exec == 0 ] ; then
    return
elif [ $quit_script == 1 ] ; then
    exit
fi

# Add execute permission for all simulation scripts
chmod +x $LIB_DIR/sim/scripts/*

# Get verdi directory
echo
echo "---------------------------------------------------"
echo

echo "Checking verdi infomation..."
info_show_n "Checking whether verdi env has been set : "

verdi_info=`which verdi 2> /dev/null`

if [ "x$verdi_info" != "x" -a "x$NOVAS_HOME" != "x" ] ; then
    info_show_e "\e[0;32m has been set \e[0m"
    info_show "Verdi env has been set correctly, skip!"
    verdi_dir=${verdi_info%%/bin*}
    verdi_install=1
    echo "Setup verdi env successful"
else
    info_show_e "\e[0;33m has not been set \e[0m"
    info_show_e "\e[0;32mStart setup verdi env \e[0m"
    info_show_n "Checking whether verdi install correctly : "
    verdi_info=`cat $WORK_DIR/.find_tmp | grep verdi | sort -r`
    # Check verdi info and add verdi to path
    if [ "x$verdi_info" != "x" ] ; then
        info_show_e "\e[0;32m ok \e[0m"
        # Check whether more than one verdi software detected
        verdi_info=`check_more_soft $verdi_info`
        info_show_n "Setup verdi env : "
        verdi_dir=${verdi_info%%/bin*}
        # Config verdi env
        export NOVAS_HOME="$verdi_dir"
        export PATH="$NOVAS_HOME/bin/":$PATH
        export LD_LIBRARY_PATH="$NOVAS_HOME/share/PLI/VCS/LINUX64":$LD_LIBRARY_PATH
        export LD_LIBRARY_PATH="$NOVAS_HOME/share/PLI/MODELSIM/LINUX64/":$LD_LIBRARY_PATH
        verdi_install=1
        info_show "verdi installed in $verdi_dir"
        echo "Setup verdi env successful"
    else
        info_show_e "\e[0;33m fail \e[0m"
        echo "No verdi elf file find, skip verdi env setup!"
        verdi_install=0
    fi
fi

# Get vcs directory
echo
echo "---------------------------------------------------"
echo

echo "Checking vcs infomation..."
info_show_n "Checking whether vcs env has been set : "

vcs_info=`which vcs 2> /dev/null`

if [ "x$vcs_info" != "x" -a "x$VCS_HOME" != "x" ] ; then
    info_show_e "\e[0;32m has been set \e[0m"
    info_show "Vcs env has been set correctly, skip!"
    echo "Setup vcs env successful"
    vcs_dir=${vcs_info%%/bin*}
    vcs_install=1
else
    info_show_e "\e[0;33m has not been set \e[0m"
    info_show_e "\e[0;32mStart setup vcs env \e[0m"
    info_show_n "Checking whether vcs install correctly : "
    vcs_info=`cat $WORK_DIR/.find_tmp | grep bin | grep vcs | sort -r`
    # Check vcs info and add vcs to path
    if [ "x$vcs_info" != "x" ] ; then
        info_show_e "\e[0;32m ok \e[0m"
        # Check whether more than one vcs software detected
        vcs_info=`check_more_soft $vcs_info`
        info_show_n "Setup vcs env : "
        vcs_dir=${vcs_info%%/bin*}
        # Config vcs env
        export VCS_HOME="$vcs_dir"
        export PATH="$VCS_HOME/bin/":$PATH
        vcs_install=1
        info_show "vcs installed in $vcs_dir"
        echo "Setup vcs env successful"
    else
        info_show_e "\e[0;33m fail \e[0m"
        echo "No vcs elf file find, skip vcs env setup!"
        vcs_install=0
    fi
fi

# Config vcs license sever
if [ $vcs_install -eq 1 ] ; then
    info_show_n "Check whether vcs license has been set : "
    vcs_lic_exist=0
    vcs_lic_setup=0
    vcs_lic=$SYNOPSYS_LIC_SETUP
    if [ "x$vcs_lic" != "x" ] ; then
        for lic in ${lic_info[@]} ; do
            if [ $lic == $vcs_lic ] ; then
                info_show_e "\e[0;32m has been set \e[0m"
                vcs_lic_exist=1
                break
            fi
        done
        if [ $vcs_lic_exist != 1 ] ; then
            export LM_LICENSE_FILE=$vcs_lic:$LM_LICENSE_FILE
            info_show_e "\e[0;33m has not been set \e[0m"
            info_show "Start setup vcs license."
            info_show "Set vcs license : $vcs_lic"
            vcs_lic_setup=1
        fi
    elif [ "x$userlic_cfg" != "x" ] ; then
        info_show_e "\e[0;33m ommit \e[0m"
        info_show "User define license detected. Skip vcs license setup."
        vcs_lic_setup=1
    else
        info_show_e "\e[0;33m ommit \e[0m"
        echo "No valid vcs license exists. Skip vcs license setup."
    fi

    # Precompile simulation library only when vcs license has been setup correctly.
    if [ $vcs_lic_exist -eq 1 -o $vcs_lic_setup -eq 1 ] ; then
        # Precompile simulation library
        info_show_n "Check vcs precompiled sim library status : "
        precomp_rslt=0
        if [ -d $precomp_lib_dir/vcs_lib ] ; then
            precomp_rslt=`grep -c "100.00 %" $precomp_lib_dir/vcs_lib/compile_simlib.log 2> /dev/null`
            if [ "x$precomp_rslt" != "x" -a "x$precomp_rslt" != "x0" ] ; then
                info_show_e "\e[0;32m existed, skip compiling \e[0m"
            elif [ -f $precomp_lib_dir/vcs_lib/lib_fin ] ; then
                rm -f $precomp_lib_dir/vcs_lib/lib_fin
            fi
        fi
        
        if [ "x$precomp_rslt" == "x" -o "x$precomp_rslt" == "x0" ] ; then
            info_show_e "\e[0;32m did not exist, start compiling \e[0m"
            echo "Precompile vcs simulation library : "
            (rotate_process "vcs_lib"  &)
            precompile_info=`exec $precomp_lib_sh vcs | grep "100.00 % complete"`

            if [ "x$precompile_info" != "x" ] ; then
                info_show_e "\e[0;32m ok \e[0m"
            else
                info_show_e "\e[0;34m fail \e[0m"
                echo "Error: Error occurs during precompiling xilinx library with vcs."
                echo "Precompile xilinx library with vcs fail"
            fi
        fi
        touch "$precomp_lib_dir/vcs_lib/lib_fin"
    fi
fi

# Get questasim directory
echo
echo "---------------------------------------------------"
echo

echo "Checking questa infomation..."
info_show_n "Checking whether questa env has been set : "
questa_info=`which vsim 2> /dev/null`

if [ "x$questa_info" != "x" ] ; then
    info_show_e "\e[0;32m has been set \e[0m"
    info_show "Questasim env has been set correctly, skip!"
    echo "Setup questasim env successful"
    questa_install=1
    questa_dir=${questa_info%%/vsim}
else
    info_show_e "\e[0;33m has not been set \e[0m"
    info_show_e "\e[0;32mStart setup questasim env \e[0m"
    info_show_n "Checking whether questasim install correctly : "
    questa_info=`cat $WORK_DIR/.find_tmp | grep vsim | grep 64 | sort -r`
    # Check questa info and add questa to path
    if [ "x$questa_info" != "x" ] ; then
        info_show_e "\e[0;32m ok \e[0m"
        # Check whether more than one questasim software detected
        questa_info=`check_more_soft $questa_info`
        info_show_n "Setup questasim env : "
        questa_dir=${questa_info%%/vsim}
        # Config questasim env
        export PATH="$questa_dir":$PATH
        questa_install=1
        info_show "questasim installed in $questa_dir"
        echo "Setup questasim env successful"
    else
        info_show_e "\e[0;33m fail \e[0m"
        echo "No questa elf file find, skip questasim env setup!"
        questa_install=0
    fi
fi

# Config questasim license sever
if [ $questa_install -eq 1 ] ; then
    info_show_n "Check whether questasim license has been set : "
    questa_lic_exist=0
    questa_lic_setup=0
    questa_lic=$MENTOR_LIC_SETUP
    if [ "x$questa_lic" != "x" ] ; then
        for lic in ${lic_info[@]} ; do
            if [ $lic == $questa_lic ] ; then
                info_show_e "\e[0;32m has been set \e[0m"
                questa_lic_exist=1
                break
            fi
        done

        if [ $questa_lic_exist != 1 ] ; then
            export LM_LICENSE_FILE=$questa_lic:$LM_LICENSE_FILE
            info_show_e "\e[0;33m has not been set \e[0m"
            info_show "Start setup questasim license."
            info_show "Set questasim license : $vcs_lic"
            questa_lic_setup=1
        fi
    elif [ "x$userlic_cfg" != "x" ] ; then
        info_show_e "\e[0;33m ommit \e[0m"
        info_show "User define license detected. Skip questasim license setup."
        questa_lic_setup=1
    else
        info_show_e "\e[0;33m ommit \e[0m"
        echo "No valid questasim license exists. Skip questasim license setup."
    fi

    # Precompile simulation library only when questasim license has been setup correctly.
    if [ $questa_lic_exist -eq 1 -o $questa_lic_setup -eq 1 ] ; then
        # Precompile simulation library
        info_show_n "Check questasim precompiled sim library status : "
        precomp_rslt=0
        if [ -d $precomp_lib_dir/questa_lib ] ; then
            precomp_rslt=`grep -c "100.00 %" $precomp_lib_dir/questa_lib/compile_simlib.log 2> /dev/null`
            if [ "x$precomp_rslt" != "x" -a "x$precomp_rslt" != "x0" ] ; then
                info_show_e "\e[0;32m existed, skip \e[0m"
            elif [ -f $precomp_lib_dir/questa_lib/lib_fin ] ; then
                rm -f $precomp_lib_dir/questa_lib/lib_fin
            fi
        fi

        if [ "x$precomp_rslt" == "x" -o "x$precomp_rslt" == "x0" ] ; then
            info_show_e "\e[0;32m did not exist, start compiling \e[0m"
            echo "Precompile questasim simulation library : "
            (rotate_process "questa_lib"  &)
            precompile_info=`exec $precomp_lib_sh questa | grep "100.00 % complete"`

            if [ "x$precompile_info" != "x" ] ; then
                info_show_e "\e[0;32m ok \e[0m"
            else
                info_show_e "\e[0;34m fail \e[0m"
                echo "Error: Error occurs during precompiling xilinx library with questasim."
                echo "Precompile xilinx library with questasim fail"
            fi
        fi
        touch "$precomp_lib_dir/questa_lib/lib_fin"
    fi
fi

rm -f $WORK_DIR/.find_tmp

if [ $fpga_dev_mode -eq 0 -a $FPGA_DEVELOP_MODE == "vivado" ] ; then
    # Check vivado ip
    echo
    echo "---------------------------------------------------"
    echo

    ip_broken=0

    echo "Checking ip infomation..."
    info_show_n "Checking whether rdimma_x8_16GB_2133Mbps ip has existed : "
    if [ -d $LIB_DIR/ip/rdimma_x8_16GB_2133Mbps ] ; then
        if [ -f $LIB_DIR/ip/rdimma_x8_16GB_2133Mbps/rdimma_x8_16GB_2133Mbps.xci ] ; then
            info_show_e "\e[0;32m existed \e[0m"
            info_show_n "Checking whether rdimma_x8_16GB_2133Mbps ip version matched : "
            ip_version=`cat $LIB_DIR/ip/rdimma_x8_16GB_2133Mbps/rdimma_x8_16GB_2133Mbps.xci | grep SWVERSION`
            ip_version=${ip_version##*SWVERSION\">}
            ip_version=${ip_version%%</spirit*}
            if [ "v$ip_version" == "$vivado_ver" ] ; then
                info_show_e "\e[0;32m matched \e[0m"
            else
                info_show_e "\e[0;33m did not match \e[0m"
                info_show "Need regenerating rdimma_x8_16GB_2133Mbps ip"
                ip_broken=1
            fi
        else
            info_show_e "\e[0;33m did not exist \e[0m"
            info_show "Need generating rdimma_x8_16GB_2133Mbps ip"
            ip_broken=1
        fi
    else
        info_show_e "\e[0;33m did not exist \e[0m"
        info_show "Need generating rdimma_x8_16GB_2133Mbps ip"
        ip_broken=1
    fi

    info_show_n "Checking whether rdimmb_x8_16GB_2133Mbps ip exists : "

    if [ -d $LIB_DIR/ip/rdimmb_x8_16GB_2133Mbps ] ; then
        if [ -f $LIB_DIR/ip/rdimmb_x8_16GB_2133Mbps/rdimmb_x8_16GB_2133Mbps.xci ] ; then
            info_show_e "\e[0;32m existed \e[0m"
            info_show_n "Checking whether rdimmb_x8_16GB_2133Mbps ip version matched : "
            ip_version=`cat $LIB_DIR/ip/rdimmb_x8_16GB_2133Mbps/rdimmb_x8_16GB_2133Mbps.xci | grep SWVERSION`
            ip_version=${ip_version##*SWVERSION\">}
            ip_version=${ip_version%%</spirit*}
            if [ "v$ip_version" == "$vivado_ver" ] ; then
                info_show_e "\e[0;32m matched \e[0m"
            else
                info_show_e "\e[0;33m did not match \e[0m"
                info_show "Need regenerating rdimmb_x8_16GB_2133Mbps ip"
                ip_broken=1
            fi
        else
            info_show_e "\e[0;33m did not exist \e[0m"
            info_show "Need generating rdimmb_x8_16GB_2133Mbps ip"
            ip_broken=1
        fi
    else
        info_show_e "\e[0;33m did not exist \e[0m"
        info_show "Need generating rdimmb_x8_16GB_2133Mbps ip"
        ip_broken=1
    fi

    info_show_n "Checking whether rdimmd_x8_16GB_2133Mbps ip exists : "

    if [ -d $LIB_DIR/ip/rdimmd_x8_16GB_2133Mbps ] ; then
        if [ -f $LIB_DIR/ip/rdimmd_x8_16GB_2133Mbps/rdimmd_x8_16GB_2133Mbps.xci ] ; then
            info_show_e "\e[0;32m existed \e[0m"
            info_show_n "Checking whether rdimmd_x8_16GB_2133Mbps ip version matched : "
            ip_version=`cat $LIB_DIR/ip/rdimmd_x8_16GB_2133Mbps/rdimmd_x8_16GB_2133Mbps.xci | grep SWVERSION`
            ip_version=${ip_version##*SWVERSION\">}
            ip_version=${ip_version%%</spirit*}
            if [ "v$ip_version" == "$vivado_ver" ] ; then
                info_show_e "\e[0;32m matched \e[0m"
            else
                info_show_e "\e[0;33m did not match \e[0m"
                info_show "Need regenerating rdimmd_x8_16GB_2133Mbps ip"
                ip_broken=1
            fi
        else
            info_show_e "\e[0;33m did not exist \e[0m"
            info_show "Need generating rdimmd_x8_16GB_2133Mbps ip"
            ip_broken=1
        fi
    else
        info_show_e "\e[0;33m did not exist \e[0m"
        info_show "Need generating rdimmd_x8_16GB_2133Mbps ip"
        ip_broken=1
    fi

    info_show_n "Checking whether ila_0 ip exists : "

    if [ -d $LIB_DIR/ip/ila_0 ] ; then
        if [ -f $LIB_DIR/ip/ila_0/ila_0.xci ] ; then
            info_show_e "\e[0;32m existed \e[0m"
            info_show_n "Checking whether ila_0 ip version matched : "
            ip_version=`cat $LIB_DIR/ip/ila_0/ila_0.xci | grep SWVERSION`
            ip_version=${ip_version##*SWVERSION\">}
            ip_version=${ip_version%%</spirit*}
            if [ "v$ip_version" == "$vivado_ver" ] ; then
                info_show_e "\e[0;32m matched \e[0m"
            else
                info_show_e "\e[0;33m did not match \e[0m"
                info_show "Need regenerating ila_0 ip"
                ip_broken=1
            fi
        else
            info_show_e "\e[0;33m did not exist \e[0m"
            info_show "Need generating ila_0 ip"
            ip_broken=1
        fi
    else
        info_show_e "\e[0;33m did not exist \e[0m"
        info_show "Need generating ila_0 ip"
        ip_broken=1
    fi

    info_show_n "Checking whether debug_bridge_0 ip exists : "

    if [ -d $LIB_DIR/ip/debug_bridge_0 ] ; then
        if [ -f $LIB_DIR/ip/debug_bridge_0/debug_bridge_0.xci ] ; then
            info_show_e "\e[0;32m existed \e[0m"
            info_show_n "Checking whether debug_bridge_0 ip version matched : "
            ip_version=`cat $LIB_DIR/ip/debug_bridge_0/debug_bridge_0.xci | grep SWVERSION`
            ip_version=${ip_version##*SWVERSION\">}
            ip_version=${ip_version%%</spirit*}
            if [ "v$ip_version" == "$vivado_ver" ] ; then
                info_show_e "\e[0;32m matched \e[0m"
            else
                info_show_e "\e[0;33m did not match \e[0m"
                info_show "Need regenerating debug_bridge_0 ip"
                ip_broken=1
            fi
        else
            info_show_e "\e[0;33m did not exist \e[0m"
            info_show "Need generating debug_bridge_0 ip"
            ip_broken=1
        fi
    else
        info_show_e "\e[0;33m did not exist \e[0m"
        info_show "Need generating debug_bridge_0 ip"
        ip_broken=1
    fi

    info_show_n "Checking whether ddr sim model exists : "

    if [ -d $LIB_DIR/sim/vip/ddr4_model -a -d $LIB_DIR/sim/vip/ddr4_rdimm_wrapper ] ; then
        if [ -f $LIB_DIR/sim/vip/ddr4_model/arch_defines.v -a -f $LIB_DIR/sim/vip/ddr4_rdimm_wrapper/ddr4_rdimm_wrapper.sv ] ; then
            info_show_e "\e[0;32m existed \e[0m"
        else
            info_show_e "\e[0;33m did not exist \e[0m"
            info_show "Need generating ddr sim model"
            ip_broken=1
        fi
    else
        info_show_e "\e[0;33m did not exist \e[0m"
        info_show "Need generating ddr sim model"
        ip_broken=1
    fi

    if [ $ip_broken -eq 1 ] ; then
        echo "Need regenerating all ip. "
        echo
        echo "---------------------------------------------------"
        echo
        echo "Regenerating ip. Please wait 3~5 minutes. "
        rm -fr $LIB_DIR/sim/vip/ddr4_model
        rm -fr $LIB_DIR/sim/vip/ddr4_rdimm_wrapper
        rm -fr $LIB_DIR/ip/rdimma_x8_16GB_2133Mbps
        rm -fr $LIB_DIR/ip/rdimmb_x8_16GB_2133Mbps
        rm -fr $LIB_DIR/ip/rdimmd_x8_16GB_2133Mbps
        rm -fr $LIB_DIR/ip/ila_0
        rm -fr $LIB_DIR/ip/debug_bridge_0
        sh $LIB_DIR/scripts/init_ip.sh > /dev/null
        echo "Finish generate ip. "
    else
        echo "Do not need regenerating ip. "
    fi
fi
echo
echo "---------------------------------------------------"
echo
####################################################################################################
#add new develop
####################################################################################################
echo "Check the driver status..."
bonding_log=/var/log/fpga/install_driver.log
mkdir -p ${bonding_log%/*}
if [ "$dev_mode_name" == "Vivado" ];then
    #***************************************************
    # if the device is exist
    #***************************************************
    declare -a bdf_list=(`lspci |grep "19e5:d503" |awk '{print $1}'`)
    if [ ${#bdf_list[*]} -eq 0 ];then
	    echo "Warning: The device does not exist."
    else
        bound="Kernel driver in use: igb_uio"
        #***************************************************
        # if the vf_driver is bonding
        #***************************************************
        if [ "`lspci -s ${bdf_list[0]} -vvv|grep "$bound"`" ];then
            echo "Driver has bound to device."
        else
            echo "Installing and binding driver to device. Please wait 3~5 minutes. "
            sh  $WORK_DIR/software/platform_config/dpdk_cfg/vivado_env_cfg.sh 2>&1 > $bonding_log
            dpdk_bind=`dpdk_nic_bind.py --status | grep d503`
            if [ -n "`echo $dpdk_bind | grep "drv=igb_uio"`" ];then
                echo "Driver installation and binding success!"
            else
                echo "ERROR: Driver installation or binding failed! Please check $bonding_log!"
                quit_script=1
            fi
                
        fi
    fi
elif [  "$dev_mode_name" == "SDAccel" ];then
    sh $WORK_DIR/software/platform_config/sdaccel_cfg/sdaccel_env_cfg.sh 2>&1 > $bonding_log
    if [ $? == 0 ];then
        echo "SDAccel configuration successful!"
    else
        echo "ERROR: SDAccel configuration failed! please check $bonding_log!"
        quit_script=1
    fi
fi

if [ $quit_script == 1 -a $script_exec == 0 ] ; then
     return
elif [ $quit_script == 1 ] ; then
     exit
fi

echo
echo "---------------------------------------------------"
echo
####################################################################################################
#download file from OBS
####################################################################################################
#check the OBS_URL
echo "Check the obs url..."

if [ "x$OBS_URL" == "x" ];then
    echo "ERROR:Current OBS_URL is empty,please Config it"
    quit_script=1
elif [ -z "`ping -c 1 -W 5 ${OBS_URL#*//} | grep 'bytes from'`" ];then
    echo "ERROR:Current $OBS_URL couldn't be accessed,please check your URL or your network"
    quit_script=1
fi

#if the URL is empty,quit the script
if [ $quit_script == 1 -a $script_exec == 0 ] ; then
     return
elif [ $quit_script == 1 ] ; then
     exit
fi
#
obs_shell_dir=$OBS_URL
#get the obs version and local version
if [ "$dev_mode_name" == "Vivado" ];then
    declare -a download_file_list=("SH_UL_BB_routed.dcp"
                                    "SH_UL_BB_routed.sha256"
                                    "ddr_72b_top.tar.gz"
                                    "ddr_72b_top.sha256"
                                )
    version_name=version_note_dpdk.txt
    shell_mode=hardware/vivado_design
    local_shell_file=SH_UL_BB_routed.dcp
    local_shell_dir=$LIB_DIR/checkpoints
    local_ddr_dir=$LIB_DIR/common/ddr_ctrl  
    declare -a local_file_sha256=("`cat $local_shell_dir/${download_file_list[0]} 2>/dev/null|sha256sum|awk '{print $1}'`"
     "`cat $local_ddr_dir/ddra_72b_top.dcp $local_ddr_dir/ddrb_72b_top.dcp $local_ddr_dir/ddrd_72b_top.dcp $LIB_DIR/sim/libs/ddra_72b_top_sim.v $LIB_DIR/sim/libs/ddrb_72b_top_sim.v $LIB_DIR/sim/libs/ddrc_72b_top_sim.v $LIB_DIR/sim/libs/ddrd_72b_top_sim.v 2>/dev/null  |sha256sum | awk '{print $1}'`"                          
    )
    declare -a download_file_name=("dcp" "ddr")

elif [  "$dev_mode_name" == "SDAccel" ];then
    declare -a download_file_list=("xilinx_huawei-vu9p-fp1_4ddr-xpr_4_1.tar.gz"
                                    "xilinx_huawei-vu9p-fp1_4ddr-xpr_4_1.sha256"
                                )
    version_name=version_note_sdaccel.txt
    shell_mode=hardware/sdaccel_design
    local_shell_file=xilinx_huawei-vu9p-fp1_4ddr-xpr_4_1.dsa
    local_shell_dir=$WORK_DIR/hardware/sdaccel_design/lib/platform/xilinx_huawei-vu9p-fp1_4ddr-xpr_4_1
    declare -a local_file_sha256=("`cat $local_shell_dir/xilinx_huawei-vu9p-fp1_4ddr-xpr_4_1.dsa $local_shell_dir/xilinx_huawei-vu9p-fp1_4ddr-xpr_4_1.spfm $local_shell_dir/xilinx_huawei-vu9p-fp1_4ddr-xpr_4_1.xpfm 2>/dev/null|sha256sum|awk '{print $1}'`")
    declare -a download_file_name=("dsa" "pfm")
fi

obs_version_note=`curl -k -s --retry 3 $obs_shell_dir/hardware/$version_name`
local_version_note=`cat $WORK_DIR/hardware/$version_name 2> /dev/null`
#download the version note Failed
if [ -n "`echo $obs_version_note | grep '<?xml version'`" -o "x$obs_version_note" == "x" ];then
     echo -e "ERROR:Failed to get $version_name from \e[0;36m$obs_shell_dir \e[0m"
     quit_script=1
 fi
#if verion is eq
if [ $quit_script != 1 ];then
    if [ "x$local_version_note" == "x" ];then
        quit_script=1
        echo "ERROR:Your project is Incomplete,please update your Project"
    elif [ "$obs_version_note" != "$local_version_note" ];then
        quit_script=1
        echo  "Warning:your version is not the latest,please update the local file from github"
    fi
fi
#If vresion not match , error will not cause quit of shell.
if [ $quit_script == 1 -a $script_exec == 0 ] ; then
    echo
    echo "---------------------------------------------------"
    echo
    return
elif [ $quit_script == 1 ] ; then
    echo
    echo "---------------------------------------------------"
    echo
    exit
fi
#if download?
to_download_file=0
if [ -f "$local_shell_dir/$local_shell_file" ];then
    for ((i=0;i<${#local_file_sha256[*]};i++))
    do
        obs_dcp_sum=`curl -k -s --retry 3 $obs_shell_dir/$shell_mode/$obs_version_note/${download_file_list[(2*$i+1)]}`
        if [ -n "`echo $obs_dcp_sum | grep '<?xml version'`" ];then
            echo -e "ERROR:Failed to get ${download_file_list[(2*$i+1)]} from \e[0;36m$obs_shell_dir/$shell_mode/$obs_version_note/ \e[0m"
            quit_script=1
            break
        fi
        if [ "${local_file_sha256[$i]}" != "$obs_dcp_sum" -a $quit_script != 1 ];then
            to_download_file=1
            break
        fi 
    done
else
    to_download_file=1
fi
#download_file
if [ $to_download_file -eq 1 -a $quit_script != 1 ];then
    echo -e "Download the \e[0;32m ${download_file_name[0]}\e[0m and \e[0;32m ${download_file_name[1]}\e[0m files..."
    cd $local_shell_dir  >/dev/null
    (
    for files in "${download_file_list[@]}"
    do 
        curl -k -s -O --retry 3 $obs_shell_dir/$shell_mode/$obs_version_note/$files &
    done
    wait
    )
    tar zxvf *.tar.gz >/dev/null
    if [ $? != 0 ];then
        echo -e "Failed to download the ${download_file_list[$i]} "
        quit_script=1

    fi
    if [ $quit_script != 1 ];then
        if [ "$dev_mode_name" == "Vivado" ];then
            declare -a download_file_sha256=("`cat $local_shell_dir/${download_file_list[0]} 2>/dev/null |sha256sum|awk '{print $1}'`"
                                        "`cat ddra_72b_top.dcp ddrb_72b_top.dcp ddrd_72b_top.dcp ddra_72b_top_sim.v ddrb_72b_top_sim.v ddrc_72b_top_sim.v ddrd_72b_top_sim.v 2>/dev/null |sha256sum| awk '{print $1}'`"                          
    )
        elif [ "$dev_mode_name" == "SDAccel"  ];then
            declare -a download_file_sha256=("`cat $local_shell_dir/xilinx_huawei-vu9p-fp1_4ddr-xpr_4_1.dsa $local_shell_dir/xilinx_huawei-vu9p-fp1_4ddr-xpr_4_1.spfm $local_shell_dir/xilinx_huawei-vu9p-fp1_4ddr-xpr_4_1.xpfm 2>/dev/null|sha256sum |awk '{print $1}'`")
        fi
        for ((i=0;i<${#download_file_sha256[*]};i++))
        do
            obs_dcp_sum=`curl -k -s --retry 3 $obs_shell_dir/$shell_mode/$obs_version_note/${download_file_list[(2*$i+1)]}`
            if [ -n "`echo $obs_dcp_sum | grep '<?xml version'`" ];then
                echo -e "ERROR:Failed to get ${download_file_list[(2*$i+1)]} from \e[0;36m$obs_shell_dir/$shell_mode/$obs_version_note/ \e[0m"
                quit_script=1
                break
            fi
            if [ "${download_file_sha256[$i]}" != "$obs_dcp_sum" -a $quit_script != 1 ];then
                echo "ERROR:Download files is incorrect"
                quit_script=1
                break
            fi 
        done
    fi
    quit_script=0
    if [ "$dev_mode_name" == "Vivado" -a $quit_script != 1  ];then
        mkdir -p $local_ddr_dir
        mv -f ddr*.dcp $local_ddr_dir >/dev/null
        mkdir -p $LIB_DIR/sim/libs
        mv -f *.v $LIB_DIR/sim/libs >/dev/null
        rm -rf ${download_file_list[2]} ${download_file_list[3]} 
    elif [ "$dev_mode_name" == "SDAccel" -a $quit_script != 1 ];then
        #info_show "SDA download"
        rm -rf *.tar.gz
    fi
    cd -  >/dev/null
elif [ $quit_script != 1  ];then
    echo -e "Do not need download the \e[0;32m${download_file_name[0]}\e[0m and \e[0;32m${download_file_name[1]}\e[0m files."
fi

echo
echo "---------------------------------------------------"
echo


if [ $quit_script == 1 -a $script_exec == 0 ] ; then
      return
elif [ $quit_script == 1 ] ; then
      exit
fi

####################################################################################################
#end new develop
####################################################################################################

echo "+-----------+--------------------------------------------------------------------------------+"
echo "|   tool    |   install home directory                                                       |"
echo "+-----------+--------------------------------------------------------------------------------+"
printf "|   vivado  | %78s |\n" $vivado_dir
echo "+-----------+--------------------------------------------------------------------------------+"
if [ $verdi_install -eq 1 ] ; then
    printf "|   verdi   | %78s |\n" $verdi_dir
    echo "+-----------+--------------------------------------------------------------------------------+"
fi
if [ $vcs_install -eq 1 ] ; then
    printf "|    vcs    | %78s |\n" $vcs_dir
    echo "+-----------+--------------------------------------------------------------------------------+"
fi
if [ $questa_install -eq 1 ] ; then
    printf "|   questa  | %78s |\n" $questa_dir
    echo "+-----------+--------------------------------------------------------------------------------+"
fi

