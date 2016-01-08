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

extern SynchConsole *synchConsole;
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
void copyStringFromMachine( int from, char *to, unsigned size) {
	int byte;
	unsigned int i;
	int offset = 0;

	do {
		for (i = 0; i < size -1; i ++)
		{
			machine->ReadMem(from + offset+i, 1, &byte);

			if ((char)byte == '\0')
			break;
			to[i] = (char)byte;
		}
		offset += i;
		to[i] = '\0';
		synchConsole->SynchPutString(to);
	}while((char)byte != '\0');

	/*
	 while (size > 0 && (char)byte != '\0')
	 {
	 machine->ReadMem(from, 1, &byte);
	 from ++;
	 *to = (char)byte;
	 to ++;
	 size --;
	 }

	 if ((char) byte != '\0')
	 to = '\0';
	 */
}

void writeStringToMachine(char* string, int to, unsigned size) {
	int i;
	for (i = 0; i < (int)size; i++)
	machine->WriteMem(to + i, 1, string[i]);
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
				interrupt->Halt();
				break;
			}
			case SC_PutChar: { // to write a character to the console
				DEBUG('m', "PutChar, system call handler. \n");

				int charint = machine->ReadRegister(4);
				char ch = (char) charint;
				synchConsole->SynchPutChar(ch);
				break;
			}
			case SC_GetChar:
			DEBUG('a', "GetChar, system call handler.\n");

			machine->WriteRegister(2, (int)synchConsole->SynchGetChar());
			break;
			case SC_PutString:
			{
				DEBUG('a', "PutString, system call handler.\n");
				char *buffer = new char[MAX_STRING_SIZE];
				copyStringFromMachine(machine->ReadRegister(4), buffer, MAX_STRING_SIZE);
				delete[] buffer;
				break;
			}
			case SC_GetString:
			{
				DEBUG('a', "GetString, system call handler.\n");

				char *kernelBuffer = new char[MAX_STRING_SIZE];
				int offset = 0;
				int mipsBuffer = machine->ReadRegister(4);
				int maxSize = machine->ReadRegister(5)-1;
				int mustRead;

				while (offset < maxSize) {
					if (maxSize - offset > MAX_STRING_SIZE)
					mustRead = MAX_STRING_SIZE;
					else
					mustRead = maxSize - offset;

					int read = synchConsole->SynchGetString(kernelBuffer, mustRead);
					writeStringToMachine(kernelBuffer, mipsBuffer + offset, read);
					offset += read;
					if (kernelBuffer[read-1] == '\n' || kernelBuffer[read-1] == EOF|| kernelBuffer[read-1] == '\0')
					break;
				}
				machine->WriteMem(mipsBuffer+offset+1, 1, '\0');
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
			case SC_Exit:
				DEBUG('a', "Shutdown, initiated by user program.\n");
				interrupt->Halt();
				break;
			case SC_UserThreadCreate:
			{
				DEBUG('a', "UserThreadCreate, system call handler.\n");
				int function = machine->ReadRegister(4);
				int arg = machine->ReadRegister(5);
				int ret = do_UserThreadCreate(function, arg);

				machine->WriteRegister(2, ret);
				break;
			}
			case SC_UserThreadExit:
				DEBUG('a', "UserThreadExit, system call handler.\n");
				do_UserThreadExit();
				break;
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
