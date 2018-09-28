//-------------------------------------------------------------------------------
//Copyright (c) 2017 Huawei Technologies Co., Ltd. All Rights Reserved.
//
//This program is free software; you can redistribute it and/or modify
//it under the terms of the Huawei Software License (the "License").
//A copy of the License is located in the "LICENSE" file accompanying 
//this file.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//Huawei Software License for more details. 
//-------------------------------------------------------------------------------


#include "xclFpgaMgmtproxy.h"

#include <fstream>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <unistd.h>

#define OS_FILE_PATH "/usr/lib64/libfpgamgmt.so"

xclFpgaMgmtProxy::xclFpgaMgmtProxy()
{
    mHandle = dlopen(OS_FILE_PATH, RTLD_LAZY | RTLD_GLOBAL);
    if (!mHandle) {
        throw std::runtime_error(dlerror());
    }

    m_FPGA_MgmtInit = (FPGA_MgmtInit)dlsym(mHandle, "FPGA_MgmtInit");
    if (!m_FPGA_MgmtInit) {
        throw std::runtime_error(dlerror());
    }
    
    m_FPGA_MgmtInquireFpgaImageInfo = (FPGA_MgmtInquireFpgaImageInfo)dlsym(mHandle, "FPGA_MgmtInquireFpgaImageInfo");
    if (!m_FPGA_MgmtInquireFpgaImageInfo) {
        throw std::runtime_error(dlerror());
    }
    
    m_FPGA_MgmtLoadHfiImage = (FPGA_MgmtLoadHfiImage)dlsym(mHandle, "FPGA_MgmtLoadHfiImage");
    if (!m_FPGA_MgmtLoadHfiImage) {
        throw std::runtime_error(dlerror());
    }

    m_FPGA_MgmtOpsMutexRlock = (FPGA_MgmtOpsMutexRlock)dlsym(mHandle, "FPGA_MgmtOpsMutexRlock");
    if (!m_FPGA_MgmtOpsMutexRlock) {
        throw std::runtime_error(dlerror());
    }

    m_FPGA_MgmtOpsMutexUnlock = (FPGA_MgmtOpsMutexUnlock)dlsym(mHandle, "FPGA_MgmtOpsMutexUnlock");
    if (!m_FPGA_MgmtOpsMutexUnlock) {
        throw std::runtime_error(dlerror());
    }
}

xclFpgaMgmtProxy::~xclFpgaMgmtProxy()
{
	 dlclose(mHandle);
}
