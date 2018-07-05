/**
 * Copyright (C) 2016 Xilinx, Inc
 * Author: Sonal Santan
 * Simple command line utility to inetract with SDX PCIe devices
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

#include <getopt.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <cstring>
#include <cctype>
#include <iostream>
#include <fstream>
#include <chrono>
#include <stdexcept>
#include <assert.h>
#include <vector>
#include <memory>
#include <map>
#include <iomanip>
#include <algorithm>


#include "xclhal.h"
#include "xcl_axi_checker_codes.h"
#include "memaccess.h"
#include "FPGA_Common.h"

#define TO_STRING(x) #x
#define BIT(x) (0x1 << x)
#define AXI_FIREWALL

class Timer {
    std::chrono::high_resolution_clock::time_point mTimeStart;
public:
    Timer() {
        reset();
    }
    long long stop() {
        std::chrono::high_resolution_clock::time_point timeEnd = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::microseconds>(timeEnd - mTimeStart).count();
    }
    void reset() {
        mTimeStart = std::chrono::high_resolution_clock::now();
    }
};

/*
 * Simple command line tool to query and interact with SDx PCIe devices
 * The tool statically links with xcldma HAL driver inorder to avoid
 * dependencies on environment variables like XILINX_OPENCL, LD_LIBRARY_PATH, etc.
 * TODO:
 * Rewrite the command line parsing to provide interface like Android adb:
 * xcldev <cmd> [options]
 */

namespace xcldev {
    enum command {
        FLASH,
        PROGRAM,
        CLOCK,
        BOOT,
        HELP,
        QUERY,
        RESET,
        RUN,
        FAN,
        DMATEST,
        LIST,
        SCAN,
        MEM,
        STATUS,
        CMD_MAX
    };
    enum subcommand {
        MEM_READ = 0,
        MEM_WRITE,
        STATUS_APM,
        STATUS_LAPC
    };
    enum statusmask {
        STATUS_NONE_MASK = 0x0,
        STATUS_APM_MASK = 0x1,
        STATUS_LAPC_MASK = 0x2
    };

    static const std::pair<std::string, command> map_pairs[] = {
#ifdef _MGMT_
        std::make_pair("flash", FLASH),
        std::make_pair("program", PROGRAM),
        std::make_pair("clock", CLOCK),
        std::make_pair("boot", BOOT),
#endif
        std::make_pair("help", HELP),
#ifdef _MGMT_
        std::make_pair("query", QUERY),
        std::make_pair("reset", RESET),
        std::make_pair("run", RUN),
        std::make_pair("fan", FAN),
#endif
        std::make_pair("dmatest", DMATEST),
        std::make_pair("list", LIST),
#ifdef _MGMT_
        std::make_pair("scan", SCAN),
#endif
 
#ifdef _MGMT_
        std::make_pair("status", STATUS),
#endif
       std::make_pair("mem", MEM)
    };

    static const std::pair<std::string, subcommand> subcmd_pairs[] = {
        std::make_pair("read", MEM_READ),
        std::make_pair("write", MEM_WRITE),
        std::make_pair("apm", STATUS_APM),
        std::make_pair("lapc", STATUS_LAPC)
    };

    static const std::map<std::string, command> commandTable(map_pairs, map_pairs + sizeof(map_pairs) / sizeof(map_pairs[0]));

    class device {
        unsigned int m_idx;
        xclDeviceHandle m_handle;
        xclDeviceInfo2 m_devinfo;
        xclAXIErrorStatus m_errinfo;
        bool m_multiclock;
    public:
        xclDeviceHandle mpub_handle;
    public:
        device(unsigned int idx, const char* log) : m_idx(idx), m_handle(nullptr), m_devinfo{} {
            m_handle = xclOpen(m_idx, log, XCL_QUIET);
            mpub_handle = m_handle;
            if (!m_handle)
                throw std::runtime_error("Failed to open device index, " + std::to_string(m_idx) +
                                         "\n\n1. Make sure that xdma and xclmgmt drivers are installed \n2. Check if " +
                                         "/dev/xdma* and /dev/xclmgmt* exists. If not, then " +
                                         "run xbinst and sudo <xbinst dir>/install.sh -f yes to force install the latest drivers.\n"+
                                         "3. Run dmesg to see if something weird is going on.");
            if (xclGetDeviceInfo2(m_handle, &m_devinfo))
                throw std::runtime_error("Unable to query device index, " + std::to_string(m_idx));
#ifdef _MGMT_
	#ifdef AXI_FIREWALL
	            if (xclGetAXIErrorStatus(m_handle, &m_errinfo))
	                throw std::runtime_error("Unable to query device index for AXI error, " + std::to_string(m_idx));
	#endif
#endif
        }

        device(device&& rhs) : m_idx(rhs.m_idx), m_handle(rhs.m_handle), m_devinfo(std::move(rhs.m_devinfo)) {
        }

        device(const device &dev) = delete;
        device& operator=(const device &dev) = delete;

        ~device() {
            xclClose(m_handle);
        }

        const char *name() const {
            return m_devinfo.mName;
        }

        int flash(const std::string& mcs1, const std::string& mcs2) {
            if (getuid() && geteuid()) {
                std::cout << "ERROR: flash operation requires root privileges" << std::endl;
                return -EACCES;
            }
            int status = xclUpgradeFirmware2(m_handle, mcs1.c_str(), mcs2.c_str());
	    if(status == 0) {
		std::cout << "Please cold boot your machime." << std::endl;
	    }
	    return status;
        }

        int reclock2(unsigned regionIndex, const unsigned short *freq) {
            const unsigned short targetFreqMHz[4] = {freq[0], freq[1], 0, 0};
            return xclReClock2(m_handle, 0, targetFreqMHz);
        }

        std::string parseCUStatus(unsigned val) const {
            char delim = '(';
            std::string status;
            if (val & 0x1) {
                status += delim;
                status += "START";
                delim = '|';
            }
            if (val & 0x2) {
                status += delim;
                status += "DONE";
                delim = '|';
            }
            if (val & 0x4) {
                status += delim;
                status += "IDLE";
                delim = '|';
            }
            if (val & 0x8) {
                status += delim;
                status += "READY";
                delim = '|';
            }
            if (val & 0x10) {
                status += delim;
                status += "RESTART";
                delim = '|';
            }
            if (status.size())
                status += ')';
            else if (val == 0x0)
                status = "(--)";
            else
                status = "(UNKNOWN)";
            return status;
        }
        std::string parseFirewallStatus(unsigned val) const {
            char delim = '(';
            std::string status;
            // Read channel error
            if (val & BIT(0)) {
                status += delim;
                status += "READ_RESPONSE_BUSY";
                delim = '|';
            }
            if (val & BIT(1)) {
                status += delim;
                status += "RECS_ARREADY_MAX_WAIT";
                delim = '|';
            }
            if (val & BIT(2)) {
                status += delim;
                status += "RECS_CONTINUOUS_RTRANSFERS_MAX_WAIT";
                delim = '|';
            }
            if (val & BIT(3)) {
                status += delim;
                status += "ERRS_RDATA_NUM";
                delim = '|';
            }
            if (val & BIT(4)) {
                status += delim;
                status += "ERRS_RID";
                delim = '|';
            }
            // Write channel error
            if (val & BIT(16)) {
                status += delim;
                status += "WRITE_RESPONSE_BUSY";
                delim = '|';
            }
            if (val & BIT(17)) {
                status += delim;
                status += "RECS_AWREADY_MAX_WAIT";
                delim = '|';
            }
            if (val & BIT(18)) {
                status += delim;
                status += "RECS_WREADY_MAX_WAIT";
                delim = '|';
            }
            if (val & BIT(19)) {
                status += delim;
                status += "RECS_WRITE_TO_BVALID_MAX_WAIT";
                delim = '|';
            }
            if (val & BIT(20)) {
                status += delim;
                status += "ERRS_BRESP";
                delim = '|';
            }
            if (status.size())
                status += ')';
            else if (val == 0x0)
                status = "(GOOD)";
            else
                status = "(UNKNOWN)";
            return status;
        }

        std::ostream& dump(std::ostream& ostr) const {
            ostr << "DSA name:       " << m_devinfo.mName << "\n";
            ostr << "HAL ver:        " << m_devinfo.mHALMajorVersion << "." << m_devinfo.mHALMinorVersion << "\n";
            ostr << "Vendor:         " << std::hex << m_devinfo.mVendorId << std::dec << "\n";
            ostr << "Device:         " << std::hex << m_devinfo.mDeviceId << std::dec << "\n";
            ostr << "Device ver:     " << m_devinfo.mDeviceVersion << "\n";
            ostr << "SDevice:        " << std::hex << m_devinfo.mSubsystemId << std::dec << "\n";
            ostr << "SVendor:        " << std::hex << m_devinfo.mSubsystemVendorId << std::dec << "\n";
            ostr << "DDR size:       " << "0x" << std::hex << m_devinfo.mDDRSize/1024 << std::dec << " KB\n";
            ostr << "DDR count:      " << m_devinfo.mDDRBankCount << "\n";
            ostr << "Data alignment: " << m_devinfo.mDataAlignment << "\n";
            ostr << "DDR free size:  " << "0x" << std::hex << m_devinfo.mDDRFreeSize/1024 << std::dec << " KB\n";
            ostr << "Min xfer size:  " << m_devinfo.mMinTransferSize << "\n";
            ostr << "OnChip Temp:    " << m_devinfo.mOnChipTemp << " C\n";
            //ostr << "Fan Temp:       " << m_devinfo.mFanTemp<< " C\n";
            ostr << "VCC INT:        " << m_devinfo.mVInt << " mV\n";
            ostr << "VCC AUX:        " << m_devinfo.mVAux << " mV\n";
            ostr << "VCC BRAM:       " << m_devinfo.mVBram << " mV\n";
            ostr << "OCL Frequency:\n";
            for(unsigned i= 0; i < m_devinfo.mNumClocks; ++i) {
                ostr << "  " << std::setw(7) << i << ":      " <<  m_devinfo.mOCLFrequency[i] << " MHz\n";
            }
            ostr << "PCIe:           " << "GEN" << m_devinfo.mPCIeLinkSpeed << " x " << m_devinfo.mPCIeLinkWidth << "\n";
            ostr << "DMA threads:    " << m_devinfo.mDMAThreads << "\n";
            //ostr << "Fan Speed:      " << m_devinfo.mFanSpeed  << "\n";
            ostr << "MIG Calibrated: " << std::boolalpha << m_devinfo.mMigCalib << std::noboolalpha << "\n";

#ifdef AXI_FIREWALL
            char       cbuf[80];
            struct tm *ts;
            time_t temp;
            ostr << "Firewall Last Error Status:\n";
            for(unsigned i= 0; i < 3; ++i) {
              ostr << "  " << std::setw(7) << i << ":      0x" << std::hex << m_errinfo.mErrFirewallStatus[i] << std::dec << " "
                  << parseFirewallStatus(m_errinfo.mErrFirewallStatus[i]) ;
              if(m_errinfo.mErrFirewallStatus[i] != 0x0) {
                temp = (time_t)m_errinfo.mErrFirewallTime[i];
                ts = localtime(&temp);
                strftime(cbuf, sizeof(cbuf), "%a %Y-%m-%d %H:%M:%S %Z",ts);
                ostr << ". Error occurred on " << cbuf;
              }
              ostr << "\n";
            }
#endif // AXI Firewall
            ostr << "CU Status:\n";
            unsigned buf[16];
            for (unsigned i = 0; i < 4; i++) {
                xclRead(m_handle, XCL_ADDR_KERNEL_CTRL, i * 4096, static_cast<void *>(buf), 16);
                ostr << "  " << std::setw(7) << i << ":      0x" << std::hex << buf[0] << std::dec << " " << parseCUStatus(buf[0]) << "\n";
            }
            return ostr;
        }

        int program(const std::string& xclbin, unsigned region) {
            std::ifstream stream(xclbin.c_str());

            if(!stream.is_open()) {
              std::cout << "ERROR: Cannot open " << xclbin << ". Check that it exists and is readable." << std::endl;
              return -ENOENT;
            }

            char temp[8];
            stream.read(temp, 8);

            if (std::strncmp(temp, "xclbin0", 8)) {
		if (std::strncmp(temp, "xclbin2", 8))
		    return -EINVAL;
	    }


            stream.seekg(0, stream.end);
            int length = stream.tellg();
            stream.seekg(0, stream.beg);

            char *buffer = new char[length];
            stream.read(buffer, length);
            const xclBin *header = (const xclBin *)buffer;
            int result = xclLockDevice(m_handle);
            if (result)
                return result;
            result = xclLoadXclBin(m_handle, header);
            delete [] buffer;
            return result;
        }
#ifdef _MGMT_
        int boot() {
            return xclBootFPGA(m_handle);
        }

        int reset(unsigned region) {
            const xclResetKind kind = (region == 0xffffffff) ? XCL_RESET_FULL : XCL_RESET_KERNEL;
            return xclResetDevice(m_handle, kind);
        }

        int run(unsigned region, unsigned cu) {
            std::cout << "ERROR: Not implemented\n";
            return -1;
        }

        int fan(unsigned speed) {
            std::cout << "ERROR: Not implemented\n";
            return -1;
        }
#endif
        int dmatest(unsigned long long blockSize) {
            void *buf = 0;
            if (posix_memalign(&buf, 4096, blockSize))
                return -1;
            std::memset(buf, 0, blockSize);

            double bw = m_devinfo.mDDRSize;
            bw /= 0x100000; // Convert to MB

            // Use plain POSIX open/pwrite/close.
            // std::ofstream causes libstdc++ to use AIO with xcldma on CentOS 6.x (but not on Ubuntu 15.10)
//            std::string baseName("/dev/xcldma/xcldma");
            std::string deviceWrite = "/dev/xdma" + std::to_string(m_idx) + "_h2c_0";
            int fd = open(deviceWrite.c_str(), O_WRONLY);
            if (fd < 0) {
                std::cout << "Unable to open device node " << deviceWrite << "\n";
                return -1;
            }
            std::cout << "INFO: Zeroing DDR with " << blockSize/1024 << " KB blocks using DMA channel 0 ...\n";
            size_t count = 0;
            Timer tim;
            for (uint64_t phy = 0; phy < m_devinfo.mDDRSize; phy += blockSize) {
                count += pwrite(fd, (const char *)buf, blockSize, phy);
            }
            double elapsed = tim.stop();
            close(fd);
            bw /= elapsed;
            bw *= 1000000; // Convert from microseconds to seconds
            if (count != m_devinfo.mDDRSize) {
                std::cout << "DMA error\n";
                return -1;
            }
            std::cout << "INFO: Host -> PCIe -> MIG write bandwidth " << bw << " MB/s\n";


            std::string deviceWrite1 = "/dev/xdma" + std::to_string(m_idx) + "_h2c_1";
            bw = m_devinfo.mDDRSize;
            bw /= 0x100000; // Convert to MB

            // Use plain POSIX open/pwrite/close.
            // std::ofstream causes libstdc++ to use AIO with xcldma on CentOS 6.x (but not on Ubuntu 15.10)
            fd = open(deviceWrite1.c_str(), O_WRONLY);
            if (fd < 0) {
              std::cout << "Unable to open device node " << deviceWrite1 << "\n";
                return -1;
            }

            std::cout << "INFO: Zeroing DDR with " << blockSize/1024 << " KB blocks using DMA channel 1 ...\n";
            count = 0;
            tim.reset();
            for (uint64_t phy = 0; phy < m_devinfo.mDDRSize; phy += blockSize) {
                count += pwrite(fd, (const char *)buf, blockSize, phy);
            }
            elapsed = tim.stop();
            close(fd);
            bw /= elapsed;
            bw *= 1000000; // Convert from microseconds to seconds
            if (count != m_devinfo.mDDRSize) {
                std::cout << "DMA error\n";
                return -1;
            }
            std::cout << "INFO: Host -> PCIe -> MIG write bandwidth " << bw << " MB/s\n";

            // Now read bandwidth
            bw = m_devinfo.mDDRSize;
            bw /= 0x100000; // Convert to MB
            std::string deviceRead = "/dev/xdma" + std::to_string(m_idx) + "_c2h_0";
            fd = open(deviceRead.c_str(), O_RDONLY);
            if (fd < 0) {
                std::cout << "Unable to open device node " << deviceRead << "\n";
                return -1;
            }
            std::cout << "INFO: Reading back " << blockSize/1024 << " KB blocks from DDR using DMA channel 0 ...\n";
            count = 0;
            tim.reset();
            for (uint64_t phy = 0; phy < m_devinfo.mDDRSize; phy += blockSize) {
                count += pread(fd, (char *)buf, blockSize, phy);
            }
            elapsed = tim.stop();
            close(fd);
            bw /= elapsed;
            bw *= 1000000; // Convert from microseconds to seconds
            if (count != m_devinfo.mDDRSize) {
                std::cout << "DMA error\n";
                return -1;
            }
            std::cout << "INFO: Host <- PCIe <- MIG read bandwidth " << bw << " MB/s\n";

            std::string deviceRead1 = "/dev/xdma" + std::to_string(m_idx) + "_c2h_1";
            fd = open(deviceRead1.c_str(), O_RDONLY);
            if (fd < 0) {
                std::cout << "Unable to open device node " << deviceRead << "\n";
                return -1;
            }
            bw = m_devinfo.mDDRSize;
            bw /= 0x100000; // Convert to MB


            std::cout << "INFO: Reading back " << blockSize/1024 << " KB blocks from DDR using DMA channel 1 ...\n";
            count = 0;
            tim.reset();
            for (uint64_t phy = 0; phy < m_devinfo.mDDRSize; phy += blockSize) {
                count += pread(fd, (char *)buf, blockSize, phy);
            }
            elapsed = tim.stop();
            close(fd);
            bw /= elapsed;
            bw *= 1000000; // Convert from microseconds to seconds
            if (count != m_devinfo.mDDRSize) {
                std::cout << "DMA error\n";
                return -1;
            }
            std::cout << "INFO: Host <- PCIe <- MIG read bandwidth " << bw << " MB/s\n";

            free(buf);
            return 0;
        }

        int memread(std::string aFilename, unsigned long long aStartAddr = 0, unsigned long long aSize = 0) {
          std::string baseName("/dev/xdma");
          baseName += std::to_string(m_idx);
          return memaccess(baseName, m_devinfo.mDDRSize, m_devinfo.mDataAlignment).read(aFilename, aStartAddr, aSize);
        }

        int memwrite(unsigned long long aStartAddr, unsigned long long aSize, unsigned int aPattern) {
          std::string baseName("/dev/xdma");
          baseName += std::to_string(m_idx);
          return memaccess(baseName, m_devinfo.mDDRSize, m_devinfo.mDataAlignment).write(aStartAddr, aSize, aPattern);
        }
#ifdef _MGMT_
        int readAPMCounters() {
            xclDebugCountersResults debugResults = {0};
            xclDebugReadIPStatus(m_handle, XCL_DEBUG_READ_TYPE_APM, &debugResults);
            std::cout << "APM Counters\n";
            std::cout << "Slot" << std::setw(16) << " Write Bytes" << std::setw(16) << " Write Trans." << std::setw(16) << " Read Bytes" << std::setw(16) << " Read Tranx." << std::endl;
            for (int i = 0; i<XAPM_MAX_NUMBER_SLOTS; ++i) {
              std::cout << "Slot-" << i << std::setw(16) <<debugResults. WriteBytes[i] << std::setw(16) << debugResults.WriteTranx[i];
              std::cout << std::setw(16) << debugResults.ReadBytes[i] << std::setw(16) << debugResults.ReadTranx[i] << std::endl;
            }
            return 0;
        }

        int readLAPCheckers(int aVerbose) {
        	xclDebugCheckersResults debugResults = {0};
            if (getuid() && geteuid()) {
                std::cout << "ERROR: Reading LAPC requires root privileges" << std::endl;
                return -EACCES;
            }
            xclDebugReadIPStatus(m_handle, XCL_DEBUG_READ_TYPE_LAPC, &debugResults);
            bool violations_found = false;
            bool invalid_codes = false;
            std::cout << "Light Weight AXI Protocol Checkers codes \n";
            for (int i = 0; i<XLAPC_MAX_NUMBER_SLOTS; ++i) {
              if (!xclAXICheckerCodes::isValidAXICheckerCodes(debugResults.OverallStatus[i],
                                    debugResults.SnapshotStatus[i], debugResults.CumulativeStatus[i])) {
                std::cout << "Slot-" << i << "\n";
                std::cout << "  Invalid codes read, skip decoding\n";
                invalid_codes = true;
              }
              else if (debugResults.OverallStatus[i]) {
                std::cout << "Slot-" << i << "\n";
                std::cout << "  First violation: \n";
                std::cout << "    " <<  xclAXICheckerCodes::decodeAXICheckerCodes(debugResults.SnapshotStatus[i]);
                //snapshot reflects first violation, cumulative has all violations
                unsigned int tCummStatus[4];
                std::transform(debugResults.CumulativeStatus[i], debugResults.CumulativeStatus[i]+4, debugResults.SnapshotStatus[i], tCummStatus, std::bit_xor<unsigned int>());
                std::cout << "  Other violations: \n";
                std::cout << "    " <<  xclAXICheckerCodes::decodeAXICheckerCodes(tCummStatus);
                violations_found = true;
              }
            }
            if (!violations_found && !invalid_codes) {
              std::cout << "No AXI violations found \n";
            }
            if (violations_found && aVerbose && !invalid_codes) {
              std::cout << "Slot" << std::setw(16) << "Overall Status" << std::setw(16) << "Snapshot0" << std::setw(16) << " Snapshot1" << std::setw(16) << " Snapshot2"  << std::setw(16) << " Snapshot3";
              std::cout <<  std::setw(16) << " Cumulative0" << std::setw(16) << " Cumulative1 " << std::setw(16) << " Cumulative2"  << std::setw(16) << " Cumulative3" << std::endl;
              for (int i = 0; i<XLAPC_MAX_NUMBER_SLOTS; ++i) {
                std::cout << "Slot-" << i << std::setw(16) << debugResults.OverallStatus[i] << std::setw(16) << debugResults.SnapshotStatus[i][0] << std::setw(16) << debugResults.SnapshotStatus[i][1];
                std::cout << std::setw(16) << debugResults.SnapshotStatus[i][2] << std::setw(16) << debugResults.SnapshotStatus[i][3];
                std::cout << std::setw(16) << debugResults.CumulativeStatus[i][0] << std::setw(16) << debugResults.CumulativeStatus[i][1];
                std::cout << std::setw(16) << debugResults.CumulativeStatus[i][2] << std::setw(16) << debugResults.CumulativeStatus[i][3] <<std::endl;
               }
            }
            return 0;
        }
#endif
    };

    static void printHelp(const std::string& exe) {
        std::cout << "Running xbsak for xilinx:huawei-vu9p-fp1:4ddr-xpr:4.1 DSA's \n\n";
        std::cout << "Usage: " << exe << " <command> [options]\n\n";
        std::cout << "Command and option summary:\n";
        std::cout << "  help\n";
        std::cout << "  list\n";
#ifdef _MGMT_
        std::cout << "  boot    [-d device]\n";
        std::cout << "  clock   [-d device] [-r region] [-f clock1_freq_MHz] [-g clock2_freq_MHz]\n";
#endif
        std::cout << "  dmatest [-d device] [-b [0x]block_size_KB]\n";
        std::cout << "  mem     --read [-d device] [-a [0x]start_addr] [-i size_bytes] [-o output filename]\n";
        std::cout << "  mem     --write [-d device] [-a [0x]start_addr] [-i size_bytes] [-e pattern_byte]\n";
#ifdef _MGMT_
        std::cout << "  fan     [-d device] -s speed\n";
        std::cout << "  flash   [-d device] -m primary_mcs [-n secondary_mcs]\n";
        std::cout << "  scan\n";
        std::cout << "  program [-d device] [-r region] -p xclbin\n";
        std::cout << "  query   [-d device [-r region]]\n";
        std::cout << "  reset   [-d device] [-h | -r region]\n";
        std::cout << "  status  --apm\n";
        std::cout << "  status  --lapc\n";
        std::cout << "  run     -d device [-r region] -c compunit\n"; TODO
#endif
        std::cout << "\nExamples:\n";
        std::cout << "List all devices\n";
        std::cout << "  " << exe << " list\n";
#ifdef _MGMT_
        std::cout << "Scan all devices\n";
        std::cout << "  " << exe << " scan\n";
        std::cout << "Boot device 1 from PROM and retrain the PCIe link without rebooting the host\n";
        std::cout << "  " << exe << " boot -d 1\n";
        std::cout << "Change the clock frequency of region 0 in device 0 to 100 MHz\n";
        std::cout << "  " << exe << " clock -f 100\n";
        std::cout << "For device 0 which supports multiple clocks, change the clock 1 to 200MHz and clock 2 to 250MHz\n";
        std::cout << "  " << exe << " clock -f 200 -g 250\n";
        std::cout << "Download the accelerator program for device 2\n";
        std::cout << "  " << exe << " program -d 2 -p a.xclbin\n";
#endif
        std::cout << "Run DMA test on device 0 with 32 KB blocks of buffer\n";
        std::cout << "  " << exe << " dmatest -d 0 -b 0x2000\n";
        std::cout << "Read 256 bytes from DDR starting at 0x1000 on device 0 into file read.out\n";
        std::cout << "  " << exe << " mem --read -d 0 -a 0x1000 -i 256 -o read.out\n";
        std::cout << "  " << "Default values for address is 0x0, size is DDR size and file is memread.out\n";
        std::cout << "Write 256 bytes to DDR starting at 0x1000 with byte 0xaa on device 0 \n";
        std::cout << "  " << exe << " mem --write -d 0 -a 0x1000 -i 256 -e 0xaa\n";
        std::cout << "  " << "Default values for address is 0x0, size is DDR size and pattern is 0x0\n";
#ifdef _MGMT_
        std::cout << "Read AXI Performance Monitor counters on the base platform (applicable only if APMs are available on base platform)\n";
        std::cout << "  " << exe << " status --apm\n";
        std::cout << "Read AXI violation codes detected by Light weight AXI Protocol Checker (applicable only if LAPC IP available on base platform)\n";
        std::cout << "  " << exe << " status --lapc\n";
#endif
    }
}


int main(int argc, char *argv[])
{
    unsigned index = 0xffffffff;
    unsigned regionIndex = 0xffffffff;
#ifdef _MGMT_
    unsigned computeIndex = 0xffffffff;
    unsigned short targetFreq[2] = {0, 0};
    unsigned fanSpeed = 0;
#endif
    unsigned long long startAddr = 0;
    unsigned int pattern_byte = 0;
    size_t sizeInBytes = 0;
    std::string outMemReadFile = "memread.out";
    std::string mcsFile1, mcsFile2;
    std::string xclbin;
    unsigned long long blockSize = 0x20000;
#ifdef _MGMT_
    bool hot = false;
#endif
    int c;

    std::vector<std::unique_ptr<xcldev::device>> deviceVec;


    try {
        unsigned int count = xclProbe();
        if (count == 0) {
            std::cout << "ERROR: No devices found\n";
            return 1;
        }
        //std::cout << " device probe result is : " << count << std::endl;
        for (unsigned i = 0; i < count; i++) {
            xcldev::device *dev_ptr=new xcldev::device(i, nullptr);
            int ret_lock=xclLockDevice(dev_ptr->mpub_handle);

            if(ret_lock){
                std::cout << "ERROR: Lock Device " << i <<" failed\n";
                return -1;
            }
            deviceVec.emplace_back(dev_ptr);
        }
    }
    catch (const std::exception& ex) {
        std::cout << ex.what() << std::endl;
        return 1;
    }

    std::cout << "INFO: Found " << deviceVec.size() << " device(s)\n";


    FPGA_IMG_INFO prloadinfo;

    UINT32 nRet = FPGA_MgmtInit();
    if (nRet)
    {
        std::cout<<"FPGA_MgmtInit : error: Fpga mgmt tools initial failed!"<<std::endl;
        std::cout<<"FPGA_MgmtInit : errno is "<<nRet<<std::endl;
        return -1;
    }

    std::cout<<"Fpga image checking..."<<std::endl;
    std::memset(&prloadinfo,0,sizeof(tagFPGA_IMG_INFO));
    nRet =  FPGA_MgmtInquireFpgaImageInfo( 0, &prloadinfo);
    if (nRet)
    {
        std::cout<<"ERROR : Inquire Fpga image info failed!"<<std::endl;
        std::cout<<"ERROR : CmdOpsStatus is :"<< prloadinfo.ulCmdOpsStatus <<std::endl;
        return -1;
    }

    UINT32 infoopsstatus = (prloadinfo.ulCmdOpsStatus & 0xffff0000) >> 16;
    if ((infoopsstatus != FPGA_OPS_STATUS_SUCCESS) || (prloadinfo.ulFpgaPrStatus != FPGA_PR_STATUS_PROGRAMMED))
    {
        std::cout<<"ERROR : Fpga image status error, check or reload xclbin!"<<std::endl;
        return -1;
    }
    
    std::cout<<"Fpga image check OK!"<<std::endl;

    INT32 prlock = 0;
    if (FPGA_MgmtOpsMutexRlock(0,&prlock))
    {
        std::cout << "ERROR: get pr_lock statue failed!\n";
        std::cout << "pr_lock value is :"<<prlock<<std::endl; 
        return -1;
    }
    
    const std::string exe(argv[0]);
    if (argc == 1) {
        xcldev::printHelp(exe);
        return 1;
    }

    argv++;
    const auto v = xcldev::commandTable.find(*argv);
    if (v == xcldev::commandTable.end()) {
        std::cout << "ERROR: Unknown comand \'" << *argv << "\'\n";
        xcldev::printHelp(exe);
        return 1;
    }

    const xcldev::command cmd = v->second;
    std::string cmdname = v->first;
    xcldev::subcommand subcmd = xcldev::MEM_READ;
#ifdef _MGMT_
    unsigned int ipmask = static_cast<unsigned int>(xcldev::STATUS_NONE_MASK);
#endif
    argc--;

    if (cmd == xcldev::HELP) {
        xcldev::printHelp(exe);
        return 1;
    }

    argv[0] = const_cast<char *>(exe.c_str());
    static struct option long_options[] = {
      {"read", no_argument, 0, xcldev::MEM_READ},
      {"write", no_argument, 0, xcldev::MEM_WRITE},
      {"apm", no_argument, 0, xcldev::STATUS_APM},
	  {"lapc", no_argument, 0, xcldev::STATUS_LAPC}
    };
    int long_index;
    const char* short_options = "a:d:e:i:r:p:f:g:m:n:c:s:b:ho:"; //don't add numbers
    while ((c = getopt_long(argc, argv, short_options, long_options, &long_index)) != -1)
    {
        if (cmd == xcldev::LIST) {
            std::cout << "ERROR: 'list' command does not accept any options\n";
            return -1;
        }
        switch (c)
        {
        //Deal with long options. Long options return the value set in option::val
        case xcldev::MEM_READ : {
          //--read
          if (cmd != xcldev::MEM) {
            std::cout << "ERROR: Option '" << long_options[long_index].name << "' cannot be used with command " << cmdname << "\n";
            return -1;
          }
          subcmd = xcldev::MEM_READ;
          break;
        }
        case xcldev::MEM_WRITE : {
          //--write
          if (cmd != xcldev::MEM) {
            std::cout << "ERROR: Option '" << long_options[long_index].name << "' cannot be used with command " << cmdname << "\n";
            return -1;
          }
          subcmd = xcldev::MEM_WRITE;
          break;
        }
#ifdef _MGMT_
        case xcldev::STATUS_APM : {
          //--apm
          if (cmd != xcldev::STATUS) {
            std::cout << "ERROR: Option '" << long_options[long_index].name << "' cannot be used with command " << cmdname << "\n";
            return -1;
          }
          ipmask |= static_cast<unsigned int>(xcldev::STATUS_APM_MASK);
          break;
        }
        case xcldev::STATUS_LAPC : {
          //--lapc
          if (cmd != xcldev::STATUS) {
            std::cout << "ERROR: Option '" << long_options[long_index].name << "' cannot be used with command " << cmdname << "\n";
            return -1;
          }
          ipmask |= static_cast<unsigned int>(xcldev::STATUS_LAPC_MASK);
          break;
        }
#endif
        //short options are dealt here
        case 'a':
            if (cmd != xcldev::MEM) {
                std::cout << "ERROR: '-a' not applicable for this command\n";
                return -1;
            }
            startAddr = std::stoll(optarg, nullptr, 0);
            break;
        case 'o':
            if (cmd != xcldev::MEM || subcmd != xcldev::MEM_READ) {
                std::cout << "ERROR: '-o' not applicable for this command\n";
                return -1;
            }
            outMemReadFile = optarg;
            break;
        case 'e':
            if (cmd != xcldev::MEM || subcmd != xcldev::MEM_WRITE) {
                std::cout << "ERROR: '-e' not applicable for this command\n";
                return -1;
            }
            pattern_byte = std::stoi(optarg, nullptr, 0);
            break;
        case 'i':
            if (cmd != xcldev::MEM) {
                std::cout << "ERROR: '-i' not applicable for this command\n";
                return -1;
            }
            sizeInBytes = std::stoll(optarg, nullptr, 0);
            break;
        case 'd':
            index = std::atoi(optarg);
            break;
#ifdef _MGMT_
        case 'r':
            if ((cmd == xcldev::FLASH) || (cmd == xcldev::BOOT) || (cmd == xcldev::DMATEST) ||(cmd == xcldev::STATUS)) {
                std::cout << "ERROR: '-r' not applicable for this command\n";
                return -1;
            }
            regionIndex = std::atoi(optarg);
            break;
        case 'p':
            if (cmd != xcldev::PROGRAM) {
                std::cout << "ERROR: '-p' only allowed with 'program' command\n";
                return -1;
            }
            xclbin = optarg;
            break;
        case 'f':
            if (cmd != xcldev::CLOCK) {
                std::cout << "ERROR: '-f' only allowed with 'clock' command\n";
                return -1;
            }
            targetFreq[0] = std::atoi(optarg);
            break;
        case 'g':
            if (cmd != xcldev::CLOCK) {
                std::cout << "ERROR: '-g' only allowed with 'clock' command\n";
                return -1;
            }
            targetFreq[1] = std::atoi(optarg);
            break;
        case 'm':
            if (cmd != xcldev::FLASH) {
                std::cout << "ERROR: '-m' only allowed with 'flash' command\n";
                return -1;
            }
            mcsFile1 = optarg;
            break;
        case 'n':
            if (cmd != xcldev::FLASH) {
                std::cout << "ERROR: '-n' only allowed with 'flash' command\n";
                return -1;
            }
            mcsFile2 = optarg;
            break;
        case 'c':
            if (cmd != xcldev::RUN) {
                std::cout << "ERROR: '-c' only allowed with 'run' command\n";
                return -1;
            }
            computeIndex = std::atoi(optarg);
            break;
        case 's':
            if (cmd != xcldev::FAN) {
                std::cout << "ERROR: '-s' only allowed with 'fan' command\n";
                return -1;
            }
            fanSpeed = std::atoi(optarg);
            break;
#endif
        case 'b':
        {
            if (cmd != xcldev::DMATEST) {
                std::cout << "ERROR: '-b' only allowed with 'dmatest' command\n";
                return -1;
            }
            std::string tmp(optarg);
            if ((tmp[0] == '0') && (std::tolower(tmp[1]) == 'x')) {
                blockSize = std::stoll(tmp, 0, 16);
            }
            else {
                blockSize = std::stoll(tmp, 0, 10);
            }

            if (blockSize & (blockSize - 1)) {
                std::cout << "ERROR: block size should be power of 2\n";
                return -1;
            }
            blockSize *= 1024; // convert kilo bytes to bytes
            break;
        }
#ifdef _MGMT_
        case 'h':
        {
            if (cmd != xcldev::RESET) {
                std::cout << "ERROR: '-h' only allowed with 'reset' command\n";
                return -1;
            }
            hot = true;
            break;
        }
#endif
        default:
            xcldev::printHelp(exe);
            return 1;
        }
    }

    if (optind != argc) {
        std::cout << "ERROR: Illegal command \'" << argv[optind++] << "\'\n";
        return -1;
    }

    if (index == 0xffffffff) index = 0;

    if (regionIndex == 0xffffffff) regionIndex = 0;

    switch (cmd) {
#ifdef _MGMT_
    case xcldev::FLASH:
    {
        if (mcsFile1.size() == 0) {
            std::cout << "ERROR: Please specify mcs file with '-m' switch\n";
            return -1;
        }
        break;
    }
    case xcldev::BOOT:
    case xcldev::RUN:
    case xcldev::FAN:
#endif
    case xcldev::DMATEST:
    case xcldev::MEM:
#ifdef _MGMT_
    case xcldev::QUERY:
    case xcldev::SCAN:
    case xcldev::STATUS:
#endif
        break;
#ifdef _MGMT_
    case xcldev::PROGRAM:
    {
        if (xclbin.size() == 0) {
            std::cout << "ERROR: Please specify xclbin file with '-p' switch\n";
            return -1;
        }
        break;
    }
    case xcldev::CLOCK:
    {
        if (!targetFreq[0] && !targetFreq[1]) {
            std::cout << "ERROR: Please specify frequency(ies) with '-f' and or '-g' switch(es)\n";
            return -1;
        }
        break;
    }
#endif
    default:
        break;
    }

#ifdef _MGMT_
    if (cmd == xcldev::SCAN) {
	xcldev::pci_devices devices;
	return devices.scan();
    }
#endif

    if (cmd == xcldev::LIST) {
        for (unsigned i = 0; i < deviceVec.size(); i++) {
            std::cout << '[' << i << "] " << deviceVec[i]->name() << std::endl;
        }
        return 0;
    }

    if (index >= deviceVec.size()) {
        std::cout << "ERROR: Device index " << index << " out of range\n";
        return 1;
    }

    int result = 0;

    switch (cmd)
    {
#ifdef _MGMT_
    case xcldev::BOOT:
        result = deviceVec[index]->boot();
        break;
    case xcldev::CLOCK:
        result = deviceVec[index]->reclock2(regionIndex, targetFreq);
        break;
    case xcldev::FAN:
        result = deviceVec[index]->fan(fanSpeed);
        break;
    case xcldev::FLASH:
        result = deviceVec[index]->flash(mcsFile1, mcsFile2);
        break;
    case xcldev::PROGRAM:
        result = deviceVec[index]->program(xclbin, regionIndex);
        break;
    case xcldev::QUERY:
        deviceVec[index]->dump(std::cout);
        break;
    case xcldev::RESET:
        if (hot) regionIndex = 0xffffffff;
        result = deviceVec[index]->reset(regionIndex);
        break;
    case xcldev::RUN:
        result = deviceVec[index]->run(regionIndex, computeIndex);
        break;
#endif
    case xcldev::DMATEST:
        result = deviceVec[index]->dmatest(blockSize);
        break;
    case xcldev::MEM:
        if (subcmd == xcldev::MEM_READ) {
          result = deviceVec[index]->memread(outMemReadFile, startAddr, sizeInBytes);
        }
        else if (subcmd == xcldev::MEM_WRITE) {
          result = deviceVec[index]->memwrite(startAddr, sizeInBytes, pattern_byte);
        }
        break;
#ifdef _MGMT_
    case xcldev::STATUS:
        if (ipmask == xcldev::STATUS_NONE_MASK) {
           //if no ip specified then read all
           ipmask = static_cast<unsigned int>(xcldev::STATUS_APM_MASK);
            if (!(getuid() && geteuid())) {
              ipmask |= static_cast<unsigned int>(xcldev::STATUS_LAPC_MASK);
            }
        }
        if (ipmask & static_cast<unsigned int>(xcldev::STATUS_APM_MASK)) {
          result = deviceVec[index]->readAPMCounters();
        }
        if (ipmask & static_cast<unsigned int>(xcldev::STATUS_LAPC_MASK)) {
          result = deviceVec[index]->readLAPCheckers(1);
        }
	break;
#endif
    default:
        std::cout << "ERROR: Not implemented\n";
        result = -1;
    }

    if(result == 0) {
        std::cout << "INFO: xbsak " << v->first << " successful." << std::endl;
    } else {
        std::cout << "ERROR: xbsak " << v->first  << " failed." << std::endl;
    }

    if (FPGA_MgmtOpsMutexUnlock(prlock))
    {
        std::cout << "ERROR: Unlock pr_lock statue failed!\n";
        return -1;
    }
    
    return result;
}
