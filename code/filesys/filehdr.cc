// filehdr.cc 
//	Routines for managing the disk file header (in UNIX, this
//	would be called the i-node).
//
//	The file header is used to locate where on disk the 
//	file's data is stored.  We implement this as a fixed size
//	table of pointers -- each entry in the table points to the 
//	disk sector containing that portion of the file data
//	(in other words, there are no indirect or doubly indirect 
//	blocks). The table size is chosen so that the file header
//	will be just big enough to fit in one disk sector, 
//
//      Unlike in a real system, we do not keep track of file permissions, 
//	ownership, last modification date, etc., in the file header. 
//
//	A file header can be initialized in two ways:
//	   for a new file, by modifying the in-memory data structure
//	     to point to the newly allocated data blocks
//	   for a file already on disk, by reading the file header from disk
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"

#include "system.h"
#include "filehdr.h"

//----------------------------------------------------------------------
// FileHeader::Allocate
// 	Initialize a fresh file header for a newly created file.
//	Allocate data blocks for the file out of the map of free disk blocks.
//	Return FALSE if there are not enough free blocks to accomodate
//	the new file.
//
//	"freeMap" is the bit map of free disk sectors
//	"fileSize" is the bit map of free disk sectors
//----------------------------------------------------------------------

	bool
FileHeader::Allocate(BitMap *freeMap, int fileSize)
{ 
	numBytes = fileSize;
	numSectors  = divRoundUp(fileSize, SectorSize);
	if (freeMap->NumClear() < numSectors)
		return FALSE;		// not enough space

	for (int i = 0; i < numSectors; i++)
		dataSectors[i] = freeMap->Find();
	return TRUE;
}

#ifdef CHANGED
/*
	bool
FileHeader::Allocate(BitMap *freeMap, int fileSize)
{ 
	if (numBytes + fileSize <=  MaxFileSize) // Juste one indirection
	{
		int sectorToAllocate = divRoundUp(numBytes + fileSize, SectorSize) - numSectors;

		if (freeMap->NumClear() < sectorToAllocate)
			return FALSE;
		for (int i = 0; i < sectorToAllocate; i ++)
		{
			dataSectors[numSectors + i] = freeMap->Find();
			ASSERT(dataSectors[numSectors + i] != -1);
		}
		numBytes += fileSize;
		numSectors += sectorToAllocate;
	}
	else 
	{
		int newDataSectors[NumSecondDirect];

		if (numBytes <= MaxFileSize) // Need to change from one indirection to two
		{
			int totalSector = (numBytes + fileSize) / SectorSize + (numBytes + fileSize) / MaxFileSize;
			if (freeMap->NumClear() < totalSector - numSectors)
				return FALSE;

			for (int i = 0; i < numSectors; i ++)
				newDataSectors[i] = dataSectors[i];
			
			numSectors = 1;
			dataSectors[0] = freeMap->Find();
			ASSERT(dataSectors[0] != -1);
			synchDisk->WriteSector(dataSectors[0], (char*)newDataSectors);
		}
		else
			synchDisk->ReadSector(dataSectors[numSectors-1], (char*)newDataSectors);
		
		// Fill last secondDirect

		int canFill = MaxFileSize * numSectors - numBytes;
		///////////////////////////////////:
	}
	numBytes = fileSize;
	numSectors  = divRoundUp(fileSize, SectorSize);
	if (freeMap->NumClear() < numSectors)
		return FALSE;		// not enough space

	for (int i = 0; i < numSectors; i++)
		dataSectors[i] = freeMap->Find();
	return TRUE;
}
*/

FileHeader::FileHeader(){
	numBytes = 0;
	numSectors = 0;
}

	bool
FileHeader::ReAllocate(BitMap *freeMap, int size)
{ 
	int numSectorToAllocate = divRoundUp(numBytes + size, SectorSize) - numSectors;
	if (freeMap->NumClear() < numSectorToAllocate)
		return FALSE;		// not enough space

	for (int i = 0; i < numSectorToAllocate; i++)
		dataSectors[numSectors + i] = freeMap->Find();

	numSectors += numSectorToAllocate;
	numBytes += size;
	
	return TRUE;
}
#endif

//----------------------------------------------------------------------
// FileHeader::Deallocate
// 	De-allocate all the space allocated for data blocks for this file.
//
//	"freeMap" is the bit map of free disk sectors
//----------------------------------------------------------------------

	void 
FileHeader::Deallocate(BitMap *freeMap)
{
	for (int i = 0; i < numSectors; i++) {
		ASSERT(freeMap->Test((int) dataSectors[i]));  // ought to be marked!
		freeMap->Clear((int) dataSectors[i]);
	}
}

//----------------------------------------------------------------------
// FileHeader::FetchFrom
// 	Fetch contents of file header from disk. 
//
//	"sector" is the disk sector containing the file header
//----------------------------------------------------------------------

	void
FileHeader::FetchFrom(int sector)
{
	synchDisk->ReadSector(sector, (char *)this);
}

//----------------------------------------------------------------------
// FileHeader::WriteBack
// 	Write the modified contents of the file header back to disk. 
//
//	"sector" is the disk sector to contain the file header
//----------------------------------------------------------------------

	void
FileHeader::WriteBack(int sector)
{
	synchDisk->WriteSector(sector, (char *)this); 
}

//----------------------------------------------------------------------
// FileHeader::ByteToSector
// 	Return which disk sector is storing a particular byte within the file.
//      This is essentially a translation from a virtual address (the
//	offset in the file) to a physical address (the sector where the
//	data at the offset is stored).
//
//	"offset" is the location within the file of the byte in question
//----------------------------------------------------------------------

	int
FileHeader::ByteToSector(int offset)
{
	return(dataSectors[offset / SectorSize]);
}

//----------------------------------------------------------------------
// FileHeader::FileLength
// 	Return the number of bytes in the file.
//----------------------------------------------------------------------

	int
FileHeader::FileLength()
{
	return numBytes;
}

//----------------------------------------------------------------------
// FileHeader::Print
// 	Print the contents of the file header, and the contents of all
//	the data blocks pointed to by the file header.
//----------------------------------------------------------------------

	void
FileHeader::Print()
{
	int i, j, k;
	char *data = new char[SectorSize];

	printf("FileHeader contents.  File size: %d.  File blocks:\n", numBytes);
	for (i = 0; i < numSectors; i++)
		printf("%d ", dataSectors[i]);
	printf("\nFile contents:\n");
	for (i = k = 0; i < numSectors; i++) {
		synchDisk->ReadSector(dataSectors[i], data);
		for (j = 0; (j < SectorSize) && (k < numBytes); j++, k++) {
			if ('\040' <= data[j] && data[j] <= '\176')   // isprint(data[j])
				printf("%c", data[j]);
			else
				printf("\\%x", (unsigned char)data[j]);
		}
		printf("\n"); 
	}
	delete [] data;
}

#ifdef CHANGED
int FileHeader::FileAllocatedLength(){
	return numSectors * SectorSize;
}

void FileHeader::SetFileLength(int size){
	numBytes = size;
}
#endif
