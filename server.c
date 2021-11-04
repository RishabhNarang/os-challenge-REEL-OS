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
#include "maxHeap.h" //max heap implementation of priority queue

// Before we turn this in for the first milestone we should ensure that we can use all these packages ////


// Note compile on the linux sever
// Assumption that we are using openssl 1.1.1
#define SERVER "REEL-OS"

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


int main(int argc, char *argv[])
{

    /*  /// Exepected Arguments ///
        argv[0]   -- script name
        argv[1]   -- port # 
    */

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
    listen(sockfd, 5);


    // new max heap
    maxHeap mh = initMaxHeap();
  

    while (1) {


        int clientfd;
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(struct sockaddr_in);

        //accept the connection
        clientfd = accept(sockfd, (struct sockaddr *)&cliAddr, &len);

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

        //PrintArray(hash, 32);
        //be64tohArray(hash, 32);

        //printf("Printing hash: \n");
        //PrintCharArray(hash, 32);

        start = be64toh(start);
        end = be64toh(end);

        //printf("Start value: %" PRIu64 "\n", start);
        //printf("End value: %" PRIu64 "\n", end);
        //printf("Priority: %d\n", q);

        //create new request node and insert into max heap
        requestNode rn = { .hash = hash, .start = start, .end = end, .priority = q};
	insert(&mh, rn); //mh.elem[0] is request with highest priority     

  
        // extract request with highest priority and remove it from max-heap
	//requestNode maxPriorityRequest = extractMax(&mh); 
	

        uint64_t result;
        Process(hash, &start, &end, &result);

        printf("\t\b\b Answer: %d \n", result);

        // Switch from little endian to big endian for network
        result = htobe64(result);


        // Send message to client
        send(clientfd, &result, sizeof(result), 0);

       
        
	// || Is it possible to make a check so we can properly exit this loop and close the socket || //
    }
    
    close(sockfd);

    
    return 0;
}

