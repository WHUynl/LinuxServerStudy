#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main (){
    //创建命名管道
    int ret = access("fifo1",F_OK);
    if(ret == -1){
        ret = mkfifo("fifo1",0777);
        if(ret == -1){
            perror("mkfifo1");
            exit(0);
        }
    }

    int ret2 = access("fifo2",F_OK);
    if(ret2 == -1){
        ret2 = mkfifo("fifo2",0777);
        if(ret2 == -1){
            perror("mkfifo2");
            exit(0);
        }
    }
    char bufW[128];
    char bufR[128];
    pid_t pid = fork();
    if(pid<0){
        perror("fork");
        exit(0);
    }

    if(pid>0){
        //B读1写2
        int fd1 = open("fifo1",O_RDONLY);
        if(fd1==-1){
            perror("open");
            exit(0);
        }
        printf("B父进程负责fifo1的读端\n");
        while(1){
            //从管道中读取数据
            memset(bufR, 0, 128);
            int ret = read(fd1, bufR, 128);
            if(ret <= 0) {
                perror("read");
                break;
            }
            printf("B的父进程收到的bufR: %s\n", bufR);
        }
    }else{
        int fd2 = open("fifo2",O_WRONLY);
        if(fd2==-1){
            perror("open");
            exit(0);
        }
        printf("B子进程负责fifo2的写端\n");
        while(1){

            memset(bufW,0,128);
            //获取标准输入的数据
            fgets(bufW,128,stdin);
            //向管道写数据
            int ret = write(fd2,bufW,strlen(bufW));
            if(ret == -1){
                perror("write");
                exit(0);
            }
            
        }
    }
    close(ret);
    close(ret2);
    return 0;
}