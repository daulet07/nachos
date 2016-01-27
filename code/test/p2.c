#include  "syscall.h"

int main()
{
	ForkExec("userpages0");
	ForkExec("userpages1");
	PutString("###\n");
	
	return 0;
}
