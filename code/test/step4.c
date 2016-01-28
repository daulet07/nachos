#include  "syscall.h"

int main()
{
	ForkExec("/test/p1");
	ForkExec("/test/p2");
	PutString("\n");
	
	return 0;
}
