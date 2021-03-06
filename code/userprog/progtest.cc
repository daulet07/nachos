// progtest.cc 
//      Test routines for demonstrating that Nachos can load
//      a user program and execute it.  
//
//      Also, routines for testing the Console hardware device.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "console.h"
#include "synchconsole.h"
#include "addrspace.h"
#include "synch.h"

//----------------------------------------------------------------------
// StartProcess
//      Run a user program.  Open the executable, load it into
//      memory, and jump to it.
//----------------------------------------------------------------------

void StartProcess(char *filename) {
	OpenFile *executable = fileSystem->Open(filename);
	AddrSpace *space;

	if (executable == NULL) {
		printf("Unable to open file %s\n", filename);
		return;
	}
	space = new AddrSpace(executable);
	currentThread->space = space;

	delete executable;		// close file

	space->InitRegisters();	// set the initial register values
	space->RestoreState();	// load page table register

	machine->Run();		// jump to the user progam
	ASSERT(FALSE);		// machine->Run never returns;
	// the address space exits
	// by doing the syscall "exit"
}

// Data structures needed for the console test.  Threads making
// I/O requests wait on a Semaphore to delay until the I/O completes.

static Console *console;
static Semaphore *readAvail;
static Semaphore *writeDone;

//----------------------------------------------------------------------
// ConsoleInterruptHandlers
//      Wake up the thread that requested the I/O.
//----------------------------------------------------------------------

static void ReadAvail(int arg) {
	readAvail->V();
}
static void WriteDone(int arg) {
	writeDone->V();
}

//----------------------------------------------------------------------
// ConsoleTest
//      Test the console by echoing characters typed at the input onto
//      the output.  Stop when the user types a 'q'.
//----------------------------------------------------------------------
void TestAsynchCOnsole(char *in, char *out);

void ConsoleTest(char *in, char *out) {
	//TestAsynchCOnsole(in, out);
#ifdef CHANGED
	delete synchConsole;
	synchConsole = NULL;
#endif

	console = new Console(in, out, ReadAvail, WriteDone, 0);
	readAvail = new Semaphore("read avail", 0);
	writeDone = new Semaphore("write done", 0);

#ifdef CHANGED
	char ch;
	for (;;) {

		readAvail->P ();	// wait for character to arrive
		ch = console->GetChar ();
		if (ch == EOF)
			return;

		if (ch == 'c')
		{
			console->PutChar('<');
			writeDone->P ();	// wait for write to finish
			console->PutChar('c');
			writeDone->P ();// wait for write to finish
			console->PutChar('>');
			writeDone->P ();// wait for write to finish
		}
		else
		{
			console->PutChar (ch);	// echo it!
			writeDone->P ();// wait for write to finish
			if (ch == 'q')
				return;// if q, quit
		}

	}
#endif //CHANGED
}

#ifdef CHANGED
void SynchConsoleTest (char *in, char *out)
{
	char ch;

	if(in != NULL && out !=NULL ){
		delete synchConsole;
		synchConsole = new SynchConsole(in, out);
	}

	while ((ch = synchConsole->SynchGetChar()) != EOF){
		synchConsole->SynchPutChar(ch);
	}

	fprintf(stderr, "Solaris: EOF detected in SynchConsole!\n");
}

/*
 static void ReadAsync(int i) {
 char ch = console->GetChar();
 console->PutChar(ch);
 console->WriteDone();
 }

 static void WriteAsync(int i) {
 }
 #include <unistd.h>

 void TestAsynchCOnsole(char *in, char *out)
 {
 console = new Console(in, out, ReadAsync, WriteAsync, 0);

 int i = 0;
 while(i < 100){
 i ++;
 sleep(1);
 currentThread->Yield();
 fprintf(stderr, "i = %d\n", i);
 }
 }
 */

#endif //CHANGED
