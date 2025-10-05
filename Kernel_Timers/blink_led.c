#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/timer.h>
#include <linux/gpio.h>

#define GPIO_NUMBER 56  //54 55 53

static struct timer_list my_timer;

void my_timer_callback(struct timer_list *t)
{
	int ret;
	printk("My Timer call-back called (%ld).\n", jiffies);
	if(gpio_get_value(GPIO_NUMBER)){
		//led off
		//start timer
		gpio_set_value(GPIO_NUMBER,0);
		ret = mod_timer(&my_timer, jiffies + msecs_to_jiffies(5000));
        if (ret) 
			printk("Error in mod_timer\n");

	}else{
		//led on 
		// start timer
		gpio_set_value(GPIO_NUMBER,1);
		ret = mod_timer(&my_timer, jiffies + msecs_to_jiffies(2000));
        if (ret) 
			printk("Error in mod_timer\n");

	}
}

int init_module(void)
{
	int ret;

	printk("Timer module installing\n");
	timer_setup(&my_timer, my_timer_callback, 0);
	printk("Starting timer to fire in 200ms (%ld)\n", jiffies);

	gpio_set_value(GPIO_NUMBER,1); 
	ret = mod_timer(&my_timer, jiffies + msecs_to_jiffies(2000));//start timer
	if (ret) 
		printk("Error in mod_timer\n");
	return 0;
}

void cleanup_module(void)
{
	int ret;
	
	ret = del_timer(&my_timer);
	if (ret)
		printk("The timer is still in use...\n");
	printk("Timer module uninstalling\n");
	return;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Embitude Trainings <info@embitude.in>");
MODULE_DESCRIPTION("Kernel Timers Demo");
