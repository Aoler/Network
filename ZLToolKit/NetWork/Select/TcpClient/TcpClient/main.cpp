//
//  main.cpp
//  TcpClient
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
        fd_set readset,writeset;
        FD_ZERO(&readset);            //每次循环都要清空集合，否则不能检测描述符变化
        FD_SET(sk, &readset);     //添加描述符
        FD_ZERO(&writeset);
        FD_SET(sk,&writeset);
        
        int maxfd = sock+1;//sockfd > fp ? (sockfd+1) : (fp+1);    //描述符最大值加1
        
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
                if (FD_ISSET(sock, &readset))  //测试sock是否可读，即是否网络上有数据
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
                    //处理收到的pbuffer
                    delete[] pbuffer;
                }
        }
        timeout.tv_sec = 0;    // 必须重新设置，因为超时时间到后会将其置零
    }
    close(sock);
    printf("client goodbye!\n");
    return 0;
}
