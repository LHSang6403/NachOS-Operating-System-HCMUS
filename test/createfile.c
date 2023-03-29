#include "syscall.h"
#include "copyright.h"
#define maxlen 32
int main()
{
	int is = Create("123.txt");
	if (is)
	{
		Printf("Can not create file \'123.txt\'\n");
	}
	else
	{
		Printf("Successfully create file\n");
	}
	Halt();
	return 0;
}
