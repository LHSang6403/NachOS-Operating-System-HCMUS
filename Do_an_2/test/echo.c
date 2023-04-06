#include "syscall.h"
#include "copyright.h"

#define MAX_LENGTH 255

#define STDIN 0
#define STDOUT 1

int main()
{
	char *a;
	Printf("Enter a string: ");
	Scanf(a, MAX_LENGTH);
	Printf("The input string: ");
	Printf(a);
	Halt();	
	return 0;
}
