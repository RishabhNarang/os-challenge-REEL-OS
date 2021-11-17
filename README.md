# REEL-OS

 Here we present our experiments and the final solution. 
 Also, describe the folder structure and branch structure
 
 
Final server implementation:



1) Multi-processing Experiment


2) Multi-threading Experiment


3) Priority Queue Experiment

To ensure that received requests are processed according to their priorities, it was decided to integrate a priority queue into the server, so that the request with the highest priority is served first. Different data structures can be used to implement a priority queue such as a max-heap and a linked-list. In the case with our server, both data structures need to support two different operations; 1) insertion of a newly received request into the queue and 2) extraction/removal of the request with the maximum priority from the queue.




4) LookUp Table Experiment
