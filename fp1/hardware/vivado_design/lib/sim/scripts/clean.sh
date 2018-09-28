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


report_dir="$USER_DIR/sim/report/"

work_dir="$USER_DIR/sim/work/"

wave_dir="$USER_DIR/sim/wave/"

if [ -d "$report_dir" ] ; then
    rm -fr $report_dir/*
fi

if [ -d "$wave_dir" ] ; then
    rm -fr $wave_dir/*
fi

if [ -d "$work_dir" ] ; then
    rm -fr $work_dir/*
fi

