#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>

#define FIRST_MINOR 0
#define MINOR_CNT 2

#define MAX_BUFFER 20
static int wrptr =0;
static int rdptr=0 ;

static dev_t dev;
static struct cdev c_dev[2];
static struct class *cl;
static char buffer[MAX_BUFFER];
struct mutex my_mutex;

static int my_open(struct inode *i, struct file *f)
{
	return 0;
}
static int my_close(struct inode *i, struct file *f)
{
	return 0;
}

static ssize_t my_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
  printk(KERN_INFO "Read first char diver, len :%d  %d \n",iminor(f->f_inode),len);
  size_t bytes_read =0;
  mutex_lock(&my_mutex);
	  	if(rdptr == wrptr){
	  	 mutex_unlock(&my_mutex);
	  	 return 0;
	  	}
	  	
		if (*off == 0)
		{
			while(rdptr != wrptr && bytes_read < len){
			if (copy_to_user(&buf[bytes_read], &buffer[rdptr], 1))
			{
				return -EFAULT;
			}
			*off += 1;
			rdptr = (rdptr + 1) % MAX_BUFFER;
			bytes_read++;
		   }
	    }

		mutex_unlock(&my_mutex);	
		printk(KERN_INFO "read end first char diver:%d\n",bytes_read);
		return bytes_read;
}

static ssize_t my_write(struct file *f, const char __user *buf, size_t len, loff_t *off)
{
	printk(KERN_INFO "write first char diver\n");
	int count = len > MAX_BUFFER ? MAX_BUFFER : len;
	int i;
	//write data of max 20 bytes ,implement circular buffer 
    mutex_lock(&my_mutex);
	for( i = 0 ; i < count;i++){
	
	if (copy_from_user(&buffer[wrptr], &buf[i], 1))
	{
	    mutex_unlock(&my_mutex);
		return -EFAULT;
	}
	
	  wrptr = (wrptr + 1) % MAX_BUFFER ;
	  if(wrptr == rdptr){
	   rdptr = (rdptr+1) % MAX_BUFFER;
	  }
	
	}
	mutex_unlock(&my_mutex);
	printk(KERN_INFO "write first char diver end\n");
	return count;
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
	
	for(i=0;i<1;i++){
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

    mutex_init(&my_mutex);  // Initialize mutex
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
	//device_destroy(cl, MKDEV(MAJOR(dev),1));
	//TODO 4: Delete the class
    class_destroy(cl);
	cdev_del(&c_dev[0]);
	//cdev_del(&c_dev[1]);
	unregister_chrdev_region(dev, MINOR_CNT);
}

module_init(fcd_init);
module_exit(fcd_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Embitude Trainings <info@embitude.in>");
MODULE_DESCRIPTION("Final Character Driver Template");
