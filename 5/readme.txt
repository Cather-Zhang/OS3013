Teammates worked on this project are: czhang10 & jdmeza.

///Design///

    Our scan function implements Hillis Steele Scan executed with multiple threads.
    First, in main thread, we read the arguments and save "total number of elements" and "number of threads" as global variables. Each thread will execute Hillis Steele algorithm on a chunk of the array, which is determined by the total size and number of trheads.
    We allocate two arrays for read (in) and write (out) for the later Hillis Steele Scan algorithm, and the input array is read and copied to the "in" array.
    We initialize a lock and a condition variable (there is only one each).
    Then, we have a loop, in which we create each thread. The argument pointer we pass in is different for each thread, which stores the value of the starting index of the chunk in the array the thread will executing on. 

    Inside our thread function "add", we have implemented the Hillis Steele Scan algorithm for a chunk. For each cross addition, the process is parallel because we read "in" and write the addition result to "out". This means that each thread is completely independent and working on different sections of the array. Thus, no lock is needed for each cross addition.
    However, we need to wait for a cross addition to finish before we can start the next cross addition (e.g. we need to finish all threads' calculation on 2-cross before we can start 4-cross).

    Therefore, a barrier is added after each thread has finished each cross addition, with a total of log2(n) (n = size of array) barriers. 
    This ensures correctness of our implementation because we can not start the next level cross addition unless we finish the previous one.
    Our barrier is constructed with a combination of lock and condition variable, with a global counter. 
    
    First, we need to lock the section up with a mutual lock, so that only one thread is allowed to enter the critical section described below.
    The critical section: first we increment the global counter, which means that "this" thread already finished the current cross addition. 
    
    Then we need to check if the global counter equals to the total number of threads. 
    If it does, that means all threads have already completed current cross addition and we can then move on to the next level of cross addition.
        To get ready for the next level cross addition, first, we need to reset the global counter to 0 again so that we can keep track of the number of threads complete next cross addition.
        Next, we need to copy the elements from "out" to "in", because the write results of this level's cross addition will be the array we need to read from.
        Finally, we use con_broadcast so we can tell all threads that they can move on (aka. removing the barrier). Then we unlock the section.
    If the global counter does not equal to the total number of threads, that means there are still threads have not completed the current cross addition yet. 
        So we need to keep waiting by using cond_wait(), and this function call will unlock the critical section and allows other threads (the ones have not completed yet) to complete executing.

    After we are done with all execution with the threads, we return to the main thread and print out our out[] array as final answer.


///Performance///
   
    My design of implementing Hillis Steele Scan with multiple threads can maximize performance the best as I can.
    Because each thread is working on separate chunk of the array. For each level of cross addition, there is no sequential dependency between threads, and the time complexity is O(logx(n), where x is the number of threads creating).
    Inside each cross addition, the time complexity is O(logx(n)) as well
    The barrier exists for correctness, that is one thing which could slows our program down because we need to wait all threads to complete current cross addition before starting the next one.
