//
//  socket_udp.h
//  ConsolePrj
//
//  Created by Ryan on 12-8-16.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include <string.h>
#include <arpa/inet.h>
#include <sys/time.h>

#include <pthread.h>

#ifndef ConsolePrj_socket_udp_h
#define ConsolePrj_socket_udp_h
typedef int (*OnReceiveData)(void * data,size_t len,void * context);
class CTcpClient
{
public:
    CTcpClient();
    ~CTcpClient();
public:
    int setremoteaddress(char * ip,int port);
    int connect();
    int close();
    int send(void * pbuffer,long len);
    int receive(void * pbuffer,int len);
    int setCallBack(OnReceiveData pf,void * context=nullptr);
    virtual void * OnSocketData(void * data,long len);
protected:
    int m_socket;
    struct sockaddr_in m_remoteAddr;
    char m_szRemoteAddress[32];
    int m_iRemotePort;
    
protected:
    pthread_t m_hTrhead;
    static void * ReadDataInBackGround(void * arg);

protected:
    bool m_bReadDataInBackGround;
    OnReceiveData m_Callback;
    void * m_pCallbackContext;
};

#endif