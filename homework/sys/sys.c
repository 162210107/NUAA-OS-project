#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_ARGS 20  // 支持最多20个参数

void sys(char *command)
{
    char *words[MAX_ARGS];  // 参数数组
    char *token;            // 分割指针
    int i = 0;              // 参数计数器
    
    // 创建可修改的副本（strtok会修改原字符串）
    char *cmd_copy = strdup(command);
    
    // 使用空格分割命令字符串
    token = strtok(cmd_copy, " ");
    while (token != NULL && i < MAX_ARGS-1) {
        words[i++] = token;
        token = strtok(NULL, " ");
    }
    words[i] = NULL;  // 必须以NULL结尾

    // 创建子进程
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {  // 子进程
        execvp(words[0], words);
        perror("execvp failed");
        exit(EXIT_FAILURE);
    } else { 
        wait(NULL);  
    }
    free(cmd_copy);  
}

int main()
{
    /* 不要修改 main 函数 */
    puts("---");
    sys("echo HELLO WORLD");
    puts("---");
    sys("ls /");
    puts("---");
    return 0;
}