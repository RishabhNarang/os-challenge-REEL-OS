#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <inttypes.h>

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
    printf("\n %d\n", count);
    uint8_t q;
    uint64_t start,end;
    memcpy(&start,&(recievedMessage[32]),8);
    memcpy(&end,&(recievedMessage[40]),8);
    memcpy(&q,&(recievedMessage[48]),1);
    start = be64toh(start);
    end = be64toh(end);
    printf("%" PRIu64 "\n", start);
    printf("%" PRIu64 "\n", end);
    printf("%hhu\n", q);

  

    // send the data
    char severMessage[8] = "nope";
    send(clientfd, severMessage, sizeof(severMessage), 0);
  }

  close(sockfd);
  return 0;
}


