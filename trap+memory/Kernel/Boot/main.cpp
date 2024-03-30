#include <Library/KoutSingle.hpp>
#include <Trap/Trap.hpp>
#include <Trap/Clock.hpp>
#include <Trap/Interrupt.hpp>
#include <Memory/PhysicalMemory.hpp>

//下面连续的几行是用于使用KoutSingle.hpp 
extern "C"
{
	void Putchar(char ch)
	{
		SBI_PUTCHAR(ch);
	}
};
namespace POS
{
	KOUT kout;
};



int main() 
{
	TrapInit();
	ClockInit();
	InterruptEnable();
	kout[Info]<<"System start success!"<<endl;
	PMM_Init();
	
	//Below do nothing...
	auto Sleep=[](int n){while (n-->0);};
	auto DeadLoop=[Sleep](const char *str)
	{
		while (1)
			Sleep(1e8),
			kout<<str;
	};
	DeadLoop(".");
	
	kout[Info]<<"Shutdown!"<<endl;
	SBI_SHUTDOWN();

	return 0;
}
