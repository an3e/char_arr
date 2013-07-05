#include "char_arr.h"

//device name
char name[] = "char_arr";

//contains the functionalities provided by our driver
//register the file operations by the driver with the VFS
static struct file_operations fops = {
	.owner	= THIS_MODULE,
};

//structure to represent our char device within kernel
//(will be filled later in the init function at runtime)
struct cdev *kernel_cdev = NULL;

//device number
dev_t devno = 0;

//information for udev
struct class *cl = NULL;	//create sysfs entry; will be filled in init function

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

	//fill our cdev structure
	kernel_cdev		= cdev_alloc();
	kernel_cdev->ops	= &fops;
	kernel_cdev->owner	= THIS_MODULE;

	//now that we have an allocated cdev structure and a valid device number
	//we can tell kernel about our initialized cdev structure and device number
	ret = cdev_add(kernel_cdev, devno, 1);
	if(ret < 0){
		printk(KERN_INFO "%s: init: unable to add cdev to kernel.\n", name);
		return ret;
	}

	cl = class_create(THIS_MODULE, name);		//create sysfs entry

	printk(KERN_INFO "%s: init: function done.\n", name);

	return 0;
}

/**************** driver exit function ****************/
static void __exit char_exit()
{
	printk(KERN_INFO "%s: exit: function start\n", name);

	printk(KERN_INFO "%s: exit: removing the /sys entry...\n", name);
	class_destroy(cl);

	printk(KERN_INFO "%s: exit: deleting the device structure from kernel...\n", name);
	cdev_del(kernel_cdev);

	printk(KERN_INFO "%s: exit: unregistering the device...\n", name);
	unregister_chrdev_region(devno, 1);

	printk(KERN_INFO "%s: exit: function done.\n", name);
}

/**************** register init & exit functions ****************/
module_init(char_init);
module_exit(char_exit);

/**************** define the driver license ****************/
MODULE_LICENSE("GPL");
