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

    int rvalue = bind(sfd,(struct sockaddr *)&addr,(socklen_t)sizeof(addr));
    
    if(rvalue==-1){
        printf("bind失败");
        return 0;
    }

    int ret = listen(sfd,5);

    if(ret==-1){
        printf("listen失败");
        return 0;
    }

    struct sockaddr_in cAddr;
    socklen_t len = sizeof(cAddr);
    int cfd =accept(sfd,(struct sockaddr*)&cAddr,&len);
    if(cfd==-1){
        printf("accept失败");
        return 0;
    }
    while(1){
        //输出客户端信息。
        char charIp[16];
        inet_ntop(AF_INET,&cAddr.sin_addr.s_addr,charIp,sizeof(cAddr));
        unsigned int cPort = ntohs(cAddr.sin_port);
        printf("客户端的ip: %s,端口号：%d\n",charIp,cPort);

        //获取客户端数据后，再返回数据
        char receive[1024];
        ret = read(cfd,&receive,sizeof(receive));
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
        write(cfd,receive,strlen(receive));
    }
    close(sfd);
    

    return 0;
}