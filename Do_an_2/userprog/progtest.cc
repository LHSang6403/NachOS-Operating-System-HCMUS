// progtest.cc
//	Test routines for demonstrating that Nachos can load
//	a user program and execute it.
//
//	Also, routines for testing the Console hardware device.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "console.h"
#include "addrspace.h"
#include "synch.h"

using namespace std;


//----------------------------------------------------------------------
// StartProcess
// 	Run a user program.  Open the executable, load it into
//	memory, and jump to it.
//----------------------------------------------------------------------

#include "string.h"
#include "progtest.h"



char* intToChar(int num) {
    char* result = new char[11]; // độ dài tối đa của một số nguyên có dấu 32 bit là 10, và cần thêm một ký tự null terminator
    int i = 0;
    if (num < 0) {
        result[i++] = '-';
        num = -num;
    }
    int div = 1;
    while (num / div >= 10) {
        div *= 10;
    }
    while (div > 0) {
        result[i++] = num / div + '0';
        num %= div;
        div /= 10;
    }
    result[i] = '\0'; // thêm ký tự null terminator
    return result;
}

// void StartProcess(int convert)
// {
//     char* filename;
//     filename = User2System(convert, MaxFileLength + 1);

//     OpenFile *executable = fileSystem->Open(filename);
//     AddrSpace *space;

//     if (executable == NULL)
//     {
//         printf("Unable to open file %s\n", filename);
//         return;
//     }
//     space = new AddrSpace(executable);
//     currentThread->space = space;

//     delete executable; // close file

//     space->InitRegisters(); // set the initial register values
//     space->RestoreState();  // load page table register

//     machine->Run(); // jump to the user progam
//     ASSERT(FALSE);  // machine->Run never returns;
//                     // the address space exits
//                     // by doing the syscall "exit"
// }

void StartProcess(char *filename)
{
    OpenFile *executable = fileSystem->Open(filename);
    AddrSpace *space;

    if (executable == NULL)
    {
        printf("Unable to open file %s\n", filename);
        return;
    }
    space = new AddrSpace(executable);
    currentThread->space = space;

    delete executable; // close file

    space->InitRegisters(); // set the initial register values
    space->RestoreState();  // load page table register

    machine->Run(); // jump to the user progam
    ASSERT(FALSE);  // machine->Run never returns;
                    // the address space exits
                    // by doing the syscall "exit"
}

// Data structures needed for the console test.  Threads making
// I/O requests wait on a Semaphore to delay until the I/O completes.


//----------------------------------------------------------------------
// ConsoleTest
// 	Test the console by echoing characters typed at the input onto
//	the output.  Stop when the user types a 'q'.
//----------------------------------------------------------------------

void ConsoleTest(char *in, char *out)
{
    char ch;

    console = new Console(in, out, ReadAvail, WriteDone, 0);
    readAvail = new Semaphore("read avail", 0);
    writeDone = new Semaphore("write done", 0);

    for (;;)
    {
        readAvail->P(); // wait for character to arrive
        ch = console->GetChar();
        console->PutChar(ch); // echo it!
        writeDone->P();       // wait for write to finish
        if (ch == 'q')
            return; // if q, quit
    }
}