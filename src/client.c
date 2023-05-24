#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <math.h>
#include <time.h>
#include <netdb.h>

#include "client.h"

char* resolveHost(const char* host_name) {
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    char *IPbuffer = NULL;

    // Set the parameters for addrinfo
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;    // Allow IPv4 or IPv6
    hints.ai_socktype = SOCK_DGRAM;    // Use UDP

    // Use getaddrinfo to resolve the host name
    int status = getaddrinfo(host_name, NULL, &hints, &result);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }

    // Iterate through the results and find the first valid IP address
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        void* addr;
        char ipstr[INET6_ADDRSTRLEN];

        // Get the pointer to the IP address
        if (rp->ai_family == AF_INET) {    // IPv4
            struct sockaddr_in* ipv4 = (struct sockaddr_in*)rp->ai_addr;
            addr = &(ipv4->sin_addr);
        } else {    // IPv6
            struct sockaddr_in6* ipv6 = (struct sockaddr_in6*)rp->ai_addr;
            addr = &(ipv6->sin6_addr);
        }

        // Convert the IP address to a string
        if (inet_ntop(rp->ai_family, addr, ipstr, sizeof(ipstr)) != NULL) {
            // Allocate memory and copy the IP address to the buffer
            IPbuffer = malloc(strlen(ipstr) + 1);
            strcpy(IPbuffer, ipstr);
            break;
        }
    }

    // Free the resolved results
    freeaddrinfo(result);

    return IPbuffer;
}

EchoClient* newEchoClient(const char* serverHost, int serverPort, int maxRetry) {
    EchoClient* client = (EchoClient *)malloc(sizeof(EchoClient));
    client->serverHost = serverHost;
    client->serverPort = serverPort;
    client->maxRetry = maxRetry;
    srand(time(NULL));
    
    return client;
}

int generateRandomNumber(int input) {
    // Generate a random number between -input and input
    int randomNumber = rand() % (input * 2 + 1) - input;

    return randomNumber;
}

void sendMessage(EchoClient* client, const char* message) {
    int sockfd;
    int retryCount = 0;
    int base = INITIAL_RETRY_INTERVAL;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    char* serverIP = resolveHost(client->serverHost);

    // Create UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    // Set server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(client->serverPort);
    server_addr.sin_addr.s_addr = inet_addr(serverIP);

    while (1) {
        if (retryCount >= client->maxRetry) {
            fprintf(stderr, "Reach max-retry\n");
            exit(1);
        }

        // Send the message to the server
        if (sendto(sockfd, message, strlen(message), 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
            perror("sendto");
            exit(1);
        }

        fprintf(stdout, "Sent message to server: %s\n", message);

        // Set non-blocking mode and wait for a response
        fd_set readSet;
        FD_ZERO(&readSet);
        FD_SET(sockfd, &readSet);

        struct timeval timeout;
        timeout.tv_sec = 1;  // Wait for one second
        timeout.tv_usec = 0;

        int selectResult = select(sockfd + 1, &readSet, NULL, NULL, &timeout);

        if (selectResult == -1) {
            perror("select");
            exit(1);
        }

        if (selectResult == 0) {
            // Time has elapsed, retry
            retryCount++;
            fprintf(stdout, "Retry count: %d\n", retryCount);

            // Calculate the next retry interval using the formula: wait_interval = (base * pow(2, n)) +/- (random_interval)
            int n = retryCount - 1;
            int random_interval = generateRandomNumber(base);
            int wait_interval = (base * pow(2, n));

            // Add Jitter
            wait_interval += random_interval;
            
            // Limit the maximum wait interval
            wait_interval = fmin(wait_interval, MAX_WAIT_INTERVAL);

            // Delay for the wait interval
            usleep(wait_interval * 1000);

            continue;
        }

        // Receive the echoed message
        ssize_t recv_len = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, NULL, NULL);

        if (recv_len == -1) {
            perror("recvfrom");
            exit(1);
        }

        buffer[recv_len] = '\0';
        fprintf(stdout, "Received echo message from server: %s\n", buffer);

        // If a response is received, break out of the retry loop
        break;
    }

    // Close the socket
    close(sockfd);
    free(serverIP);
}

void cleanupEchoClient(EchoClient* client) {
    if (client) free(client);
}
