#include <Memory/PhysicalMemory.hpp>
#include <Library/KoutSingle.hpp>

//PhysicalMemoryManager *OS16_PMM=nullptr;
int MemInfo=31,
	MemTest=31;

/*
	This code comes from PAL_OperatingSystem and modified to match.
	By:qianpinyi && PeaceSheep
*/

//constexpr Uint32 PageSizeBit=12,
//				 PageSize=1<<PageSizeBit;
//				 
//constexpr Uint64 PageSizeN[3]{PageSize,PageSize*512,PageSize*512*512};
//				 
//constexpr Uint64 PhysicalVirtualMemoryOffset=0xFFFFFFFF40000000ull,
//				 PhysicalKernelStartAddr=0x80020000ull;
//
//inline Uint64 PhysicalMemorySize()
//{return 0x600000;}
//
//inline Uint64 PhysicalMemoryPhysicalStart()
//{return 0x80000000;}
//
//inline Uint64 PhymemVirmemOffset()
//{return PhysicalVirtualMemoryOffset;}
//
//inline Uint64 PhysicalMemoryVirtualEnd()
//{return PhymemVirmemOffset()+PhysicalMemoryPhysicalStart()+PhysicalMemorySize();}
//
//extern "C"
//{
//	extern char kernelstart[];
//	extern char textstart[];
//	extern char textend[];
//	extern char rodatastart[];
//	extern char rodataend[];
//	extern char datastart[];
//	extern char dataend[];
//	extern char bssstart[];
//	extern char bssend[];
//	extern char kernelend[];
//	extern char freememstart[];
//	extern char bootstack[];
//	extern char bootstacktop[];
//	
//	inline Uint64 FreeMemBase()
//	{return (Uint64)freememstart;}
//};

#define FreeMemBase FreeMemoryStart
#define PhysicalMemoryPhysicalStart PhysicalMemoryStart
#define PhymemVirmemOffset() 0

struct Page
{
	Uint64 flags,
		   num,//The pages managed by this struct after this
		   ref;//How many page entry referred to it.
	Page *pre,
		 *nxt;
	
	inline Uint64 Index() const
	{return ((Uint64)this-FreeMemBase())/sizeof(Page);}
	
	inline void* KAddr() const
	{return (void*)(FreeMemBase()+Index()*PageSize);}
//	
//	inline void* PAddr() const
//	{return (void*)(FreeMemBase()+Index()*PageSize-PhymemVirmemOffset());}
};



class POS_PhysicalMemoryManager_LinkTable:public PhysicalMemoryManager
{
	protected:
		Page head;
		Page *PagesEndAddr;
		Uint64 PageCount;
		
		void MergePage(Page *p);
		
	public:
		const char* Name() const;
		Page* _GetPageFromAddr(void *addr);
		Page* AllocPage(Uint64 count);
		void Free(Page *p);
		ErrorType _Init();
		//Won't destroy so Destroy is not include.
		
		inline void FreePage(Page *p)
		{Free(p);}
		
		
		virtual Page* GetPageFromAddr(void *addr)
		{return _GetPageFromAddr(addr);}
		virtual void* AllocPages(Uint64 count)
		{return AllocPage(count);}
		virtual void FreePages(Page *p)
		{Free(p);}
		virtual void Init()
		{_Init();}
};

#define PhysicalMemoryManager1 POS_PhysicalMemoryManager_LinkTable

extern PhysicalMemoryManager1 POS_PMM;

//void* Kmalloc(Uint64 size);
//void  Kfree(void *);
//
//template <typename T> inline T* KmallocT()
//{return (T*)Kmalloc(sizeof(T));}

//#endif
//#include <Memory/PhysicalMemory.hpp>
//#include <Library/Kout.hpp>
//#include <Trap/Interrupt.hpp>
//
//using namespace POS;

Page* PhysicalMemoryManager1::AllocPage(unsigned long long count)
{
//	ISASBC;//??
    Page *p=head.nxt;
    while(p)
    {
		MergePage(p);
        if(count<p->num)
        {
            Page * np = p+count;
            int num = p->num;
            p->num=count;
            p->pre->nxt = np;
            if (p->nxt)
            	p->nxt->pre=np;
            np->pre=p->pre;
			np->nxt=p->nxt;            
            np->num = num - count;
            p->flags = 1;
            PageCount-=count;
            return p;
        }
        else if(p->num==count)
        {
            p->pre->nxt = p->nxt;
            if(p->nxt)
            {
                p->nxt->pre = p->pre;
            }
            p->flags = 1;
            PageCount-=count;
            return p;
        }
        p = p->nxt;
    }
    kout[Fault]<<"PhysicalMemoryManager::AllocPage "<<count<<" failed! Maybe run out of Memory! Left PageCount "<<PageCount<<endl;
    return nullptr;
}

void  PhysicalMemoryManager1::MergePage(Page * p)
{
    if(p+p->num<PagesEndAddr&&(p+p->num)->flags==0)
    {
        Page * nxt = p+p->num;
        p->num+=nxt->num;
        p->nxt = nxt->nxt;
        if(nxt->nxt)
        {
            nxt->nxt->pre = p;
        }
    }
}

void PhysicalMemoryManager1::Free(Page *p)
{
	if (p==nullptr) return;
//	ISASBC;
	PageCount+=p->num;
    p->flags = 0;
    Page * pre_p = head.nxt;
    if(pre_p==nullptr||pre_p > p)
    {
        Page * nxt_p = pre_p;
        head.nxt = p;
        p->pre = &head;
        p->nxt = nxt_p;
        if(nxt_p)
        {
            nxt_p->pre = p;
        }
        MergePage(p);
        return;
    }

    while(pre_p->nxt&&pre_p->nxt < p)
    {
        pre_p = pre_p->nxt;
    }

    Page* nxt_p = pre_p->nxt;
    pre_p ->nxt = p;
    p->pre = pre_p;

    p->nxt = nxt_p;
    if(nxt_p)
    {
        nxt_p->pre = p;
    }
    MergePage(p);
}

ErrorType PhysicalMemoryManager1::_Init()
{
	using namespace POS;
	kout[MemInfo]<<"POS_PMM init..."<<endl;
    Page * page = (Page*)FreeMemBase();
    PageCount= (PhysicalMemorySize()+PhysicalMemoryPhysicalStart() + PhymemVirmemOffset() -  FreeMemBase()) / PageSize;
//	MemsetT<char>((char*)FreeMemBase(),0,PhysicalMemorySize()+PhysicalMemoryPhysicalStart() + PhymemVirmemOffset() -  FreeMemBase());
		
    kout[MemInfo]<<"Free physical page count: "<<PageCount<<endl;
    for(int i=0;i<PageCount;i++)
    {
        page[i].flags = 0;
    }
    Uint64 page_need_size = PageCount * sizeof(Page);
    Uint64 page_need_page_num = page_need_size / PageSize + 1;
    kout[MemInfo]<<"Page manager used page count: "<<page_need_page_num<<endl;
    page[0].num = page_need_page_num;
    page[0].flags = 1;
    
    page[page_need_page_num].num = PageCount - page_need_page_num;
    page[page_need_page_num].nxt = nullptr;
    head.nxt = page + page_need_page_num;
    page[page_need_page_num].pre = &head;
    PagesEndAddr = page + PageCount;
    PageCount-=page_need_page_num;
    kout[MemInfo]<<"POS_PMM init OK"<<endl;
//    return ERR_None;
	return 0;
}

//PhysicalMemoryManager::~PhysicalMemoryManager() {}
//
//PhysicalMemoryManager::PhysicalMemoryManager() {}

const char* PhysicalMemoryManager1::Name()  const
{return "POS_DefaultPMM";}

Page *PhysicalMemoryManager1::_GetPageFromAddr(void * addr)
{return (Page*)(void*)(FreeMemBase() + ((Uint64)addr -FreeMemBase()) / PageSize * sizeof(Page));}

PhysicalMemoryManager1 POS_PMM;

#undef PhysicalMemoryManager
#undef FreeMemBase

void PMM_Init()
{
	MemInfo=kout.RegisterType("MemInfo",Cyan);
	MemTest=kout.RegisterType("MemTest",LightCyan);
	POS_PMM._Init();
	//OS16_PMM=&POS_PMM;
	kout.SwitchTypeOnoff(MemTest,false); 
}

void * Kmalloc(Uint64 size)
{
	kout[MemTest]<<"Kmalloc "<<(void*)size<<endl;
    Uint64 page_num = size / PageSize;
    if(size % PageSize)
    {
        page_num++;
    }
    Page * p = POS_PMM.AllocPage(page_num);
    kout[MemTest]<<"Kmalloc "<<(void*)size<<" success with page "<<p<<" and address "<<p->KAddr()<<endl;
	return p->KAddr();
}

void Kfree(void *p)
{
	kout[MemTest]<<"Kfree "<<p<<endl;
	POS_PMM.Free(POS_PMM.GetPageFromAddr(p));
	kout[MemTest]<<"Kfree "<<p<<" Success"<<endl;
}