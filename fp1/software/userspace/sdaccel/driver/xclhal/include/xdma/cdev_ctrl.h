#ifndef _XDMA_IOCALLS_POSIX_H_
#define _XDMA_IOCALLS_POSIX_H_

#include <linux/ioctl.h>

/* Use 'x' as magic number */
#define XDMA_IOC_MAGIC	'x'
/* XL OpenCL X->58(ASCII), L->6C(ASCII), O->0 C->C L->6C(ASCII); */
#define XDMA_XCL_MAGIC 0X586C0C6C

#define IOCTL_XDMA_PERF_V1 (1)
#define XDMA_ADDRMODE_MEMORY (0)
#define XDMA_ADDRMODE_FIXED (1)

/*
 * S means "Set" through a ptr,
 * T means "Tell" directly with the argument value
 * G means "Get": reply by setting through a pointer
 * Q means "Query": response is on the return value
 * X means "eXchange": switch G and S atomically
 * H means "sHift": switch T and Q atomically
 *
 * _IO(type,nr)		    no arguments
 * _IOR(type,nr,datatype)   read data from driver
 * _IOW(type,nr.datatype)   write data to driver
 * _IORW(type,nr,datatype)  read/write data
 *
 * _IOC_DIR(nr)		    returns direction
 * _IOC_TYPE(nr)	    returns magic
 * _IOC_NR(nr)		    returns number
 * _IOC_SIZE(nr)	    returns size
 */

enum XDMA_IOC_TYPES {
	XDMA_IOC_NOP,
	XDMA_IOC_INFO,
	XDMA_IOC_OFFLINE,
	XDMA_IOC_ONLINE,
	XDMA_IOC_MAX,
	XDMA_IOC_INFO_EX
};

struct xdma_ioc_base {
	unsigned int magic;
	unsigned int command;
};

struct xdma_ioc_info {
        struct xdma_ioc_base base;
        unsigned short       vendor;
        unsigned short       device;
        unsigned short       subsystem_vendor;
        unsigned short       subsystem_device;
        unsigned             dma_engine_version;
        unsigned             driver_version;
        unsigned long long   feature_id;
};

struct xdma_ioc_info_ex {
        struct xdma_ioc_base base;
	unsigned 			device_version;	
	unsigned long long 	time_stamp;
	unsigned short 		ddr_channel_num;
	unsigned short 		ddr_channel_size;
	unsigned short 		pcie_link_width;
	unsigned short 		pcie_link_speed;
	char 				vbnv[64];
	char 				fpga[64];
	unsigned short 		onchip_temp;
	unsigned short 		fan_temp;
	unsigned short 		fan_speed;
	unsigned short 		vcc_int;
	unsigned short 		vcc_aux;
	unsigned short 		vcc_bram;
	unsigned short 		ocl_frequency[4];
	bool 				mig_calibration[4];
	unsigned short 		num_clocks;
	bool 				isXPR;
};


/* IOCTL codes */
#define XDMA_IOCINFO		_IOWR(XDMA_IOC_MAGIC, XDMA_IOC_INFO, \
					struct xdma_ioc_info)
#define XDMA_IOCINFO_EX		_IOWR(XDMA_IOC_MAGIC, XDMA_IOC_INFO_EX, \
					struct xdma_ioc_info_ex)					
#define XDMA_IOCOFFLINE		_IO(XDMA_IOC_MAGIC, XDMA_IOC_OFFLINE)
#define XDMA_IOCONLINE		_IO(XDMA_IOC_MAGIC, XDMA_IOC_ONLINE)

#define IOCTL_XDMA_ADDRMODE_SET	_IOW('q', 4, int)
#define IOCTL_XDMA_ADDRMODE_GET	_IOR('q', 5, int)
#define IOCTL_XDMA_ALIGN_GET	_IOR('q', 6, int)

#endif /* _XDMA_IOCALLS_POSIX_H_ */
