//we will write a kernel module
#include <linux/module.h>
#include <linux/kernel.h>

//we will use copy_to_user() and copy_from_user()
#include <asm/uaccess.h>

//we want to write a characteer driver
#include <linux/cdev.h>

//we need struct file_operations for our driver
#include <linux/fs.h>

//we want to create an entry in the /dev and /sys fs
#include <linux/device.h>

//function prototypes
static int	__init	char_init( void );	//init function
static void	__exit	char_exit( void );	//exit function

int	char_open(	struct inode *inode,
			struct file *filp );

ssize_t	char_read(	struct file *filp,	//a pointer to the file to read from
			char __user *buf,	//destination buffer
			size_t count,		//number of bytes to copy into destination buffer
			loff_t *f_pos );	//pointer to the current position in the file

//data structure which represents our device
struct char_device{
        char array[100];
        struct semaphore sem;
};
