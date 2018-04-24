#ifndef _XCL_MGMT_SHIM_H_
#define _XCL_MGMT_SHIM_H_


/**
 * Copyright (C) 2015-2016 Xilinx, Inc
 * Author: Umang Parekh
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

#include "xclhal.h"
#include "xclperf.h"

#ifdef _MGMT_
#include "mgmt-reg.h"
#include "mgmt-ioctl.h"
#endif

#include "perfmon_parameters.h"



#include <fstream>
#include <list>
#include <vector>
#include <string>
#include <mutex>
#include "FPGA_Common.h"
#include "xclFpgaMgmtproxy.h"

// Work around GCC 4.8 + XDMA BAR implementation bugs
// With -O3 PCIe BAR read/write are not reliable hence force -O2 as max
// optimization level for pcieBarRead() and pcieBarWrite()
#if defined(__GNUC__) && defined(NDEBUG)
#define SHIM_O2 __attribute__ ((optimize("-O2")))
#else
#define SHIM_O2
#endif

namespace xclxdma {

    template <typename T> class AlignedAllocator {
        void *mBuffer;
        size_t mCount;
    public:
        T *getBuffer() {
            return (T *)mBuffer;
        }

        size_t size() const {
            return mCount * sizeof(T);
        }

        AlignedAllocator(size_t alignment, size_t count) : mBuffer(0), mCount(count) {
            if (posix_memalign(&mBuffer, alignment, count * sizeof(T))) {
                mBuffer = 0;
            }
        }
        ~AlignedAllocator() {
            if (mBuffer)
                free(mBuffer);
			mBuffer = NULL;
        }
    };

    const unsigned SHIM_USER_BAR = 0x0;
    const unsigned SHIM_MGMT_BAR = 0x10000;

    class MemoryManager;
    class DataMover;
    // XDMA Shim
    class XDMAShim {

        struct ELARecord {
            unsigned mStartAddress;
            unsigned mEndAddress;
            unsigned mDataCount;

            std::streampos mDataPos;
            ELARecord() : mStartAddress(0), mEndAddress(0),
                          mDataCount(0), mDataPos(0) {}
        };

        typedef std::list<ELARecord> ELARecordList;

        typedef std::list<std::pair<uint64_t, uint64_t> > PairList;

    public:

        // Bitstreams
        
        int xclLoadXclBin(const xclBin *buffer);
        int xclLoadAxlf(const axlf *buffer);
        int xclUpgradeFirmware(const char *fileName);
        int xclUpgradeFirmware2(const char *file1, const char* file2);
        int xclUpgradeFirmwareXSpi(const char *fileName, int device_index=0);
        int xclTestXSpi(int device_index);
        int xclBootFPGA();
        int resetDevice(xclResetKind kind);
        int xclReClock2(unsigned short region, const unsigned short *targetFreqMHz);

        // Raw read/write
        size_t xclWrite(xclAddressSpace space, uint64_t offset, const void *hostBuf, size_t size);
        size_t xclRead(xclAddressSpace space, uint64_t offset, void *hostBuf, size_t size);

        // Buffer management
        uint64_t xclAllocDeviceBuffer(size_t size);
        uint64_t xclAllocDeviceBuffer2(size_t size, xclMemoryDomains domain, unsigned flags);
        void xclFreeDeviceBuffer(uint64_t buf);
        size_t xclCopyBufferHost2Device(uint64_t dest, const void *src, size_t size, size_t seek);
        size_t xclCopyBufferDevice2Host(void *dest, uint64_t src, size_t size, size_t skip);

   
        // Control
        double xclGetDeviceClockFreqMHz();
        double xclGetReadMaxBandwidthMBps();
        double xclGetWriteMaxBandwidthMBps();
        void xclSetOclRegionProfilingNumberSlots(uint32_t numSlots);
        size_t xclPerfMonClockTraining(xclPerfMonType type);
        // Counters
        size_t xclPerfMonStartCounters(xclPerfMonType type);
        size_t xclPerfMonStopCounters(xclPerfMonType type);
        size_t xclPerfMonReadCounters(xclPerfMonType type, xclCounterResults& counterResults);


        uint64_t getProtocolCheckerBaseAddress(int type);
        uint32_t getCheckerNumberSlots(int type);
        size_t xclDebugReadCounters(xclDebugCountersResults* debugResult);
        size_t xclDebugReadCheckers(xclDebugCheckersResults* checkerResult);

     
        size_t xclPerfMonStartTrace(xclPerfMonType type, uint32_t startTrigger);
        size_t xclPerfMonStopTrace(xclPerfMonType type);
        uint32_t xclPerfMonGetTraceCount(xclPerfMonType type);
        size_t xclPerfMonReadTrace(xclPerfMonType type, xclTraceResultsVector& traceVector);

        // Sanity checks
        int xclGetAXIErrorStatus(xclAXIErrorStatus *info);
        int xclGetDeviceInfo2(xclDeviceInfo2 *info);
        static XDMAShim *handleCheck(void *handle);
        static unsigned xclProbe();
        bool xclLockDevice();
        unsigned getTAG() const {
            return mTag;
        }
        bool isGood() const;

        ~XDMAShim();
        XDMAShim(unsigned index, const char *logfileName, xclVerbosityLevel verbosity);

    private:
        bool zeroOutDDR();
        // return true if CU status is IDLE (0x4) else false
        bool checkCUStatus();

        bool isXPR() const {
            return ((mDeviceInfo.mDeviceId == 0x4907) || (mDeviceInfo.mDeviceId == 0x4807));
        }
        void initMemoryManager();

        // Core DMA code
        // Upper two denote PF, lower two bytes denote BAR
        // USERPF == 0x0
        // MGTPF == 0x10000
        SHIM_O2 int pcieBarRead(unsigned int pf_bar, unsigned long long offset, void* buffer,
                                unsigned long long length);
        SHIM_O2 int pcieBarWrite(unsigned int pf_bar, unsigned long long offset, const void* buffer,
                                 unsigned long long length);
        int freezeAXIGate();
        int freeAXIGate();

        // PROM flashing
        int prepare(unsigned startAddress, unsigned endAddress);
        int program(std::ifstream& mcsStream, const ELARecord& rd);
        int program(std::ifstream& mcsStream);
        int waitForReady(unsigned code, bool verbose = true);
        int waitAndFinish(unsigned code, unsigned data, bool verbose = true);

        //XSpi flashing.
        bool prepareXSpi();
        int programXSpi(std::ifstream& mcsStream, const ELARecord& rd);
        int programXSpi(std::ifstream& mcsStream);
        bool waitTxEmpty();
        bool isFlashReady();
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


   
        bool isDSAVersion(unsigned majorVersion, unsigned minorVersion, bool onlyThisVersion);
        unsigned getBankCount();
        uint64_t getHostTraceTimeNsec();
        uint64_t getPerfMonBaseAddress(xclPerfMonType type);
        uint64_t getPerfMonFifoBaseAddress(xclPerfMonType type, uint32_t fifonum);
        uint64_t getPerfMonFifoReadBaseAddress(xclPerfMonType type, uint32_t fifonum);
        uint32_t getPerfMonNumberSlots(xclPerfMonType type);
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
        // This is a hidden signature of this class and helps in preventing
        // user errors when incorrect pointers are passed in as handles.
        const unsigned mTag;
        const int mBoardNumber;
        const size_t maxDMASize;
        bool mLocked;
        INT32 prlock;
        xclFpgaMgmtProxy fpgamgmt_obj;

#ifndef _WINDOWS
// TODO: Windows build support
        // mOffsets doesn't seem to be used
        // and it caused window compilation error when we try to initialize it
        const uint64_t mOffsets[XCL_ADDR_SPACE_MAX];
#endif
        DataMover *mDataMover;
        int mUserHandle;
        int mMgtHandle;
        uint32_t mOclRegionProfilingNumberSlots;

        char *mUserMap;
        char *mMgtMap;
        std::ofstream mLogStream;
        xclVerbosityLevel mVerbosity;
        std::string mBinfile;
        ELARecordList mRecordList;
        std::vector<MemoryManager *> mDDRMemoryManager;
        xclDeviceInfo2 mDeviceInfo;

    public:
        static const unsigned TAG;
    };
}

#endif
