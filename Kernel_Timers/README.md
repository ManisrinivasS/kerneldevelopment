Linux kernel timers - software timers are managed by kernel that executes a callback after timer expires.

jiffies - are the kernel tick counter, it is maintained by kernel. we can print the jiffies values in callback function.

static struct timer_list my_timer - is the timer struct declaration and we need to add include file (linux/timer.h) 

timer_setup(&my_timer, my_timer_callback, 0) - initializes timer 
            > first param- variable
            > callback function triggers after timer expires
            > intital value is zero


mod_timer(&my_timer, jiffies + msecs_to_jiffies(200)) - set the timer to some value and when timer expires tigger a callback.
                    >msecs_to_jiffies(1000) converts milliseconds to jiffies.



summary:
This kernel module creates a timer that first fires after 200ms and then reschedules itself to fire every second thereafter. Each time it fires, it prints the current jiffies count to the kernel log (dmesg). When you remove the module, the timer is deleted cleanly.