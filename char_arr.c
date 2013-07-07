#include "char_arr.h"

//device name
char name[] = "char_arr";

//contains the functionalities provided by our driver
//register the file operations by the driver with the VFS
static struct file_operations fops = {
	.owner	= THIS_MODULE,
	.open	= char_open,
};

//structure to represent our char device within kernel
//(will be filled later in the init function at runtime)
struct cdev *kernel_cdev = NULL;

//structure to represent our device data structure
struct char_device char_arr;

//device number
dev_t devno = 0;

//information for udev
struct class *cl = NULL;	//create sysfs entry; will be filled in init function
struct device *pdev= NULL;

/**************** driver init function ****************/
static int __init char_init()
{
	int ret = -1, major = 0;

	printk(KERN_INFO "%s: init: function start\n", name);

	//try to allocate device number for our driver
	ret = alloc_chrdev_region( &devno,	//output parameter for first assigned number
					0,	//first of the requested range of minor numbers
					1,	//number of minor numbers required
					name);	//the name of associated device or driver
						//(same name will appear in /proc/devices)
	if(ret < 0){
		printk(KERN_INFO "%s: init: major number allocation failed.\n", name);
		return ret;
	}

	//print the major number after successfull device number allocation
	major = MAJOR(devno);
	printk(KERN_INFO "%s: init: allocated major number: %i.\n", name, major);

	printk(KERN_INFO "%s: init: creating /sys entry.\n", name);
	cl = class_create(THIS_MODULE, name);		//create sysfs entry

	printk(KERN_INFO "%s: init: creating /dev entry.\n", name);
	device_create(cl, NULL, devno, NULL, name);	//create a device file under /dev

	//fill our cdev structure
	kernel_cdev		= cdev_alloc();
	kernel_cdev->ops	= &fops;
	kernel_cdev->owner	= THIS_MODULE;

	//now that we have an allocated cdev structure and a valid device number
	//we can tell kernel about our initialized cdev structure and device number
	printk(KERN_INFO "%s: init: registering cdev structure.\n", name);
	ret = cdev_add(kernel_cdev, devno, 1);
	if(ret < 0){
		printk(KERN_INFO "%s: init: unable to add cdev to kernel.\n", name);
		return ret;
	}

	//initialize the device semaphore as unlocked
	sema_init(&char_arr.sem, 1);

	printk(KERN_INFO "%s: init: function done.\n", name);

	return 0;
}

/**************** driver exit function ****************/
static void __exit char_exit()
{
	printk(KERN_INFO "%s: exit: function start\n", name);

	printk(KERN_INFO "%s: exit: removing the /dev entry...\n", name);
	device_destroy(cl, devno);

	printk(KERN_INFO "%s: exit: removing the /sys entry...\n", name);
	class_destroy(cl);

	printk(KERN_INFO "%s: exit: deleting the device structure from kernel...\n", name);
	cdev_del(kernel_cdev);

	printk(KERN_INFO "%s: exit: unregistering the device...\n", name);
	unregister_chrdev_region(devno, 1);

	printk(KERN_INFO "%s: exit: function done.\n", name);
}

/**************** driver open function ****************/
int char_open(struct inode *inode, struct file *filp)
{
	printk(KERN_INFO "%s: opening the device...\n", name);
	//check if this is the only process working on the device data
	if(down_interruptible(&char_arr.sem)){
		printk(KERN_INFO "%s: could not hold semaphore.", name);
		return -1;
	}

	return 0;
}

/**************** register init & exit functions ****************/
module_init(char_init);
module_exit(char_exit);

/**************** define the driver license ****************/
MODULE_LICENSE("GPL");
