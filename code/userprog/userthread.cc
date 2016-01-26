#ifdef CHANGED
#include "userthread.h"

static void StartUserThread(int f) {

	Args * param = (Args*) f;

	ASSERT(param!=NULL);
	ASSERT(param->f!=0);

	machine->WriteRegister(RetAddrReg,param->callBack); // to call UserThreadExit implicit

	machine->WriteRegister(PCReg,param->f);
	machine->WriteRegister(NextPCReg,(param->f)+4);
	machine->WriteRegister(4,param->arg);

	delete(param);
	int stack = currentThread->space->GetStackForThread();

	if (stack == -1)
	{
		currentThread->Finish();
	}

	machine->WriteRegister(StackReg, stack);

	machine->Run();
}

int do_UserThreadCreate(int f, int arg, int callBack)
{
	if (currentThread->space->GetNbThread() >= currentThread->space->GetMaxThread())
	{
		return -1;
	}
	int newId = currentThread->space->NewThread();
	Thread *newThread = new Thread("User Thread", newId);

	if(newThread == NULL) {
		DEBUG('t',"Error while creating thread");
		return -1;
	}

	Args *args = new Args;
	args->f = f;
	args->arg = arg;
	args->callBack = callBack;

	newThread->Fork(StartUserThread, (int)args);

	return newId;
}

void do_UserThreadExit()
{
	currentThread->Finish();
}

void do_UserThreadJoin(int threadId){
	currentThread->space->JoinThread(threadId);
}

#endif //CHANGED
