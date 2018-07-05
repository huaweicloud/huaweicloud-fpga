/*-
 *   BSD LICENSE
 *
 *   Copyright(c)  2017 Huawei Technologies Co., Ltd. All rights reserved.
 *   All rights reserved.
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *     * Neither the name of Huawei Technologies Co., Ltd  nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <dirent.h>

#include <rte_devargs.h>
#include <rte_memcpy.h>
#include <rte_string_fns.h>
#include <rte_log.h>

#include "securec.h"
#include "ul_get_port_status.h"
#include "FPGA_Common.h"

#define PCI_DEVICES_MAX             (8)
#define DEVICE_VENDOR_ID            (0x19e5)
#define DEVICE_DEVICE_ID            (0xd503)
#define SYSFS_DRIVER_DEVICES        "/sys/bus/pci/drivers/igb_uio"

static pci_device_info g_acce_devices[PCI_DEVICES_MAX];
static unsigned int g_dev_count = 0;

static int
pci_parse_sysfs_value(const char *filename, unsigned long *val)
{
	FILE *f;
	char buf[BUFSIZ];
	char *end = NULL;

	if ((f = fopen(filename, "r")) == NULL) {
		printf("%s(): cannot open sysfs value %s\n",
			__func__, filename);
		return -1;
	}

	if (fgets(buf, sizeof(buf), f) == NULL) {
		printf("%s(): cannot read sysfs value %s\n",
			__func__, filename);
		fclose(f);
		return -1;
	}
	*val = strtoul(buf, &end, 0);
	if ((buf[0] == '\0') || (end == NULL) || (*end != '\n')) {
		printf("%s(): cannot parse sysfs value %s\n",
				__func__, filename);
		fclose(f);
		return -1;
	}
	fclose(f);
	return 0;
}

static int
parse_pci_addr_format(const char *buf, int bufsize, uint16_t *domain,
		uint8_t *bus, uint8_t *devid, uint8_t *function)
{
	/* first split on ':' */
    union splitaddr {
		struct {
			char *domain;
			char *bus;
			char *devid;
			char *function;
		}bdf;
		char *str[PCI_FMT_NVAL]; /* last element-separator is "." not ":" */
	} splitaddr;

	char *buf_copy = strndup(buf, bufsize);
	if (buf_copy == NULL)
		return -1;

	if (rte_strsplit(buf_copy, bufsize, splitaddr.str, PCI_FMT_NVAL, ':')
			!= PCI_FMT_NVAL - 1)
		goto error;
	/* final split is on '.' between devid and function */
	splitaddr.bdf.function = strchr(splitaddr.bdf.devid,'.');
	if (splitaddr.bdf.function == NULL)
		goto error;
	*splitaddr.bdf.function++ = '\0';

	/* now convert to int values */
	errno = 0;
	*domain = (uint16_t)strtoul(splitaddr.bdf.domain, NULL, 16);
	*bus = (uint8_t)strtoul(splitaddr.bdf.bus, NULL, 16);
	*devid = (uint8_t)strtoul(splitaddr.bdf.devid, NULL, 16);
	*function = (uint8_t)strtoul(splitaddr.bdf.function, NULL, 10);
	if (errno != 0)
		goto error;

	free(buf_copy); /* free the copy made with strdup */
	return 0;
error:
	free(buf_copy);
	return -1;
}

static void
pci_scan_one(const char *dirname, uint16_t domain, uint8_t bus,
         uint8_t devid, uint8_t function)

{
    pci_device_info pci_device;
    char filename[PATH_MAX];
    unsigned long tmp;
    unsigned int i = 0, j = 0;
    int ret = 0;
    
    memset(&pci_device, 0, sizeof(pci_device));
   
    pci_device.addr.domain = domain;
	pci_device.addr.bus = bus;
	pci_device.addr.devid = devid;
	pci_device.addr.function = function;

    /* get vendor id */
	(void)snprintf_s(filename, sizeof(filename), sizeof(filename) - 1, "%s/vendor", dirname);   
	if (pci_parse_sysfs_value(filename, &tmp) < 0) {
		return;
	}
	pci_device.id.vendor_id = (uint16_t)tmp;

	/* get device id */
	(void)snprintf_s(filename, sizeof(filename), sizeof(filename) - 1, "%s/device", dirname);
	if (pci_parse_sysfs_value(filename, &tmp) < 0) {
		return;
	}
	pci_device.id.device_id = (uint16_t)tmp;

    /* get the requested device */
    if(DEVICE_VENDOR_ID != pci_device.id.vendor_id      \
        || DEVICE_DEVICE_ID != pci_device.id.device_id)
        return;

    if(g_dev_count == 0) {
        g_acce_devices[g_dev_count] = pci_device;
        g_dev_count++;
        return;
    }

    /* sort the device based on addr */
    for(i = 0; i < g_dev_count; i++) {
        ret = rte_eal_compare_pci_addr(&pci_device.addr, &g_acce_devices[i].addr);
        if (ret > 0)
	        continue;
        else if(ret < 0) {
            for(j = g_dev_count; j > i; j--) {
                g_acce_devices[j] = g_acce_devices[j - 1];
            }
            g_acce_devices[i] = pci_device;
            g_dev_count++;
            return;
        }
        else {
            printf("something must be wrong.\n");
            return;
        }
    }
    /* add to tail */
    g_acce_devices[g_dev_count] = pci_device;
    g_dev_count++;
    
    return;    
}
/*
 * pci_port_status_init_env: scan all acce devices(sort) to get the device info,
 * including bdf, vendor id and device id.
*/
int pci_port_status_init_env() {
    DIR *dir;
    unsigned int port_idx = 0, i = 0;
    struct dirent *e;
    char dirname[PATH_MAX] = { 0 };
    uint16_t domain;
	uint8_t bus, devid, function;

    dir = opendir(SYSFS_PCI_DEVICES);
	if (dir == NULL) {
		printf("opendir failed.\n");
		return -1;
	}

    /* scan sysfs pci dir to find devices */
    while ((e = readdir(dir)) != NULL) {
    	if (e->d_name[0] == '.')
    		continue;

    	if (parse_pci_addr_format(e->d_name, sizeof(e->d_name), &domain,        \
    			&bus, &devid, &function) != 0)
    		continue;

    	(void)snprintf_s(dirname, sizeof(dirname), sizeof(dirname) - 1, "%s/%s", SYSFS_PCI_DEVICES,     \
    		 e->d_name);
    	(void)pci_scan_one(dirname, domain, bus, devid, function);
    }

    /* allocate port_id based on bind status */
    for(i = 0; i < g_dev_count; i++) {
        (void)snprintf_s(dirname, sizeof(dirname), sizeof(dirname) - 1, "%s/%04x:%02x:%02x.%x",      \
            SYSFS_DRIVER_DEVICES,   \
            g_acce_devices[i].addr.domain,     \
    	    g_acce_devices[i].addr.bus,        \
    	    g_acce_devices[i].addr.devid,      \
    	    g_acce_devices[i].addr.function);
        if(!access(dirname, F_OK)) {
            g_acce_devices[i].bind_status = BIND;
            g_acce_devices[i].port_id = port_idx;
            port_idx++;
        }
        else {
            g_acce_devices[i].bind_status = NOT_BIND;
        }
    }

    (void)closedir(dir);
   
	return 0;
}
/*
 * pci_port_id_to_slot_id: convert port id to slot id.
*/
int pci_port_id_to_slot_id(unsigned int port_id, unsigned int *slot_id) {
    int ret = 0;
    char bdf_port[PATH_MAX] = { 0 };

    ret = get_device_dbdf_by_port_id(port_id, bdf_port);
    if(ret) {
        printf("call get_device_dbdf_by_port_id fail .\n");
        return ret;
    }

    ret = FPGA_PciGetSlotByBdf(bdf_port, slot_id);
    if(ret) {
        printf("call FPGA_PciGetSlotByBdf fail .\n");
        return ret;
    }

    return 0;
}
/*
 * pci_slot_id_to_port_id: convert slot id to port id.
*/
int pci_slot_id_to_port_id(unsigned int slot_id, unsigned int *port_id) {
    unsigned int i = 0;
    int ret = 0;
    char bdf_slot[PATH_MAX] = { 0 };
    char bdf_port[PATH_MAX] = { 0 };

    /* get bdf from slot_id */
    ret = FPGA_PciGetBdfBySlot(slot_id, bdf_slot);
    if(ret) {
        printf("call FPGA_PciGetBdfBySlot fail .\n");
        return ret;
    }
    
    /* get port_id from bdf */
    for(i = 0; i < g_dev_count; i++) {
        (void)snprintf_s(bdf_port, sizeof(bdf_port), sizeof(bdf_port) - 1, "%04x:%02x:%02x.%x",      \
            g_acce_devices[i].addr.domain,     \
		    g_acce_devices[i].addr.bus,        \
		    g_acce_devices[i].addr.devid,      \
		    g_acce_devices[i].addr.function);

        if(strncmp(bdf_slot, bdf_port, sizeof(bdf_slot))) {
           continue; 
        }

        if(NOT_BIND == g_acce_devices[i].bind_status) {
            printf("\033[1;31;40mdevice of slot_id = %d not binded.\033[0m\r\n", slot_id);
            return -1;
        }
        else {
            *port_id = g_acce_devices[i].port_id;
            return 0;
        }
    }
    //printf("\033[1;31;40mnot find this device, please check slot_id = %d\033[0m\r\n", slot_id);
    
    return -2;
}
/*
 * get_device_dbdf_by_port_id: get the BDF of device based on port id.
*/
int get_device_dbdf_by_port_id(unsigned int port_id, char* dbdf) {
    unsigned int i = 0;
    
    if(NULL == dbdf) {
        printf("get_device_dbdf_by_port_id param null.\n");
        return -1;
    }

    if(PCI_DEVICES_MAX < port_id) {
        printf("get_device_dbdf_by_port_id port_id=%d error.\n", port_id);
        return -2;
    }

    /* scan all acce devices to find the device */
    for(i = 0; i < g_dev_count; i++) {
        if(g_acce_devices[i].port_id != port_id) {
            continue;
        }

        (void)snprintf_s(dbdf, PATH_MAX, PATH_MAX - 1, "%04x:%02x:%02x.%x",     \
            g_acce_devices[i].addr.domain,  \
            g_acce_devices[i].addr.bus,     \
            g_acce_devices[i].addr.devid,   \
            g_acce_devices[i].addr.function);
        return 0;
    }

    printf("\033[1;31;40mfind no device which port_id is %d.\033[0m\r\n", port_id);
    return -3;
}