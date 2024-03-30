#ifndef OS16_PHYSICALMEMORY_HPP
#define OS16_PHYSICALMEMORY_HPP

#include "../Types.hpp"

constexpr Uint32 PageSizeBit=12,
				 PageSize=1<<PageSizeBit;

extern "C"
{
	extern char kernelend[];//链接脚本里PROVIDE的符号，可获取其地址 
	extern char freememstart[];
};

inline Uint64 PhysicalMemoryStart()
{return 0x80000000;}//硬编码qemu物理内存起始地址 

inline Uint64 PhysicalMemorySize()
{return 0x08000000;}//128MB的内存大小 

inline Uint64 FreeMemoryStart()//可用于分配的自由内存起始地址 
{return (Uint64)freememstart;}

inline Uint64 FreeMemorySize()//自由内存大小 
{return PhysicalMemoryStart()+PhysicalMemorySize()-FreeMemoryStart();}

struct Page;
class PhysicalMemoryManager//页级物理内存管理器 
{
	public://目前仅提供最基础的功能，后续会更改和增加 
		virtual Page* GetPageFromAddr(void *addr)=0;//通过地址获得物理页结构体 
	    virtual void* AllocPages(Uint64 count)=0;//分配连续的count个物理页 
	    virtual void FreePages(Page *p)=0;//回收分配的p所在的一系列物理页 
	    virtual void Init()=0;//初始化物理内存管理器
};
//extern PhysicalMemoryManager*OS16_PMM;

void PMM_Init();
void* Kmalloc(Uint64 size);
void  Kfree(void *addr);

template <typename T> inline T* KmallocT()
{return (T*)Kmalloc(sizeof(T));}

#endif
