#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <inttypes.h>
#include <openssl/sha.h>
// Note compile on the linux sever

#define SERVER "REEL-OS"


int main(int argc, char *argv[])
{

  /*  /// Exepected Arguments ///

          argv[0]   -- script name
          argv[1]   -- port # 

  */

  unsigned int port;

  if(argc >= 2){
      // Get the port number from the command-line arguments
      port = atoi(argv[1]);
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
    printf("\n Bytes received count: %d\n", count);
    uint8_t hash_be[32];
    uint8_t q;
    uint64_t start,end;
    memcpy(hash_be,recievedMessage,32);
    memcpy(&start,&(recievedMessage[32]),8);
    memcpy(&end,&(recievedMessage[40]),8);
    memcpy(&q,&(recievedMessage[48]),1);
    uint8_t hash_host[32];
    printf("Printing the 8 bit numbers(hash sent by client) ... \n");
    for(int i =0; i< 32; i++){
	hash_host[i] = hash_be[32-i-1];
	printf("%" PRIu8 "",hash_host[i]);
	printf("     ");
    }
    printf("\nEnding Printing.. \n\n");
    printf("\n Printing the binary version of each byte(hash sent by client)...");
    for(int i =0; i< 32; i++){
        bin(hash_host[i]);
	printf("\n");
    }
    printf("The hash is %s", hash_host);
    start = be64toh(start);
    end = be64toh(end);
    printf("\n start value is %" PRIu64 "\n", start);
    printf("end value is %" PRIu64 "\n", end);
    printf("q value is %hhu\n", q);
    uint64_t result;
    for(uint64_t test = start; test < end; test++){
	uint8_t hash_test[32];

	for(int i=0;i<32;i++){
            printf("%" PRIu8 "   ", hash_test[i]);
        }
	printf("\n\n calculating the hash now for %" PRIu64 " \n",test);
	SHA256(&test, 8, hash_test);
	for(int i=0;i<32;i++){
            printf("%" PRIu8 "   ", hash_test[i]);
        }
        
	printf("\n Printing the binary version of each byte(hash test)...");
        for(int i =0; i< 32; i++){
          bin(hash_test[i]);
          printf("\n");
        }

	int found = 0;
	printf("Checking for equality of hashes... \n");
	for(int i=0;i<32;i++){
	    if(hash_test[i]!=hash_host[i]) break;
	    else if (i==31) found = 1;
	    else continue;
	}
	if(found==1){
	    result = test;
	    printf("Found the number %"PRIu64"\n", test);
	    break;
	}
    } 

    // send the data
    send(clientfd, &result, sizeof(uint64_t), 0);
  }

  close(sockfd);
  return 0;
}

void bin(uint8_t n)
{
    /* step 1 */
    if (n > 1)
        bin(n / 2);
 
    /* step 2 */
    printf("%d", n % 2);
}


