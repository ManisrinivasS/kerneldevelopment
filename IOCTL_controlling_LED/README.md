IOCTL - input output control , ioctl is one type of system call 
        We have other system calls like procfs, sysfs etc
      
      Implement ioctl sys call in kernel space and passing commands from the user space to controll leds.
      
      From userspace call the ioctl method with set/get with args to turn on/off the led
            long ioctl( "file descriptor","ioctl command","Arguments");
            
      Define the IOCTLS in userspace and the same defination will define kernel space also
          #define GPIO_SET_LED	_IOW('l',2,int)
          #define GPIO_GET_LED	_IOR('l',3,int)


 **ioctl function:**
 
       int  ioctl(struct file *file,unsigned int cmd,unsigned long arg)
       
       **parameter:**
           file pointer pointing to the file that was passed by the application
           cmd - GPIO_SET_LED and GPIO_GET_LED two cmds are used
           arg - give args to led on or off 
        
