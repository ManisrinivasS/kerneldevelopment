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



typedef struct job{
   unsigned int data;
    struct job* next;
}Jobqueue;

static Jobqueue *lowprorityjob=NULL;
static Jobqueue *highprorityjob=NULL;



static struct task_struct *low_consumer_thread;
static struct task_struct *high_consumer_thread;
static struct task_struct *producer_thread;

static int thread_id_low;
static int thread_id_high;
static int thread_id_prod;

struct semaphore sema_Low;
struct semaphore sema_High;
struct mutex mutex_Low;
struct mutex mutex_High;


static void enqueue_job(Jobqueue *newjob, Jobqueue **queue){
    newjob->next = NULL;
    if(*queue ==NULL){
        *queue = newjob;
    }else{
        Jobqueue *temp = *queue;
        while(temp->next != NULL){
            temp=temp->next;
        }
        temp->next = newjob;
    }
}

static Jobqueue *dequeue(Jobqueue **queue){

    if(!*queue)
      return NULL;

    Jobqueue *temp = *queue;
    *queue=temp->next;
    return temp;
}

static int producerthread_fn(void *arg){
   unsigned int rand_val;

    while(!kthread_should_stop()){

        Jobqueue *newjob= (Jobqueue*)kmalloc(sizeof(Jobqueue),GFP_KERNEL);

        if(!newjob){
            printk("job allocation not created\n");
            continue;
        }


        get_random_bytes(&rand_val, sizeof(rand_val));
       // printk(KERN_INFO "Produced data............................%d\n\n", newjob->data);
       newjob->data= rand_val % 100;          // Keep data within 0-99 range
      // printk("rad new %d %d",rand_val,newjob->data);
      // if (newjob->data < 0)
        //     newjob->data = -newjob->data;
      
        
     
        int priority = newjob->data % 2;
        if(priority == 0){
            mutex_lock(&mutex_High);
            printk(KERN_INFO "Prod HPJ %u   @%px\n", newjob->data,newjob);
            enqueue_job(newjob,&highprorityjob);
            mutex_unlock(&mutex_High);
            up(&sema_High);
            
        }else {

            mutex_lock(&mutex_Low);
            printk(KERN_INFO "Prod LPJ %u   @%px\n", newjob->data,newjob);
            enqueue_job(newjob,&lowprorityjob);
            mutex_unlock(&mutex_Low);
            up(&sema_Low);
           
        }
        ssleep(1);
    }

      

   return 0;
}

static int consumer__fn_b(void *arg){

     Jobqueue *job;
     Jobqueue *lowjob;
    //  printk("consumer threads running\n");
     
     while(!kthread_should_stop()){


        if(down_interruptible(&sema_High) == 0){
            if (kthread_should_stop()) break;
                
            mutex_lock(&mutex_High);
            job = dequeue(&highprorityjob);
           
            mutex_unlock(&mutex_High);
            if(job){
                printk("consA H-data  %u  @%px\n",job->data,job);
                kfree(job);
             }
            }
        

        if(down_interruptible(&sema_Low) == 0){
            if (kthread_should_stop()) break;
            mutex_lock(&mutex_Low);
            lowjob = dequeue(&lowprorityjob);
            
            mutex_unlock(&mutex_Low);
            if(lowjob){
                printk("consA L-data %u  @%px\n",lowjob->data,lowjob);
                kfree(lowjob);
             }
            }
            
        ssleep(1);
        }
       
return 0;
}


static int consumer_fn_a(void *arg){

    Jobqueue *job;

        
    while(!kthread_should_stop()){
           if(down_interruptible(&sema_Low) == 0){

            if (kthread_should_stop()) break;
            mutex_lock(&mutex_Low);
            job = dequeue(&lowprorityjob);
            
            mutex_unlock(&mutex_Low);
            if(job){
                printk("cons B L-data %d  @%px\n",job->data,job);
                kfree(job);
             }
        }
        ssleep(1);
    }
    

  return 0;
}


static int __init init_consprod(void){
    printk("Priority based consumer and producer\n");
    sema_init(&sema_High,0);
    sema_init(&sema_Low,0);
    mutex_init(&mutex_High);
    mutex_init(&mutex_Low);

    producer_thread = kthread_run(producerthread_fn,&thread_id_prod,"producer");
    low_consumer_thread=kthread_run(consumer_fn_a,&thread_id_low,"consumer_low");
    high_consumer_thread=kthread_run(consumer__fn_b,&thread_id_high,"consumer_high");
    

    return 0;
}

static void __exit cleanup_consprod(void){
    printk("module cleaning up\n");
    up(&sema_High);
    up(&sema_High);
    up(&sema_Low); 
    up(&sema_Low); 


    if(high_consumer_thread){
     kthread_stop(high_consumer_thread);
     printk(KERN_INFO "high_consumer_thread stopped");
    } 

   if(low_consumer_thread){
    kthread_stop(low_consumer_thread);
    printk(KERN_INFO "low_consumer_thread stopped");
   }

   if(producer_thread){
    kthread_stop(producer_thread);
    printk(KERN_INFO "producer_consumer_thread stopped");
  }

   // Optional: Free leftover jobs if any (safe cleanup)
   Jobqueue *job;
   while ((job = dequeue(&highprorityjob)) != NULL)
       kfree(job);
   while ((job = dequeue(&lowprorityjob)) != NULL)
       kfree(job);

}

module_init(init_consprod);
module_exit(cleanup_consprod);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kernel drivers learning");
MODULE_DESCRIPTION("Bounder buffer consumer producer problem");
