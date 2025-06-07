#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// 线程参数结构体
typedef struct {
    int *arr;
    int start;
    int end;
} SortArgs;

// 选择排序线程函数
void* select_sort_thread(void *arg) {
    SortArgs *args = (SortArgs*)arg;
    for (int i = args->start; i <= args->end; i++) {
        int min_idx = i;
        for (int j = i+1; j <= args->end; j++) {
            if (args->arr[j] < args->arr[min_idx]) {
                min_idx = j;
            }
        }
        int temp = args->arr[i];
        args->arr[i] = args->arr[min_idx];
        args->arr[min_idx] = temp;
    }
    return NULL;
}

// 归并函数
void merge(int arr[], int l, int m, int r) {
    int n1 = m - l + 1;
    int n2 = r - m;
    int L[n1], R[n2];

    for (int i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (int j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];

    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) {
        arr[k++] = (L[i] <= R[j]) ? L[i++] : R[j++];
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
}

// 主排序函数
void parallel_sort(int *array, int size) {
    pthread_t threads[2];
    SortArgs args[2];

    // 划分任务区间
    args[0] = (SortArgs){array, 0, size/2 - 1};
    args[1] = (SortArgs){array, size/2, size - 1};

    // 创建双线程
    pthread_create(&threads[0], NULL, select_sort_thread, &args[0]);
    pthread_create(&threads[1], NULL, select_sort_thread, &args[1]);

    // 等待线程完成
    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);

    // 归并已排序的两部分
    merge(array, 0, size/2 - 1, size - 1);
}