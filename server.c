#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <inttypes.h>
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <pthread.h>
#include <semaphore.h>
#include "maxHeapTest.h" //max heap implementation of priority queue

// Before we turn this in for the first milestone we should ensure that we can use all these packages ////


// Note compile on the linux sever
// Assumption that we are using openssl 1.1.1
#define SERVER "REEL-OS"


sem_t count_request_not_assigned;
pthread_mutex_t queue_lock;
//maxHeap mh = initMaxHeap();

void * socketThread(void *arg);
void * workerThread(void *arg);
int main(int argc, char *argv[]){

    unsigned int port;

    if (argc >= 2)
    {
        // Get the port number from the command-line arguments
        port = atoi(argv[1]);
    }else{

        // Throw an error if we don't have enough arguments for the script
        return 1;
    }

    int sockfd;
    struct sockaddr_in serverAddr, cliAddr;
    socklen_t addr_size;
    int len = sizeof(cliAddr);

    //create a socket file descriptor
    sockfd = socket(PF_INET, SOCK_STREAM, 0);

    //fill the serverAddr structure
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // bind the address
    bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

    // listen
    listen(sockfd, 1000);
    pthread_t tid[1000];
    //////////////////////////////////////////////////////////
    int num_worker_threads = 4;
    pthread_t workers[num_worker_threads];
    sem_init(&count_request_not_assigned, 0, 0);
    if (pthread_mutex_init(&queue_lock, NULL) != 0) {
    	printf("\n mutex init has failed\n");
        return 1;
    }
    maxHeap *mh = initMaxHeap(1000);
    printf("Max heap initialized with size = %d \n", mh->curSize);
    for(int i = 0; i< num_worker_threads; i++){
	if( pthread_create(&workers[i], NULL, workerThread, mh) != 0 )
           printf("Failed to create worker thread number %d \n", i);
	else
	   printf("Created worker thread number %d \n", i);
    }
    /////////////////////////////////////////////////////////////////
    int i = 0;
    printf("Entering the main while loop \n");
    while (1)
    {


        int clientfd;
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(struct sockaddr_in);

        //accept the connection
        clientfd = accept(sockfd, (struct sockaddr *)&cliAddr, &len);

	if (clientfd<0)
	{
	    printf("Accept failed with i = %d \n", i);
	    continue;
	}

	/// Recieving Component ///
        char recievedMessage[49];

	 // -1 is error message
        int count = recv(clientfd, &recievedMessage, sizeof(recievedMessage), MSG_WAITALL);
        /*printf("Bytes Recieved: %d\n", count);*/

        // uint8_t is a single byte size integer, therefore since out message is 32 bytes long we need to set hash array to 32
        unsigned char hash[32];
        uint64_t start, end;
        uint8_t q;

        memcpy(&hash, &(recievedMessage[0]), 32);
        memcpy(&start, &(recievedMessage[32]), 8);
        memcpy(&end, &(recievedMessage[40]), 8);
        memcpy(&q, &(recievedMessage[48]), 1);


        /* --- The hash doesn't have to be swapped in byte order since the hashing done by openssl will return in the same
            byte order as network byte order, I will keep the function to swap byte order of array if the need arises --- */

     	printf("\n \n Got a new request on main thread: \n");
        //PrintArray(hash, 32);
        //be64tohArray(hash, 32);

        printf("Printing hash: \n");
        PrintCharArray(hash, 32);

        start = be64toh(start);
        end = be64toh(end);

    	printf("Start value: %" PRIu64 "\n", start);
    	printf("End value: %" PRIu64 "\n", end);
    	printf("Priority: %d\n", q);

	//printf("Serving the request with i = %d \n",i);
	int* clientFdPtr = (int*)malloc(sizeof(int));
	*clientFdPtr = clientfd;
	requestNode *rn = (requestNode *)malloc(sizeof(requestNode));
	memcpy(&(rn->hash), &hash, 32);
	rn->start = start;
	rn->end = end;
	rn->priority = q;
	rn->clientfd = clientfd;	
	//requestNode rn = { memcpy(&rn.hash, &hash, 32), .start = start, .end = end, .priority = q, .clientfd = *clientFdPtr};

	printf("\n \n Created requestNode on main thread: \n");
        //PrintArray(hash, 32);
        //be64tohArray(hash, 32);

        printf("Printing hash: \n");
        PrintCharArray(rn->hash, 32);

        printf("Start value: %" PRIu64 "\n", rn->start);
        printf("End value: %" PRIu64 "\n", rn->end);
        printf("Priority: %d\n", rn->priority);

	pthread_mutex_lock(&queue_lock);
	insert(mh, *rn);
	printf("Inserted!. Heapsize is %d \n",mh->curSize);
	pthread_mutex_unlock(&queue_lock);
	sem_post(&count_request_not_assigned);
	//if( i >= 100)
        //{
          //i = 0;
          //while(i < 100)
          //{
            //pthread_join(tid[i++],NULL);
          //}
          //i = 0;
        //}

    }

    close(sockfd);
    return 0;
}

void * workerThread(void *arg)
{
    printf("worker thread entered \n");
    maxHeap *mh = (maxHeap *)arg;
    printf("got the maxheap \n");
    while(1){
	sem_wait(&count_request_not_assigned);
    	printf("Inside worker thread trying to get a request from queue\n");
    	//recv(newSocket , client_message , 2000 , 0);
    	// Send message to the client socket

    	/////////////////////////////////////////////////////////////////////////////////////////
	pthread_mutex_lock(&queue_lock);
	//printf("The queue element is  %d \n,",mh->heap[0].priority);
	requestNode *request = extractMax(mh);
	printf("Extracted from queue. Heapsize is %d\n", mh->curSize);
	printf("The extracted hash value is \n,");
	PrintCharArray(request->hash,32);
        printf("The extracted start value is %" PRIu64 "\n",request->start);
        printf("The extracted end value is  %" PRIu64 "\n",request->end);
        printf("The extracted priority value is  %d \n,",request->priority);
	printf("The extracted clientfd value is %d \n",request->clientfd);
	pthread_mutex_unlock(&queue_lock);
	int clientfd = request->clientfd;
    	uint64_t result;
    	Process(request->hash, &(request->start), &(request->end), &result);

    	//printf("\t\b\b Answer: %d \n", result);

    	// Switch from little endian to big endian for network
    	result = htobe64(result);
    	// Send message to client
    	int bytes_sent = send(clientfd, &result, sizeof(result), 0);
    	if(bytes_sent < 0){
            printf("Failed to send the result to fd = %d \n", clientfd);
    	}
    	///////////////////////////////////////////////////////////////////////////////////////


    	printf("Finished the thread for fd = %d \n",clientfd);
    	//send(newSocket,buffer,13,0);
    	//printf("Exit socketThread \n");
    	close(clientfd);
    	//pthread_exit(NULL); 
    }
}

void * socketThread(void *arg)
{
    int clientfd = *((int *)arg);
    printf("Inside the thread for fd = %d \n",clientfd);
    //recv(newSocket , client_message , 2000 , 0);
    // Send message to the client socket

    /////////////////////////////////////////////////////////////////////////////////////////
    /// Recieving Component ///
    char recievedMessage[49];

    // -1 is error message
    int count = recv(clientfd, &recievedMessage, sizeof(recievedMessage), MSG_WAITALL);
    if(count == 0)
    {
	printf("Client disconnected with fd = %d \n", clientfd);
    }
    else if(count == -1)
    {
	printf("recv failed with fd = %d \n", clientfd);
    }
    /*printf("Bytes Recieved: %d\n", count);*/

    // uint8_t is a single byte size integer, therefore since out message is 32 bytes long we need to set hash array to 32
    unsigned char hash[32];
    uint64_t start, end;
    uint8_t q;

    memcpy(&hash, &(recievedMessage[0]), 32);
    memcpy(&start, &(recievedMessage[32]), 8);
    memcpy(&end, &(recievedMessage[40]), 8);
    memcpy(&q, &(recievedMessage[48]), 1);


    /* --- The hash doesn't have to be swapped in byte order since the hashing done by openssl will return in the same
            byte order as network byte order, I will keep the function to swap byte order of array if the need arises --- */
    //printf("\n \n got request hash on main as : \n");
    //PrintArray(hash, 32);

    //PrintArray(hash, 32);
    //be64tohArray(hash, 32);

    //printf("Printing hash: \n");
    //PrintCharArray(hash, 32);

    start = be64toh(start);
    end = be64toh(end);

    //printf("Start value: %" PRIu64 "\n", start);
    //printf("End value: %" PRIu64 "\n", end);
    //printf("Priority: %d\n", q);

    uint64_t result;
    Process(hash, &start, &end, &result);

    //printf("\t\b\b Answer: %d \n", result);

    // Switch from little endian to big endian for network
    result = htobe64(result);


    // Send message to client
    int bytes_sent = send(clientfd, &result, sizeof(result), 0);
    if(bytes_sent < 0){
	printf("Failed to send the result to fd = %d \n", clientfd);
    }
    ///////////////////////////////////////////////////////////////////////////////////////


    printf("Finished the thread for fd = %d \n",clientfd);
    //send(newSocket,buffer,13,0);
    //printf("Exit socketThread \n");
    close(clientfd);
    pthread_exit(NULL);
}
void Process(const uint8_t *hash, const uint64_t *start, const uint64_t *end, uint64_t *result)
{

    EVP_MD_CTX *mdctx;
    const EVP_MD *md;
    unsigned char md_value[32];
    unsigned int md_len;

    md = EVP_get_digestbyname("sha256");
    mdctx = EVP_MD_CTX_new();

    for (uint64_t i = *start; i < *end; i++)
    {
        //printf("Check value: %d \n", i);
        /*
            Parameter explanation:
                0: Pointer to the value that is being hashed
                1: Size (in bytes) of the value being hashed
                2: Pointer to the unsigned char array that will store the resulting hash
                3: Pointer to the size of the hashed value (to be set by the Digest function)
                4: Pointer to the EVP_MD (unsure exactly what it does but does set the type of hashing)
                5: Just keep at NULL (it changed the engine implementation I believe)
        */
        EVP_Digest(&i, sizeof(i), md_value, &md_len, md, NULL);

        // printf("Digest is: ");
        // for (unsigned int k = 0; k < md_len; k++)
        //     printf("%02x", md_value[k]);
        // printf("\n");

        // Compare hashResult and recieved hash
        if (HashCompare(hash, md_value) == 0)
        {
            *result = i;
            EVP_MD_CTX_free(mdctx);
            return;
        }
    }

    EVP_MD_CTX_free(mdctx);

    // If value cannot be found return the start of the value (might be better to have a different default value but we should discuss it)
            // We should make process a returnable bool value (but bools don't exist in C so we would have to make our own)
    *result = *start;

    return;
}

void be64tohArray(uint8_t *arr, size_t len)
{
    // Swap from big endian to little endian by swapping values by traversing n / 2 times w/ O(n) access
    /*
        [a, b, c, d]  
         ^        ^
         |        | swapped using common tmp term then progress
         d        a
    */
    for (uint8_t i = 0; i < (len / 2); i++)
    {
        uint8_t j = len - 1 - i;
        uint8_t tmp = arr[j];
        arr[j] = arr[i];
        arr[i] = tmp;
    }
}




void PrintArray(const uint8_t *arr, const size_t len)
{
    // Prints out uint8_t array
    // Could be made generic but idk how to do that in C  - Erik Priest
    for (unsigned int i = 0; i < len; i++)
    {
        printf("%02x", arr[i]);
    }

    printf("\n");
}

void PrintCharArray(const unsigned char *arr, const size_t len)
{
    for (unsigned int i = 0; i < len; i++)
    {
        printf("%02x", arr[i]);
    }

    printf("\n");
}
int HashCompare(const uint8_t *hash, const unsigned char *hashResult)
{

    /*
        Simple O(n) comparison 
    */

    for (unsigned int i = 0; i < 32; i++)
    {
        if (hash[i] != hashResult[i])
        {
            return 1;
        }
    }
    
    // Zero means everything matches
    return 0;
}
