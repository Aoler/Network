//
//  main.cpp
//  TcpClient
//
//  Created by Haitao on 21/8/17.
//  Copyright © 2017年 mess. All rights reserved.
//

#include <iostream>
#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<netinet/in.h>

void use(char* a)
{
    printf("#%s [port_server]\n",a);
}

int main(int argc,char* argv[])
{
    printf("main start\n");
    if(argc < 3)
    {
        use(argv[0]);
        return 3;
    }
    printf("use is ok\n");
    int sock = socket(AF_INET,SOCK_STREAM,0);
    if(sock < 0){
        perror("socket");
        return 1;
    }
    printf("create socket is ok\n");
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[2]));
    server.sin_addr.s_addr = inet_addr((argv[1]));
    int conn = connect(sock,(struct sockaddr*)&server , sizeof(server));
    if(conn < 0){
        perror("connect");
        close(sock);
        return 2;
    }
    while(1)
    {
        printf("please enter#");
        fflush(stdout);
        char buf[1024];
        fgets(buf,sizeof(buf),stdin);
        buf[strlen(buf) - 1] = '\0';
        write(sock,buf,sizeof(buf));
        char* str = "quit";
        if(strcmp(buf,str) == 0)
        {
            break;
        }
        printf("server echo#");
        fflush(stdout);
        size_t r2 = read(sock , buf, sizeof(buf));
        if(r2 > 0){
            printf("%s\n",buf);
        }else{
            continue;
        }
    }
    close(sock);
    printf("client goodbye!\n");
    return 0;
}
