/**
 * Copyright (C) 2015-2016 Xilinx, Inc
 * In-System Programming of BPI PROM using PCIe
 * Based on XAPP518 (v1.3) April 23, 2014
 * Author: Sonal Santan
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

#include <iostream>
#include <string>
#include <list>
#include <fstream>
#include <cassert>
#include <thread>
#include <cstring>

#include <sys/ioctl.h>

#include "shim.h"
#include "cdev_ctrl.h"

#ifdef _MGMT_
#include "mgmt-ioctl.h"
#endif

#ifdef WINDOWS
#define __func__ __FUNCTION__
#endif

namespace xclxdma {
    int XDMAShim::freezeAXIGate() {
#ifndef _MGMT_

	return -1;
#else
		
        if (mLogStream.is_open()) {
            mLogStream << __func__ << ", " << std::this_thread::get_id() << std::endl;
        }
        unsigned char buf = 0x0;
        return pcieBarWrite(SHIM_MGMT_BAR, AXI_GATE_OFFSET, &buf, 1);
#endif		
    }

    int XDMAShim::freeAXIGate() {
        if (mLogStream.is_open()) {
            mLogStream << __func__ << ", " << std::this_thread::get_id() << std::endl;
        }

#ifndef _MGMT_

	return -1;
#else


        // First pulse the OCL RESET. This is important for PR with multiple
        // clocks as it resets the edge triggered clock converter FIFO
#ifndef _WINDOWS
        const timespec interval = {0, 500};
#endif
        unsigned char buf = 0x2;
        if (pcieBarWrite(SHIM_MGMT_BAR, AXI_GATE_OFFSET, &buf, 1))
            return -1;
        buf = 0x0;
#ifndef _WINDOWS
// TODO: Windows build support
//  nanosleep is defined in unistd.h
        nanosleep(&interval, 0);
#endif
        if (pcieBarWrite(SHIM_MGMT_BAR, AXI_GATE_OFFSET, &buf, 1))
            return -1;
        buf = 0x2;
#ifndef _WINDOWS
// TODO: Windows build support
//  nanosleep is defined in unistd.h
        nanosleep(&interval, 0);
#endif
        if (pcieBarWrite(SHIM_MGMT_BAR, AXI_GATE_OFFSET, &buf, 1))
            return -1;
        buf = 0x3;
#ifndef _WINDOWS
// TODO: Windows build support
//    nanosleep is defined in unistd.h
        nanosleep(&interval, 0);
#endif
        return pcieBarWrite(SHIM_MGMT_BAR, AXI_GATE_OFFSET, &buf, 1);
#endif
		
    }


    int XDMAShim::xclUpgradeFirmware(const char *mcsFile) {
#ifndef _MGMT_

	return -1;
#else

		
        if (mLogStream.is_open()) {
            mLogStream << __func__ << ", " << std::this_thread::get_id() << ", " << mcsFile << std::endl;
        }

        if (!mMgtMap)
            return -EACCES;

        std::cout << "INFO: Reseting hardware\n";
        if (freezeAXIGate() != 0) {
            return -1;
        }

#ifndef _WINDOWS
// TODO: Windows build support
//  timespec
        const timespec req = {0, 5000};
        nanosleep(&req, 0);
#endif
        if (freeAXIGate() != 0) {
            return -1;
        }
#ifndef _WINDOWS
// TODO: Windows build support
//  nanosleep is defined in unistd.h
        nanosleep(&req, 0);
#endif

        std::string line;
        std::ifstream mcsStream(mcsFile);
        std::string startAddress;
        ELARecord rd;
        bool endRecordFound = false;

        if(!mcsStream.is_open()) {
          std::cout << "ERROR: Cannot open " << mcsFile << ". Check that it exists and is readable." << std::endl;
          return -ENOENT;
        }

        std::cout << "INFO: Parsing file " << mcsFile << std::endl;
        while (!mcsStream.eof() && !endRecordFound) {
            std::string line;
            std::getline(mcsStream, line);
            if (line.size() == 0) {
                continue;
            }
            if (line[0] != ':') {
                return -1;
            }
            const unsigned dataLen = std::stoi(line.substr(1, 2), 0 , 16);
            const unsigned address = std::stoi(line.substr(3, 4), 0, 16);
            const unsigned recordType = std::stoi(line.substr(7, 2), 0 , 16);
            switch (recordType) {
            case 0x00:
            {
                if (dataLen > 16) {
                    // For xilinx mcs files data length should be 16 for all rds
                    // except for the last one which can be smaller
                    return -1;
                }
                if (address != rd.mDataCount) {
                    return -1;
                }
                if (rd.mEndAddress != address) {
                    return -1;
                }
                rd.mDataCount += dataLen;
                rd.mEndAddress += dataLen;
                break;
            }
            case 0x01:
            {
                if (startAddress.size() == 0) {
                    break;
                }
                mRecordList.push_back(rd);
                endRecordFound = true;
                break;
            }
            case 0x02:
            {
                break;
            }
            case 0x04:
            {
                if (address != 0x0) {
                    return -1;
                }
                if (dataLen != 2) {
                    return -1;
                }
                std::string newAddress = line.substr(9, dataLen * 2);
                if (startAddress.size()) {
                    // Finish the old rd
                    mRecordList.push_back(rd);
                }
                // Start a new rd
                rd.mStartAddress = std::stoi(newAddress, 0 , 16);
                rd.mDataPos = mcsStream.tellg();
                rd.mEndAddress = 0;
                rd.mDataCount = 0;
                startAddress = newAddress;
            }
            }
        }

        mcsStream.seekg(0);
        std::cout << "INFO: Found " << mRecordList.size() << " ELA Records\n";

        return program(mcsStream);
		
#endif
		
    }

    int XDMAShim::prepare(unsigned startAddress, unsigned endAddress) {

#ifndef _MGMT_
	return -1;
#else

	//Send the "hi" msb address command
	std::cout << "sending hi cmd" << std::endl;
        unsigned addrHi = startAddress;
        addrHi = (addrHi >>= 24) & 0xF;
        unsigned cmdHi = START_ADDR_HI_CMD;
        cmdHi |= addrHi;
        if (pcieBarWrite(SHIM_MGMT_BAR, BPI_FLASH_OFFSET, &cmdHi, 4)) {
            return -1;
        }
	std::cout << "done sending hi cmd " << std::hex << cmdHi << std::dec<< std::endl;
	//End sending "hi" msb address command
        startAddress &= 0x00ffffff; // truncate to 24 bits
        startAddress >>= 8; // Pick the middle 16 bits
        endAddress &= 0x00ffffff; // truncate to 24 bits

        if (waitForReady(READY_STAT)) {
            return -1;
        }

        std::cout << "INFO: Sending the address range\n";
        // Send start and end address
        unsigned command = START_ADDR_CMD;
        command |= startAddress;
        if (pcieBarWrite(SHIM_MGMT_BAR, BPI_FLASH_OFFSET, &command, 4)) {
            return -1;
        }

        command = END_ADDR_CMD;
        command |= endAddress;
        if (pcieBarWrite(SHIM_MGMT_BAR, BPI_FLASH_OFFSET, &command, 4)) {
            return -1;
        }

        std::cout << "INFO: Sending unlock command\n";
        // Send unlock command
        command = UNLOCK_CMD;
        if (pcieBarWrite(SHIM_MGMT_BAR, BPI_FLASH_OFFSET, &command, 4)) {
            return -1;
        }
        if (waitForReady(READY_STAT)) {
            return -1;
        }

        // Send erase command
        std::cout << "INFO: Sending erase command\n";
        command = ERASE_CMD;
        if (pcieBarWrite(SHIM_MGMT_BAR, BPI_FLASH_OFFSET, &command, 4)) {
            return -1;
        }
        // now hanging here
        if (waitForReady(ERASE_STAT)) {
            return -1;
        }

        if (waitForReady(READY_STAT)) {
            return -1;
        }

        // Send program command
        std::cout << "INFO: Erasing the address range\n";
        command = PROGRAM_CMD;
        if (pcieBarWrite(SHIM_MGMT_BAR, BPI_FLASH_OFFSET, &command, 4)) {
            return -1;
        }

        if (waitForReady(PROGRAM_STAT)) {
            return -1;
        }

        return 0;
#endif
    }

    int XDMAShim::program(std::ifstream& mcsStream, const ELARecord& rd) {

#ifndef _MGMT_

	return -1;
#else

		
        if (mLogStream.is_open()) {
            mLogStream << __func__ << ", " << std::this_thread::get_id() << std::endl;
        }
#ifndef _WINDOWS
// TODO: Windows build support
//  timespec
        const timespec req = {0, 2000};
#endif

        std::cout << "Programming block (" << std::hex << rd.mStartAddress << ", " << rd.mEndAddress << std::dec << ")" << std::endl;
        assert(mcsStream.tellg() < rd.mDataPos);
        mcsStream.seekg(rd.mDataPos, std::ifstream::beg);
        unsigned char buffer[64];
        int bufferIndex = 0;
        for (unsigned index = rd.mDataCount; index > 0;) {
            std::string line;
            std::getline(mcsStream, line);
            const unsigned dataLen = std::stoi(line.substr(1, 2), 0 , 16);
            index -= dataLen;
            const unsigned recordType = std::stoi(line.substr(7, 2), 0 , 16);
            if (recordType != 0x00) {
                continue;
            }
            const std::string data = line.substr(9, dataLen * 2);
            // Write in byte swapped order
            for (unsigned i = 0; i < data.length(); i += 2) {
                if ((bufferIndex % 4) == 0) {
                    bufferIndex += 4;
                }
                assert(bufferIndex <= 64);
                unsigned value = std::stoi(data.substr(i, 2), 0, 16);
                buffer[--bufferIndex] = (unsigned char)value;
                if ((bufferIndex % 4) == 0) {
                    bufferIndex += 4;
                }
                if (bufferIndex == 64) {
                    break;
                }
            }

            assert((bufferIndex % 4) == 0);
            assert(bufferIndex <= 64);
            if (bufferIndex == 64) {
                if (waitForReady(PROGRAM_STAT, false)) {
                    return -1;
                }
                if (pcieBarWrite(SHIM_MGMT_BAR, BPI_FLASH_OFFSET, buffer, 64)) {
                    return -1;
                }
                if (waitForReady(PROGRAM_STAT, false)) {
                    return -1;
                }
#ifndef _WINDOWS
// TODO: Windows build support
//   nanosleep is defined in unistd.h
                nanosleep(&req, 0);
#endif
                bufferIndex = 0;
            }
        }
        if (bufferIndex) {
            if (waitForReady(PROGRAM_STAT, false)) {
                return -1;
            }
            if (pcieBarWrite(SHIM_MGMT_BAR, BPI_FLASH_OFFSET, buffer, bufferIndex)) {
                return -1;
            }
            if (waitForReady(PROGRAM_STAT, false)) {
                return -1;
            }
#ifndef _WINDOWS
// TODO: Windows build support
//   nanosleep is defined in unistd.h
            nanosleep(&req, 0);
#endif
        }
        return 0;

#endif		
    }

    int XDMAShim::program(std::ifstream& mcsStream) {
#ifndef _MGMT_

	return -1;
#else
		
        int status = 0;
        for (ELARecordList::iterator i = mRecordList.begin(), e = mRecordList.end(); i != e; ++i) {
            i->mStartAddress <<= 16;
            i->mEndAddress += i->mStartAddress;
            // Convert from 2 bytes address to 4 bytes address
            i->mStartAddress /= 2;
            i->mEndAddress /= 2;
        }
        std::cout << "INFO: Start address 0x" << std::hex << mRecordList.front().mStartAddress << std::dec << "\n";
        std::cout << "INFO: End address 0x" << std::hex << mRecordList.back().mEndAddress << std::dec << "\n";
        if (prepare(mRecordList.front().mStartAddress, mRecordList.back().mEndAddress)) {
            std::cout << "ERROR: Could not unlock or erase the blocks\n";
            return -1;
        }
#ifndef _WINDOWS
// TODO: Windows build support
//  timespec
        const timespec req = {0, 1000};
#endif
	int beatCount = 0;
        for (ELARecordList::iterator i = mRecordList.begin(), e = mRecordList.end(); i != e; ++i)
	{
	    beatCount++;
	    if(beatCount%10==0) {
		std::cout << "." << std::flush;
	    }

            if (program(mcsStream, *i)) {
                std::cout << "ERROR: Could not program the block\n";
                return -1;
            }
#ifndef _WINDOWS
// TODO: Windows build support
//   nanosleep is defined in unistd.h
            nanosleep(&req, 0);
#endif
        }
	std::cout << std::endl;
        // Now keep writing 0xff till the hardware says ready
        if (waitAndFinish(READY_STAT, 0xff)) {
            return -1;
        }	
        return status;
#endif
    }

    int XDMAShim::waitForReady(unsigned code, bool verbose) {
#ifndef _MGMT_

	return -1;
#else
		
        unsigned status = ~code;
        long long delay = 0;
#ifndef _WINDOWS
// TODO: Windows build support
//  timespec
        const timespec req = {0, 5000};
#endif
        if (verbose) {
            std::cout << "INFO: Waiting for hardware\n";
        }
        while ((status != code) && (delay < 30000000000)) {
#ifndef _WINDOWS
// TODO: Windows build support
//    nanosleep is defined in unistd.h
            nanosleep(&req, 0);
#endif
            if (pcieBarRead(SHIM_MGMT_BAR, BPI_FLASH_OFFSET, &status, 4)) {
                return -1;
            }
            delay += 5000;
        }
        return (status == code) ? 0 : -1;
#endif
    }

    int XDMAShim::waitAndFinish(unsigned code, unsigned data, bool verbose) {
        unsigned status = ~code;
        long long delay = 0;
#ifndef _MGMT_
    return -1;
#else
#ifndef _WINDOWS
// TODO: Windows build support
//  timespec
        const timespec req = {0, 5000};
#endif
        if (verbose) {
            std::cout << "INFO: Finishing up\n";
        }
        if (pcieBarRead(SHIM_MGMT_BAR, BPI_FLASH_OFFSET, &status, 4)) {
            return -1;
        }
        while ((status != code) && (delay < 30000000000)) {
#ifndef _WINDOWS
// TODO: Windows build support
//    nanosleep is defined in unistd.h
            nanosleep(&req, 0);
#endif
            if (pcieBarWrite(SHIM_MGMT_BAR, BPI_FLASH_OFFSET, &data, 4)) {
                return -1;
            }
            if (pcieBarRead(SHIM_MGMT_BAR, BPI_FLASH_OFFSET, &status, 4)) {
                return -1;
            }
            delay += 5000;
        }
        return (status == code) ? 0 : -1;
#endif    
}

    int XDMAShim::xclBootFPGA() {
#ifdef _MGMT_		
        xdma_ioc_base base = {0X586C0C6C, XCLMGMT_IOC_REBOOT};
        return ioctl(mMgtHandle, XCLMGMT_IOC_REBOOT, &base);
#else
	// do samthing
	std::cout<<"ERROR: xclBootFPGA is not supported in this version!\n";
	return -1;
#endif
    }
}
