### LookUp Table Experiment

For this experiment I made a dictionary with a lookup function. This allows the program to fetch the hashes if they were already calculated, if the hash doesnt exist then it calculate it and stores it in the dictionary. The more hashes the program has to calculate, the more efficient this experiment is. 

### Multi-processing Experiment - Erik Priest
With this experiment I tested if multi-processing, through the creation of child processes to handle client requests would speed up the performance of the server. With our initial server implmentation for the milestone we ran into issues of our server timing out due to the server being to slow at processing requests. 

The multi-processing solution I came up with relies on the following loop.
![alt text](https://github.com/adam-p/markdown-here/raw/master/src/common/images/icon48.png "Logo Title Text 1")

I tested with various number of child processes such as 10, and 100. I found that it reliably passed the run_client.sh and run_client_milestone.sh however, it would slow to a crawl on run_client_final.sh with 100 child processes possibly due to limited resources. 

View the code [here](https://github.com/SirEsquireGoatTheThird/os-challenge-REEL-OS/tree/multi-processing---Erik-Priest-experiment)