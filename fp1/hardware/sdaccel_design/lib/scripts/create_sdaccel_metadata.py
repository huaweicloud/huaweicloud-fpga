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
'''create ocl metadata.json'''
import json
import xml.dom.minidom
import os,sys,re
import time
import hashlib

#*********************************
# Parse the xclbin file
#*********************************

xclbin_path=os.path.abspath(sys.argv[1])
xclbin_name=os.path.basename(os.path.splitext(xclbin_path)[0])
dcp_path=os.path.abspath(sys.argv[2])

bitstream=open(dcp_path,"rb").read()
dcp_sha256=hashlib.sha256(bitstream).hexdigest()

json_path=os.path.abspath(os.path.join(os.path.dirname(xclbin_path),"../bin/manifest.json"))
if os.path.exists(json_path):
    os.remove(json_path)


#read xclbin get the xml index
f=open(xclbin_path,"rb")
bit_len=f.read().index("<?xml")
f.seek(bit_len,0)
xml_len=f.read().index("</project>")
f.seek(bit_len,0)
xml_len +=10
xml_stream=f.read(xml_len)
f.close()
#*********************************
# get xclbin_timestamp and clock_freq and clock_freqone
#*********************************
try:
    dom = xml.dom.minidom.parseString(xml_stream)
    item=dom.getElementsByTagName('platform')[0]
    xclbin_timestamp=item.getAttribute('featureRomTime')

    itemlist=dom.getElementsByTagName('clock')
except:
    print "ERROR:get xclbin_timestamp and clock_freq fail"
    sys.exit(1)

for x in itemlist:
    #print x.getAttribute('port')
    if x.getAttribute('port')=="DATA_CLK":
        clockFreq=x.getAttribute('frequency')

    if x.getAttribute('port')=="KERNEL_CLK":
        clockFreqone=x.getAttribute('frequency')
		
clock_split=re.compile(r'([0-9]\d*)([a-zA-Z]*)').findall(clockFreq)
clock_freq=str(int(clock_split[0][0]))

clock_splitone=re.compile(r'([0-9]\d*)([a-zA-Z]*)').findall(clockFreqone)
clock_freqone=str(int(clock_splitone[0][0]))

file_time=os.path.getctime(xclbin_path)
create_time=time.strftime('%Y-%m-%d %H:%M:%S',time.localtime(file_time))
#**********************************
#get shell id and hdk_version
#**********************************
version_path=os.path.abspath(os.path.join(sys.path[0],"../../../"))
with open(os.path.join(version_path,"version_note_sdaccel.txt")) as f:
    file_content=f.read()
shell_id=re.compile(r'([0-9]\d*)').findall(file_content)[0]

with open(os.path.join(version_path,"version_hdk_tag.txt")) as f:
    hdk_version=f.read().strip()

#**********************************
#  metadata json
#**********************************
metadata={}
metadata['version']="2.0"
metadata['fpga_board']="vu9p_vb"
metadata['fpga_vendor']="xilinx"
metadata['fpga_chip']="vu9p"
metadata['bitstream_format']="bin"
metadata['flow_type']="sdaccel"
metadata['xclbin_timestamp']=xclbin_timestamp
metadata['clock_freq']=clock_freq
metadata['clock_freq2']=clock_freqone
metadata['shell_id']="0x"+str(shell_id)
metadata['pci_vendor_id']="0x19e5"
metadata['pci_device_id']="0xD512"
metadata['pci_subsystem_id']="0x4341"
metadata['pci_subsystem_vendor_id']="0x10ee"
metadata['tool_version']="2017.4.op"
metadata['hdk_version']=hdk_version
metadata['create_time']=create_time
metadata['dcp_sha256']=dcp_sha256

meta=json.dumps(metadata)
with open(json_path,"w") as f:
    f.write(meta)

