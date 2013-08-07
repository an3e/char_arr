//we will write a kernel module
#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/version.h>

//we will use copy_to_user() and copy_from_user()
#include <asm/uaccess.h>

//we want to write a characteer driver
#include <linux/cdev.h>

//we need struct file_operations for our driver
#include <linux/fs.h>

//we want to create an entry in the /proc fs
#include <linux/proc_fs.h>

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

ssize_t	char_write(	struct file *filp,	//a pointer to the file to write to
			const char __user *buf,	//source buffer
			size_t count,		//nmbr of bytes to copy to the dest. buffer
			loff_t *f_pos );	//pointer to the current position in the file

int	char_close(	struct inode *inode,
			struct file *filp );

int	char_read_proc( char *buf,
			char **start,
			off_t offset,
			int count,
			int *eof,
			void *data );

//data structure which represents our device
struct char_device{
        char array[100];
        struct semaphore sem;
};
