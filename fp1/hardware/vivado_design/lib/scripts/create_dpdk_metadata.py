#!/usr/bin/env python
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
'''create ocl metadata.json'''
import json
import os,sys,re
import time

file_time=os.path.getctime(sys.argv[1])
create_time=time.strftime('%Y-%m-%d %H:%M:%S',time.localtime(file_time))
json_path=os.path.abspath(os.path.join(os.path.dirname(sys.argv[1]),"../../reports/metadata.json"))

#**********************************
#get shell id and hdk_version
#**********************************
version_path=os.path.abspath(os.path.join(sys.path[0],"../../../"))
with open(os.path.join(version_path,"version_note_dpdk.txt")) as f:
    file_content=f.read()
shell_id=re.compile(r'([0-9]\d*)').findall(file_content)[0]

with open(os.path.join(version_path,"version_hdk_tag.txt")) as f:
    hdk_version=f.read().strip()
#**********************************
#  metadata json
#**********************************
metadata={}
metadata['version']="AEIv1.0"
metadata['fpga_board']="vu9p_vb"
metadata['fpga_vendor']="xilinx"
metadata['fpga_chip']="vu9p"
metadata['bitstream_format']="bin"
metadata['flow_type']="vivado"
metadata['shell_id']="0x"+str(shell_id)
metadata['pci_vendor_id']="0x19e5"
metadata['pci_device_id']="0xD502"
metadata['pci_subsystem_id']="0x2000"
metadata['pci_subsystem_vendor_id']="0x19e5"
metadata['hdk_version']=hdk_version
metadata['create_time']=create_time

meta=json.dumps(metadata)
with open(json_path,"w") as f:
    f.write(meta)
