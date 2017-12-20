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



#Copyright(c) 2017, Huawei Technology Inc, All right reserved Department
dcp_file_name=$1
dcp_hash=`sha256sum $dcp_file_name`
dcp_hash=${dcp_hash% *}
dcp_path=${dcp_file_name%/*}

cat > $dcp_path/../../reports/manifest.txt <<_eop 
dcp_file_name=$dcp_file_name
manifest_format_version=1
pci_vendor_id="0x19e5"
pci_device_id="0xD503"
pci_subsystem_id="-"
pci_subsystem_vendor_id="-"
dcp_hash=$dcp_hash
shell_type="0x101"
shell_version="0x0023"
hdk_version="Vivado 2017.2"
date=`date +%Y/%m/%d_%H:%M:%S`
_eop

