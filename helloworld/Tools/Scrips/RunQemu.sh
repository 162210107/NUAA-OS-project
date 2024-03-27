qemu-system-riscv64 -machine virt -m 128M -nographic -bios ../../SBI/opensbi-qemu.elf -kernel ../../Build/OS16.elf
#在QEMU模拟器上启动一个基于RISC-V 64位架构的虚拟机
#qemu-system-riscv64: 这是命令的主体，代表启动一个面向RISC-V 64位架构的QEMU虚拟机
#-machine virt: 指定虚拟机使用的是“virt”虚拟硬件平台，这是一种为QEMU设计的通用虚拟平台
#-m 128M: 为虚拟机分配128MB的内存
#-nographic: 以无图形界面模式运行，即所有的输出和输入都通过控制台进行
#-bios ../../SBI/opensbi-qemu.elf: 指定BIOS文件的位置，这里用的是OpenSBI的一个ELF（Executable and Linkable Format）文件，作为启动前的固件界面
#-kernel ../../Build/OS16.elf: 指定内核文件的位置，这里加载的是一个名为OS16.elf的内核
#../../SBI/opensbi-qemu.elf 表示从当前目录开始，先上移两级目录（../..），然后进入 SBI 目录，最后访问 opensbi-qemu.elf 文件