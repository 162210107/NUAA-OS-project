#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc < 3) { 
        fprintf(stderr, "Usage: %s <filename> <filename>\n", argv[0]);
        return 1;
    }
    FILE *fp1 = fopen(argv[1], "r");  
    if (fp1 == NULL) {
        perror("Error opening file"); 
        return 2;
    }
    FILE *fp2 = fopen(argv[2], "w");  
    if (fp2 == NULL) {
        perror("Error opening file"); 
        return 3;
    }
    int ch;
    while ((ch = fgetc(fp1)) != EOF) {  
        fputc(ch, fp2); // 正确写入字符到文件 
    }
    fclose(fp1); 
    fclose(fp2);
    return 0;
}