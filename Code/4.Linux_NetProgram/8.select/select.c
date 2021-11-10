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

    //创建一个fd_set的集合，存放需要检测文件描述符
    //rdset 只允许 clear和set 进行操作，内核只能修改tmp
    fd_set rdset,tmp;
    FD_ZERO(&rdset);
    FD_SET(lfd,&rdset);
    int maxfd = lfd;

    while(1){
        tmp = rdset;
        //调用select,让内核帮助检测哪些文件描述符有数据。
        int ret = select(maxfd+1,&tmp,NULL,NULL,NULL);
        if(ret == -1){
            perror("select");
        }else if(ret == 0){
            continue;
        }else{
            //说明有描述符发生了变化，但是select只是单纯的返回变化数量
            if(FD_ISSET(lfd,&tmp)){
                //表示有新的客户端连接进来
                struct sockaddr_in client;
                int len = sizeof(client);
                int cfd = accept(lfd,(struct sockaddr*)&client,&len);
                //新建的用于通信的文件描述符也要放入到fd_set中
                FD_SET(cfd,&rdset);
                maxfd = maxfd > cfd ? maxfd : cfd;
            }

            //监听的文件描述符用于是第一个
            for(int i = lfd+1;i<=maxfd;i++){
                if(FD_ISSET(i,&tmp)){
                    //说明客户端发来了数据。
                    char buf [1024]  = {0};
                    int len = read(i,buf,sizeof(buf));
                    if(len==-1){
                        perror("read");
                        return 0;
                    }else if(len==0){
                        printf("client closed\n");
                        close(i);
                        FD_CLR(i,&rdset);
                    }else{
                        printf("读到%s",buf);
                        write(i,buf,sizeof(buf));
                    }
                }
            }
        }
    }
    return 0;
}