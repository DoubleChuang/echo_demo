#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <math.h>
#include <time.h>
#include <netdb.h>

#define BUFFER_SIZE 1024
#define INITIAL_RETRY_INTERVAL 500  // Initial retry interval (in milliseconds)
#define MAX_WAIT_INTERVAL 8000  // Maximum wait interval (in milliseconds)

typedef struct {
    const char* serverHost;
    int serverPort;
    const char* message;
    int maxRetry;
} EchoClient;

EchoClient* newEchoClient(const char*, int, int);
void sendMessage(EchoClient*, const char*);
void cleanupEchoClient(EchoClient*);
