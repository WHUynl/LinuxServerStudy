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

int main() {
    //创建socket,注意SOCK_DGRAM
    int lfd = socket(PF_INET,SOCK_DGRAM,0);

    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    inet_pton(AF_INET,"192.168.110.128",&saddr.sin_addr.s_addr);
    saddr.sin_port = htons(9999);

    //绑定
    bind(lfd,(struct sockaddr *)&saddr,sizeof(saddr));
    
    while(1){
        char buf[128];
        struct sockaddr_in cliaddr;
        socklen_t len = sizeof(cliaddr);
        int ret = recvfrom(lfd,buf,sizeof(buf),0,(struct sockaddr *)&cliaddr,&len);
        if(ret == -1){
            perror("recvfrom\n");
        }

        printf("客户端数据%s\n",buf);

        //发送数据
        sendto(lfd,buf,sizeof(buf),0,(struct sockaddr *)&cliaddr,len);

        
    }
    close(lfd);
    return 0;
}