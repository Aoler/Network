//
//  main.cpp
//  TcpServer
//
//  Created by Haitao on 21/8/17.
//  Copyright © 2017年 mess. All rights reserved.
//

#include<stdio.h>
#include<sys/select.h>
#include<sys/time.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/socket.h>
#include<string.h>
#include<stdlib.h>
#include<arpa/inet.h>



static void usage(const char* proc)
{
    printf("Usage : %s [local_ip] [local_port]\n",proc);
}


int fds[sizeof(fd_set)*8];

int startup(char* ip,int port)
{
    int sock = socket(AF_INET,SOCK_STREAM,0);
    if(sock < 0){
        perror("socket");
        exit(2);
    }
    
    struct sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_port = htons(port);
    local.sin_addr.s_addr = inet_addr(ip);
    
    if(bind(sock, (struct sockaddr*)&local , sizeof(local))<0)
    {
        perror("bind");
        exit(3);
    }
    if(listen(sock ,10)< 0)
    {
        perror("lisen");
        exit(4);
    }
    return sock;
}

int main(int argc,char* argv[])
{
    if(argc != 3)
    {
        usage(argv[0]);
        return 1;
    }
    
    int listen_sock = startup(argv[1],atoi(argv[2]));
    fd_set rfds;
    fd_set wfds;
    int nums = sizeof(fds)/sizeof(fds[0]);
    int i =1;
    for(; i<nums; i++){
        fds[i] = -1;
    }
    
    while(1)
    {
        
        FD_ZERO(&rfds);
        FD_ZERO(&wfds);
        int max = -1;
        //struct timevlal timeout = {5.0};
        fds[0] = listen_sock;
        for(i = 0;i < nums;i++){
            if(fds[i] > -1){
                FD_SET(fds[i],&rfds);  //将读文件集合里的sock对应的bit置为1
                FD_SET(fds[i],&wfds);  //将写文件集合里的sock对应的bit置为1
                if(max < fds[i])
                {
                    max = fds[i];
                }
            }
        }
        switch(select(max+1,&rfds,&wfds,NULL,0))  //若有事件发生，对应文件描述符里sock对应的bit会置为1，没有事件，则置为0
        {
            case 0:
                printf( "chao  shi le");
                break;
            case -1:
                printf("select");
                break;
                
            default:
                i = 0;
                for(; i < nums ; i++)
                {
                    if(fds[i] == -1){
                        continue;
                    }
                    if(i == 0 && FD_ISSET(fds[i],&rfds))
                    {
                        struct sockaddr_in client;
                        socklen_t len = sizeof(client);
                        
                        int new_sock = accept(fds[i],(struct sockaddr*)&client,&len);
                        if(new_sock < 0)
                        {
                            perror("accept") ;
                            continue;
                        }
                        
                        printf("get a client[%s:%d]\n",inet_ntoa(client.sin_addr),ntohs(client.sin_port));
                        int j = 1;
                        for(;j< nums;j++){
                            if(fds[j] == -1){
                                break;   //如果连接队列没有满，继续accept
                            }
                        }
                        if(j == nums){
                            close(new_sock);  //如果sock队列已满，close new sock，拒绝连接
                        }else{
                            fds[j] = new_sock;
                        }
                    }
                    else if(i != 0&& FD_ISSET(fds[i],&rfds))  //对应的sock可以读，有数据
                    {
                        char buf[1024];
                        ssize_t s = read(fds[i],buf,sizeof(buf));
                        
                        if(s > 0)
                        {
                            printf("client# %s\n",buf);
                        }else if(s == 0)
                        {
                            printf("client is quit!!!!!\n");
                            
                            close(fds[i]);
                            fds[i] = -1;
                            
                        }
                        else{
                            perror("read error");
                            close(fds[i]);
                            fds[i] = -1;
                            
                        }
                    }
                    else if(i != 0&& FD_ISSET(fds[i],&wfds)) //对应的sock可以写
                    {
                    
                    }
                   
                }
                
        }  
    }  
    return 0;  
}
