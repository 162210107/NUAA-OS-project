#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>

#define MAX_PATH_LEN 4096

void find_str(int showstr,char* target,char* path){
    struct stat st;
    if (stat(path, &st) == -1) {
        perror("stat error");
        return;
    }

    // 如果是普通文件
    if (S_ISREG(st.st_mode)) {
        FILE *fp = fopen(path, "r");
        if (!fp) {
            perror("fopen error");
            return;
        }

        char line[1024];
        while (fgets(line, sizeof(line), fp)) {
            if (strstr(line, target)) {
                if (showstr) {
                    printf("%s\n", path);  // -l模式只输出路径
                    break;
                } else {
                    printf("%s:%s", path, line);  // 非-l模式输出完整信息
                }
            }
        }
        fclose(fp);
        return;
    }

    // 如果是目录
    if (S_ISDIR(st.st_mode)) {
        DIR *dir = opendir(path);
        if (!dir) {
            perror("opendir error");
            return;
        }

        struct dirent *entry;
        while ((entry = readdir(dir))) {
            // 跳过.和..目录
            if (strcmp(entry->d_name, ".") == 0 || 
                strcmp(entry->d_name, "..") == 0) continue;

            // 构建子路径
            char subpath[MAX_PATH_LEN];
            snprintf(subpath, MAX_PATH_LEN, "%s/%s", path, entry->d_name);
            
            // 递归处理子项
            find_str(showstr, target, subpath);
        }
        closedir(dir);
    }
}

int main(int argc, char *argv[]) {
    if (argc >4||argc<3 ) { 
        fprintf(stderr, "Usage: %s {-l} <string> <dir>\n", argv[0]);
        return 1;
    }
    int showdir=0;
    char* str;
    char* path;
    if(strcmp(argv[1], "-r") == 0){
        showdir=1;
        str=argv[2];
        path=argv[3];
    }else{
        str=argv[1];
        path=argv[2];
    }
    find_str(showdir,str,path);
    return 0;
}