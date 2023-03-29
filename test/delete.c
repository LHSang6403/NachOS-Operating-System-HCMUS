#include "syscall.h"
#include "copyright.h"

#define MAX_LENGTH 255

int main()
{
    char name[MAX_LENGTH];
    int is;
    Printf("Input file name: ");
    Scanf(name, MAX_LENGTH);
    DeleteFile(name);
    is = Open("test.txt");
    if (is)
    {
        Printf("Opened file test.txt\n");
        DeleteFile("test.txt");
    }
    Halt();
    return 0;
}