#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include "server.h"

EchoServer* server = NULL;

static void sig(int signum)
{
    if (server) cleanupEchoServer(server);

    fprintf(stdout, "\n[%d] Bye!\n", signum);
    exit(0);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <serverPort>\n", argv[0]);
        return 1;
    }

    if (
        signal(SIGINT, sig) == SIG_ERR ||
        signal(SIGTERM, sig) == SIG_ERR
    ) {
        fprintf(stderr, "error: failed to bind signal handler\n");
        return 1;
    }

    // const char* serverIP = argv[1];
    int serverPort = atoi(argv[1]);

    server = newEchoServer(serverPort);
    runEchoServer(server);
    cleanupEchoServer(server);

    return 0;
}