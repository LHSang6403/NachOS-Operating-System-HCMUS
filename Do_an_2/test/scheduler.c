#include "syscall.h"
#include "copyright.h"

#define MAXLENGTH 255

int main()
{
    int pingID, pongID;
    char *temp;
    Printf("Ping-pong testing ..... \n\n");
    pingID = Exec("/home/huybean2003/Desktop/HDH/nachos/nachos-3.4/code/test/ping");
    pongID = Exec("/home/huybean2003/Desktop/HDH/nachos/nachos-3.4/code/test/pong");
    //Exec("/home/huybean2003/Desktop/HDH/nachos/nachos-3.4/code/test/ping");

    while(1)
    {

    }
}