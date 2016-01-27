#include "syscall.h"
int main()
{
	ProcessId id[4];
	if ((id[0] = ForkExec("/IO/putchar")) == -1)
		PutString("No Memory to fork the process\n");
	if ((id[1] = ForkExec("/IO/getstring")) == -1)
		PutString("No Memory to fork the process\n");
	if ((id[2] = ForkExec("/IO/putstring")) == -1)
		PutString("No Memory to fork the process\n");

	int i;
	for (i = 0; i < 3; i ++)
	{
		PutString("Wait ");
		PutInt(id[i]);
		PutChar('\n');
		Wait(id[i]);
		PutString("Finish\n");
	}
	PutString("End main\n");
	return 0;
}
