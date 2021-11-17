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

The max-heap/linked-list plot shows no discernible difference in scores between the two implementations which is not quite as expected. For a small number of requests, say 5 or 10, the difference in scoring should not be noticeable, which is also the case. However, in theory the max-heap should be faster than the linked-list as the number of performed 
INSERT/EXTRACTMAX operations grows with the number of requests, since O(logn)+O(logn)=O(logn) is asymptotically faster than O(n)+O(1)=O(n). In conclusion, the experiment does not determine which of the proposed solutions - a max-heap or a linked-list - is the best for implementing a priority queue for the server, because the scores are so similar. A max-heap solution was adopted though.


### LookUp Table Experiment

### Multi-processing Experiment - Erik Priest
With this experiment I tested if multi-processing would speed up the calculation of the hash for the server. I tested with various number of child processes such as 10, and 100. I found that it reliably passed the run_client.sh and run_client_milestone.sh however, it would slow to a crawl on run_client_final.sh possibly due to the max number of child processes being 100. The multi-processing relies on the server loop of obtaining a message -> adding to priority queue -> create child process if max is not met -> creates child process to handle request -> server repeats. In the child, it processes the request and then sends the message, once sent it dies. 
 

### Multi-threading Experiment

 
## Final Server 
