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

#######################################################################################################################
## get script path
#######################################################################################################################
if [[ "$0" =~ ^\/.* ]]; then
    script=$0
else
    script=$(pwd)/$0
fi
script=$(readlink -f $script)
script_path=${script%/*}
cd $script_path
#######################################################################################################################
## Function usage
#######################################################################################################################
function usage
{
    echo " "
    echo "Usage: build.sh [options]"
    echo "Options:"
    echo "   -s | -S | -synth          Only run synthesis "
    echo "   -i | -I | -impl           Only run implementation "
    echo "   -p | -P | -pr             Only run pr_verify "
    echo "   -b | -B | -bit            Only run bitgen "
    echo "   -e | -E | -encrypt        Encrypting RTL Files "
    echo "   -t [num]                  Build after [num] seconds"
    echo " "
    echo "   -s_strategy_help          Synthesis Supported values include: "
    echo "                                 * DEFAULT                     "
    echo "                                 * AreaOptimized_high          "
    echo "                                 * AreaOptimized_medium        "
    echo "                                 * AreaMultThresholdDSP        "
    echo "                                 * AlternateRoutability        "
    echo "                                 * PerfOptimized_high          "
    echo "                                 * PerfThresholdCarry          "
    echo "                                 * RuntimeOptimized            "
    echo "   -i_strategy_help          Implementation Supported values include: "
    echo "                                 * DEFAULT                    "
    echo "                                 * Explore                    "
    echo "                                 * ExplorePostRoutePhysOpt    "
    echo "                                 * WLBlockPlacement           "
    echo "                                 * WLBlockPlacementFanoutOpt  "
    echo "                                 * NetDelay_high              "
    echo "                                 * NetDelay_low               "
    echo "                                 * Retiming                   "
    echo "                                 * ExtraTimingOpt             "
    echo "                                 * RefinePlacement            "
    echo "                                 * SpreadSLLs                 "
    echo "                                 * BalanceSLLs                "
    echo "                                 * SpreadLogic_high           "
    echo "                                 * SpreadLogic_medium         "
    echo "                                 * SpreadLogic_low            "       
    echo "                                 * SpreadLogic_Explore        "
    echo "                                 * SSI_SpreadLogic_high       "
    echo "                                 * SSI_SpreadLogic_low        "
    echo "                                 * SSI_SpreadLogic_Explore    "
    echo "                                 * Area_Explore               "
    echo "                                 * Area_ExploreSequential     "
    echo "                                 * Area_ExploreWithRemap      "
    echo "                                 * Power_DefaultOpt           "
    echo "                                 * Power_ExploreArea          "
    echo "                                 * Flow_RunPhysOpt            "
    echo "                                 * Flow_RunPostRoutePhysOpt   "
    echo "                                 * Flow_RuntimeOptimized      "
    echo "                                 * Flow_Quick                 "
    echo " "

    echo "For more details about strategy  Arguments,please use  -s_strategy_help or -i_strategy_help ."
    echo " "
}
#######################################################################################################################
## Function s_usage
#######################################################################################################################
function syn_usage
{
    echo -e "\033[0;36;1mDEFAULT:\033[0m "
    echo " "
    echo
    echo -e "\033[0;36;1mAreaOptimized_high:\033[0m  "
    echo "     Performs general area optimizations including changing the threshold for control set optimizations, forcing  
     ternary adder implementation, applying lower thresholds for use of carry chain in comparators and also area  
     optimized mux optimizations.                                                                                         "
    echo " "

    echo
    echo -e "\033[0;36;1mAreaOptimized_medium:\033[0m"
    echo "     Performs general area optimizations including changing the threshold for control set optimizations, forcing  
     ternary adder implementation, lowering multiplier threshold of inference into DSP blocks, moving  shift      
     register into BRAM, applying lower thresholds for use of carry chain in comparators and also area optimized 
     mux optimizations                                                                                                    "
    echo " "
    
    echo
    echo -e "\033[0;36;1mAreaMultThresholdDSP:\033[0m"
    echo "     Default options plus the AreaMultThresholdDSP directive which will lower the threshold for inference of 
     multipliers into DSP blocks"
    echo " "
    
    echo
    echo -e "\033[0;36;1mAlternateRoutability:\033[0m"
    echo "     Performs optimizations which creates alternative logic technology mapping, including disabling LUT combining,
     forcing F7/F8/F9 to logic, increasing the threshold of shift register inference."
    echo " "

    echo
    echo -e "\033[0;36;1mPerfOptimized_high:\033[0m"
    echo "     Higher performance designs, resource sharing is turned off, the global fanout guide is set to a lower number,
     FSM extraction forced to one-hot, LUT combining is disabled, equivalent registers are preserved, 
     SRL are inferred with a larger threshold."
    echo " "

    echo
    echo -e "\033[0;36;1mPerfThresholdCarry:\033[0m "
    echo "     Default options plus the FewerCarryChains directive for less inference of carry chains, turning off the LUT
     combining, resource sharing off, retaining equivalent registers"
    echo " "

    echo
    echo -e "\033[0;36;1mRuntimeOptimized:\033[0m"
    echo "     Trades off Performance and Area for better Runtime."
    echo
    echo

}
#######################################################################################################################
## Function i_usage
#######################################################################################################################
function impl_usage
{
    echo -e "\033[0;36;1mDEFAULT:\033[0m"
    echo ""
   
    echo 
    echo -e "\033[0;36;1mExplore:\033[0m"
    echo "     Uses multiple algorithms for optimization, placement, and routing to get potentially better results."
    echo ""
     
    echo
    echo -e "\033[0;36;1mExplorePostRoutePhysOpt:\033[0m"
    echo "     Similar to Peformance_Explore, but enables the physical optimization step (phys_opt_design) with the
     Explore directive after routing."
    echo ""
    
    echo
    echo -e "\033[0;36;1mWLBlockPlacement:\033[0m     "
    echo "     Ignore timing constraints for placing Block RAM and DSPs, use wirelength instead."
    echo ""
    
    echo
    echo -e "\033[0;36;1mWLBlockPlacementFanoutOpt:\033[0m"
    echo "     Ignore timing constraints for placing Block RAM and DSPs, use wirelength instead, and perform 
     aggressive replication of high fanout drivers."
    echo ""
    
    echo
    echo -e "\033[0;36;1mNetDelay_high:\033[0m   "
    echo "     To compensate for optimistic delay estimation, add extra delay cost to long distance and high fanout 
     connections. (high setting, most pessimistic)"
    echo ""
   
    echo 
    echo -e "\033[0;36;1mNetDelay_low:\033[0m  "
    echo "     To compensate for optimistic delay estimation, add extra delay cost to long distance and high fanout 
     connections. low setting, least pessimistic)"
    echo ""
    
    echo
    echo -e "\033[0;36;1mRetiming:\033[0m           "
    echo "     Combines retiming in phys_opt_design with extra placement optimization and higher router delay cost."
    echo ""
    
    
    echo
    echo -e "\033[0;36;1mExtraTimingOpt:\033[0m   "
    echo "     Includes alternate algorithms for timing-driven optimization"
    echo ""
    
    echo
    echo -e "\033[0;36;1mRefinePlacement:\033[0m   "
    echo "     Increase placer effort in the post-placement optimization phase, and disable timing relaxation in the
     router."
    echo ""
    
    echo
    echo -e "\033[0;36;1mSpreadSLLs:\033[0m    "
    echo "     A placement variation for SSI devices with tendency to move SLR crossings horizontally."
    echo ""
    
    echo
    echo -e "\033[0;36;1mBalanceSLLs:\033[0m    "
    echo "     A placement variation for SSI devices with more aggressive crossings of SLR boundaries."
    echo ""
    
    echo
    echo -e "\033[0;36;1mSpreadLogic_high:\033[0m "
    echo "     Spread logic throughout the device to avoid creating congested regions. (high setting: highest degree 
     of spreading)"
    echo ""
    
    echo
    echo -e "\033[0;36;1mSpreadLogic_medium:\033[0m "
    echo "     Spread logic throughout the device to avoid creating congested regions. (medium setting)"
    echo ""
    
    echo
    echo -e "\033[0;36;1mSpreadLogic_low:\033[0m  "
    echo "     Spread logic throughout the device to avoid creating congested regions. (low setting: lowest degree
     of spreading)"
    echo ""
    
    echo
    echo -e "\033[0;36;1mSpreadLogic_Explore:\033[0m"
    echo "     Spread logic throughout the device to avoid creating congested regions and run route_design Explore."
    echo ""
    
    echo
    echo -e "\033[0;36;1mSSI_SpreadLogic_high:\033[0m"
    echo "     Spread logic throughout SSI device to avoid creating congested regions. (high setting: highest degree
     of spreading)"
    echo ""
    
    echo
    echo -e "\033[0;36;1mSSI_SpreadLogic_low:\033[0m"
    echo "     Spread logic throughout SSI device to avoid creating congested regions. (low setting: minimal spreading)"
    echo ""

    echo
    echo -e "\033[0;36;1mSSI_SpreadLogic_Explore:\033[0m "
    echo "     Spread logic throughout the device to avoid creating congested regions and run route_design Explore."
    echo ""

    echo
    echo -e "\033[0;36;1mArea_Explore:\033[0m"
    echo "     Uses multiple optimization algorithms to get potentially fewer LUTs."
    echo ""

    echo
    echo -e "\033[0;36;1mArea_ExploreSequential:\033[0m"
    echo "     Uses multiple optimization algorithms to get potentially fewer LUTs and registers."
    echo ""
    
    echo
    echo -e "\033[0;36;1mArea_ExploreWithRemap:\033[0m"
    echo "     Adds the remap optimization to reduce logic"
    echo ""
    
    echo
    echo -e "\033[0;36;1mPower_DefaultOpt:\033[0m "
    echo "     Adds power optimization (power_opt_design) to reduce power consumption."
    echo ""
    
    echo
    echo -e "\033[0;36;1mPower_ExploreArea:\033[0m"
    echo "     Combines power optimization (power_opt_design) with sequential area reduction to reduce power consumption."
    echo ""

    echo
    echo -e "\033[0;36;1mFlow_RunPhysOpt:\033[0m"
    echo "     Similar to the Implementation Run Defaults, but enables the physical optimization step (phys_opt_design)."
    echo ""

    echo
    echo -e "\033[0;36;1mFlow_RunPostRoutePhysOpt:\033[0m"
    echo "     Similar to the Implementation Run Defaults, but enables the physical optimization step (phys_opt_design)
     before and after routing."
    echo ""
    
    echo
     echo -e "\033[0;36;1mFlow_RuntimeOptimized:\033[0m"
    echo "     Each implementation step trades design performance for better runtime. Physical optimization 
     (phys_opt_design) is disabled."
    echo ""

    echo
    echo -e "\033[0;36;1mFlow_Quick:\033[0m   "
    echo  "     Fastest possible runtime, all timing-driven behavior disabled. Useful for utilization estimation."
    echo 
    echo
}
#######################################################################################################################
## Set the initial value of the step-by-step running
#######################################################################################################################
syhth_en=1
impl_en=1
pr_en=0
bit_en=0
encrypt_en=0
delay_time=0
#######################################################################################################################
## Get the script parameter and running function or change the parameter value
#######################################################################################################################
while [ "$1" != "" ]; do
    case $1 in
        -h | -H | -help | --help )      usage
                                        exit
                                        ;;
        -s_strategy_help )              syn_usage
                                        exit
                                        ;;
        -i_strategy_help )              impl_usage
                                        exit
                                        ;;
        -s | -S | -synth )              syhth_en=0
                                        ;;
        -i | -I | -impl )               impl_en=0
                                        ;;
        -p | -P | -pr )                 pr_en=1
                                        ;;
        -b | -B | -bit )                bit_en=1
                                        ;;
        -e | -E | -encrypt )            encrypt_en=1
                                        ;;
        -t )                            shift
                                        delay_time=$1
                                        ;;
        * ) 
        echo "ERROR:'$1' invalid character!  "   
        echo "        please input the '-h','-H','-help' or '--help' character to get help of build.sh"
        echo
        exit
    esac
    shift
done

#######################################################################################################################
## get user configuration
#######################################################################################################################
source $script_path/usr_prj_cfg

if [ "x${USR_PRJ_NAME}" = x -o "x${USR_TOP}" = x  ]; then
    echo "ERROR:The parameter value cannot be empty,you need to complete usr_prj_cfg file! "
    exit
fi

sleep $delay_time
#######################################################################################################################
## Set the value of the step-by-step
#######################################################################################################################
if [[ $syhth_en == 0 || $impl_en == 0 || $pr_en == 1 || $bit_en == 1 ]]; then

    syhth_en=$[! $syhth_en]
    impl_en=$[! $impl_en]
fi
#######################################################################################################################
## echo the information of the set
#######################################################################################################################
echo "---------------------------------------------------------------------------------"
export SYNTH_EN=$syhth_en
echo -e "INFO: SYNTH_EN      | $SYNTH_EN"

export IMPL_EN=$impl_en
echo -e "INFO: IMPL_EN       | $IMPL_EN"

export PR_EN=$pr_en
echo -e "INFO: PR_EN         | $PR_EN"

export BIT_EN=$bit_en
echo -e "INFO: BIT_EN        | $BIT_EN"

export ENCRYPT_EN=$encrypt_en
echo -e "INFO: ENCRYPT_EN    | $ENCRYPT_EN"
echo "---------------------------------------------------------------------------------"
#######################################################################################################################
## get user configuration parameter and set to global variables
#######################################################################################################################

#---------------------------pri_name----------------------------------#

export PRJ_NAME=${USR_PRJ_NAME}
echo -e "INFO: PRJ_NAME      | $PRJ_NAME"

#---------------------------usr_top-----------------------------------#

export TOP=${USR_TOP}
echo -e "INFO: TOP           | $TOP"

#---------------------------usr_dir-----------------------------------#

export UL_DIR=$(readlink -f $script_path/../)
echo -e "INFO: UL_DIR        | $UL_DIR"

#-------------------------syn_strategy--------------------------------#

export SYN_STRATEGY=${USR_SYN_STRATEGY}
echo -e "INFO: SYN_STRATEGY  | $SYN_STRATEGY"

#------------------------impl_strategy--------------------------------#

export IMPL_STRATEGY=${USR_IMPL_STRATEGY}
echo -e "INFO: IMPL_STRATEGY | $IMPL_STRATEGY"

#------------------------usr_constaraints-----------------------------#
echo "---------------------------------------------------------------------------------"
echo "${USR_CONSTRAINTS}" > $script_path/constraints/$PRJ_NAME.xdc
echo -e "INFO: USR_CONSTRAINTS $USR_CONSTRAINTS"
echo "---------------------------------------------------------------------------------"
#######################################################################################################################
## run the build sh
#######################################################################################################################
source $script_path/../../../lib/scripts/fpga_design_run.sh
wait
