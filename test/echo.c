#include "syscall.h"
#include "copyright.h"

#define MAX_LENGTH 255

#define STDIN 0
#define STDOUT 1

int main()
{
	char *a;
	Printf("Enter a string: \0");
	Scanf(a, MAX_LENGTH);
	Printf(a);	
	return 0;
}
