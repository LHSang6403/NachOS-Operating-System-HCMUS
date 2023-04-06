#include "syscall.h"
#include "copyright.h"

#define MAX_LENGTH 255

int main()
{
    OpenFileId src;
    OpenFileId dest;

    int filesize;
    char *temp;
    int i;

    char source[MAX_LENGTH];
    char destination[MAX_LENGTH];

    Printf("Input source file: ");
    Scanf(source, MAX_LENGTH);
    Printf("\nInput destination file: ");
    Scanf(destination, MAX_LENGTH);

    if ((src = Open(source)) == -1)
    {
        Printf("\nFile source is having trouble when open\n");
        Halt();
        return 0;
    }
    if ((src = Open(source)) == -1 && (dest = Open(destination)) == -1)
    {
        Printf("\nBoth files have trouble when open\n");
        Halt();
        return 0;
    }

    if ((src = Open(source)) == 1 && (dest = Open(destination)) == 1)
    {
        Printf("\nBoth files open successfully\n");
    }

    if ((src = Open(source)) != -1 && (dest = Open(destination)) == -1)
    {
        char *keyboard;
        Printf("\nFile destination have trouble when open\nDo you want to continue with a new file (Y/n): ");
        Scanf(keyboard, MAX_LENGTH);
        if (keyboard[0] == 'y' || keyboard == 'Y')
        {
            Printf("\nPlease input a new file name: ");
            Scanf(destination, MAX_LENGTH);
            CreateFile(destination);
            dest = Open(destination);
        }
        else
        {
            Halt();
            return 0;
        }
    }

    filesize = Seek(-1, src);
    Seek(0, src);

    //Seek(0, destination);
    i = 0;
    while (i < filesize)
    {
        Read(temp, 1, src);
        Write(temp, 1, dest);
        i++;
    }
    Printf("\nCopy file successfully\n");

    Close(src);
    Close(dest);

    Halt();
    return 0;
}