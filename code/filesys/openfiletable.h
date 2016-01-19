#ifdef CHANGED
#ifndef OFT_H
#define OFT_H

//#include "system.h"
#include "directory.h"
#include "bitmap.h"
#define MAX_PATH_LENGTH 100

#define MAX_OPEN_FILE 10

typedef struct{
	bool inUse;
	char path[MAX_PATH_LENGTH];
	char name[FileNameMaxLen];
	OpenFile *file;
	int count;
}FileTableEntry;

class OpenFileTable{
	public:
		OpenFileTable();
		~OpenFileTable();
		int Open(OpenFile *file, const char *from, const char *name);
		void Close(int index);
		int IsOpen(const char *from, const char *name);
		bool CanOpen();

		int FRead(char* buffer, int size, int fileId);
		void FWrite(char* buffer, int size, int fileId);

	private:
		FileTableEntry *table;
		BitMap *map;
};

#endif //OFT_H
#endif //CHANGED
