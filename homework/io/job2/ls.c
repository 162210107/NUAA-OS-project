#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

void print_permissions(__mode_t mode) {
    printf("%c%c%c%c%c%c%c%c%c ",
        (mode & S_IRUSR) ? 'r' : '-',
        (mode & S_IWUSR) ? 'w' : '-',
        (mode & S_IXUSR) ? 'x' : '-',
        (mode & S_IRGRP) ? 'r' : '-',
        (mode & S_IWGRP) ? 'w' : '-',
        (mode & S_IXGRP) ? 'x' : '-',
        (mode & S_IROTH) ? 'r' : '-',
        (mode & S_IWOTH) ? 'w' : '-',
        (mode & S_IXOTH) ? 'x' : '-');
}

void list_files(int show_details) {
    DIR *dir = opendir(".");
    if (!dir) {
        perror("opendir");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') continue;  // 跳过隐藏文件

        if (show_details) {
            struct stat st;
            if (stat(entry->d_name, &st) == -1) {
                perror("stat");
                continue;
            }
            print_permissions(st.st_mode);
        }

        printf("%s\n", entry->d_name);
    }
    closedir(dir);
}

int main(int argc, char *argv[]) {
    int show_details = 0;
    if (argc > 1 && strcmp(argv[1], "-l") == 0) {
        show_details = 1;
    }

    list_files(show_details);
    return 0;
}