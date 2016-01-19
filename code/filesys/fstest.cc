// fstest.cc 
//	Simple test routines for the file system.  
//
//	We implement:
//	   Copy -- copy a file from UNIX to Nachos
//	   Print -- cat the contents of a Nachos file 
//	   Perftest -- a stress test for the Nachos file system
//		read and write a really large file in tiny chunks
//		(won't work on baseline system!)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"

#include "utility.h"
#include "filesys.h"
#include "system.h"
#include "thread.h"
#include "disk.h"
#include "stats.h"

#define TransferSize 	10 	// make it small, just to be difficult

//----------------------------------------------------------------------
// Copy
// 	Copy the contents of the UNIX file "from" to the Nachos file "to"
//----------------------------------------------------------------------

	void
Copy(const char *from, const char *to)
{
	FILE *fp;
	OpenFile* openFile;
	int amountRead, fileLength;
	char *buffer;

	// Open UNIX file
	if ((fp = fopen(from, "r")) == NULL) {	 
		printf("Copy: couldn't open input file %s\n", from);
		return;
	}

	// Figure out length of UNIX file
	fseek(fp, 0, 2);		
	fileLength = ftell(fp);
	fseek(fp, 0, 0);

	// Create a Nachos file of the same length
	DEBUG('f', "Copying file %s, size %d, to file %s\n", from, fileLength, to);
#ifndef CHANGED
	if (!fileSystem->Create(to, fileLength)) {	 // Create Nachos file
#else
	if (!fileSystem->CreateFile(to, fileLength)) {	 // Create Nachos file
#endif
		printf("Copy: couldn't create output file %s\n", to);
		fclose(fp);
		return;
	}

	openFile = fileSystem->Open(to);
	ASSERT(openFile != NULL);

	// Copy the data in TransferSize chunks
	buffer = new char[TransferSize];
	while ((amountRead = fread(buffer, sizeof(char), TransferSize, fp)) > 0)
		openFile->Write(buffer, amountRead);	
	delete [] buffer;

	// Close the UNIX and the Nachos files
	delete openFile;
	fclose(fp);
}

//----------------------------------------------------------------------
// Print
// 	Print the contents of the Nachos file "name".
//----------------------------------------------------------------------

	void
Print(char *name)
{
	OpenFile *openFile;    
	int i, amountRead;
	char *buffer;

	if ((openFile = fileSystem->Open(name)) == NULL) {
		printf("Print: unable to open file %s\n", name);
		return;
	}

	buffer = new char[TransferSize];
	while ((amountRead = openFile->Read(buffer, TransferSize)) > 0)
		for (i = 0; i < amountRead; i++)
			printf("%c", buffer[i]);
	delete [] buffer;

	delete openFile;		// close the Nachos file
	return;
}

//----------------------------------------------------------------------
// PerformanceTest
// 	Stress the Nachos file system by creating a large file, writing
//	it out a bit at a time, reading it back a bit at a time, and then
//	deleting the file.
//
//	Implemented as three separate routines:
//	  FileWrite -- write the file
//	  FileRead -- read the file
//	  PerformanceTest -- overall control, and print out performance #'s
//----------------------------------------------------------------------

#define FileName 	"TestFile"
#define Contents 	"1234567890"
#define ContentSize 	strlen(Contents)
#define FileSize 	((int)(ContentSize * 5000))

	static void 
FileWrite()
{
	OpenFile *openFile;    
	int i, numBytes;

	printf("Sequential write of %d byte file, in %zd byte chunks\n", 
			FileSize, ContentSize);
#ifndef CHANGED
	if (!fileSystem->Create(FileName, 0)) {
#else
	if (!fileSystem->CreateFile("/", FileName, 0)) {
#endif
		printf("Perf test: can't create %s\n", FileName);
		return;
	}
	openFile = fileSystem->Open(FileName);
	if (openFile == NULL) {
		printf("Perf test: unable to open %s\n", FileName);
		return;
	}
	for (i = 0; i < FileSize; i += ContentSize) {
		numBytes = openFile->Write(Contents, ContentSize);
		if (numBytes < 10) {
			printf("Perf test: unable to write %s\n", FileName);
			delete openFile;
			return;
		}
	}
	delete openFile;	// close file
}

	static void 
FileRead()
{
	OpenFile *openFile;    
	char *buffer = new char[ContentSize];
	int i, numBytes;

	printf("Sequential read of %d byte file, in %zd byte chunks\n", 
			FileSize, ContentSize);

	if ((openFile = fileSystem->Open(FileName)) == NULL) {
		printf("Perf test: unable to open file %s\n", FileName);
		delete [] buffer;
		return;
	}
	for (i = 0; i < FileSize; i += ContentSize) {
		numBytes = openFile->Read(buffer, ContentSize);
		if ((numBytes < 10) || strncmp(buffer, Contents, ContentSize)) {
			printf("Perf test: unable to read %s\n", FileName);
			delete openFile;
			delete [] buffer;
			return;
		}
	}
	delete [] buffer;
	delete openFile;	// close file
}

	void
PerformanceTest()
{
	printf("Starting file system performance test:\n");
	stats->Print();
	FileWrite();
	FileRead();
#ifndef CHANGED
	if (!fileSystem->Remove(FileName)) {
#else
	if (!fileSystem->RemoveFile(FileName)) {
#endif
		printf("Perf test: unable to remove %s\n", FileName);
		return;
	}
	stats->Print();
}

#ifdef CHANGED
#include "filesys.h"

void Shell(){
	Directory *currentDir = fileSystem->GetRoot();

	bool run = true;
	char *arg = new char[MAX_STRING_SIZE];
	while (run)
	{
		printf("<My Shell>: ");
		fflush(stdout);
		int read = synchConsole->SynchGetString(arg, MAX_STRING_SIZE);
		arg[read-1] = '\0';

		if (!strcmp(arg, "exit"))
			run = false;
		else if (!strcmp(arg, "list"))
			fileSystem->Print();
		else if (!strncmp(arg, "ls", 2))
		{
			char *name = strchr(arg, ' ');
			if (name == NULL)
				printf("ls <path>\n");
			else
			{
				name ++;
				char *end = strchr(name, ' ');
				if (end != NULL)
					printf("ls <path>\n");
				else
				{
					Directory *dir = fileSystem->GetDir(name);
					if (dir != NULL)
					{
						dir->List();
						delete dir;
					}
					else
						printf("%s not found\n", name);
				}
			}
		}
		else if (!strncmp(arg, "mkdir", 5))
		{
			char *from = strchr(arg, ' ');
			if (from == NULL)
				printf("mkdir <from> <file>\n");
			else
			{
				from ++;
				char *file = strchr(from, ' ');
				if (file == NULL)
					printf("mkdir <from> <file>\n");
				else
				{
					*file = '\0';
					file ++;

					char *end = strchr(file, ' ');
					if (end != NULL)
						printf("mkdir <from> <file>\n");
					else
					{
						if (!fileSystem->CreateDir(from, file))
							printf("Error\n");
					}
				}
			}
		}

	}

	printf("good by\n");

	delete []arg;
	delete currentDir;
}
#endif //CHANGED
