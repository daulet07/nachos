// addrspace.h 
//      Data structures to keep track of executing user programs 
//      (address spaces).
//
//      For now, we don't keep any information about address spaces.
//      The user level CPU state is saved and restored in the thread
//      executing the user program (see thread.h).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef ADDRSPACE_H
#define ADDRSPACE_H

#include "copyright.h"
#include "filesys.h"

#ifdef CHANGED
#include "bitmap.h"
#include "listwait.h"
class Lock;
#define NbPagesPerThread 2
#include "usersemaphore.h"
#endif

#define UserStackSize	2048//4096 //2048	//1024	// increase this as necessary!

#ifdef CHANGED
bool CanCreateNewSpace(OpenFile *exec);
#endif

class AddrSpace
{
	public:
		AddrSpace (OpenFile * executable);	// Create an address space,
		// initializing it with the program
		// stored in the file "executable"
		~AddrSpace ();		// De-allocate an address space

		void InitRegisters ();	// Initialize user-level CPU registers,
		// before jumping to user code

		void SaveState ();		// Save/restore address space-specific
		void RestoreState ();	// info on a context switch 

#ifdef CHANGED
		int NewThread(); //return the id of the new thread
		int GetStackForThread();
		int GetNbThread();
		void WaitThread();
		void EndThread();
		void JoinThread(unsigned int id);
		void DeallocateMapStack(int position);
		int GetMaxThread();
		UserSemList *userSem;

		int AddOpenFile(int fileId);
		bool CanOpenFile();
		int GetOpenFileId(int fileId);
		void CloseOpenFile(int fileId);

		int GetId();
#endif

	private:
		TranslationEntry * pageTable;	// Assume linear page table translation
		// for now!
		unsigned int numPages;	// Number of pages in the virtual 
		// address space

#ifdef CHANGED
		unsigned int nbThread;
		unsigned int idThread;
		BitMap *memoryMap;
		Lock *haltLock;
		ListWaiter *listThread;
		int maxThreads;
		Lock* lockId;

		BitMap *openFileMap;
		int *openFileTable;

		int processId;
#endif
};

#endif // ADDRSPACE_H
