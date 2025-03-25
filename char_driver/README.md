# Linux Device Driver Dev(BBB)
**1.Code Exercise**
final_char_driver 
   o.Implemented char driver, single driver servers two driver files i.e. 1 MAJOR and 2 MINOR devices files.
        
   o.Implement the open, close, read and write operations on device driver

                ![Screenshot from 2025-03-25 20-06-03](https://github.com/user-attachments/assets/6299c2a4-1249-4a80-82be-77a732ec2629)

  **2. Code Explanation**
         -> ret = **alloc_chrdev_region**(&dev, FIRST_MINOR, MINOR_CNT, "final_driver")) 
	       // alloc_chrdev_region - to register and dynamically allocate devices range of numbers - 
	       // Devices will be identified by number not with symbolic name
	       // MAJOR and MINOR - will assign dynamically, minor number will start usally from the zero value.
	       // paramters : 
	                      **dev_t *dev:** Output parameter for first assigned number
		              **unsigned baseminor:** first minor in the range of numbers
		              **unsinged count:** required number of minor devices
		              **const char *name:** name of driver 
		
         **create device files:**
	 -> cdev_init(&c_dev[i], &driver_fops);
                //Initalize the character device , and initialize file operations structure driver_fops	  
		
	 -> if ((ret = cdev_add(&c_dev[i], MKDEV(MAJOR(dev), i), 1)) < 0)
                // cdev_add registers a device with the kernel
		// MKDEV- will holds the major and minor numbers and helps us to create the dev_t identifies for the device
	
        **Create and register a class with sysfs**
	 -> cl=class_create(THIS_MODULE,"char");

	 -> dev_ret=device_create(cl,NULL,MKDEV(MAJOR(dev), i),NULL,"finalchar%d",i);
           // create dev file in /dev/mychar*, now user applicatins can  interact with kernel driver via a device file operations






