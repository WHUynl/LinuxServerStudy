#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

int main() {
    int sfd = socket(AF_INET,SOCK_STREAM,0);
    if (sfd==-1)
    {
        printf("socket建立失败");
        return 0;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    inet_pton(AF_INET,"192.168.110.128",&addr.sin_addr.s_addr);
    addr.sin_port = htons(9999);

    int ret = connect(sfd,(struct sockaddr*)&addr,(socklen_t)sizeof(addr));
    if(ret<0){
        printf("连接失败");
        return 0;
    }

    while(1) {
        char sendBuf[1024] = {0};
        fgets(sendBuf, sizeof(sendBuf), stdin);

        write(sfd, sendBuf, strlen(sendBuf) + 1);

        // 接收
        int len = read(sfd, sendBuf, sizeof(sendBuf));
        if(len == -1) {
            perror("read");
            return -1;
        }else if(len > 0) {
            printf("read buf = %s\n", sendBuf);
        } else {
            printf("服务器已经断开连接...\n");
          
        }
    }
    close(sfd);

    return 0;
}