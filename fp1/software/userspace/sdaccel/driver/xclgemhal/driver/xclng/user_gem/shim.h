#ifndef _XOCL_GEM_SHIM_H_
#define _XOCL_GEM_SHIM_H_

/**
 * Copyright (C) 2016-2017 Xilinx, Inc

 * Author(s): Umang Parekh
 *          : Sonal Santan
 *          : Ryan Radjabi
 * XOCL GEM HAL Driver layered on top of XOCL kernel driver
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

#include "driver/include/xclhal2.h"
#include "driver/xclng/include/drm/drm.h"
#include "driver/xclng/include/xocl_ioctl.h"
#include "driver/xclng/include/mgmt-ioctl.h"
#include "driver/xclng/include/mgmt-reg.h"
#include <mutex>
#include <cstdint>
#include <fstream>
#include <list>
#include <map>
#include <utility>
#include <cassert>

#include "FPGA_Common.h"
#include "xclFpgaMgmtproxy.h"

namespace xocl {

struct AddresRange;

std::ostream& operator<< (std::ostream &strm, const AddresRange &rng);

/**
 * Simple tuple struct to store non overlapping address ranges: address and size
 */
struct AddresRange : public std::pair<uint64_t, size_t> {
    // size will be zero when we are looking up an address that was passed by the user
    AddresRange(uint64_t addr, size_t size = 0) : std::pair<uint64_t, size_t>(std::make_pair(addr, size)) {
        //std::cout << "CTOR(" << addr << ',' << size << ")\n";
    }
    AddresRange(AddresRange && rhs) : std::pair<uint64_t, size_t>(std::move(rhs)) {
        //std::cout << "MOVE CTOR(" << rhs.first << ',' << rhs.second << ")\n";
    }

    AddresRange(const AddresRange &rhs) = delete;
    AddresRange& operator=(const AddresRange &rhs) = delete;

    // Comparison operator is useful when using AddressRange as a key in std::map
    // Note one operand in the comparator may have only the address without the size
    // However both operands in the comparator will not have zero size
    bool operator < (const AddresRange& other) const {
        //std::cout << *this << " < " << other << "\n";
        if ((this->second != 0) && (other.second != 0))
            // regular ranges
            return (this->first < other.first);
        if (other.second == 0)
            // second range just has an address
            // (1000, 100) < (1200, 0)
            // (1000, 100) < (1100, 0) first range ends at 1099
            return ((this->first + this->second) <= other.first);
        assert(this->second == 0);
        // this range just has an address
        // (1100, 0) < (1200, 100)
        return (this->first < other.first);
    }
};

/**
 * Simple map of address range to its bo handle and mapped virtual address
 */
static const std::pair<unsigned, char *> mNullValue = std::make_pair(0xffffffff, nullptr);
class RangeTable {
    std::map<AddresRange, std::pair<unsigned, char *>> mTable;
    mutable std::mutex mMutex;
public:
    void insert(uint64_t addr, size_t size, std::pair<unsigned, char *> bo) {
        // assert(find(addr) == 0xffffffff);
        std::lock_guard<std::mutex> lock(mMutex);
        mTable[AddresRange(addr, size)] = bo;
    }

    std::pair<unsigned, char *> erase(uint64_t addr) {
        std::lock_guard<std::mutex> lock(mMutex);
        std::map<AddresRange, std::pair<unsigned, char *>>::const_iterator i = mTable.find(AddresRange(addr));
        if (i == mTable.end())
            return mNullValue;
        std::pair<unsigned, char *> result = i->second;
        mTable.erase(i);
        return result;
    }

    std::pair<unsigned, char *> find(uint64_t addr) const {
        std::lock_guard<std::mutex> lock(mMutex);
        std::map<AddresRange, std::pair<unsigned, char *>>::const_iterator i = mTable.find(AddresRange(addr));
        if (i == mTable.end())
            return mNullValue;
        return i->second;
    }
};

const unsigned SHIM_USER_BAR = 0x0;
const unsigned SHIM_MGMT_BAR = 0x10000;
const uint64_t mNullAddr = 0xffffffffffffffffull;
const uint64_t mNullBO = 0xffffffff;

class XOCLShim {
    struct ELARecord {
        unsigned mStartAddress;
        unsigned mEndAddress;
        unsigned mDataCount;

        std::streampos mDataPos;
        ELARecord() : mStartAddress(0), mEndAddress(0),
                mDataCount(0), mDataPos(0) {}
    };

    typedef std::list<ELARecord> ELARecordList;


public:
    ~XOCLShim();
    XOCLShim(unsigned index, const char *logfileName, xclVerbosityLevel verbosity);
    void init(unsigned index, const char *logfileName, xclVerbosityLevel verbosity);
    // Raw read/write
    size_t xclWrite(xclAddressSpace space, uint64_t offset, const void *hostBuf, size_t size);
    size_t xclRead(xclAddressSpace space, uint64_t offset, void *hostBuf, size_t size);
    unsigned int xclAllocBO(size_t size, xclBOKind domain, unsigned flags);
    unsigned int xclAllocUserPtrBO(void *userptr, size_t size, unsigned flags);
    void xclFreeBO(unsigned int boHandle);
    int xclWriteBO(unsigned int boHandle, const void *src, size_t size, size_t seek);
    int xclReadBO(unsigned int boHandle, void *dst, size_t size, size_t skip);
    void *xclMapBO(unsigned int boHandle, bool write);
    int xclSyncBO(unsigned int boHandle, xclBOSyncDirection dir, size_t size, size_t offset);
    int xclExportBO(unsigned int boHandle);
    unsigned int xclImportBO(int fd, unsigned flags);
    int xclGetBOProperties(unsigned int boHandle, xclBOProperties *properties);

    // Bitstream/bin download
    int xclLoadXclBin(const xclBin *buffer);
    int xclGetErrorStatus(xclErrorStatus *info);
    int xclGetDeviceInfo2(xclDeviceInfo2 *info);
    bool isGood() const;
    static XOCLShim *handleCheck(void * handle);
    int resetDevice(xclResetKind kind);
    bool xclLockDevice();
    bool xclUnlockDevice();
    int xclReClock2(unsigned short region, const unsigned short *targetFreqMHz);
    int xclGetUsageInfo(xclDeviceUsage *info);

    int xclUpgradeFirmware(const char *fileName);
    int xclUpgradeFirmware2(const char *file1, const char* file2);
    int xclUpgradeFirmwareXSpi(const char *fileName, int device_index=0);
    int xclTestXSpi(int device_index);
    int xclBootFPGA();
    int xclRemoveAndScanFPGA();

    // Legacy buffer management API support
    uint64_t xclAllocDeviceBuffer(size_t size);
    uint64_t xclAllocDeviceBuffer2(size_t size, xclMemoryDomains domain, unsigned flags);
    void xclFreeDeviceBuffer(uint64_t buf);
    size_t xclCopyBufferHost2Device(uint64_t dest, const void *src, size_t size, size_t seek);
    size_t xclCopyBufferDevice2Host(void *dest, uint64_t src, size_t size, size_t skip);

    ssize_t xclUnmgdPwrite(unsigned flags, const void *buf, size_t count, uint64_t offset);
    ssize_t xclUnmgdPread(unsigned flags, void *buf, size_t count, uint64_t offset);

    // Performance monitoring
    // Control
    double xclGetDeviceClockFreqMHz();
    double xclGetReadMaxBandwidthMBps();
    double xclGetWriteMaxBandwidthMBps();
    void xclSetProfilingNumberSlots(xclPerfMonType type, uint32_t numSlots);
    uint32_t getPerfMonNumberSlots(xclPerfMonType type);
    void getPerfMonSlotName(xclPerfMonType type, uint32_t slotnum,
                            char* slotName, uint32_t length);
    size_t xclPerfMonClockTraining(xclPerfMonType type);
    // Counters
    size_t xclPerfMonStartCounters(xclPerfMonType type);
    size_t xclPerfMonStopCounters(xclPerfMonType type);
    size_t xclPerfMonReadCounters(xclPerfMonType type, xclCounterResults& counterResults);

    //debug related
    uint32_t getCheckerNumberSlots(int type);
    uint32_t getIPCountAddrNames(int type, uint64_t *baseAddress, std::string * portNames);
    size_t xclDebugReadCounters(xclDebugCountersResults* debugResult);
    size_t xclDebugReadCheckers(xclDebugCheckersResults* checkerResult);

    // Trace
    size_t xclPerfMonStartTrace(xclPerfMonType type, uint32_t startTrigger);
    size_t xclPerfMonStopTrace(xclPerfMonType type);
    uint32_t xclPerfMonGetTraceCount(xclPerfMonType type);
    size_t xclPerfMonReadTrace(xclPerfMonType type, xclTraceResultsVector& traceVector);

    // Execute and interrupt abstraction
    int xclExecBuf(unsigned int cmdBO);
    int xclRegisterEventNotify(unsigned int userInterrupt, int fd);
    int xclExecWait(int timeoutMilliSec);

    int getBoardNumber( void ) { return mBoardNumber; }
    const char *getLogfileName( void ) { return mLogfileName; }
    xclVerbosityLevel getVerbosity( void ) { return mVerbosity; }

private:
    xclVerbosityLevel mVerbosity;
    std::ofstream mLogStream;
    int mUserHandle;
    int mMgtHandle;
    char *mUserMap;
    int mBoardNumber;
    char *mMgtMap;
    bool mLocked;
    const char *mLogfileName;
    uint64_t mOffsets[XCL_ADDR_SPACE_MAX];
    xclDeviceInfo2 mDeviceInfo;
    RangeTable mLegacyAddressTable;
    ELARecordList mRecordList;
    uint32_t mMemoryProfilingNumberSlots;
    uint32_t mOclRegionProfilingNumberSlots;
    std::string mDevUserName;
    //Those three variables are for loading xclbin in host and 
    //avoiding conficts between loading and running app
    INT32 prlock;
    xclFpgaMgmtProxy fpgamgmt_obj;
    int slot_id;

private:
    bool zeroOutDDR();
    bool isXPR() const {
        return ((mDeviceInfo.mSubsystemId >> 12) == 4);
    }

    int xclLoadAxlf(const axlf *buffer);

    // Upper two denote PF, lower two bytes denote BAR
    // USERPF == 0x0
    // MGTPF == 0x10000

    int pcieBarRead(unsigned int pf_bar, unsigned long long offset, void* buffer, unsigned long long length);
    int pcieBarWrite(unsigned int pf_bar, unsigned long long offset, const void* buffer, unsigned long long length);
    int freezeAXIGate();
    int freeAXIGate();
    // PROM flashing
    int prepare_microblaze(unsigned startAddress, unsigned endAddress);
    int prepare(unsigned startAddress, unsigned endAddress);
    int program_microblaze(std::ifstream& mcsStream, const ELARecord& record);
    int program(std::ifstream& mcsStream, const ELARecord& record);
    int program(std::ifstream& mcsStream);
    int waitForReady_microblaze(unsigned code, bool verbose = true);
    int waitForReady(unsigned code, bool verbose = true);
    int waitAndFinish_microblaze(unsigned code, unsigned data, bool verbose = true);
    int waitAndFinish(unsigned code, unsigned data, bool verbose = true);

    //XSpi flashing.
    bool prepareXSpi();
    int programXSpi(std::ifstream& mcsStream, const ELARecord& record);
    int programXSpi(std::ifstream& mcsStream);
    bool waitTxEmpty();
    bool isFlashReady();
    //bool windDownWrites();
    bool bulkErase();
    bool sectorErase(unsigned Addr);
    bool writeEnable();
#if 0
    bool dataTransfer(bool read);
#endif
    bool readPage(unsigned addr, uint8_t readCmd = 0xff);
    bool writePage(unsigned addr, uint8_t writeCmd = 0xff);
    unsigned readReg(unsigned offset);
    int writeReg(unsigned regOffset, unsigned value);
    bool finalTransfer(uint8_t *sendBufPtr, uint8_t *recvBufPtr, int byteCount);
    bool getFlashId();
    //All remaining read /write register commands can be issued through this function.
    bool readRegister(unsigned commandCode, unsigned bytes);
    bool writeRegister(unsigned commandCode, unsigned value, unsigned bytes);
    bool select4ByteAddressMode();
    bool deSelect4ByteAddressMode();


    // Performance monitoring helper functions
    bool isDSAVersion(unsigned majorVersion, unsigned minorVersion, bool onlyThisVersion);
    unsigned getBankCount();
    uint64_t getHostTraceTimeNsec();
    uint64_t getPerfMonBaseAddress(xclPerfMonType type, uint32_t slotNum);
    uint64_t getPerfMonFifoBaseAddress(xclPerfMonType type, uint32_t fifonum);
    uint64_t getPerfMonFifoReadBaseAddress(xclPerfMonType type, uint32_t fifonum);
    uint32_t getPerfMonNumberSamples(xclPerfMonType type);
    uint32_t getPerfMonNumberFifos(xclPerfMonType type);
    uint32_t getPerfMonByteScaleFactor(xclPerfMonType type);
    uint8_t  getPerfMonShowIDS(xclPerfMonType type);
    uint8_t  getPerfMonShowLEN(xclPerfMonType type);
    uint32_t getPerfMonSlotStartBit(xclPerfMonType type, uint32_t slotnum);
    uint32_t getPerfMonSlotDataWidth(xclPerfMonType type, uint32_t slotnum);
    size_t resetFifos(xclPerfMonType type);
    uint32_t bin2dec(std::string str, int start, int number);
    uint32_t bin2dec(const char * str, int start, int number);
    std::string dec2bin(uint32_t n);
    std::string dec2bin(uint32_t n, unsigned bits);

private:
    // Information extracted from platform linker
    bool mIsDebugIpLayoutRead = false;
    bool mIsDeviceProfiling = false;
    uint64_t mPerfMonFifoCtrlBaseAddress;
    uint64_t mPerfMonFifoReadBaseAddress;
    uint64_t mPerfMonBaseAddress[XSPM_MAX_NUMBER_SLOTS];
    std::string mPerfMonSlotName[XSPM_MAX_NUMBER_SLOTS];

}; // end class XOCLShim

}; // end namespace xocl

#endif

// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689
