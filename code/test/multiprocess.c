#include "syscall.h"
int main()
{
	PutString("1\n");
	ForkExec("userpages0");
	PutString("2\n");
	ForkExec("userpages1");
	PutString("End main\n");
	return 0;
}
