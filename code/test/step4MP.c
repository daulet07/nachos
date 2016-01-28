#include "syscall.h"
int main()
{
	PutString("Testing Multi Processing and multi threading.............\n\n");	
	ProcessId id[10];
	if ((id[0] = ForkExec("/test/p1")) == -1){
		PutString("\nNo Memory to fork the process");
		PutInt(id[0]);
	}
	if ((id[1] = ForkExec("/test/p1")) == -1){
		PutString("No Memory to fork the process\n");
		PutInt(id[1]);
	}
	if ((id[2] = ForkExec("/test/p1")) == -1){
		PutString("\nNo Memory to fork the process");
		PutInt(id[2]);
	}
	if ((id[3] = ForkExec("/test/p1")) == -1){
		PutString("\nNo Memory to fork the process");
		PutInt(id[3]);
	}
	if ((id[4] = ForkExec("/test/p1")) == -1){
		PutString("\nNo Memory to fork the process");
		PutInt(id[4]);
	}
	if ((id[5] = ForkExec("/test/p1")) == -1){
		PutString("\nNo Memory to fork the process");
		PutInt(id[5]);
	}
	if ((id[6] = ForkExec("/test/p1")) == -1){
		PutString("\nNo Memory to fork the process");
		PutInt(id[6]);
	}
	if ((id[7] = ForkExec("/test/p1")) == -1){
		PutString("\nNo Memory to fork the process");
		PutInt(id[7]);
	}
	if ((id[8] = ForkExec("/test/p1")) == -1){
		PutString("\nNo Memory to fork the process");
		PutInt(id[8]);
	}
	if ((id[9] = ForkExec("/test/p1")) == -1){
		PutString("\nNo Memory to fork the process");
		PutInt(id[9]);
	}
	
	int i;
	for (i = 0; i < 10; i ++)
	{
		//PutString("Wait ");
		//PutInt(id[i]);
		//PutChar('\n');
		Wait(id[i]);
		//PutString("Finish\n");
	}
	PutString("\nEnd main\n");
	return 0;
}
