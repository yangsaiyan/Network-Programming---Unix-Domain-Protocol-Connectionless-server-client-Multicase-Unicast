#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_PATH "tpf_unix_sock.server"
#define BUFFER_SIZE 256

int main(void){
    int server_socket;
    struct sockaddr_un server_sockaddr, client_sockaddr;
    socklen_t client_addr_len = sizeof(client_sockaddr); 
    char buffer[BUFFER_SIZE];

    if ((server_socket = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1){
        perror("socket");
        exit(EXIT_FAILURE);
    }
    
    unlink(SOCKET_PATH);
    
    memset(&server_sockaddr, 0, sizeof(struct sockaddr_un));
    server_sockaddr.sun_family = AF_UNIX;
    strcpy(server_sockaddr.sun_path, SOCKET_PATH); 
        
    if ((bind(server_socket, (struct sockaddr *) &server_sockaddr, sizeof(struct sockaddr_un))) == -1){
        perror("bind");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
    
    printf("Server is ready to receive messages...\n");
    
    memset(buffer, 0, BUFFER_SIZE);
    if (recvfrom(server_socket, buffer, BUFFER_SIZE, 0, (struct sockaddr *) &client_sockaddr, &client_addr_len) == -1) {
        perror("recvfrom");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
    printf("Received message: %s\n", buffer);
    
    if (sendto(server_socket, buffer, BUFFER_SIZE, 0, (struct sockaddr *) &client_sockaddr, client_addr_len) == -1) {
        
        perror("sendto");
    }   
    
    close(server_socket);
    unlink(SOCKET_PATH);

    return 0;
}
