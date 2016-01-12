#ifdef CHANGED
#include "fork.h"
#include "thread.h"
#include "addrspace.h"
#include "system.h"

static void StartUserProcess(int arg){
	currentThread->space = (AddrSpace*)arg;
	currentThread->space->InitRegisters();	// set the initial register values
	currentThread->space->RestoreState();	// load page table register


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
	machine->addProcess();

	newThread->Fork(StartUserProcess, (int)space);
	//scheduler->ReadyToRun(newThread);

	return 0;
}

#endif //CHANGED
