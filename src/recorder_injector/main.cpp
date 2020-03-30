//---------------------------------------------------------------------------
//
// Name:        core.cpp
// Author:      ex0ticprgmr 2020 & beziak 2009-2011
// Description: BynaCam is free Tibia movie recording & playing software.
//              BynaCam Reloaded v2 is using Tibia GUI engine
//              Designed exclusively for Tibia 8.6
// BynaCam Reloaded v2 is placed in public domain.
// Anyone is free to copy, modify, publish, use, compile, sell, or distribute the original BynaCam code, 
// either in source code form or as a compiled binary, for any purpose, commercial or non-commercial, and by any means.
//---------------------------------------------------------------------------


#include <windows.h>
#include <string>

using namespace std;
typedef HANDLE WINAPI (*_MyCreateRemoteThread)(HANDLE hProcess,LPSECURITY_ATTRIBUTES lpThreadAttributes,SIZE_T dwStackSize,LPTHREAD_START_ROUTINE lpStartAddress,LPVOID lpParameter,DWORD dwCreationFlags,LPDWORD lpThreadId);
_MyCreateRemoteThread MyCreateRemoteThread;

bool showerror(LPSTR function_name, LPSTR helptext)
{
    char *temp; 
    int errcode = GetLastError();
    if (errcode != 0)
    {
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, errcode, 0,
        (LPSTR)&temp, 0, NULL); 
        
        std::string str;
        str.append("Function: ");
        str.append(function_name);
        str.append("\r\nReason: ");
        str.append(temp);
        str.append("\r\n");
        str.append(helptext);
        LocalFree(temp);
        MessageBox(GetForegroundWindow(), str.c_str(), "BynaCam Loading Error!", MB_OK | MB_ICONERROR);
        return true;
    }
    return false;
}

BOOL InjectDLL(HANDLE hProcess, char *dllPath)
{
    LPVOID RemoteString, LoadLibPtr, CreateRemThreadPtr;
    WaitForInputIdle(hProcess, INFINITE);
    LoadLibPtr = (LPVOID)GetProcAddress(GetModuleHandle("kernel32"), "LoadLibraryA");
    CreateRemThreadPtr = (LPVOID)GetProcAddress(GetModuleHandle("kernel32"), "CreateRemoteThread");
    memcpy(&MyCreateRemoteThread, &CreateRemThreadPtr, 4);
    RemoteString = (LPVOID)VirtualAllocEx(hProcess, NULL, strlen(dllPath)+1, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    WriteProcessMemory(hProcess, (LPVOID)RemoteString, dllPath, strlen(dllPath), NULL);
    HANDLE Thread = MyCreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibPtr, (LPVOID)RemoteString, 0, NULL);
    if (Thread == NULL) { showerror("CreateRemoteThread()", "Unable to start remote thread.\r\nCheck your antivirus configuration."); return -1; }
    
    CloseHandle(hProcess);
    return true;
}

int main(int argc, char* argv[])
{
    strcpy(&argv[0][string(argv[0]).rfind("\\") + 1], "BynaCam.dll");
    
    HWND hWnd = FindWindow("TibiaClient", NULL);
    if (hWnd == NULL) { showerror("FindWindow()", "You should open Tibia client first."); return -1; }
    
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid == 0) { showerror("GetWindowThreadProcessId()", "Try to run BynaCam with administrator privileges."); return -1; }
      
    FILE* fDll = fopen(argv[0],"r");
    if (fDll == NULL) { MessageBox(GetForegroundWindow(), "BynaCam.dll not found!\r\nPlease reinstall an application!", "BynaCam Loading Error!", MB_OK | MB_ICONERROR); return -1; }
    fclose(fDll);
    
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (hProcess == NULL) { showerror("OpenProcess()", "Try to run BynaCam with administrator privileges."); return -1; }
    
    InjectDLL(hProcess, argv[0]);
    return EXIT_SUCCESS;
}
