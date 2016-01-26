// filesys.cc 
//	Routines to manage the overall operation of the file system.
//	Implements routines to map from textual file names to files.
//
//	Each file in the file system has:
//	   A file header, stored in a sector on disk 
//		(the size of the file header data structure is arranged
//		to be precisely the size of 1 disk sector)
//	   A number of data blocks
//	   An entry in the file system directory
//
// 	The file system consists of several data structures:
//	   A bitmap of free disk sectors (cf. bitmap.h)
//	   A directory of file names and file headers
//
//      Both the bitmap and the directory are represented as normal
//	files.  Their file headers are located in specific sectors
//	(sector 0 and sector 1), so that the file system can find them 
//	on bootup.
//
//	The file system assumes that the bitmap and directory files are
//	kept "open" continuously while Nachos is running.
//
//	For those operations (such as Create, Remove) that modify the
//	directory and/or bitmap, if the operation succeeds, the changes
//	are written immediately back to disk (the two files are kept
//	open during all this time).  If the operation fails, and we have
//	modified part of the directory and/or bitmap, we simply discard
//	the changed version, without writing it back to disk.
//
// 	Our implementation at this point has the following restrictions:
//
//	   there is no synchronization for concurrent accesses
//	   files have a fixed size, set when the file is created
//	   files cannot be bigger than about 3KB in size
//	   there is no hierarchical directory structure, and only a limited
//	     number of files can be added to the system
//	   there is no attempt to make the system robust to failures
//	    (if Nachos exits in the middle of an operation that modifies
//	    the file system, it may corrupt the disk)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"

#include "disk.h"
#include "bitmap.h"
#include "directory.h"
#include "filehdr.h"
#include "filesys.h"

#ifdef CHANGED
#include "openfiletable.h"
#include "system.h"
#endif

// Sectors containing the file headers for the bitmap of free sectors,
// and the directory of files.  These file headers are placed in well-known 
// sectors, so that they can be located on boot-up.
#define FreeMapSector 		0
#define DirectorySector 	1

// Initial file sizes for the bitmap and directory; until the file system
// supports extensible files, the directory size sets the maximum number 
// of files that can be loaded onto the disk.
#define FreeMapFileSize 	(NumSectors / BitsInByte)
#define NumDirEntries 		10
#define DirectoryFileSize 	(sizeof(DirectoryEntry) * NumDirEntries)


//----------------------------------------------------------------------
// FileSystem::FileSystem
// 	Initialize the file system.  If format = TRUE, the disk has
//	nothing on it, and we need to initialize the disk to contain
//	an empty directory, and a bitmap of free sectors (with almost but
//	not all of the sectors marked as free).  
//
//	If format = FALSE, we just have to open the files
//	representing the bitmap and the directory.
//
//	"format" -- should we initialize the disk?
//----------------------------------------------------------------------

FileSystem::FileSystem(bool format)
{ 
	DEBUG('f', "Initializing the file system.\n");
#ifdef CHANGED
	kernelFTable = new OpenFileTable();
#endif
	if (format) {
		BitMap *freeMap = new BitMap(NumSectors);
		Directory *directory = new Directory(NumDirEntries);
		FileHeader *mapHdr = new FileHeader;
		FileHeader *dirHdr = new FileHeader;

		DEBUG('f', "Formatting the file system.\n");

		// First, allocate space for FileHeaders for the directory and bitmap
		// (make sure no one else grabs these!)
		freeMap->Mark(FreeMapSector);	    
		freeMap->Mark(DirectorySector);

		// Second, allocate space for the data blocks containing the contents
		// of the directory and bitmap files.  There better be enough space!

		ASSERT(mapHdr->Allocate(freeMap, FreeMapFileSize));
		ASSERT(dirHdr->Allocate(freeMap, DirectoryFileSize));

		// Flush the bitmap and directory FileHeaders back to disk
		// We need to do this before we can "Open" the file, since open
		// reads the file header off of disk (and currently the disk has garbage
		// on it!).

		DEBUG('f', "Writing headers back to disk.\n");
		mapHdr->WriteBack(FreeMapSector);    
		dirHdr->WriteBack(DirectorySector);

		// OK to open the bitmap and directory files now
		// The file system operations assume these two files are left open
		// while Nachos is running.

		freeMapFile = new OpenFile(FreeMapSector);
		directoryFile = new OpenFile(DirectorySector);

		// Once we have the files "open", we can write the initial version
		// of each file back to disk.  The directory at this point is completely
		// empty; but the bitmap has been changed to reflect the fact that
		// sectors on the disk have been allocated for the file headers and
		// to hold the file data for the directory and bitmap.

		DEBUG('f', "Writing bitmap and directory back to disk.\n");
		freeMap->WriteBack(freeMapFile);	 // flush changes to disk
		directory->WriteBack(directoryFile);

		if (DebugIsEnabled('f')) {
			freeMap->Print();
			directory->Print();

			delete freeMap; 
			delete directory; 
			delete mapHdr; 
			delete dirHdr;
		}
	} else {
		// if we are not formatting the disk, just open the files representing
		// the bitmap and directory; these are left open while Nachos is running
		freeMapFile = new OpenFile(FreeMapSector);
		directoryFile = new OpenFile(DirectorySector);
	}

}

#ifdef CHANGED
FileSystem::~FileSystem()
{
	delete kernelFTable;
}
#endif

//----------------------------------------------------------------------
// FileSystem::Create
// 	Create a file in the Nachos file system (similar to UNIX create).
//	Since we can't increase the size of files dynamically, we have
//	to give Create the initial size of the file.
//
//	The steps to create a file are:
//	  Make sure the file doesn't already exist
//        Allocate a sector for the file header
// 	  Allocate space on disk for the data blocks for the file
//	  Add the name to the directory
//	  Store the new file header on disk 
//	  Flush the changes to the bitmap and the directory back to disk
//
//	Return TRUE if everything goes ok, otherwise, return FALSE.
//
// 	Create fails if:
//   		file is already in directory
//	 	no free space for file header
//	 	no free entry for file in directory
//	 	no free space for data blocks for the file 
//
// 	Note that this implementation assumes there is no concurrent access
//	to the file system!
//
//	"name" -- name of file to be created
//	"initialSize" -- size of file to be created
//----------------------------------------------------------------------
#ifndef CHANGED
	bool
FileSystem::Create(const char *name, int initialSize)
{
	Directory *directory;
	BitMap *freeMap;
	FileHeader *hdr;
	int sector;
	bool success;

	DEBUG('f', "Creating file %s, size %d\n", name, initialSize);

	directory = new Directory(NumDirEntries);
	directory->FetchFrom(directoryFile);

	if (directory->Find(name) != -1)
		success = FALSE;			// file is already in directory
	else {	
		freeMap = new BitMap(NumSectors);
		freeMap->FetchFrom(freeMapFile);
		sector = freeMap->Find();	// find a sector to hold the file header
		if (sector == -1) 		
			success = FALSE;		// no free block for file header 
		else if (!directory->Add(name, sector))
			success = FALSE;	// no space in directory
		else {
			hdr = new FileHeader;
			if (!hdr->Allocate(freeMap, initialSize))
				success = FALSE;	// no space on disk for data
			else {	
				success = TRUE;
				// everthing worked, flush all changes back to disk
				hdr->WriteBack(sector); 		
				directory->WriteBack(directoryFile);
				freeMap->WriteBack(freeMapFile);
			}
			delete hdr;
		}
		delete freeMap;
	}
	delete directory;
	return success;
}

#else
	bool 
FileSystem::CreateFile(const char *path, int initialSize)
{
	if (*path != '/')
		return false;

	char from[MAX_PATH_LENGTH];
	strcpy(from, path);
	char *name = NULL;
	parsePath(from, &name);
	if (*name == '\0')
		return false;

	if (*from == '\0')
		return CreateFile("/", name, initialSize);
	else
		return CreateFile(from, name, initialSize);
}

	bool 
FileSystem::CreateFile(const char *from, const char *name, int initialSize)
{
	Directory *directory;
	BitMap *freeMap;
	FileHeader *hdr;
	int sector;
	bool success;

	DEBUG('f', "Creating file %s, size %d\n", name, initialSize);

	directory = SearchDir(from);
	if (directory == NULL)
		return FALSE;
	else
	{
		if (directory->Find(name) != -1)
			success = FALSE;			// file is already in directory
		else {	
			freeMap = new BitMap(NumSectors);
			freeMap->FetchFrom(freeMapFile);
			sector = freeMap->Find();	// find a sector to hold the file header
			if (sector == -1) 		
				success = FALSE;		// no free block for file header 
			else if (!directory->AddFile(name, sector))
				success = FALSE;	// no space in directory
			else {
				hdr = new FileHeader;
				if (!hdr->Allocate(freeMap, initialSize))
					success = FALSE;	// no space on disk for data
				else {	
					success = TRUE;
					// everthing worked, flush all changes back to disk
					hdr->WriteBack(sector); 		
					//directory->WriteBack(directoryFile);
					directory->Flush();
					freeMap->WriteBack(freeMapFile);
				}
				delete hdr;
			}
			delete freeMap;
		}
		delete directory;
	}
	return success;
}

bool
FileSystem::CreateDir(const char *path)
{
	if (*path != '/')
		return false;

	char from[MAX_PATH_LENGTH];
	strcpy(from, path);
	char *name = NULL;
	parsePath(from, &name);
	if (*name == '\0')
		return false;

	if (*from == '\0')
		return CreateDir("/", name);
	else
		return CreateDir(from, name);
}

bool
FileSystem::CreateDir(const char *from, const char *name)
{
	Directory *fromDir;
	BitMap *freeMap;
	FileHeader *hdr;
	int sector;
	bool success;

	DEBUG('f', "Creating directory %s, in %s\n", name, from);

	fromDir = SearchDir(from);
	if (fromDir == NULL)
		success = FALSE;
	else
	{
		if (fromDir->Find(name) != -1)
			success = FALSE;			// file is already in directory
		else {	
			freeMap = new BitMap(NumSectors);
			freeMap->FetchFrom(freeMapFile);
			sector = freeMap->Find();	// find a sector to hold the file header
			if (sector == -1) 		
				success = FALSE;		// no free block for file header 
			else if (!fromDir->AddDir(name, sector))
				success = FALSE;	// no space in directory
			else {
				hdr = new FileHeader;
				if (!hdr->Allocate(freeMap, DirectoryFileSize))
					success = FALSE;	// no space on disk for data
				else {	
					success = TRUE;
					// everthing worked, flush all changes back to disk
					hdr->WriteBack(sector); 		
					//fromDir->WriteBack(directoryFile);
					Directory *newDir = new Directory(NumDirEntries);
					newDir->SetMySector(sector);
					newDir->SetParentSector(fromDir->GetSector());
					OpenFile *file = new OpenFile(sector);
					newDir->WriteBack(file);
					delete file;
					delete newDir;

					fromDir->Flush();
					freeMap->WriteBack(freeMapFile);
				}
				delete hdr;
			}
		}
		delete freeMap;
		delete fromDir;
	}

	return success;
}

Directory *FileSystem::GetDir(const char *name){
	Directory *dir = SearchDir(name);

	return dir;
}
#endif //CHANGED
//----------------------------------------------------------------------
// FileSystem::Open
// 	Open a file for reading and writing.  
//	To open a file:
//	  Find the location of the file's header, using the directory 
//	  Bring the header into memory
//
//	"name" -- the text name of the file to be opened
//----------------------------------------------------------------------

#ifndef CHANGED
	OpenFile *
FileSystem::Open(const char *name)
{ 
	Directory *directory = new Directory(NumDirEntries);
	OpenFile *openFile = NULL;
	int sector;

	DEBUG('f', "Opening file %s\n", name);
	directory->FetchFrom(directoryFile);
	sector = directory->Find(name); 
	if (sector >= 0) 		
		openFile = new OpenFile(sector);	// name was found in directory 
	delete directory;
	return openFile;				// return NULL if not found
}

#else

	int 
FileSystem::FOpen(const char *path)
{ 
	if (*path != '/')
		return -1;

	char from[MAX_PATH_LENGTH];
	strcpy(from, path);
	char *name = NULL;
	parsePath(from, &name);
	if (*name == '\0')
		return -1;

	if (*from == '\0')
		return FOpen("/", name);
	else
		return FOpen(from, name);
}


	int
FileSystem::FOpen(const char *from, const char *name)
{ 
	int openFileId = kernelFTable->Open(from, name);
	return openFileId;
	/*
	int openFileId = kernelFTable->IsOpen(from, name);
	if (openFileId == -1)
	{
		if (!kernelFTable->CanOpen())
			return -1;

		OpenFile *file = Open(from, name);
		if (file == NULL)
			return -1;

		openFileId = kernelFTable->Open(file, from, name);
		if (openFileId != -1)
		{
			kernelFTable->AppendProcess(openFileId);
			openFileId = currentThread->space->addOpenFile(openFileId);
		}
	}
	else
	{
		if (kernelFTable->GetProcessEntry(openFileId) == NULL)
		{
			kernelFTable->AppendProcess(openFileId);
			openFileId = currentThread->space->addOpenFile(openFileId);
		}
		else
			openFileId = currentThread->space->getOpenFileId(openFileId);
	}

	return openFileId;				// return NULL if not found
	*/
}

	OpenFile * 
FileSystem::Open(const char *path)
{ 
	if (*path != '/')
		return NULL;

	char from[MAX_PATH_LENGTH];
	strcpy(from, path);
	char *name = NULL;
	parsePath(from, &name);
	if (*name == '\0')
		return NULL;

	if (*from == '\0')
		return Open("/", name);
	else
		return Open(from, name);
}


	OpenFile *
FileSystem::Open(const char *from, const char *name)
{ 
	OpenFile *openFile = NULL;

	Directory *directory = new Directory(NumDirEntries);
	int sector;

	DEBUG('f', "Opening file %s%s\n", from, name);
	directory = SearchDir(from);
	if (directory != NULL)
	{
		sector = directory->Find(name); 
		if (sector >= 0) 		
			openFile = new OpenFile(sector);	// name was found in directory 
		delete directory;
	}
	return openFile;				// return NULL if not found
}

#endif //CHANGED

//----------------------------------------------------------------------
// FileSystem::Remove
// 	Delete a file from the file system.  This requires:
//	    Remove it from the directory
//	    Delete the space for its header
//	    Delete the space for its data blocks
//	    Write changes to directory, bitmap back to disk
//
//	Return TRUE if the file was deleted, FALSE if the file wasn't
//	in the file system.
//
//	"name" -- the text name of the file to be removed
//----------------------------------------------------------------------

#ifndef CHANGED
	bool
FileSystem::Remove(const char *name)
{ 
	Directory *directory;
	BitMap *freeMap;
	FileHeader *fileHdr;
	int sector;

	directory = new Directory(NumDirEntries);
	directory->FetchFrom(directoryFile);
	sector = directory->Find(name);
	if (sector == -1) {
		delete directory;
		return FALSE;			 // file not found 
	}
	fileHdr = new FileHeader;
	fileHdr->FetchFrom(sector);

	freeMap = new BitMap(NumSectors);
	freeMap->FetchFrom(freeMapFile);

	fileHdr->Deallocate(freeMap);  		// remove data blocks
	freeMap->Clear(sector);			// remove header block
	directory->Remove(name);

	freeMap->WriteBack(freeMapFile);		// flush to disk
	directory->WriteBack(directoryFile);        // flush to disk
	delete fileHdr;
	delete directory;
	delete freeMap;
	return TRUE;
} 

#else
	bool
FileSystem::RemoveFile(const char *path)
{
	if (*path != '/')
		return NULL;

	char from[MAX_PATH_LENGTH];
	strcpy(from, path);
	char *name = NULL;
	parsePath(from, &name);
	if (*name == '\0')
		return NULL;

	if (*from == '\0')
		return RemoveFile("/", name);
	else
		return RemoveFile(from, name);
}

	bool
FileSystem::RemoveFile(const char *from, const char *name)
{ 
	Directory *directory;
	BitMap *freeMap;
	FileHeader *fileHdr;
	int sector;

	directory = SearchDir(from);
	sector = directory->Find(name);
	if (sector == -1) {
		delete directory;
		return FALSE;			 // file not found 
	}
	fileHdr = new FileHeader;
	fileHdr->FetchFrom(sector);

	freeMap = new BitMap(NumSectors);
	freeMap->FetchFrom(freeMapFile);

	fileHdr->Deallocate(freeMap);  		// remove data blocks
	freeMap->Clear(sector);			// remove header block
	directory->Remove(name);

	freeMap->WriteBack(freeMapFile);		// flush to disk
	directory->Flush();        // flush to disk
	delete fileHdr;
	delete directory;
	delete freeMap;
	return TRUE;
} 
#endif //CHANGED

//----------------------------------------------------------------------
// FileSystem::List
// 	List all the files in the file system directory.
//----------------------------------------------------------------------

	void
FileSystem::List()
{
	Directory *directory = new Directory(NumDirEntries);

	directory->FetchFrom(directoryFile);
	directory->List();
	delete directory;
}

//----------------------------------------------------------------------
// FileSystem::Print
// 	Print everything about the file system:
//	  the contents of the bitmap
//	  the contents of the directory
//	  for each file in the directory,
//	      the contents of the file header
//	      the data in the file
//----------------------------------------------------------------------

	void
FileSystem::Print()
{
	FileHeader *bitHdr = new FileHeader;
	FileHeader *dirHdr = new FileHeader;
	BitMap *freeMap = new BitMap(NumSectors);
	Directory *directory = new Directory(NumDirEntries);

	printf("Bit map file header:\n");
	bitHdr->FetchFrom(FreeMapSector);
	bitHdr->Print();

	printf("Directory file header:\n");
	dirHdr->FetchFrom(DirectorySector);
	dirHdr->Print();

	freeMap->FetchFrom(freeMapFile);
	freeMap->Print();

	directory->FetchFrom(directoryFile);
	directory->Print();

	delete bitHdr;
	delete dirHdr;
	delete freeMap;
	delete directory;
} 

#ifdef CHANGED

Directory *FileSystem::GetRoot(){
	Directory *directory = new Directory(NumDirEntries);
	directory->FetchFrom(directoryFile);
	return directory;
}
#include <string.h>

Directory *FileSystem::SearchDir(const char *name){
	Directory *dir;
	int position;
	char fullPath[MAX_PATH_LENGTH];
	char *path = fullPath;
	strncpy(fullPath, name, MAX_PATH_LENGTH);

	ASSERT(fullPath[MAX_PATH_LENGTH-1] == '\0');

	if (*path != '/')
		return NULL;
	path ++;

	dir = GetRoot();

	while (*path != '\0')
	{
		char *temp = path;
		while (*temp != '/' && *temp != '\0')
			temp ++;

		if (*temp == '/')
		{
			*(temp) = '\0';
			position = dir->FindIndex(path);
			path = temp+1;
		}
		else
		{
			position = dir->FindIndex(path);
			*path = '\0';
		}

		if (position < 0 || !dir->IsDir(position))
		{
			delete dir;
			return NULL;
		}

		int sector = dir->Find(position);
		OpenFile *file = new OpenFile(sector);

		dir->FetchFrom(file);
		delete file;
	}

	return dir;
}

void FileSystem::parsePath(char *path, char** name){
	*name = path;
	while(**name != '\0')
		(*name) ++;
	
	while(**name != '/')
		(*name) --;

	**name = '\0';
	(*name) ++;
}

void FileSystem::FClose(int index){
	int fileId = currentThread->space->GetOpenFileId(index);
	currentThread->space->CloseOpenFile(index);
	kernelFTable->Close(fileId);
}

int FileSystem::FRead(char* buffer, int size, int fileId){
	fileId = currentThread->space->GetOpenFileId(fileId);
	return kernelFTable->FRead(buffer, size, fileId);
}

void FileSystem::FWrite(char* buffer, int size, int fileId){
	fileId = currentThread->space->GetOpenFileId(fileId);
	kernelFTable->FWrite(buffer, size, fileId);
}

bool FileSystem::ReAllocate(FileHeader* hdr, int numByte){
	BitMap *freeMap = new BitMap(NumSectors);
	freeMap->FetchFrom(freeMapFile);
	int result = hdr->ReAllocate(freeMap, numByte);
	freeMap->WriteBack(freeMapFile);

	return result;
}
	bool
FileSystem::RemoveDir(const char *path)
{
	if (*path != '/')
		return NULL;

	char from[MAX_PATH_LENGTH];
	strcpy(from, path);
	char *name = NULL;
	parsePath(from, &name);
	if (*name == '\0')
		return NULL;

	if (*from == '\0')
		return RemoveDir("/", name);
	else
		return RemoveDir(from, name);
}

	bool
FileSystem::RemoveDir(const char *from, const char *name)
{ 
	Directory *directory, *parentDir;
	BitMap *freeMap;
	OpenFile *file;
	int sector;

	parentDir = SearchDir(from);
	sector = parentDir->Find(name);
	if (sector == -1 || !parentDir->IsDir(parentDir->FindIndex(name))) {
		delete parentDir;
		return FALSE;			 // file not found 
	}
	file = new OpenFile(sector);

	directory = new Directory(NumDirEntries);
	directory->FetchFrom(file);
	delete file;

	if (!directory->IsEmpty())
	{
		delete parentDir;
		delete directory;
		return FALSE;
	}

	freeMap = new BitMap(NumSectors);
	freeMap->FetchFrom(freeMapFile);

	freeMap->Clear(sector);			// remove header block
	parentDir->Remove(name);

	freeMap->WriteBack(freeMapFile);		// flush to disk
	parentDir->Flush();
	delete directory;
	delete parentDir;
	delete freeMap;
	return TRUE;
} 

#endif //CHANGED
