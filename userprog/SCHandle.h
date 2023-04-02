#ifndef SC_HANDLE
#define SC_HANDLE

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "filesys.h"
#include "filehdr.h"
#include "system.h"
#include "openfile.h"
#include "machine.h"
#include "utility.h"

#define MaxFileLength 32
#define MaxLength 100000000

char *User2System(int virtAddr, int limit)
{
    int i; // pos
    int oneChar;
    char *kernelBuf = NULL;
    kernelBuf = new char[limit + 1]; //need for terminal string
    if (kernelBuf == NULL)
    {
        return kernelBuf;
    }
    memset(kernelBuf, 0, limit + 1);
    //printf("\n Filename u2s:");
    for (i = 0; i < limit; i++)
    {
        machine->ReadMem(virtAddr + i, 1, &oneChar);
        kernelBuf[i] = (char)oneChar;
        //printf("%c",kernelBuf[i]);
        if (oneChar == 0)
        {
            break;
        }
    }
    return kernelBuf;
}
//Ngược lại ta có hàm System2User như sau:

// Input: - User space address (int)
// - Limit of buffer (int)
// - Buffer (char[])
// Output:- Number of bytes copied (int)
// Purpose: Copy buffer from System memory space to User memory space
int System2User(int virtAddr, int len, char *buffer)
{
    if (len < 0)
    {
        return -1;
    }
    if (len == 0)
    {
        return len;
    }
    int i = 0;
    int oneChar = 0;
    do
    {
        oneChar = (int)buffer[i];
        machine->WriteMem(virtAddr + i, 1, oneChar);
        i++;
    } while (i < len && oneChar != 0);
    return i;
}

class SC_Handle
{
public:
    void Syscall_Halt()
    {
        DEBUG('a', "\n Shutdown, initiated by user program.");
        printf("\n\n Shutdown, initiated by user program.");
        interrupt->Halt();
    }
    
    void Syscall_Print()
    {
        int virtAddr = machine->ReadRegister(4); //tham so thu i trong ham
        char *temp = new char[MaxLength];
        //memset(temp, 0, MaxLength);
        temp = User2System(virtAddr, MaxLength);
        int i = 0;
        while (temp[i] != 0) // Goi tung chu cai trong chuoi
        {
            synchConsole->Write(temp + i, 1);
            i++;
        }
        // synchConsole->Write(temp + i, 1);
        //i++;
        delete[] temp;
        return;
    }

    void Syscall_Scan()
    {
        char *buf = new char[255];

        int bufAddrUser = machine->ReadRegister(4); // Lay tham so can in
        int length = machine->ReadRegister(5);      // Lay do dai cua tham so
        //	Doc length cua buffer vao I/O
        //	Tra ve so byte doc duoc
        int sz = synchConsole->Read(buf, length);
        System2User(bufAddrUser, sz, buf);
        delete[] buf;
    }

    void Syscall_Create()
    {
        int virtAddr;
        char *filename;
        DEBUG('a', "\n SC_Create call ...");
        DEBUG('a', "\n Reading virtual address of filename");
        // Lấy tham số tên tập tin từ thanh ghi r4
        virtAddr = machine->ReadRegister(4);
        DEBUG('a', "\n Reading filename.");
        // MaxFileLength là = 32
        filename = User2System(virtAddr, MaxFileLength + 1);
        if (filename == NULL)
        {
            printf("\n Not enough memory in system");
            DEBUG('a', "\n Not enough memory in system");
            machine->WriteRegister(2, -1);
            // trả về lỗi cho chương trình người dùng
            delete filename;
            return;
        }
        DEBUG('a', "\n Finish reading filename.");
        //DEBUG('a',"\n File name : '"<<filename<<"'");
        // Create file with size = 0
        // Dùng đối tượng fileSystem của lớp OpenFile để tạo file,
        // việc tạo file này là sử dụng các thủ tục tạo file của hệ điều
        // hành Linux, chúng ta không quản ly trực tiếp các block trên
        // đĩa cứng cấp phát cho file, việc quản ly các block của file
        // trên ổ đĩa là một đồ án khác
        if (!fileSystem->Create(filename, 0))
        {
            printf("\n Error create file '%s'", filename);
            machine->WriteRegister(2, -1);
            delete filename;
            return;
        }
        machine->WriteRegister(2, 0);
        // trả về cho chương trình người dùng thành công
        delete filename;
    }

    void Syscall_OpenFile()
    {
        int bufAddr = machine->ReadRegister(4);

        char *buf;
        // if already opened 10 files
        if (fileSystem->pos > 10)
        {
            machine->WriteRegister(2, -1);
            delete[] buf;
            return;
        }

        // Neu mo sdtin hay stdout, giu nguyen so luong file
        buf = User2System(bufAddr, MaxFileLength + 1); // copy the address of file's name from User to System
        if (strcmp(buf, "stdin") == 0)
        {
            printf("Stdin mode\n");
            machine->WriteRegister(2, 0);
            delete[] buf;
            return;
        }
        if (strcmp(buf, "stdout") == 0)
        {
            printf("Stdout mode\n");
            machine->WriteRegister(2, 1);
            delete[] buf;
            return;
        }

        if ((fileSystem->_openFile[fileSystem->pos] = fileSystem->Open(buf)) != NULL)
        {
            printf("\nOpen file success '%s'\n", buf);
            machine->WriteRegister(2, fileSystem->pos - 1);
        }
        else
        {
            printf("Can not open file '%s'", buf);
            machine->WriteRegister(2, -1);
        }
        delete[] buf;
        return;
    }

    void Syscall_CloseFile()
    {
        int no = machine->ReadRegister(4);
        int i = fileSystem->pos;

        // Neu so luong file la i < vi tri file can dong -> sai logic
        if (i < no)
        {
            printf("Close file failed \n");
            machine->WriteRegister(2, -1);
            return;
        }

        fileSystem->_openFile[no] = NULL;
        delete fileSystem->_openFile[no]; // Dong file
        machine->WriteRegister(2, 0);     // Tra ve 0 neu thanh cong
        printf("\nClose file success\n");
    }

    void Syscall_ReadFile()
    {
        int virtAddr = machine->ReadRegister(4);     // Lay dia chi cua tham so can doc tren thanh ghi 4
        int BufferNumber = machine->ReadRegister(5); // Lay dia so luong byte can doc tren thanh ghi 5
        int openf_id = machine->ReadRegister(6);     // Lay dia chi cua OpenFileId cua file

        int m_pos = fileSystem->_openFile[openf_id]->GetCurrentPos(); // Lay vi tri hien tai cua file nay trong filesystem

        // Kiem tra gioi han hop le
        if (openf_id < 0 || openf_id > 10)
        {
            machine->WriteRegister(2, -1);
            return;
        }

        // Neu file khong ton tai, loi
        if (fileSystem->_openFile[openf_id] == NULL) // Doc loi, tra ve trang thai -1
        {
            machine->WriteRegister(2, -1);
            return;
        }

        char *buf = User2System(virtAddr, BufferNumber); // copy chuỗi vùng nhớ từ User sang System với bộ đệm buffer dài charcount

        if (openf_id == 0) // Doc tu stdin, ==0 do stdin ở đầu mảng
        {
            // lấy chuỗi nhập từ bàn phím lưu vào trong buf
            int sz = synchConsole->Read(buf, BufferNumber); // sz là số byte thực sự được đọc, được trả về từ hàm Read của synchConsole
            System2User(virtAddr, sz, buf);
            machine->WriteRegister(2, sz); // ghi size đọc thực tế

            delete[] buf;
            return;
        }

        char *tempbuf = new char[BufferNumber];
        if (openf_id == 1) // stdout
        {
            int length = synchConsole->Read(tempbuf, BufferNumber);
            System2User(virtAddr, length, tempbuf);
            machine->WriteRegister(2, length);
        }

        int before = fileSystem->_openFile[openf_id]->GetCurrentPos();
        if ((fileSystem->_openFile[openf_id]->Read(buf, BufferNumber)) > 0) // Doc tung ky tu cua file
        {
            // copy data tu kernel sang user
            int after = fileSystem->_openFile[openf_id]->GetCurrentPos(); // Lay vi tri byte sau khi Read xong o đieu kien
            System2User(virtAddr, BufferNumber, buf);
            machine->WriteRegister(2, after - before + 1); // after & before duoc dung de
        }                                                  // Tra ve so byte thuc su
        else
        {
            machine->WriteRegister(2, -1);
        }
        delete[] buf;
    }

    void Syscall_WriteFile()
    {
        int virtAddr = machine->ReadRegister(4);
        int charcount = machine->ReadRegister(5);
        int openf_id = machine->ReadRegister(6);
        int i = fileSystem->pos;

        if (openf_id > i || openf_id <= 0 ) // File vuot qua openf_id, khi id khong ton tai
        {
            machine->WriteRegister(2, -1);
            return;
        }

        if (fileSystem->_openFile[openf_id] == NULL)
        {
            machine->WriteRegister(2, -1);
            return;
        }

        // read-only file
        if (fileSystem->_openFile[openf_id]->type == 1)
        {
            printf("Read-only file!!! Can't write");
            machine->WriteRegister(2, -1);
            return;
        }

        // write to console
        char *buf = User2System(virtAddr, charcount);
        if (openf_id == 1)
        {
            int i = 0;
            while (buf[i] != '\0' && buf[i] != '\n')
            {
                synchConsole->Write(buf + i, 1);
                i++;
            }
            buf[i] = '\n';
            synchConsole->Write(buf + i, 1); // Ky tu cuoi cung (neu can)

            machine->WriteRegister(2, i - 1);
            delete[] buf;
        }

        int before = fileSystem->_openFile[openf_id]->GetCurrentPos();
        if ((fileSystem->_openFile[openf_id]->Write(buf, charcount)) != 0)
        {
            int after = fileSystem->_openFile[openf_id]->GetCurrentPos();
            System2User(virtAddr, after - before, buf);
            machine->WriteRegister(2, after - before + 1);
            delete[] buf;
            return;
        }
    }

    void Syscall_SeekFile()
    {
        int pos = machine->ReadRegister(4);
        int openf_id = machine->ReadRegister(5);

        // seek into files: stdin, stdout, `out of domain` fileSystem
        if (openf_id < 1 || openf_id > fileSystem->pos || fileSystem->_openFile[openf_id] == NULL)
        {
            machine->WriteRegister(2, -1);
            return;
        }

        int len = fileSystem->_openFile[openf_id]->Length(); // file size

        if (pos > len) // try to move file ptr to pos, pos > len --> wrong
        {
            machine->WriteRegister(2, -1);
            return;
        }

        if (pos == -1) // move file ptr to the end of file
            pos = len;

        fileSystem->_openFile[openf_id]->Seek(pos);
        machine->WriteRegister(2, pos);
    }

    void Syscall_Delete()
    {
        int virtAddr;
        char *filename;
        int openf_id;
        DEBUG('a', "\n SC_DeleteFile call ...");
        DEBUG('a', "\n Reading virtual address of filename");
        // Lấy tham số tên tập tin và id từ thanh ghi r4, r5
        virtAddr = machine->ReadRegister(4);
        openf_id = machine->ReadRegister(5);
        DEBUG('a', "\n Reading filename.");

        // MaxFileLength là = 32
        filename = User2System(virtAddr, MaxFileLength + 1);
        if (filename == NULL)
        {
            printf("\n Not enough memory in system");
            DEBUG('a', "\n Not enough memory in system");
            machine->WriteRegister(2, -1);
            // trả về lỗi cho chương trình người dùng
            delete filename;
            return;
        }
        // Kiem tra gioi han hop le
        if (openf_id < 0 || openf_id > 10)
        {
            machine->WriteRegister(2, -1);
            return;
        }

        // Neu file khong mo nhung tenfile ton tai -> delete
        if (fileSystem->_openFile[openf_id] == NULL)
        {
            if (!fileSystem->Remove(filename))
            {
                printf("\n Error deleting file '%s'", filename);
                machine->WriteRegister(2, -1);
                delete filename;
                return;
            }
            printf("\nDelete file '%s' successfully", filename);
            machine->WriteRegister(2, 0);
            delete filename;
            return;
        }
        else
        {
            printf("\n File '%s' is opening, can't be deleted", filename);
            machine->WriteRegister(2, -1);
            return;
        }
        return;
    }
};

#endif