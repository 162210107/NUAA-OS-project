# 自己实现
# 要求可执行程序名是 sh
# 定义编译器和编译选项
CC = gcc
CFLAGS = -Wall -Wextra -g  # 启用警告和调试信息
TARGET = sh                 # 目标可执行文件名

# 源文件与目标文件映射
SRCS = cmd.c parse.c main.c
OBJS = $(SRCS:.c=.o)       # 自动生成目标文件列表：cmd.o parse.o main.o

# 默认目标：生成可执行文件
all: $(TARGET)

# 链接目标文件生成可执行文件
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

# 模式规则：编译 .c 文件生成 .o 文件
# 依赖对应的 .h 文件，确保头文件修改时重新编译
%.o: %.c cmd.h parse.h
	$(CC) $(CFLAGS) -c $< -o $@

# 清理生成的文件
.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJS)