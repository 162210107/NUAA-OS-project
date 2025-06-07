#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>

typedef struct{
    char *path;
    char *target;
} Task;

typedef struct{
    Task *tasks;
    int capacity;
    int size;
    int front;
    int rear;
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
} TaskQueue;

void task_queue_init(TaskQueue *q, int capacity)
{
    q->capacity = capacity;
    q->tasks = malloc(sizeof(Task) * capacity);
    q->size = 0;
    q->front = 0;
    q->rear = 0;
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->not_empty, NULL);
    pthread_cond_init(&q->not_full, NULL);
}

void task_queue_enqueue(TaskQueue *q, Task task)
{
    pthread_mutex_lock(&q->mutex);
    while (q->size == q->capacity)
    {
        pthread_cond_wait(&q->not_full, &q->mutex);
    }
    q->tasks[q->rear] = task;
    q->rear = (q->rear + 1) % q->capacity;
    q->size++;
    pthread_cond_signal(&q->not_empty);
    pthread_mutex_unlock(&q->mutex);
}

Task task_queue_dequeue(TaskQueue *q)
{
    pthread_mutex_lock(&q->mutex);
    while (q->size == 0)
    {
        pthread_cond_wait(&q->not_empty, &q->mutex);
    }
    Task task = q->tasks[q->front];
    q->front = (q->front + 1) % q->capacity;
    q->size--;
    pthread_cond_signal(&q->not_full);
    pthread_mutex_unlock(&q->mutex);
    return task;
}

pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;

void *worker_thread(void *arg)
{
    TaskQueue *queue = (TaskQueue *)arg;
    while (1){
        Task task = task_queue_dequeue(queue);
        if (task.path == NULL && task.target == NULL)
            break;
        FILE *file = fopen(task.path, "r");
        if (!file){
            free(task.path);
            continue;
        }
        char line[256];
        while (fgets(line, sizeof(line), file)){
            if (strstr(line, task.target)){
                pthread_mutex_lock(&print_mutex);
                printf("%s:%s", task.path, line);
                pthread_mutex_unlock(&print_mutex);
            }
        }
        fclose(file);
        free(task.path);
    }
    pthread_exit(NULL);
}

void enqueue_tasks_from_dir(TaskQueue *queue, char *path, char *target, int recursive)
{
    DIR *dir = opendir(path);
    if (!dir){
        perror("opendir");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL){
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        char sub_path[1024];
        snprintf(sub_path, sizeof(sub_path), "%s/%s", path, entry->d_name);
        struct stat st;
        if (lstat(sub_path, &st) == -1){
            perror("lstat");
            continue;
        }

        if (S_ISDIR(st.st_mode))
            if (recursive)
                enqueue_tasks_from_dir(queue, sub_path, target, recursive);
        else if (S_ISREG(st.st_mode)){
            Task task;
            task.path = strdup(sub_path);
            task.target = target;
            task_queue_enqueue(queue, task);
        }
    }
    closedir(dir);
}

void usage()
{
    puts("Usage:");
    puts(" pgrep string path");
    puts(" pgrep -r string path");
}

int main(int argc, char *argv[])
{
    if (argc < 3){
        usage();
        return 0;
    }

    int recursive = 0;
    char *string;
    char *path;

    if (strcmp(argv[1], "-r") == 0){
        if (argc < 4){
            usage();
            return 1;
        }
        recursive = 1;
        string = argv[2];
        path = argv[3];
    }
    else{
        string = argv[1];
        path = argv[2];
    }

    TaskQueue queue;
    task_queue_init(&queue, 100);

    pthread_t workers[2];
    for (int i = 0; i < 2; i++)
        pthread_create(&workers[i], NULL, worker_thread, &queue);

    struct stat st;
    if (stat(path, &st) == -1){
        perror("stat");
        exit(1);
    }

    if (S_ISDIR(st.st_mode))
        enqueue_tasks_from_dir(&queue, path, string, recursive);
    else if (S_ISREG(st.st_mode)){
        Task task;
        task.path = strdup(path);
        task.target = string;
        task_queue_enqueue(&queue, task);
    }
    else{
        fprintf(stderr, "Unsupported file type\n");
        exit(1);
    }

    for (int i = 0; i < 2; i++){
        Task term_task = {NULL, NULL};
        task_queue_enqueue(&queue, term_task);
    }

    for (int i = 0; i < 2; i++)
        pthread_join(workers[i], NULL);

    free(queue.tasks);
    pthread_mutex_destroy(&queue.mutex);
    pthread_cond_destroy(&queue.not_empty);
    pthread_cond_destroy(&queue.not_full);

    return 0;
}
