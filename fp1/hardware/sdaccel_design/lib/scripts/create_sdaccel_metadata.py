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
import xml.dom.minidom
import os,sys,re
import time

#*********************************
# Parse the xclbin file
#*********************************
#xcl_header_len=496
xclbin_path=os.path.abspath(sys.argv[1])
xclbin_name=os.path.basename(os.path.splitext(xclbin_path)[0])

#read xclbin get the xml index

xcl_magic_word='\xFF\xFF\xFF\xFF\x00\x00\x00\xBB\x11\x22\x00\x44'
f=open(xclbin_path,"rb")
xcl_header_len=f.read().index(xcl_magic_word)
f.seek(xcl_header_len,0)
bit_len=f.read().index("<?xml")
f.seek(xcl_header_len,0)
bin_stream=f.read(bit_len)
bin_stream_len=len(bin_stream)
#binstream len 4 byte alignment
if bin_stream_len%4 != 0:
    add_lens = 4-bin_stream_len%4
    bin_stream_len += add_lens
    bin_stream += '\x00'*add_lens

xml_stream=f.read()
f.close()

#creat bin file
bin_path=os.path.abspath(os.path.join(os.path.dirname(xclbin_path),xclbin_name+".bin"))
with open(bin_path,"wb") as f:
    f.write(bin_stream)
del bin_stream

#*********************************
# get xclbin_timestamp and clock_freq
#*********************************

dom = xml.dom.minidom.parseString(xml_stream)
item=dom.getElementsByTagName('platform')[0]
xclbin_timestamp=item.getAttribute('featureRomTime')

itemlist=dom.getElementsByTagName('clock')
for x in itemlist:
    #print x.getAttribute('port')
    if x.getAttribute('port')=="DATA_CLK":
        clockFreq=x.getAttribute('frequency')

clock_split=re.compile(r'([0-9]\d*)([a-zA-Z]*)').findall(clockFreq)
clock_freq=str(int(clock_split[0][0]))

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
metadata['version']="AEIv1.0"
metadata['fpga_board']="vu9p_vb"
metadata['fpga_vendor']="xilinx"
metadata['fpga_chip']="vu9p"
metadata['bitstream_format']="bin"
metadata['flow_type']="sdaccel"
metadata['xclbin_timestamp']=xclbin_timestamp
metadata['clock_freq']=clock_freq
metadata['shell_id']="0x"+str(shell_id)
metadata['pci_vendor_id']="0x19e5"
metadata['pci_device_id']="0xD512"
metadata['pci_subsystem_id']="0x4341"
metadata['pci_subsystem_vendor_id']="0x10ee"
metadata['hdk_version']=hdk_version
metadata['create_time']=create_time

json_path=os.path.abspath(os.path.join(os.path.dirname(xclbin_path),"../log/metadata.json"))
meta=json.dumps(metadata)
with open(json_path,"w") as f:
    f.write(meta)

