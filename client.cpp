#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>

int main (int argc, char* argv[])
{
    if(argc != 3)   return -1;

    int sockfd;
    struct sockaddr_in servaddr;
    char buf[1024];
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {printf("socket failed\n"); return -1;}

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);
    servaddr.sin_port = htons(atoi(argv[2]));

    if(connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0)
    {
        printf("connect(%s:%s) failed\n", argv[1], argv[2]); close(sockfd); return -1;
    }

    printf("connect ok\n");

    /*for(int i = 0; i < 20000000; ++i)
    {
        memset(buf, 0, sizeof(buf));
        printf("please input:"); scanf("%s", buf);

        if(send(sockfd, buf, strlen(buf), 0) <= 0)
        {
            printf("write failed\n"); close(sockfd); return -1;
        }

        memset(buf, 0, sizeof(buf));
        if(recv(sockfd, buf, sizeof(buf), 0) <= 0)
        {
            printf("read failed\n"); close(sockfd); return -1;
        }
        printf("recv:%s\n", buf);
    }*/
    for(int i = 0; i < 1; ++i)
    {
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "这是第%d个超级女生.", i);
        char tmpbuf[1024];
        memset(tmpbuf, 0, sizeof(tmpbuf));
        int len = strlen(buf);
        memcpy(tmpbuf, &len, sizeof(len));
        memcpy(tmpbuf+sizeof(len), buf, len);

        send(sockfd, tmpbuf, len + sizeof(len), 0);
       
    }

    for(int i = 0; i < 1; ++i)
    {
        int len;
        recv(sockfd, &len, sizeof(len), 0);
        memset(buf, 0, sizeof(buf));

        recv(sockfd, buf, len, 0);
        printf("recv:%s\n", buf);
    }
    return 0;
}