Teammates worked on this project are: czhang10 & jdmeza.

///Design///

    Our scan function implements Hillis Steele Scan executed with multiple threads.
    First, in main thread, we read the arguments and save "total number of elements" and "number of threads" as global variables. Each thread will execute Hillis Steele algorithm on a chunk of the array, which is determined by the total size and number of trheads.
    We allocate two arrays for read (in) and write (out) for the later Hillis Steele Scan algorithm, and the input array is read and copied to the "in" array.

    Then, we have a loop, in which we create each thread. The argument pointer we pass in is different for each thread, which stores the value of the starting index of the chunk in the array the thread will executing on. 

    Inside our thread function "add", we have implemented the Hillis Steele Scan algorithm for a chunk. For each cross addition, the process is parallel because we read "in" and write the addition result to "out". This means that each thread is completely independent and working on different sections of the array. 
    Thus, no lock is needed for each cross addition and this maximize our concurrency. 
    However, we need to wait for a cross addition to finish before we can start the next cross addition (e.g. we need to finish all threads' calculation on 2-cross before we can start 4-cross).

    Therefore, a barrier is added after each thread has finished each cross addition, with a total of log2(n) (n = size of array) barriers. 
    This ensures correctness of our implementation because we can not start the next level cross addition unless we finish the previous one.
    Our barrier is constructed as a 2 phased semaphore barrier, with a global counter to keep track of the number of threads that have finished current cross addition. 
    
    This implementation was referenced from "The Little Book of Semaphores" by Allen B. Downey, chapter 3.7.5 "Reusable barrier solution".
    We need to 3 semaphores - 1 for mutual exclusion lock and 2 for barriers because it is a two phased barrier,  we force all threads to wait twice to ensure that all threads have finished execution.
    Barrier1 is initialized to 0 and barrier2 is initialized to 1. Mutex is initialized to 1.

    In the first phase of our barrier: 
    First, we need to decrement the mutex semaphore to "sleep" the other threads, preventing them entering the critical section, which is to increment the global counter (this prevents race condition and deadlocks).
    In the critical section,  we increment the global counter, which means that "this" thread already finished the current cross addition. 
    
    Then we need to check if the global counter equals to the total number of threads. 
    If it does, that means all threads have already completed current cross addition and we can then move on to the next level of cross addition.
        To get ready for the next level cross addition, first, we need to copy the elements from "out" to "in", because the write results of this level's cross addition will be the array we need to read from.
        Then we decrement barrier2 for phase 2 and increment barrier1 so we can exit phase 1 barrier and allow the thread to access phase 2 barrier. Then we unlock the section.
    If the global counter does not equal to the total number of threads, that means there are still threads have not completed the current cross addition yet. 
        So we need to keep waiting by drecrementing barrier1, allowing other threads to finish execution.

    Then we can proceed to the second phase barrier, which is basically the same procedure as the first phase. The only difference is that we decremen the counter each time until it reaches 0 (to get ready for our next barrier for the next level cross addition).
    And if counter equals to 0, we decrement barrier1 and increment barrier2 to release all threads from barrier2, from the whole barrier.

    After we are done with all execution with the threads, we return to the main thread and print out our out[] array as final answer.


///Performance///
   
    My design of implementing Hillis Steele Scan with multiple threads and a two phased semaphore barrier can maximize performance the best as I can.
    Because each thread is working on separate chunk of the array. For each level of cross addition, there is no sequential dependency between threads, and the time complexity is O(logx(n), where x is the number of threads creating).
    Inside each cross addition, the time complexity is O(logx(n)) as well
    The barrier exists for correctness, that is one thing which could slows our program down because we need to wait all threads to complete current cross addition before starting the next one.

    While deveoping our program, we first used a lock and condition variable as our barrier, but it gives us ~1.9X performance compared to the serial sum. Then we changed to semaphore barrier because semaphore is faster than mutex.
    Our two-phased semaphore barrier gives us ~1.63X performance compared to serial.
    And this is because multiple threads can acquire semaphore at a time, while only one thread can acquire mutex at a time. Semaphore is also only changing the value binarily, making it faster. 