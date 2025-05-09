#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/errno.h>
#include <asm/uaccess.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/random.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include <linux/semaphore.h>


#define BUF_SIZE 5
int buffer[BUF_SIZE];
static int head=0, tail=0;

static struct task_struct *thread_cons[2], *thread_prod[2];
static int thread_id[2];
static int thread_id_prod[2];

struct semaphore my_semEmpty;
struct semaphore my_semFull;
struct mutex my_mutex;


static void enqueue_job(int val){
  buffer[tail]=val;
  tail = (tail+1)%BUF_SIZE;

}

static int dequeue(void){
    int val= buffer[head];
    head=(head+1) % BUF_SIZE;
    return val;
}

static int producerthread(void *arg){
    unsigned int i;
    int id=*(int *)arg;
    unsigned int j;
    unsigned int rand_val;

    while(!kthread_should_stop()){

        get_random_bytes(&rand_val, sizeof(rand_val));
        rand_val %=  100; 
       
        down_interruptible(&my_semEmpty); 
        mutex_lock(&my_mutex);
        enqueue_job(rand_val);
        printk(KERN_INFO "Producer %d: produced item %d\n", id, rand_val);
        mutex_unlock(&my_mutex);
        up(&my_semFull);

        get_random_bytes(&j, sizeof(j));
		ssleep(j % 3);
       if (signal_pending(current))
			break;
   }
    
    printk(KERN_INFO "Producer Thread Stopping\n");
    thread_prod[id] = NULL;
	do_exit(0);
}

static int consumerthread(void *arg){
     int res;
     int id;
     unsigned int i;
     
     while(!kthread_should_stop()){
        down_interruptible(&my_semFull); 
        mutex_lock(&my_mutex);
        res=dequeue();
        printk("counsmer data:%d\n",res);
        mutex_unlock(&my_mutex);
        up(&my_semEmpty);
       
        get_random_bytes(&i, sizeof(i));
		ssleep(i % 2);
        if (signal_pending(current))
			break;
     }

     id = *(int *)arg;
 	thread_cons[id] = NULL;
	printk(KERN_INFO "Consumer Thread Stopping\n");
	do_exit(0);
}


static int __init init_consprod(void){
    printk("module init\n");


    sema_init(&my_semEmpty, BUF_SIZE);
    sema_init(&my_semFull,0);
    mutex_init(&my_mutex);
    int i,j;

//-------thread creation for consumer-----------------//
    for( i = 0 ; i <2;i++){
        thread_id[i]=i;
        thread_cons[i]=kthread_run(consumerthread,&thread_id[i],"my_consumer");
        if(thread_cons[i]){
            printk(KERN_INFO "Consumer thread %d created\n",i);
        }else{
            printk(KERN_INFO "Consumer threadd failed\n");
            return -1;
        }
    }
//-------thread creation for producer-----------------//

    for( j =0;j<2;j++){
        thread_id_prod[j] = j;
        thread_prod[j] = kthread_run(producerthread,&thread_id_prod[j],"my_producer");
        if(thread_prod[j]){
            printk(KERN_INFO "Producer thread %d created", j);
        }else{
            printk(KERN_INFO "Failed to create producer thread\n");
            return -1;
        }
    }

    return 0;
}

static void __exit cleanup_consprod(void){
    printk("module cleaning up\n");
    int i;
    up(&my_semFull);   // Wake sleeping consumers
    up(&my_semEmpty); 
    for( i=0;i<2;i++){

        if(thread_cons[i] != NULL){
            kthread_stop(thread_cons[i]);
            printk(KERN_INFO "consumer thread %d stopped",i);
        }
        if(thread_prod[i] != NULL){
            kthread_stop(thread_prod[i]);
            printk(KERN_INFO "producer thread %d stopped",i);
        }
       
    }
}

module_init(init_consprod);
module_exit(cleanup_consprod);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kernel drivers learning");
MODULE_DESCRIPTION("Bounder buffer consumer producer problem");
