#!/bin/bash
#
#-------------------------------------------------------------------------------
#      Copyright (c) 2017 Huawei Technologies Co., Ltd. All Rights Reserved.
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


script_path=$1
cat > $script_path/../prj/bin/manifest.txt <<_eop 
manifest_format_version=1
pci_vendor_id="0x19e5"
pci_device_id="0xD512"
pci_subsystem_id="-"
pci_subsystem_vendor_id="-"
shell_type="0x121"
shell_version="0x0005"
hdk_version="SDx 2017.1"
date=`date +%Y/%m/%d_%H:%M:%S`
_eop