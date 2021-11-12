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

    //设置广播属性
    int op = 1;
    setsockopt(lfd,SOL_SOCKET,SO_BROADCAST,&op,sizeof(op));

    //创建广播地址
    struct sockaddr_in cliaddr;
    cliaddr.sin_family = AF_INET;
    inet_pton(AF_INET,"192.168.110.128",&cliaddr.sin_addr.s_addr);
    cliaddr.sin_port = htons(9999);
    int number = 0;
    
    while(1){
        char sendBuf[128];
        sprintf(sendBuf, "hello , i am client %d", number++);
        //发送数据
        sendto(lfd,sendBuf,sizeof(sendBuf),0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));
        printf("服务端发送的数据：%s\n",sendBuf);
        sleep(1);
    }
    close(lfd);
    return 0;
}