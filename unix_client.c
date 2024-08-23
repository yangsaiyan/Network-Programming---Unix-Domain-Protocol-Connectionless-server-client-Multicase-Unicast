#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SERVER_PATH "tpf_unix_sock.server"

#define DATA "Hello from client\n"
#define BUFFER_SIZE 256

int main(void) {
    int client_socket;
    struct sockaddr_un server_addr; 
    char buffer[BUFFER_SIZE];
    socklen_t server_addr_len = sizeof(server_addr);
   
    if ((client_socket = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1) {
    
        perror("socket");
        exit(EXIT_FAILURE);
    }
    
    //set up and bind to a temporary client address
    struct sockaddr_un client_addr;
    memset(&client_addr, 0, sizeof(struct sockaddr_un));
    client_addr.sun_family = AF_UNIX;
    sprintf(client_addr.sun_path, "/tmp/client.%ld", (long)getpid());

    if (bind(client_socket, (struct sockaddr *)&client_addr, sizeof(struct sockaddr_un)) == -1) {
    
        perror("bind");
        close(client_socket);
        exit(EXIT_FAILURE);
    }
    
    memset(&server_addr, 0, sizeof(struct sockaddr_un));
    server_addr.sun_family = AF_UNIX;        
    strcpy(server_addr.sun_path, SERVER_PATH); 

    strcpy(buffer, DATA);
    printf("Sending data...\n");

    if (sendto(client_socket, buffer, BUFFER_SIZE, 0, (struct sockaddr *) &server_addr, server_addr_len) == -1) {
        //send to server
        perror("sendto");
        close(client_socket);
        exit(EXIT_FAILURE);
    }   
    printf("Data sent!\n");
    
       memset(buffer, 0, BUFFER_SIZE);
    if (recvfrom(client_socket, buffer, BUFFER_SIZE, 0, (struct sockaddr *) &server_addr, &server_addr_len) == -1)  {
        //receive from server
        perror("recvfrom");
        unlink(client_addr.sun_path);
        close(client_socket);
        exit(EXIT_FAILURE);
    }
    printf("Received message: %s\n", buffer);
    
    //remove the temporary socket file and close the socket
    unlink(client_addr.sun_path);
    close(client_socket);

    return 0;
}
