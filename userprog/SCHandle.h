#ifndef SC_HANDLE
#define SC_HANDLE

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "filesys.h"
#include "filehdr.h"
#include "system.h"
#include "openfile.h"
#include "machine.h"
#include "utility.h"

#define MaxFileLength 32
#define MaxLength 100000000

char *User2System(int virtAddr, int limit);

//Ngược lại ta có hàm System2User như sau:

// Input: - User space address (int)
// - Limit of buffer (int)
// - Buffer (char[])
// Output:- Number of bytes copied (int)
// Purpose: Copy buffer from System memory space to User memory space
int System2User(int virtAddr, int len, char *buffer);

class SC_Handle
{
    public:
        void Syscall_Halt();
        void Syscall_Print();
        void Syscall_Scan();
        void Syscall_Create(); 
        void Syscall_OpenFile();
        void Syscall_CloseFile();
        void Syscall_ReadFile();
        void Syscall_WriteFile();
        void Syscall_SeekFile();
        void Syscall_Delete();
};

#endif