#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(){

  int clientSocket, portNum, nBytes;
  char buffer[1024];
  struct sockaddr_in serverAddr;
  socklen_t addr_size;
  
  clientSocket = socket(PF_INET, SOCK_DGRAM, 0);
  
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(4999); // 5000 - 1
  serverAddr.sin_addr.s_addr = inet_addr("10.0.2.15");
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  
  addr_size = sizeof serverAddr;
  
  while(1){
  
    printf("Type a sentence to send to server:");
    fgets(buffer,1024,stdin);
    buffer[strcspn(buffer, "\n")] = 0;//remove new line
    printf("You typed: %s\n",buffer);
    if(strcmp(buffer, "Good bye") == 0){ //compare input
    
      close(clientSocket);
      return 0;
    }
    
    nBytes = strlen(buffer);
    sendto(clientSocket,buffer,nBytes,0,(struct sockaddr *)&serverAddr, addr_size); //send to multicast_sender
  }
  close(clientSocket);
  return 0;
}
