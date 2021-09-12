#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

// Note compile on the linux sever

main()
{
  int sockfd, clientfd;
  struct sockaddr_in serverAddr, cliAddr;
  socklen_t addr_size;
  int len = sizeof(cliAddr);
  char serverMessage[256] = "Hello from server\n";

  //create a socket file descriptor
  sockfd = socket(PF_INET, SOCK_STREAM, 0);

  //fill the serverAddr structure
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(9002);
  serverAddr.sin_addr.s_addr = INADDR_ANY;

  // bind the address
  bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

  // listen
  if (listen(sockfd, 5) == 0)

    //accept the connection
    clientfd = accept(sockfd, (struct sockaddr *)&cliAddr, &len);

  // send the data
  send(clientfd, serverMessage, sizeof(serverMessage), 0);

  close(sockfd);
  return 0;
}
