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

             +-------------+             +----------------+
P1 ---> | mutex_lock() | --(success)->| Critical Section |
        +-------------+             +----------------+
                                       |
                                       v
                              +-----------------+
                              | mutex_unlock()   |
                              +-----------------+

Meanwhile:
P2 and P3 are BLOCKED at mutex_lock() and WAITING

          
**SEMAPHOREs**
It has two types counting sema and binary sema. 
CS can be acquired by 'n' num of threads 
usage: down(), up()

Semaphore count: 2

P1 --> down() -> count = 1  --> Critical Section
P2 --> down() -> count = 0  --> Critical Section
P3 --> down() -> BLOCKED (count = 0)

[When P1 calls up()]
count = 1 --> P3 wakes up and enters

