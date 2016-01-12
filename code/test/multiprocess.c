#include "syscall.h"
int main()
{
	ForkExec("userpages0");
	ForkExec("userpages1");
	PutString("End main\n");
	return 0;
}
