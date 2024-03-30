#ifndef OS16_RISCV_HPP
#define OS16_RISCV_HPP

//SBI Ecall��Ƕ���� 
#define SBI_ECALL(__num,__a0,__a1,__a2)								\
({																	\
 	register unsigned long a0 asm("a0")=(unsigned long)(__a0);		\
	register unsigned long a1 asm("a1") = (unsigned long)(__a1);	\
	register unsigned long a2 asm("a2") = (unsigned long)(__a2);	\
	register unsigned long a7 asm("a7") = (unsigned long)(__num);	\
	asm volatile ("ecall"											\
			:"+r"(a0)												\
			:"r"(a1),"r"(a2),"r"(a7)								\
			:"memory");												\
	a0;																\
})

//����SBI���ù��� 
#define SBI_PUTCHAR(__a0) SBI_ECALL(1,__a0,0,0)
#define SBI_GETCHAR() SBI_ECALL(2,0,0,0)
#define SBI_SET_TIMER(t) SBI_ECALL(0,t,0,0)
#define SBI_SHUTDOWN() SBI_ECALL(8,0,0,0)

//��CSR 
#define CSR_READ(reg)							\
({												\
	unsigned long long re;						\
	asm volatile("csrr %0, " #reg : "=r"(re));	\
	re;											\
})

//дCSR 
#define CSR_WRITE(reg,val)							\
({													\
	asm volatile("csrw " #reg ", %0" :: "rK"(val));	\
})

//��CSR��дCSR 
#define CSR_SWAP(reg,val)											\
({																	\
	unsigned long long re;											\
	asm volatile("csrrw %0, " #reg ", %1" : "=r"(re) : "rK"(val));	\
	re;																\
})

//��CSR����bit��λ�� 
#define CSR_SET(reg,bit)											\
({																	\
	unsigned long long re;											\
	asm volatile("csrrs %0, " #reg ", %1" : "=r"(re) : "rK"(bit));	\
	re;																\
})

//��CSR����!bit��λ�� 
#define CSR_CLEAR(reg,bit)											\
({																	\
	unsigned long long re;											\
	asm volatile("csrrc %0, " #reg ", %1" : "=r"(re) : "rK"(bit));	\
	re;																\
})

#endif
