#include "char_arr.h"

//device name
char name[] = "char_arr";

/**************** driver init function ****************/
static int __init char_init()
{
	printk(KERN_INFO "%s: init function\n", name);

	printk(KERN_INFO "%s: init function done.\n", name);

	return 0;
}

/**************** driver exit function ****************/
static void __exit char_exit()
{
	printk(KERN_INFO "%s: exit function\n", name);

	printk(KERN_INFO "%s: exit function done.\n", name);
}

/**************** register init & exit functions ****************/
module_init(char_init);
module_exit(char_exit);

/**************** define the driver license ****************/
MODULE_LICENSE("GPL");
