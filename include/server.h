#include <arpa/inet.h>

#define BUFFER_SIZE 1024

typedef struct {
    int sockfd;
    struct sockaddr_in server_addr;
} EchoServer;

EchoServer* newEchoServer(int);
void runEchoServer(EchoServer*);
void cleanupEchoServer(EchoServer*);