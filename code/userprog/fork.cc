#ifdef CHANGED
#include "fork.h"
#include "thread.h"
#include "addrspace.h"
#include "system.h"

static void StartUserProcess(int arg){
	currentThread->space->InitRegisters();	// set the initial register values
	currentThread->space->RestoreState();	// load page table register

	fprintf(stderr, "A new process start\n");
	fprintf(stderr, "========================================================\n");

	machine->Run();
}

int do_UserForkExec(char *pName){
	OpenFile *exec = fileSystem->Open(pName);	
	AddrSpace *space;

	if (exec == NULL) {
		printf("Unable to open file %s\n", pName);
		return -1;
	}
	space = new AddrSpace(exec);
	
	delete exec;		// close file
	Thread *newThread = new Thread("new user process");
	newThread->space = space;

	newThread->Fork(StartUserProcess, (int)0);
	//scheduler->ReadyToRun(newThread);

	return 0;
}

#endif //CHANGED
