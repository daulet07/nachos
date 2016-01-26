#include "syscall.h"
int main()
{
	if (ForkExec("/us0") == -1)
		PutString("No Memory to fork the process\n");
	if (ForkExec("/us1") == -1)
		PutString("No Memory to fork the process\n");
	if (ForkExec("/us0") == -1)
		PutString("No Memory to fork the process\n");
	if (ForkExec("/us1") == -1)
		PutString("No Memory to fork the process\n");
	if (ForkExec("/us0") == -1)
		PutString("No Memory to fork the process\n");
	if (ForkExec("/us1") == -1)
		PutString("No Memory to fork the process\n");
	if (ForkExec("/us0") == -1)
		PutString("No Memory to fork the process\n");
	if (ForkExec("/us1") == -1)
		PutString("No Memory to fork the process\n");
	if (ForkExec("/us0") == -1)
		PutString("No Memory to fork the process\n");
//	if (ForkExec("userpages1") == -1)
		//PutString("No Memory to fork the process\n");
	PutString("End main\n");
	return 0;
}
