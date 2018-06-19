//
//  socket_udp.cpp
//  ConsolePrj
//
//  Created by Ryan on 12-8-16.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#include "TcpClient.h"
#include <stdio.h>
#include <fcntl.h>

#include <errno.h>

#include <sys/ioctl.h>
#include <signal.h>
#include <iostream>

#include <assert.h>

#ifdef __FD_SETSIZE
#undef __FD_SETSIZE
#endif

#define __FD_SETSIZE    32768




CTcpClient::CTcpClient()
{
    m_socket = socket(AF_INET, SOCK_STREAM, 0);
    
    struct timeval tv;
    tv.tv_sec=2;
    tv.tv_usec=0;
    setsockopt(m_socket,SOL_SOCKET,SO_RCVTIMEO,&tv,sizeof(tv));
    setsockopt(m_socket,SOL_SOCKET,SO_SNDTIMEO,&tv,sizeof(tv));
    
    //initial remote address
    memset(&m_remoteAddr, 0,sizeof(m_remoteAddr));
    m_iRemotePort=0x00;
    memset(m_szRemoteAddress, 0, sizeof(m_szRemoteAddress));
    m_bReadDataInBackGround = false;
}

CTcpClient::~CTcpClient()
{
    this->close();
}

int CTcpClient::connect()
{
    //return ::connect(m_socket,(struct sockaddr *)&m_remoteAddr,sizeof(struct sockaddr));
    int status;
    fd_set set;
    FD_ZERO(&set);
    FD_SET(m_socket, &set);
    
    struct timeval  timeout;
    timeout.tv_sec = 3;
    timeout.tv_usec = 0;
    
    fcntl(m_socket, F_SETFL, O_NONBLOCK);

    if ( (status = ::connect(m_socket, (struct sockaddr*)&m_remoteAddr, sizeof(m_remoteAddr))) == -1)
    {
        if ( errno != EINPROGRESS )
        {
            printf("Connect failed,socket connect inprogress!\r\n");
            return status;
        }
        
        //status = select(m_socket+1, NULL, &set, NULL, &timeout);
        
        status =select(m_socket+1, NULL, &set, NULL, &timeout); //should use timeout
        
        if (status==0) //timeout
        {
            printf("Connect faile,timeout!");
            return -2;
        }
    }
    
    
    fcntl(m_socket, F_SETFL, fcntl(m_socket, F_GETFL, 0) & ~O_NONBLOCK);
    
    std::cout<<"[TCPIP] CTcpIp:Connect to device successful,with address:"<<m_szRemoteAddress<<":"<<m_iRemotePort<<std::endl;
    
    //Create thread for reading data at background
    m_bReadDataInBackGround = true;
    pthread_create(&m_hTrhead, nullptr, ReadDataInBackGround, this);
    //signal(SIGPIPE, SIG_IGN);
    return status;
}

int CTcpClient::close()
{
    m_bReadDataInBackGround = false;
    void * p;
    pthread_join(m_hTrhead, &p);
    return ::close(m_socket);  //close socket
}

int CTcpClient::setCallBack(OnReceiveData pf,void * context)
{
    m_Callback = pf;
    m_pCallbackContext = context;
    return 0;
}

void * CTcpClient::OnSocketData(void *data, long len)
{
    if (m_Callback) {
        m_Callback(data,len,m_pCallbackContext);
    }
    return 0;
}

void * CTcpClient::ReadDataInBackGround(void *arg)
{
    CTcpClient * pThis = (CTcpClient *)arg;
    while (pThis->m_bReadDataInBackGround) {
        pthread_testcancel();
        
        int sk = pThis->m_socket;
        fd_set readset,writeset;
        FD_ZERO(&readset);            //每次循环都要清空集合，否则不能检测描述符变化
        FD_SET(sk, &readset);     //添加描述符
        FD_ZERO(&writeset);
        FD_SET(sk,&writeset);
        
        int maxfd = sk+1;//sockfd > fp ? (sockfd+1) : (fp+1);    //描述符最大值加1
        
        struct timeval  timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;
        
        int ret = select(maxfd, &readset, NULL, NULL, &timeout);   // 非阻塞模式
        switch( ret)
        {
            case -1:
                //return -1;
                break;
            case 0:
                break;
            default:
                if (FD_ISSET(pThis->m_socket, &readset))  //测试sock是否可读，即是否网络上有数据
                {
                    int bytes;
                    int n = ioctl(sk, FIONREAD,&bytes);
                    if (n==-1) {
                        std::cout<<"[IO CTRL],get FIONREAD failed,with return:"<<errno<<std::endl;
                        continue;
                    }
                    if (bytes<=0) {
                        break;
                    }
                    unsigned char * pbuffer = new unsigned char[bytes];
                    memset(pbuffer, 0, bytes);
                    size_t recvbytes = recv(sk, pbuffer, bytes, MSG_WAITALL);
                    if (recvbytes <= 0 ) {
                        delete[] pbuffer;
                        break;
                    }
                    pThis->OnSocketData(pbuffer, recvbytes);
                    delete[] pbuffer;
                }
        }
        timeout.tv_sec = 0;    // 必须重新设置，因为超时时间到后会将其置零

        usleep(1000*1);
    }
    return nullptr;
}


int CTcpClient::setremoteaddress(char *ip, int port)
{
    //initial remote address
    //assert(ip,"Invalid ip address!");
    bzero(&m_remoteAddr, sizeof(struct sockaddr_in));
    m_remoteAddr.sin_family = AF_INET;
    m_remoteAddr.sin_port=htons(port);
    m_remoteAddr.sin_addr.s_addr=inet_addr(ip);
    strcpy(m_szRemoteAddress, ip);
    m_iRemotePort = port;
    return 0;
}
int CTcpClient::send(void *pbuffer, long len)
{
    //int ret = (int)sendto(m_socket, pbuffer, len, 0, (struct  sockaddr *)&m_remoteAddr, sizeof(struct sockaddr));
    int ret = (int)write(m_socket, pbuffer, len);
    if (ret<0) {
        std::cout<<"[TCPCLIENT]:Writ socket faile,with return error : "<<errno<<std::endl;
    }
    return ret;
}

int CTcpClient::receive(void *pbuffer, int len)
{
    //int ret = (int)recvfrom(m_socket, pbuffer, len, 0, (struct sockaddr *)&address, &fromlen);
    int ret = (int)recv(m_socket, pbuffer,len, 0);
    return ret;
}