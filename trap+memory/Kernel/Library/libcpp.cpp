#include <Memory/PhysicalMemory.hpp>

extern "C"{
    void* __cxa_pure_virtual=0;
}
using size_t=long unsigned int;

void* operator new(size_t size)
{return Kmalloc(size);}

void* operator new[](size_t size)
{return Kmalloc(size);}

void operator delete(void *p,size_t size)
{Kfree(p);}

void operator delete[](void *p)
{Kfree(p);}
