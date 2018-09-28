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

#######################################################################################################################
## Get Arguments
#######################################################################################################################
syn_strategy=$SYN_STRATEGY
impl_strategy=$IMPL_STRATEGY

#######################################################################################################################
## Export The environment variables.
#######################################################################################################################
script=$(readlink -f ${BASH_SOURCE[0]})
  echo -e "INFO: script         | $script"
full_script=$(readlink -f $script)
  echo -e "INFO: full_script    | $full_script"
script_dir=$(dirname $full_script)
  echo -e "INFO: script_dir     | $script_dir"
export FPGA_SCRIPT_DIR=$script_dir
  echo -e "INFO: FPGA_SCRIPT_DIR| $FPGA_SCRIPT_DIR"
export LIB_DIR=$(readlink -f $FPGA_SCRIPT_DIR/../../lib)
  echo -e "INFO: LIB_DIR        | $LIB_DIR"


export ENCRYPT_DIR=$UL_DIR/src_encrypt
  echo -e "INFO: ENCRYPT_DIR    | $ENCRYPT_DIR"
export UNUSED_IF_DIR=$LIB_DIR/interfaces
  echo -e "INFO: UNUSED_IF_DIR  | $UNUSED_IF_DIR"

#######################################################################################################################
## clear the src_encrypt directory
#######################################################################################################################  
rm -rf $ENCRYPT_DIR/*

log_file="$UL_DIR/prj/${PRJ_NAME}_terminal_run.log"
#  remove exists log_file

rm -f $UL_DIR/prj/*.log
rm -f $UL_DIR/prj/*.jou
rm -rf "$UL_DIR/prj/build/reports/${PRJ_NAME}_final_timing_summary.rpt"
cd $UL_DIR/
echo -e "INFO: log_file       | $log_file"

#######################################################################################################################
## Get Src_files of ENCRYPT_DIR .Search all files in src directory
####################################################################################################################### 
cp -rf $UL_DIR/src/* $ENCRYPT_DIR

#######################################################################################################################
## Get interfaces of ENCRYPT_DIR .Search all files in interfaces directory
#######################################################################################################################
cp -rf $UNUSED_IF_DIR $ENCRYPT_DIR


#######################################################################################################################
## Function print_pass
#######################################################################################################################
function print_pass {
    echo ""                                                                      |tee -a -i $log_file
    echo "             PPPPPPPPPP        AAAA        SSSSSSSS      SSSSSSSS   "  |tee -a -i $log_file
    echo "             PPPP    PPPP    AAAAAAAA    SSSS    SSSS  SSSS    SSSS "  |tee -a -i $log_file
    echo "             PPPP    PPPP  AAAA    AAAA  SSSS          SSSS         "  |tee -a -i $log_file
    echo "             PPPP    PPPP  AAAA    AAAA    SSSS          SSSS       "  |tee -a -i $log_file
    echo "             PPPPPPPPPP    AAAA    AAAA      SSSS          SSSS     "  |tee -a -i $log_file
    echo "             PPPP          AAAAAAAAAAAA        SSSS          SSSS   "  |tee -a -i $log_file
    echo "             PPPP          AAAA    AAAA          SSSS          SSSS "  |tee -a -i $log_file
    echo "             PPPP          AAAA    AAAA  SSSS    SSSS  SSSS    SSSS "  |tee -a -i $log_file
    echo "             PPPP          AAAA    AAAA    SSSSSSSS      SSSSSSSS   "  |tee -a -i $log_file
    echo ""                                                                      |tee -a -i $log_file
}

#######################################################################################################################
## Function print_fail
#######################################################################################################################
function print_fail {
    echo ""                                                                      |tee -a -i $log_file
    echo "             FFFFFFFFFFFF      AAAA        IIIIIIII    LLLL         "  |tee -a -i $log_file
    echo "             FFFF            AAAAAAAA        IIII      LLLL         "  |tee -a -i $log_file
    echo "             FFFF          AAAA    AAAA      IIII      LLLL         "  |tee -a -i $log_file
    echo "             FFFF          AAAA    AAAA      IIII      LLLL         "  |tee -a -i $log_file
    echo "             FFFFFFFFFF    AAAA    AAAA      IIII      LLLL         "  |tee -a -i $log_file
    echo "             FFFF          AAAAAAAAAAAA      IIII      LLLL         "  |tee -a -i $log_file
    echo "             FFFF          AAAA    AAAA      IIII      LLLL         "  |tee -a -i $log_file
    echo "             FFFF          AAAA    AAAA      IIII      LLLL         "  |tee -a -i $log_file
    echo "             FFFF          AAAA    AAAA    IIIIIIII    LLLLLLLLLLLL "  |tee -a -i $log_file
    echo ""                                                                      |tee -a -i $log_file
}

#######################################################################################################################
## Function print_fatal
#######################################################################################################################
function print_fatal {
    echo ""                                                                              |tee -a -i $log_file
    echo "    FFFFFFFFFFFF      AAAA      TTTTTTTTTTTT      AAAA      LLLL          E "  |tee -a -i $log_file
    echo "    FFFF            AAAAAAAA        TTTT        AAAAAAAA    LLLL            "  |tee -a -i $log_file
    echo "    FFFF          AAAA    AAAA      TTTT      AAAA    AAAA  LLLL          R "  |tee -a -i $log_file
    echo "    FFFF          AAAA    AAAA      TTTT      AAAA    AAAA  LLLL            "  |tee -a -i $log_file
    echo "    FFFFFFFFFF    AAAA    AAAA      TTTT      AAAA    AAAA  LLLL          R "  |tee -a -i $log_file
    echo "    FFFF          AAAAAAAAAAAA      TTTT      AAAAAAAAAAAA  LLLL            "  |tee -a -i $log_file
    echo "    FFFF          AAAA    AAAA      TTTT      AAAA    AAAA  LLLL          O "  |tee -a -i $log_file
    echo "    FFFF          AAAA    AAAA      TTTT      AAAA    AAAA  LLLL            "  |tee -a -i $log_file
    echo "    FFFF          AAAA    AAAA      TTTT      AAAA    AAAA  LLLLLLLLLLLL  R "  |tee -a -i $log_file
    echo ""                                                                              |tee -a -i $log_file
}

#######################################################################################################################
## Function print_unknown
#######################################################################################################################
function print_unknown {
    echo ""                            |tee -a -i $log_file
    echo "                ??????    "  |tee -a -i $log_file
    echo "              ??????????  "  |tee -a -i $log_file
    echo "             ???      ??? "  |tee -a -i $log_file
    echo "                   ?????  "  |tee -a -i $log_file
    echo "                  ???     "  |tee -a -i $log_file
    echo "                 ???      "  |tee -a -i $log_file
    echo "                          "  |tee -a -i $log_file
    echo "                 ???      "  |tee -a -i $log_file
    echo "                 ???      "  |tee -a -i $log_file
    echo ""                            |tee -a -i $log_file
}


#######################################################################################################################
## Function run_prj
#######################################################################################################################
function run_prj {
    local start_time=$(date +%s)

    echo "1. Running Vivado  $(date +%Y%m%d_%H:%M:%S)"  |tee -a -i $log_file

    vivado -mode batch -nojournal -notrace -source $LIB_DIR/scripts/build_facs.tcl 2>&1 |tee -a -i $log_file

    local end_time=$(date +%s)
    ((time_cost=$end_time - $start_time))
    final_time_cost="[$(($time_cost/86400))d_$(($time_cost%86400/3600))h_$((($time_cost%3600)/60))m_$(($time_cost%60))s]"
    #######################################################################################################################
    ## judge synth result
    #######################################################################################################################
    fail_string=""
    unknown_string="warning:"

    if [ $SYNTH_EN == 1 ];then
	    str=`cat $log_file |grep "synth_design completed successfully"`
	    if [ -n "$str" ];then
		    synth_result="successfully"
	    else
		    str=`cat $log_file |grep "synth_design failed"`
		    if [ -n "$str" ];then
			    synth_result="failed"
			    fail_string=$str
		    else
			    synth_result="unknown"
			    unknown_string="Cannot find synth report file or any synth information in report file.
			    Please check the status of $log_file."
		    fi
	    fi
    else
	    synth_result="NA"
    fi
    #######################################################################################################################
    ## judge impl result
    #######################################################################################################################
    if [ $IMPL_EN == 1 ];then
	    timing_info_file="$UL_DIR/prj/build/reports/${PRJ_NAME}_final_timing_summary.rpt"
	    str=`cat $timing_info_file |grep "All user specified timing constraints are met"`
	    if [ -n "$str" ];then
		    impl_result="successfully"
		    if [ -f $UL_DIR/prj/build/checkpoints/to_facs/*.dcp ];then
		    	dcp_name=`echo $UL_DIR/prj/build/checkpoints/to_facs/*.dcp`
		    	sh $FPGA_SCRIPT_DIR/creat_dpdk_manifest.sh $dcp_name
		    fi
	    else
		    str=`cat $timing_info_file |grep "Timing constraints are not met"`
		    if [ -n "$str" ];then
			    impl_result="failed"
			    fail_string=$str
			    rm -f $UL_DIR/prj/build/reports/manifest.txt
		    else
			    impl_result="unknown"
			    unknown_string="Cannot find timing report file or any timing information in timing report file.
			    Please check the status of $timing_info_file"
		    fi
	    fi
    else
	    impl_result="NA"
    fi
    #######################################################################################################################
    ## judge pr result
    #######################################################################################################################
    if [ $PR_EN == 1 ];then
	    str=`cat $log_file |grep -w -A1 "PR_VERIFY: check points"`
	    if [ -n "$str" ];then
		    str=`echo $str | grep "are compatible"`
		    if [ -n "$str" ];then
			    pr_result="successfully"
		    else
			    pr_result="failed"
			    fail_string=$str
		    fi
	    else
		    pr_result="unknown"
 		    unknown_string="Cannot find pr report file or any pr information in report file.
		    Please check the status of $log_file."
	    fi
    else
	    pr_result="NA"
    fi
    #######################################################################################################################
    ## judge bitgen result
    #######################################################################################################################
    if [ $BIT_EN == 1 ];then
	    str=`cat $log_file |grep "write_bitstream completed successfully"`
	    if [ -n "$str" ];then
		    bitgen_result="successfully"
		    rm -f $UL_DIR/prj/build/checkpoints/to_facs/${PRJ_NAME}.bi*

	    else
		    str=`cat $log_file |grep "write_bitstream failed"`
		    if [ -n "$str" ];then
			    bitgen_result="failed"
			    fail_string=$str
		    else
			    bitgen_result="unknown"
			    unknown_string="Cannot find bitgen report file or any bitgen information in report file.
			    Please check the status of $log_file."
		    fi
	    fi
    else
	    bitgen_result="NA"
    fi
    #######################################################################################################################
    ## printf result
    #######################################################################################################################
    echo                                                                                      |tee -a -i $log_file
    echo "---------------------------------------------------------------------------------"  |tee -a -i $log_file
    echo                                                                                      |tee -a -i $log_file
    echo "2. Complete!  $(date +%Y%m%d_%H:%d:%S)"                                             |tee -a -i $log_file
    echo                                                                                      |tee -a -i $log_file
    echo "+-----------+-------------------------------------------------------------------+"  |tee -a -i $log_file
    printf "|   time    | %65s |\n" $final_time_cost                                          |tee -a -i $log_file
    echo "+-----------+-------------------------------------------------------------------+"  |tee -a -i $log_file
    printf "|   synth   | %65s |\n" $synth_result                                             |tee -a -i $log_file
    echo "+-----------+-------------------------------------------------------------------+"  |tee -a -i $log_file
    printf "|   impl    | %65s |\n" $impl_result                                              |tee -a -i $log_file
    echo "+-----------+-------------------------------------------------------------------+"  |tee -a -i $log_file
    printf "|    pr     | %65s |\n" $pr_result                                                |tee -a -i $log_file
    echo "+-----------+-------------------------------------------------------------------+"  |tee -a -i $log_file
    printf "|   bitgen  | %65s |\n" $bitgen_result                                            |tee -a -i $log_file
    echo "+-----------+-------------------------------------------------------------------+"  |tee -a -i $log_file
    #######################################################################################################################
    ## printf final result
    #######################################################################################################################
    total_result="$synth_result $impl_result $pr_result $bitgen_result"
    str=`cat $log_file |grep "ERROR:"`
    if [ -n "$str" ]; then
        print_fatal
        echo "$str"                                     |tee -a -i $log_file
    else
	    str=`echo $total_result |grep "failed"`
	    if [ -n "$str" ];then
		    print_fail
		    echo "$fail_string"
	    else
		    str=`echo $total_result |grep "unknown"`
		    if [ -n "$str" ];then
			    print_unknown
			    echo $unknown_string        |tee -a -i $log_file
		    else
			    print_pass
		    fi
	    fi


    fi
    echo 
    echo
    echo "+------------------------------------------------------------------------------+"  |tee -a -i $log_file
    echo "|                                    END                                       |"  |tee -a -i $log_file
    echo "+------------------------------------------------------------------------------+"  |tee -a -i $log_file

}
#######################################################################################################################
## main
#######################################################################################################################
echo "+-------------------------------------------------------------------------------+"|tee -a -i $log_file
echo "|                                  RUN PROJECT                                  |"|tee -a -i $log_file
echo "+-------------------------------------------------------------------------------+"|tee -a -i $log_file
run_prj

