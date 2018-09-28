/**
 * Copyright (C) 2016-2018 Xilinx, Inc
 * Author: Hem Neema
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
#ifndef _XCL_SCAN_H_
#define _XCL_SCAN_H_

#include <iostream>
#include <cassert>
#include <fstream>
#include <dirent.h>
#include <string>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <vector>
#include <algorithm>
#include <sys/utsname.h>
#include <cstdlib>
#include <gnu/libc-version.h>


//TODO: can get this from config.h : PCI_PATH_SYS_BUS_PCI
#define ROOT_DIR "/sys/bus/pci"
#define XILINX_ID 0x10ee
#define ADVANTECH_ID 0x13fe
#define OBJ_BUF_SIZE 1024
#define DRIVER_BUF_SIZE 1024
#define INVALID_DEV 0xffff
#define HUAWEI_ID 0x19e5

namespace xcldev {

    
/*
 * get_val_string
 *
 * Given a directory, get the value in a given key.
 * Returns the value as string.
 */
std::string get_val_string(std::string& dir, const char* key);

/*
 * get_val_int
 *
 * Given a directory, get the value in a given key.
 * Returns the value as long int.
 */
int get_val_int(std::string& dir, const char* key);

/*
 * get_render_value
 */
int get_render_value(std::string& dir);

class pci_device_scanner {

public:
    struct device_info {
	unsigned user_instance;
	unsigned mgmt_instance;
	std::string user_name;
	std::string mgmt_name;
	size_t user_bar0_size;
	size_t mgmt_bar0_size;
    };
    
    // userpf instance, mgmt instance, device
    static std::vector<struct device_info> device_list;
private:
    struct pci_device {
        int domain;
        uint8_t bus, dev, func;
        uint16_t vendor_id = 0, device_id = 0, subsystem_id = 0;
        uint16_t instance = INVALID_DEV;
        std::string device_name;
        std::string driver_name = "???", driver_version = "???";
        size_t bar0_size;
    };

    bool add_device(struct pci_device& device) {
        if ( device.func == 1) {
            mgmt_devices.emplace_back(device);
        } else if ( device.func == 0) {
            user_devices.emplace_back(device);
        } else {
            assert(0);
            return false;
        }
        return true;
    }

    bool print_paths() {
        std::cout << "XILINX_OPENCL=\"";
        if ( const char* opencl = std::getenv("XILINX_OPENCL")) {
            std::cout << opencl << "\"";
        } else {
            std::cout << "\"";
        }
        std::cout << std::endl;

        std::cout << "LD_LIBRARY_PATH=\"";
        if ( const char* ld = std::getenv("LD_LIBRARY_PATH")) {
            std::cout << ld << "\"";
        } else {
            std::cout << "\"";
        }
        std::cout << std::endl;
        return true;
    }

    /*
     * Print Linux release and distribution.
     */
    bool print_system_info() {
        struct utsname sysinfo;
        if ( uname(&sysinfo) < 0) {
            return false;
        }
        std::cout << sysinfo.sysname << ":" << sysinfo.release << ":" << sysinfo.version << ":" << sysinfo.machine << std::endl;

        // print linux distribution name and version from /etc/os-release file
        std::ifstream ifs;
        bool found = false;
        std::string distro;
        ifs.open( "/etc/system-release", std::ifstream::binary );
        if( ifs.good() ) { // confirmed for RHEL/CentOS
            std::getline( ifs, distro );
            found = true;
        } else { // confirmed for Ubuntu
            ifs.open( "/etc/lsb-release", std::ifstream::binary );
            if( ifs.good() ) {
                std::string readString;
                while( std::getline( ifs, readString ) && !found ) {
                    if( readString.find( "DISTRIB_DESCRIPTION=" ) == 0 ) {
                        distro = readString.substr( readString.find("=")+2, readString.length() ); // +2 excludes equals and quotes (2 chars)
                        distro = distro.substr( 0, distro.length()-1 ); // exclude the final quotes char
                        found = true;
                    }
                }
            }
        }

        if( found ) {
            std::cout << "Distribution: " << distro << std::endl;
        } else {
            std::cout << "Unable to find OS distribution and version." << std::endl;
        }

        std::cout << "GLIBC: " << gnu_get_libc_version() << std::endl;
        return true;
    }

    bool print_pci_info() {
        auto print = [](struct pci_device& dev) {
            std::cout << std::hex << dev.device_id << ":0x" << dev.subsystem_id << ":[" << std::dec;
            if(!dev.driver_name.empty()) {
                if( dev.instance == INVALID_DEV ) {
                    std::cout << dev.driver_name << ":" << dev.driver_version << ":" << "???" << "]" << std::endl;
                } else {
                    std::cout << dev.driver_name << ":" << dev.driver_version << ":" << dev.instance << "]" << std::endl;
                }
            } else {
                std::cout << "]" << std::endl;
            }
        };

        int i = 0;
        for (auto mdev : mgmt_devices) {
            std::cout << "[" << i << "]" << "mgmt:0x";
            print(mdev);
            for (auto udev : user_devices) {
                if ( (mdev.domain == udev.domain) &&
                        (mdev.bus == udev.bus) &&
                        (mdev.dev == udev.dev) &&
                        (mdev.func == (udev.func + 1)) )
                {
                    std::cout << "[" << i << "]" << "user:0x";
                    print(udev);
                }
            }
            ++i;
        }

        return true;
    }

    void add_to_device_list() {
        for (auto &udev : user_devices) {
            struct device_info temp = {udev.instance, udev.instance,
                                       udev.device_name, udev.device_name,
                                       udev.bar0_size, udev.bar0_size};
            if( (temp.user_instance != INVALID_DEV)) {
                device_list.emplace_back(temp);
            }
        }
    }

    size_t bar_size(const std::string &dir, unsigned bar) const {
        std::ifstream ifs(dir + "/resource");
        if (!ifs.good())
            return 0;
        std::string line;
        for (unsigned i = 0; i <= bar; i++) {
            line.clear();
            std::getline(ifs, line);
        }
        long long start, end, meta;
        if (sscanf(line.c_str(), "0x%llx 0x%llx 0x%llx", &start, &end, &meta) != 3)
            return 0;
        return end - start;
    }

private:
    std::vector<pci_device> mgmt_devices;
    std::vector<pci_device> user_devices;

public:
    int scan(bool print) {
        // need to clear the following lists: mgmt_devices, user_devices, xcldev::device_list
        mgmt_devices.clear();
        user_devices.clear();
        device_list.clear();
        
        if ( !print_system_info() ) {
            std::cout << "Unable to determine system info " << std::endl;
        }
        std::cout << "--- " << std::endl;
        if ( !print_paths() ) {
            std::cout << "Unable to determine PATH/LD_LIBRARY_PATH info " << std::endl;
        }
        std::cout << "--- " << std::endl;

        std::string dirname;
        DIR *dir;
        struct dirent *entry;
        unsigned int dom, bus, dev, func;

        dirname = ROOT_DIR;
        dirname += "/devices/";
        dir = opendir(dirname.c_str());
        if ( !dir ) {
            std::cout << "Cannot open " << dirname << std::endl;
            return -1;
        }

        while ( ( entry = readdir(dir) ) ) {
            if ( entry->d_name[0] == '.' ) {
                continue;
            }

            if ( sscanf(entry->d_name, "%x:%x:%x.%d", &dom, &bus, &dev, &func) < 4 ) {
                std::cout << "scan: Couldn't parse entry name " << entry->d_name << std::endl;
            }

            std::string subdir = dirname + entry->d_name;
            std::string subdir2 = dirname + entry->d_name;

            pci_device device; // generic pci device
            device.domain = dom;
            device.bus = bus;
            device.dev = dev;
            device.func = func;
            device.device_name = entry->d_name;
            device.vendor_id = get_val_int(subdir, "vendor");
            if ( ( device.vendor_id != XILINX_ID ) && ( device.vendor_id != ADVANTECH_ID ) && ( device.vendor_id != HUAWEI_ID)) {
                continue;
            }

            // Xilinx device from here
            device.device_id = get_val_int(subdir, "device");
            device.subsystem_id = get_val_int(subdir, "subsystem_device");
            device.bar0_size = bar_size(subdir, 0);

            //Get the driver name.
            char driverName[DRIVER_BUF_SIZE];
            memset(driverName, 0, DRIVER_BUF_SIZE);
            subdir += "/driver";
            int err = readlink(subdir.c_str(), driverName, DRIVER_BUF_SIZE);
            if ( err < 0 ) {
                add_device(device); // add device even if it is incomplete
                continue;
            }
            if ( err >= DRIVER_BUF_SIZE ) {
                std::cout << "Driver name is too big " << std::endl;
                closedir( dir );
                return -1;
            }

            driverName[err] = '\0'; //Address coverity.

            device.driver_name = driverName;
            size_t found = device.driver_name.find_last_of("/");
            if ( found != std::string::npos ) {
                device.driver_name = device.driver_name.substr(found + 1);
            }

            //Get driver version
            subdir += "/module/";
            std::string version = get_val_string(subdir, "version");
            version.erase(std::remove(version.begin(), version.end(), '\n'), version.end());
            device.driver_version = version;

            if ( device.func == 1 ) {
                device.instance = get_val_int(subdir2, "instance");
            } else {
                std::string drm_dir = subdir2;
                drm_dir += "/drm";
                device.instance = get_render_value(drm_dir);
            }

            if ( !add_device(device) )
            {
                closedir(dir);
                return -1;
            }
        }
        add_to_device_list();

        closedir(dir);

        if ( print ) {
            return print_pci_info() ? 0 : -1;
        } else {
            return 0;
        }
    };

}; // end class pci_device_scanner

}; // end namespace xcldev

#endif

// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689
