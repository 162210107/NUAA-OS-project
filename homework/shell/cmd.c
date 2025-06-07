#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "cmd.h"

void exec_cmd(struct cmd *cmd)
{
    /* 在此重定向输入 */
    if (cmd->input != NULL) {
        int input_fd = open(cmd->input, O_RDONLY);
        if (input_fd == -1) {
            perror("open input failed");
            exit(EXIT_FAILURE);
        }
        dup2(input_fd, STDIN_FILENO);  // STDIN_FILENO宏定义为0
        close(input_fd);
    }
    /* 在此重定向输出 */
    if (cmd->output != NULL) {
        int output_fd = open(cmd->output, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (output_fd == -1) {
            perror("open output failed");
            exit(EXIT_FAILURE);
        }
        dup2(output_fd, STDOUT_FILENO);
        close(output_fd);
    }
    /* 完成此函数 */
    char *args=cmd->argv[1];
    execvp(cmd->argv[0], cmd->argv);
    perror("exec");
    exit(0);
}

int builtin_cmd(struct cmd *cmd)
{
    /* 实现 cd 命令, 返回 1 */
    if (strcmp(cmd->argv[0], "cd") == 0) {
        char *target = cmd->argv[1];  // 获取参数（如 cd /tmp）
        if (!target) target = getenv("HOME");  // 无参数时切换到家目录
        if (chdir(target) != 0) {
            perror("chdir error");
        } else {
            // 更新 PWD 环境变量
            char cwd[1024];
            if (getcwd(cwd, sizeof(cwd)) != NULL) {
                setenv("PWD", cwd, 1);  // 第三个参数 1 表示覆盖旧值
            } else {
                perror("getcwd error");
            }
        }
        return 1;
    }

    /* 实现 pwd 命令, 返回 1 */
    if (strcmp(cmd->argv[0], "pwd") == 0) {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s\n", cwd);
        } else {
            perror("getcwd error");
        }
        return 1;
    }

    /* 实现 exit 命令, 返回 1 */
    if (strcmp(cmd->argv[0], "exit") == 0) {
        exit(0);
        return 1;
    }

    return 0;
}

void exec_pipe_cmd(int cmdc, struct cmd *cmdv)
{
    //printf("cmdc :%d\n",cmdc);
    if (cmdc == 1){
        exec_cmd(&cmdv[0]);
        return ;
    }

    /* 处理管道中包含 2 个命令的情况 */
    int pipefds[cmdc][2];
    pid_t pid;

    // 为每个管道创建描述符
    for (int i = 0; i < cmdc - 1; i++) {
        if (pipe(pipefds[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < cmdc; i++) {
        pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {  // 子进程处理当前命令
            // 输入重定向：非首命令则从前一个管道读
            if (i > 0) {
                dup2(pipefds[i-1][0], STDIN_FILENO);
                close(pipefds[i-1][0]);
                close(pipefds[i-1][1]);
            }

            // 输出重定向：非末命令则写入下一个管道
            if (i < cmdc - 1) {
                dup2(pipefds[i][1], STDOUT_FILENO);
                close(pipefds[i][0]);
                close(pipefds[i][1]);
            }

            // 关闭所有无关管道描述符
            for (int j = 0; j < cmdc - 1; j++) {
                if (j != i && j != i-1) {
                    close(pipefds[j][0]);
                    close(pipefds[j][1]);
                }
            }

            exec_cmd(&cmdv[i]);
            exit(0);
        } else {  // 父进程关闭已用描述符并等待
            if (i > 0) {
                close(pipefds[i-1][0]);
                close(pipefds[i-1][1]);
            }
            wait(NULL);
        }
    }
    exit(0);
    return;
}
