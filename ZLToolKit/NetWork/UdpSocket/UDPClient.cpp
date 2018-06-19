//
//  UDPClient.cpp
//  UDPClient
//
//  Created by Haitao on 24/6/16.
//  Copyright © 2016年 mess. All rights reserved.
//

#include "UDPClient.h"


CUdpClient::CUdpClient()
{
    memset(&server, 0,sizeof(server));
    memset(&client, 0,sizeof(client));
}

CUdpClient::~CUdpClient()
{
    this->close();

}

int CUdpClient::open(const char * ip,const int port)
{
    this->setremoteaddress(ip, port);
    int state=this->connect();
    if (0==state) {
        printf("[UDP Client]connect sucessful\n");
    }
    return 0;

}


int CUdpClient::setremoteaddress(const char * ip,const int port)
{
    bzero(&server,sizeof(server));
    server.sin_family=AF_INET;
    server.sin_port=htons(port);
    server.sin_addr.s_addr= inet_addr(ip);
    return 0;
}

int CUdpClient::connect()
{
    if((m_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        printf("[UDP Client]Creating UDP socket failed\n");
        return -1;
        
    }
    return 0;
}

int CUdpClient::send(void *pbuffer, long len)
{
    long n=sendto(m_socket, pbuffer, len, 0, (struct sockaddr *)&server, sizeof(server));
    if (-1==n) {
        printf("[UDP Client]call sendto error");
        return -1;
    }
    return 0;
}

int CUdpClient::receive(void *pbuffer, int len)
{
    socklen_t addrlen;
    long num =recvfrom(m_socket,pbuffer,len,MSG_DONTWAIT,(struct sockaddr*)&client,&addrlen);
    if (-1==num) {
        printf("[UDP Client]call recvfrom error");
        return -1;
    }
    
    return 0;
}

int CUdpClient::close()
{
    ::close(m_socket);
    return 0;
}


