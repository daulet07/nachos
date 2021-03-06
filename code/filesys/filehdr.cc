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

#ifndef CHANGED
	bool
FileHeader::Allocate(BitMap *freeMap, int fileSize)
{ 
#ifdef CHANGED
	if (fileSize > (int)MaxFileSize)
	{
		fprintf(stderr, "File to big\n");
		return FALSE;
	}
#endif
	numBytes = fileSize;
	numSectors  = divRoundUp(fileSize, SectorSize);
	if (freeMap->NumClear() < numSectors)
		return FALSE;		// not enough space

	for (int i = 0; i < numSectors; i++)
		dataSectors[i] = freeMap->Find();
	return TRUE;
}
#else
	bool
FileHeader::Allocate(BitMap *freeMap, int fileSize)
{ 
	if (fileSize > (int)MaxBigFileSize)
	{
		fprintf(stderr, "File to big\n");
		return FALSE;
	}
	numBytes = fileSize;
	int numAllSectors = divRoundUp(fileSize, SectorSize);
	int numHdr = divRoundUp(numAllSectors, NumSecondDirect);

	if (freeMap->NumClear() < numAllSectors + numHdr)
		return FALSE;		// not enough space

	int i = numAllSectors;
	int tab[NumSecondDirect];
	int numSectLastHdr = 0;
	numSectors = 0;
	numBytes = fileSize;
	dataSectors[numSectors ++] = freeMap->Find();
	while (i > 0)
	{
		if (numSectLastHdr == NumSecondDirect)
		{
			synchDisk->WriteSector(dataSectors[numSectors-1], (char*)tab);
			dataSectors[numSectors ++] = freeMap->Find();
			numSectLastHdr = 0;
		}

		tab[numSectLastHdr++] = freeMap->Find();
		i --;
	}
	synchDisk->WriteSector(dataSectors[numSectors-1], (char*)tab);

	return TRUE;
}


FileHeader::FileHeader(){
	numBytes = 0;
	numSectors = 0;
	//numSectLastHdr = 0;
}

#ifndef CHANGED
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
#else

	bool
FileHeader::ReAllocate(BitMap *freeMap, int size)
{ 
	int totalNumSector = divRoundUp(numBytes + size, SectorSize);
	int totalNumSubHdr = divRoundUp(totalNumSector, NumSecondDirect);

	//int numSectorToAllocate = (totalNumSector + totalNumSubHdr) - ((numSectors-1)*NumSecondDirect + numSectLastHdr+ numSectors);
	int numSectorToAllocate = (totalNumSector + totalNumSubHdr) - (divRoundUp(numBytes, SectorSize) + numSectors);

	if (freeMap->NumClear() < numSectorToAllocate)
		return FALSE;		// not enough space

	int tab[NumSecondDirect];
	synchDisk->ReadSector(dataSectors[numSectors-1], (char*)tab);

	int numSectLastHdr = divRoundUp(numBytes, SectorSize) % NumSecondDirect;

	while (numSectorToAllocate > 0)
	{
		if (numSectLastHdr == (int)NumSecondDirect)
		{
			numSectors ++;
			numSectLastHdr = 0;
			synchDisk->WriteSector(dataSectors[numSectors-1], (char*)tab);
			dataSectors[numSectors++] = freeMap->Find();
		}
		tab[numSectLastHdr++] = freeMap->Find();
		numSectorToAllocate --;
	}
	synchDisk->WriteSector(dataSectors[numSectors-1], (char*)tab);
	numBytes += size;
	return TRUE;
}
#endif //CHANGED
#endif //CHANGED

//----------------------------------------------------------------------
// FileHeader::Deallocate
// 	De-allocate all the space allocated for data blocks for this file.
//
//	"freeMap" is the bit map of free disk sectors
//----------------------------------------------------------------------

#ifndef CHANGED
	void 
FileHeader::Deallocate(BitMap *freeMap)
{
	for (int i = 0; i < numSectors; i++) {
		ASSERT(freeMap->Test((int) dataSectors[i]));  // ought to be marked!
		freeMap->Clear((int) dataSectors[i]);
	}
}
#else
	void 
FileHeader::Deallocate(BitMap *freeMap)
{
	int tab[NumSecondDirect];
	synchDisk->ReadSector(dataSectors[numSectors-1], (char *)tab);

	int numSectLastHdr = divRoundUp(numBytes, SectorSize) % NumSecondDirect;

	while (numSectors != 1 || numSectLastHdr != 0)
	{
		if (numSectLastHdr <= 0)
		{
			numSectLastHdr = NumSecondDirect -1;
			freeMap->Clear((int)dataSectors[--numSectors]);
			synchDisk->ReadSector(dataSectors[numSectors-1], (char *)tab);
		}
		freeMap->Clear((int)tab[--numSectLastHdr]);
	}


	freeMap->Clear((int) dataSectors[0]);
}
#ifdef CHANGED
#endif //CHANGED
#endif //CHANGED

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

#ifndef CHANGED
	int
FileHeader::ByteToSector(int offset)
{
	return(dataSectors[offset / SectorSize]);
}
#else
	int
FileHeader::ByteToSector(int offset)
{
	int tab[NumSecondDirect];
	int numSect = offset/ SectorSize;
	int numHdr = numSect / NumSecondDirect;
	synchDisk->ReadSector(dataSectors[numHdr], (char*)tab); 


	int sector = tab[numSect - numHdr * NumSecondDirect];
	return sector;
}
#endif //CHANGED

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

