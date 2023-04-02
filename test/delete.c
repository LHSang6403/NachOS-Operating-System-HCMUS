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
    Delete(name, IdFile);
    Close(IdFile);
    Delete(name, IdFile);

    Halt();
    return 0;
}