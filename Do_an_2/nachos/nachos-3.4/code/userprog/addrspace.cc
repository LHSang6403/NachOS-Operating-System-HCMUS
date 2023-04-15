// addrspace.cc
//	Routines to manage address spaces (executing user programs).
//
//	In order to run a user program, you must:
//
//	1. link with the -N -T 0 option
//	2. run coff2noff to convert the object file to Nachos format
//		(Nachos object code format is essentially just a simpler
//		version of the UNIX executable object code format)
//	3. load the NOFF file into the Nachos file system
//		(if you haven't implemented the file system yet, you
//		don't need to do this last step)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "addrspace.h"
#include "noff.h"
#ifdef HOST_SPARC
#include <strings.h>
#endif

//----------------------------------------------------------------------
// SwapHeader
// 	Do little endian to big endian conversion on the bytes in the
//	object file header, in case the file was generated on a little
//	endian machine, and we're now running on a big endian machine.
//----------------------------------------------------------------------

static void
SwapHeader(NoffHeader *noffH)
{
    noffH->noffMagic = WordToHost(noffH->noffMagic);
    noffH->code.size = WordToHost(noffH->code.size);
    noffH->code.virtualAddr = WordToHost(noffH->code.virtualAddr);
    noffH->code.inFileAddr = WordToHost(noffH->code.inFileAddr);
    noffH->initData.size = WordToHost(noffH->initData.size);
    noffH->initData.virtualAddr = WordToHost(noffH->initData.virtualAddr);
    noffH->initData.inFileAddr = WordToHost(noffH->initData.inFileAddr);
    noffH->uninitData.size = WordToHost(noffH->uninitData.size);
    noffH->uninitData.virtualAddr = WordToHost(noffH->uninitData.virtualAddr);
    noffH->uninitData.inFileAddr = WordToHost(noffH->uninitData.inFileAddr);
}
//BitMap *AddrSpace::bitmap = new BitMap(NumPhysPages);
//----------------------------------------------------------------------
// AddrSpace::AddrSpace
// 	Create an address space to run a user program.
//	Load the program from a file "executable", and set everything
//	up so that we can start executing user instructions.
//
//	Assumes that the object code file is in NOFF format.
//
//	First, set up the translation from program memory to physical
//	memory.  For now, this is really simple (1:1), since we are
//	only uniprogramming, and we have a single unsegmented page table
//
//	"executable" is the file containing the object code to load into memory
//----------------------------------------------------------------------

AddrSpace::AddrSpace(OpenFile *executable)
{
    NoffHeader noffH;
    unsigned int size;

    executable->ReadAt((char *)&noffH, sizeof(noffH), 0);
    if ((noffH.noffMagic != NOFFMAGIC) &&
        (WordToHost(noffH.noffMagic) == NOFFMAGIC))
        SwapHeader(&noffH);
    ASSERT(noffH.noffMagic == NOFFMAGIC);

    // how big is address space?
    size = noffH.code.size + noffH.initData.size + noffH.uninitData.size + UserStackSize; // we need to increase the size
                                                                                          // to leave room for the stack
    numPages = divRoundUp(size, PageSize);
    size = numPages * PageSize;

    ASSERT(numPages <= NumPhysPages); // check we're not trying
                                      // to run anything too big --
                                      // at least until we have
                                      // virtual memory

    DEBUG('a', "Initializing address space, num pages %d, size %d\n",
          numPages, size);
    // first, set up the translation
    pageTable = new TranslationEntry[numPages];
    unsigned int i;
    for (i = 0; i < numPages; i++)
    {
        pageTable[i].virtualPage = i;
        pageTable[i].physicalPage = bitmapPhysPage->Find(); // Su dung ham Find() de tim phan tu dau tien dang trong trong trang vat ly
        pageTable[i].valid = TRUE;
        pageTable[i].use = FALSE;
        pageTable[i].dirty = FALSE;
        pageTable[i].readOnly = FALSE; // if the code segment was entirely on
        // a separate page, we could set its
        // pages to be read-only

        // zero out the entire address space, to zero the unitialized data segment
        // and the stack segment
        bzero(&(machine->mainMemory[pageTable[i].physicalPage * PageSize]), PageSize);
    }

    // then, copy in the code and data segments into memory

    // Tính số trang cần thiết để lưu trữ mã máy và dữ liệu khởi tạo
    numCodePage = divRoundUp(noffH.code.size, PageSize);
    // Tính kích thước của trang cuối cùng lưu trữ mã máy
    lastCodePageSize = noffH.code.size - (numCodePage - 1) * PageSize;
    // Tính kích thước của trang đầu tiên lưu trữ dữ liệu khởi tạo (Tạm thời)
    unsigned int tempSize = noffH.initData.size - (PageSize - lastCodePageSize);

    // Nếu kích thước của dữ liệu khởi tạo không đủ để tạo một trang, không cần phải lưu trữ dữ liệu khởi tạo
    if (tempSize < 0)
    {
        numDataPage = 0;
        firstDataPageSize = noffH.initData.size;
    }
    // Nếu đủ để tạo một trang, tính số trang cần thiết và kích thước của trang cuối cùng
    else
    {
        // Tính lại kích thước của trang đầu tiên lưu trữ dữ liệu khởi tạo
        numDataPage = divRoundUp(tempSize, PageSize);
        lastDataPageSize = tempSize - (numDataPage - 1) * PageSize;
        firstDataPageSize = PageSize - lastCodePageSize;
    }

    // Đọc CodePage và lưu vào bộ nhớ vật lý (Physical memory)
    for (i = 0; i < numCodePage; i++)
    {
        // Đọc một trang CodePage từ tệp thực thi và lưu vào bộ nhớ vật lý tại địa chỉ ảo tương ứng
        // Nếu là trang cuối cùng, sử dụng kích thước của trang cuối cùng đã tính toán trước đó
        // Ngược lại, sử dụng kích thước của trang mặc định
        if (noffH.code.size > 0)
        {
            executable->ReadAt(&(machine->mainMemory[noffH.code.virtualAddr + pageTable[i].physicalPage * PageSize]), i < (numCodePage - 1) ? PageSize : lastCodePageSize, noffH.code.inFileAddr + i * PageSize);
        }
    }

    // Nếu kích thước của trang cuối cùng của CodePage nhỏ hơn kích thước trang mặc định,
    // cần đọc dữ liệu khởi tạo và lưu vào trang đầu tiên lưu trữ dữ liệu khởi tạo (firstDataPageSize)
    if (lastCodePageSize < PageSize)
    {
        // Đọc dữ liệu khởi tạo từ tệp thực thi và lưu vào bộ nhớ vật lý
        if (firstDataPageSize > 0)
        {
            executable->ReadAt(&(machine->mainMemory[noffH.code.virtualAddr + (pageTable[i - 1].physicalPage * PageSize + lastCodePageSize)]), firstDataPageSize, noffH.initData.inFileAddr);
        }
    }

    for (int j = 0; j < numDataPage; j++)
    {
        // Đọc một trang DataPage từ tệp thực thi và lưu vào bộ nhớ vật lý tại địa chỉ ảo tương ứng
        // Nếu là trang cuối cùng, sử dụng kích thước của trang cuối cùng đã tính toán trước đó
        // Ngược lại, sử dụng kích thước của trang mặc định
        if (noffH.initData.size > 0)
        {
            executable->ReadAt(&(machine->mainMemory[noffH.code.virtualAddr + pageTable[i].physicalPage * PageSize]), j < (numDataPage - 1) ? PageSize : lastDataPageSize, noffH.initData.inFileAddr + j * PageSize + firstDataPageSize);
            i++;
        }
    }

    // Khởi tạo ID cho Space ID
    spaceId = InitializeSpaceID();
}

unsigned int AddrSpace::InitializeSpaceID()
{
    return Random() % 10000;
}

//----------------------------------------------------------------------
// AddrSpace::InitializeSpaceID
// 	Initializing space ID randomly 
//----------------------------------------------------------------------

AddrSpace::~AddrSpace()
{
    // Vòng lặp for được sử dụng để duyệt qua tất cả các trang được lưu trong pageTable
    // Và giải phóng các trang này bằng cách xóa các bit tương ứng trên bitmapPhysPage
    for (int i = 0; i < numPages; i++)
    {
        bitmapPhysPage->Clear(pageTable[i].physicalPage);
    }
    delete pageTable;
}

//----------------------------------------------------------------------
// AddrSpace::InitRegisters
// 	Set the initial values for the user-level register set.
//
// 	We write these directly into the "machine" registers, so
//	that we can immediately jump to user code.  Note that these
//	will be saved/restored into the currentThread->userRegisters
//	when this thread is context switched out.
//----------------------------------------------------------------------

void AddrSpace::InitRegisters()
{
    int i;

    for (i = 0; i < NumTotalRegs; i++)
        machine->WriteRegister(i, 0);

    // Initial program counter -- must be location of "Start"
    machine->WriteRegister(PCReg, 0);

    // Need to also tell MIPS where next instruction is, because
    // of branch delay possibility
    machine->WriteRegister(NextPCReg, 4);

    // Set the stack register to the end of the address space, where we
    // allocated the stack; but subtract off a bit, to make sure we don't
    // accidentally reference off the end!
    machine->WriteRegister(StackReg, numPages * PageSize - 16);
    DEBUG('a', "Initializing stack register to %d\n", numPages * PageSize - 16);
}

//----------------------------------------------------------------------
// AddrSpace::SaveState
// 	On a context switch, save any machine state, specific
//	to this address space, that needs saving.
//
//	For now, nothing!
//----------------------------------------------------------------------

void AddrSpace::SaveState()
{
}

//----------------------------------------------------------------------
// AddrSpace::RestoreState
// 	On a context switch, restore the machine state so that
//	this address space can run.
//
//      For now, tell the machine where to find the page table.
//----------------------------------------------------------------------

void AddrSpace::RestoreState()
{
    machine->pageTable = pageTable;
    machine->pageTableSize = numPages;
}
