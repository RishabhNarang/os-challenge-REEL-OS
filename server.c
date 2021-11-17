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
#include "maxHeap.h" //max heap implementation of priority queue

// Note compile on the linux sever
// Assumption that we are using openssl 1.1.1
#define SERVER "REEL-OS"


sem_t count_request_not_assigned;//This is number of requests available to be taken up by a worker thread
pthread_mutex_t queue_lock; // mutex to lock priority queue

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
    int num_worker_threads = 20;
    pthread_t workers[num_worker_threads];
    sem_init(&count_request_not_assigned, 0, 0);
    
    if (pthread_mutex_init(&queue_lock, NULL) != 0) {
    	printf("\n mutex init has failed\n");
        return 1;
    }
    maxHeap *mh = initMaxHeap(1000);
    
    for(int i = 0; i< num_worker_threads; i++){
	if( pthread_create(&workers[i], NULL, workerThread, mh) != 0 )
           printf("Failed to create worker thread number %d \n", i);
	else
	   printf("Created worker thread number %d \n", i);
    }

    int i = 0;
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
        
        unsigned char hash[32];
        uint64_t start, end;
        uint8_t q;

        memcpy(&hash, &(recievedMessage[0]), 32);
        memcpy(&start, &(recievedMessage[32]), 8);
        memcpy(&end, &(recievedMessage[40]), 8);
        memcpy(&q, &(recievedMessage[48]), 1);

        start = be64toh(start);
        end = be64toh(end);

	requestNode *rn = (requestNode *)malloc(sizeof(requestNode));
	memcpy(&(rn->hash), &hash, 32);
	rn->start = start;
	rn->end = end;
	rn->priority = q;
	rn->clientfd = clientfd;	
	
	pthread_mutex_lock(&queue_lock);
	insert(mh, *rn);
	pthread_mutex_unlock(&queue_lock);
	sem_post(&count_request_not_assigned);
    }

    close(sockfd);
    return 0;
}

void * workerThread(void *arg)
{
    maxHeap *mh = (maxHeap *)arg;
    while(1){

	sem_wait(&count_request_not_assigned); // wait until a request is ready to be processed
    	
	pthread_mutex_lock(&queue_lock);
	requestNode *request = extractMax(mh);
	pthread_mutex_unlock(&queue_lock);
	int clientfd = request->clientfd;
    	uint64_t result;
    	Process(request->hash, &(request->start), &(request->end), &result);

    	// Switch from little endian to big endian for network
    	result = htobe64(result);
    	// Send result to client
    	int bytes_sent = send(clientfd, &result, sizeof(result), 0);
    	if(bytes_sent < 0){
            printf("Failed to send the result to fd = %d \n", clientfd);
    	}
    	
    	close(clientfd); 
    }
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

        // Compare hashResult and recieved hash
        if (HashCompare(hash, md_value) == 0)
        {
            *result = i;
            EVP_MD_CTX_free(mdctx);
            return;
        }
    }

    EVP_MD_CTX_free(mdctx);

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
