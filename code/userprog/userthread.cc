#ifdef CHANGED
#include "userthread.h"


static void StartUserThread(int f){
	//	fprintf(stderr, "first call of a new thread\n");
	//Get args
	Args *args = (Args*)f;

	int func = args->f;
	int arg = args->arg;
	delete(args);

	currentThread->space->InitRegisters();
	currentThread->space->RestoreState();

	int stack = currentThread->space->getStackForThread();
	//	fprintf(stderr, "Stack = %d\n", stack);

	machine->WriteRegister(StackReg, stack);
	machine->WriteRegister(PCReg, func);
	machine->WriteRegister(NextPCReg, func+4);
	machine->WriteRegister(4, arg);

	//Initilize registers


	machine->Run();

}

int do_UserThreadCreate(int f, int arg){
	//Serialize the args
	//struct Args *args = malloc(sizeof(struct Args))
	Args *args = new Args;
	args->f = f;
	args->arg = arg;

	//Create the new thread
	//Thread *newThread = new Thread(currentThread->getName());
	Thread *newThread = new Thread("User Thread");
	newThread->Fork(StartUserThread, (int)args);


	return 0;
}

void UserThreadExit()
{
	currentThread->Finish();
}


#endif //CHANGED
