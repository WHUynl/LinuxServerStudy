/*
    标准C库的进程退出的函数
    #include <stdlib.h>
    void exit(int status);

    Linux系统的进程退出函数
    #include <unistd.h>
    void _exit(int status);

    status参数：是进程退出时的一个状态信息。父进程回收子进程资源的时候可以获取到。
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {

/*
标准C库的exit()在调用系统的_exit()之前，还会：
    1.调用退出处理函数
    2.刷新缓冲区，并关闭文件描述符。
所以 exit(0)会打印world，但_exit(0)不会打印（world在缓冲区里未被刷新）
*/
    printf("hello\n");
    printf("world");

    // exit(0);
    _exit(0);
    
    return 0;
}