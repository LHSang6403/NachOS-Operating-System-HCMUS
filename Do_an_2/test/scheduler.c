#include "syscall.h"
#include "copyright.h"

#define MAXLENGTH 255

int main()
{
    int pingID, pongID;
    char *temp;
    Printf("Ping-pong testing ..... \n\n");
    pingID = Exec("./test/ping");
    pongID = Exec("./test/pong");
    while(1)
    {

    }
}