#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/timer.h>
#include <linux/hrtimer.h>

#define TIMER_INTERVAL_MS 1000   // 1 second
static struct hrtimer my_hrtimer;
static ktime_t kt_period;

/* 
 * Callback function — called every period automatically.
 */
enum hrtimer_restart my_hrtimer_callback(struct hrtimer *timer)
{
    printk(KERN_INFO "hrtimer callback called at %lld ns\n",
           ktime_to_ns(ktime_get()));

    /* Re-trigger the timer automatically (multi-shot behavior) */
    hrtimer_forward_now(timer, kt_period);

    return HRTIMER_RESTART;  // <--- important: makes it periodic
}

int init_module(void)
{
	int ret;

	printk("High resolution multishot timer\n");

	 /* Set the period (in nanoseconds) */
    kt_period = ktime_set(0, TIMER_INTERVAL_MS * 1E6);  // ms to ns
	/* Initialize the high-resolution timer */
    hrtimer_init(&my_hrtimer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
    my_hrtimer.function = my_hrtimer_callback;

    /* Start the timer */
   ret= hrtimer_start(&my_hrtimer, kt_period, HRTIMER_MODE_REL);

	

	if (ret) 
		printk("Error in mod_timer\n");
	return 0;
}

void cleanup_module(void)
{
	int ret;
	
	ret = hrtimer_cancel(&my_hrtimer);
	if (ret)
		printk("The timer is still in use...\n");
	printk("Timer module uninstalling\n");
	return;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Embitude Trainings <info@embitude.in>");
MODULE_DESCRIPTION("Kernel Timers Demo");
