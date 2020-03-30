//---------------------------------------------------------------------------
//
// Name:        dllmain.cpp
// Author:      ex0ticprgmr 2020 & beziak 2009-2011
// Description: BynaCam is free Tibia movie recording & playing software.
//              BynaCam Reloaded v2 is using Tibia GUI engine
//              Designed exclusively for Tibia 8.6
// BynaCam Player is placed in public domain.
// Anyone is free to copy, modify, publish, use, compile, sell, or distribute the original BynaCam code, 
// either in source code form or as a compiled binary, for any purpose, commercial or non-commercial, and by any means.
//---------------------------------------------------------------------------


#include <windows.h>
#include <sstream>
#include <process.h>
#include <math.h>
#include <float.h>
#include "var.h"
#include "connection.h"
#include "lzmadecoder.h"
#include "file.h"
#include "packetenumerator.h"

#define fequal(a,b) (fabs((a) - (b)) < FLT_EPSILON)

DWORD SCROLLBARHOOK_DST = 0x4D0554;
DWORD SCROLLBARHOOK_1 = 0x4C27B0;
DWORD SCROLLBARHOOK_2 = 0x473C80;
DWORD SCROLLBARHOOK_3 = 0x4C4110;
DWORD SCROLLBARHOOK_4 = 0x4C0C60;
DWORD SCROLLBARHOOK_5 = 0x47F0C0;
DWORD SCROLLBARHOOK_6 = 0x64F5C8;
DWORD SCROLLBAR_CMP_NOPADDR = 0x4C1415;
DWORD SCROLLBAR_CMP1_ADDR = SCROLLBAR_CMP_NOPADDR + 1;
DWORD SCROLLBAR_CMP2_ADDR = 0x4C413E + 1;
DWORD SCROLLBAR_CMP3_ADDR = 0x4C461E + 1;

DWORD MCADDR = 0x50BCA4;
DWORD MCVAL = 0xEB;

CHAR* camfilename;

extern "C" __declspec(dllexport) DWORD getmcaddr()
{
    return MCADDR;   
}       
extern "C" __declspec(dllexport) DWORD getmcvalue()
{
    return MCVAL;   
}    

typedef int __cdecl (TF_PRINTFPS)(DWORD surface, DWORD x, DWORD y, DWORD fontnumber, DWORD red, DWORD green, DWORD blue, CHAR* text, DWORD align);
TF_PRINTFPS* TfPrintFps;
typedef int TF_GETNEXTPACKET();
TF_GETNEXTPACKET* TfGetNextPacket;

int OnGetNextPacket();
DWORD WINAPI ThreadProc(LPVOID lpParameter);
void ModifyToken();
void SetMessageHandlerHook();
LRESULT APIENTRY TibiaMessageHander(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void Nop(DWORD address, int size);
void HookCall(DWORD dwCallAddress, DWORD dwNewAddress, LPDWORD pOldAddress);
void __cdecl MyPrintFps(DWORD surface, DWORD x, DWORD y, DWORD fontnumber, DWORD red, DWORD green, DWORD blue, CHAR* text, DWORD align);
void scrollbar_proc();
void WriteByte(DWORD address, BYTE byte);
void SetScrollBarHook();
bool PreparePacket(Socket* s, uint8_t* bytes, uint32_t len, uint32_t pTime, uint32_t cTime, bool nodelay);
void MyCamTime(int times, int* sec, int* min, int* hour);
extern "C" __declspec(noinline) void __stdcall dragupdate_proc();
extern "C" __declspec(noinline) void __stdcall dragpostupdate_proc();
void newHandler();
void __stdcall wmclose_proc(DWORD param);

WNDPROC wndProc;
short loginport = 43272, gameport = 43273;
uint8_t* camdata;
PacketEnumerator* packetEnum;
float speed = 1.0f;
int rewindto = 0;

/* scrollbar data
*/
DWORD data1[0xFFUL];
DWORD data2[0xFFUL];
DWORD data3[0xFFUL];
CHAR bool1 = 1;
DWORD data4[0xFFUL];

DWORD currentValue;
DWORD setValue = 0;
DWORD movieLen = 0;

DWORD textx = (DWORD)-1;
DWORD texty = (DWORD)-1;
//////////////////////////

DWORD startVal = 0; //35736B
DWORD cValue = 0;  //351541
DWORD sValue = 0; //35736F 
bool stop = false;
bool paused = false;

DWORD printTime = 0;

int OnGetNextPacket()
{
    int ret = TfGetNextPacket();
    if (ret == 0x0A)
       bool1 = 0;
    return ret;
}

void SetMessageHandlerHook()
{
    wndProc = (WNDPROC)SetWindowLongPtr(*TIBIA_HWND, GWLP_WNDPROC, (LONG)&TibiaMessageHander); // Set message pointer
}

void SetFpsHook()
{
    HookCall(CALL_PRINT_FPS, (DWORD)&MyPrintFps, (DWORD*)&TfPrintFps); 
    Nop(NOP_PRINT_FPS, 6);
}

void SetScrollBarHook()
{
    Nop(SCROLLBAR_CMP_NOPADDR, 1);  
    WriteByte(SCROLLBAR_CMP1_ADDR, 0x3B);  
    WriteByte(SCROLLBAR_CMP1_ADDR + 1, 0xDB);  
    WriteByte(SCROLLBAR_CMP2_ADDR, 0xDB);  
    WriteByte(SCROLLBAR_CMP3_ADDR, 0xDB);  
  
    WriteByte(SCROLLBAR_HOOK, 0xE9);  
    HookCall(SCROLLBAR_HOOK, (DWORD)&scrollbar_proc, NULL);    
}

void WriteByte(DWORD address, BYTE byte)
{
    DWORD dwOldProtect, dwNewProtect;
    VirtualProtectEx(GetCurrentProcess(), (LPVOID)(address), 1, PAGE_EXECUTE_WRITECOPY, &dwOldProtect);
    memset((LPVOID)(address), byte, 1);
    VirtualProtectEx(GetCurrentProcess(), (LPVOID)(address), 1, dwOldProtect, &dwNewProtect);
}

void Nop(DWORD address, int size)
{
    DWORD dwOldProtect, dwNewProtect;
    VirtualProtectEx(GetCurrentProcess(), (LPVOID)(address), 1, PAGE_EXECUTE_WRITECOPY, &dwOldProtect);
    memset((LPVOID)(address), 0x90, size);
    VirtualProtectEx(GetCurrentProcess(), (LPVOID)(address), 1, dwOldProtect, &dwNewProtect);
}

void HookCall(DWORD dwCallAddress, DWORD dwNewAddress, LPDWORD pOldAddress)
{
	DWORD dwOldProtect, dwNewProtect, dwOldCall, dwNewCall;
	BYTE call[4];

	dwNewCall = dwNewAddress - dwCallAddress - 5;
	*(DWORD*)call = dwNewCall;

	VirtualProtectEx(GetCurrentProcess(), (LPVOID)(dwCallAddress + 1), 4, PAGE_EXECUTE_WRITECOPY, &dwOldProtect);
	if(pOldAddress)
	{
		memcpy(&dwOldCall, (LPVOID)(dwCallAddress+1), 4);
		*pOldAddress = dwCallAddress + dwOldCall + 5;
	}
	dwCallAddress += 1;
	*(DWORD*)dwCallAddress = *(DWORD*)&call;
	VirtualProtectEx(GetCurrentProcess(), (LPVOID)(dwCallAddress), 5, dwOldProtect, &dwNewProtect);
}

bool GetCamData(const std::string filen)
{
    File* file = new File(filen); 
    if (!file->IsGood())
       return false;
       
    uint8_t* buffer = file->GetBuffer();
    buffer += 0x24;
    uint32_t tsize = *(uint32_t*)buffer;
    buffer += (tsize + 4);
    uint32_t csize = *(uint32_t*)buffer;
    buffer += 4;
    uint32_t outLen = *(uint32_t*)(buffer + 5);
    LzmaDecoder* decoder = new LzmaDecoder((uint8_t*)(buffer + 5 + 8), buffer, csize, outLen);
    camdata = new uint8_t[outLen];
    memcpy(camdata, decoder->GetBuffer(), outLen);
    delete decoder;
    delete file;
    return true;
}

void CreateLoginServer(void* val)
{
    SocketServer in(loginport,5);
    while (true)
    {//INFINITY =D
          Socket* s=in.Accept();
          while (s->ReceiveBytes().length() == 0)
                Sleep(10);
          
          Connection::SendLoginPacket(s, gameport, (long*)XTeaKey);
          s->Close();
    }
}

void CreateGameServer(void* val)
{
    static unsigned char FirstPacket[14] = {0x0C, 0x00, 0x32, 0x02, 0x86, 0x07, 0x06, 0x00, 0x1F, 0x95, 0xBC, 0x00, 0x00, 0xBB};
    SocketServer game(gameport,5);
    Socket* s = game.Accept();
    std::string buffer;
    buffer.append((const char*)FirstPacket, sizeof(FirstPacket));    
    s->SendBytes(buffer);
    while (s->ReceiveBytes().length() == 0)
        Sleep(10);  
    
    uint32_t pTime;
    uint32_t outLen;
    uint8_t* dp;
    uint32_t cTime; 
    bool nodelay = false;
    
    while (true)
    {   
        if (rewindto != 0)
        {       
             if (nodelay == false)     
             {   
                 if(rewindto <= cTime)
                 {      
                       packetEnum->offset = 6;  
                       packetEnum->curpacket = 0;
                       packetEnum->curtime = 0;
                       cTime = 0;
                 }
                 nodelay = true;  
             }
             if (cTime >= rewindto)
             {
                   nodelay = false;
                   rewindto = 0;    
             }
        }
        pTime = packetEnum->GetCurrentTime();
        dp = packetEnum->GetNextPacket(&outLen);
        cTime = packetEnum->GetCurrentTime();
        PreparePacket(s, dp, outLen, pTime, cTime, nodelay);
        printTime = ((rewindto != 0) ? rewindto : cTime);
        
        while (stop || paused)
            Sleep(10);
            
        cValue = (printTime >> 6);
    }
}

bool PreparePacket(Socket* s, uint8_t* bytes, uint32_t len, uint32_t pTime, uint32_t cTime, bool nodelay)
{
     if (bytes == NULL)
     {
         printTime = packetEnum->time;  
         cValue = (printTime >> 6);   
         Sleep(4000);  
         TerminateProcess(GetCurrentProcess(), 0);
     }
     static uint8_t* zeros[6] = {0,0,0,0,0,0}; 
     NetworkMessage* msg = new NetworkMessage();
     msg->WriteBytes((char*)zeros, 6);
     msg->WriteInt16(len);
     msg->WriteBytes((char*)bytes, len);
     msg->PrepareToSend((long*)XTeaKey);
     
     std::string buffer;
     buffer.append((const char*)msg->data, msg->offset);    
     s->SendBytes(buffer);
     delete msg;
     
     if (!nodelay)
     {
        int delay = (DWORD)((cTime - pTime) / speed);
        if (delay > 1)
        {
           int min = 100;
           float oldspeed = speed;
           while (true)
           {
              if ((float)oldspeed != (float)speed || rewindto != 0)
              break;  
              if (delay > min)  
              {
               Sleep(min);
               delay -= min;
               continue;
              }  
              Sleep(delay);
              break;
           }
        }
     }
     return true;
}

void __stdcall wmclose_proc(DWORD param)
{
    TerminateProcess(GetCurrentProcess(), 0); 
}

DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
    HookCall(CALL_GET_NEXT_PACKET, (DWORD)&OnGetNextPacket, (LPDWORD)&TfGetNextPacket);  
    HookCall(WMCLOSE_HOOK, (DWORD)&wmclose_proc, NULL);  
    Nop(STATUSMSG_NOP, 5);  
    SetMessageHandlerHook();  
    SetFpsHook();
    SetScrollBarHook();
    Connection::SetLoginServers(loginport);
    if (!GetCamData(camfilename))
    {
        MessageBox(GetForegroundWindow(), "BynaCam could not open file!!", "Critical Error!", MB_OK | MB_ICONSTOP);
        TerminateProcess(GetCurrentProcess(), 1);                       
    }
    packetEnum = new PacketEnumerator(camdata); /// PACKET ENUMERATOR
    movieLen = packetEnum->GetTotalTime();
    
    _beginthread(CreateLoginServer, 0, NULL);
    _beginthread(CreateGameServer, 0, NULL);
    Sleep(1000);                   
    Connection::AutoLogin();
    return 0;
}

void ModifyToken()
{
    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;
    if(OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
    {
        LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tkp.Privileges[0].Luid);
        tkp.PrivilegeCount = 1;
        tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        AdjustTokenPrivileges(hToken, 0, &tkp, sizeof(tkp), NULL, NULL);
    }
}

LRESULT APIENTRY TibiaMessageHander(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
          case WM_TIMER:
               {
                     static clock_t cl = clock(); 
                     int myCamTime = ((sValue << 6) / 1000); // Calcualte total running time
                     int tSec = myCamTime % 60;
                     int tMin = (myCamTime / 60) % 60;
                     int tHour = (myCamTime / 60) / 60;
                     int cCamTime = (movieLen / 1000); // Calcualte total running time
                     int cSec = cCamTime % 60;
                     int cMin = (cCamTime / 60) % 60;
                     int cHour = (cCamTime / 60) / 60;
                     static char title[1024];
                     sprintf(title, "BynaCam - %gx - [%02d:%02d:%02d / %02d:%02d:%02d]", speed, tHour, tMin, tSec, cHour, cMin, cSec);
                     SetWindowText(*TIBIA_HWND, title);
                     break;   
               } 
          case WM_KEYDOWN:
               {
                     if (wParam == VK_PAUSE)
                     {
                          paused = !paused;      
                     }     
                     if (wParam == VK_BACK)
                     {    
                          if (rewindto == 0)  
                          {   
                             if (printTime > 30000) 
                                rewindto = printTime - 30000;
                             else
                                rewindto = 1;
                          }
                     } 
                     if (wParam == VK_UP)
                     {         
                        speed = 50.0f;
                     } 
                     else if (wParam == VK_DOWN)
                     {
                        speed = 1.0f;
                     }
                     else if (wParam == VK_RIGHT)
                     {
                        if (speed >= 1.0f && speed < 50.0f) 
                        { 
                             speed += 1.0f;
                        }
                        else if(speed >= 0.1f && speed < 1.0f)
                        {
                             speed += 0.1f;
                        }
                     }
                     else if (wParam == VK_LEFT)
                     {
                          if (speed > 1.0f)
                             speed -= 1.0f;
                          else if (speed <= 1.0f)
                          {
                             if (fequal(speed, 0.2f))  
                                speed = 0.1f;
                             else if (!fequal(speed, 0.1f))
                                speed -= 0.1f;   
                          }
                     }
                        
                     if (wParam == VK_UP || wParam == VK_DOWN || wParam == VK_RIGHT || wParam == VK_LEFT || wParam == VK_BACK)
                        return DefWindowProc(hWnd, uMsg, wParam, lParam);
                     break;     
               }     
    }

    return CallWindowProc(wndProc, hWnd, uMsg, wParam, lParam ); // return the message
} 

void __cdecl MyPrintFps(DWORD surface, DWORD x, DWORD y, DWORD fontnumber, DWORD red, DWORD green, DWORD blue, CHAR* text, DWORD align)
{
    if (textx == -1 && texty == -1)
       return;
        
    int myCamTime = ((sValue << 6) / 1000); // Calcualte total running time
    int tSec = myCamTime % 60;
    int tMin = (myCamTime / 60) % 60;
    int tHour = (myCamTime / 60) / 60;
    static char text2[128];
    sprintf(text2, "%02d:%02d:%02d", tHour, tMin, tSec);
    TfPrintFps(surface, textx, texty, fontnumber, 0xFF, 0xFF, 0xFF, text2, align);
}

clock_t cl = 0;
DWORD oldcval = 0xFFFFFFFF; 
DWORD ddtime = 500;
extern "C" __declspec(noinline)void __stdcall dragupdate_proc()
{                
        if (startVal <= currentValue)
            ddtime = 0;    
        else
            ddtime = 500;
            
        stop = true;
        printTime = (cValue << 6);   
        cValue = currentValue;
        
        if (cValue != oldcval)
        {      
             oldcval = cValue;
             cl = clock();
        }
} 

extern "C" __declspec(noinline) void __stdcall dragpostupdate_proc()  
{
       if (stop && (clock() - cl) > ddtime)
       {
             rewindto = (oldcval << 6);
             if (rewindto == 0) rewindto = 1;
             cl = 0;
             oldcval = 0xFFFFFFFF;  
             stop = false;
       }
}


void scrollbar_proc() 
{           
     __asm(".intel_syntax noprefix \n"
           "pushad \n"
           "mov %esi, _SCROLLBARHOOK_6 \n"
           "mov %esi, dword ptr[%esi] \n"
           "mov %esi, dword ptr[%esi+0x30] \n"
           "mov _data1, %esi \n"
           "cmp byte ptr[_bool1], 0 \n"
           "jnz redraw \n"
           "mov byte ptr[_bool1], 1 \n"
           "mov %edi, dword ptr[%esi+0x24] \n"
           "mov _data2, %edi \n"
           "push %esi \n"
           "push 0x0f \n"
           "push %esi \n"
           "push 1 \n"
           "push 0x1d4 \n"
           "push _startVal \n"
           "mov %eax, _movieLen \n"
           "shr %eax, 6\n"
           "push %eax \n"
           "push 0 \n"
           "mov %ecx, offset _data3 \n"
           "mov %eax, _SCROLLBARHOOK_5 \n"
           "call %eax \n"
           "mov dword ptr[%esi+0x54], %eax \n"
           "mov _data4, %eax \n"
           "push %eax \n"
           "mov %ecx, %esi \n"
           "mov %eax, _SCROLLBARHOOK_4 \n"
           "call %eax \n"
           "pop %esi \n"   
           "redraw: \n"      
           "sub esp,0x10 \n"
           "mov %eax, %esp \n"
           "mov dword ptr[%eax], 0 \n"
           "mov %ecx, dword ptr[%esi+0x20] \n"
           "sub %ecx,0x0C \n"
           "mov dword ptr[%eax+0x4],%ecx \n"
           "sub %ecx,5 \n"
           "mov %edi, _data2 \n"
           "mov dword ptr[%edi+0x20],%ecx \n"
           "mov dword ptr[%eax+0x8],0 \n"
           "mov dword ptr[%eax+0xC],0x0C \n"
           "mov %eax, _data4 \n"
           "push %eax \n"
           "mov %ecx, %esi \n"
           "mov %edx, _SCROLLBARHOOK_3 \n"
           "call %edx \n"
           //////////////////////////////////////////     
           "mov %ecx,_data4 \n"
           "mov %ecx,dword ptr[%ecx+0x2C] \n"
           "mov %eax,dword ptr[%ecx+0x30] \n"
           "mov _currentValue, %eax \n"
           "CMP %EAX, DWORD PTR[_startVal] \n"
           "JE l1 \n"
           "pushad \n"
           "call _dragupdate_proc\n"
           "popad \n"
           "l1: \n"
           "pushad \n"
           "call _dragpostupdate_proc \n"
           "popad \n"
           "MOV %EAX, _cValue \n"
           "MOV _startVal, %EAX \n"
           "MOV _sValue, %EAX \n"
           "push %eax \n"
           "mov %eax,_SCROLLBARHOOK_2 \n"
           "call %eax \n"
           "mov %ecx,%esi \n"
           "mov %eax,_SCROLLBARHOOK_1 \n"
           "call %eax \n"
           "popad \n"
           "pop %ebp \n"
           "push %eax \n"
           "call dword ptr[%edx+0x44] \n"
           "mov %esi, dword ptr[%ESI+0x44] \n"           
           //calculating text position
           "pushad \n"
           "MOV %ESI, _data1 \n"
           "MOV %EAX,DWORD PTR DS:[%ESI+0x14] \n"
           "ADD %EAX,DWORD PTR DS:[%ESI+0x1C] \n"
           "SUB %EAX,0x25 \n"
           "MUL DWORD PTR[_sValue] \n"
           "MOV %ECX, _movieLen \n"
           "SHR %ECX, 0x6 \n"
           "DIV %ECX \n"
           "MOV %ECX,DWORD PTR[%ESI+0x18] \n"
           "ADD %ECX,DWORD PTR[%ESI+0x20] \n"
           "ADD %EAX,0x0C \n"
           "SUB %ECX,0x18 \n"
           "MOV _textx, %EAX \n"
           "MOV _texty, %ECX \n"
           "popad \n"
           "mov %eax, _SCROLLBARHOOK_DST \n"
           "jmp eax \n"
           ".att_syntax \n");
}

void newHandler()
{
     MessageBox(GetForegroundWindow(), "Out of memory!!", "Critical Error!", MB_OK | MB_ICONSTOP);
     TerminateProcess(GetCurrentProcess(), 1);
}

extern "C" BOOL APIENTRY DllMain (HINSTANCE hInst     /* Library instance handle. */ ,
                       DWORD reason        /* Reason this function is being called. */ ,
                       LPVOID reserved     /* Not used. */ )
{
    switch (reason)
    {
      case DLL_PROCESS_ATTACH:
      {
            camfilename = GetCommandLineA();
            set_new_handler(newHandler);
            ModifyToken(); 
            if (!CreateThread(NULL,0,ThreadProc,NULL,0,NULL))
            {
                  MessageBox(0, "Error while creating thread!\r\nCreateThread failed.", "Fatal Error!", MB_ICONERROR | MB_OK);
                  return FALSE;
            }                  
            break;
      }
      case DLL_PROCESS_DETACH:
        break;

      case DLL_THREAD_ATTACH:
        break;

      case DLL_THREAD_DETACH:
        break;
    }

    /* Returns TRUE on success, FALSE on failure */
    return TRUE;
}
