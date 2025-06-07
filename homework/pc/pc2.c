#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define BUFFER_SIZE 4
#define TOTAL_CHARS 8

// 缓冲区定义
char buffer1[BUFFER_SIZE], buffer2[BUFFER_SIZE];
int in1 = 0, out1 = 0;  // buffer1指针
int in2 = 0, out2 = 0;  // buffer2指针

// 信号量定义
sem_t mutex1, empty1, full1;  // buffer1信号量
sem_t mutex2, empty2, full2;  // buffer2信号量

void* producer(void* arg) {
    char chars[] = {'a','b','c','d','e','f','g','h'};
    for(int i=0; i<TOTAL_CHARS; i++) {
        sem_wait(&empty1);  // 等待buffer1空位
        sem_wait(&mutex1);  // 获取buffer1互斥锁
        
        buffer1[in1] = chars[i];
        in1 = (in1 + 1) % BUFFER_SIZE;
        
        sem_post(&mutex1);
        sem_post(&full1);   // 通知buffer1有数据
    }
    return NULL;
}

void* calculator(void* arg) {
    for(int i=0; i<TOTAL_CHARS; i++) {
        // 从buffer1取数据
        sem_wait(&full1);   // 等待buffer1数据
        sem_wait(&mutex1);
        
        char c = buffer1[out1];
        out1 = (out1 + 1) % BUFFER_SIZE;
        
        sem_post(&mutex1);
        sem_post(&empty1);  // 释放buffer1空位
        
        // 转换并放入buffer2
        sem_wait(&empty2);  // 等待buffer2空位
        sem_wait(&mutex2);  // 获取buffer2互斥锁
        
        buffer2[in2] = c - 32;  // 小写转大写
        in2 = (in2 + 1) % BUFFER_SIZE;
        
        sem_post(&mutex2);
        sem_post(&full2);   // 通知buffer2有数据
    }
    return NULL;
}

void* consumer(void* arg) {
    for(int i=0; i<TOTAL_CHARS; i++) {
        sem_wait(&full2);   // 等待buffer2数据
        sem_wait(&mutex2);
        
        char c = buffer2[out2];
        out2 = (out2 + 1) % BUFFER_SIZE;
        
        sem_post(&mutex2);
        sem_post(&empty2);  // 释放buffer2空位
        
        printf("%c\n", c);
    }
    return NULL;
}

int main() {
    // 初始化信号量
    sem_init(&mutex1, 0, 1);
    sem_init(&empty1, 0, BUFFER_SIZE);  // 初始空位数量
    sem_init(&full1, 0, 0);
    sem_init(&mutex2, 0, 1);
    sem_init(&empty2, 0, BUFFER_SIZE);
    sem_init(&full2, 0, 0);

    pthread_t producer_tid, calculator_tid, consumer_tid;
    
    // 创建线程
    pthread_create(&producer_tid, NULL, producer, NULL);
    pthread_create(&calculator_tid, NULL, calculator, NULL);
    pthread_create(&consumer_tid, NULL, consumer, NULL);

    // 等待线程结束
    pthread_join(producer_tid, NULL);
    pthread_join(calculator_tid, NULL);
    pthread_join(consumer_tid, NULL);

    // 销毁信号量
    sem_destroy(&mutex1);
    sem_destroy(&empty1);
    sem_destroy(&full1);
    sem_destroy(&mutex2);
    sem_destroy(&empty2);
    sem_destroy(&full2);

    return 0;
}