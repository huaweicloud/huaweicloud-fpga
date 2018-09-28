/**
 * Copyright (C) 2016-2017 Xilinx, Inc
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

/*
 * Copyright (C) 2015 Xilinx, Inc
 * Author: Paul Schumacher
 * Performance Monitoring using PCIe for XDMA HAL Driver
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
//#include "datamover.h"
#include "driver/xclng/include/mgmt-reg.h"
#include "driver/xclng/include/mgmt-ioctl.h"
#include "driver/xclng/include/xocl_ioctl.h"
#include "driver/include/xclperf.h"
#include "perfmon_parameters.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include <cstdio>
#include <cstring>
#include <cassert>
#include <algorithm>
#include <thread>
#include <vector>
#include <time.h>
#include <string.h>

#ifndef _WINDOWS
// TODO: Windows build support
//    unistd.h is linux only header file
//    it is included for read, write, close, lseek64
#include <unistd.h>
#endif

#ifdef _WINDOWS
#define __func__ __FUNCTION__
#endif

#define FAST_OFFLOAD_MAJOR 2
#define FAST_OFFLOAD_MINOR 2

namespace xocl {

  static int unmgdPread(int fd, void *buffer, size_t size, uint64_t addr)
  {
    drm_xocl_pread_unmgd unmgd = {0, 0, addr, size, reinterpret_cast<uint64_t>(buffer)};
    return ioctl(fd, DRM_IOCTL_XOCL_PREAD_UNMGD, &unmgd);
  }
  // Memory alignment for DDR and AXI-MM trace access
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
      	  {
              free(mBuffer);
              mBuffer = 0;
       	  }
      }
  };

  // ****************
  // Helper functions
  // ****************

  bool XOCLShim::isDSAVersion(unsigned majorVersion, unsigned minorVersion, bool onlyThisVersion) {
    unsigned checkVersion = (majorVersion << 4) + (minorVersion);
    if (onlyThisVersion)
      return (mDeviceInfo.mDeviceVersion == checkVersion);
    return (mDeviceInfo.mDeviceVersion >= checkVersion);
  }

  unsigned XOCLShim::getBankCount() {
    return mDeviceInfo.mDDRBankCount;
  }

  // Set number of profiling slots in monitor
  void XOCLShim::xclSetProfilingNumberSlots(xclPerfMonType type, uint32_t numSlots) {
    if (mLogStream.is_open())
      mLogStream << __func__ << ", " << std::this_thread::get_id()
                 << ", " << type << ", " << numSlots << std::endl;

    if (type == XCL_PERF_MON_MEMORY)
      mMemoryProfilingNumberSlots = numSlots;
    else if (type == XCL_PERF_MON_OCL_REGION)
      mOclRegionProfilingNumberSlots = numSlots;
  }

  // Get host timestamp to write to APM
  // IMPORTANT NOTE: this *must* be compatible with the method of generating
  // timestamps as defined in RTProfile::getTraceTime()
  uint64_t XOCLShim::getHostTraceTimeNsec() {
    struct timespec now;
    int err;
    if ((err = clock_gettime(CLOCK_MONOTONIC, &now)) < 0)
      return 0;

    return (uint64_t) now.tv_sec * 1000000000UL + (uint64_t) now.tv_nsec;
  }

  uint64_t XOCLShim::getPerfMonBaseAddress(xclPerfMonType type, uint32_t slotNum) {
    if (type == XCL_PERF_MON_MEMORY)         return mPerfMonBaseAddress[slotNum];
    if (type == XCL_PERF_MON_HOST_INTERFACE) return PERFMON1_OFFSET;
    if (type == XCL_PERF_MON_OCL_REGION)     return PERFMON2_OFFSET;
    return 0;
  }

  uint64_t XOCLShim::getPerfMonFifoBaseAddress(xclPerfMonType type, uint32_t fifonum) {
    if (type == XCL_PERF_MON_MEMORY) {
        return mPerfMonFifoCtrlBaseAddress;
    }
    // This is not updated
    if (type == XCL_PERF_MON_OCL_REGION) {
      if (fifonum == 0) return (PERFMON2_OFFSET + XPAR_AXI_PERF_MON_2_TRACE_OFFSET_0);
      if (fifonum == 1) return (PERFMON2_OFFSET + XPAR_AXI_PERF_MON_2_TRACE_OFFSET_1);
      if (fifonum == 2) return (PERFMON2_OFFSET + XPAR_AXI_PERF_MON_2_TRACE_OFFSET_2);
      return 0;
    }
    return 0;
  }

  uint64_t XOCLShim::getPerfMonFifoReadBaseAddress(xclPerfMonType type, uint32_t fifonum) {
    if (type == XCL_PERF_MON_MEMORY) {
        return mPerfMonFifoReadBaseAddress;
    }
    // This is not updated
    if (type == XCL_PERF_MON_OCL_REGION) {
      if (fifonum == 0) return (PERFMON2_OFFSET + XPAR_AXI_PERF_MON_2_TRACE_OFFSET_0);
      if (fifonum == 1) return (PERFMON2_OFFSET + XPAR_AXI_PERF_MON_2_TRACE_OFFSET_1);
      if (fifonum == 2) return (PERFMON2_OFFSET + XPAR_AXI_PERF_MON_2_TRACE_OFFSET_2);
      return 0;
    }
    return 0;
  }
  

  uint32_t XOCLShim::getPerfMonNumberFifos(xclPerfMonType type) {
    if (type == XCL_PERF_MON_MEMORY)
      return XPAR_AXI_PERF_MON_0_TRACE_NUMBER_FIFO;
    if (type == XCL_PERF_MON_HOST_INTERFACE)
      return XPAR_AXI_PERF_MON_1_TRACE_NUMBER_FIFO;
    if (type == XCL_PERF_MON_OCL_REGION) {
      if (mOclRegionProfilingNumberSlots > 4)
        return 3;
      else
        return 2;
    }
    return 0;
  }

  uint32_t XOCLShim::getPerfMonNumberSlots(xclPerfMonType type) {
    if (type == XCL_PERF_MON_MEMORY) {
      // NOTE: used to be (getBankCount() + 1)
      return mMemoryProfilingNumberSlots;
    }
    if (type == XCL_PERF_MON_HOST_INTERFACE) {
      return XPAR_AXI_PERF_MON_1_NUMBER_SLOTS;
    }
    if (type == XCL_PERF_MON_OCL_REGION) {
      return mOclRegionProfilingNumberSlots;
    }
    return 1;
  }

  void XOCLShim::getPerfMonSlotName(xclPerfMonType type, uint32_t slotnum,
		                            char* slotName, uint32_t length) {
    std::string str = (slotnum < XSPM_MAX_NUMBER_SLOTS) ? mPerfMonSlotName[slotnum] : "";
    strncpy(slotName, str.c_str(), length);
  }

  uint32_t XOCLShim::getPerfMonNumberSamples(xclPerfMonType type) {
    if (type == XCL_PERF_MON_MEMORY) return XPAR_AXI_PERF_MON_0_TRACE_NUMBER_SAMPLES;
    if (type == XCL_PERF_MON_HOST_INTERFACE) return XPAR_AXI_PERF_MON_1_TRACE_NUMBER_SAMPLES;
    // TODO: get number of samples from metadata
    if (type == XCL_PERF_MON_OCL_REGION) return XPAR_AXI_PERF_MON_2_TRACE_NUMBER_SAMPLES;
    return 0;
  }

  uint8_t XOCLShim::getPerfMonShowIDS(xclPerfMonType type) {
    if (type == XCL_PERF_MON_MEMORY) {
      if (isDSAVersion(1, 0, true))
        return 0;
      if (getBankCount() > 1)
        return XPAR_AXI_PERF_MON_0_SHOW_AXI_IDS_2DDR;
      return XPAR_AXI_PERF_MON_0_SHOW_AXI_IDS;
    }
    if (type == XCL_PERF_MON_HOST_INTERFACE) {
      return XPAR_AXI_PERF_MON_1_SHOW_AXI_IDS;
    }
    // TODO: get show IDs
    if (type == XCL_PERF_MON_OCL_REGION) {
      return XPAR_AXI_PERF_MON_2_SHOW_AXI_IDS;
    }
    return 0;
  }

  uint8_t XOCLShim::getPerfMonShowLEN(xclPerfMonType type) {
    if (type == XCL_PERF_MON_MEMORY) {
      if (getBankCount() > 1)
        return XPAR_AXI_PERF_MON_0_SHOW_AXI_LEN_2DDR;
      return XPAR_AXI_PERF_MON_0_SHOW_AXI_LEN;
    }
    if (type == XCL_PERF_MON_HOST_INTERFACE) {
      return XPAR_AXI_PERF_MON_1_SHOW_AXI_LEN;
    }
    // TODO: get show IDs
    if (type == XCL_PERF_MON_OCL_REGION) {
      return XPAR_AXI_PERF_MON_2_SHOW_AXI_LEN;
    }
    return 0;
  }

  uint32_t XOCLShim::getPerfMonSlotStartBit(xclPerfMonType type, uint32_t slotnum) {
    // NOTE: ID widths also set to 5 in HEAD/data/sdaccel/board_support/alpha_data/common/xclplat/xclplat_ip.tcl
    uint32_t bitsPerID = 5;
    uint8_t showIDs = getPerfMonShowIDS(type);
    uint8_t showLen = getPerfMonShowLEN(type);
    uint32_t bitsPerSlot = 10 + (bitsPerID * 4 * showIDs) + (16 * showLen);
    return (18 + (bitsPerSlot * slotnum));
  }

  uint32_t XOCLShim::getPerfMonSlotDataWidth(xclPerfMonType type, uint32_t slotnum) {
    // TODO: this only supports slot 0
    if (slotnum == 0) return XPAR_AXI_PERF_MON_0_SLOT0_DATA_WIDTH;
    if (slotnum == 1) return XPAR_AXI_PERF_MON_0_SLOT1_DATA_WIDTH;
    if (slotnum == 2) return XPAR_AXI_PERF_MON_0_SLOT2_DATA_WIDTH;
    if (slotnum == 3) return XPAR_AXI_PERF_MON_0_SLOT3_DATA_WIDTH;
    if (slotnum == 4) return XPAR_AXI_PERF_MON_0_SLOT4_DATA_WIDTH;
    if (slotnum == 5) return XPAR_AXI_PERF_MON_0_SLOT5_DATA_WIDTH;
    if (slotnum == 6) return XPAR_AXI_PERF_MON_0_SLOT6_DATA_WIDTH;
    if (slotnum == 7) return XPAR_AXI_PERF_MON_0_SLOT7_DATA_WIDTH;
    return XPAR_AXI_PERF_MON_0_SLOT0_DATA_WIDTH;
  }

  // Get the device clock frequency (in MHz)
  double XOCLShim::xclGetDeviceClockFreqMHz() {
    unsigned clockFreq = mDeviceInfo.mOCLFrequency[0];
    if (clockFreq == 0)
      clockFreq = 200;

    //if (mLogStream.is_open())
    //  mLogStream << __func__ << ": clock freq = " << clockFreq << std::endl;
    return ((double)clockFreq);
  }

  // Get the maximum bandwidth for host reads from the device (in MB/sec)
  // NOTE: for now, set to: (256/8 bytes) * 300 MHz = 9600 MBps
  double XOCLShim::xclGetReadMaxBandwidthMBps() {
    return 9600.0;
  }

  // Get the maximum bandwidth for host writes to the device (in MB/sec)
  // NOTE: for now, set to: (256/8 bytes) * 300 MHz = 9600 MBps
  double XOCLShim::xclGetWriteMaxBandwidthMBps() {
    return 9600.0;
  }

  // Convert binary string to decimal
  uint32_t XOCLShim::bin2dec(std::string str, int start, int number) {
    return bin2dec(str.c_str(), start, number);
  }

  // Convert binary char * to decimal
  uint32_t XOCLShim::bin2dec(const char* ptr, int start, int number) {
    const char* temp_ptr = ptr + start;
    uint32_t value = 0;
    int i = 0;

    do {
      if (*temp_ptr != '0' && *temp_ptr!= '1')
        return value;
      value <<= 1;
      if(*temp_ptr=='1')
        value += 1;
      i++;
      temp_ptr++;
    } while (i < number);

    return value;
  }

  // Convert decimal to binary string
  // NOTE: length of string is always sizeof(uint32_t) * 8
  std::string XOCLShim::dec2bin(uint32_t n) {
    char result[(sizeof(uint32_t) * 8) + 1];
    unsigned index = sizeof(uint32_t) * 8;
    result[index] = '\0';

    do {
      result[ --index ] = '0' + (n & 1);
    } while (n >>= 1);

    for (int i=index-1; i >= 0; --i)
      result[i] = '0';

    return std::string( result );
  }

  // Convert decimal to binary string of length bits
  std::string XOCLShim::dec2bin(uint32_t n, unsigned bits) {
    char result[bits + 1];
    unsigned index = bits;
    result[index] = '\0';

    do result[ --index ] = '0' + (n & 1);
    while (n >>= 1);

    for (int i=index-1; i >= 0; --i)
      result[i] = '0';

    return std::string( result );
  }

  // Reset all APM trace AXI stream FIFOs
  size_t XOCLShim::resetFifos(xclPerfMonType type) {

    uint64_t resetCoreAddress = getPerfMonFifoBaseAddress(type, 0) + AXI_FIFO_SRR;
    uint64_t resetFifoAddress = getPerfMonFifoBaseAddress(type, 0) + AXI_FIFO_RDFR;
    size_t size = 0;
    uint32_t regValue = AXI_FIFO_RESET_VALUE;

    size += xclWrite(XCL_ADDR_SPACE_DEVICE_PERFMON, resetCoreAddress, &regValue, 4);
    size += xclWrite(XCL_ADDR_SPACE_DEVICE_PERFMON, resetFifoAddress, &regValue, 4);
    return size;
  }

  // ********
  // Counters
  // ********

  // Start device counters performance monitoring
  size_t XOCLShim::xclPerfMonStartCounters(xclPerfMonType type) {
    if (mLogStream.is_open()) {
      mLogStream << __func__ << ", " << std::this_thread::get_id() << ", "
          << type << ", Start device counters..." << std::endl;
    }

    if (!mIsDeviceProfiling)
   	  return 0;

    size_t size = 0;
    uint32_t regValue;
    uint64_t baseAddress;
    uint32_t numSlots = getPerfMonNumberSlots(type);

    for (uint32_t i=0; i < numSlots; i++) {
      baseAddress = getPerfMonBaseAddress(type,i);
      
      // 1. Reset AXI - MM monitor metric counters
      size += xclRead(XCL_ADDR_SPACE_DEVICE_PERFMON, baseAddress + XSPM_CONTROL_OFFSET, &regValue, 4);

      regValue = regValue | XSPM_CR_COUNTER_RESET_MASK;
      size += xclWrite(XCL_ADDR_SPACE_DEVICE_PERFMON, baseAddress + XSPM_CONTROL_OFFSET, &regValue, 4);

      regValue = regValue & ~(XSPM_CR_COUNTER_RESET_MASK);
      size += xclWrite(XCL_ADDR_SPACE_DEVICE_PERFMON, baseAddress + XSPM_CONTROL_OFFSET, &regValue, 4);

      // 2. Start AXI-MM monitor metric counters
      regValue = regValue | XSPM_CR_COUNTER_ENABLE_MASK;
      size += xclWrite(XCL_ADDR_SPACE_DEVICE_PERFMON, baseAddress + XSPM_CONTROL_OFFSET, &regValue, 4);

      // 3. Read from sample register to ensure total time is read again at end
      size += xclRead(XCL_ADDR_SPACE_DEVICE_PERFMON, baseAddress + XSPM_SAMPLE_OFFSET, &regValue, 4);
    }
    return size;
  }

  // Stop both profile and trace performance monitoring
  size_t XOCLShim::xclPerfMonStopCounters(xclPerfMonType type) {
    if (mLogStream.is_open()) {
      mLogStream << __func__ << ", " << std::this_thread::get_id() << ", "
          << type << ", Stop and reset device counters..." << std::endl;
    }

    if (!mIsDeviceProfiling)
   	  return 0;

    size_t size = 0;
    uint32_t regValue;
    uint64_t baseAddress;
    uint32_t numSlots = getPerfMonNumberSlots(type);

    for (uint32_t i=0; i < numSlots; i++) {
    baseAddress = getPerfMonBaseAddress(type,i);

    // 1. Stop SPM metric counters
    size += xclRead(XCL_ADDR_SPACE_DEVICE_PERFMON, baseAddress + XSPM_CONTROL_OFFSET, &regValue, 4);

    regValue = regValue & ~(XSPM_CR_COUNTER_ENABLE_MASK);
    size += xclWrite(XCL_ADDR_SPACE_DEVICE_PERFMON, baseAddress + XSPM_CONTROL_OFFSET, &regValue, 4);
    }
    return size;
  }

  // Read SPM performance counters
  size_t XOCLShim::xclPerfMonReadCounters(xclPerfMonType type, xclCounterResults& counterResults) {
    if (mLogStream.is_open()) {
      mLogStream << __func__ << ", " << std::this_thread::get_id()
      << ", " << type << ", " << &counterResults
      << ", Read device counters..." << std::endl;
    }

    // Initialize all values in struct to 0
    memset(&counterResults, 0, sizeof(xclCounterResults));

    if (!mIsDeviceProfiling)
   	  return 0;

    size_t size = 0;
    uint64_t baseAddress;
    uint32_t sampleInterval;
    uint32_t numSlots = getPerfMonNumberSlots(type);

    for (uint32_t s=0; s < numSlots; s++) {
      baseAddress = getPerfMonBaseAddress(type,s);

      // Read sample interval register
      // NOTE: this also latches the sampled metric counters
      size += xclRead(XCL_ADDR_SPACE_DEVICE_PERFMON, 
                    baseAddress + XSPM_SAMPLE_OFFSET, 
                    &sampleInterval, 4);
      // Need to do this for every xilmon  
      if (s==0){
        counterResults.SampleIntervalUsec = sampleInterval / xclGetDeviceClockFreqMHz();
      }

      size += xclRead(XCL_ADDR_SPACE_DEVICE_PERFMON, 
                      baseAddress + XSPM_SAMPLE_WRITE_BYTES_OFFSET, 
                      &counterResults.WriteBytes[s], 4); 
      size += xclRead(XCL_ADDR_SPACE_DEVICE_PERFMON, 
                      baseAddress + XSPM_SAMPLE_WRITE_TRANX_OFFSET, 
                      &counterResults.WriteTranx[s], 4);
      size += xclRead(XCL_ADDR_SPACE_DEVICE_PERFMON, 
                      baseAddress + XSPM_SAMPLE_WRITE_LATENCY_OFFSET, 
                      &counterResults.WriteLatency[s], 4);
      size += xclRead(XCL_ADDR_SPACE_DEVICE_PERFMON, 
                      baseAddress + XSPM_SAMPLE_READ_BYTES_OFFSET, 
                      &counterResults.ReadBytes[s], 4);
      size += xclRead(XCL_ADDR_SPACE_DEVICE_PERFMON, 
                      baseAddress + XSPM_SAMPLE_READ_TRANX_OFFSET, 
                      &counterResults.ReadTranx[s], 4);
      size += xclRead(XCL_ADDR_SPACE_DEVICE_PERFMON, 
                      baseAddress + XSPM_SAMPLE_READ_LATENCY_OFFSET, 
                      &counterResults.ReadLatency[s], 4);

      if (mLogStream.is_open()) {
        mLogStream << "Reading ...SlotNum : " << s << std::endl;
        mLogStream << "Reading ...WriteBytes : " << counterResults.WriteBytes[s] << std::endl;
        mLogStream << "Reading ...WriteTranx : " << counterResults.WriteTranx[s] << std::endl;
        mLogStream << "Reading ...WriteLatency : " << counterResults.WriteLatency[s] << std::endl;
        mLogStream << "Reading ...ReadBytes : " << counterResults.ReadBytes[s] << std::endl;
        mLogStream << "Reading ...ReadTranx : " << counterResults.ReadTranx[s] << std::endl;
        mLogStream << "Reading ...ReadLatency : " << counterResults.ReadLatency[s] << std::endl;
      }
    }
    return size;
  }

  // *****
  // Trace
  // *****

  // Clock training used in converting device trace timestamps to host domain
  size_t XOCLShim::xclPerfMonClockTraining(xclPerfMonType type) {
    if (mLogStream.is_open()) {
      mLogStream << __func__ << ", " << std::this_thread::get_id() << ", "
          << type << ", Send clock training..." << std::endl;
    }
    // This will be enabled later. We're snapping first event to start of cu.
    return 1;
  }

  // Start trace performance monitoring
  size_t XOCLShim::xclPerfMonStartTrace(xclPerfMonType type, uint32_t startTrigger) {
    if (mLogStream.is_open()) {
      mLogStream << __func__ << ", " << std::this_thread::get_id()
      << ", " << type << ", " << startTrigger
      << ", Start device tracing..." << std::endl;
    }

    if (!mIsDeviceProfiling)
   	  return 0;

    size_t size = 0;
    xclPerfMonGetTraceCount(type);
    size += resetFifos(type);
    xclPerfMonGetTraceCount(type);
    return size;
  }

  // Stop trace performance monitoring
  size_t XOCLShim::xclPerfMonStopTrace(xclPerfMonType type) {
    if (mLogStream.is_open()) {
      mLogStream << __func__ << ", " << std::this_thread::get_id() << ", "
          << type << ", Stop and reset device tracing..." << std::endl;
    }

    if (!mIsDeviceProfiling)
   	  return 0;

    size_t size = 0;
    xclPerfMonGetTraceCount(type);
    size += resetFifos(type);
    return size;
  }

  // Get trace word count
  uint32_t XOCLShim::xclPerfMonGetTraceCount(xclPerfMonType type) {
    if (mLogStream.is_open()) {
      mLogStream << __func__ << ", " << std::this_thread::get_id()
      << ", " << type << std::endl;
    }

    if (!mIsDeviceProfiling)
   	  return 0;

    xclAddressSpace addressSpace = (type == XCL_PERF_MON_OCL_REGION) ?
        XCL_ADDR_KERNEL_CTRL : XCL_ADDR_SPACE_DEVICE_PERFMON;

    uint32_t fifoCount = 0;
    uint32_t numSamples = 0;
    uint32_t numBytes = 0;
    xclRead(addressSpace, getPerfMonFifoBaseAddress(type, 0) + AXI_FIFO_RLR, &fifoCount, 4);
    // Read bits 22:0 per AXI-Stream FIFO product guide (PG080, 10/1/14)
    numBytes = fifoCount & 0x7FFFFF;
    numSamples = numBytes / (XPAR_AXI_PERF_MON_0_TRACE_WORD_WIDTH/8);

    if (mLogStream.is_open()) {
      mLogStream << "  No. of trace samples = " << std::dec << numSamples
          << " (fifoCount = 0x" << std::hex << fifoCount << ")" << std::dec << std::endl;
    }

    return numSamples;
  }

  // Read all values from APM trace AXI stream FIFOs
  size_t XOCLShim::xclPerfMonReadTrace(xclPerfMonType type, xclTraceResultsVector& traceVector) {
    if (mLogStream.is_open()) {
      mLogStream << __func__ << ", " << std::this_thread::get_id()
      << ", " << type << ", " << &traceVector
      << ", Reading device trace stream..." << std::endl;
    }

    traceVector.mLength = 0;
    if (!mIsDeviceProfiling)
   	  return 0;

    uint32_t numSamples = xclPerfMonGetTraceCount(type);
    if (numSamples == 0)
      return 0;

    uint64_t fifoReadAddress[] = {0, 0, 0};
    if (type == XCL_PERF_MON_MEMORY) {
      fifoReadAddress[0] = getPerfMonFifoReadBaseAddress(type, 0) + AXI_FIFO_RDFD_AXI_FULL;
    }
    else {
      for (int i=0; i < 3; i++)
        fifoReadAddress[i] = getPerfMonFifoReadBaseAddress(type, i) + AXI_FIFO_RDFD;
    }

    size_t size = 0;

    // Limit to max number of samples so we don't overrun trace buffer on host
    uint32_t maxSamples = getPerfMonNumberSamples(type);
    numSamples = (numSamples > maxSamples) ? maxSamples : numSamples;
    traceVector.mLength = numSamples;

    const uint32_t bytesPerSample = (XPAR_AXI_PERF_MON_0_TRACE_WORD_WIDTH / 8);
    const uint32_t wordsPerSample = (XPAR_AXI_PERF_MON_0_TRACE_WORD_WIDTH / 32);
    //uint32_t numBytes = numSamples * bytesPerSample;
    uint32_t numWords = numSamples * wordsPerSample;

    // Create trace buffer on host (requires alignment)
    const int BUFFER_BYTES = MAX_TRACE_NUMBER_SAMPLES * bytesPerSample;
    const int BUFFER_WORDS = MAX_TRACE_NUMBER_SAMPLES * wordsPerSample;
#ifndef _WINDOWS
// TODO: Windows build support
//    alignas is defined in c++11
#if GCC_VERSION >= 40800
    alignas(AXI_FIFO_RDFD_AXI_FULL) uint32_t hostbuf[BUFFER_WORDS];
#else
    AlignedAllocator<uint32_t> alignedBuffer(AXI_FIFO_RDFD_AXI_FULL, BUFFER_WORDS);
    uint32_t* hostbuf = alignedBuffer.getBuffer();
#endif
#else
    uint32_t hostbuf[BUFFER_WORDS];
#endif

    // ******************************
    // Read all words from trace FIFO
    // NOTE: DSA Version >= 2.2
    // ******************************
    if (type == XCL_PERF_MON_MEMORY && isDSAVersion(FAST_OFFLOAD_MAJOR, FAST_OFFLOAD_MINOR, false)) {
      memset((void *)hostbuf, 0, BUFFER_BYTES);

      // Iterate over chunks
      // NOTE: AXI limits this to 4K bytes per transfer
      uint32_t chunkSizeWords = 256 * wordsPerSample;
      uint32_t chunkSizeBytes = 4 * chunkSizeWords;
      uint32_t words=0;

      // Read trace a chunk of bytes at a time
      if (numWords > chunkSizeWords) {
        for (; words < (numWords-chunkSizeWords); words += chunkSizeWords) {
          if (mLogStream.is_open()) {
            mLogStream << __func__ << ": reading " << chunkSizeBytes << " bytes from 0x"
                << std::hex << fifoReadAddress[0] << " and writing it to 0x"
                << (void *)(hostbuf + words) << std::dec << std::endl;
          }

          if (unmgdPread(mUserHandle, (void *)(hostbuf + words), chunkSizeBytes,  fifoReadAddress[0]) < 0)
            return 0;

          size += chunkSizeBytes;
        }
      }

      // Read remainder of trace not divisible by chunk size
      if (words < numWords) {
        chunkSizeBytes = 4 * (numWords - words);

        if (mLogStream.is_open()) {
          mLogStream << __func__ << ": reading " << chunkSizeBytes << " bytes from 0x"
              << std::hex << fifoReadAddress[0] << " and writing it to 0x"
              << (void *)(hostbuf + words) << std::dec << std::endl;
        }

        if (unmgdPread(mUserHandle, (void *)(hostbuf + words), chunkSizeBytes,  fifoReadAddress[0]) < 0)
            return 0;

        size += chunkSizeBytes;
      }

      if (mLogStream.is_open()) {
        mLogStream << __func__ << ": done reading " << size << " bytes " << std::endl;
      }
    }

    // ******************************
    // Read & process all trace FIFOs
    // ******************************
    for (uint32_t wordnum=0; wordnum < numSamples; wordnum++) {
      uint32_t index = wordsPerSample * wordnum;
      xclTraceResults results;
      uint64_t temp = 0;

      temp = *(hostbuf + index) | (uint64_t)*(hostbuf + index + 1) << 32;
      if (!temp)
        continue;

      // Initialize result to 0
      memset(&results, 0, sizeof(xclTraceResults));
      // SDSoC Packet Format
      results.Timestamp = temp & 0x1FFFFFFFFFFF;
      results.EventType = ((temp >> 45) & 0xF) ? XCL_PERF_MON_END_EVENT : 
          XCL_PERF_MON_START_EVENT;
      results.TraceID = (temp >> 49) & 0xFFF;
      results.Reserved = (temp >> 61) & 0x1;
      results.Overflow = (temp >> 62) & 0x1;
      results.Error = (temp >> 63) & 0x1;
      results.EventID = XCL_PERF_MON_HW_EVENT;
      traceVector.mArray[wordnum] = results;

      if (mLogStream.is_open()) {
        mLogStream << "  Trace sample " << std::dec << wordnum << ": ";
        mLogStream << dec2bin(uint32_t(temp>>32)) << " " << dec2bin(uint32_t(temp&0xFFFFFFFF));
        mLogStream << std::endl;
        mLogStream << " Timestamp : " << results.Timestamp << "   ";
        mLogStream << "Event Type : " << results.EventType << "   ";
        mLogStream << "slotID : " << results.TraceID << "   ";
        mLogStream << "Start, Stop : " << static_cast<int>(results.Reserved) << "   ";
        mLogStream << "Overflow : " << static_cast<int>(results.Overflow) << "   ";
        mLogStream << "Error : " << static_cast<int>(results.Error) << "   ";
        mLogStream << std::endl;
      }
    }

    return size;
  }

} // namespace xocl_gem


size_t xclPerfMonStartCounters(xclDeviceHandle handle, xclPerfMonType type)
{
  xocl::XOCLShim *drv = xocl::XOCLShim::handleCheck(handle);
  return drv ? drv->xclPerfMonStartCounters(type) : -ENODEV;
}

size_t xclPerfMonStopCounters(xclDeviceHandle handle, xclPerfMonType type)
{
  xocl::XOCLShim *drv = xocl::XOCLShim::handleCheck(handle);
  return drv ? drv->xclPerfMonStopCounters(type) : -ENODEV;
}

size_t xclPerfMonReadCounters(xclDeviceHandle handle, xclPerfMonType type, xclCounterResults& counterResults)
{
  xocl::XOCLShim *drv = xocl::XOCLShim::handleCheck(handle);
  return drv ? drv->xclPerfMonReadCounters(type, counterResults) : -ENODEV;
}

size_t xclPerfMonClockTraining(xclDeviceHandle handle, xclPerfMonType type)
{
  xocl::XOCLShim *drv = xocl::XOCLShim::handleCheck(handle);
  return drv ? drv->xclPerfMonClockTraining(type) : -ENODEV;
}

size_t xclPerfMonStartTrace(xclDeviceHandle handle, xclPerfMonType type, uint32_t startTrigger)
{
  xocl::XOCLShim *drv = xocl::XOCLShim::handleCheck(handle);
  return drv ? drv->xclPerfMonStartTrace(type, startTrigger) : -ENODEV;
}

size_t xclPerfMonStopTrace(xclDeviceHandle handle, xclPerfMonType type)
{
  xocl::XOCLShim *drv = xocl::XOCLShim::handleCheck(handle);
  return drv ? drv->xclPerfMonStopTrace(type) : -ENODEV;
}

uint32_t xclPerfMonGetTraceCount(xclDeviceHandle handle, xclPerfMonType type)
{
  xocl::XOCLShim *drv = xocl::XOCLShim::handleCheck(handle);
  return drv ? drv->xclPerfMonGetTraceCount(type) : -ENODEV;
}

size_t xclPerfMonReadTrace(xclDeviceHandle handle, xclPerfMonType type, xclTraceResultsVector& traceVector)
{
  xocl::XOCLShim *drv = xocl::XOCLShim::handleCheck(handle);
  return drv ? drv->xclPerfMonReadTrace(type, traceVector) : -ENODEV;
}

double xclGetDeviceClockFreqMHz(xclDeviceHandle handle)
{
  xocl::XOCLShim *drv = xocl::XOCLShim::handleCheck(handle);
  return drv ? drv->xclGetDeviceClockFreqMHz() : 0.0;
}

double xclGetReadMaxBandwidthMBps(xclDeviceHandle handle)
{
  xocl::XOCLShim *drv = xocl::XOCLShim::handleCheck(handle);
  return drv ? drv->xclGetReadMaxBandwidthMBps() : 0.0;
}


double xclGetWriteMaxBandwidthMBps(xclDeviceHandle handle)
{
  xocl::XOCLShim *drv = xocl::XOCLShim::handleCheck(handle);
  return drv ? drv->xclGetWriteMaxBandwidthMBps() : 0.0;
}

size_t xclGetDeviceTimestamp(xclDeviceHandle handle)
{
  return 0;
}

void xclSetProfilingNumberSlots(xclDeviceHandle handle, xclPerfMonType type, uint32_t numSlots)
{
  xocl::XOCLShim *drv = xocl::XOCLShim::handleCheck(handle);
  if (!drv)
    return;
  return drv->xclSetProfilingNumberSlots(type, numSlots);
}

uint32_t xclGetProfilingNumberSlots(xclDeviceHandle handle, xclPerfMonType type)
{
  xocl::XOCLShim *drv = xocl::XOCLShim::handleCheck(handle);
  if (!drv)
    return 2;
  return drv->getPerfMonNumberSlots(type);
}

void xclGetProfilingSlotName(xclDeviceHandle handle, xclPerfMonType type, uint32_t slotnum,
		                     char* slotName, uint32_t length)
{
  xocl::XOCLShim *drv = xocl::XOCLShim::handleCheck(handle);
  if (!drv)
    return;
  return drv->getPerfMonSlotName(type, slotnum, slotName, length);
}

void xclWriteHostEvent(xclDeviceHandle handle, xclPerfMonEventType type,
                       xclPerfMonEventID id)
{
  // don't do anything
}

// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689
