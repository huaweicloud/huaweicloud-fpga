#define pr_fmt(fmt)     KBUILD_MODNAME ":%s: " fmt, __func__

#include <linux/ioctl.h>
#include "version.h"
#include "xdma_cdev.h"
#include "cdev_ctrl.h"

/*
 * character device file operations for control bus (through control bridge)
 */
#define BITSEL(x) (0x1 << x)
#define OCL_CLKWIZ_CONFIG_OFFSET(n)   (0x200 + 4 * (n))

const static unsigned clock_baseaddr[2] = {
	0x050000,
	0x051000
};


static unsigned get_ocl_frequency(const struct xdma_dev * dev, unsigned offset)
{
	u32 val;
	const u64 input = 100;
	u32 mul0, div0;
	u32 mul_frac0 = 0;
	u32 div1;
	u32 div_frac1 = 0;
	u64 freq;

        val = ioread32(dev->bar[0] + offset + 0x04);
	if ((val & 1) == 0)
		return 0;

	val = ioread32(dev->bar[0] + offset + OCL_CLKWIZ_CONFIG_OFFSET(0));


	div0 = val & 0xff;
	mul0 = (val & 0xff00) >> 8;
	if (val & BITSEL(26)) {
		mul_frac0 = val >> 16;
		mul_frac0 &= 0x3ff;
	}

	/*
	 * Multiply both numerator (mul0) and the denominator (div0) with 1000 to
	 * account for fractional portion of multiplier
	 */
	mul0 *= 1000;
	mul0 += mul_frac0;
	div0 *= 1000;

	val = ioread32(dev->bar[0] + offset + OCL_CLKWIZ_CONFIG_OFFSET(2));

	div1 = val &0xff;
	if (val & BITSEL(18)) {
		div_frac1 = val >> 8;
		div_frac1 &= 0x3ff;
	}

	/*
	 * Multiply both numerator (mul0) and the denominator (div1) with 1000 to
	 * account for fractional portion of divider
	 */

	div1 *= 1000;
	div1 += div_frac1;

	div0 *= div1;
	mul0 *= 1000;
	if (div0 == 0) {

		return 0;
	}
	freq = (input * mul0)/div0;
	return freq;

}


void fill_frequency_info(const  struct xdma_dev * dev, struct xdma_ioc_info_ex *obj)
{
	int i;
	for(i = 0; i < 2; ++i) {
		obj->ocl_frequency[i] = get_ocl_frequency(dev, clock_baseaddr[i]);
	}
}



static ssize_t char_ctrl_read(struct file *fp, char __user *buf, size_t count,
		loff_t *pos)
{
	struct xdma_cdev *xcdev = (struct xdma_cdev *)fp->private_data;
	struct xdma_dev *xdev;
	void *reg;
	u32 w;
	int rv;

	rv = xcdev_check(__func__, xcdev, 0);
	if (rv < 0)
	{
		
		return rv;	
	}
		
	xdev = xcdev->xdev;

	/* only 32-bit aligned and 32-bit multiples */
	if (*pos & 3)
		return -EPROTO;
	/* first address is BAR base plus file position offset */
	reg = xdev->bar[xcdev->bar] + *pos;
	
	w = ioread32(reg);
	dbg_sg("char_ctrl_read(@%p, count=%ld, pos=%d) value = 0x%08x\n", reg,
		(long)count, (int)*pos, w);
	rv = copy_to_user(buf, &w, 4);
	if (rv)
		dbg_sg("Copy to userspace failed but continuing\n");

	*pos += 4;
	return 4;
}

static ssize_t char_ctrl_write(struct file *file, const char __user *buf,
			size_t count, loff_t *pos)
{
	struct xdma_cdev *xcdev = (struct xdma_cdev *)file->private_data;
	struct xdma_dev *xdev;
	void *reg;
	u32 w;
	int rv;

	rv = xcdev_check(__func__, xcdev, 0);
	if (rv < 0)
		return rv;	
	xdev = xcdev->xdev;

	/* only 32-bit aligned and 32-bit multiples */
	if (*pos & 3)
		return -EPROTO;

	/* first address is BAR base plus file position offset */
	reg = xdev->bar[xcdev->bar] + *pos;
	rv = copy_from_user(&w, buf, 4);
	if (rv) {
		pr_info("copy from user failed %d/4, but continuing.\n", rv);
	}

	dbg_sg("char_ctrl_write(0x%08x @%p, count=%ld, pos=%d)\n", w, reg,
		(long)count, (int)*pos);
	
	iowrite32(w, reg);
	*pos += 4;
	return 4;
}

static long version_ioctl(struct xdma_cdev *xcdev, void __user *arg)
{
	struct xdma_ioc_info obj;
	struct xdma_dev *xdev = xcdev->xdev;
	
	memset(&obj, 0, sizeof(obj));
	obj.vendor = xdev->pdev->vendor;
	obj.device = xdev->pdev->device;
	obj.subsystem_vendor = xdev->pdev->subsystem_vendor;
	obj.subsystem_device = xdev->pdev->subsystem_device;
	obj.feature_id = xdev->feature_id;
	obj.driver_version = DRV_MOD_VERSION_NUMBER;
	if (copy_to_user(arg, &obj, sizeof(struct xdma_ioc_info)))
		return -EFAULT;
	return 0;
}


bool is_XPR(struct FeatureRomHeader *header, const struct xdma_dev *xdev)
{
#ifdef USE_FEATURE_ROM
	if(header->region[0].XPR == 1) {
#else
	if ((((xdev->pdev->device >> 5) & 0x0007) == 0x0001) || (((xdev->pdev->device >> 5) & 0x0007) == 0x0000)) {
#endif
		printk(KERN_INFO "DMA XPR dsa detected. \n");
		return 1;
	}
	else {
		printk(KERN_INFO "DMAXPR dsa not detected. \n");
		return 0;
	}
}

// Max delta with this approximation found to be ~  1.3172 C
int16_t onchip_temp(uint32_t temp)
{
	s64 t = (temp * 50138);
	t = t >> 16;
	t = t / 100;
	t = t - 274;
	return t;

}

unsigned short to_volt(uint32_t volt)
{
	unsigned short volts = ((volt * 1000 * 3) >> 16) ;
	return volts;
}


static long device_info_ioctl(struct xdma_cdev *xcdev, void __user *arg)
{
	struct xdma_ioc_info_ex obj;
	struct xdma_dev *xdev = xcdev->xdev;
	int i;

	u32 val;
	u16 stat;
	long result;
      struct FeatureRomHeader header;
      
      memset(&obj, 0, sizeof(obj));
      
	val = ioread32(xdev->bar[0] + 0x032000);
	obj.mig_calibration[0] = (val & BITSEL(0)) ? true : false;
	obj.mig_calibration[1] = (val & BITSEL(0)) ? true : false;
	obj.mig_calibration[2] = (val & BITSEL(0)) ? true : false;
	obj.mig_calibration[3] = (val & BITSEL(0)) ? true : false;

	fill_frequency_info( xdev, &obj);
	// From Sysmon
	val = ioread32(xdev->bar[0] + 0x0A0000 + 0x400);
	obj.onchip_temp = onchip_temp(val);
	val = ioread32(xdev->bar[0] + 0x0A0000 + 0x404);
	obj.vcc_int = to_volt(val);
	val = ioread32(xdev->bar[0] + 0x0A0000 + 0x408);
	obj.vcc_aux= to_volt(val);
	val = ioread32(xdev->bar[0] + 0x0A0000 + 0x418);
	obj.vcc_bram= to_volt(val);

	// TODO: Fill in the right values.
	obj.fan_temp	= 0;
	obj.fan_speed	= 0;
		
	memcpy_fromio(&header, xdev->bar[0] + 0x0B0000, sizeof(struct FeatureRomHeader));
	
	obj.ddr_channel_num = header.DDRChannelCount;
	obj.ddr_channel_size = header.DDRChannelSize;
	memcpy(obj.vbnv, header.VBNVName, 64);
	memcpy(obj.fpga, header.FPGAPartName, 64);
	obj.time_stamp = header.TimeSinceEpoch;

	obj.num_clocks = 0;
	for(i = 0; i < 4; ++i) {
		if (header.region[0].clk[i] != 0)
			obj.num_clocks++;
	}

	obj.isXPR = is_XPR(&header, xdev);

	obj.pcie_link_width = 0;
	obj.pcie_link_speed = 0;
	result = pcie_capability_read_word(xdev->pdev, PCI_EXP_LNKSTA, &stat);
	if (result) {
		printk(KERN_INFO "DMA %s PCIe link status is busy. \n", __FUNCTION__);
		return -1;
	}
	obj.pcie_link_width = (stat & PCI_EXP_LNKSTA_NLW) >> PCI_EXP_LNKSTA_NLW_SHIFT;
	obj.pcie_link_speed = stat & PCI_EXP_LNKSTA_CLS;

	if (copy_to_user(arg, &obj, sizeof(struct xdma_ioc_info_ex)))
		return -EFAULT;
	return 0;
}

long char_ctrl_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	struct xdma_cdev *xcdev = (struct xdma_cdev *)filp->private_data;
	struct xdma_dev *xdev;
	
	long result = 0;
	int rv;

	rv = xcdev_check(__func__, xcdev, 0);
	if (rv < 0)
	{
		return rv;
	}
	xdev = xcdev->xdev;

	pr_info("cmd 0x%x, xdev 0x%p, pdev 0x%p.\n", cmd, xdev, xdev->pdev);

	if (_IOC_TYPE(cmd) != XDMA_IOC_MAGIC) {
		pr_err("==cmd %x, bad magic (0x%x)/(0x%x).\n",
			 cmd, _IOC_TYPE(cmd), XDMA_IOC_MAGIC);
		return -ENOTTY;
	}

	if (_IOC_DIR(cmd) & _IOC_READ)
		result = !access_ok(VERIFY_WRITE, (void __user *)arg,
				_IOC_SIZE(cmd));
	else if (_IOC_DIR(cmd) & _IOC_WRITE)
		result =  !access_ok(VERIFY_READ, (void __user *)arg,
				_IOC_SIZE(cmd));

	if (result) {
		pr_err("bad access %ld.\n", result);
		return -EFAULT;
	}

	switch (cmd) {
	case XDMA_IOCINFO:
		return version_ioctl(xcdev, (void __user *)arg);
	case XDMA_IOCINFO_EX:
		return device_info_ioctl(xcdev, (void __user *)arg);
	case XDMA_IOCOFFLINE:
		if (!xdev) {
			pr_info("cmd %u, xdev NULL.\n", cmd);
			return -EINVAL;
		}
		xdma_device_offline(xdev->pdev, xdev);
		break;
	case XDMA_IOCONLINE:
		if (!xdev) {
			pr_info("cmd %u, xdev NULL.\n", cmd);
			return -EINVAL;
		}
		xdma_device_online(xdev->pdev, xdev);
		break;
	default:
		pr_err("UNKNOWN ioctl cmd 0x%x.\n", cmd);
		return -ENOTTY;
	}
	return 0;
}

/* maps the PCIe BAR into user space for memory-like access using mmap() */
static int bridge_mmap(struct file *file, struct vm_area_struct *vma)
{
	struct xdma_dev *xdev;
	struct xdma_cdev *xcdev = (struct xdma_cdev *)file->private_data;
	unsigned long off;
	unsigned long phys;
	unsigned long vsize;
	unsigned long psize;
	int rv;

	rv = xcdev_check(__func__, xcdev, 0);
	if (rv < 0)
		return rv;	
	xdev = xcdev->xdev;

	off = vma->vm_pgoff << PAGE_SHIFT;
	/* BAR physical address */
	phys = pci_resource_start(xdev->pdev, xcdev->bar) + off;
	vsize = vma->vm_end - vma->vm_start;
	/* complete resource */
	psize = pci_resource_end(xdev->pdev, xcdev->bar) -
		pci_resource_start(xdev->pdev, xcdev->bar) + 1 - off;

	dbg_sg("mmap(): xcdev = 0x%08lx\n", (unsigned long)xcdev);
	dbg_sg("mmap(): cdev->bar = %d\n", xcdev->bar);
	dbg_sg("mmap(): xdev = 0x%p\n", xdev);
	dbg_sg("mmap(): pci_dev = 0x%08lx\n", (unsigned long)xdev->pdev);

	dbg_sg("off = 0x%lx\n", off);
	dbg_sg("start = 0x%llx\n",
		(unsigned long long)pci_resource_start(xdev->pdev,
		xcdev->bar));
	dbg_sg("phys = 0x%lx\n", phys);

	if (vsize > psize)
		return -EINVAL;
	/*
	 * pages must not be cached as this would result in cache line sized
	 * accesses to the end point
	 */
	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
	/*
	 * prevent touching the pages (byte access) for swap-in,
	 * and prevent the pages from being swapped out
	 */
	vma->vm_flags |= VMEM_FLAGS;
	/* make MMIO accessible to user space */
	rv = io_remap_pfn_range(vma, vma->vm_start, phys >> PAGE_SHIFT,
			vsize, vma->vm_page_prot);
	dbg_sg("vma=0x%p, vma->vm_start=0x%lx, phys=0x%lx, size=%lu = %d\n",
		vma, vma->vm_start, phys >> PAGE_SHIFT, vsize, rc);

	if (rv)
		return -EAGAIN;
	return 0;
}

/*
 * character device file operations for control bus (through control bridge)
 */
static const struct file_operations ctrl_fops = {
	.owner = THIS_MODULE,
	.open = char_open,
	.release = char_close,
	.read = char_ctrl_read,
	.write = char_ctrl_write,
	.mmap = bridge_mmap,
	.unlocked_ioctl = char_ctrl_ioctl,
};

void cdev_ctrl_init(struct xdma_cdev *xcdev)
{
	cdev_init(&xcdev->cdev, &ctrl_fops);
}
