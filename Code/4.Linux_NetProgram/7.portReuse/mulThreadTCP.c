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

struct sockInfo{
    int cfd; //客户端文件描述符
    pthread_t pid;
    struct sockaddr_in addr;
};

struct sockInfo sockInfos[128];

//子线程和客户端进行通信 cfd通信 cliadder客户端信息
void* working(void* arg){
    struct sockInfo* pInfo = (struct sockInfo*) arg;
    //子进程获取客户端的ip
    char charIp[16];
    inet_ntop(AF_INET,&(pInfo->addr.sin_addr.s_addr),charIp,sizeof(pInfo->addr.sin_addr.s_addr));
    unsigned int cPort = ntohs(pInfo->addr.sin_port);
    printf("客户端的ip: %s,端口号：%d\n",charIp,cPort);

            while(1){
                //获取客户端数据后，再返回数据
                char receive[1024];
                int ret = recv(pInfo->cfd,receive,sizeof(receive),0);
                
                if(ret<0){
                    printf("读取失败\n");
                }else if (ret==0)
                {
                    printf("客户端断开了链接\n");
                    close(pInfo->cfd);
                    break;
                }else{
                    printf("读取到%s\n",receive);
                }  
            // 小写转大写
            for(int i = 0; i < ret; ++i) {
                receive[i] = toupper(receive[i]);
            } 
            // 大写字符串发给客户端
            ret = send(pInfo->cfd, receive, strlen(receive) + 1, 0);
        }
}

int main() {
    //初始化sockInfos信息
    int max = sizeof(sockInfos)/sizeof(sockInfos[0]);
    for(int i=0;i<max;i++){
        bzero(&sockInfos[i],sizeof(sockInfos[i]));
        sockInfos[i].cfd = -1;
        sockInfos[i].pid = -1;
    }
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

    int optval = 1;
    //端口复用,防止服务器主动或者突然断开后，不能立即解绑端口,进而导致如果立即重新监听该端口，会bind失败
    setsockopt(sfd,SOL_SOCKET,SO_REUSEADDR,&optval,sizeof(optval));
    int rvalue = bind(sfd,(struct sockaddr *)&addr,(socklen_t)sizeof(addr));
    
    if(rvalue==-1){
        printf("bind失败");
        return 0;
    }

    int ret = listen(sfd,128);

    if(ret==-1){
        printf("listen失败");
        return 0;
    }
        while(1){
        struct sockaddr_in cAddr;
        socklen_t len = sizeof(cAddr);
        int cfd =accept(sfd,(struct sockaddr*)&cAddr,&len);
        if(cfd==-1){
            //通过判断错误类型解决，回收子进程带来的报错
            if(errno == EINTR){
                continue;
            }
            printf("accept失败");
            return 0;
        }
        struct sockInfo* pInfo;
        for(int i=0;i<max;i++){
            //找到空闲sockInfos
            if(sockInfos[i].cfd == -1){
                pInfo = &sockInfos[i];
                break;
            }
            if(i == max-1){
                sleep(1);
                i=0;
            }
        }
        pInfo->cfd = cfd;
        memcpy(&pInfo->addr,&cAddr,len);
        pthread_t pid;
        pid = pthread_create(&pInfo->pid,NULL,working,(void *)pInfo);
        pthread_detach(pInfo->pid);
    }
    
    close(sfd);

    return 0;
}