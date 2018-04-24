/**
 * Copyright (C) 2017 Xilinx, Inc
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

#include <dirent.h>
#include <string>
#include <vector>
#include <algorithm>
#include <sys/utsname.h>
#include <cstdlib>

//TODO: can get this from config.h : PCI_PATH_SYS_BUS_PCI
#define ROOT_DIR "/sys/bus/pci"
#define XILINX_ID 0x10ee
#define OBJ_BUF_SIZE 1024
#define DRIVER_BUF_SIZE 1024


namespace xcldev {

    class pci_devices 
    {
	private:
	    struct pci_device {
		int domain;
		uint8_t bus, dev, func;
		uint16_t vendor_id=0, device_id=0, subsystem_id=0;
		std::string driver_name = "???", driver_version = "??";
	    };
	    std::string get_string_value(std::string& dir, const char* object){
		std::string file = dir + "/" + object;
		int fd = open(file.c_str(), O_RDONLY);
		if(fd < 0) {
		    std::cout << "Unable to open " << file << std::endl;
		    return nullptr;
		}

		char buf[OBJ_BUF_SIZE];
		memset(buf, 0, OBJ_BUF_SIZE);
		int err = read(fd, buf, OBJ_BUF_SIZE);

		if((err < 0) || (err >= OBJ_BUF_SIZE)) {
		    std::cout << "Unable to read contents of " << file << std::endl;
		}
		return buf;
	    }

	    int get_value(std::string& dir, const char* object) {
		std::string buf = get_string_value(dir,object);
		return strtol(buf.c_str(), NULL, 0);
	    }

	    bool add_device(struct pci_device& device) {
		if(device.func == 1)
		    mgmt_devices.emplace_back(device);
		else if(device.func == 0)
		    user_devices.emplace_back(device);
		else {
		    assert(0);
		    return false;
		}
		return true;
	    }

	    bool print_paths() {
		std::cout << "XILINX_OPENCL=\"";
		if(const char* opencl = std::getenv("XILINX_OPENCL")) {
		    std::cout << opencl << "\"";
		}else
		    std::cout << "\"";
		std::cout << std::endl;

		std::cout << "LD_LIBRARY_PATH=\"";
		if(const char* ld = std::getenv("LD_LIBRARY_PATH")) {
		    std::cout << ld << "\"";
		}else
		    std::cout << "\"";
		std::cout << std::endl;
		return true;
	    }

	    bool print_system_info() {
		struct utsname sysinfo;
		if (uname(&sysinfo) < 0) {
		    return false;
		}
	        std::cout << sysinfo.sysname << ":" << sysinfo.release << ":" <<  sysinfo.version << ":" << sysinfo.machine << std::endl;
		return true;
	    }

	    bool print_pci_info() 
	    {
		auto print = [](struct pci_device& dev) {
		    std::cout << std::hex << dev.device_id << ":0x" << dev.subsystem_id << ":[" << std::dec ;
		    if(!dev.driver_name.empty())
			std::cout << dev.driver_name << ":" << dev.driver_version << "]" << std::endl;
		    else
			std::cout << "]" << std::endl;
		};

		int i = 0;
		for(auto mdev: mgmt_devices) {
		    std::cout << "[" << i  << "]" << "mgmt:0x";
		    print(mdev);
		    for(auto udev: user_devices) {
			if(udev.device_id == mdev.device_id +1) {
			    std::cout << "[" << i  << "]" << "user:0x";
			    print(udev);
			}
		    }
		    ++i;
		}

		return true;
	    }
	private:
	    std::vector<pci_device> mgmt_devices;
	    std::vector<pci_device> user_devices;

	public:
	    int scan() 
	    {
		if(!print_system_info()) {
		    std::cout << "Unable to determine system info " << std::endl;
		}
		std::cout << "--- " << std::endl;
		if(!print_paths()) {
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
		if (!dir) {
		    std::cout << "Cannot open " << dirname << std::endl;
		    return -1;
		}

		while ((entry = readdir(dir)))
		{
		    if (entry->d_name[0] == '.')
			continue;

		    if (sscanf(entry->d_name, "%x:%x:%x.%d", &dom, &bus, &dev, &func) < 4) {
			std::cout << "scan: Couldn't parse entry name "<< entry->d_name << std::endl;
		    }

		    std::string subdir = dirname + entry->d_name;

		    pci_device device;
		    device.domain = dom;
		    device.bus = bus;
		    device.dev = dev;
		    device.func = func;
		    device.vendor_id = get_value(subdir,"vendor");
		    if(device.vendor_id != XILINX_ID)
			continue;
		    device.device_id = get_value(subdir, "device");
		    device.subsystem_id = get_value(subdir, "subsystem_device");

		    //Get the driver name.
		    char driverName[DRIVER_BUF_SIZE];
		    memset(driverName, 0, DRIVER_BUF_SIZE);
		    subdir += "/driver";
		    int err = readlink(subdir.c_str(),driverName, DRIVER_BUF_SIZE); 
		    if(err < 0) {
			add_device(device);
			continue;
		    }
		    if(err >= DRIVER_BUF_SIZE) {
			std::cout << "Driver name is too big " << std::endl;
			return -1;
		    }

		    device.driver_name = driverName;
		    size_t found = device.driver_name.find_last_of("/");
		    if(found !=std::string::npos) {
			device.driver_name = device.driver_name.substr(found+1);
		    }


		    //Get driver version
		    subdir += "/module/";
		    std::string version = get_string_value(subdir, "version");
		    version.erase(std::remove(version.begin(), version.end(), '\n'), version.end());
		    device.driver_version = version;

		    if(!add_device(device))
			return -1;

		}

		return print_pci_info() ? 0:-1;
	    };

    };
};

