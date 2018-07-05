/**
 * Copyright (C) 2016-2018 Xilinx, Inc
 * Author(s): Umang Parekh
 *          : Sonal Santan
 * PCIe HAL Driver layered on top of XOCL GEM kernel driver
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

#include "shim.h"
#include <errno.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <cstring>
#include <thread>
#include <chrono>
#include <unistd.h>
#include <sys/file.h>
#include <poll.h>

#include "driver/include/xclbin.h"
#include "scan.h"
#include "xbsak.h"

#ifdef NDEBUG
# undef NDEBUG
# include<cassert>
#endif

// Copy bytes word (32bit) by word.
//
// Neither memcpy, nor std::copy work as they become byte copying on
// some platforms
inline void* wordcopy(void *dst, const void* src, size_t bytes)
{
    // assert dest is 4 byte aligned
    assert((reinterpret_cast<intptr_t>(dst) % 4) == 0);

    using word = uint32_t;
    auto d = reinterpret_cast<word*>(dst);
    auto s = reinterpret_cast<const word*>(src);
    auto w = bytes/sizeof(word);

    for (size_t i=0; i<w; ++i)
    {
        d[i] = s[i];
    }

    return dst;
}

namespace xocl {
// This list will get populated in xclProbe
// 0 -> /dev/dri/renderD129
// 1 -> /dev/dri/renderD130
static std::mutex deviceListMutex;
//  static std::vector<std::pair<int, int>> deviceList;

#define GB(x)   ((size_t) (x) << 30)

static const std::map<std::string, std::string> deviceOld2NewNameMap = {
    std::pair<std::string, std::string>("huawei:vu9p_dynamic:fp1:5.0", "huawei_vu9p_dynamic_fp1_5.0"),
    std::pair<std::string, std::string>("xilinx:adm-pcie-7v3:1ddr:3.0", "xilinx_adm-pcie-7v3_1ddr_3_0"),
    std::pair<std::string, std::string>("xilinx:adm-pcie-8k5:2ddr:4.0", "xilinx_adm-pcie-8k5_2ddr_4_0"),
    std::pair<std::string, std::string>("xilinx:adm-pcie-ku3:2ddr-xpr:4.0", "xilinx_adm-pcie-ku3_2ddr-xpr_4_0"),
    std::pair<std::string, std::string>("xilinx:adm-pcie-ku3:2ddr:4.0", "xilinx_adm-pcie-ku3_2ddr_4_0"),
    std::pair<std::string, std::string>("xilinx:aws-vu9p-f1:4ddr-xpr-2pr:4.0", "xilinx_aws-vu9p-f1_4ddr-xpr-2pr_4_0"),
    std::pair<std::string, std::string>("xilinx:kcu1500:4ddr-xpr:4.0", "xilinx_kcu1500_4ddr-xpr_4_0"),
    std::pair<std::string, std::string>("xilinx:kcu1500:4ddr-xpr:4.3", "xilinx_kcu1500_4ddr-xpr_4_3"),
    std::pair<std::string, std::string>("xilinx:vcu1525:4ddr-xpr:4.2", "xilinx_vcu1525_4ddr-xpr_4_2"),
    std::pair<std::string, std::string>("xilinx:xil-accel-rd-ku115:4ddr-xpr:4.0", "xilinx_xil-accel-rd-ku115_4ddr-xpr_4_0"),
    std::pair<std::string, std::string>("xilinx:xil-accel-rd-vu9p-hp:4ddr-xpr:4.2", "xilinx_xil-accel-rd-vu9p-hp_4ddr-xpr_4_2"),
    std::pair<std::string, std::string>("xilinx:xil-accel-rd-vu9p:4ddr-xpr-xare:4.6", "xilinx_xil-accel-rd-vu9p_4ddr-xpr-xare_4_6"),
    std::pair<std::string, std::string>("xilinx:xil-accel-rd-vu9p:4ddr-xpr:4.0", "xilinx_xil-accel-rd-vu9p_4ddr-xpr_4_0"),
    std::pair<std::string, std::string>("xilinx:xil-accel-rd-vu9p:4ddr-xpr:4.2", "xilinx_xil-accel-rd-vu9p_4ddr-xpr_4_2"),
    std::pair<std::string, std::string>("xilinx:zc706:linux-uart:1.0", "xilinx_zc706_linux-uart_1_0"),
    std::pair<std::string, std::string>("xilinx:zcu102:1HP:1.1", "xilinx_zcu102_1HP_1_1"),
    std::pair<std::string, std::string>("xilinx:zcu102:4HP:1.2", "xilinx_zcu102_4HP_1_2")
};

const std::string newDeviceName(const std::string& name) {
    auto i = deviceOld2NewNameMap.find(name);
    return (i == deviceOld2NewNameMap.end()) ? name : i->second;
}

unsigned numClocks(const std::string& name) {
    return name.compare(0, 15, "xilinx_adm-pcie", 15) ? 2 : 1;
}

static std::string parseCUStatus(unsigned val) {
    char delim = '(';
    std::string status;
    if ( val & 0x1) {
        status += delim;
        status += "START";
        delim = '|';
    }
    if ( val & 0x2) {
        status += delim;
        status += "DONE";
        delim = '|';
    }
    if ( val & 0x4) {
        status += delim;
        status += "IDLE";
        delim = '|';
    }
    if ( val & 0x8) {
        status += delim;
        status += "READY";
        delim = '|';
    }
    if ( val & 0x10) {
        status += delim;
        status += "RESTART";
        delim = '|';
    }
    if ( status.size()) {
        status += ')';
    } else if ( val == 0x0) {
        status = "(--)";
    } else {
        status = "(UNKNOWN)";
    }

    return status;
}

std::ostream& operator<< (std::ostream &strm, const AddresRange &rng)
{
    strm << "[" << rng.first << ", " << rng.second << "]";
    return strm;
}

XOCLShim::XOCLShim(unsigned index, const char *logfileName,
                   xclVerbosityLevel verbosity) : mVerbosity(verbosity),
                                                  mBoardNumber(index),
                                                  mMgtMap(0),
                                                  mLocked(false),
                                                  mOffsets{0x0, 0x0, OCL_CTLR_BASE, 0x0, 0x0},
                                                  mOclRegionProfilingNumberSlots(XPAR_AXI_PERF_MON_2_NUMBER_SLOTS)
{
    //Assume this slot_id is equal with index
    slot_id = index;
    mLogfileName = nullptr;
    init(index, logfileName, verbosity);
    prlock=0;
}

void XOCLShim::init(unsigned index, const char *logfileName, xclVerbosityLevel verbosity)
{
    const std::string devName = "/dev/dri/renderD" + std::to_string(xcldev::pci_device_scanner::device_list[index].user_instance);
    mUserHandle = open(devName.c_str(), O_RDWR);
    if(mUserHandle > 0) {
        // Lets map 4M
        mUserMap = (char *)mmap(0, xcldev::pci_device_scanner::device_list[index].user_bar0_size, PROT_READ | PROT_WRITE, MAP_SHARED, mUserHandle, 0);
        if (mUserMap == MAP_FAILED) {
            std::cout << "Map failed: " << devName << std::endl;
            close(mUserHandle);
            mUserHandle = -1;
        }
    } else {
        std::cout << "Cannot open: " << devName << std::endl;
    }
    if( logfileName != nullptr ) {
        mLogStream.open(logfileName);
        mLogStream << "FUNCTION, THREAD ID, ARG..." << std::endl;
        mLogStream << __func__ << ", " << std::this_thread::get_id() << std::endl;
    }
    
#ifdef _MGMT_
	 std::string mgmtFile = "/dev/xclmgmt"+ std::to_string(xcldev::pci_device_scanner::device_list[index].mgmt_instance);
	 mMgtHandle = open(mgmtFile.c_str(), O_RDWR | O_SYNC);
	 if(mMgtHandle < 0)
	{
        std::cout << "Could not open " << mgmtFile << std::endl;
        throw std::runtime_error("Could not open file: " + mgmtFile );
	}
	 mMgtMap = (char *)mmap(0, xcldev::pci_device_scanner::device_list[index].mgmt_bar0_size, PROT_READ | PROT_WRITE, MAP_SHARED, mMgtHandle, 0);
	 if (mMgtMap == MAP_FAILED) // Not an error if user is not privileged
	     mMgtMap = nullptr;

    if (xclGetDeviceInfo2(&mDeviceInfo)) {
        std::cout<<__func__<< ", " <<" GetDeviceInfo from MgmtPf failed! "<<std::endl;
        close(mMgtHandle);
        mMgtHandle = -1;
    }
#else
    if (xclGetDeviceInfo2(&mDeviceInfo)) {
        std::cout<<__func__<< ", " <<" GetDeviceInfo from UserPf failed! "<<std::endl;
    }
#endif

    try {
        std::string dev_name = "/sys/bus/pci/devices/" + xcldev::pci_device_scanner::device_list[index].user_name;
        uint64_t dr_base_addr = xcldev::get_val_int(dev_name, "dr_base_addr");
        //std::cout << "dr_base_offset: " << dr_base_addr << std::endl;
        mOffsets[XCL_ADDR_KERNEL_CTRL] += dr_base_addr;
    }
    catch (std::exception &ex) {
        std::cout << ex.what() <<  std::endl;
    }

    //
    // Profiling - defaults
    // Class-level defaults: mIsDebugIpLayoutRead = mIsDeviceProfiling = false
    mDevUserName = xcldev::pci_device_scanner::device_list[index].user_name;
    mMemoryProfilingNumberSlots = 0;
    mPerfMonFifoCtrlBaseAddress = 0x00;
    mPerfMonFifoReadBaseAddress = 0x00;
}

XOCLShim::~XOCLShim()
{
    if (mLogStream.is_open()) {
        mLogStream << __func__ << ", " << std::this_thread::get_id() << std::endl;
        mLogStream.close();
    }

    if (mUserMap != MAP_FAILED)
        (void)munmap(mUserMap, xcldev::pci_device_scanner::device_list[mBoardNumber].user_bar0_size);

#ifdef _MGMT_
    if (mMgtMap)
        (void)munmap(mMgtMap, xcldev::pci_device_scanner::device_list[mBoardNumber].mgmt_bar0_size);
#endif

    if (mUserHandle > 0)
        close(mUserHandle);

#ifdef _MGMT_
    if (mMgtHandle > 0)
        close(mMgtHandle);
#endif

    if (prlock)
    {
        if (fpgamgmt_obj.m_FPGA_MgmtOpsMutexUnlock(prlock))
        {
            std::cout << "ERROR: Unlock pr_lock statue failed!\n";
        }
    }
}

int XOCLShim::pcieBarRead(unsigned int pf_bar, unsigned long long offset, void* buffer,
                          unsigned long long length)
{
    const char *mem = 0;
    switch (pf_bar) {
        case 0:
        {
            // BAR0 on PF0
            mem = mUserMap;
            break;
        }
        case 0x10000:
        {
            // BAR0 on PF1
            mem = mMgtMap;
            break;
        }
        default:
        {
            return -1;
        }
    }
    wordcopy(buffer, mem + offset, length);
    return 0;
}

int XOCLShim::pcieBarWrite(unsigned int pf_bar, unsigned long long offset, const void* buffer,
                           unsigned long long length)
{
    char *mem = 0;
    switch (pf_bar) {
        case 0:
        {
            // BAR0 on PF0
            mem = mUserMap;
            break;
        }
        case 0x10000:
        {
            // BAR0 on PF1
            mem = mMgtMap;
            break;
        }
        default:
        {
            return -1;
        }
    }

    wordcopy(mem + offset, buffer, length);
    return 0;
}


size_t XOCLShim::xclWrite(xclAddressSpace space, uint64_t offset, const void *hostBuf, size_t size)
{
    switch (space) {
        case XCL_ADDR_SPACE_DEVICE_PERFMON:
        {
            //offset += mOffsets[XCL_ADDR_SPACE_DEVICE_PERFMON];
            if (pcieBarWrite(SHIM_USER_BAR, offset, hostBuf, size) == 0) {
                return size;
            }
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
                               << std::hex << offset + i << ", 0x" << std::hex << std::setw(8)
                               << std::setfill('0') << reg[i] << std::dec << std::endl;
                }
            }
            if (pcieBarWrite(SHIM_USER_BAR, offset, hostBuf, size) == 0) {
                return size;
            }
            return -1;
        }
        case XCL_ADDR_SPACE_DEVICE_CHECKER:
        default:
        {
            return -EPERM;
        }
    }
}

size_t XOCLShim::xclRead(xclAddressSpace space, uint64_t offset, void *hostBuf, size_t size)
{
    if (mLogStream.is_open()) {
        mLogStream << __func__ << ", " << std::this_thread::get_id() << ", " << space << ", "
                   << offset << ", " << hostBuf << ", " << size << std::endl;
    }

    switch (space) {
        case XCL_ADDR_SPACE_DEVICE_PERFMON:
        {
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
            if (pcieBarRead(SHIM_USER_BAR, offset, hostBuf, size) == 0) {
                return size;
            }
            return -1;
        }
        default:
        {
            return -EPERM;
        }
    }
}

// Assume that the memory is always
// created for the device ddr for now. Ignoring the flags as well.
unsigned int XOCLShim::xclAllocBO(size_t size, xclBOKind domain, unsigned flags)
{   
    drm_xocl_create_bo info = {size, mNullBO, flags};
    int result = ioctl(mUserHandle, DRM_IOCTL_XOCL_CREATE_BO, &info);
    return result ? mNullBO : info.handle;
}

unsigned int XOCLShim::xclAllocUserPtrBO(void *userptr, size_t size, unsigned flags)
{
    drm_xocl_userptr_bo user = {reinterpret_cast<uint64_t>(userptr), size, mNullBO, flags};
    int result = ioctl(mUserHandle, DRM_IOCTL_XOCL_USERPTR_BO, &user);
    return result ? mNullBO : user.handle;
}

void XOCLShim::xclFreeBO(unsigned int boHandle)
{
    drm_gem_close closeInfo = {boHandle, 0};
    (void)ioctl(mUserHandle, DRM_IOCTL_GEM_CLOSE, &closeInfo);
}

int XOCLShim::xclWriteBO(unsigned int boHandle, const void *src, size_t size, size_t seek)
{
    drm_xocl_pwrite_bo pwriteInfo = { boHandle, 0, seek, size, reinterpret_cast<uint64_t>(src) };
    return ioctl(mUserHandle, DRM_IOCTL_XOCL_PWRITE_BO, &pwriteInfo);
}

int XOCLShim::xclReadBO(unsigned int boHandle, void *dst, size_t size, size_t skip)
{
    drm_xocl_pread_bo preadInfo = { boHandle, 0, skip, size, reinterpret_cast<uint64_t>(dst) };
    return ioctl(mUserHandle, DRM_IOCTL_XOCL_PREAD_BO, &preadInfo);
}

void *XOCLShim::xclMapBO(unsigned int boHandle, bool write)
{
    drm_xocl_info_bo info = { boHandle, 0, 0 };
    int result = ioctl(mUserHandle, DRM_IOCTL_XOCL_INFO_BO, &info);
    if (result) {
        return nullptr;
    }

    drm_xocl_map_bo mapInfo = { boHandle, 0, 0 };
    result = ioctl(mUserHandle, DRM_IOCTL_XOCL_MAP_BO, &mapInfo);
    if (result) {
        return nullptr;
    }    
    return mmap(0, info.size, (write ? (PROT_READ|PROT_WRITE) : PROT_READ),
              MAP_SHARED, mUserHandle, mapInfo.offset);
}

int XOCLShim::xclSyncBO(unsigned int boHandle, xclBOSyncDirection dir,
                        size_t size, size_t offset)
{
    drm_xocl_sync_bo_dir drm_dir = (dir == XCL_BO_SYNC_BO_TO_DEVICE) ?
            DRM_XOCL_SYNC_BO_TO_DEVICE :
            DRM_XOCL_SYNC_BO_FROM_DEVICE;
    drm_xocl_sync_bo syncInfo = {boHandle, 0, size, offset, drm_dir};
    return ioctl(mUserHandle, DRM_IOCTL_XOCL_SYNC_BO, &syncInfo);
}

int XOCLShim::xclGetErrorStatus(xclErrorStatus *info)
{
#ifndef _MGMT_
	return 0;
#else

#ifdef AXI_FIREWALL
    char buf[80];
    int ret;

    std::memset(info, 0, sizeof(xclErrorStatus));
    xclErrorStatus err_obj;
    std::memset(&err_obj, 0, sizeof(xclErrorStatus));
    ret = ioctl(mMgtHandle, XCLMGMT_IOCERRINFO, &err_obj);
    if (ret)
        return ret;

    info->mNumFirewalls = (err_obj.mNumFirewalls <= 8) ? err_obj.mNumFirewalls : 8;
    std::memcpy(&info->mAXIErrorStatus[0], &err_obj.mAXIErrorStatus[0], sizeof(struct xclAXIErrorStatus) * info->mNumFirewalls);
#endif  // AXI Firewall
    return 0;
#endif
}

int XOCLShim::xclGetDeviceInfo2(xclDeviceInfo2 *info)
{
    std::memset(info, 0, sizeof(xclDeviceInfo2));
    info->mMagic = 0X586C0C6C;
    info->mHALMajorVersion = XCLHAL_MAJOR_VER;
    info->mHALMajorVersion = XCLHAL_MINOR_VER;
    info->mMinTransferSize = DDR_BUFFER_ALIGNMENT;
    info->mDMAThreads = 2;
    unsigned char tmp_mMigCalib = 0;

#ifndef _MGMT_
    drm_xocl_info_udev udev_info;
    std::memset(&udev_info, 0, sizeof(struct drm_xocl_info_udev));
    int ret = ioctl(mUserHandle, DRM_IOCTL_XOCL_INFO_UDEV, &udev_info);
    if (ret) {
        return ret;
    }
    info->mVendorId = udev_info.vendor;
    info->mDeviceId = udev_info.device;
    info->mSubsystemId = udev_info.subsystem_device;
    info->mSubsystemVendorId = udev_info.subsystem_vendor;
    info->mDeviceVersion = udev_info.subsystem_device & 0x00ff;
    info->mDataAlignment = KB(4);
    info->mDDRSize = GB(udev_info.ddr_channel_size);
    info->mDDRBankCount = udev_info.ddr_channel_count;
    info->mDDRSize *= info->mDDRBankCount;
    const std::string name = newDeviceName(udev_info.vbnv);
    std::memcpy(info->mName, name.c_str(), name.size() + 1);

    info->mNumClocks = 2;
    for (int i = 0; i < info->mNumClocks; ++i) {
        info->mOCLFrequency[i] = 0;
    }

    info->mOnChipTemp  = 0;
    info->mFanTemp     = 0;
    info->mVInt        = 0;
    info->mVAux        = 0;
    info->mVBram       = 0;
    info->mMigCalib    = 1;
    info->mPCIeLinkWidth = 0;
    info->mPCIeLinkSpeed = 0;

    return 0;
    
#else
    // TODO: Windows build support
    //    XDMA_IOCINFO depends on _IOW, which is defined indirectly by <linux/ioctl.h>
    //    ioctl is defined in sys/ioctl.h
    xclmgmt_ioc_info obj;
    std::memset(&obj, 0, sizeof(xclmgmt_ioc_info));
    int ret = ioctl(mMgtHandle, XCLMGMT_IOCINFO, &obj);
    if (ret) {
        return ret;
    }
    info->mVendorId = obj.vendor;
    info->mDeviceId = obj.device;
    info->mSubsystemId = obj.subsystem_device;
    info->mSubsystemVendorId = obj.subsystem_vendor;
    info->mDeviceVersion = obj.subsystem_device & 0x00ff;

    // TUL cards (0x8238) have 4 GB / bank;
    // VU9P's (923F) also have 4GB/bank.
    // other cards have 8 GB memory / bank
    
    info->mDataAlignment = KB(4);
    info->mDDRSize = GB(obj.ddr_channel_size);
    info->mDDRBankCount = obj.ddr_channel_num;
    info->mDDRSize *= info->mDDRBankCount;

    const std::string name = newDeviceName(obj.vbnv);
    std::memcpy(info->mName, name.c_str(), name.size() + 1);

    info->mNumClocks = numClocks(info->mName);

    for (int i = 0; i < info->mNumClocks; ++i) {
        info->mOCLFrequency[i] = obj.ocl_frequency[i];
    }

    info->mOnChipTemp  = obj.onchip_temp;
    info->mFanTemp     = obj.fan_temp;
    info->mVInt        = obj.vcc_int;
    info->mVAux        = obj.vcc_aux;
    info->mVBram       = obj.vcc_bram;
	// map 4 bool value into 4 bits in one byte
	tmp_mMigCalib |= (unsigned char)obj.mig_calibration[3];
	tmp_mMigCalib << 3;
	tmp_mMigCalib |= (unsigned char)obj.mig_calibration[2];
	tmp_mMigCalib << 2;
	tmp_mMigCalib |= (unsigned char)obj.mig_calibration[1];
	tmp_mMigCalib << 1;
	tmp_mMigCalib |= (unsigned char)obj.mig_calibration[0];
	*(unsigned char *)&info->mMigCalib = tmp_mMigCalib;
    info->mPCIeLinkWidth = obj.pcie_link_width;
    info->mPCIeLinkSpeed = obj.pcie_link_speed;

    return 0;
#endif
}

int XOCLShim::resetDevice(xclResetKind kind)
{
    // Call a new IOCTL to just reset the OCL region
    if (kind == XCL_RESET_FULL) {
        int ret =  ioctl(mMgtHandle, XCLMGMT_IOCHOTRESET);
        return ret;
    }
    else if (kind == XCL_RESET_KERNEL) {
        return ioctl(mMgtHandle, XCLMGMT_IOCOCLRESET);
    }
    return -EINVAL;
}

bool XOCLShim::xclLockDevice()
{
    if (flock(mUserHandle, LOCK_EX | LOCK_NB) == -1) {
        return false;
    }

    mLocked = true;
    return true;
}

bool XOCLShim::xclUnlockDevice()
{
    if (flock(mUserHandle, LOCK_UN) == -1) {
			return false;
	}

    mLocked = false;
    return true;
}

int XOCLShim::xclReClock2(unsigned short region, const unsigned short *targetFreqMHz)
{
#ifndef _MGMT_	
	return 0;
#else
    std::cout << __func__ << " ERROR: xclReClock2 is not supported in this version!" << std::endl;
    xclmgmt_ioc_freqscaling obj;
    std::memset(&obj, 0, sizeof(xclmgmt_ioc_freqscaling));
    obj.ocl_region = region;
    obj.ocl_target_freq[0] = targetFreqMHz[0];
    obj.ocl_target_freq[1] = targetFreqMHz[1];
    return ioctl(mMgtHandle, XCLMGMT_IOCFREQSCALE, &obj);
#endif
}

bool XOCLShim::zeroOutDDR()
{
    // Zero out the DDR so MIG ECC believes we have touched all the bits
    // and it does not complain when we try to read back without explicit
    // write. The latter usually happens as a result of read-modify-write
    // TODO: Try and speed this up.
    // [1] Possibly move to kernel mode driver.
    // [2] Zero out specific buffers when they are allocated

    // TODO: Implement this
#ifdef _FUTRUE_IMPLEMENT_
    static const unsigned long long BLOCK_SIZE = 0x4000000;
    void *buf = 0;
    if (posix_memalign(&buf, DDR_BUFFER_ALIGNMENT, BLOCK_SIZE))
        return false;
    memset(buf, 0, BLOCK_SIZE);
    mDataMover->pset64(buf, BLOCK_SIZE, 0, mDeviceInfo.mDDRSize/BLOCK_SIZE);
    free(buf);
#endif
    return true;
}

int XOCLShim::xclLoadXclBin(const xclBin *buffer)
{
    int ret = 0;
    const char *xclbininmemory = reinterpret_cast<char*> (const_cast<xclBin*> (buffer));

    if (!memcmp(xclbininmemory, "xclbin2", 8)){
        ret = xclLoadAxlf(reinterpret_cast<const axlf*>(xclbininmemory));
    }
    else {
        if (mLogStream.is_open()) {
          mLogStream << __func__ << ", " << std::this_thread::get_id() << ", " << buffer << std::endl;
        }
        
        if (!mLocked) {
          return -EPERM;
        }
#ifdef _MGMT_
    const unsigned cmd = XCLMGMT_IOCICAPDOWNLOAD;
    xclmgmt_ioc_bitstream obj = { const_cast<xclBin *>(buffer) };
    ret = ioctl(mMgtHandle, cmd, &obj);

    // If it is an XPR DSA, zero out the DDR again as downloading the XCLBIN
    // reinitializes the DDR and results in ECC error.
    if (isXPR() && ret == 0)
    {
      if (mLogStream.is_open()) {
        mLogStream << __func__ << "XPR Device found, zeroing out DDR again.." << std::endl;
      }

      if (zeroOutDDR() == false) {
        if (mLogStream.is_open()) {
          mLogStream << __func__ << "zeroing out DDR failed" << std::endl;
        }
        return -EIO;
      }
    }
#endif
    std::cout << __func__ << " ERROR: xclLoadXclBin is not supported with mgmt handle in this version!" << std::endl;
    return 0;
    }

    // Note: We have frequently seen that downloading the bitstream causes the CU status
    // to go bad. This indicates an HLS issue (most probably). It is better to fail here
    // rather than crashing/erroring out later. This should save a lot of debugging time.
    
    return ret;
}

int XOCLShim::xclLoadAxlf(const axlf *buffer)
{
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
    
    if (mLogStream.is_open()) {
        mLogStream << __func__ << ", " << std::this_thread::get_id() << ", " << buffer << std::endl;
    }

    if (!mLocked) {
        return -EPERM;
    }

#ifdef _MGMT_
    std::cout << __func__ << " Info: xclLoadAxlf with mgmt is not supported in this version!" << std::endl;
    std::cout << __func__ << "ERROR: There is no mgmt device channel!" << std::endl;
    const unsigned cmd = XCLMGMT_IOCICAPDOWNLOAD_AXLF;
    xclmgmt_ioc_bitstream_axlf obj = {const_cast<axlf *>(buffer)};
    int ret = ioctl(mMgtHandle, cmd, &obj);
    if(0 != ret) {
        return ret;
    }
    
#else
    //Downloading the XCLBIN with m_FPGA_MgmtLoadHfiImage in host
    std::memset(xclbin_aeiid, 0, sizeof(xclbin_aeiid));    
    char *xclbintoload = reinterpret_cast<char*> (const_cast<axlf*> (buffer));
    memcpy(xclbin_aeiid,xclbintoload+AEI_RPLACE_OFFSET,AEI_ID_LEN);
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
    }
    else
    {
        std::cout<<"Loading pr image status error!"<<std::endl;
        std::cout<<"Ops_status_processing : error , "<<__LINE__<<" : CmdOpsStatus is :"<<prloadinfo.ulCmdOpsStatus<<std::endl;
        std::cout<<"PR status processing : error ,  "<<__LINE__<<" : FpgaPr status is :"<<prloadinfo.ulFpgaPrStatus<<std::endl;
        std::cout<<"PR status processing : error ,  "<<__LINE__<<" : FpgaPr aeiid is :"<<prloadinfo.acHfid<<std::endl;
        return -1;
    }             

    // If it is an XPR DSA, zero out the DDR again as downloading the XCLBIN
    // reinitializes the DDR and results in ECC error.
    if(isXPR())
    {
        if (mLogStream.is_open()) {
            mLogStream << __func__ << "XPR Device found, zeroing out DDR again.." << std::endl;
        }

        if (zeroOutDDR() == false)
        {
            if (mLogStream.is_open()) {
                mLogStream <<  __func__ << "zeroing out DDR failed" << std::endl;
            }
            return -EIO;
        }
    }

    // Note: We have frequently seen that downloading the bitstream causes the CU status
    // to go bad. This indicates an HLS issue (most probably). It is better to fail here
    // rather than crashing/erroring out later. This should save a lot of debugging time.
    
    drm_xocl_axlf axlf_obj = {const_cast<axlf *>(buffer)};
    int result = ioctl(mUserHandle, DRM_IOCTL_XOCL_READ_AXLF, &axlf_obj);
    if (result)
    {
        std::cout << __func__ << " Error: Xocl read axlf failed!" << std::endl;
        return result;
    }
#ifdef _MGMT_
    return ret;
#else
    return 0;
#endif
#endif
}

int XOCLShim::xclExportBO(unsigned int boHandle)
{
    drm_prime_handle info = {boHandle, 0, -1};
    int result = ioctl(mUserHandle, DRM_IOCTL_PRIME_HANDLE_TO_FD, &info);
    return !result ? info.fd : result;
}

unsigned int XOCLShim::xclImportBO(int fd, unsigned flags)
{
    drm_prime_handle info = {mNullBO, flags, fd};
    int result = ioctl(mUserHandle, DRM_IOCTL_PRIME_FD_TO_HANDLE, &info);
    if (result) {
        std::cout << __func__ << " ERROR: FD to handle IOCTL failed" << std::endl;
    }
    return !result ? info.handle : mNullBO;
}

int XOCLShim::xclGetBOProperties(unsigned int boHandle, xclBOProperties *properties)
{  
    drm_xocl_info_bo info = {boHandle, 0, mNullBO, mNullAddr};
    int result = ioctl(mUserHandle, DRM_IOCTL_XOCL_INFO_BO, &info);
    properties->handle = info.handle;
    properties->flags  = info.flags;
    properties->size   = info.size;
    properties->paddr  = info.paddr;
    properties->domain = XCL_BO_DEVICE_RAM; // currently all BO domains are XCL_BO_DEVICE_RAM
    return result;
}

int XOCLShim::xclGetUsageInfo(xclDeviceUsage *info)
{
    drm_xocl_usage_stat stat;
    std::memset(&stat, 0, sizeof(stat));
    int result = ioctl(mUserHandle, DRM_IOCTL_XOCL_USAGE_STAT, &stat);
    if (result) {
        return result;
    }
    std::memset(info, 0, sizeof(xclDeviceUsage));
    std::memcpy(info->h2c, stat.h2c, sizeof(size_t) * 8);
    std::memcpy(info->c2h, stat.c2h, sizeof(size_t) * 8);
    for (int i = 0; i < 8; i++) {
        info->ddrMemUsed[i] = stat.mm[i].memory_usage;
        info->ddrBOAllocated[i] = stat.mm[i].bo_count;
    }
    return 0;
}

bool XOCLShim::isGood() const {
    return (mUserHandle >= 0) && (mMgtHandle >= 0);
}

XOCLShim *XOCLShim::handleCheck(void *handle) {
    // Sanity checks
    if (!handle) {
        return 0;
    }
    
    if (!((XOCLShim *) handle)->isGood()) {
        return 0;
    }

    return (XOCLShim *) handle;
}

uint64_t XOCLShim::xclAllocDeviceBuffer(size_t size)
{
    if (mLogStream.is_open()) {
        mLogStream << __func__ << ", " << std::this_thread::get_id() << ", " << size << std::endl;
    }
    uint64_t result = mNullAddr;
    unsigned boHandle = xclAllocBO(size, XCL_BO_DEVICE_RAM, 0x0);
    if (boHandle == mNullBO) {
        return result;
    }

    drm_xocl_info_bo boInfo = {boHandle, 0, 0, 0};
    if (ioctl(mUserHandle, DRM_IOCTL_XOCL_INFO_BO, &boInfo)) {
        return result;
    }

    void *hbuf = xclMapBO(boHandle, true);
    if (hbuf == MAP_FAILED) {
        xclFreeBO(boHandle);
        return mNullAddr;
    }
    mLegacyAddressTable.insert(boInfo.paddr, size, std::make_pair(boHandle, (char *)hbuf));
    return boInfo.paddr;
}

uint64_t XOCLShim::xclAllocDeviceBuffer2(size_t size, xclMemoryDomains domain, unsigned flags)
{
   if (mLogStream.is_open()) {
        mLogStream << __func__ << ", " << std::this_thread::get_id() << ", " << size << ", "
                   << domain << ", " << flags << std::endl;
    }
    uint64_t result = mNullAddr;
    if (domain != XCL_MEM_DEVICE_RAM) {
        return result;
    }

    unsigned ddr = 1;
    ddr <<= flags;
    unsigned boHandle = xclAllocBO(size, XCL_BO_DEVICE_RAM, ddr);
    if (boHandle == mNullBO) {
        return result;
    }

    drm_xocl_info_bo boInfo = {boHandle, 0, 0, 0};
    if (ioctl(mUserHandle, DRM_IOCTL_XOCL_INFO_BO, &boInfo)) {
        return result;
    }

    void *hbuf = xclMapBO(boHandle, true);
    if (hbuf == MAP_FAILED) {
        xclFreeBO(boHandle);
        return mNullAddr;
    }
    mLegacyAddressTable.insert(boInfo.paddr, size, std::make_pair(boHandle, (char *)hbuf));
    return boInfo.paddr;
}

void XOCLShim::xclFreeDeviceBuffer(uint64_t buf)
{
    if (mLogStream.is_open()) {
        mLogStream << __func__ << ", " << std::this_thread::get_id() << ", " << buf << std::endl;
    }

    std::pair<unsigned, char *> bo = mLegacyAddressTable.erase(buf);
    drm_xocl_info_bo boInfo = {bo.first, 0, 0, 0};
    if (!ioctl(mUserHandle, DRM_IOCTL_XOCL_INFO_BO, &boInfo)) {
        munmap(bo.second, boInfo.size);
    }
    xclFreeBO(bo.first);
}


size_t XOCLShim::xclCopyBufferHost2Device(uint64_t dest, const void *src, size_t size, size_t seek)
{
    if (mLogStream.is_open()) {
        mLogStream << __func__ << ", " << std::this_thread::get_id() << ", " << dest << ", "
                   << src << ", " << size << ", " << seek << std::endl;
    }

    std::pair<unsigned, char *> bo = mLegacyAddressTable.find(dest);
    std::memcpy(bo.second + seek, src, size);
    int result = xclSyncBO(bo.first, XCL_BO_SYNC_BO_TO_DEVICE, size, seek);
    if (result) {
        return result;
    }
    return size;
}


size_t XOCLShim::xclCopyBufferDevice2Host(void *dest, uint64_t src, size_t size, size_t skip)
{
    if (mLogStream.is_open()) {
        mLogStream << __func__ << ", " << std::this_thread::get_id() << ", " << dest << ", "
                << src << ", " << size << ", " << skip << std::endl;
    }

    std::pair<unsigned, char *> bo = mLegacyAddressTable.find(src);
    int result = xclSyncBO(bo.first, XCL_BO_SYNC_BO_FROM_DEVICE, size, skip);
    if (result) {
        return 0;
    }
    std::memcpy(dest, bo.second + skip, size);
    return size;
}


ssize_t XOCLShim::xclUnmgdPwrite(unsigned flags, const void *buf, size_t count, uint64_t offset)
{
    if (flags) {
        return -EINVAL;
    }
    drm_xocl_pwrite_unmgd unmgd = {0, 0, offset, count, reinterpret_cast<uint64_t>(buf)};
    return ioctl(mUserHandle, DRM_IOCTL_XOCL_PWRITE_UNMGD, &unmgd);
}

ssize_t XOCLShim::xclUnmgdPread(unsigned flags, void *buf, size_t count, uint64_t offset)
{
    if (flags) {
        return -EINVAL;
    }
    drm_xocl_pread_unmgd unmgd = {0, 0, offset, count, reinterpret_cast<uint64_t>(buf)};
    return ioctl(mUserHandle, DRM_IOCTL_XOCL_PREAD_UNMGD, &unmgd);
}

int XOCLShim::xclExecBuf(unsigned int cmdBO)
{
    drm_xocl_execbuf exec = {0, cmdBO};
    return ioctl(mUserHandle, DRM_IOCTL_XOCL_EXECBUF, &exec);
}

int XOCLShim::xclRegisterEventNotify(unsigned int userInterrupt, int fd)
{
    drm_xocl_user_intr userIntr = {0, fd, (int)userInterrupt};
    return ioctl(mUserHandle, DRM_IOCTL_XOCL_USER_INTR, &userIntr);
}

  int XOCLShim::xclExecWait(int timeoutMilliSec)
  {
    std::vector<pollfd> uifdVector;
    pollfd info = {mUserHandle, POLLIN, 0};
    uifdVector.push_back(info);
    return poll(&uifdVector[0], uifdVector.size(), timeoutMilliSec);
  }
}; //end namespace xocl


static int getUserSlotNo(int fd) {
    drm_xocl_info obj;
    std::memset(&obj, 0, sizeof(drm_xocl_info));
    int ret = ioctl(fd, DRM_IOCTL_XOCL_INFO, &obj);
    if (ret) {
        return ret;
    }
    return obj.pci_slot;
}

static int getMgmtSlotNo(int handle) {

    xclmgmt_ioc_info obj;
    std::memset(&obj, 0, sizeof(xclmgmt_ioc_info));
    int ret = ioctl(handle, XCLMGMT_IOCINFO, &obj);
    if (ret) {
        return ret;
    }
    return obj.pci_slot;
}

unsigned xclProbe() {
    std::lock_guard<std::mutex> lock(xocl::deviceListMutex);

    if(xcldev::pci_device_scanner::device_list.size()) {
        return xcldev::pci_device_scanner::device_list.size();
    }

    xcldev::pci_device_scanner devScanner;
    devScanner.scan(false);
    return xcldev::pci_device_scanner::device_list.size();// xocl::deviceList.size();
}


xclDeviceHandle xclOpen(unsigned deviceIndex, const char *logFileName, xclVerbosityLevel level)
{
    if(xcldev::pci_device_scanner::device_list.size() <= deviceIndex) {
        printf("Cannot find index %d \n", deviceIndex);
        return nullptr;
    }

    xocl::XOCLShim *handle = new xocl::XOCLShim(deviceIndex, logFileName, level);
    if (!xocl::XOCLShim::handleCheck(handle)) {
        delete handle;
        handle = nullptr;
    }
    return static_cast<xclDeviceHandle>(handle);
}

void xclClose(xclDeviceHandle handle) {
    xocl::XOCLShim *drv = xocl::XOCLShim::handleCheck(handle);
    if (drv) {
        delete drv;
    }
}

int xclLoadXclBin(xclDeviceHandle handle, const xclBin *buffer)
{
    xocl::XOCLShim *drv = xocl::XOCLShim::handleCheck(handle);
    return drv ? drv->xclLoadXclBin(buffer) : -ENODEV;
}

size_t xclWrite(xclDeviceHandle handle, xclAddressSpace space, uint64_t offset, const void *hostBuf, size_t size)
{
    xocl::XOCLShim *drv = xocl::XOCLShim::handleCheck(handle);
    return drv ? drv->xclWrite(space, offset, hostBuf, size) : -ENODEV;
}

size_t xclRead(xclDeviceHandle handle, xclAddressSpace space, uint64_t offset, void *hostBuf, size_t size)
{
    xocl::XOCLShim *drv = xocl::XOCLShim::handleCheck(handle);
    return drv ? drv->xclRead(space, offset, hostBuf, size) : -ENODEV;
}

int xclGetErrorStatus(xclDeviceHandle handle, xclErrorStatus *info)
{
    xocl::XOCLShim *drv = xocl::XOCLShim::handleCheck(handle);
    if (!drv) {
        return -1;
    }
    return drv->xclGetErrorStatus(info);
}

int xclGetDeviceInfo2(xclDeviceHandle handle, xclDeviceInfo2 *info)
{
    xocl::XOCLShim *drv = xocl::XOCLShim::handleCheck(handle);
    return drv ? drv->xclGetDeviceInfo2(info) : -ENODEV;
}

unsigned int xclVersion () {
    return 2;
}
//#ifdef XCLHAL2

unsigned int xclAllocBO(xclDeviceHandle handle, size_t size, xclBOKind domain, unsigned flags)
{
    xocl::XOCLShim *drv = xocl::XOCLShim::handleCheck(handle);
    return drv ? drv->xclAllocBO(size, domain, flags) : -ENODEV;
}

unsigned int xclAllocUserPtrBO(xclDeviceHandle handle, void *userptr, size_t size, unsigned flags)
{
    xocl::XOCLShim *drv = xocl::XOCLShim::handleCheck(handle);
    return drv ? drv->xclAllocUserPtrBO(userptr, size, flags) : -ENODEV;
}

void xclFreeBO(xclDeviceHandle handle, unsigned int boHandle) {
    xocl::XOCLShim *drv = xocl::XOCLShim::handleCheck(handle);
    if (!drv) {
        return;
    }
    drv->xclFreeBO(boHandle);
}

int xclWriteBO(xclDeviceHandle handle, unsigned int boHandle, const void *src, size_t size, size_t seek)
{
    xocl::XOCLShim *drv = xocl::XOCLShim::handleCheck(handle);
    return drv ? drv->xclWriteBO(boHandle, src, size, seek) : -ENODEV;
}

size_t xclReadBO(xclDeviceHandle handle, unsigned int boHandle, void *dst, size_t size, size_t skip)
{
    xocl::XOCLShim *drv = xocl::XOCLShim::handleCheck(handle);
    return drv ? drv->xclReadBO(boHandle, dst, size, skip) : -ENODEV;
}

void *xclMapBO(xclDeviceHandle handle, unsigned int boHandle, bool write)
{
    xocl::XOCLShim *drv = xocl::XOCLShim::handleCheck(handle);
    return drv ? drv->xclMapBO(boHandle, write) : nullptr;
}


int xclSyncBO(xclDeviceHandle handle, unsigned int boHandle, xclBOSyncDirection dir, size_t size, size_t offset)
{
    xocl::XOCLShim *drv = xocl::XOCLShim::handleCheck(handle);
    return drv ? drv->xclSyncBO(boHandle, dir, size, offset) : -ENODEV;
}

int xclReClock2(xclDeviceHandle handle, unsigned short region, const unsigned short *targetFreqMHz)
{
    xocl::XOCLShim *drv = xocl::XOCLShim::handleCheck(handle);
    return drv ? drv->xclReClock2(region, targetFreqMHz) : -ENODEV;
}

int xclLockDevice(xclDeviceHandle handle)
{
    xocl::XOCLShim *drv = xocl::XOCLShim::handleCheck(handle);
    if (!drv)
        return -ENODEV;
    return drv->xclLockDevice() ? 0 : 1;
}

int xclUnlockDevice(xclDeviceHandle handle)
{
    xocl::XOCLShim *drv = xocl::XOCLShim::handleCheck(handle);
    if (!drv)
        return -ENODEV;
    return drv->xclUnlockDevice() ? 0 : 1;
}

int xclResetDevice(xclDeviceHandle handle, xclResetKind kind)
{
    xocl::XOCLShim *drv = xocl::XOCLShim::handleCheck(handle);
    return drv ? drv->resetDevice(kind) : -ENODEV;
}

/*
 * xclBootFPGA
 *
 * Sequence:
 *   1) call boot ioctl
 *   2) close the device, unload the driver
 *   3) remove and scan
 *   4) rescan pci devices
 *   5) reload the driver (done by the calling function xcldev::boot())
 *
 * Return 0 on success, -1 on failure.
 */
int xclBootFPGA(xclDeviceHandle handle)
{
    int retVal = -1;

    xocl::XOCLShim *drv = xocl::XOCLShim::handleCheck(handle);
    
    if (0 == drv)
    {
        //null ptr, return error
        return -1;
    }
	
    retVal = drv->xclBootFPGA(); // boot ioctl

    if( retVal == 0 )
    {
        xclClose(handle); // close the device, unload the driver
        retVal = xclRemoveAndScanFPGA(); // remove and scan
    }

    if( retVal == 0 )
    {
        xcldev::pci_device_scanner devScanner;
        devScanner.scan( true ); // rescan pci devices
    }

    return retVal;
}

int xclRemoveAndScanFPGA( void )
{
    const std::string devPath =    "/devices/";
    const std::string removePath = "/remove";
    const std::string pciPath =    "/sys/bus/pci";
    const std::string rescanPath = "/rescan";
    const char *input = "1\n";

    // remove devices "echo 1 > /sys/bus/pci/devices/<deviceHandle>/remove"
    for (int i = 0; i < xcldev::pci_device_scanner::device_list.size(); i++)
    {
        std::string dev_name_pf_user = pciPath + devPath + xcldev::pci_device_scanner::device_list[i].user_name + removePath;
        std::string dev_name_pf_mgmt = pciPath + devPath + xcldev::pci_device_scanner::device_list[i].mgmt_name + removePath;

        std::ofstream userFile( dev_name_pf_user );
        if( !userFile.is_open() ) {
            perror( dev_name_pf_user.c_str() );
            return errno;
        }
        userFile << input;

        std::ofstream mgmtFile( dev_name_pf_mgmt );
        if( !mgmtFile.is_open() ) {
            perror( dev_name_pf_mgmt.c_str() );
            return errno;
        }
        mgmtFile << input;
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));
    // initiate rescan "echo 1 > /sys/bus/pci/rescan"
    std::ofstream rescanFile( pciPath + rescanPath );
    if( !rescanFile.is_open() ) {
        perror( std::string( pciPath + rescanPath ).c_str() );
        return errno;
    }
    rescanFile << input;

    return 0;
}

int xclUpgradeFirmware(xclDeviceHandle handle, const char *fileName)
{
    xocl::XOCLShim *drv = xocl::XOCLShim::handleCheck(handle);
    return drv ? drv->xclUpgradeFirmware(fileName) : -ENODEV;
}

int xclUpgradeFirmware2(xclDeviceHandle handle, const char *fileName1, const char* fileName2)
{
    xocl::XOCLShim *drv = xocl::XOCLShim::handleCheck(handle);
    if (!drv) {
        return -ENODEV;
    }

    if(!fileName2 || std::strlen(fileName2) == 0) {
        return drv->xclUpgradeFirmware(fileName1);
    } else {
        return drv->xclUpgradeFirmware2(fileName1, fileName2);
    }
}

int xclUpgradeFirmwareXSpi(xclDeviceHandle handle, const char *fileName, int index)
{
    xocl::XOCLShim *drv = xocl::XOCLShim::handleCheck(handle);
    if (!drv) {
        return -1;
    }
    return drv->xclUpgradeFirmwareXSpi(fileName, index);
}

// Support for XCLHAL1 legacy API's

uint64_t xclAllocDeviceBuffer(xclDeviceHandle handle, size_t size)
{
    xocl::XOCLShim *drv = xocl::XOCLShim::handleCheck(handle);
    return drv ? drv->xclAllocDeviceBuffer(size) : xocl::mNullAddr;
}


uint64_t xclAllocDeviceBuffer2(xclDeviceHandle handle, size_t size, xclMemoryDomains domain, unsigned flags)
{
    xocl::XOCLShim *drv = xocl::XOCLShim::handleCheck(handle);
    return drv ? drv->xclAllocDeviceBuffer2(size, domain, flags) : xocl::mNullAddr;
}


void xclFreeDeviceBuffer(xclDeviceHandle handle, uint64_t buf)
{
    xocl::XOCLShim *drv = xocl::XOCLShim::handleCheck(handle);
    if (!drv) {
        return;
    }
    drv->xclFreeDeviceBuffer(buf);
}


size_t xclCopyBufferHost2Device(xclDeviceHandle handle, uint64_t dest, const void *src, size_t size, size_t seek)
{   
    xocl::XOCLShim *drv = xocl::XOCLShim::handleCheck(handle);
    return drv ? drv->xclCopyBufferHost2Device(dest, src, size, seek) : -ENODEV;
}


size_t xclCopyBufferDevice2Host(xclDeviceHandle handle, void *dest, uint64_t src, size_t size, size_t skip)
{
    xocl::XOCLShim *drv = xocl::XOCLShim::handleCheck(handle);
    return drv ? drv->xclCopyBufferDevice2Host(dest, src, size, skip) : -ENODEV;
}

int xclExportBO(xclDeviceHandle handle, unsigned int boHandle)
{
    xocl::XOCLShim *drv = xocl::XOCLShim::handleCheck(handle);
    return drv ? drv->xclExportBO(boHandle) : -ENODEV;
}

unsigned int xclImportBO(xclDeviceHandle handle, int fd, unsigned flags)
{
    xocl::XOCLShim *drv = xocl::XOCLShim::handleCheck(handle);
    if (!drv) {
        std::cout << __func__ << ", " << std::this_thread::get_id() << ", handle & XOCL Device are bad" << std::endl;
    }
    return drv ? drv->xclImportBO(fd, flags) : -ENODEV;
}

ssize_t xclUnmgdPwrite(xclDeviceHandle handle, unsigned flags, const void *buf, size_t count, uint64_t offset)
{
    xocl::XOCLShim *drv = xocl::XOCLShim::handleCheck(handle);
    return drv ? drv->xclUnmgdPwrite(flags, buf, count, offset) : -ENODEV;
}

ssize_t xclUnmgdPread(xclDeviceHandle handle, unsigned flags, void *buf, size_t count, uint64_t offset)
{
    xocl::XOCLShim *drv = xocl::XOCLShim::handleCheck(handle);
    return drv ? drv->xclUnmgdPread(flags, buf, count, offset) : -ENODEV;
}

int xclGetBOProperties(xclDeviceHandle handle, unsigned int boHandle, xclBOProperties *properties)
{   
    xocl::XOCLShim *drv = xocl::XOCLShim::handleCheck(handle);
    return drv ? drv->xclGetBOProperties(boHandle, properties) : -ENODEV;
}

int xclGetUsageInfo(xclDeviceHandle handle, xclDeviceUsage *info)
{
    xocl::XOCLShim *drv = xocl::XOCLShim::handleCheck(handle);
    return drv ? drv->xclGetUsageInfo(info) : -ENODEV;
}

int xclExecBuf(xclDeviceHandle handle, unsigned int cmdBO)
{
    xocl::XOCLShim *drv = xocl::XOCLShim::handleCheck(handle);
    return drv ? drv->xclExecBuf(cmdBO) : -ENODEV;
}

int xclRegisterEventNotify(xclDeviceHandle handle, unsigned int userInterrupt, int fd)
{
    xocl::XOCLShim *drv = xocl::XOCLShim::handleCheck(handle);
    return drv ? drv->xclRegisterEventNotify(userInterrupt, fd) : -ENODEV;
}

int xclXbsak(int argc, char *argv[])
{
    return xcldev::xclXbsak(argc, argv);
}

int xclExecWait(xclDeviceHandle handle, int timeoutMilliSec)
{
  xocl::XOCLShim *drv = xocl::XOCLShim::handleCheck(handle);
  return drv ? drv->xclExecWait(timeoutMilliSec) : -ENODEV;
}
