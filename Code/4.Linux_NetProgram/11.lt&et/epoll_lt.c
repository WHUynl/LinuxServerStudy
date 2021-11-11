#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <wait.h>
#include <errno.h>
#include <ctype.h>
#include <sys/select.h>
#include <sys/epoll.h>

int main() {
    //创建
    int lfd = socket(PF_INET,SOCK_STREAM,0);

    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    inet_pton(AF_INET,"192.168.110.128",&saddr.sin_addr.s_addr);
    saddr.sin_port = htons(9999);

    //绑定
    bind(lfd,(struct sockaddr *)&saddr,sizeof(saddr));

    //监听
    listen(lfd,8);
    //创建epoll实例
    int epfd = epoll_create(1);
    //将监听描述符相关信息加到epoll实例中
    struct epoll_event epev;
    epev.events = EPOLLIN;
    epev.data.fd  = lfd;
    epoll_ctl(epfd,EPOLL_CTL_ADD,lfd,&epev);
    struct epoll_event epevs[1024];
    while(1){
        
        int ret = epoll_wait(epfd,epevs,1024,-1);
        if(ret == -1){
            perror("epoll_wait");
            return 0;
        }else{
            //有数据发生改变了
            //监听的文件描述符用于是第一个
            printf("ret = %d\n",ret);
            for(int i = 0;i<ret;i++){
                //监听描述符发生改变，说明有新的连接
                if(epevs[i].data.fd == lfd){
                    struct sockaddr_in client;
                    socklen_t len = sizeof(client);
                    int cfd = accept(lfd,(struct sockaddr*)&client,&len);
                    //追加信息给epoll进行监听。
                    struct epoll_event epev;
                    epev.events = EPOLLIN;
                    epev.data.fd  = cfd;
                    epoll_ctl(epfd,EPOLL_CTL_ADD,cfd,&epev);
                }else{
                        if(epevs[i].events & EPOLLOUT){
                            continue;
                        }
                        //数据到达进行通信
                        char buf [5]  = {0};
                        int len = read(epevs[i].data.fd,buf,sizeof(buf));
                        if(len==-1){
                            perror("read");
                            return 0;
                        }else if(len==0){
                            printf("client closed\n");
                            close(epevs[i].data.fd);
                            epoll_ctl(epfd,EPOLL_CTL_DEL,epevs[i].data.fd,NULL);
                        }else{
                            printf("读到%s\n",buf);
                            write(epevs[i].data.fd,buf,sizeof(buf));
                        }
                    }                    
                }
            }
        }
    close(lfd);
    close(epfd);
    return 0;
}