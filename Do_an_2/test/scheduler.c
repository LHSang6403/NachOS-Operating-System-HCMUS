#include "syscall.h"

int main()
{
    int pingID, pongID;
    Printf("Ping-pong testing ..... \n\n");
    Exec("/home/huybean2003/Desktop/HDH/nachos/nachos-3.4/code/test/ping.c");
    Exec("/home/huybean2003/Desktop/HDH/nachos/nachos-3.4/code/test/pong.c");
    while (1)
    {
        /* code */
    }
    
}