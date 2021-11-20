#include "include/http_conn.h"

int http_conn:: m_epollfd = -1;
int http_conn:: m_user_count =0; 

//设置文件描述符非阻塞
void setnonblocking(int fd){
    int old_flag = fcntl(fd,F_GETFL);
    int new_flag = old_flag|O_NONBLOCK;
    fcntl(fd,F_SETFL,new_flag);
}

//添加文件描述符到epoll
void addfd(int epollfd,int fd, bool one_shot){
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLRDHUP;

    if(one_shot){
        event.events|=EPOLLONESHOT;
    }


    epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&event);

    //设置文件描述符非阻塞,否则其没有数据时会阻塞在那里
    setnonblocking(fd);
}
//删除文件描述符
void removefd(int epollfd,int fd){
    epoll_ctl(epollfd,EPOLL_CTL_DEL,fd,NULL);
    close(fd);
}

//修改文件描述符,重置socket上EPOLLONESHOT事件，以确保下一次可读时EPOLLIN事件能被触发。
void modfd(int epollfd,int fd,int ev){
    epoll_event event;
    event.data.fd = fd;
    event.events = ev | EPOLLRDHUP;

}

//初始化连接
void http_conn::init(int sockfd , const struct sockaddr_in& addr){
    m_sockfd = sockfd;
    m_address = addr;

    //设置端口复用
    int reuse = 1;
    setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse));

    //添加到epoll对象中
    addfd(m_epollfd,sockfd,true);
    m_user_count++;//总用户数+1
}

//关闭连接
void http_conn::close_conn(){
    if(m_sockfd !=-1){
        removefd(m_epollfd,m_sockfd);
        m_sockfd = -1;
        m_user_count--;
    }
}

    //非阻塞读数据
bool http_conn::read(){
    printf("一次性读完数据\n");
    return true;
}

    //非阻塞写数据
bool http_conn::write(){
    printf("一次性写完数据\n");
    return true;
}
http_conn::http_conn(/* args */)
{
}

http_conn::~http_conn()
{
}

//由线程中的工作线程调用，这是处理HTTP请求的入口函数
void http_conn::process()
{
printf("解析HTTP请求\n");

printf("返回处理结果\n");

}