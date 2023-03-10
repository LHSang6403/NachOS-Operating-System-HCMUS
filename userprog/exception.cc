// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#define MaxFileLength 32
#define MaxLength 255

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions
//	are in machine.h.
//----------------------------------------------------------------------

void inc_program_counter()
{
	/* set previous programm counter (debugging only)
     * similar to: registers[PrevPCReg] = registers[PCReg];*/
	machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));

	/* set programm counter to next instruction
     * similar to: registers[PCReg] = registers[NextPCReg]*/
	machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));

	/* set next programm counter for brach execution
     * similar to: registers[NextPCReg] = pcAfter;*/
	machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg) + 4);
}

char *User2System(int virtAddr, int limit)
{
	int i; // index
	int oneChar;
	char *kernelBuf = NULL;
	kernelBuf = new char[limit + 1]; //need for terminal string
	if (kernelBuf == NULL)
		return kernelBuf;
	memset(kernelBuf, 0, limit + 1);
	//printf("\n Filename u2s:");
	for (i = 0; i < limit; i++)
	{
		machine->ReadMem(virtAddr + i, 1, &oneChar);
		kernelBuf[i] = (char)oneChar;
		//printf("%c",kernelBuf[i]);
		if (oneChar == 0)
			break;
	}
	return kernelBuf;
}
//Ng?????c l???i ta c?? h??m System2User nh?? sau:

// Input: - User space address (int)
// - Limit of buffer (int)
// - Buffer (char[])
// Output:- Number of bytes copied (int)
// Purpose: Copy buffer from System memory space to User memory space
int System2User(int virtAddr, int len, char *buffer)
{
	if (len < 0)
		return -1;
	if (len == 0)
		return len;
	int i = 0;
	int oneChar = 0;
	do
	{
		oneChar = (int)buffer[i];
		machine->WriteMem(virtAddr + i, 1, oneChar);
		i++;
	} while (i < len && oneChar != 0);
	return i;
}
void ExceptionHandler(ExceptionType which)
{
	int type = machine->ReadRegister(2);

	switch (which)
	{
	case NoException:
		return;

	case SyscallException:
		switch (type)
		{
		case SC_Halt:
			DEBUG('a', "\n Shutdown, initiated by user program.");
			printf("\n\n Shutdown, initiated by user program.");
			interrupt->Halt();
			break;
		case SC_Print:
		{
			int virtAddr = machine->ReadRegister(4);
			char *temp = new char[MaxLength];
			//memset(temp, 0, MaxLength);
			temp = User2System(virtAddr, MaxLength);
			int i = 0;
			while (temp[i] != 0)
			{
				synchConsole->Write(temp + i, 1);
				i++;
			}
			temp[i] = '\n';
			synchConsole->Write(temp + i, 1);
			delete[] temp;
			break;
		}
		case SC_Create:
		{
			int virtAddr;
			char *filename;
			DEBUG('a', "\n SC_Create call ...");
			DEBUG('a', "\n Reading virtual address of filename");
			// L???y tham s??? t??n t???p tin t??? thanh ghi r4
			virtAddr = machine->ReadRegister(4);
			DEBUG('a', "\n Reading filename.");
			// MaxFileLength l?? = 32
			filename = User2System(virtAddr, MaxFileLength + 1);
			if (filename == NULL)
			{
				printf("\n Not enough memory in system");
				DEBUG('a', "\n Not enough memory in system");
				machine->WriteRegister(2, -1);
				// tr??? v??? l???i cho ch????ng tr??nh ng?????i d??ng
				delete filename;
				return;
			}
			DEBUG('a', "\n Finish reading filename.");
			//DEBUG('a',"\n File name : '"<<filename<<"'");
			// Create file with size = 0
			// D??ng ?????i t?????ng fileSystem c???a l???p OpenFile ????? t???o file,
			// vi???c t???o file n??y l?? s??? d???ng c??c th??? t???c t???o file c???a h??? ??i???u
			// h??nh Linux, ch??ng ta kh??ng qu???n ly tr???c ti???p c??c block tr??n
			// ????a c???ng c???p ph??t cho file, vi???c qu???n ly c??c block c???a file
			// tr??n ??? ????a l?? m???t ????? ??n kh??c
			if (!fileSystem->Create(filename, 0))
			{
				printf("\n Error create file '%s'", filename);
				machine->WriteRegister(2, -1);
				delete filename;
				return;
			}
			machine->WriteRegister(2, 0);
			// tr??? v??? cho ch????ng tr??nh ng?????i d??ng th??nh c??ng
			delete filename;
			break;
		}
		case SC_Scanf:
		{
			char *buf = new char[32];
			if (buf == 0)
				break;
			int bufAddrUser = machine->ReadRegister(4);
			int length = machine->ReadRegister(5);
			int sz = synchConsole->Read(buf, length);
			System2User(bufAddrUser, sz, buf);
			delete[] buf;
		}
		default:
			printf("\n Unexpected user mode exception (%d %d)", which, type);
			interrupt->Halt();
		}
		inc_program_counter();
	}
}
