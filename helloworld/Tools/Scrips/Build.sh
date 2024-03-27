rm -r ../../Build/*
riscv64-unknown-elf-g++ -nostdlib -c ../../Kernel/Boot/main.cpp -o ../../Build/main.o  -I"../../Include" -mcmodel=medany
riscv64-unknown-elf-g++ -nostdlib -c ../../Kernel/Boot/Start.S  -o ../../Build/Start.o -I"../../Include" -mcmodel=medany
riscv64-unknown-elf-ld -o ../../Build/OS16.elf -T ../../Linker/Kernel.ld ../../Build/*.o

#编译和链接一个基于RISC-V的操作系统内核
#首先清空了编译输出目录，然后编译了两个源文件（一个C++文件和一个汇编文件），最后将它们链接成一个可执行文件

#rm -r ../../Build/*
#这个命令用于删除Build目录及其下所有文件和子目录
#rm是删除文件或目录的命令，-r表示递归删除，../../Build/*指定了要删除的目标，即当前目录上两级的Build目录下的所有内容

#riscv64-unknown-elf-g++ -nostdlib -c ../../Kernel/Boot/main.cpp -o ../../Build/main.o -I"../../Include" -mcmodel=medany
#这是使用riscv64-unknown-elf-g++编译器来编译main.cpp文件的命令
#-nostdlib表示在编译时不使用标准库，这通常用于系统底层编程
#-c表示只编译不链接。源代码文件是../../Kernel/Boot/main.cpp，输出的目标文件是../../Build/main.o
#-I"../../Include"指定了头文件搜索路径，-mcmodel=medany设置了代码模型为medany，适用于中等大小的代码模型

#riscv64-unknown-elf-g++ -nostdlib -c ../../Kernel/Boot/Start.S -o ../../Build/Start.o -I"../../Include" -mcmodel=medany
#与上一条命令类似，这一条命令编译的是汇编源文件Start.S。输出的目标文件是../../Build/Start.o

#riscv64-unknown-elf-ld -o ../../Build/OS16.elf -T ../../Linker/Kernel.ld ../../Build/*.o
#这是链接命令，使用riscv64-unknown-elf-ld链接器
#-o ../../Build/OS16.elf指定了输出的可执行文件位置和名称
#-T ../../Linker/Kernel.ld指定了链接脚本，控制了链接过程
#../../Build/*.o表示将Build目录下所有.o后缀的目标文件链接成最终的可执行文件OS16.elf