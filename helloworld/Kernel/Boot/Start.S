	.section .text.entry
	//声明了一个新的节,.text.entry节，这部分会被链接器放到text段第一个顺序处，保证启动优先顺序
	
kernel_entry://当前地址,在其他地方（比如C/C++代码中）可以通过这个标签引用这个地址
	.global kernel_entry//在整个程序中都是可见的，允许其他文件中的代码引用这个地址
	la sp, boot_stack_top//设置当前栈顶为boot_stack_top 
	call main//跳转到C++实现的main函数 
	
SBI_Putchar_2://SBI_Putchar_2函数，由于外部调用时传入的第一个参数放到了a0(参见SBI_Putchar_1) 
	.global SBI_Putchar_2//声明全局可见 
	li a7,1//将1加载到a7，a7用于指定SBI调用的编号
	ecall//环境调用，根据a7寄存器的值触发相应的SBI服务
	ret//从当前函数返回 

	.align 12//将代码段对齐到4KB，确保栈的起始地址对齐
	
	.section .bss.stack//.bss段通常用于存放程序启动时需要被初始化为零的变量
	
boot_stack://启动栈的最低地址 
	.global boot_stack
	
	.zero 4096//填充4KB空间为0，此处大小随意，不一定要4KB 
	
boot_stack_top://启动栈的高地址，栈从这里开始，向低地址生长 
	.global boot_stack_top
