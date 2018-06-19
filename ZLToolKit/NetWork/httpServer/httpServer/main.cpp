#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <cassert>
//#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>

#include "threadpool.h"
#include "http_conn.h"

#define MAX_FD 65536
#define MAX_EVENT_NUMBER 10000

extern int addfd( int epollfd, int fd, bool one_shot );
extern int removefd( int epollfd, int fd );

void show_error( int connfd, const char* info )
{
    printf( "%s", info );
    send( connfd, info, strlen( info ), 0 );
    close( connfd );
}


int main( int argc, char* argv[] )
{
    if( argc <= 2 )
    {
        printf( "usage: %s ip_address port_number\n", argv[0]);
        return 1;
    }
    const char* ip = argv[1];
    int port = atoi( argv[2] );
	

    threadpool< http_conn >* pool = NULL;
    try
    {
        pool = new threadpool< http_conn >(10,100000);
    }
    catch( ... )
    {
        return 1;
    }

	/*预先为每个可能的客户连接分配一个http_conn对象*/
    http_conn* users = new http_conn[ MAX_FD ];
    assert( users );
    int user_count = 0;

    int listenfd = socket( PF_INET, SOCK_STREAM, 0 );
    assert( listenfd >= 0 );
    struct linger tmp = { 1, 0 };
    setsockopt( listenfd, SOL_SOCKET, SO_LINGER, &tmp, sizeof( tmp ) );

    int ret = 0;
    struct sockaddr_in address;
    bzero( &address, sizeof( address ) );
    address.sin_family = AF_INET;
    inet_pton( AF_INET, ip, &address.sin_addr );
    address.sin_port = htons( port );

    ret = bind( listenfd, ( struct sockaddr* )&address, sizeof( address ) );
    assert( ret >= 0 );

    ret = listen( listenfd, 5 );
    assert( ret >= 0 );

    //epoll_event events[ MAX_EVENT_NUMBER ];
    //int epollfd = epoll_create( 5 );

    int epollfd = kqueue();
    
    
    assert( epollfd != -1 );
    addfd( epollfd, listenfd, false );
    http_conn::m_epollfd = epollfd;

    while( true )
    {
//        int number = epoll_wait( epollfd, events, MAX_EVENT_NUMBER, -1 );
//        if ( ( number < 0 ) && ( errno != EINTR ) )
//        {
//            printf( "epoll failure\n" );
//            break;
//        }
        long waitms=1000;
        struct timespec timeout;
        timeout.tv_sec = waitms / 1000;
        timeout.tv_nsec = (waitms % 1000) * 1000 * 1000;
        const int kMaxEvents = 20;
        struct kevent activeEvs[kMaxEvents];
        int number = kevent(epollfd, NULL, 0, activeEvs, kMaxEvents, &timeout);
        printf("epoll_wait return %d\n", number);
        
        if (number < 0) {
            printf("kevent 出错!\n");
            continue;
        }else if(number == 0){
            printf("kenvent 超时!\n");
            continue;
        }
        
        for (int i = 0; i < number; i ++) {
            int sockfd = (int)(intptr_t)activeEvs[i].udata; //对应的sockt
            int events = activeEvs[i].filter;  //事件类型
            size_t dataSize=activeEvs[i].data; //可读取活着可写的大小
            if (events == EVFILT_READ) {
                if (sockfd == listenfd) {
                    struct sockaddr_in client_address;
                    socklen_t client_addrlength = sizeof( client_address );
                    int connfd = accept( listenfd, ( struct sockaddr* )&client_address, &client_addrlength );
                    if ( connfd < 0 )
                    {
                        printf( "errno is: %d\n", errno );
                        continue;
                    }
                    if( http_conn::m_user_count >= MAX_FD )
                    {
                        show_error( connfd, "Internal server busy" );
                        continue;
                    }
                    /*初始化客户连接*/
                    users[connfd].init( connfd, client_address );
                } else {
                    /*根据读的结果，决定是将任务添加到线程池还是关闭连接*/
                    if( users[sockfd].read() )
                    {
                        pool->append( users + sockfd );
                    }
                    else
                    {
                        users[sockfd].close_conn();
                    }

                }
            } else if (events == EVFILT_WRITE) {
                /*根据写的结果，决定是否关闭连接*/
                if( !users[sockfd].write() )
                {
                    users[sockfd].close_conn();
                }
            } else {
                //exit_if(1, "unknown event");
            }
        }

    }

    close( epollfd );
    close( listenfd );
    delete [] users;
    delete pool;
    return 0;
}
