**Bounded Buffer Problem**


     ** Problem Statement:**
      
                  Design a system where:
                  
                  A fixed-size circular buffer (e.g., size = 5) is shared between a producer and consumer.
                  
                  The producer must wait if the buffer is full.
                  
                  The consumer must wait if the buffer is empty.
                  
                  Multiple producers and consumers may be active at the same time.


**Features and Requirements:**

            Use:
                    
                    A mutex to protect access to the buffer (critical section).
                    
                    A semaphore empty_slots initialized to buffer size.
                    
                    A semaphore full_slots initialized to 0.
                    
            Producer:
            
                  Waits (down) on empty_slots, then locks the buffer, adds an item, unlocks it, and signals full_slots.
            
            Consumer:
            
                 Waits (down) on full_slots, locks the buffer, removes an item, unlocks it, and signals empty_slots.
