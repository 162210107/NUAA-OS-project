#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "parse.h"

void parse_cmd(char *line, struct cmd *cmd)
{
    cmd->argc = 0;
    cmd->input = NULL;
    cmd->output = NULL;

    /* 使用 strtok 完成此函数 */
    const char delim[] = " ";

    char *tok;
    tok = strtok(line, delim);
    while (tok != NULL)
    {
        if (tok[0] == '<')
        {
            // 处理输入重定向
            if (strlen(tok) == 1) {  // 单独符号 '<'
                tok = strtok(NULL, delim);  // 取下一个token作为文件名
                if (tok != NULL) {
                    cmd->input = tok;
                } else {
                    // 错误处理：文件名缺失
                    printf(" < file name is lost!\n");
                }
            } else {  // 符号与文件名相连（如 <log.txt）
                cmd->input = tok + 1;  // 跳过 '<' 符号
            }
        }
        else if (tok[0] == '>')
        {
            // 处理输入重定向
            if (strlen(tok) == 1) {  // 单独符号 '>'
                tok = strtok(NULL, delim);  // 取下一个token作为文件名
                if (tok != NULL) {
                    cmd->output = tok;
                } else {
                    // 错误处理：文件名缺失
                    printf(" > file name is lost!\n");
                }
            } else {  // 符号与文件名相连（如 >log.txt）
                cmd->output = tok + 1;  // 跳过 '>' 符号
            }
        }
        else
        {
            cmd->argv[cmd->argc++] = tok;
        }
        tok = strtok(NULL, delim);
    }

    /* 完成此函数，要处理 > < 的情况 */

    cmd->argv[cmd->argc] = NULL;
}

void dump_cmd(struct cmd *cmd)
{
    printf("argc = %d\n", cmd->argc);
    int i;
    for (i = 0; i < cmd->argc; i++)
    {
        printf("argv[%d] = (%s)\n", i, cmd->argv[i]);
    }

    char *input = cmd->input ? cmd->input : "NULL";
    printf("input = (%s)\n", input);

    char *output = cmd->output ? cmd->output : "NULL";
    printf("output = (%s)\n", output);
}

int parse_pipe_cmd(char *line, struct cmd *cmdv)
{
    /* 使用 strtok_r 完成此函数 */
    char *saveptr_pipe, *saveptr_cmd;
    char *token_pipe;
    int cmd_count = 0;

    // 第一层分割：按管道符分割命令段
    token_pipe = strtok_r(line, "|", &saveptr_pipe);
    while (token_pipe != NULL && cmd_count < MAX_CMDC) {
        struct cmd *current_cmd = &cmdv[cmd_count];

        int argc = 0;
        // 第二层分割：按空格分割命令参数
        parse_cmd(token_pipe,current_cmd);

        cmd_count++;
        token_pipe = strtok_r(NULL, "|", &saveptr_pipe);
    }

    return cmd_count;
}

void dump_pipe_cmd(int cmdc, struct cmd *cmdv)
{
    int i;

    printf("pipe cmd, cmdc = %d\n", cmdc);
    for (i = 0; i < cmdc; i++)
    {
        struct cmd *cmd = cmdv + i;
        dump_cmd(cmd);
    }
}
