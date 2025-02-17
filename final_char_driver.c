#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/uaccess.h>

#define FIRST_MINOR 0
#define MINOR_CNT 2

static dev_t dev;
static struct cdev c_dev[2];
static struct class *cl;

static int my_open(struct inode *i, struct file *f)
{
	return 0;
}
static int my_close(struct inode *i, struct file *f)
{
	return 0;
}

static char c = 'A';

static ssize_t my_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
  printk(KERN_INFO "Read first char diver\n");
	if (*off == 0)
	{
		if (copy_to_user(buf, &c, 1))
		{
			return -EFAULT;
		}
		*off += 1;
		return 1;
	}
	else
		return 0;
}
static ssize_t my_write(struct file *f, const char __user *buf, size_t len, loff_t *off)
{
	
	if (copy_from_user(&c, buf + len - 1, 1))
	{
		return -EFAULT;
	}
	if (iminor(f->f_inode) == 0) {
	
        printk(KERN_INFO "Write invoked on first device file  \n");
        } else if (iminor(f->f_inode) == 1) {
        printk(KERN_INFO "Write invoked on second device file   \n");
        } 
	return len;
}

static struct file_operations driver_fops =
{
	.owner = THIS_MODULE,
	.open = my_open,
	.release = my_close,
	.read = my_read,
	.write = my_write
};

static int __init fcd_init(void)
{
	int ret;
	int i ;
	struct device *dev_ret;

	if ((ret = alloc_chrdev_region(&dev, FIRST_MINOR, MINOR_CNT, "final_driver")) < 0)
	{
		return ret;
	}
	
	//TODO 1: Create the class with name char & assign it to cl.
	cl=class_create(THIS_MODULE,"char");
	//Refer include/linux/device.h for class_create() prototype
	if (IS_ERR(cl))
	{
		//cdev_del(&c_dev);
		unregister_chrdev_region(dev, MINOR_CNT);
		return PTR_ERR(cl);
	}
	
	for(i=0;i<2;i++){
	cdev_init(&c_dev[i], &driver_fops);
	c_dev[i].owner = THIS_MODULE;

	if ((ret = cdev_add(&c_dev[i], MKDEV(MAJOR(dev), i), 1)) < 0)
	{
		unregister_chrdev_region(dev, MINOR_CNT);
		return ret;
	}

	
	//TODO 2: Create the device & assign it to dev_ret
	//Refer include/linux/device.h for device_create() prototype. 2nd & 4th arguments are NULL
  dev_ret=device_create(cl,NULL,MKDEV(MAJOR(dev), i),NULL,"finalchar%d",i);

//   struct device *device_create(, struct device *parent,vdev_t devt, 
// 								void *drvdata, const char *fmt, ...);




	if (IS_ERR(dev_ret))
	{
		class_destroy(cl);
		cdev_del(&c_dev[i]);
		unregister_chrdev_region(dev, MINOR_CNT);
		return PTR_ERR(dev_ret);
	}
    }

	return 0;
}

static void __exit fcd_exit(void)
{
	//TODO 3: Delete the device
	device_destroy(cl, MKDEV(MAJOR(dev),0));
	device_destroy(cl, MKDEV(MAJOR(dev),1));
	//TODO 4: Delete the class
    class_destroy(cl);

	cdev_del(&c_dev[0]);
	cdev_del(&c_dev[1]);
	unregister_chrdev_region(dev, MINOR_CNT);
}

module_init(fcd_init);
module_exit(fcd_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Embitude Trainings <info@embitude.in>");
MODULE_DESCRIPTION("Final Character Driver Template");
