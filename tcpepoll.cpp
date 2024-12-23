#include "EchoServer.h"
#include <signal.h>

EchoServer *echoserver;
void Stop(int sig)
{
    printf("sig=%d\n", sig);
    echoserver->Stop();
    delete echoserver;
    exit(0);
}

int main(int argc, char *argv[])
{
    if(argc != 3)
    {
        return -1;
    }
    signal(SIGTERM, Stop);
    signal(SIGINT, Stop);

    echoserver = new EchoServer(argv[1], atoi(argv[2]), 3, 5);
    echoserver->Start();

    return 0;
}