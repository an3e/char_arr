#include "char_arr.h"

//device name
char name[] = "char_arr";

//device number
dev_t devno = 0;

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

	printk(KERN_INFO "%s: init: function done.\n", name);

	return 0;
}

/**************** driver exit function ****************/
static void __exit char_exit()
{
	printk(KERN_INFO "%s: exit: function start\n", name);

	printk(KERN_INFO "%s: exit: unregistering the device...\n", name);
	unregister_chrdev_region(devno, 1);

	printk(KERN_INFO "%s: exit: function done.\n", name);
}

/**************** register init & exit functions ****************/
module_init(char_init);
module_exit(char_exit);

/**************** define the driver license ****************/
MODULE_LICENSE("GPL");
