#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

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

    int sock;
    struct sockaddr_in addr;
    struct ip_mreq mc_req;
    char buffer[1024];
    int nbytes;
    socklen_t addrlen;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        error("socket()");
    }

    u_int yes = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&yes, sizeof(yes)) < 0) {
        error("Reusing ADDR failed");
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        error("bind()");
    }

    mc_req.imr_multiaddr.s_addr = inet_addr(multicast_ip);
    mc_req.imr_interface.s_addr = inet_addr(interface_ip);

    if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void *)&mc_req, sizeof(mc_req)) < 0) {
        error("setsockopt()");
    }

    addrlen = sizeof(addr);
    while (1) {
        nbytes = recvfrom(sock, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&addr, &addrlen);
        //receive from multicast_sender
        if (nbytes < 0) {
            error("recvfrom()");
        }
        buffer[nbytes] = '\0';
        printf("Received message: %s\n", buffer);
    }

    close(sock);
    return 0;
}
