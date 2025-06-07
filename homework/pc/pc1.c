#include <stdio.h>
#include <pthread.h>

#define BUFFER_SIZE 4
#define TOTAL_CHARS 8

// 缓冲区结构体
typedef struct {
    char data[BUFFER_SIZE];
    int count;
    int in;
    int out;
    pthread_mutex_t mutex;
    pthread_cond_t not_full;
    pthread_cond_t not_empty;
} Buffer;

Buffer buffer1 = { .count=0, .in=0, .out=0 };
Buffer buffer2 = { .count=0, .in=0, .out=0 };

// 生产者线程函数
void* producer(void *arg) {
    char chars[] = {'a','b','c','d','e','f','g','h'};
    
    for(int i=0; i<TOTAL_CHARS; i++) {
        pthread_mutex_lock(&buffer1.mutex);
        while(buffer1.count == BUFFER_SIZE)  // 缓冲区满等待
            pthread_cond_wait(&buffer1.not_full, &buffer1.mutex);
        
        buffer1.data[buffer1.in] = chars[i];
        buffer1.in = (buffer1.in + 1) % BUFFER_SIZE;
        buffer1.count++;
        
        pthread_cond_signal(&buffer1.not_empty); // 唤醒计算者
        pthread_mutex_unlock(&buffer1.mutex);
    }
    return NULL;
}

// 计算者线程函数
void* calculator(void *arg) {
    for(int i=0; i<TOTAL_CHARS; i++) {
        // 从buffer1取数据
        pthread_mutex_lock(&buffer1.mutex);
        while(buffer1.count == 0)
            pthread_cond_wait(&buffer1.not_empty, &buffer1.mutex);
        
        char c = buffer1.data[buffer1.out];
        buffer1.out = (buffer1.out + 1) % BUFFER_SIZE;
        buffer1.count--;
        
        pthread_cond_signal(&buffer1.not_full);
        pthread_mutex_unlock(&buffer1.mutex);

        // 转换并放入buffer2
        pthread_mutex_lock(&buffer2.mutex);
        while(buffer2.count == BUFFER_SIZE)
            pthread_cond_wait(&buffer2.not_full, &buffer2.mutex);
        
        buffer2.data[buffer2.in] = c - 32; // 小写转大写
        buffer2.in = (buffer2.in + 1) % BUFFER_SIZE;
        buffer2.count++;
        
        pthread_cond_signal(&buffer2.not_empty);
        pthread_mutex_unlock(&buffer2.mutex);
    }
    return NULL;
}

// 消费者线程函数
void* consumer(void *arg) {
    for(int i=0; i<TOTAL_CHARS; i++) {
        pthread_mutex_lock(&buffer2.mutex);
        while(buffer2.count == 0)
            pthread_cond_wait(&buffer2.not_empty, &buffer2.mutex);
        
        char c = buffer2.data[buffer2.out];
        buffer2.out = (buffer2.out + 1) % BUFFER_SIZE;
        buffer2.count--;
        
        pthread_cond_signal(&buffer2.not_full);
        pthread_mutex_unlock(&buffer2.mutex);
        
        printf("%c\n", c); // 打印大写字符
    }
    return NULL;
}

int main() {
    // 初始化同步工具
    pthread_mutex_init(&buffer1.mutex, NULL);
    pthread_mutex_init(&buffer2.mutex, NULL);
    pthread_cond_init(&buffer1.not_full, NULL);
    pthread_cond_init(&buffer1.not_empty, NULL);
    pthread_cond_init(&buffer2.not_full, NULL);
    pthread_cond_init(&buffer2.not_empty, NULL);

    pthread_t producer_tid, calculator_tid, consumer_tid;
    
    // 创建线程
    pthread_create(&producer_tid, NULL, producer, NULL);
    pthread_create(&calculator_tid, NULL, calculator, NULL);
    pthread_create(&consumer_tid, NULL, consumer, NULL);

    // 等待所有线程结束
    pthread_join(producer_tid, NULL);
    pthread_join(calculator_tid, NULL);
    pthread_join(consumer_tid, NULL);

    // 销毁同步工具
    pthread_mutex_destroy(&buffer1.mutex);
    pthread_mutex_destroy(&buffer2.mutex);
    pthread_cond_destroy(&buffer1.not_full);
    pthread_cond_destroy(&buffer1.not_empty);
    pthread_cond_destroy(&buffer2.not_full);
    pthread_cond_destroy(&buffer2.not_empty);

    return 0;
}