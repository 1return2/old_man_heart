#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <json-c/json.h>

int main(int argc, char ** argv)
{
    if(argc < 2)
    {
        printf("please input ip and port\n");
        exit(1);
    }
    int serverfd;
    serverfd = socket(AF_INET, SOCK_STREAM, 0);
    if(serverfd < 0)
    {
        printf("socket error\n");
        exit(1);
    }
    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[2]));
    int ret = inet_pton(AF_INET, argv[1], &serv_addr.sin_addr.s_addr);
    if(ret < 0)
    {
        printf("inet_pton error\n");
        exit(1);
    }
    ret = connect(serverfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if(ret < 0 )
    {
        printf("connect error\n");
        exit(1);
    } 



    while(1)
    {
        char Abuf[20];
        char Nbuf[20];
        memset(&Abuf, 0, sizeof(Abuf));
        memset(&Nbuf, 0, sizeof(Nbuf));
        struct json_object *json = json_object_new_object();
        printf("输入A\n");
        fgets(Abuf, sizeof(Abuf), stdin);
        printf("输入B\n");
        fgets(Nbuf, sizeof(Nbuf), stdin);


        json_object_object_add(json, "A", json_object_new_string(Abuf));
        json_object_object_add(json, "N", json_object_new_string(Nbuf));
       
        const char *buf = json_object_to_json_string(json);
        ret = send(serverfd, buf, strlen(buf), 0);
        if (-1 == ret)
        {
            perror("send");
            exit(1);
        }
        printf("字符串 %s 发送成功 长度 %ld!\n", buf, strlen(buf));
        memset(buf, 0, sizeof(buf));
        json_object_put(json);
    }
    close(serverfd);
    
    return 0;
}

