// filesys.h
//	Data structures to represent the Nachos file system.
//
//	A file system is a set of files stored on disk, organized
//	into directories.  Operations on the file system have to
//	do with "naming" -- creating, opening, and deleting files,
//	given a textual file name.  Operations on an individual
//	"open" file (read, write, close) are to be found in the OpenFile
//	class (openfile.h).
//
//	We define two separate implementations of the file system.
//	The "STUB" version just re-defines the Nachos file system
//	operations as operations on the native UNIX file system on the machine
//	running the Nachos simulation.  This is provided in case the
//	multiprogramming and virtual memory assignments (which make use
//	of the file system) are done before the file system assignment.
//
//	The other version is a "real" file system, built on top of
//	a disk simulator.  The disk is simulated using the native UNIX
//	file system (in a file named "DISK").
//
//	In the "real" implementation, there are two key data structures used
//	in the file system.  There is a single "root" directory, listing
//	all of the files in the file system; unlike UNIX, the baseline
//	system does not provide a hierarchical directory structure.
//	In addition, there is a bitmap for allocating
//	disk sectors.  Both the root directory and the bitmap are themselves
//	stored as files in the Nachos file system -- this causes an interesting
//	bootstrap problem when the simulated disk is initialized.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#ifndef FS_H
#define FS_H

#include "copyright.h"
#include "openfile.h"
#include "disk.h"
#include "bitmap.h"
#include "directory.h"
#include "filehdr.h"

#define FreeMapSector 0
#define DirectorySector 1

#define FreeMapFileSize (NumSectors / BitsInByte)
#define NumDirEntries 10
#define DirectoryFileSize (sizeof(DirectoryEntry) * NumDirEntries)

#ifdef FILESYS_STUB // Temporarily implement file system calls as
// calls to UNIX, until the real file system
// implementation is available

class FileSystem
{
public:
	OpenFile **_openFile; // FileSystem is a table containing 10 OpenFile
	int pos;			  // Each OpenFile has an index; dont forget openfile[0]: stdin,
						  // openfile[1]: stdout
	FileSystem(bool format)
	{
		_openFile = new OpenFile *[10];
		pos = 0;
		for (int i = 0; i < 10; i++)
		{
			_openFile[i] = NULL;
		}
		this->Create("stdin", 0);
		this->Create("stdout", 0);

		OpenFile *temp = this->Open("stdin", 0); // index = 1
		_openFile[0] = temp;					 // index = 1
		_openFile[0]->type = 1;
		temp = this->Open("stdout", 0);
		_openFile[1] = temp; // index = 2
		_openFile[1]->type = 0;
		this->pos = 1;
		delete temp;
	}

	~FileSystem()
	{
		for (int i = 0; i < 10; i++)
		{
			if (_openFile[i] != NULL)
				delete _openFile[i];
		}
		delete[] _openFile;
	}

	bool Create(char *name, int initialSize)
	{
		int fileDescriptor = OpenForWrite(name);

		if (fileDescriptor == -1)
			return FALSE;
		Close(fileDescriptor);
		return TRUE;
	}

	OpenFile *Open(char *name)
	{
		int fileDescriptor = OpenForReadWrite(name, FALSE);

		if (fileDescriptor == -1)
		{
			return NULL;
		}
		pos += 1; // open one file and index inscrease
		return new OpenFile(fileDescriptor);
	}

	OpenFile *Open(char *name, int type)
	{
		int fileDescriptor = OpenForReadWrite(name, FALSE);

		if (fileDescriptor == -1)
		{
			return NULL;
		}
		pos += 1; // open one file and index inscrease
		return new OpenFile(fileDescriptor, type);
	}

	bool Remove(char *name) { return Unlink(name) == 0; }
};

#else // FILESYS
class FileSystem
{
public:
	OpenFile **_openFile;
	int pos;
	FileSystem(bool format); // Initialize the file system.
							 // Must be called *after* "synchDisk"
							 // has been initialized.
							 // If "format", there is nothing on
							 // the disk, so initialize the directory
							 // and the bitmap of free blocks.

	bool Create(char *name, int initialSize);
	// Create a file (UNIX creat)

	OpenFile *Open(char *name); // Open a file (UNIX open)
	OpenFile *Open(char *name, int type);
	bool Remove(char *name); // Delete a file (UNIX unlink)

	void List(); // List all the files in the file system

	void Print(); // List all the files and their contents

private:
	OpenFile *freeMapFile;	 // Bit map of free disk blocks,
							 // represented as a file
	OpenFile *directoryFile; // "Root" directory -- list of
							 // file names, represented as a file
};

#endif // FILESYS

#endif // FS_H
