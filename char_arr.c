#include "char_arr.h"

//device name
char name[] = "char_arr";
char proc_entry_name[] = "char_arr";

//contains the functionalities provided by our driver
//register the file operations by the driver with the VFS
static struct file_operations fops = {
	.owner	= THIS_MODULE,
	.open	= char_open,
	.read	= char_read,
	.write	= char_write,
	.release= char_release,
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
	int ret = -1;

	printk(KERN_INFO "%s: init: function start\n", name);

	//try to allocate device number for our driver
	printk(KERN_INFO "%s: init: allocating device number...\n", name);
	ret = alloc_chrdev_region( 	&devno,	//output parameter for first assigned number
					0,	//first of the requested range of minor numbers
					1,	//number of minor numbers required
					name);	//the name of associated device or driver
						//(same name will appear in /proc/devices)
	if( ret < 0 ){
		goto failed_alloc_chrdev_region;
	} else {
		printk(KERN_INFO "%s: init: allocated major number: %i.\n", name, MAJOR(devno));
	}

	printk(KERN_INFO "%s: init: allocating struct cdev...\n", name);
	kernel_cdev		= cdev_alloc();
	if (kernel_cdev == NULL){
		goto failed_cdev_alloc;
	}
	//initialize our driver object
	kernel_cdev->ops	= &fops;
	kernel_cdev->owner	= THIS_MODULE;

	//now that we have an allocated & initialized cdev structure
	//and a valid device number we can tell kernel about
	//our initialized cdev structure and device number
	printk(KERN_INFO "%s: init: registering cdev structure....\n", name);
	ret = cdev_add(kernel_cdev, devno, 1);
	if( ret < 0 ){
		goto failed_cdev_add;
	}

	printk(KERN_INFO "%s: init: creating /sys entry....\n", name);
	cl = class_create(THIS_MODULE, name);		//create sysfs entry
	if(cl == NULL){
		goto failed_class_create;
	}

	printk(KERN_INFO "%s: init: creating /dev entry....\n", name);
	pdev = device_create(cl, NULL, devno, NULL, name);	//create a device file under /dev
	if(pdev == NULL){
		goto failed_device_create;
	}

	//create an entry in /proc fs
	#if LINUX_VERSION_CODE < KERNEL_VERSION(3,10,1)
		create_proc_read_entry(proc_entry_name, 0, NULL, char_read_proc, NULL);
	#endif

	//initialize the device semaphore as unlocked
	sema_init(&char_arr.sem, 1);

	printk(KERN_INFO "%s: init: function done.\n", name);

	return 0;

	//do clean up work in case of failure
failed_device_create:
	class_destroy(cl);
failed_class_create:
failed_cdev_add:
	kobject_put(&kernel_cdev->kobj);
failed_cdev_alloc:
	unregister_chrdev_region(devno, 1);
failed_alloc_chrdev_region:
	return -EIO;
}

/**************** driver exit function ****************/
static void __exit char_exit()
{
	printk(KERN_INFO "%s: exit: function start\n", name);

	printk(KERN_INFO "%s: exit: removing the /dev entry...\n", name);
	device_destroy(cl, devno);

	printk(KERN_INFO "%s: exit: removing the /sys entry...\n", name);
	class_destroy(cl);

	printk(KERN_INFO "%s: exit: deleting the cdev structure from kernel...\n", name);
	cdev_del(kernel_cdev);	//contains kobject_put() & more

	printk(KERN_INFO "%s: exit: unregistering the device number...\n", name);
	unregister_chrdev_region(devno, 1);

	printk(KERN_INFO "%s: exit: function done.\n", name);
}

/**************** driver open function ****************/
int char_open(	struct inode *inode,
		struct file *filp )
{
	printk(KERN_INFO "%s: opening the device...\n", name);

	//check if this is the only process working on the device data
	if(down_interruptible(&char_arr.sem)){
		printk(KERN_INFO "%s: could not hold semaphore.", name);
		return -1;
	}

	return 0;
}

/**************** driver read function ****************/
ssize_t char_read(	struct file *filp,
			char __user *buf,
			size_t count,
			loff_t *f_pos )
{
	unsigned long not_copied = 0;

	printk(KERN_INFO "%s: reading from device...\n", name);

	if(count > sizeof(char_arr.array)){
		printk(KERN_INFO "%s: triminig in read function\n", name);
		count = sizeof(char_arr.array);
	}
	not_copied = copy_to_user(buf, char_arr.array, count);

	return not_copied;
}

/**************** driver relese function ****************/
int char_release(	struct inode *inode,
			struct file *filp)
{
	printk(KERN_INFO "%s: releasing semaphore\n", name);

	//release the semaphore
	up(&char_arr.sem);

	return 0;
}

/**************** driver write function ****************/
ssize_t char_write(	struct file *filp,
			const char __user *buf,
			size_t count,
			loff_t *f_pos )
{
	unsigned long not_copied = 0;

	printk(KERN_INFO "%s: writing to device ...\n", name);

	if(count > sizeof(char_arr.array)){
		printk(KERN_INFO "%s: triming in write function\n", name);
		count = sizeof(char_arr.array);
	}
	not_copied = copy_from_user(char_arr.array, buf, count);

	return count - not_copied;	//return the number of written bytes
}

/**************** driver read proc fs entry ****************/
int char_read_proc(	char *buf,
			char **start,
			off_t offset,
			int count,
			int *eof,
			void *data ) 
{
	int len = sprintf(buf, "Hello world\n");

	return len;
}

/**************** register init & exit functions ****************/
module_init(char_init);
module_exit(char_exit);

/**************** define the driver license ****************/
MODULE_LICENSE("GPL");
