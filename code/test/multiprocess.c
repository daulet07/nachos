#include "syscall.h"
int main()
{
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
	if (ForkExec("userpages1") == -1)
		PutString("No Memory to fork the process\n");
	if (ForkExec("userpages0") == -1)
		PutString("No Memory to fork the process\n");
	if (ForkExec("userpages1") == -1)
		PutString("No Memory to fork the process\n");
	if (ForkExec("userpages0") == -1)
		PutString("No Memory to fork the process\n");
//	if (ForkExec("userpages1") == -1)
		//PutString("No Memory to fork the process\n");
	PutString("End main\n");
	return 0;
}
