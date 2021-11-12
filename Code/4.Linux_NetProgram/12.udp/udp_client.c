#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

int main (){

    int sfd = socket(AF_INET,SOCK_DGRAM,0);
    if (sfd==-1)
    {
        printf("socket建立失败");
        return 0;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    inet_pton(AF_INET,"192.168.110.128",&addr.sin_addr.s_addr);
    addr.sin_port = htons(9999);
    int number = 0;
    while(1){
        char sendBuf[128];
        sprintf(sendBuf, "hello , i am client %d", number++);
        //发送数据
        sendto(sfd,sendBuf,sizeof(sendBuf),0,(struct sockaddr *)&addr,sizeof(addr));

                // 接收数据
        int num = recvfrom(sfd, sendBuf, sizeof(sendBuf), 0, NULL, NULL);
        printf("server say : %s\n", sendBuf);

        sleep(1);
    }

    close(sfd);
    return 0;
}