// addrspace.cc 
//      Routines to manage address spaces (executing user programs).
//
//      In order to run a user program, you must:
//
//      1. link with the -N -T 0 option 
//      2. run coff2noff to convert the object file to Nachos format
//              (Nachos object code format is essentially just a simpler
//              version of the UNIX executable object code format)
//      3. load the NOFF file into the Nachos file system
//              (if you haven't implemented the file system yet, you
//              don't need to do this last step)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "addrspace.h"
#include "noff.h"

#ifdef CHANGED
#include "frameProvider.h"
#define MAX_OPEN_FILE 10
#endif

#include <strings.h>		/* for bzero */

//----------------------------------------------------------------------
// SwapHeader
//      Do little endian to big endian conversion on the bytes in the 
//      object file header, in case the file was generated on a little
//      endian machine, and we're now running on a big endian machine.
//----------------------------------------------------------------------

	static void
SwapHeader (NoffHeader * noffH)
{
	noffH->noffMagic = WordToHost (noffH->noffMagic);
	noffH->code.size = WordToHost (noffH->code.size);
	noffH->code.virtualAddr = WordToHost (noffH->code.virtualAddr);
	noffH->code.inFileAddr = WordToHost (noffH->code.inFileAddr);
	noffH->initData.size = WordToHost (noffH->initData.size);
	noffH->initData.virtualAddr = WordToHost (noffH->initData.virtualAddr);
	noffH->initData.inFileAddr = WordToHost (noffH->initData.inFileAddr);
	noffH->uninitData.size = WordToHost (noffH->uninitData.size);
	noffH->uninitData.virtualAddr =
		WordToHost (noffH->uninitData.virtualAddr);
	noffH->uninitData.inFileAddr = WordToHost (noffH->uninitData.inFileAddr);
}

#ifdef CHANGED
static void ReadAtVirtual( OpenFile *executable, int virtualaddr, int numBytes, int position, TranslationEntry *pageTable,unsigned numPages){
	char buffer[numBytes];

	TranslationEntry* oldPt = machine->pageTable;
	unsigned int oldNumP = machine->pageTableSize;
	
	machine->pageTable = pageTable;
	machine->pageTableSize = numPages;

	int read = executable->ReadAt(buffer, numBytes, position);

	for (int i = 0; i < read; i ++)
		machine->WriteMem(virtualaddr+i, 1, buffer[i]);

	machine->pageTable = oldPt;
	machine->pageTableSize = oldNumP;
}

static FrameProvider frameProvider;
#endif //CHANGED

//----------------------------------------------------------------------
// AddrSpace::AddrSpace
//      Create an address space to run a user program.
//      Load the program from a file "executable", and set everything
//      up so that we can start executing user instructions.
//
//      Assumes that the object code file is in NOFF format.
//
//      First, set up the translation from program memory to physical 
//      memory.  For now, this is really simple (1:1), since we are
//      only uniprogramming, and we have a single unsegmented page table
//
//      "executable" is the file containing the object code to load into memory
//----------------------------------------------------------------------

#ifdef CHANGED
bool CanCreateNewSpace(OpenFile *exec){
	NoffHeader noffH;
	int size;

	exec->ReadAt ((char *) &noffH, sizeof (noffH), 0);
	if ((noffH.noffMagic != NOFFMAGIC) &&
			(WordToHost (noffH.noffMagic) == NOFFMAGIC))
		SwapHeader (&noffH);
	ASSERT (noffH.noffMagic == NOFFMAGIC);

	// how big is address space?
	size = noffH.code.size + noffH.initData.size + noffH.uninitData.size + UserStackSize;	// we need to increase the size

	int numPages = divRoundUp (size, PageSize);
	size = numPages * PageSize;
	if (numPages > NumPhysPages || frameProvider.NumAvailFrame() < numPages)
		return false;

	return true;
}
#endif //CHANGED

AddrSpace::AddrSpace (OpenFile * executable)
{
	NoffHeader noffH;
	unsigned int i, size;

	executable->ReadAt ((char *) &noffH, sizeof (noffH), 0);
	if ((noffH.noffMagic != NOFFMAGIC) &&
			(WordToHost (noffH.noffMagic) == NOFFMAGIC))
		SwapHeader (&noffH);
	ASSERT (noffH.noffMagic == NOFFMAGIC);

	// how big is address space?
	size = noffH.code.size + noffH.initData.size + noffH.uninitData.size + UserStackSize;	// we need to increase the size
	// to leave room for the stack
	numPages = divRoundUp (size, PageSize);
	size = numPages * PageSize;

	ASSERT (numPages <= NumPhysPages);	// check we're not trying
	// to run anything too big --
	// at least until we have
	// virtual memory
#ifdef CHANGED
	listThread = new ListWaiter();
	nbThread = 1;
	idThread = 1;
	haltLock = new Lock("Lock for halt");
	haltLock->Acquire();
	lockId = new Lock("Lock for id");
	memoryMap = new BitMap(numPages/NbPagesPerThread);
	userSem = new UserSemList();

	processId = machine->AddProcess();
	
	openFileMap = new BitMap(MAX_OPEN_FILE);
	openFileTable = new int[MAX_OPEN_FILE];

	for (i = 2; i < MAX_OPEN_FILE; i ++)
		openFileTable[i] = -1;
	openFileTable[0] = 0;
	openFileTable[1] = 1;
	openFileMap->Mark(0);
	openFileMap->Mark(1);
#endif

	DEBUG ('a', "Initializing address space, num pages %d, size %d\n",
			numPages, size);
	// first, set up the translation 
	pageTable = new TranslationEntry[numPages];

	for (i = 0; i < numPages; i++)
	{
		pageTable[i].virtualPage = i;	// for now, virtual page # = phys page #
#ifndef CHANGED
		pageTable[i].physicalPage = i;
#else
		pageTable[i].physicalPage = frameProvider.GetEmptyFrame();
#endif
		pageTable[i].valid = TRUE;
		pageTable[i].use = FALSE;
		pageTable[i].dirty = FALSE;
		pageTable[i].readOnly = FALSE;	// if the code segment was entirely on 
		// a separate page, we could set its 
		// pages to be read-only
	}

	// zero out the entire address space, to zero the unitialized data segment 
	// and the stack segment
//	bzero (machine->mainMemory, size);

	// then, copy in the code and data segments into memory
	if (noffH.code.size > 0)
	{
		DEBUG ('a', "Initializing code segment, at 0x%x, size %d\n",
				noffH.code.virtualAddr, noffH.code.size);
#ifndef CHANGED
		executable->ReadAt (&(machine->mainMemory[noffH.code.virtualAddr]),
				noffH.code.size, noffH.code.inFileAddr);
#else
		ReadAtVirtual(executable, noffH.code.virtualAddr, noffH.code.size, noffH.code.inFileAddr, pageTable, numPages);
#endif
	}
	if (noffH.initData.size > 0)
	{
		DEBUG ('a', "Initializing data segment, at 0x%x, size %d\n",
				noffH.initData.virtualAddr, noffH.initData.size);
#ifndef CHANGED
		executable->ReadAt (&
				(machine->mainMemory
				 [noffH.initData.virtualAddr]),
				noffH.initData.size, noffH.initData.inFileAddr);
#else
		ReadAtVirtual(executable, noffH.initData.virtualAddr, noffH.initData.size, noffH.initData.inFileAddr, pageTable, numPages);
#endif
	}

#ifdef CHANGED
	unsigned int nbAllocated = divRoundUp(divRoundUp(noffH.initData.virtualAddr+ noffH.initData.size, PageSize),NbPagesPerThread);
	for (i = 0; i < nbAllocated; i ++)
		memoryMap->Mark(i);

	maxThreads = numPages/NbPagesPerThread - nbAllocated;
#endif

}

//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
//      Dealloate an address space.  Nothing for now!
//----------------------------------------------------------------------

AddrSpace::~AddrSpace ()
{
	// End of modification

#ifdef CHANGED
	delete haltLock;
	delete memoryMap;
	delete listThread;
	delete lockId;
	delete userSem;

	for (unsigned int i = 0; i < numPages; i ++)
		frameProvider.ReleaseFrame(pageTable[i].physicalPage);
#endif

	// LB: Missing [] for delete
	// delete pageTable;
	delete [] pageTable;
}

//----------------------------------------------------------------------
// AddrSpace::InitRegisters
//      Set the initial values for the user-level register set.
//
//      We write these directly into the "machine" registers, so
//      that we can immediately jump to user code.  Note that these
//      will be saved/restored into the currentThread->userRegisters
//      when this thread is context switched out.
//----------------------------------------------------------------------

	void
AddrSpace::InitRegisters ()
{
	int i;

	for (i = 0; i < NumTotalRegs; i++)
		machine->WriteRegister (i, 0);

	// Initial program counter -- must be location of "Start"
	machine->WriteRegister (PCReg, 0);

	// Need to also tell MIPS where next instruction is, because
	// of branch delay possibility
	machine->WriteRegister (NextPCReg, 4);

	// Set the stack register to the end of the address space, where we
	// allocated the stack; but subtract off a bit, to make sure we don't
	// accidentally reference off the end!
	machine->WriteRegister (StackReg, numPages * PageSize - 16);
	DEBUG ('a', "Initializing stack register to %d\n",
			numPages * PageSize - 16);

#ifdef CHANGED
	memoryMap->Mark(numPages/NbPagesPerThread-1);
#endif //CHANGED
}

//----------------------------------------------------------------------
// AddrSpace::SaveState
//      On a context switch, save any machine state, specific
//      to this address space, that needs saving.
//
//      For now, nothing!
//----------------------------------------------------------------------

	void
AddrSpace::SaveState ()
{
	pageTable = machine->pageTable;
	numPages = machine->pageTableSize;
}

//----------------------------------------------------------------------
// AddrSpace::RestoreState
//      On a context switch, restore the machine state so that
//      this address space can run.
//
//      For now, tell the machine where to find the page table.
//----------------------------------------------------------------------

	void
AddrSpace::RestoreState ()
{
	machine->pageTable = pageTable;
	machine->pageTableSize = numPages;
}

#ifdef CHANGED

int AddrSpace::NewThread(){

	lockId->Acquire();

	nbThread ++;
	idThread ++;
	int id = idThread;
	listThread->New(id);

	lockId->Release();

	return id;
}

int AddrSpace::GetNbThread(){
	return nbThread;
}

// must return -1 if no space available
int AddrSpace::GetStackForThread(){
	int position = memoryMap->Find();
	if (position == -1)
		return -1;

	currentThread->SetStackPosition(position);
	return (position+1)*NbPagesPerThread*PageSize-16;
}

void AddrSpace::WaitThread(){
	haltLock->Acquire();
	haltLock->Release();
}

void AddrSpace::EndThread(){
	lockId->Acquire();
	nbThread --;
	lockId->Release();

	if (currentThread->GetId() != 1)
		listThread->End(currentThread->GetId());
	
	DeallocateMapStack(currentThread->GetStackPosition());

	haltLock->Release();

	if (GetNbThread() == 0)
		delete this;
}

void AddrSpace::JoinThread(unsigned int id){
	listThread->Wait(id);
}

void AddrSpace::DeallocateMapStack(int position){
	if (position != -1)
		memoryMap->Clear(position);
}

int AddrSpace::GetMaxThread(){
	return maxThreads;
}

int AddrSpace::AddOpenFile(int fileId){
	if (!CanOpenFile())
		return -1;

	int index = GetOpenFileId(fileId);
	if (index == -1)
	{
		index = openFileMap->Find();
		ASSERT(index != -1);
		openFileTable[index] = fileId;
	}

	return index;
}

int AddrSpace::GetOpenFileId(int fileId){
	for (int i = 0; i < MAX_OPEN_FILE; i ++)
		if (openFileTable[i] == fileId)
			return i;

	return -1;
}

void AddrSpace::CloseOpenFile(int fileId){
	int index = GetOpenFileId(fileId);
	if (index != -1)
	{
		openFileMap->Clear(index);
		openFileTable[index] = -1;
	}
}

bool AddrSpace::CanOpenFile(){
	return openFileMap->NumClear() > 0;
}

int AddrSpace::GetId(){
	return processId;
}
#endif
