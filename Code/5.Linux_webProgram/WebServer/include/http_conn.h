#ifndef HTTP_CONN_H
#define HTTP_CONN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/epoll.h>
#include <sys/mman.h>
#include "locker.h"
#include <stdarg.h>

class http_conn
{
private:
    /* data */
    int m_sockfd; // 该HTTP连接的socket
    struct sockaddr_in m_address; //通信的socket地址

public:

    static int m_epollfd;// 所有的socket上的事件都被注册到同一个epollfd
    static int m_user_count; //统计用户的数量
    
    http_conn(/* args */);
    ~http_conn();

    //解析客户端的请求，处理客户端的请求
    void process();

    //初始化
    void init(int sockfd , const struct sockaddr_in& );

    //关闭连接
    void close_conn();

    //非阻塞读数据
    bool read();

    //非阻塞写数据
    bool write();
};



#endif