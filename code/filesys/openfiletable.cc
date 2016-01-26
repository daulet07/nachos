#ifdef CHANGED
#include "filesys.h"
#include "openfiletable.h"
#include "system.h"

OpenFileTable::OpenFileTable(){
	map = new BitMap(MAX_OPEN_FILE);
	table = new FileTableEntry[MAX_OPEN_FILE];
	
	for (int i = 0; i < MAX_OPEN_FILE; i ++)
	{
		table[i].inUse = false;
		table[i].file = NULL;
		table[i].process = NULL;
	}
	table[0].inUse = true;
	map->Mark(0);
	table[1].inUse = true;
	map->Mark(1);
}

OpenFileTable::~OpenFileTable(){
	for (int i = 2; i < MAX_OPEN_FILE; i ++)
		if (table[i].inUse)
		{
			while (table[i].process != NULL)
			{
				ProcessTableEntry *proc = table[i].process;
				table[i].process = proc->next;
				delete proc;
			}
			delete table[i].file;
		}

	delete map;
	delete [] table;
}


ProcessTableEntry* OpenFileTable::GetProcessEntry(int index){
	ProcessTableEntry *proc = table[index].process;
	int procId = currentThread->space->GetId();
	while (proc != NULL && proc->processId != procId)
		proc = proc->next;

	return proc;
}

void OpenFileTable::AppendProcess(int index){
	ProcessTableEntry *proc = new ProcessTableEntry;

	proc->processId = currentThread->space->GetId();
	proc->position = 0;
	proc->next = table[index].process;

	if (table[index].process!= NULL)
		table[index].process->prev = proc;

	table[index].process = proc;
}

int OpenFileTable::Open(const char *from, const char *name){
	int fileId = IsOpen(from, name);
	if (fileId == -1) // The file is not allready open
	{
		if (!CanOpen() || !currentThread->space->CanOpenFile())
			return -1;

		OpenFile *file = fileSystem->Open(from, name);
		if (file == NULL)
			return -1;

		int index = map->Find();

		ASSERT(index >= 2);

		table[index].inUse = true;
		strncpy(table[index].path, from, MAX_PATH_LENGTH);
		strncpy(table[index].name, name, FileNameMaxLen);
		table[index].file = file;
		table[index].process = NULL;

		AppendProcess(index);
		
		fileId = currentThread->space->AddOpenFile(index);
	}
	else if (GetProcessEntry(fileId) == NULL) // Current thread  have not open it
	{
		if (!currentThread->space->CanOpenFile())
			return -1;

		AppendProcess(fileId);
		fileId = currentThread->space->AddOpenFile(fileId);

	}
	else // File is open for the thread
	{
		fileId = currentThread->space->GetOpenFileId(fileId);
	}
			
	ASSERT(fileId >= 2);
	return fileId;
	/*
	int index = map->Find();
	if (index >= 2)
	{
		table[index].inUse = true;
		strncpy(table[index].path, from, MAX_PATH_LENGTH);
		strncpy(table[index].name, name, FileNameMaxLen);
		table[index].file = file;
		table[index].count = 0;
		table[index].process = NULL;
	}
	return index;
	*/
}

int OpenFileTable::IsOpen(const char *from, const char *name){
	for (int i = 0; i < MAX_OPEN_FILE; i ++)
		if (table[i].inUse && 
				strcmp(from, table[i].path) == 0 && 
				strcmp(name, table[i].name) == 0)
			return i;
	return -1;
}

bool OpenFileTable::CanOpen(){
	return map->NumClear() > 0;
}

void OpenFileTable::Close(int index){
	if (index >= 2 && index < MAX_OPEN_FILE && table[index].inUse)
	{
		ProcessTableEntry *procEntry = GetProcessEntry(index);
		if (procEntry != NULL)
		{
			if (table[index].process == procEntry)
				table[index].process = procEntry->next;
			else
			{
				procEntry->prev->next = procEntry->next;
				if (procEntry->next != NULL)
					procEntry->next->prev = procEntry->prev;
			}

			delete procEntry;

			if (table[index].process == NULL)
			{
				map->Clear(index);
				table[index].inUse = false;
				table[index].file->FlushHeader();
				delete table[index].file;
			}
		}
	}
}

int OpenFileTable::FRead(char* buffer, int size, int fileId){
	if (fileId < 2)
// TO DO
		return 0;

	if (!table[fileId].inUse)
		return 0;

	ProcessTableEntry *procEntry = GetProcessEntry(fileId);
	if (procEntry == NULL)
		return 0;

	OpenFile *file = table[fileId].file;
	int num = file->ReadAt(buffer, size, procEntry->position);
	procEntry->position += num;
	buffer[num] = '\0';
	return num;
}

void OpenFileTable::FWrite(char* buffer, int size, int fileId){
	if (fileId < 2)
// TO DO
		return;

	if (table[fileId].inUse)
	{
		ProcessTableEntry *procEntry = GetProcessEntry(fileId);
		if (procEntry == NULL)
			return;

		OpenFile *file = table[fileId].file;
		int num = file->WriteAt(buffer, size, procEntry->position);
		procEntry->position += num;
	}
}
#endif //CHANGED
