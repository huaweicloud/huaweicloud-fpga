/**
 * Copyright (C) 2015-2016 Xilinx, Inc
 * Author: Sonal Santan
 * XDMA HAL Driver layered on top of XDMA kernel driver
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You may
 * not use this file except in compliance with the License. A copy of the
 * License is located at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

/**
 * To enable a new platform.
 *  1. Check the DSA Name is correct; that DSA Name matching works fine
 *  2. DDR Size is set correctly.
 *  3. MCAP/ICAP/PCAP programming mode is correctly chosen.
 *  4. Makefiles are update both for user and kernel mode.
 */

#include "shim.h"
#include "memorymanager.h"
#include "datamover.h"

// XDMA IOCTL File. Need to rename this file but ideally should be renamed in XDMA driver first.
#include "cdev_ctrl.h"
#include <errno.h>
/*
 * Define GCC version macro so we can use newer C++11 features
 * if possible
 */
#define GCC_VERSION (__GNUC__ * 10000 \
                     + __GNUC_MINOR__ * 100 \
                     + __GNUC_PATCHLEVEL__)

#include <sys/types.h>

#ifndef _WINDOWS
// TODO: Windows build support
//    sys/mman.h is linux only header file
//    it is included for mmap
#include <sys/mman.h>
#endif

#ifndef _WINDOWS
// TODO: Windows build support
//    unistd.h is linux only header file
//    it is included for read, write, close, lseek64
#include <unistd.h>
#endif

#include <sys/stat.h>
#include <fcntl.h>

#ifndef _WINDOWS
// TODO: Windows build support
//    sys/ioctl.h is linux only header file
//    it is included for ioctl
#include <sys/ioctl.h>
#endif

#ifndef _WINDOWS
// TODO: Windows build support
//    sys/file.h is linux only header file
//    it is included for flock
#include <sys/file.h>
#endif


#include <cstdio>
#include <cstring>
#include <cassert>
#include <algorithm>
#include <stdlib.h>
#include <thread>
#include <chrono>
#include <iostream>
#include <mutex>
#include <map>

#include "xclbin.h"
#include "cdev_ctrl.h"
#include "FPGA_Common.h"
#include "xclFpgaMgmtproxy.h"

/**
 * DDR Zero IP Register definition
 */

#define DDR_ZERO_BASE	           	0x0B0000
#define DDR_ZERO_CONFIG_REG_OFFSET 	0x10
#define DDR_ZERO_CTRL_REG_OFFSET	0x0

#ifdef _MGMT_
#define XCLMGMT_IOC_MAGIC	'X'
#define XCLMGMT_IOC_OCL_RESET   0x3
#define XCLMGMT_IOC_HOT_RESET   0x4
#define XCLMGMT_IOCOCLRESET       _IO  (XCLMGMT_IOC_MAGIC,XCLMGMT_IOC_OCL_RESET)
#define XCLMGMT_IOCHOTRESET       _IO  (XCLMGMT_IOC_MAGIC,XCLMGMT_IOC_HOT_RESET)
#endif

#ifdef _WINDOWS
#define __func__ __FUNCTION__
#endif

#ifndef _MGMT_
#define KB(x)   ((unsigned) (x) << 10)
#define MB(x)   ((unsigned) (x) << 20)

#define OCL_CU_CTRL_RANGE      	KB(4)
#define DDR_BUFFER_ALIGNMENT  0x40
#define MMAP_SIZE_USER         	MB(4)
#define PERFMON0_BASE              0x100000
#define PERFMON0_BASE              0x100000
#define OCL_CTLR_BASE              0x000000
#endif


#ifdef _WINDOWS
#define MAP_FAILED (void *)-1
#endif

#define GB(x)   ((size_t) (x) << 30)

struct deviceInfo {
  std::string           name;
  unsigned short        numClocks;
  size_t                ddrSize;
};

// Takes in the device ID and subsystem ID and returns the device info
static  std::map<unsigned int, struct deviceInfo> deviceInfoMap = {
    std::pair<unsigned int, struct deviceInfo>(0x6D2F4340, {"xilinx:bittware-xupp3r-vu9p:4ddr-xpr:4.0", 2, GB(16)}  ),
    std::pair<unsigned int, struct deviceInfo>(0x4A274340, {"xilinx:xil-accel-rd-ku115:4ddr-xpr:4.0"  , 2, GB(4) }  ),
    std::pair<unsigned int, struct deviceInfo>(0x4B274340, {"xilinx:kcu1500:4ddr-xpr:4.0"             , 2, GB(4) }  ),
    std::pair<unsigned int, struct deviceInfo>(0xd5124341, {"xilinx:huawei-vu9p-fp1:4ddr-xpr:4.1" , 2, GB(16)}  ),
    std::pair<unsigned int, struct deviceInfo>(0x692F4340, {"xilinx:xil-accel-rd-vu9p-hp:4ddr-xpr:4.0", 2, GB(4) }  ),
    std::pair<unsigned int, struct deviceInfo>(0x6A2F4340, {"xilinx:vcu1525:4ddr-xpr:4.0"             , 2, GB(4) }  ),
    std::pair<unsigned int, struct deviceInfo>(0x49072340, {"xilinx:adm-pcie-8k5:2ddr:4.0"            , 1, GB(8) }  ),
    std::pair<unsigned int, struct deviceInfo>(0x48272340, {"xilinx:adm-pcie-ku3:2ddr-xpr:4.0"        , 1, GB(8) }  ),
    std::pair<unsigned int, struct deviceInfo>(0x48072340, {"xilinx:adm-pcie-ku3:2ddr:4.0"            , 1, GB(8) }  ),
    std::pair<unsigned int, struct deviceInfo>(0x28071340, {"xilinx:adm-pcie-7v3:1ddr:4.0"            , 1, GB(8) }  )
};

namespace xclxdma {
#define PRINTENDFUNC if (mLogStream.is_open()) mLogStream << __func__ << " ended " << std::endl;
    const unsigned XDMAShim::TAG = 0X586C0C6C; // XL OpenCL X->58(ASCII), L->6C(ASCII), O->0 C->C L->6C(ASCII);

    // return true if CU status is IDLE (0x4) else false
    bool XDMAShim::checkCUStatus()
    {
      bool ret = true;
      
      unsigned buf[1];
      for (unsigned i = 0; i < 4; i++) {
          xclRead( XCL_ADDR_KERNEL_CTRL, i * 4096, static_cast<void *>(buf), 4);
          
          if((buf[0] != 0x4) &&  (buf[0] != 0x0)) {
            return false;
          }
      }
      return ret;
    }

    int XDMAShim::xclLoadXclBin(const xclBin *buffer)
    {
 #ifndef _MGMT_
    
 #define AEI_ID_LEN_MAX       36
 #define AEI_ID_LEN           32
 #define AEI_RPLACE_OFFSET    416
 #define FPGAMGMT_TMOUT_COUNT 300
             
             FPGA_IMG_INFO prloadinfo;
             char xclbin_aeiid[AEI_ID_LEN_MAX];
             
             int nRet;
             int infoopsstatus;
             int tprocessout;
             static bool fpgamgmtinitlflag=false;
             tprocessout = 0;
             std::memset(xclbin_aeiid, 0, sizeof(xclbin_aeiid));
             
             char *xclbininmemory = reinterpret_cast<char*> (const_cast<xclBin*> (buffer));

             memcpy(xclbin_aeiid,xclbininmemory+AEI_RPLACE_OFFSET,AEI_ID_LEN);
             std::cout<<"Sending aei id is : "<<xclbin_aeiid<<std::endl;
             
             if (!fpgamgmtinitlflag)
             {
                nRet = fpgamgmt_obj.m_FPGA_MgmtInit();
                if(nRet)
    		    {
    		        std::cout<<"FPGA_MgmtInit : error: Fpga mailbox initial failed!"<<std::endl;
    		        std::cout<<"FPGA_MgmtInit : errno is "<<nRet<<std::endl;
    		        return -1;
    		    }
                 fpgamgmtinitlflag = true;
             }

             tprocessout = FPGAMGMT_TMOUT_COUNT;
             std::cout<<"start inquire info \n"<<std::endl;
             std::memset(&prloadinfo,0,sizeof(tagFPGA_IMG_INFO));
             std::cout<<"Loading device slot is : "<<slot_id<<std::endl;
		do
		{
			nRet = fpgamgmt_obj.m_FPGA_MgmtInquireFpgaImageInfo( slot_id, &prloadinfo);
                   
			if (nRet)
			{
				std::cout<<"Ops_status_processing : error, "<<__LINE__<<" : Inquire Fpga image info failed!"<<std::endl;
				std::cout<<"Ops_status_processing : error,  "<<__LINE__<<" : Inquire Fpga image info count is :"<<tprocessout<<std::endl;
				std::cout<<"Ops_status_processing : CmdOpsStatus is :"<< prloadinfo.ulCmdOpsStatus <<std::endl;
                         	return -1;
			}
			
			infoopsstatus = (prloadinfo.ulCmdOpsStatus & 0xffff0000) >> 16;
		
			if (!tprocessout)
			{
				std::cout<<"Ops_status_processing : error, "<<__LINE__<<" : Inquire Fpga image info proccess timeout!"<<std::endl;
				std::cout<<"Ops_status_processing : error , "<<__LINE__<<" : CmdOpsStatus is :"<<prloadinfo.ulCmdOpsStatus<<std::endl;		
				return -1;
			}

                   if ((infoopsstatus == FPGA_OPS_STATUS_PROCESSING) || (prloadinfo.ulFpgaPrStatus == FPGA_PR_STATUS_PROGRAMMING))
                   {
                       sleep(1);
                   }
			
			tprocessout--;
			
		}while((infoopsstatus == FPGA_OPS_STATUS_PROCESSING) || (prloadinfo.ulFpgaPrStatus == FPGA_PR_STATUS_PROGRAMMING));
		
		std::cout<<"Operation status is : "<<infoopsstatus<<std::endl;

        if (prloadinfo.ulFpgaPrStatus == FPGA_PR_STATUS_EXCEPTION)
        {
            std::cout<<"PR status is FPGA_PR_STATUS_EXCEPTION !"<<std::endl;
            return -1;
        }
        
		nRet = fpgamgmt_obj.m_FPGA_MgmtLoadHfiImage(slot_id, xclbin_aeiid );
		if (nRet)
		{
			std::cout<<"FPGA_MgmtLoadHfiImage : error, "<<__LINE__<<" : Loading pr image failed!"<<std::endl;
			return -1;
		}


             tprocessout = FPGAMGMT_TMOUT_COUNT;
		do
		{
			nRet = fpgamgmt_obj.m_FPGA_MgmtInquireFpgaImageInfo( slot_id, &prloadinfo);
			if (nRet)
			{
				std::cout<<"Ops_status_processing : error, "<<__LINE__<<" : Inquire Fpga image info failed!"<<std::endl;
				std::cout<<"Ops_status_processing : error, "<<__LINE__<<" : Inquire Fpga image info count is :"<<tprocessout<<std::endl;
				std::cout<<"Ops_status_processing : CmdOpsStatus is :"<< prloadinfo.ulCmdOpsStatus <<std::endl;
                          return -1;
			}
			
			infoopsstatus = (prloadinfo.ulCmdOpsStatus & 0xffff0000) >> 16;
		
			if (!tprocessout)
			{
				std::cout<<"Ops_status_processing : error,  "<<__LINE__<<" : Inquire Fpga image info proccess timeout!"<<std::endl;
				std::cout<<"Ops_status_processing : error ,  "<<__LINE__<<" : CmdOpsStatus is :"<<prloadinfo.ulCmdOpsStatus<<std::endl;		
				return -1;
			}
			
			if ((infoopsstatus == FPGA_OPS_STATUS_PROCESSING) || (prloadinfo.ulFpgaPrStatus == FPGA_PR_STATUS_PROGRAMMING))
                   {
                       sleep(1);
                   }
			tprocessout--;
			
		}while((infoopsstatus == FPGA_OPS_STATUS_PROCESSING) || (prloadinfo.ulFpgaPrStatus == FPGA_PR_STATUS_PROGRAMMING));
		
		if ((!strncmp(xclbin_aeiid,prloadinfo.acHfid,AEI_ID_LEN)) && (infoopsstatus == FPGA_OPS_STATUS_SUCCESS) && (prloadinfo.ulFpgaPrStatus == FPGA_PR_STATUS_PROGRAMMED))
		{
			std::cout<<"Loading pr image completed!"<<std::endl;

                   if (fpgamgmt_obj.m_FPGA_MgmtOpsMutexRlock( slot_id, &prlock))
                   {
                           std::cout << "ERROR: get pr_lock statue failed!\n";
                           std::cout << "pr_lock value is :"<<prlock<<std::endl; 
                           return -1;
                   }
			return 0;
		}
		else
		{
			std::cout<<"Loading pr image status error!"<<std::endl;
			std::cout<<"Ops_status_processing : error , "<<__LINE__<<" : CmdOpsStatus is :"<<prloadinfo.ulCmdOpsStatus<<std::endl;
			std::cout<<"PR status processing : error ,  "<<__LINE__<<" : FpgaPr status is :"<<prloadinfo.ulFpgaPrStatus<<std::endl;
                   std::cout<<"PR status processing : error ,  "<<__LINE__<<" : FpgaPr aeiid is :"<<prloadinfo.acHfid<<std::endl;
			return -1;
		}             

 #else
        std::cout<<"ERROR: xclLoadXclBin is not supported in this version!\n";
        std::cout<<"ERROR: There is no mgmt device channel!\n";
        char *xclbininmemory = reinterpret_cast<char*> (const_cast<xclBin*> (buffer));

        if (!memcmp(xclbininmemory, "xclbin2", 8)){
            return xclLoadAxlf(reinterpret_cast<axlf*>(xclbininmemory));
        }

        if (mLogStream.is_open()) {
            mLogStream << __func__ << ", " << std::this_thread::get_id() << ", " << buffer << std::endl;
        }

        if (!mLocked)
            return -EPERM;

#ifndef _WINDOWS

        const unsigned cmd = XCLMGMT_IOCICAPDOWNLOAD;
        xclmgmt_ioc_bitstream obj = {const_cast<xclBin *>(buffer)};
        int ret = ioctl(mMgtHandle, cmd, &obj);
        if(0 != ret)
          return ret;

        // If it is an XPR DSA, zero out the DDR again as downloading the XCLBIN
        // reinitializes the DDR and results in ECC error.
        if(isXPR()) {
          if (mLogStream.is_open()) {
              mLogStream << __func__ << "XPR Device found, zeroing out DDR again.." << std::endl;
          }

          if (zeroOutDDR() == false){
            if (mLogStream.is_open()) {
                mLogStream <<  __func__ << "zeroing out DDR failed" << std::endl;
            }
            return -EIO;
          }
        }

        return ret;

#endif

#endif
    }

   int XDMAShim::xclLoadAxlf(const axlf *buffer)
   {
 #ifndef _MGMT_
   	return 0;
 #else
       if (mLogStream.is_open()) {
         mLogStream << __func__ << ", " << std::this_thread::get_id() << ", " << buffer << std::endl;
       }

       if (!mLocked)
           return -EPERM;

       const unsigned cmd = XCLMGMT_IOCICAPDOWNLOAD_AXLF;
       xclmgmt_ioc_bitstream_axlf obj = {const_cast<axlf *>(buffer)};
       int ret = ioctl(mMgtHandle, cmd, &obj);
       if(0 != ret)
         return ret;

       // If it is an XPR DSA, zero out the DDR again as downloading the XCLBIN
       // reinitializes the DDR and results in ECC error.
       if(isXPR()) {
         if (mLogStream.is_open()) {
             mLogStream << __func__ << "XPR Device found, zeroing out DDR again.." << std::endl;
         }

         if (zeroOutDDR() == false){
           if (mLogStream.is_open()) {
               mLogStream <<  __func__ << "zeroing out DDR failed" << std::endl;
           }
           return -EIO;
         }
       }

       return ret;
#endif	   
   }

    size_t XDMAShim::xclWrite(xclAddressSpace space, uint64_t offset, const void *hostBuf, size_t size) {
        if (mLogStream.is_open()) {
            mLogStream << __func__ << ", " << std::this_thread::get_id() << ", " << space << ", "
                       << offset << ", " << hostBuf << ", " << size << std::endl;
        }

        if (!mLocked)
            return -1;

        switch (space) {
        case XCL_ADDR_SPACE_DEVICE_RAM:
        {
            const size_t totalSize = size;
            const char *curr = static_cast<const char *>(hostBuf);
            while (size > maxDMASize) {
#ifndef _WINDOWS
// TODO: Windows build support
              if (mDataMover->pwrite64(curr,maxDMASize,offset) < 0)
                return -1;
#endif
                offset += maxDMASize;
                curr += maxDMASize;
                size -= maxDMASize;
            }
#ifndef _WINDOWS
// TODO: Windows build support
            if (mDataMover->pwrite64(curr,size,offset) < 0)
              return -1;
#endif
            return totalSize;
        }
        case XCL_ADDR_SPACE_DEVICE_PERFMON:
        {
            offset += mOffsets[XCL_ADDR_SPACE_DEVICE_PERFMON];
            if (pcieBarWrite(SHIM_USER_BAR, offset, hostBuf, size) == 0) {
                return size;
            }
            return -1;
        }
        case XCL_ADDR_SPACE_DEVICE_CHECKER:
        {
            PRINTENDFUNC;
            return -1;
        }
        case XCL_ADDR_KERNEL_CTRL:
        {
            offset += mOffsets[XCL_ADDR_KERNEL_CTRL];
            if (mLogStream.is_open()) {
                const unsigned *reg = static_cast<const unsigned *>(hostBuf);
                size_t regSize = size / 4;
                if (regSize > 32)
                    regSize = 32;
                for (unsigned i = 0; i < regSize; i++) {
                    mLogStream << __func__ << ", " << std::this_thread::get_id() << ", " << space << ", 0x"
                               << std::hex << offset + i << std::dec << ", 0x" << std::hex << reg[i] << std::dec << std::endl;

                }
            }
            if (pcieBarWrite(SHIM_USER_BAR, offset, hostBuf, size) == 0) {
                return size;
            }
            return -1;
        }
        default:
        {
            return -1;
        }
        }
    }

    size_t XDMAShim::xclRead(xclAddressSpace space, uint64_t offset, void *hostBuf, size_t size) {
        if (mLogStream.is_open()) {
            mLogStream << __func__ << ", " << std::this_thread::get_id() << ", " << space << ", "
                       << offset << ", " << hostBuf << ", " << size << std::endl;
        }

        switch (space) {
        case XCL_ADDR_SPACE_DEVICE_RAM:
        {
            const size_t totalSize = size;
            char *curr = static_cast<char*>(hostBuf);
            while (size > maxDMASize) {
#ifndef _WINDOWS
// TODO: Windows build support
              if (mDataMover->pread64(curr,maxDMASize,offset) < 0)
                return -1;
#endif
                offset += maxDMASize;
                curr += maxDMASize;
                size -= maxDMASize;
            }

#ifndef _WINDOWS
// TODO: Windows build support
            if (mDataMover->pread64(curr,size,offset) < 0)
              return -1;
#endif
            return totalSize;
        }
        case XCL_ADDR_SPACE_DEVICE_PERFMON:
        {
            offset += mOffsets[XCL_ADDR_SPACE_DEVICE_PERFMON];
            if (pcieBarRead(SHIM_USER_BAR, offset, hostBuf, size) == 0) {
                return size;
            }
            return -1;
        }
        case XCL_ADDR_KERNEL_CTRL:
        {
            offset += mOffsets[XCL_ADDR_KERNEL_CTRL];
            int result = pcieBarRead(SHIM_USER_BAR, offset, hostBuf, size);
            if (mLogStream.is_open()) {
                const unsigned *reg = static_cast<const unsigned *>(hostBuf);
                size_t regSize = size / 4;
                if (regSize > 4)
                    regSize = 4;
                for (unsigned i = 0; i < regSize; i++) {
                    mLogStream << __func__ << ", " << std::this_thread::get_id() << ", " << space << ", 0x"
                               << std::hex << offset + i << std::dec << ", 0x" << std::hex << reg[i] << std::dec << std::endl;
                }
            }
            return !result ? size : 0;
        }
        case XCL_ADDR_SPACE_DEVICE_CHECKER:
        {
            if (pcieBarRead(SHIM_MGMT_BAR, offset, hostBuf, size) == 0) {
                return size;
            }
            return -1;
        }
        default:
        {
            return -1;
        }
        }
    }

    uint64_t XDMAShim::xclAllocDeviceBuffer(size_t size) {
        if (mLogStream.is_open()) {
            mLogStream << __func__ << ", " << std::this_thread::get_id() << ", " << size << std::endl;
        }

        if (size == 0)
            size = DDR_BUFFER_ALIGNMENT;

        uint64_t result = MemoryManager::mNull;
        for (auto i : mDDRMemoryManager) {
            result = i->alloc(size);
            if (result != MemoryManager::mNull)
                break;
        }
        return result;
    }

    uint64_t XDMAShim::xclAllocDeviceBuffer2(size_t size, xclMemoryDomains domain, unsigned flags)
    {
        if (mLogStream.is_open()) {
            mLogStream << __func__ << ", " << std::this_thread::get_id() << ", " << size << ", "
                       << domain << ", " << flags << std::endl;
        }

        if (domain != XCL_MEM_DEVICE_RAM)
            return MemoryManager::mNull;

        if (size == 0)
            size = DDR_BUFFER_ALIGNMENT;

        if (flags >= mDDRMemoryManager.size()) {
            return MemoryManager::mNull;
        }
        return mDDRMemoryManager[flags]->alloc(size);
    }

    void XDMAShim::xclFreeDeviceBuffer(uint64_t buf) {
        if (mLogStream.is_open()) {
            mLogStream << __func__ << ", " << std::this_thread::get_id() << ", " << buf << std::endl;
        }

        uint64_t size = 0;
        for (auto i : mDDRMemoryManager) {
            size += i->size();
            if (buf < size) {
                i->free(buf);
            }
        }
    }


    size_t XDMAShim::xclCopyBufferHost2Device(uint64_t dest, const void *src, size_t size, size_t seek) {
        if (mLogStream.is_open()) {
            mLogStream << __func__ << ", " << std::this_thread::get_id() << ", " << dest << ", "
                       << src << ", " << size << ", " << seek << std::endl;
        }
#if 0
        {
            // Ensure that this buffer was allocated by memory manager before
            const uint64_t v = MemoryManager::mNull;
            std::pair<uint64_t, uint64_t> buf = std::make_pair(v, v);
            uint64_t high = 0;
            for (auto i : mDDRMemoryManager) {
                high += i->size();
                if (dest < high) {
                    buf = i->lookup(dest);
                    break;
                }
            }
            if (MemoryManager::isNullAlloc(buf))
                return -1;

            if (buf.second < (size + seek))
                return -1;
        }
#endif
        dest += seek;
        return xclWrite(XCL_ADDR_SPACE_DEVICE_RAM, dest, src, size);
    }


    size_t XDMAShim::xclCopyBufferDevice2Host(void *dest, uint64_t src, size_t size, size_t skip) {
        if (mLogStream.is_open()) {
            mLogStream << __func__ << ", " << std::this_thread::get_id() << ", " << dest << ", "
                       << src << ", " << size << ", " << skip << std::endl;
        }

#if 0
        {
            // Ensure that this buffer was allocated by memory manager before
            const uint64_t v = MemoryManager::mNull;
            std::pair<uint64_t, uint64_t> buf = std::make_pair(v, v);
            uint64_t high = 0;
            for (auto i : mDDRMemoryManager) {
                high += i->size();
                if (src < high) {
                    buf = i->lookup(src);
                    break;
                }
            }
            if (MemoryManager::isNullAlloc(buf))
                return -1;

            if (buf.second < (size + skip))
                return -1;
        }
#endif
        src += skip;
        return xclRead(XCL_ADDR_SPACE_DEVICE_RAM, src, dest, size);
    }


    XDMAShim *XDMAShim::handleCheck(void *handle) {
        // Sanity checks
        if (!handle)
            return 0;
        if (*(unsigned *)handle != TAG)
            return 0;
        if (!((XDMAShim *)handle)->isGood()) {
            return 0;
        }

        return (XDMAShim *)handle;
    }

    unsigned XDMAShim::xclProbe() {
        char file_name_buf[128];
		std::memset(file_name_buf, 0, 128);
        unsigned i  = 0;
        for (i = 0; i < 64; i++) {
            std::sprintf((char *)&file_name_buf, "/dev/xdma%d_user", i);
#ifndef _WINDOWS
            int fd = open(file_name_buf, O_RDWR);
            if (fd < 0) {
                return i;
            }
            close(fd);
#endif
        }
        return i;
    }

    void XDMAShim::initMemoryManager()
    {
        if (!mDeviceInfo.mDDRBankCount)
            return;
        const uint64_t bankSize = mDeviceInfo.mDDRSize / mDeviceInfo.mDDRBankCount;
        uint64_t start = 0;
        for (unsigned i = 0; i < mDeviceInfo.mDDRBankCount; i++) {
	    //Made the alignment 4k : CR 966701. earlier it was DDR_BUFFER_ALIGNMENT
            mDDRMemoryManager.push_back(new MemoryManager(bankSize, start, 4096));
            start += bankSize;
        }
    }

    XDMAShim::~XDMAShim()
    {
        if (mUserMap != MAP_FAILED) {
            int ret = munmap(mUserMap, MMAP_SIZE_USER);
            if(-1 == ret){
                std::cout<<"ERROR:the user map munmap failed...\n";
            }
        }
#ifdef _MGMT_		
        if (mMgtMap != MAP_FAILED) {
            int ret = munmap(mMgtMap, MMAP_SIZE_USER);
            if(-1 == ret){
                std::cout<<"ERROR:the user map munmap failed...\n";
            }
        }

        if (mMgtHandle > 0) {
            close(mMgtHandle);
        }
		
#endif		
        if (mUserHandle > 0) {
            close(mUserHandle);
        }

        delete mDataMover;
		mDataMover = NULL;
        for (auto i : mDDRMemoryManager) {
            delete i;
        }
		mDDRMemoryManager.clear();
        if (mLogStream.is_open()) {
            mLogStream << __func__ << ", " << std::this_thread::get_id() << std::endl;
            mLogStream.close();
        }

        if (prlock)
        {
            if (fpgamgmt_obj.m_FPGA_MgmtOpsMutexUnlock(prlock))
            {
                std::cout << "ERROR: Unlock pr_lock statue failed!\n";
            }
        }
    }

    XDMAShim::XDMAShim(unsigned index, const char *logfileName,
                       xclVerbosityLevel verbosity) : mTag(TAG), mBoardNumber(index),
                                                      maxDMASize(0xfa0000),
                                                      mLocked(false),
                                                      mOffsets{0x0, 0x0, OCL_CTLR_BASE, PERFMON0_BASE},
                                                      mOclRegionProfilingNumberSlots(XPAR_AXI_PERF_MON_2_NUMBER_SLOTS),
                                                      mMgtMap(0)
    {
        slot_id = index;
        //
        mDataMover = new DataMover(mBoardNumber, 2 /* 1 channel each dir */);
        char file_name_buf[128];
		std::memset(file_name_buf, 0, 128);
        std::sprintf((char *)&file_name_buf, "/dev/xdma%d_user", mBoardNumber);
        mUserHandle = open(file_name_buf, O_RDWR | O_SYNC);
        if(mUserHandle < 0)
            std::cout << "Could not open " << file_name_buf << std::endl;
        mUserMap = (char *)mmap(0, MMAP_SIZE_USER, PROT_READ | PROT_WRITE, MAP_SHARED, mUserHandle, 0);
        if (mUserMap == MAP_FAILED) {
			if(mUserHandle > 0)
				close(mUserHandle);
            mUserHandle = -1;
        }
        //logfileName = "u.log";
        if (logfileName && (logfileName[0] != '\0')) {
            mLogStream.open(logfileName);
            mLogStream << "FUNCTION, THREAD ID, ARG..."  << std::endl;
            mLogStream << __func__ << ", " << std::this_thread::get_id() << std::endl;
        }

#ifdef _MGMT_
        std::fill(&file_name_buf[0], &file_name_buf[0] + 128, 0);
        std::sprintf((char *)&file_name_buf, "/dev/xclmgmt%d", mBoardNumber);
        mMgtHandle = open(file_name_buf, O_RDWR | O_SYNC);
        if(mMgtHandle < 0)
            std::cout << "Could not open " << file_name_buf << std::endl;

        mMgtMap = (char *)mmap(0, MMAP_SIZE_USER, PROT_READ | PROT_WRITE, MAP_SHARED, mMgtHandle, 0);
        if (mMgtMap == MAP_FAILED) // Not an error if user is not privileged
            mMgtMap = nullptr;
        if (xclGetDeviceInfo2(&mDeviceInfo)) {
			if( mMgtHandle > 0 )
				close(mMgtHandle);
          mMgtHandle = -1;
        }
#else
        if (xclGetDeviceInfo2(&mDeviceInfo)) {
			if(mUserHandle > 0)
				close(mUserHandle);
          mUserHandle = -1;
        }
#endif
        initMemoryManager();
        prlock=0;
    }

    bool XDMAShim::isGood() const {
        if (!mDataMover)
            return false;
        if (mUserHandle < 0)
            return false;
#ifdef _MGMT_
        std::cout<<"mMgtHandle is not supported in this version for check!\n";
        if (mMgtHandle < 0)
            return false;
#endif
        return mDataMover->isGood();
        // TODO: Add sanity check for card state
    }


    int XDMAShim::pcieBarRead(unsigned int pf_bar, unsigned long long offset, void* buffer,
                              unsigned long long length) {
        const char *mem = 0;
        switch (pf_bar) {
        case 0:
        {
            // BAR0 on PF0
            if ((length + offset) > MMAP_SIZE_USER) {
                return -1;
            }
            mem = mUserMap;
            break;
        }
        case 0x10000:
        {
#ifdef _MGMT_			
            // BAR0 on PF1
            mem = mMgtMap;
            break;
#else
            std::cout<<"ERROR: Read mgmt bar space is not supported in this version!\n";
            return -1;
#endif
        }
        default:
        {
            return -1;
        }
        }

        char *qBuf = (char *)buffer;
        while (length >= 4) {
            *(unsigned *)qBuf = *(unsigned *)(mem + offset);
            offset += 4;
            qBuf += 4;
            length -= 4;
        }
        while (length) {
            *qBuf = *(mem + offset);
            offset++;
            qBuf++;
            length--;
        }

        return 0;
    }

    int XDMAShim::pcieBarWrite(unsigned int pf_bar, unsigned long long offset, const void* buffer,
                               unsigned long long length) {
        char *mem = 0;
        switch (pf_bar) {
        case 0:
        {
            // BAR0 on PF0
            if ((length + offset) > MMAP_SIZE_USER) {
                return -1;
            }
            mem = mUserMap;
            break;
        }
        case 0x10000:
        {
#ifdef _MGMT_			
            // BAR0 on PF1
            mem = mMgtMap;
	    break;
#else
            std::cout<<"ERROR:Write mgmt bar space is not supported in this version!\n";
		return -1;
#endif
        }
        default:
        {
            return -1;
        }
        }

        char *qBuf = (char *)buffer;
        while (length >= 4) {
            *(unsigned *)(mem + offset) = *(unsigned *)qBuf;
            offset += 4;
            qBuf += 4;
            length -= 4;
        }
        while (length) {
            *(mem + offset) = *qBuf;
            offset++;
            qBuf++;
            length--;
        }

        return 0;
    }

    bool XDMAShim::zeroOutDDR()
    {
#ifdef ENABLE_DDR_ZERO_IP
      short num_ddr = mDeviceInfo.mDDRBankCount;
      short individual_ddr_size = mDeviceInfo.mDDRSize/mDeviceInfo.mDDRBankCount;
      unsigned long val = (0x0000 << 16 ) | ((individual_ddr_size & 0xff) << 8) | (num_ddr & 0xff);
      std::cout << "Zero out val : 0x" << hex <<
      pcieBarWrite(SHIM_USER_BAR, DDR_ZERO_BASE + DDR_ZERO_CONFIG_REG_OFFSET, &val, 4);
#else
#if 0
      // Zero out the DDR so MIG ECC believes we have touched all the bits
      // and it does not complain when we try to read back without explicit
      // write. The latter usually happens as a result of read-modify-write
      // TODO: Try and speed this up.
      // [1] Possibly move to kernel mode driver.
      // [2] Zero out specific buffers when they are allocated
      static const unsigned long long BLOCK_SIZE = 0x4000000;
      void *buf = 0;
      if (posix_memalign(&buf, DDR_BUFFER_ALIGNMENT, BLOCK_SIZE))
          return false;
      memset(buf, 0, BLOCK_SIZE);
      mDataMover->pset64(buf, BLOCK_SIZE, 0, mDeviceInfo.mDDRSize/BLOCK_SIZE);
      free(buf);
#endif
      return true;
#endif
    }

    bool XDMAShim::xclLockDevice()
    {
        if (mDataMover->lock() == false)
            return false;

        if (flock(mUserHandle, LOCK_EX | LOCK_NB) == -1) {
            mDataMover->unlock();
            return false;
        }
        mLocked = true;

        return zeroOutDDR();
    }

    int XDMAShim::xclGetAXIErrorStatus(xclAXIErrorStatus *info)
    {
#ifndef _MGMT_
	return 0;
#else
	
#ifdef AXI_FIREWALL
        char buf[80];
        struct tm *ts;
        time_t temp;
        int ret;

        std::memset(info, 0, sizeof(xclAXIErrorStatus));
        xclmgmt_ioc_firewall_info firewall_obj;
        std::memset(&firewall_obj, 0, sizeof(xclmgmt_ioc_firewall_info));
        ret = ioctl(mMgtHandle, XCLMGMT_IOCFIREWALL, &firewall_obj);
        if (ret)
            return ret;
        for(int i = 0; i < XCLMGMT_NUM_FIREWALL_IPS; ++i) {
          info->mErrFirewallTime[i] = firewall_obj.err_firewall_time[i];
          info->mErrFirewallStatus[i] = firewall_obj.err_firewall_status[i];
        }
#endif  // AXI Firewall
#endif
        return 0;
    }

    int XDMAShim::xclGetDeviceInfo2(xclDeviceInfo2 *info)
    {
   
        std::memset(info, 0, sizeof(xclDeviceInfo2));
	 
#ifndef _WINDOWS
// TODO: Windows build support
//    XDMA_IOCINFO depends on _IOW, which is defined indirectly by <linux/ioctl.h>
//    ioctl is defined in sys/ioctl.h

#ifndef _MGMT_
        xdma_ioc_info obj1;
        std::memset(&obj1, 0, sizeof(xdma_ioc_info));
        int ret = ioctl(mUserHandle, XDMA_IOCINFO, &obj1);
        if (ret)
            return ret;		
	
	 xdma_ioc_info_ex obj2;
	 std::memset(&obj2, 0, sizeof(xdma_ioc_info_ex));
	 ret = ioctl(mUserHandle, XDMA_IOCINFO_EX, &obj2);
	 if (ret)
            return ret;
#else
       xclmgmt_ioc_info obj;
       std::memset(&obj, 0, sizeof(xclmgmt_ioc_info));
       ret = ioctl(mMgtHandle, XCLMGMT_IOCINFO, &obj);
       if (ret)
            return ret;
#endif

#endif
	
        info->mVendorId = obj1.vendor;
        info->mDeviceId = obj1.device;
        info->mSubsystemId = obj1.subsystem_device;
        info->mSubsystemVendorId = obj1.subsystem_vendor;
        info->mDeviceVersion = obj1.subsystem_device & 0x00ff;
		info->mDataAlignment = DDR_BUFFER_ALIGNMENT;
		 
        info->mNumClocks = deviceInfoMap[(info->mDeviceId << 16) | info->mSubsystemId].numClocks;
        for (int i = 0; i < info->mNumClocks; ++i) {
          info->mOCLFrequency[i] = obj2.ocl_frequency[i];
        }

        info->mDDRSize = GB(obj2.ddr_channel_size);
        info->mDDRBankCount = obj2.ddr_channel_num;
        info->mDDRSize *= info->mDDRBankCount;

        for (auto i : mDDRMemoryManager) {
            info->mDDRFreeSize += i->freeSize();
        }
		
		memcpy(info->mName, obj2.vbnv, 64);
        info->mOnChipTemp  = obj2.onchip_temp;
        info->mFanTemp     = obj2.fan_temp;
        info->mVInt        = obj2.vcc_int;
        info->mVAux        = obj2.vcc_aux;
        info->mVBram       = obj2.vcc_bram;
        info->mMigCalib    = obj2.mig_calibration[0];
        info->mPCIeLinkWidth = obj2.pcie_link_width;
        info->mPCIeLinkSpeed = obj2.pcie_link_speed;
        return 0;
    }


    int XDMAShim::resetDevice(xclResetKind kind) {
        long ret = -1;
#ifndef _WINDOWS
// TODO: Windows build support
//    XDMA_IOCRESET depends on _IOW, which is defined indirectly by <linux/ioctl.h>
//    ioctl is defined in sys/ioctl.h
        for (auto i : mDDRMemoryManager) {
            i->reset();
        }

        // Call a new IOCTL to just reset the OCL region
        if (kind == XCL_RESET_FULL) {
            ret = ioctl(mUserHandle, XDMA_IOCOFFLINE);
            if(ret != 0)
              return ret;
#ifdef _MGMT_
            std::cout<<"XCLMGMT_IOCHOTRESET is not supported in this version!\n";
            ret =  ioctl(mMgtHandle, XCLMGMT_IOCHOTRESET);
            if(ret != 0)
              return ret;
#endif			
            ret = ioctl(mUserHandle, XDMA_IOCONLINE);
            return ret;
        }
        else if (kind == XCL_RESET_KERNEL) {
#ifdef _MGMT_	
            std::cout<<"XCLMGMT_IOCOCLRESET is not supported in this version!\n";
            return ioctl(mMgtHandle, XCLMGMT_IOCOCLRESET);
#else
            std::cout<<"Kernel reset is not supported in this version!\n";
	     return -1;
#endif
        }
        return -EINVAL;
#else
        return 0;
#endif
    }

    int XDMAShim::xclReClock2(unsigned short region, const unsigned short *targetFreqMHz)
    {

#ifndef _MGMT_	
	return 0;
#else
        std::cout<<"xclReClock2 is not supported in this version!\n";
        xclmgmt_ioc_freqscaling obj;
        std::memset(&obj, 0, sizeof(xclmgmt_ioc_freqscaling));
        obj.ocl_region = region;
        obj.ocl_target_freq[0] = targetFreqMHz[0];
        obj.ocl_target_freq[1] = targetFreqMHz[1];		
        return ioctl(mMgtHandle, XCLMGMT_IOCFREQSCALE, &obj);
#endif
    }
}


xclDeviceHandle xclOpen(unsigned index, const char *logfileName, xclVerbosityLevel level)
{
    xclxdma::XDMAShim *handle = new xclxdma::XDMAShim(index, logfileName, level);
    if (!xclxdma::XDMAShim::handleCheck(handle)) {
        delete handle;
        handle = 0;
    }

    return (xclDeviceHandle *)handle;
}

void xclClose(xclDeviceHandle handle)
{
    if (xclxdma::XDMAShim::handleCheck(handle)) {
        delete ((xclxdma::XDMAShim *)handle);
    }
}

int xclGetAXIErrorStatus(xclDeviceHandle handle, xclAXIErrorStatus *info)
{
#ifdef _MGMT_  
    std::cout<<"ERROR: xclGetAXIErrorStatus is not supported in this version!\n";
    xclxdma::XDMAShim *drv = xclxdma::XDMAShim::handleCheck(handle);
    if (!drv)
    return -1;
    return drv->xclGetAXIErrorStatus(info);
#else
    return -1;
#endif
        
}

int xclGetDeviceInfo2(xclDeviceHandle handle, xclDeviceInfo2 *info)
{
    xclxdma::XDMAShim *drv = xclxdma::XDMAShim::handleCheck(handle);
    if (!drv)
        return -1;
    return drv->xclGetDeviceInfo2(info);
}

int xclLoadXclBin(xclDeviceHandle handle, const xclBin *buffer)
{
    xclxdma::XDMAShim *drv = xclxdma::XDMAShim::handleCheck(handle);
    if (!drv)
        return -1;
    return drv->xclLoadXclBin(buffer);
}

size_t xclWrite(xclDeviceHandle handle, xclAddressSpace space, uint64_t offset, const void *hostBuf, size_t size)
{
    xclxdma::XDMAShim *drv = xclxdma::XDMAShim::handleCheck(handle);
    if (!drv)
        return -1;
    return drv->xclWrite(space, offset, hostBuf, size);
}

size_t xclRead(xclDeviceHandle handle, xclAddressSpace space, uint64_t offset, void *hostBuf, size_t size)
{
    xclxdma::XDMAShim *drv = xclxdma::XDMAShim::handleCheck(handle);
    if (!drv)
        return -1;
    return drv->xclRead(space, offset, hostBuf, size);
}


uint64_t xclAllocDeviceBuffer(xclDeviceHandle handle, size_t size)
{
    xclxdma::XDMAShim *drv = xclxdma::XDMAShim::handleCheck(handle);
    if (!drv)
        return -1;
    return drv->xclAllocDeviceBuffer(size);
}


uint64_t xclAllocDeviceBuffer2(xclDeviceHandle handle, size_t size, xclMemoryDomains domain,
                               unsigned flags)
{
    xclxdma::XDMAShim *drv = xclxdma::XDMAShim::handleCheck(handle);
    if (!drv)
        return -1;
    return drv->xclAllocDeviceBuffer2(size, domain, flags);
}


void xclFreeDeviceBuffer(xclDeviceHandle handle, uint64_t buf)
{
    xclxdma::XDMAShim *drv = xclxdma::XDMAShim::handleCheck(handle);
    if (!drv)
        return;
    return drv->xclFreeDeviceBuffer(buf);
}


size_t xclCopyBufferHost2Device(xclDeviceHandle handle, uint64_t dest, const void *src, size_t size, size_t seek)
{
    xclxdma::XDMAShim *drv = xclxdma::XDMAShim::handleCheck(handle);
    if (!drv)
        return -1;
    return drv->xclCopyBufferHost2Device(dest, src, size, seek);
}


size_t xclCopyBufferDevice2Host(xclDeviceHandle handle, void *dest, uint64_t src, size_t size, size_t skip)
{
    xclxdma::XDMAShim *drv = xclxdma::XDMAShim::handleCheck(handle);
    if (!drv)
        return -1;
    return drv->xclCopyBufferDevice2Host(dest, src, size, skip);
}


//This will be deprecated.
int xclUpgradeFirmware(xclDeviceHandle handle, const char *fileName)
{
#ifdef _MGMT_
    xclxdma::XDMAShim *drv = xclxdma::XDMAShim::handleCheck(handle);
    if (!drv)
        return -1;
    return drv->xclUpgradeFirmware(fileName);
#else
    std::cout<<"ERROR: xclUpgradeFirmware is not supported in this version!\n";
    return -1;
#endif
}

int xclUpgradeFirmware2(xclDeviceHandle handle, const char *fileName1, const char* fileName2)
{
#ifdef _MGMT_
    xclxdma::XDMAShim *drv = xclxdma::XDMAShim::handleCheck(handle);
    if (!drv)
        return -1;

    if(!fileName2 || std::strlen(fileName2) == 0)
      return drv->xclUpgradeFirmware(fileName1);
    else
      return drv->xclUpgradeFirmware2(fileName1, fileName2);
#else
    std::cout<<"ERROR: xclUpgradeFirmware2 is not supported in this version!\n";
    return -1;
#endif
}

int xclUpgradeFirmwareXSpi(xclDeviceHandle handle, const char *fileName, int index)
{
 #ifdef _MGMT_
    xclxdma::XDMAShim *drv = xclxdma::XDMAShim::handleCheck(handle);
    if (!drv)
        return -1;
    return drv->xclUpgradeFirmwareXSpi(fileName, index);
 #else
     std::cout<<"ERROR: xclUpgradeFirmwareXSpi is not supported in this version!\n";
     return -1;
 #endif
}

int xclTestXSpi(xclDeviceHandle handle, int index)
{
#ifdef _MGMT_
    xclxdma::XDMAShim *drv = xclxdma::XDMAShim::handleCheck(handle);
    if (!drv)
        return -1;
    return drv->xclTestXSpi(index);
#else
    std::cout<<"ERROR: xclTestXSpi is not supported in this version!\n";
    return -1;
#endif
}

int xclBootFPGA(xclDeviceHandle handle)
{
    xclxdma::XDMAShim *drv = xclxdma::XDMAShim::handleCheck(handle);
    if (!drv)
        return -1;
    return drv->xclBootFPGA();
}

unsigned xclProbe()
{
    return xclxdma::XDMAShim::xclProbe();
}


int xclResetDevice(xclDeviceHandle handle, xclResetKind kind)
{
    xclxdma::XDMAShim *drv = xclxdma::XDMAShim::handleCheck(handle);
    if (!drv)
        return -1;
#ifdef _MGMT_
    std::cout<<"ERROR: xclResetDevice is not supported in this version!\n";
    return drv->resetDevice(kind);
#else
    return -1;
#endif

}


int xclReClock2(xclDeviceHandle handle, unsigned short region, const unsigned short *targetFreqMHz)
{
#ifdef _MGMT_
      std::cout<<"ERROR: xclReClock2 is not supported in this version!\n";
	xclxdma::XDMAShim *drv = xclxdma::XDMAShim::handleCheck(handle);
	if (!drv)
	    return -1;
	return drv->xclReClock2(region, targetFreqMHz);
#else
      return 0;
#endif
}


int xclLockDevice(xclDeviceHandle handle)
{
    xclxdma::XDMAShim *drv = xclxdma::XDMAShim::handleCheck(handle);
    if (!drv)
        return -1;
    return drv->xclLockDevice() ? 0 : -1;
}
