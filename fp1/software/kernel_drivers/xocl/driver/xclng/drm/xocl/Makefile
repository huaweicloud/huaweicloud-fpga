obj-m	+= xocl.o
ccflags-y := -Iinclude/drm

xocl-y := \
	xocl_sysfs.o \
	xocl_bo.o \
	xocl_drv.o \
	xocl_xdma.o \
	xocl_ioctl.o \
	xocl_test.o \
	xocl_ctx.o \
	xocl_xvc.o \
	xocl_exec.o \
	libxdma.o

export PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/root/bin

CONFIG_MODULE_SIG=n
KERNELDIR ?= /lib/modules/$(shell uname -r)/build

PWD       := $(shell pwd)

ROOT := $(dir $(M))
XILINXINCLUDE := -I$(ROOT)/../include -I$(ROOT)/../../include

all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

install: all
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules_install
	depmod -a
	install -m 644 10-xocl.rules /etc/udev/rules.d
	-rmmod -s xocl || true
	-rmmod -s xdma || true
	-modprobe xocl

clean:
	rm -rf *.o *.o.d *~ core .depend .*.cmd *.ko *.ko.unsigned *.mod.c .tmp_versions *.symvers .#* *.save *.bak Modules.* modules.order Module.markers *.bin


CFLAGS_xocl_xdma.o := $(XILINXINCLUDE)
CFLAGS_xocl_sysfs.o := $(XILINXINCLUDE)
CFLAGS_xocl_bo.o := $(XILINXINCLUDE)
CFLAGS_xocl_drv.o := $(XILINXINCLUDE)
CFLAGS_xocl_ioctl.o := $(XILINXINCLUDE)
CFLAGS_xocl_test.o := $(XILINXINCLUDE)
CFLAGS_xocl_ctx.o := $(XILINXINCLUDE)
CFLAGS_xocl_exec.o := $(XILINXINCLUDE)
CFLAGS_libxdma.o := $(XILINXINCLUDE)
