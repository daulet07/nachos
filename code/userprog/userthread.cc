#ifdef CHANGED
#include "userthread.h"

static void StartUserThread(int f) {

	Args * param = (Args*) f;

	ASSERT(param!=NULL);
	ASSERT(param->f!=0);
	ASSERT(param->arg!=0);

	machine->WriteRegister(PCReg,param->f);
	machine->WriteRegister(NextPCReg,(param->f)+4);
	machine->WriteRegister(4,param->arg);

	delete(param);
	currentThread->space->increaseThread();
	int stack = currentThread->space->getStackForThread();
	fprintf(stderr, "stack = %d\n", stack);

	if (stack == -1)
		currentThread->Finish();

	machine->WriteRegister(StackReg, stack);

	machine->Run();
}

int do_UserThreadCreate(int f, int arg)
{
	Thread *newThread = new Thread("User Thread");

	if(newThread == NULL) {
		DEBUG('t',"Error while creating thread");
		return -1;
	}

	Args *args = new Args;
	args->f = f;
	args->arg = arg;

	newThread->Fork(StartUserThread, (int)args);

	return 0;
}

void do_UserThreadExit()
{
	currentThread->Finish();
}

void do_UserThreadJoin(int threadId){
//	thread->waitThread();
}

#endif //CHANGED
