#ifdef CHANGED
#include "openfiletable.h"

OpenFileTable::OpenFileTable(){
	map = new BitMap(MAX_OPEN_FILE);
	table = new FileTableEntry[MAX_OPEN_FILE];
	
	for (int i = 0; i < MAX_OPEN_FILE; i ++)
	{
		table[i].inUse = false;
		table[i].file = NULL;
		table[i].count = 0;
	}
	table[0].inUse = true;
	map->Mark(0);
	table[1].inUse = true;
	map->Mark(1);
}

OpenFileTable::~OpenFileTable(){
	for (int i = 2; i < MAX_OPEN_FILE; i ++)
		if (table[i].inUse)
			delete table[i].file;

	delete map;
	delete [] table;
}

int OpenFileTable::Open(OpenFile *file, const char *from, const char *name){
	int index = map->Find();
	if (index >= 2)
	{
		table[index].inUse = true;
		strncpy(table[index].path, from, MAX_PATH_LENGTH);
		strncpy(table[index].name, name, FileNameMaxLen);
		table[index].file = file;
		table[index].count = 1;
		return index;
	}
	return -1;
}

int OpenFileTable::IsOpen(const char *from, const char *name){
	for (int i = 0; i < MAX_OPEN_FILE; i ++)
		if (table[i].inUse && 
			strcmp(from, table[i].path) == 0 && 
			strcmp(name, table[i].name) == 0)
		{
			table[i].count ++;
			return i;
		}
	return -1;
}

bool OpenFileTable::CanOpen(){
	return map->NumClear() > 0;
}

void OpenFileTable::Close(int index){
	if (index >= 2 && index < MAX_OPEN_FILE && table[index].inUse)
	{
		table[index].count --;
		if (table[index].count == 0)
		{
			map->Clear(index);
			table[index].inUse = false;
			delete table[index].file;
		}
	}
}

int OpenFileTable::FRead(char* buffer, int size, int fileId){
	if (fileId < 2)
		return 0;

	if (!table[fileId].inUse)
		return 0;
	OpenFile *file = table[fileId].file;
	return file->Read(buffer, size);
}

void OpenFileTable::FWrite(char* buffer, int size, int fileId){
	if (fileId < 2)
		return;

	if (table[fileId].inUse)
	{
		OpenFile *file = table[fileId].file;
		file->Write(buffer, size);
	}
}
#endif //CHANGED
