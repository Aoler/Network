//
//  main.cpp
//  TcpServer
//
//  Created by Haitao on 21/8/17.
//  Copyright © 2017年 mess. All rights reserved.
//

#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<netinet/in.h>

#define _BACKLOG_ 10
int getsock(int port)
{
    int sock = socket(AF_INET,SOCK_STREAM,0);
    if(sock < 0)
    {
        perror("socket");
        exit(1);
    }
    
    printf("%d: socket create is ok\n",sock);
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(sock,(struct sockaddr*)&server,sizeof(server)) < 0){
        perror("bind");
        close(sock);
        exit(2);
    }
    printf("bind is ok\n");
    if(listen(sock, _BACKLOG_) < 0){
        perror("listen");
        close(sock);
        return 3;
    }
    printf("listen is ok\n");
    return sock;
}

void use(const char* a)
{
    printf("#%s [port_server]\n",a);
}

int main(int argc,char* argv[])
{
    printf("main start\n");
    if(argc < 2)
    {
        use(argv[0]);
        exit(6);
    }
    int listen_sock = getsock(atoi(argv[1]));
    
    
    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    
    printf("wait accept.....\n");
    
    while(1)
    {
        int new_sock = accept(listen_sock,(struct sockaddr*)&client,&len);
        if(new_sock < 0)
        {
            perror("accept");
            close(new_sock);
            exit(4);
        }
        printf("[%s] [%d]:accept is ok\n",inet_ntoa(client.sin_addr),ntohs(\
                                                                           client.sin_port));
        pid_t pid = fork();
        if(pid < 0){
            close(new_sock);
            printf("process creation failed\n");
            continue;
        }
        else if(pid == 0){
            
            close(listen_sock);
            if(fork() > 0){
                exit(2);
            }
            else{
                
                while(1)
                {
                    fflush(stdout);
                    char buf[1024];
                    ssize_t i = read(new_sock, buf, sizeof(buf));
                    if( i > 0){
                        printf("[%s] [%d]:client say#%s\n",inet_ntoa(client.sin_addr),ntohs(client.sin_port),buf);
                    }
                    else if(i == 0){
                        close(new_sock);
                        printf("[%s] [%d]:client goodbye\n",inet_ntoa(client.sin_addr),ntohs(\
                                                                                             client.sin_port));
                        break;
                    }
                    else
                    {
                        perror("read");
                        break;
                    }
                    printf("Enter to [%s][%d]#",inet_ntoa(client.sin_addr),ntohs(\
                                                                                 client.sin_port));
                    fflush(stdout);
                    fgets(buf , sizeof(buf), stdin);
                    buf[strlen(buf) - 1] = '\0';
                    write(new_sock,buf,strlen(buf));
                }  
            }  
        }  
        else  
        {  
            close(new_sock);  
            waitpid(pid , NULL,0);  
        }  
    }  
    close(listen_sock);  
    return 0;
    
}
    
