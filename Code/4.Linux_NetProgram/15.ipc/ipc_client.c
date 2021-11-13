#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/un.h>

int main() {
    //删除上次遗留的套接字文件，防止通信失败
    unlink("client.sock");
    int sfd = socket(AF_LOCAL,SOCK_STREAM,0);
    if (sfd==-1)
    {
        printf("socket建立失败");
        return 0;
    }

    //绑定本地套接字文件
    struct sockaddr_un addr;
    addr.sun_family = AF_LOCAL;
    strcpy(addr.sun_path,"client.sock");

    int rvalue = bind(sfd,(struct sockaddr *)&addr,(socklen_t)sizeof(addr));
    
    if(rvalue==-1){
        printf("bind失败");
        return 0;
    }


    //连接服务器套接字文件
    struct sockaddr_un saddr;
    saddr.sun_family = AF_LOCAL;
    strcpy(saddr.sun_path,"server.sock");


    int ret = connect(sfd,(struct sockaddr*)&saddr,(socklen_t)sizeof(saddr));
    if(ret<0){
        printf("连接失败");
        return 0;
    }

    while(1){
        //发送数据
        char* data ="hello";
        send(sfd,data,strlen(data),0);

        //获取服务器数据
        char receive[1024];
        ret = recv(sfd,&receive,sizeof(receive),0);
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