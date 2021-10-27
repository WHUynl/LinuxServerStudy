/*
    生产者消费者模型（自习粗略的版本）
*/
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

//创建信号量
pthread_mutex_t mutex;

struct Food{
    int num;
    char name;
    struct Food* next; 
};

struct Food* head = NULL;
struct Food* tail = NULL;
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
            tail = head;
        }else{
            tail->next = food;
            tail = tail->next;
        }
        printf("面包师%C做了第%d个食物。\n",name,num);
        ++num;
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