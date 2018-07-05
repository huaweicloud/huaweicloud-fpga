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
import struct
from sys import exit
import os,sys

position_aei=416

if sys.argv[1] == '-h' or sys.argv[1] == '--help' :
    sys.stdout.write("Example: ./xclbinaddaei <xclbin> <aeiid>\n")
    exit(0)

argscnt=len(sys.argv)
if argscnt != 3 :
    sys.stdout.write("ERROR: Parameters invalid!\n")
    sys.stdout.write("Example: ./xclbinaddaei.py <xclbin> <aeiid>\n")
    exit(1)

sys.argv[1].strip()
sys.argv[2].strip()

if sys.argv[1].endswith('.xclbin') :
    print ""
else :
    print "Input parameter \"%s\" is not a .xclbin file, Please check and retry!" %(sys.argv[1])
    exit(1)

len_aeiid = len(sys.argv[2])

if len_aeiid != 32 :
    print "AEI id length is not 32, please check and retry"
    exit(1)

print "Xclbin file is :", sys.argv[1] 
print "Input AEI ID is :", sys.argv[2] 

f_xclbin = open(sys.argv[1],"rb+")
f_xclbin.seek(position_aei)
f_xclbin.write(sys.argv[2])

sys.stdout.write("AEI ID has been inserted to xclbin!\n")
sys.stdout.write("\n")

f_xclbin.seek(0)

sys.stdout.write("New xclbin head is :\n")
for i in range(512):
    h_xclbin = f_xclbin.read(1)
    if ((i+1)%16 == 0):
        sys.stdout.write("%4x" % ord(h_xclbin))
        print ""
    else:
        sys.stdout.write("%4x" % ord(h_xclbin))

f_xclbin.seek(position_aei)

sys.stdout.write("AEI ID from xclbin is :\n")
sys.stdout.write("%s\n" % f_xclbin.read(32))

f_xclbin.close()
