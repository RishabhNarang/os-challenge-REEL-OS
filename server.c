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
#include <sys/mman.h>
#include "maxHeap.h"
#include "queue.h"

#define SERVER "REEL-OS"

size_t* current_process_count;

int main(int argc, char *argv[]){

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
    listen(sockfd, 256);

    struct maxHeap request_queue = initMaxHeap(); 

    #pragma region // Multi-Process related variables

        const size_t MAX_PROCESS_COUNT = 100; // If this doens't work just put 100 instead (rlimit_nproc)
        current_process_count = mmap(NULL, sizeof(size_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS , -1, 0);
        *current_process_count = 0;

        // Share process count with 
        

        const pid_t parentPID = getpid();

    #pragma endregion

    size_t messsageCount = 0;

    while(1){

        int clientfd;
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(struct sockaddr_in);

        //accept the connection
        clientfd = accept(sockfd, (struct sockaddr *)&cliAddr, &len);


        // Check if messaged was received and in the buffer, if in the buffer than add to the queue
        /// Recieving Component ///
        char recievedMessage[49];

        // -1 is error message
        int count = recv(clientfd, &recievedMessage, sizeof(recievedMessage), MSG_WAITALL);
        /*printf("Bytes Recieved: %d\n", count);*/

        // uint8_t is a single byte size integer, therefore since out message is 32 bytes long we need to set hash array to 32
        uint64_t start, end;
        uint8_t priority;
        unsigned char hash[32];

        // Check if message was recieved
        if(count != -1){
            //printf("Message was recieved: %ld \n", messsageCount);
            messsageCount++;

            requestNode node;

            memcpy(&node.hash, &(recievedMessage[0]), 32);
            memcpy(&start, &(recievedMessage[32]), 8);
            memcpy(&end, &(recievedMessage[40]), 8);
            memcpy(&priority, &(recievedMessage[48]), 1);

            node.start = be64toh(start);
            node.end = be64toh(end);
            node.priority = be64toh(priority);

            // printf("Message recieved -> \n");
            // printf("Start: %ld End: %ld \n Hash: \n", node.start, node.end);
            // PrintCharArray(node.hash, 32);
            // printf("\n");

            insert(&request_queue, node);
        }
        
        if(*current_process_count < MAX_PROCESS_COUNT && request_queue.heapSize > 0){
            //printf("Child number {%d} \n", *current_process_count);
            ForkChild(&request_queue, clientfd);
            *current_process_count += 1;
        }else{
            //printf("Max child count met\n");
            int child_status;
            //waitpid(pd, &child_status, 0);
            wait(&child_status);

            //printf("Child finished with status {%i}\n", child_status);

            if(child_status == -1)
                printf("Child returned with an error");
                continue;

            ForkChild(&request_queue, clientfd);
            *current_process_count += 1;
            // Run fork again and other process
        }
        // || Is it possible to make a check so we can properly exit this loop and close the socket? || //
    }
    close(sockfd);
    return 0;
}

/// Forks and creates a child ///
void ForkChild(struct maxHeap *request_queue, int clientfd){
    //printf("Child was created \n");
    
    requestNode currentRequest = extractMax(request_queue);

    pid_t pd = fork();

    if(pd == -1){
        printf("Fork failed\n");
        // Add current request back to queue
        insert(request_queue, currentRequest);
    }else if(pd == 0){
        // The entire child process

        clock_t start = clock();
        /*Do something*/
        

        uint64_t result;
        uint64_t leresult;

        // Does the child actually have the correct information to do the calculation?
        // printf("Child %ld has the following data: \n", getpid());
        // printf("Start: %ld End: %ld \n Hash: \n", currentRequest.start, currentRequest.end);
        // PrintCharArray(currentRequest.hash, 32);

        Process(currentRequest.hash, &currentRequest.start, &currentRequest.end, &result);
        // printf("Result: %ld \n", result);
        leresult = result;
        result = htobe64(result);

        int sendBytes = send(clientfd, &result, sizeof(result), 0);
        if(sendBytes == -1)
            printf("Send failed \n");

        
        clock_t end = clock();
        float seconds = (float)(end - start) / CLOCKS_PER_SEC;

        printf("Message sent: Elasped Time: %d  Result: %"PRId64"\n", seconds, leresult);

        *current_process_count -= 1;
        //printf("Child number {%d} \n", *current_process_count);
        exit(0);
    }else{
        // Parents code, do nothing
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
