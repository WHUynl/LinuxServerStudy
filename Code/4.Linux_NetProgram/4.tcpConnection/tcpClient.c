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

    while(1){
        //发送数据
        char* data ="hello";
        write(sfd,data,strlen(data));

        //获取服务器数据
        char receive[1024];
        ret = read(sfd,&receive,sizeof(receive));
        if(ret<0){
            printf("读取失败\n");
        }else if (ret==0)
        {
            printf("客户端断开了链接\n");
        }else{
            printf("读取到%s\n",receive);
        }
        sleep(1);
    }

    close(sfd);

    return 0;
}