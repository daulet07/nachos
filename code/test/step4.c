#include  "syscall.h"

int main()
{
	ForkExec("p1");
	ForkExec("p2");
	PutString("\n");
	
	return 0;
}
