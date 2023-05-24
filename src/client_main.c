#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "client.h"

EchoClient* client = NULL;

static void sig(int signum)
{
    if (client) cleanupEchoClient(client);

    fprintf(stdout, "\n[%d] Bye!\n", signum);
    exit(0);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <serverHost> <serverPort> <message>\n", argv[0]);
        return 1;
    }

    if (
        signal(SIGINT, sig) == SIG_ERR ||
        signal(SIGTERM, sig) == SIG_ERR
    ) {
        fprintf(stderr, "error: failed to bind signal handler\n");
        return 1;
    }

    const char* serverHost = argv[1];
    int serverPort = atoi(argv[2]);
    const char* message = argv[3];
    int maxRetry = 10;

    EchoClient* client = newEchoClient(serverHost, serverPort, maxRetry);
    sendMessage(client, message);
    cleanupEchoClient(client);

    return 0;
}