#include "syscall.h"
int main()
{
	if (ForkExec("step4") == -1)
		PutString("No Memory to fork the process\n");
	if (ForkExec("p1") == -1)
		PutString("No Memory to fork the process\n");
	/*if (ForkExec("userpages0") == -1)
		PutString("No Memory to fork the process\n");
	if (ForkExec("userpages1") == -1)
		PutString("No Memory to fork the process\n");
	if (ForkExec("userpages0") == -1)
		PutString("No Memory to fork the process\n");
	if (ForkExec("userpages1") == -1)
		PutString("No Memory to fork the process\n");
	if (ForkExec("userpages0") == -1)
		PutString("No Memory to fork the process\n");
	if (ForkExec("userpages1") == -1)
		PutString("No Memory to fork the process\n");
	if (ForkExec("userpages0") == -1)
		PutString("No Memory to fork the process\n");
	/ *if (ForkExec("putchar") == -1)
		PutString("No Memory to fork the process\n");*/
	PutString("End main\n");
	return 0;
}
