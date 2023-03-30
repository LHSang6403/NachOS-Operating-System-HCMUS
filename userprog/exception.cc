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
#include "SCHandle.h"

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

void Increase_PC() // PrevPC = CurrPC; CurrPC = NextPC; NextPC + 4bytes
{
	/* set previous programm counter (debugging only)
     * similar to: registers[PrevPCReg] = registers[PCReg];*/
	machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));  // PCReg: current Programm counter, then write this to the previous PC

	/* set programm counter to next instruction
     * similar to: registers[PCReg] = registers[NextPCReg]*/
	machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg)); // get value of the next PC, then write it to the current PC

	/* set next programm counter for brach execution
     * similar to: registers[NextPCReg] = pcAfter;*/
	machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg) + 4); // write the next 4 bytes to the next PC
}

void ExceptionHandler(ExceptionType which)
{
	int type = machine->ReadRegister(2);
	SC_Handle A;

	switch (which)
	{
	case NoException:
		return;

	case SyscallException:
	{
		switch (type)
		{
		case SC_Halt:
		{
			A.Syscall_Halt();
			break;
		}
		case SC_Print:
		{
			A.Syscall_Print();
			break;
		}
		case SC_Create:
		{
			A.Syscall_Create();
			break;
		}
		case SC_Scanf:
		{
			A.Syscall_Scan();
			break;
		}
		case SC_Open:
		{
			A.Syscall_OpenFile();
			break;
		}
		case SC_Close:
		{
			A.Syscall_CloseFile();
			break;
		}
		case SC_Read:
		{
			A.Syscall_ReadFile();
			break;
		}
		case SC_Write:
		{
			A.Syscall_WriteFile();
			break;
		}
		case SC_SeekFile:
		{
			A.Syscall_SeekFile();
			break;
		}
		case SC_DeleteFile:
		{
			A.Syscall_Delete();
			break;
		}
		default:
			printf("\n Unexpected user mode exception (%d %d)", which, type);
			interrupt->Halt();
		}
		Increase_PC(); // Increase PC after each syscall
		break;
	}
	case PageFaultException:
	{
		printf("\nNo valid translation found.\n");
		ASSERT(FALSE);
		break;
	}
	case ReadOnlyException:
	{
		printf("\nWrite attempted to page marked \"read-only\".\n");
		ASSERT(FALSE);
		break;
	}
	case BusErrorException:
	{
		printf("\nTranslation resulted in an invalid physical address.\n");
		ASSERT(FALSE);
		break;
	}
	case AddressErrorException:
	{
		printf("\nUnaligned reference or one that was beyond the end of the address space.\n");
		ASSERT(FALSE);
		break;
	}
	case OverflowException:
	{
		printf("\nInteger overflow in add or sub.\n");
		ASSERT(FALSE);
		break;
	}
	case IllegalInstrException:
	{
		printf("\nUnimplemented or reserved instr\n");
		ASSERT(FALSE);
		break;
	}
	case NumExceptionTypes:
	{
		printf("\nNumExceptionTypes\n");
		ASSERT(FALSE);
		break;
	}
	}
}
