# REEL-OS

 Here we present our experiments and the final solution. 
 Also, describe the folder structure and branch structure


## Experiments
 
### Priority Queue Experiment

To ensure that received requests are processed according to their priorities, it was decided to integrate a priority queue into the server, so that the request with the highest priority is served first. Different data structures can be used to implement a priority queue such as a max-heap and a linked-list. In the case with our server, both data structures need to support two different operations; 1) insertion of a newly received request into the queue and 2) extraction/removal of the request with the maximum priority from the queue. In theory, the running times of these operations are different for max heaps and linked lists. They are as follows:

Unsorted Linked List: INSERT in O(1) time	and EXTRACTMAX	in O(n) time. Max Heap:	INSERT in	O(log n) time and EXTRACTMAX in	O(log n) time.  

The different running times of these operations would presumably also result in different average latencies for a max-heap solution and a linked-list solution and thus different scores. The purpose of this experiment is to determine if this is the case and to identify the priority queue solution which yields the lowest scores. 

In the experiment, scores are gathered for the max-heap implementation and the linked-list implementation and compared. For the max-heap and the linked-list all incoming requests are first inserted into the priority queue and then extracted, processed and returned to the client one by one. 
The same configuration parameters of the client were used in both cases: seed = 0 , start = 100 , difficulty = 200 , repet = 2 , delay = 100 , lambda = 3. 
In both cases, the same number of requests were sent: [2, 5, 10, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000] 

Both implementations yielded 100% reliability. The scores are: [Max-heap and linked-list figure](https://github.com/SirEsquireGoatTheThird/os-challenge-REEL-OS/blob/Priority-Queue-Test-Branch/experiment_plot.png)

[Experiment code](https://github.com/SirEsquireGoatTheThird/os-challenge-REEL-OS/tree/Priority-Queue-Test-Branch)

The max-heap/linked-list plot shows no discernible difference in scores between the two implementations which is not quite as expected. For a small number of requests, say 5 or 10, the difference in scoring should not be noticeable, which is also the case. However, in theory the max-heap should be faster than the linked-list as the number of performed 
INSERT/EXTRACTMAX operations grows with the number of requests, since O(logn)+O(logn)=O(logn) is asymptotically faster than O(n)+O(1)=O(n). In conclusion, the experiment does not determine which of the proposed solutions - a max-heap or a linked-list - is the best for implementing a priority queue for the server, because the scores are so similar. A max-heap solution was adopted though.


### LookUp Table Experiment

For this experiment I made a dictionary with a lookup function. This allows the program to fetch the hashes if they were already calculated, if the hash doesnt exist then it calculate it and stores it in the dictionary. The more hashes the program has to calculate, the more efficient this experiment is. 

### Multi-processing Experiment - Erik Priest
With this experiment I tested if multi-processing, through the creation of child processes to handle client requests would speed up the performance of the server. With our initial server implmentation we ran into issues of our server timing out due to the server performance. 

The multi-processing solution I came up with relies on the following loop.
![alt text](https://github.com/SirEsquireGoatTheThird/os-challenge-REEL-OS/blob/da257b7330feda6a79d84d536ffa277da7a0a036/multi-processing-diagram.png "Multi-processing diagram")

I tested with various number of child processes such as 10, 50, and 100. I found that it reliably passed the run_client.sh and run_client_milestone.sh. Given the following data I found that our multiprocessing ran best at 50 max child processes on the server. Although the multiprocessing performed worst than the milestone server implementation because of the overhead with creating child processes I still believe it was the better choice for large and difficult processing requests. 

![alt text](https://github.com/SirEsquireGoatTheThird/os-challenge-REEL-OS/blob/dc693c2d8f3a05d20a0c1dda9514f6a542f40052/ClientPeformance.PNG "Client Score")
![alt text](https://github.com/SirEsquireGoatTheThird/os-challenge-REEL-OS/blob/dc693c2d8f3a05d20a0c1dda9514f6a542f40052/MilestonePerformance.PNG "Client Milestone Score")

View the repository [here](https://github.com/SirEsquireGoatTheThird/os-challenge-REEL-OS/tree/multi-processing---Erik-Priest-experiment)
 

### Multi-threading Experiment

This experiment is done to improve the performance of the sequential processing of the requests. So we have considered using priority and/or dictionary along with multithreading to improve the performance. Each of the three sub-experiments have been documented below for more explanation:


1. Multiple threads

In order to speed up the processing of the incoming requests, threads (pthread library) have been used for each incoming request. So for the milestone, with 100 requests coming in, 100 threads was generated. As soon as a request comes in, the main thread creates a new thread that handles the processing of that request. There is no synchronization between the threads and each of them share the 4CPUs from the VM. 
        
View the code [here](https://github.com/SirEsquireGoatTheThird/os-challenge-REEL-OS/tree/multi-threading)

2. Threads with priority queue

Since the requests with a higher priority needs to be processed first, a max-heap implementation was used to maintain a queue of requests coming in. And instead of spawning a new thread as soon as it comes in, we have X number of worker threads waiting to fetch requests from the priority queue. This way we ensure that the higher priority requests will be extracted before the lower priority requests. Semaphores are used as signalling method so that any free worker threads can pickup a request from the queue and mutex is used for mutual exclusion between threads(only when inserting and extracting requests in the queue).
        
Tests were conducted for 4,10 and 20 worker threads. 
        
View the code [here](https://github.com/SirEsquireGoatTheThird/os-challenge-REEL-OS/tree/multi-threading-with-pq)
        
3. Threads with priority queue and dictionary

Further improvements were maded by adding a dictionary to the 'threads with priority queue' implementation. Before signalling any of the worker threads that a new request has been added to the queue, the main thread checks if the dictionary already has the result stored in it. If it already exists, then immediately return the result to client and not add it to the priority queue. Otherwise, it is added to the priority queue and one of the worker threads will eventually pick it up. Same synchronization and mutextes technique were used in this experiment as well. 
        
Tests were conducted for 4,10 and 20 worker threads. 
        
View the code [here](https://github.com/SirEsquireGoatTheThird/os-challenge-REEL-OS/tree/multi-threading-with-pq-and-dict)


Following is a plot comparing the results obtained from multithreading with Heaps&Dict vs just using the Heap. As expected, the performance improved when using Dict as well since some of the duplicated threads will be processed much faster since it's just a lookup from the dictionary instead of brute forcing the hash values. So we have chosen to go with multithreading with priority queue and dictionary for the final version.

As we increase the number of threads, the performace seem to decline as well. This could be because with more number of threads, the CPU% that each thread gets will be less and the higher priority requests will increase the resultant score. Also the true parallelism we can get is 4 threads since the Vms are assigned 4 CPUs only, so we have chosen to go with 4 worker threads.

![](/MultiThreading-Exp.png)
 
## Final Server 
