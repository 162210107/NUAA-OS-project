#include <stdio.h>
#include <pthread.h>

#define N 1000000  // 总计算项数，保证精度

double master_sum = 0.0;
double worker_sum = 0.0;

void *worker(void *arg) {
    for (int i = N/2 + 1; i <= N; i++) {
        int denominator = 2 * i - 1;  // 分母项
        double term = 1.0 / denominator;
        if (i % 2 == 0) worker_sum -= term;  // 偶数项为负
        else worker_sum += term;             // 奇数项为正
    }
    return NULL;
}

void master() {
    for (int i = 1; i <= N/2; i++) {
        int denominator = 2 * i - 1;
        double term = 1.0 / denominator;
        if (i % 2 == 0) master_sum -= term;  // 符号交替逻辑
        else master_sum += term;
    }
}

int main() {
    pthread_t tid;
    pthread_create(&tid, NULL, worker, NULL);  // 创建辅助线程
    master();                                  // 主线程计算前半部分
    pthread_join(tid, NULL);                   // 等待辅助线程结束
    double PI = 4 * (master_sum + worker_sum); // 合并结果并乘以
    printf("%.2f\n", PI);                      // 精确到小数点后两位
    return 0;
}