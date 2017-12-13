# !/bin/sh
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


# Modify DDR4 Phy param MR1 to 13'b0001000001001
modify_ddr_mr1 () {
    ip_name=$1
    modify_file="$LIB_DIR/ip/$ip_name/rtl/ip_top/"$ip_name"_ddr4.sv"
    mr1_keyword="parameter         MR1"
    mr1_oldval="13'b0001000000001"
    mr1_newval="13'b0001000001001"
    mr1_old="$mr1_keyword                       = $mr1_oldval,"
    mr1_new="$mr1_keyword                       = $mr1_newval,"

    if [ ! -f $modify_file ] ; then
        echo "Error: ip files $modify_file do not exists."
        exit -1
    fi
    MR1=`cat $modify_file | grep "$mr1_keyword" | awk '{print $4}'`
    if [ "x$MR1" == "x" ] ; then
        echo "Error: Parameter MR1 not found."
        exit -1
    elif [ $MR1 == $mr1_newval ] ; then
        echo "Info: Parameter MR1 has been changed. Ignore"
    else
        sed -i "s/$mr1_old/$mr1_new/g" $modify_file
    fi
}

export CURRENT_USER=`whoami`
TMP_DIR=/tmp/`whoami`/$WORK_DIR

models_dir=$LIB_DIR/sim/vip

if [ ! -d $TMP_DIR ] ; then
    mkdir -p $TMP_DIR
fi

cd $TMP_DIR

# Generate all ip
vivado -mode batch -source $LIB_DIR/scripts/init_ip.tcl

ddr4_model_dir=$models_dir/ddr4_model
ddr4_rdimm_model_dir=$models_dir/ddr4_rdimm_wrapper
ddr4_imports_dir=$TMP_DIR/tmp_prj/tmp_ip_ex/rdimma_x8_16GB_2133Mbps_ex/imports

if [ ! -d $ddr4_model_dir ]; then 
    mkdir -p $ddr4_model_dir
fi
if [ ! -d $ddr4_rdimm_model_dir ]; then
    mkdir -p $ddr4_rdimm_model_dir
fi

# Copy all ddr4 sim model files to lib directory
cp $ddr4_imports_dir/arch_defines.v              $ddr4_model_dir/
cp $ddr4_imports_dir/arch_package.sv             $ddr4_model_dir/
cp $ddr4_imports_dir/ddr4_model.sv               $ddr4_model_dir/
cp $ddr4_imports_dir/ddr4_sdram_model_wrapper.sv $ddr4_model_dir/
cp $ddr4_imports_dir/interface.sv                $ddr4_model_dir/
cp $ddr4_imports_dir/MemoryArray.sv              $ddr4_model_dir/
cp $ddr4_imports_dir/proj_package.sv             $ddr4_model_dir/
cp $ddr4_imports_dir/StateTableCore.sv           $ddr4_model_dir/
cp $ddr4_imports_dir/StateTable.sv               $ddr4_model_dir/
cp $ddr4_imports_dir/timing_tasks.sv             $ddr4_model_dir/
sed -i "s/DDR4_8G_X8/DDR4_16G_X8/g" $ddr4_model_dir/ddr4_sdram_model_wrapper.sv

cp $ddr4_imports_dir/ddr4_bi_delay.sv            $ddr4_rdimm_model_dir/
cp $ddr4_imports_dir/ddr4_db_delay_model.sv      $ddr4_rdimm_model_dir/
cp $ddr4_imports_dir/ddr4_db_dly_dir.sv          $ddr4_rdimm_model_dir/
cp $ddr4_imports_dir/ddr4_dimm.sv                $ddr4_rdimm_model_dir/
cp $ddr4_imports_dir/ddr4_dir_detect.sv          $ddr4_rdimm_model_dir/
cp $ddr4_imports_dir/ddr4_rank.sv                $ddr4_rdimm_model_dir/
cp $ddr4_imports_dir/ddr4_rcd_model.sv           $ddr4_rdimm_model_dir/
cp $ddr4_imports_dir/ddr4_rdimm_wrapper.sv       $ddr4_rdimm_model_dir/
sed -i "s/if(DIMM_MODEL == \"LRDIMM\") begin /if(DIMM_MODEL == \"LRDIMM\") begin : LRDIMM /g" $ddr4_rdimm_model_dir/ddr4_rdimm_wrapper.sv
sed -i 's#else begin //!LRDIMM#else begin : NOLRDIMM //!LRDIMM#g' $ddr4_rdimm_model_dir/ddr4_rdimm_wrapper.sv

# Modified ddra MR to new value
modify_ddr_mr1 "rdimma_x8_16GB_2133Mbps"

# Modified ddrb MR to new value
modify_ddr_mr1 "rdimmb_x8_16GB_2133Mbps"

# Modified ddrd MR to new value
modify_ddr_mr1 "rdimmd_x8_16GB_2133Mbps"

# Delete temp files
rm -fr $TMP_DIR

