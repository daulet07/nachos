// exception.cc 
//      Entry point into the Nachos kernel from user programs.
//      There are two kinds of things that can cause control to
//      transfer back to here from user code:
//
//      syscall -- The user code explicitly requests to call a procedure
//      in the Nachos kernel.  Right now, the only function we support is
//      "Halt".
//
//      exceptions -- The user code does something that the CPU can't handle.
//      For instance, accessing memory that doesn't exist, arithmetic errors,
//      etc.  
//
//      Interrupts (which can also cause control to transfer from user
//      code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"

#ifdef CHANGED
#include "userthread.h"
#include "fork.h"
#endif //CHANGED
//----------------------------------------------------------------------
// UpdatePC : Increments the Program Counter register in order to resume
// the user program immediately after the "syscall" instruction.
//----------------------------------------------------------------------
static void UpdatePC() {
	int pc = machine->ReadRegister(PCReg);
	machine->WriteRegister(PrevPCReg, pc);
	pc = machine->ReadRegister(NextPCReg);
	machine->WriteRegister(PCReg, pc);
	pc += 4;
	machine->WriteRegister(NextPCReg, pc);
}

#ifdef CHANGED
int copyStringFromMachine( int from, char *to, unsigned size) {
	int byte;
	unsigned int i = 0;
	do
	{
		machine->ReadMem(from + i, 1, &byte);
		to[i] = (char)byte;
		i ++;
	}while ((char)byte != '\0' && i < size);
	return i;
}

void writeStringToMachine(char* string, int to, unsigned size) {
	int i;
	for (i = 0; i < (int)size; i++)
		machine->WriteMem(to + i, 1, string[i]);
}

static void haltMachine() {
	while (currentThread->space->GetNbThread() > 1)
		currentThread->space->WaitThread();

	if (machine->GetNumProcess() == 1)
		interrupt->Halt();

	if (currentThread->space->GetNbThread() == 1)
		machine->EndProcess(currentThread->space->GetId());

	currentThread->Finish();
}

#endif //CHANGED

//----------------------------------------------------------------------
// ExceptionHandler
//      Entry point into the Nachos kernel.  Called when a user program
//      is executing, and either does a syscall, or generates an addressing
//      or arithmetic exception.
//
//      For system calls, the following is the calling convention:
//
//      system call code -- r2
//              arg1 -- r4
//              arg2 -- r5
//              arg3 -- r6
//              arg4 -- r7
//
//      The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//      "which" is the kind of exception.  The list of possible exceptions 
//      are in machine.h.
//----------------------------------------------------------------------

void ExceptionHandler(ExceptionType which) {
	int type = machine->ReadRegister(2);

#ifndef CHANGED // Noter le if*n*def
	if ((which == SyscallException) && (type == SC_Halt)) {
		DEBUG('a', "Shutdown, initiated by user program.\n");
		interrupt->Halt();
	} else {
		printf("Unexpected user mode exception %d %d\n", which, type);
		ASSERT(FALSE);
	}
#else // CHANGED
	if (which == SyscallException)
	{
		switch (type) {
			case SC_Halt: {
				DEBUG('a', "Shutdown, initiated by user program.\n");
				haltMachine();
				break;
			}
			case SC_PutChar: { // to write a character to the console
				DEBUG('m', "PutChar, system call handler. \n");

				int charint = machine->ReadRegister(4);
				char ch = (char) charint;
				synchConsole->SynchPutChar(ch);
				break;
			}
			case SC_GetChar: {
				DEBUG('a', "GetChar, system call handler.\n");

				machine->WriteRegister(2, (int)synchConsole->SynchGetChar());
				break;
			}
			case SC_PutString:
			{
				DEBUG('a', "PutString, system call handler.\n");
				char *buffer = new char[MAX_STRING_SIZE];
				int size, alreadyRead = 0;
				do {
					size = copyStringFromMachine(machine->ReadRegister(4) + alreadyRead, buffer, MAX_STRING_SIZE);
					synchConsole->SynchPutString(buffer, size);
					alreadyRead += size;
				}
				while (/*size < MAX_STRING_SIZE && */buffer[size-1] != '\0');

				delete[] buffer;
				break;
			}
			case SC_GetString:
			{
				DEBUG('a', "GetString, system call handler.\n");

				char *kernelBuffer = new char[MAX_STRING_SIZE];
				int offset = 0;
				int mipsBuffer = machine->ReadRegister(4);
				int maxSize = machine->ReadRegister(5);
				int mustRead, read;

				do{
					if (maxSize <  offset + MAX_STRING_SIZE)
						mustRead = MAX_STRING_SIZE;
					else
						mustRead = offset + MAX_STRING_SIZE - maxSize;
					read = synchConsole->SynchGetString(kernelBuffer, mustRead);
					if (kernelBuffer[read-1] == '\n')
						kernelBuffer[read-1] = '\0';
					writeStringToMachine(kernelBuffer, mipsBuffer + offset, read);
					offset += read;
				} while(kernelBuffer[read-1] != '\0');

				delete [] kernelBuffer;
				break;
			}
			case SC_PutInt:
			{
				DEBUG('a', "PutInt, system call handler.\n");
				int number = machine->ReadRegister(4);
				synchConsole->SynchPutInt(number);
				break;
			}
			case SC_GetInt:
			{
				DEBUG('a', "GetInt, system call handler.\n");
				int pointer = machine->ReadRegister(4);
				int num;
				synchConsole->SynchGetInt(&num);

				machine->WriteMem(pointer, sizeof(int), num);

				break;
			}
			case SC_Exit: {
				DEBUG('a', "Shutdown, initiated by user program.\n");
				haltMachine();
				break;
			}
			case SC_UserThreadCreate:
			{
				DEBUG('a', "UserThreadCreate, system call handler.\n");
				int function = machine->ReadRegister(4);
				int arg = machine->ReadRegister(5);
				int callBack = machine->ReadRegister(6);
				int ret = do_UserThreadCreate(function, arg, callBack);

				machine->WriteRegister(2, ret);
				break;
			}
			case SC_UserThreadExit: {
				DEBUG('a', "UserThreadExit, system call handler.\n");
				do_UserThreadExit();
				break;
			}
			case SC_UserThreadJoin:
			{
				DEBUG('a', "UserThreadJoin, system call handler.\n");
				int threadId = machine->ReadRegister(4);
				do_UserThreadJoin(threadId);
				break;
			}
			case SC_ForkExec:
			{
				DEBUG('a', "ForkExec, system call handler.\n");

				char *exec = new char[MAX_STRING_SIZE];
				copyStringFromMachine(machine->ReadRegister(4), exec, MAX_STRING_SIZE);
				int ret = do_UserForkExec(exec);
				machine->WriteRegister(2, ret);
				delete exec;
				break;
			}
			case SC_Wait:
			{
				DEBUG('s', "SC_Wait.\n");
				int id = machine->ReadRegister(4);
				
				do_UserWait(id);
				break;
			}
			case SC_SemInit:
			{
				DEBUG('s', "SC_SemInit.\n");
				int reg4 = machine->ReadRegister(4);
				int result = currentThread->space->userSem->sem_create(reg4);
				machine->WriteRegister(2, result);
				break;
			}
			case SC_P:
			{
				DEBUG('s', "SC_P.\n");
				int reg4 = machine->ReadRegister(4);
				currentThread->space->userSem->sem_p(reg4);
				break;
			}
			case SC_V:
			{
				DEBUG('s', "SC_V.\n");
				int reg4 = machine->ReadRegister(4);
				currentThread->space->userSem->sem_v(reg4);
				break;
			}
			case SC_SemDestroy:
			{
				DEBUG('s', "SC_SemDestroy.\n");
				int reg4 = machine->ReadRegister(4);
				currentThread->space->userSem->sem_destroy(reg4);
				break;
			}
#ifndef FILESYS_STUB 		// Temporarily implement file system calls as 
			case SC_FCreate:
			{
				DEBUG('s', "SC_FCreate.\n");
				char path[MAX_STRING_SIZE];
				copyStringFromMachine(machine->ReadRegister(4), path, MAX_STRING_SIZE);
				int result = fileSystem->CreateFile(path, 0);
				machine->WriteRegister(2, result);
				break;
			}
			case SC_FOpen:
			{
				DEBUG('s', "SC_FOpen.\n");
//				OpenFileId FOpen (char *name);
				char path[MAX_STRING_SIZE];
				copyStringFromMachine(machine->ReadRegister(4), path, MAX_STRING_SIZE);
				int fileId = fileSystem->FOpen(path);
				machine->WriteRegister(2, fileId);
				break;
			}
			case SC_FRead:
			{
				DEBUG('s', "SC_FRead.\n");

				int userBuffer = machine->ReadRegister(4);
				int size = machine->ReadRegister(5);
				int fileId = machine->ReadRegister(6);

				char kernelBuffer[size];

				int numRead = fileSystem->FRead(kernelBuffer, size, fileId);

				writeStringToMachine(kernelBuffer, userBuffer, numRead);

				machine->WriteRegister(2, numRead);
//				int FRead (char *buffer, int size, OpenFileId id);
				break;
			}
			case SC_FWrite:
			{
				DEBUG('s', "SC_FWrite.\n");
				int userBuffer = machine->ReadRegister(4);
//				int size = machine->ReadRegister(5);
				int fileId = machine->ReadRegister(5);
				int alreadyRead = 0;
				int size;

				char kernelBuffer[MAX_STRING_SIZE];
				do{
					size = copyStringFromMachine(userBuffer + alreadyRead, kernelBuffer, MAX_STRING_SIZE);
					alreadyRead += size;
					if (kernelBuffer[size-1] == '\0')
						fileSystem->FWrite(kernelBuffer, size-1, fileId);
					else
						fileSystem->FWrite(kernelBuffer, size, fileId);
				} while (kernelBuffer[size-1] != '\0');

//				void FWrite (char *buffer, OpenFileId id);
//				void FWrite (char *buffer, int size, OpenFileId id);
				break;
			}
			case SC_FClose:
			{
				DEBUG('s', "SC_FClose.\n");
				fileSystem->FClose(machine->ReadRegister(4));
//				void FClose (OpenFileId id);
				break;
			}
			case SC_ListDir:
			{
				DEBUG('s', "SC_ListDir.\n");
				char *path = new char[MAX_STRING_SIZE];
				copyStringFromMachine(machine->ReadRegister(4), path, MAX_STRING_SIZE);
				Directory *dir = fileSystem->GetDir(path);
				if (dir != NULL)
				{
					dir->List();
					delete dir;
				}
				else
					printf("%s not found\n", path);

				delete []path;
				break;
			}
			case SC_MkDir:
			{
				DEBUG('s', "SC_MkDir.\n");
				char *path = new char[MAX_STRING_SIZE];
				copyStringFromMachine(machine->ReadRegister(4), path, MAX_STRING_SIZE);
				bool result = fileSystem->CreateDir(path);
				machine->WriteRegister(2, result);
				break;
			}
			case SC_Rm:
			{
				DEBUG('s', "SC_Rm.\n");
				char *path = new char[MAX_STRING_SIZE];
				copyStringFromMachine(machine->ReadRegister(4), path, MAX_STRING_SIZE);
				bool result = fileSystem->RemoveFile(path);
				machine->WriteRegister(2, result);
				break;
			}
			case SC_RmDir:
			{
				DEBUG('s', "SC_RmDir.\n");
				char *path = new char[MAX_STRING_SIZE];
				copyStringFromMachine(machine->ReadRegister(4), path, MAX_STRING_SIZE);
				bool result = fileSystem->RemoveDir(path);
				machine->WriteRegister(2, result);
				break;
			}
			case SC_IsDir:
			{
				DEBUG('s', "SC_IsDir.\n");
				char *path = new char[MAX_STRING_SIZE];
				copyStringFromMachine(machine->ReadRegister(4), path, MAX_STRING_SIZE);
				bool result = fileSystem->IsDir(path);
				machine->WriteRegister(2, result);
				break;
			}
#endif
			default: {
						 printf("Unexpected user mode exception %d %d\n", which, type);
						 ASSERT(FALSE);
						 break;
					 }
		} // end of switch case
	} // end of if statement
	else
	{
		printf("Unexpected user mode exception %d %d\n", which, type);
		ASSERT(FALSE);
	}
#endif // CHANGED
	UpdatePC();
} // end of ExceptionHandler
