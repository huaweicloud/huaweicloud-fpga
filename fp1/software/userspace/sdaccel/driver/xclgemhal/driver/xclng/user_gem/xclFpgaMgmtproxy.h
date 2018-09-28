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
#ifndef XCLFPGA_MGMT_PROXY_H
#define XCLFPGA_MGMT_PROXY_H

#include <stdexcept>
#include <cstdlib>
#include <dlfcn.h>
#include "FPGA_Common.h"

class xclFpgaMgmtProxy
{
public:
    typedef UINT32 (* FPGA_MgmtInit)();
    typedef UINT32 (* FPGA_MgmtInquireFpgaImageInfo)(UINT32 ulSlotIndex, FPGA_IMG_INFO *pstrImgInfo);
    typedef UINT32 (* FPGA_MgmtLoadHfiImage)(UINT32 ulSlotIndex, INT8 *pcHfiId);
    typedef UINT32 (* FPGA_MgmtOpsMutexRlock)(UINT32 ulSlotId, INT32 * plFd );
    typedef UINT32 (* FPGA_MgmtOpsMutexUnlock)(INT32 lFd );
public:
	xclFpgaMgmtProxy();
	~xclFpgaMgmtProxy();
public:

	FPGA_MgmtInit m_FPGA_MgmtInit;
    FPGA_MgmtInquireFpgaImageInfo m_FPGA_MgmtInquireFpgaImageInfo;
    FPGA_MgmtLoadHfiImage m_FPGA_MgmtLoadHfiImage;
    FPGA_MgmtOpsMutexRlock m_FPGA_MgmtOpsMutexRlock;
    FPGA_MgmtOpsMutexUnlock m_FPGA_MgmtOpsMutexUnlock;
	void *mHandle;
};


#endif
