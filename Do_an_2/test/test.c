#include "syscall.h"
#include "copyright.h"

#define STDIN 0
#define STDOUT 1

int main()
{
    Printf("Ping-pong testing ..... \n\n");
    Exec("/home/huybean2003/Desktop/HDH/nachos/nachos-3.4/code/test/ping.c");
    Exec("/home/huybean2003/Desktop/HDH/nachos/nachos-3.4/code/test/pong.c");
    while (1)
    {
        /* code */
    }
    
}