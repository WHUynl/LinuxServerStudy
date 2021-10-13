/*
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <unistd.h>

    int stat(const char *pathname, struct stat *statbuf);
        作用：获取一个文件相关的一些信息
        参数:
            - pathname：操作的文件的路径
            - statbuf：结构体变量，传出参数，用于保存获取到的文件的信息
        返回值：
            成功：返回0
            失败：返回-1 设置errno

    int lstat(const char *pathname, struct stat *statbuf);
        作用：获取软链接文件的相关信息
        参数:
            - pathname：操作的文件的路径
            - statbuf：结构体变量，传出参数，用于保存获取到的文件的信息
        返回值：
            成功：返回0
            失败：返回-1 设置errno

*/

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>

int main() {

    struct stat statbuf;

    int ret = stat("a.txt", &statbuf);

    if(ret == -1) {
        perror("stat");
        return -1;
    }
    //获得文件大小
    printf("size: %ld\n", statbuf.st_size);
    //获得文件的类型,int值为32768转化为2进制，得到是普通文件
    printf("type: %d\n",statbuf.st_mode&S_IFMT);
    //获得文件的读写权利（此处用others的执行权利为例），得到是0
    printf("mode: %d\n",statbuf.st_mode&S_IXOTH);

    return 0;
}