//
//  UDPClient.hpp
//  UDPClient
//
//  Created by Haitao on 24/6/16.
//  Copyright © 2016年 mess. All rights reserved.
//

#ifndef UDPClient_hpp
#define UDPClient_hpp

#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<arpa/inet.h>

class CUdpClient
{
public:
    CUdpClient();
    ~CUdpClient();
public:
    int open(const char * ip,const int port);
    int setremoteaddress(const char * ip,const int port);
    int connect();
    int close();
    int send(void * pbuffer,long len);
    int receive(void * pbuffer,int len);
protected:
    int m_socket;
    struct sockaddr_in server;
    struct sockaddr_in client;


};

#endif /* UDPClient_hpp */
