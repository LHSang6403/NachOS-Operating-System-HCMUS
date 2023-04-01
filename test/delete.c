#include "syscall.h"
#include "copyright.h"

#define MAX_LENGTH 255

int main()
{
    OpenFileId IdFile;
    char name[MAX_LENGTH];
    int is;
    Printf("Input file name: ");
    Scanf(name, MAX_LENGTH);
    //DeleteFile(name,IdFile);
    IdFile = Open(name);
    Printf("Opened file \n");
    DeleteFile(name, IdFile);
    Close(IdFile);
    DeleteFile(name, IdFile);
    Halt();
    return 0;
}