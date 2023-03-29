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

<<<<<<< HEAD
void Increase_PC()
=======
void inc_program_counter() // PrevPC = CurrPC; CurrPC = NextPC; NextPC + 4
>>>>>>> b234f5893828cf45b929ee61bccfef40c4febde9
{
	/* set previous programm counter (debugging only)
     * similar to: registers[PrevPCReg] = registers[PCReg];*/
	machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg)); // PCReg: current Programm counter, then write this to the previous PC

	/* set programm counter to next instruction
     * similar to: registers[PCReg] = registers[NextPCReg]*/
	machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg)); // get value of the next PC, then write it to the current PC

	/* set next programm counter for brach execution
     * similar to: registers[NextPCReg] = pcAfter;*/
	machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg) + 4); // write the next 4 bytes to the next PC
}

<<<<<<< HEAD
=======
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
//Ngược lại ta có hàm System2User như sau:

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

>>>>>>> b234f5893828cf45b929ee61bccfef40c4febde9
void ExceptionHandler(ExceptionType which)
{
	int type = machine->ReadRegister(2);
	SC_Handle A;

	switch (which)
	{
	case NoException:
		return;

    case PageFaultException:
        DEBUG('a', "\n No valid translation found"); // debug
        printf("\n\n No valid translation found"); // run time
        interrupt->Halt(); // close os
        break;

    case ReadOnlyException:
        DEBUG('a', "\n Write attempted to page marked \"read-only\"");
        printf("\n\n Write attempted to page marked \"read-only\"");
        interrupt->Halt();
        break;

    case BusErrorException:
        DEBUG('a', "\n Translation resulted in an invalid physical address");
        printf("\n\n Translation resulted in an  invalid physical address");
        interrupt->Halt();
        break;

    case AddressErrorException:
        DEBUG('a', "\n Unaligned reference or one that was beyond the end of the address space");
        printf("\n\n Unaligned reference or one that was beyond the end of the address space");
        interrupt->Halt();
        break;

    case OverflowException:
        DEBUG('a', "\n Integer overflow in add or sub");
        printf("\n\n Integer overflow in add or sub");
        interrupt->Halt();
        break;

    case IllegalInstrException:
        DEBUG('a', "\n Unimplemented or reserved instr");
        printf("\n\n Unimplemented or reserved instr");
        interrupt->Halt();
        break;

    case NumExceptionTypes:
        DEBUG('a', "\n Num Exception Types");
        printf("\n\n Num Exception Types");
        interrupt->Halt();
        break;

	case SyscallException:
	{
		switch (type)
		{
<<<<<<< HEAD
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
		Increase_PC();
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
=======
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
				// Lấy tham số tên tập tin từ thanh ghi r4
				virtAddr = machine->ReadRegister(4);
				DEBUG('a', "\n Reading filename.");
				// MaxFileLength là = 32
				filename = User2System(virtAddr, MaxFileLength + 1);
				if (filename == NULL)
				{
					printf("\n Not enough memory in system");
					DEBUG('a', "\n Not enough memory in system");
					machine->WriteRegister(2, -1);
					// trả về lỗi cho chương trình người dùng
					delete filename;
					return;
				}
				DEBUG('a', "\n Finish reading filename.");
				//DEBUG('a',"\n File name : '"<<filename<<"'");
				// Create file with size = 0
				// Dùng đối tượng fileSystem của lớp OpenFile để tạo file,
				// việc tạo file này là sử dụng các thủ tục tạo file của hệ điều
				// hành Linux, chúng ta không quản ly trực tiếp các block trên
				// đĩa cứng cấp phát cho file, việc quản ly các block của file
				// trên ổ đĩa là một đồ án khác
				if (!fileSystem->Create(filename, 0))
				{
					printf("\n Error create file '%s'", filename);
					machine->WriteRegister(2, -1);
					delete filename;
					return;
				}
				machine->WriteRegister(2, 0);
				// trả về cho chương trình người dùng thành công
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
		inc_program_counter(); // increase PC after each syscall
>>>>>>> b234f5893828cf45b929ee61bccfef40c4febde9
	}
}
