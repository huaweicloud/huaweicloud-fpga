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
#import some module

import os,sys
import json
import struct
import hashlib

'''help'''
def user_age():
    print "\033[36;1muser_info:\033[0m"
    print "python AEI_packager.py $metada_json_path $user_bin_path"
    sys.exit(0)

if len(sys.argv)==1:
    user_age()
if sys.argv[1]=='-help' or sys.argv=='-h' or sys.argv=='-H' or len(sys.argv)!=3:
    user_age()

'''get the argv'''
if os.path.exists(sys.argv[1]) and os.path.exists(sys.argv[2]):
    json_path=sys.argv[1]
    bin_path=sys.argv[2]
else:
    print "ERROR:the json or bin file is not found"
    sys.exit(0)

aei_bin=os.path.basename(os.path.splitext(bin_path)[0])
AEI_bin_path=os.path.abspath(os.path.join(os.path.dirname(bin_path),aei_bin+"_aei.bin"))

#***************************#
#     AEI version v1.0      #
#***************************#
AEI_ver="AEIv1.0"

#***************************#
#     metadata_len          #
#***************************#

#get metadata and metada_len

metadata=json.dumps(json.load(open(json_path,"r")))
metadata_len=len(metadata)

#metadata len 4 byte alignment

if metadata_len%4 != 0:
    add_lens = 4-metadata_len%4
    metadata_len += add_lens
    metadata += " "*add_lens
pack_format=">8s2i64s"+str(metadata_len)+"s"

#***************************#
#      bitstream_len        #
#***************************#

bitstream_len=os.path.getsize(bin_path)
bitstream=open(bin_path,"rb").read()

#***************************#
#        reserved           #
#***************************#

reserved=""

#***************************#
#    get AEI_header         #
#***************************#
AEI_header = struct.pack(pack_format,AEI_ver,metadata_len,bitstream_len,reserved,metadata)

#***************************#
#       creat AEI bin       #
#***************************#

with open(AEI_bin_path,"wb") as AEI_bin:
    AEI_bin.write(AEI_header+bitstream)

#**************************+
#       sha256sum          |
#**************************+

AEI_sha256=hashlib.sha256(AEI_header+bitstream).digest()
tail=struct.pack(">32s",AEI_sha256)

with open(AEI_bin_path,"ab") as AEI_bin:
     AEI_bin.write(tail)


