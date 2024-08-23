#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <Multicast IP> <Interface IP> <Port>\n", argv[0]);
        exit(1);
    }

    const char *multicast_ip = argv[1];
    const char *interface_ip = argv[2];
    int port = atoi(argv[3]);

    int sock, uniSock;//sock for multicast, uniSock for unicast
    struct sockaddr_in addr, serverAddr, clientAddr;
    struct in_addr local_interface;
    char message[1024];

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        error("socket()");
    }
    
    uniSock = socket(PF_INET, SOCK_DGRAM, 0);

    local_interface.s_addr = inet_addr(interface_ip);
    if (setsockopt(sock, IPPROTO_IP, IP_MULTICAST_IF, (char *)&local_interface, sizeof(local_interface)) < 0) {
        error("setsockopt()");
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(argv[2]);
    serverAddr.sin_port = htons(port - 1); //5000 - 1
    memset(&serverAddr.sin_zero, '\0', sizeof(serverAddr.sin_zero));

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(multicast_ip);
    addr.sin_port = htons(port);
    
    bind(uniSock, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

    socklen_t addr_len = sizeof(addr);
    socklen_t client_addr_len = sizeof(clientAddr);

    while (1) {
    
        memset(message, 0, sizeof(message));
        int bytes_received = recvfrom(uniSock, message, sizeof(message) - 1, 0, (struct sockaddr *) &clientAddr,         &client_addr_len); //receive from unicast_sender
        if (bytes_received < 0) {
            error("recvfrom()");
        }
        message[bytes_received] = '\0';
        
        printf("Received msg from UDP client: %s\n",message);
        printf("Sending msg to the multicast receiver(s)\n");

        if (sendto(sock, message, strlen(message), 0, (struct sockaddr *) &addr, addr_len) < 0) {
            error("sendto()"); //send to multicast_receiver
        }
    }

    close(sock);
    return 0;
}
