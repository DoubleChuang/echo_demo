#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "server.h"


EchoServer* newEchoServer(int serverPort) {

    EchoServer *server = malloc(sizeof(EchoServer));
    
    // Create UDP socket
    if ((server->sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    // Set server address
    memset(&server->server_addr, 0, sizeof(server->server_addr));
    server->server_addr.sin_family = AF_INET;
    server->server_addr.sin_port = htons(serverPort);
    server->server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server->sockfd, (struct sockaddr *)&server->server_addr, sizeof(server->server_addr)) == -1) {
        perror("bind");
        exit(1);
    }

    return server;
}

void runEchoServer(EchoServer* server) {
    char buffer[BUFFER_SIZE];

    fprintf(stdout, "Echo Server is running...\n");

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);

        // Receive message from the client
        ssize_t recv_len = recvfrom(server->sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_len);

        if (recv_len == -1) {
            perror("recvfrom");
            exit(1);
        }

        buffer[recv_len] = '\0';
        fprintf(stdout, "Received message from client: %s\n", buffer);

        // Send the received message back to the client
        if (sendto(server->sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&client_addr, addr_len) == -1) {
            perror("sendto");
            exit(1);
        }

        fprintf(stdout, "Sent message back to client: %s\n", buffer);
    }

    // Close the socket
    close(server->sockfd);
}

void cleanupEchoServer(EchoServer* server) {
    if (server) free(server);
}
