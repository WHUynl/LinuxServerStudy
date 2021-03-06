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
#include <sys/poll.h>

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
    //初始化检测的文件描述符数组
    struct pollfd fds[2048];
    for(int i=0;i<2048;i++){
        fds[i].fd = -1;
        fds[i].events = POLLIN;
    }
    //第0个还是监听的文件描述符
    fds[0].fd = lfd;
    int nfds = 0;
    while(1){
        int ret = poll(fds,nfds+1,-1);
        if(ret == -1){
            perror("poll");
        }else if(ret == 0){
            continue;
        }else{
            //使用 & 运算来检测返回值，千万不能使用 ==
            if(fds[0].revents & POLLIN ){
                //表示有新的客户端连接进来
                struct sockaddr_in client;
                int len = sizeof(client);
                int cfd = accept(lfd,(struct sockaddr*)&client,&len);
                //新建的用于通信的文件描述符也要放入到数组中

                for(int i =1;i<2048;i++){
                    if(fds[i].fd == -1){
                        fds[i].fd = cfd;
                        fds[i].events = POLLIN;
                        break;
                    }
                }
                nfds = nfds > cfd ? nfds : cfd;
            }

            //监听的文件描述符用于是第一个
            for(int i = 1;i<=nfds;i++){
                if(fds[i].revents & POLLIN){
                    //说明客户端发来了数据。
                    char buf [1024]  = {0};
                    int len = read(fds[i].fd,buf,sizeof(buf));
                    if(len==-1){
                        perror("read");
                        return 0;
                    }else if(len==0){
                        printf("client closed\n");
                        close(fds[i].fd);
                        fds[i].fd = -1;
                    }else{
                        printf("读到%s",buf);
                        write(fds[i].fd,buf,sizeof(buf));
                    }
                }
            }
        }
    }
    return 0;
}