obj-m += xdma.o
xdma-objs := \
	libxdma.o \
	xdma_mod.o \
	cdev_ctrl.o \
	cdev_events.o \
	cdev_sgdma.o \
	xdma_cdev.o \

export PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/root/bin

CONFIG_MODULE_SIG=n
KERNELDIR ?= /lib/modules/$(shell uname -r)/build

PWD       := $(shell pwd)

ROOT := $(dir $(M))
XILINXINCLUDE := -I$(ROOT)/include -I$(ROOT)../include

all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

install: all
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules_install
	depmod -a
	install -m 644 10-xdma.rules /etc/udev/rules.d
	-rmmod -s xdma || true
	-modprobe xdma

clean:
	rm -rf *.o *.o.d *~ core .depend .*.cmd *.ko *.ko.unsigned *.mod.c .tmp_versions *.symvers .#* *.save *.bak Modules.* modules.order Module.markers *.bin

CFLAGS_libxdma.o := $(XILINXINCLUDE)
CFLAGS_xdma_mod.o := $(XILINXINCLUDE)
CFLAGS_cdev_sgdma.o := $(XILINXINCLUDE)
CFLAGS_cdev_ctrl.o := $(XILINXINCLUDE)
