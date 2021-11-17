# REEL-OS

 Here we present our experiments and the final solution. 
 Also, describe the folder structure and branch structure


## Experiments
 
### Priority Queue Experiment

To ensure that received requests are processed according to their priorities, it was decided to integrate a priority queue into the server, so that the request with the highest priority is served first. Different data structures can be used to implement a priority queue such as a max-heap and a linked-list. In the case with our server, both data structures need to support two different operations; 1) insertion of a newly received request into the queue and 2) extraction/removal of the request with the maximum priority from the queue. In theory, the running times of these operations are different for max heaps and linked lists. They are as follows:

Unsorted Linked List: INSERT in O(1) time	and EXTRACTMAX	in O(n) time. Max Heap:	INSERT in	O(log n) time and EXTRACTMAX in	O(log n) time.  

The different running times of these operations would presumably also result in different average latencies for a max-heap solution and a linked-list solution and thus different scores. The purpose of this experiment is to determine if this is the case and to identify the priority queue solution which yields the lowest scores. 

[Max-heap and linked-list figure](https://github.com/SirEsquireGoatTheThird/os-challenge-REEL-OS/blob/Priority-Queue-Test-Branch/experiment_plot.png)


### LookUp Table Experiment

### Multi-processing Experiment - Erik Priest
With this experiment I tested if multi-processing would speed up the calculation of the hash for the server. I tested with various number of child processes such as 10, and 100. I found that it reliably passed the run_client.sh and run_client_milestone.sh however, it would slow to a crawl on run_client_final.sh possibly due to the max number of child processes being 100. The multi-processing relies on the server loop of obtaining a message -> adding to priority queue -> create child process if max is not met -> creates child process to handle request -> server repeats. In the child, it processes the request and then sends the message, once sent it dies. 
 

### Multi-threading Experiment

 
## Final Server 
