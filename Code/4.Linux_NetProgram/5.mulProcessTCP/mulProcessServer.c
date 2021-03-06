#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <wait.h>
#include <errno.h>
//使用信号捕捉来回收子进程
void recyleChild(int arg){
    //未决信号集只能处理一个
    while(1){
        int ret = waitpid(-1,NULL,WNOHANG);
        if(ret ==-1){
            //所有均已回收
            break;
        }else if(ret == 0){
            //还有或者的子进程
            break;
        }else{
            printf("子进程%d被回收\n",arg);
        }
    }
}

int main() {

    //注册新号
    struct sigaction act;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    act.sa_handler  = recyleChild;
    sigaction(SIGCHLD,&act,NULL); 

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

    int ret = listen(sfd,128);

    if(ret==-1){
        printf("listen失败");
        return 0;
    }


    //不断循环等待客户端接收连接
    while(1){
        struct sockaddr_in cAddr;
        socklen_t len = sizeof(cAddr);
        //注意在accept软中断后(如使用信号回收子进程)，恢复时则accept不再阻塞，导致cfd报错，从而退出主进程
        int cfd =accept(sfd,(struct sockaddr*)&cAddr,&len);
        if(cfd==-1){
            //通过判断错误类型解决，回收子进程带来的报错
            if(errno == EINTR){
                continue;
            }
            printf("accept失败");
            return 0;
        }
        //接收成功后，则创建一个子进程来处理
        pid_t pid = fork();
        if(pid==0){
            //子进程获取客户端的ip
            char charIp[16];
            inet_ntop(AF_INET,&cAddr.sin_addr.s_addr,charIp,sizeof(cAddr));
            unsigned int cPort = ntohs(cAddr.sin_port);
            printf("客户端的ip: %s,端口号：%d\n",charIp,cPort);

            while(1){
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
            }
    }

    

    
    close(sfd);
    

    return 0;
}