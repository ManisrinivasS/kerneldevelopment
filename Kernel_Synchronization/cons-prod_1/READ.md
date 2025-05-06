Problem Statement:
You are asked to implement a producer-consumer system where:

A producer thread generates jobs (integers) and places them in a shared job queue.

One or more consumer threads fetch jobs from this queue and process them.

The queue has unlimited size, but consumers must not try to consume a job if the queue is empty.

Multiple consumers must not access the queue at the same time.

Job production and consumption happen at random intervals.

**Features:**
    o. one producer and two consumers
    o. shared and simple job queue
    o. protecting jobs for race conditions using mutex
    o. informing jobs availbility using semaphore
    o. safe memory handling while creating malloc.
    
