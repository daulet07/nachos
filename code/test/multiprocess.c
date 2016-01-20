#include "syscall.h"
int main()
{
	if (ForkExec("userpages0") == -1)
		PutString("Not Forked\n");
	if (ForkExec("userpages1") == -1)
		PutString("Not Forked\n");
	if (ForkExec("userpages0") == -1)
		PutString("Not Forked\n");
	if (ForkExec("userpages1") == -1)
		PutString("Not Forked\n");
	if (ForkExec("userpages0") == -1)
		PutString("Not Forked\n");
	if (ForkExec("userpages1") == -1)
		PutString("Not Forked\n");
	if (ForkExec("userpages0") == -1)
		PutString("Not Forked\n");
	if (ForkExec("userpages1") == -1)
		PutString("Not Forked\n");
	PutString("End main\n");
	return 0;
}
