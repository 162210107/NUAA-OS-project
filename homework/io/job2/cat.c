#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {  // 检查是否传入了文件名参数
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    FILE *fp = fopen(argv[1], "r");  // 以只读模式打开文件
    if (fp == NULL) {
        perror("Error opening file");  // 输出具体错误信息
        return 2;
    }

    int ch;
    while ((ch = fgetc(fp)) != EOF) {  // 逐字符读取文件内容
        putchar(ch);  // 将字符输出到标准输出
    }
    fclose(fp);  // 关闭文件
    return 0;
}