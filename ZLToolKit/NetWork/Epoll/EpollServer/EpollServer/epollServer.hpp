//
//  main.cpp
//  EpollServer
//
//  Created by Haitao on 22/8/17.
//  Copyright © 2017年 mess. All rights reserved.
//
//
//#include <fcntl.h>
//#include<stdlib.h>
//#include<stdio.h>
//#include<string.h>
//#include<errno.h>
//#include<sys/socket.h>
//#include<sys/epoll.h>
//#include<sys/types.h>
//#include<netinet/in.h>
//#include<arpa/inet.h>
//#include<unistd.h>
//
//#define SIZE 60
//
//void usage(const char* argv)
//{
//    printf("%s:[ip] [port]\n", argv);
//}
//
//int startup(char* ip, int port) //创建监听套接字不解释
//{
//    int sock = socket(AF_INET, SOCK_STREAM, 0);
//    if (sock < 0)
//    {
//        perror("sock");
//        exit(2);
//    }
//    
//    int opt = 1;
//    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
//    
//    //2
//    struct sockaddr_in local;
//    local.sin_family = AF_INET;
//    local.sin_port = htons(port);
//    local.sin_addr.s_addr = inet_addr(ip);
//    
//    if (bind(sock, (struct sockaddr*)&local, sizeof(local)) < 0)
//    {
//        perror("bind");
//        exit(3);
//    }
//    if (listen(sock, 10) < 0)
//    {
//        perror("listen");
//        exit(4);
//    }
//    return sock;
//}
//
//int main(int argc, char* argv[])
//{
//    if (argc != 3)
//    {
//        usage(argv[0]);
//        exit(1);
//    }
//    int listen_sock = startup(argv[1], atoi(argv[2]));
//    int epoll = epoll_create(500); //创建epoll
//    
//    struct epoll_event ev;
//    ev.events = EPOLLIN;
//    ev.data.fd = listen_sock;
//    
//    epoll_ctl(epoll, EPOLL_CTL_ADD, listen_sock, &ev); //这里首先把监听套接字添加进红黑树
//    
//    struct epoll_event revs[SIZE];
//    int nums = 0;
//    int i = 0;
//    while (1)
//    {
//        switch (nums = epoll_wait(epoll, revs, SIZE, -1))
//        {
//            case -1:
//            {
//                perror("wait");
//                break;
//            }
//            case 0:
//            {
//                perror("time out");
//                break;
//            }
//            default:
//            {
//                struct sockaddr_in client;
//                socklen_t len = sizeof(client);
//                
//                for (i = 0; i < nums; i++)
//                {
//                    int rsock = revs[i].data.fd;
//                    if (rsock == listen_sock)
//                    {
//                        int new_sock = accept(listen_sock, (struct sockaddr*)&client, &len); //创建链接套接字
//                        
//                        if (new_sock > 0)
//                        {
//                            printf("get a new client:%s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
//                        }
//                        
//                        ev.events = EPOLLIN | EPOLLET;
//                        ev.data.fd = new_sock;
//                        
//                        epoll_ctl(epoll, EPOLL_CTL_ADD, new_sock, &ev);//将链接套接字存储在红黑当中，开始监视.
//                    }
//                    
//                    else
//                    {
//                        if (revs[i].events & EPOLLIN) // 读时间就绪
//                        {
//                            char buf[1024];
//                            ssize_t s = read(rsock, buf, sizeof(buf)-1);
//                            if (s > 0)
//                            {
//                                buf[s] = '\0';
//                                printf("client ->#%s\n", buf);
//                                
//                                ev.events = EPOLLOUT;
//                                ev.data.fd = rsock;
//                                epoll_ctl(epoll, EPOLL_CTL_MOD, rsock, &ev); //读完结束后 切换监视写状态.
//                            }
//                            else if (s == 0)
//                            {
//                                printf(" client  :%d is quit\n", rsock);
//                                epoll_ctl(epoll, EPOLL_CTL_DEL, rsock, NULL);
//                                close(rsock);
//                            }
//                            else
//                            {
//                                perror("read");
//                            }
//                        }
//                        else if (revs[i].events & EPOLLOUT)//这里用了一点html的小语句
//                        {
//                            const char* msg = "HTTP/1.0.200 OK\r\n\r\n<html><h2>shuowoailimengting</h2></html>\r\n";
//                            write(rsock, msg, strlen(msg));
//                            epoll_ctl(epoll, EPOLL_CTL_DEL, rsock, NULL);
//                            close(rsock);
//                        }
//                        else
//                        {
//                        }
//                    }
//                }
//                break;
//            }
//        }
//        
//    }
//}
