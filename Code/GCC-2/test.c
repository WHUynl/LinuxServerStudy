#include <stdio.h>

int main(){
    int a =10;
#ifdef DEBUG
    printf("这是DEBUG模式");
#endif
    for(int i=0;i<3;++i){
        printf("hello,GCC!\n");
    }
    return 0;
}