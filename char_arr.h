//we will write a kernel module
#include <linux/module.h>
#include <linux/kernel.h>

//we want to write a characteer driver
#include <linux/cdev.h>

//we need struct file_operations for our driver
#include <linux/fs.h>

//we want to create an entry in the /dev and /sys fs
#include <linux/device.h>

//function prototypes
static int	__init	char_init( void );	//init function
static void	__exit	char_exit( void );	//exit function
