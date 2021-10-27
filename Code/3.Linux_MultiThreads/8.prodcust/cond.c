/*
    生产者消费者模型（条件变量版本）
*/
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

//创建信号量
pthread_mutex_t mutex;

//创建条件变量
pthread_cond_t cond;

struct Food{
    int num;
    char name;
    struct Food* next; 
};

struct Food* head = NULL;
int num = 1;
void* makeFood(void* args){
    char name = *((char*)args);
    while (1)
    {
        pthread_mutex_lock(&mutex);
        struct Food* food = (struct Food*)malloc(sizeof(struct Food));
        food->name = name;
        food->num = num;
        food->next=NULL;
        if(head==NULL){
            head = food;
            
        }else{
            head->next = food;
            head = head->next;
        }
        printf("面包师%C做了第%d个食物。\n",name,num);
        ++num;
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
        sleep(num%5+1);
    }
    
}

void* takeFood(void* args){
    char name = *((char*)args);
    while (1)
    {
        pthread_mutex_lock(&mutex);
        if(head==NULL){
            printf("顾客%C没有拿走面包。\n",name);
            pthread_cond_wait(&cond,&mutex);
            pthread_mutex_unlock(&mutex);
        }else{
            printf("顾客%C拿走了面包师%C做的第%d个食物。\n",name,head->name,head->num);
            struct Food* temp = head;
            head=head->next;
            free(temp);
            pthread_mutex_unlock(&mutex);
        }
        sleep(num%3+1);
    }
    
}

int main(){
    pthread_mutex_init(&mutex,NULL);
    pthread_t prod[5],cust[3];
    for(int i=0;i<5;i++){
        char name = (char)(i+'d');
        void* arg = &name;
        pthread_create(&prod[i],NULL,makeFood,arg);
        pthread_detach(prod[i]);
    }

    for(int i=0;i<3;i++){
        char name = (char)(i+'D');
        void* arg = &name;
        pthread_create(&cust[i],NULL,takeFood,arg);
        pthread_detach(cust[i]);
    }
        while(1) {
        sleep(10);
    }
    pthread_mutex_destroy(&mutex);
    pthread_exit(NULL);
    return 0;
}