#include "syscall.h"
#include "copyright.h"

#define MAX_LENGTH 255

#define STDIN 0
#define STDOUT 1

int main(int argc, char* argv[])
{
	OpenFileId fileId;
	int i = 0;
	char *c;
	int filesz;
	char name[MAX_LENGTH];
	
	Printf("Input file name:");
	Scanf(name, MAX_LENGTH);
	if ((fileId = Open(name)) == -1)
	{
		Printf("Can not open file ");
		return 0;
	}
	filesz = SeekFile(-1, fileId);
	i = SeekFile(0, fileId);

	while(i<filesz)
	{	
		Read(c,1, fileId);
		Printf(c);
		i++;
	}

	Printf("\nRead file done\n");
	Close(fileId);
	Halt();
	return 0;
}
