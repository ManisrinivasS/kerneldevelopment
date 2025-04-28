Kernel Synchronization :
o.sync is required to avoid dead locks or race conditions 
o.Like pthreads which are running in userspace,kernel also have kernel threads
o.for example :
     > Accessing a global variable b/w two threads, without proper sync mechianims variable will be corrupt and that leads to a race conditions. to avoid RC we use sync mechanims called mutex- mutual execulsive.
     > only one thread can access global variable by locking the critical section and unlock the CS when work done. 
     > when other thread tries to access the CS, it should goes to sleep untill lock available.

