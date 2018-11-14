#!/usr/bin/env python
#
#-------------------------------------------------------------------------------
#      Copyright 2018 Huawei Technologies Co., Ltd. All Rights Reserved.
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
'''create dpdk manifest.json'''
import json
import os,sys,re
import time
import struct
import hashlib

file_time=os.path.getctime(sys.argv[1])
create_time=time.strftime('%Y-%m-%d %H:%M:%S',time.localtime(file_time))
json_path=os.path.abspath(os.path.join(os.path.dirname(sys.argv[1]),"../../checkpoints/to_facs/manifest.json"))
if os.path.exists(json_path):
    os.remove(json_path)

#**********************************
#get sha256 of the dcp file
#**********************************
'''get the argv'''
if os.path.exists(sys.argv[1]):
    dcp_path=sys.argv[1]
else:
    print "ERROR:the dcp file is not found"
    sys.exit(1)
	
bitstream=open(dcp_path,"rb").read()
dcp_sha256=hashlib.sha256(bitstream).hexdigest()	
#print dcp_sha256	
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
#  manifest json
#**********************************
manifest={}
manifest['version']="2.0"
manifest['fpga_board']="vu9p_vb"
manifest['fpga_vendor']="xilinx"
manifest['fpga_chip']="vu9p"
manifest['bitstream_format']="bin"
manifest['flow_type']="vivado"
manifest['shell_id']="0x"+str(shell_id)
manifest['pci_vendor_id']="0x19e5"
manifest['pci_device_id']="0xD503"
manifest['pci_subsystem_id']="0x2000"
manifest['pci_subsystem_vendor_id']="0x19e5"
manifest['hdk_version']=hdk_version
manifest['tool_version']="2017.2"
manifest['create_time']=create_time
manifest['dcp_sha256']=dcp_sha256
meta=json.dumps(manifest)
#print json_path
with open(json_path,"w") as f:
    f.write(meta)
