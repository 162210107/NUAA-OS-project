#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
    int thread_id;     // 线程编号
    int total_threads; // 总线程数
    long long terms;   // 总计算项数
    double sum;        // 线程局部计算结果
} ThreadArgs;

void* calculate_part(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    long long start = args->thread_id * (args->terms / args->total_threads);
    long long end = (args->thread_id + 1) * (args->terms / args->total_threads);
    
    double local_sum = 0.0;
    for (long long i = start; i < end; i++) {
        int sign = (i % 2 == 0) ? 1 : -1;  // 符号交替逻辑优化
        double denominator = 2.0 * i + 1;  // 分母项公式优化
        local_sum += sign / denominator;
    }
    
    args->sum = local_sum;
    return NULL;
}

int main(int argc, char* argv[]) {
    const int N = 4;          // 线程数(需根据CPU核心数调整)
    const long long M = 10000000L; // 总项数(保证精度)
    
    pthread_t threads[N];
    ThreadArgs thread_args[N];
    
    // 创建N个线程并分配计算任务
    for (int i = 0; i < N; i++) {
        thread_args[i] = (ThreadArgs){
            .thread_id = i,
            .total_threads = N,
            .terms = M,
            .sum = 0.0
        };
        pthread_create(&threads[i], NULL, calculate_part, &thread_args[i]);
    }
    
    // 等待所有线程完成并累加结果
    double total_sum = 0.0;
    for (int i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
        total_sum += thread_args[i].sum;
    }
    
    // 输出结果
    double PI = 4.0 * total_sum;
    printf("%.2f\n", PI); // 确保小数点后两位精度
    return 0;
}