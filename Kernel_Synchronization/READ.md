Kernel Synchronization :

sync techinques in kernel space is used to manage access to shared resources by multiple threads, processes , interrrupts safely.
Because kernel is highly concurrent , handling systemcalls, hardware interrupts, multiple CPU's, it needs to ensure data should not corrupted or race condition dont occur. 
Like pthreads which are running in userspace,kernel also have kernel threads

why we need kernel sync?

Imagine two kernel threads accessing shared variable
     o. without sync techinque : both threads trying to access,wants to read and write back to variable same time leads data corrupt or race condition.
     o. with sync techinque: only one will update the variable using locking techinque and releases after work done.


Common kernel sycn mechanisms:

**MUTEX** - mutual exclusion locks
it is also called locking mechainsm, when mutex acquires the lock no other thread can access the CS until it unlocks.
usage : mutex_lock(), mutex_unlock()
          struct mutex my_mutex
          mutex_lock(&my_mutex);
          mutex_unlock(&my_mutex); 
          
**SEMAPHOREs**
It has two types counting sema and binary sema. 
CS can be acquired by 'n' num of threads 
usage: down(), up()
