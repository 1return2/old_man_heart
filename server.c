#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h> 
#include <arpa/inet.h> 
#include <sys/select.h>
#include <sys/time.h>
#include <poll.h>
#include <sys/epoll.h>
#include <json-c/json.h>
#define MAXLNE  4096
#define POLLSIZE 1024
int main(int argc, char **argv) 
{
    int listenfd, connfd, n;
    struct sockaddr_in servaddr;
    char *buff = (char *)malloc(sizeof(char) * 1024);
 
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
        return 0;
    }
 
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(atoi(argv[1]));
 
    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
        printf("bind socket error: %s(errno: %d)\n", strerror(errno), errno);
        return 0;
    }
 
    if (listen(listenfd, 10) == -1) {
        printf("listen socket error: %s(errno: %d)\n", strerror(errno), errno);
        return 0;
    }
    printf("listen...\n");


    struct pollfd pollarrfd[POLLSIZE] = {0};
    int maxfd = listenfd;
    char clie_IP[BUFSIZ];
    pollarrfd[0].fd = listenfd;
    pollarrfd[0].events = POLLIN;
    struct sockaddr_in sockclient;
    socklen_t len = sizeof(sockclient);
    while(1)
    {
        int Numready = poll(pollarrfd, maxfd+1, -1);
        if(Numready < 0)
        {
            printf("poll error\n");
            exit(1);
        }
        if(pollarrfd[0].revents & POLLIN)
        {
            connfd = accept(listenfd, (struct sockaddr*)&sockclient, &len);
            if(connfd < 0)
            {
                printf("accept error\n");
                exit(1);
            }
            printf("accept\n");
            printf("------connfd:%d,client ip:%s, port:%d---:\n", connfd, inet_ntop(AF_INET, &sockclient.sin_addr.s_addr,
                                                                        clie_IP, sizeof(clie_IP)), ntohs(sockclient.sin_port));
            pollarrfd[connfd].fd = connfd;
            pollarrfd[connfd].events = POLLIN;
            if(connfd > maxfd)maxfd = connfd;
            if(--Numready == 0)continue;
        }
        for(int i = 1; i<=maxfd; i++)
        {
            if(pollarrfd[i].revents & POLLIN)
            {
                n = recv(i, buff, sizeof(buff), 0);
                if(n<0)
                {
                    printf("recv error\n");
                    exit(1);
                }
                if(n>0)
                {
                    buff[n] = '\0';
                    printf("msg from client[%d]:%s\n", i, buff);
                }
                if(n==0)
                {
                    close(i);
                    printf("socket[%d] close\n", i);
                }
            }
        }
    }
    

 
    close(listenfd);
    return 0;
}




