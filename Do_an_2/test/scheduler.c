#include "syscall.h"
#include "copyright.h"

void main()
{
    int pingID, pongID;
    Printf("Ping-Pong test starting ... \n\n");
    pingID = Exec("./test/ping.c");
    pongID = Exec ("./test/pong.c");
   
    while(1){};

   // Halt();
}