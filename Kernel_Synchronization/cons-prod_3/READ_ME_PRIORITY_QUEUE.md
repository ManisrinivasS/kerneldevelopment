Producer thread generates random jobs with numbers marked as high priority and  numbers as low priority.

**Two consumer threads:**

high_consumer_thread handles both high and low priority jobs.

low_consumer_thread handles only low priority jobs.

**Synchronization:**

mutex_High, mutex_Low to protect shared queues.

sema_High, sema_Low to block consumers until jobs are available.

Proper cleanup with kthread_stop() and up() to unblock waiting threads during module unload.

 **Issues and Learnings:**
1. Consumer not processing low-priority jobs (initially)
Problem: You observed that consA L-data was not printed.

Root Cause: The high-priority consumer was exiting early or blocked before it could process low-priority jobs.

Fix: Ensured both semaphores (sema_High, sema_Low) were checked with proper down logic, and kthread_should_stop() was added after unblocking.

**2. Hanging on rmmod**
Problem: rmmod caused a hang due to blocked threads.

Root Cause: Threads were blocked on down() (waiting on semaphore) and couldn't reach kthread_should_stop().

Fix: Added up() calls in cleanup_consprod() to unblock the threads before stopping them.


up(&sema_High);
up(&sema_Low);

Also ensured that down_interruptible() was used instead of down_timeout() for responsiveness to kthread_stop().

**3. Unfreed memory on unload**
Problem: Jobs left in the queue weren’t freed.

Fix: Cleanup code now dequeues and frees all jobs from both queues:


while ((job = dequeue(&highprorityjob)) != NULL)
    kfree(job);
**4. Thread exit order**
Lesson: It's important to stop consumers before the producer to avoid enqueueing jobs when no consumers exist.

**5. Concurrency correctness**
Lesson: All queue accesses are wrapped with proper mutex_lock()/unlock() to avoid race conditions.

**summary:**

Use of sema_init, mutex_init, kthread_run, kthread_should_stop.

Clean module init/exit functions.

Proper log messages for observability.

Job queue implemented as a simple linked list.

Thread-safe operations across multiple threads.
