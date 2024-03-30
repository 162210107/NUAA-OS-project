	.section .text.entry
kernel_entry:
	.global kernel_entry
	la sp, boot_stack_top
	call main
	
	.align 12
	.section .bss.stack
boot_stack:
	.global boot_stack
	
	.zero 4096
	
boot_stack_top:
	.global boot_stack_top
