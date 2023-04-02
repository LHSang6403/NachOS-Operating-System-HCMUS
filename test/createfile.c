#include "syscall.h"
#include "copyright.h"
#define maxlen 32
#define MAX_LENGTH 255
int main()
{
	char filename[MAX_LENGTH];
	int isCreate;
	Printf("Input file name to create: ");
	Scanf(filename, MAX_LENGTH);
	isCreate = CreateFile(filename);
	if (isCreate == 0) // tra ve 
	{
		Printf("\nSuccessfully create file ");
		Printf(filename);
	}
	else
	{
		Printf("\nCan't create file ");
		Printf(filename);
	}
	Halt();
	return 0;
}
