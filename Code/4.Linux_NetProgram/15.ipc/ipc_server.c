#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/un.h>


int main() {
    //删除上次遗留的套接字文件，防止通信失败
    unlink("server.sock");
    int sfd = socket(AF_LOCAL,SOCK_STREAM,0);
    if (sfd==-1)
    {
        printf("socket建立失败");
        return 0;
    }

    //绑定本地套接字文件
    struct sockaddr_un addr;
    addr.sun_family = AF_LOCAL;
    strcpy(addr.sun_path,"server.sock");

    int rvalue = bind(sfd,(struct sockaddr *)&addr,(socklen_t)sizeof(addr));
    
    if(rvalue==-1){
        printf("bind失败");
        return 0;
    }


    //监听套接字文件
    int ret = listen(sfd,5);

    if(ret==-1){
        printf("listen失败");
        return 0;
    }

    struct sockaddr_un cAddr;
    socklen_t len = sizeof(cAddr);
    int cfd =accept(sfd,(struct sockaddr*)&cAddr,&len);
    if(cfd==-1){
        printf("accept失败");
        return 0;
    }
    while(1){
        //获取客户端数据后，再返回数据
        char receive[1024];
        ret = recv(cfd,receive,sizeof(receive),0);
        if(ret<0){
            printf("读取失败\n");
        }else if (ret==0)
        {
            printf("客户端断开了链接\n");
            close(cfd);
            break;
        }else{
            printf("读取到%s\n",receive);
        }
        
        //发送数据
        send(cfd,receive,strlen(receive),0);
    }
    close(sfd);
    

    return 0;
}