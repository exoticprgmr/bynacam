//---------------------------------------------------------------------------
//
// Name:        hooks.h
// Author:      ex0ticprgmr 2020 & beziak 2009-2011
// Description: BynaCam is free Tibia movie recording & playing software.
//              BynaCam Reloaded v2 is using Tibia GUI engine
//              Designed exclusively for Tibia 8.6
// BynaCam Reloaded v2 is placed in public domain.
// Anyone is free to copy, modify, publish, use, compile, sell, or distribute the original BynaCam code, 
// either in source code form or as a compiled binary, for any purpose, commercial or non-commercial, and by any means.
//---------------------------------------------------------------------------

#ifndef _HOOKS_H_
#define _HOOKS_H_

#include <windows.h>

class Hooks
{
      public:
             static void Nop(DWORD address, int size)
             {
                    DWORD dwOldProtect, dwNewProtect;
                    VirtualProtect((LPVOID)(address), 1, PAGE_EXECUTE_WRITECOPY, &dwOldProtect);
                    memset((LPVOID)(address), 0x90, size);
                    VirtualProtect((LPVOID)(address), 1, dwOldProtect, &dwNewProtect);
             }

             static void HookCall(DWORD dwCallAddress, DWORD dwNewAddress, LPDWORD pOldAddress)
             {
	                DWORD dwOldProtect, dwNewProtect, dwOldCall, dwNewCall;
	                BYTE call[4];

	                dwNewCall = dwNewAddress - dwCallAddress - 5;
	                *(DWORD*)call = dwNewCall;

	                VirtualProtect((LPVOID)(dwCallAddress + 1), 4, PAGE_EXECUTE_WRITECOPY, &dwOldProtect);
	                if(pOldAddress)
                    {
		             memcpy(&dwOldCall, (LPVOID)(dwCallAddress+1), 4);
		             *pOldAddress = dwCallAddress + dwOldCall + 5;
	                }
	                dwCallAddress += 1;
	                *(DWORD*)dwCallAddress = *(DWORD*)&call;
	                VirtualProtect((LPVOID)(dwCallAddress), 5, dwOldProtect, &dwNewProtect);
             }
             static void WriteBytes(DWORD address, LPBYTE bytes, DWORD size)
             {
                    DWORD dwOldProtect, dwNewProtect;
	                VirtualProtect((LPVOID)address, size, PAGE_EXECUTE_WRITECOPY, &dwOldProtect);
		            memcpy((LPVOID)address, bytes, size);
	                VirtualProtect((LPVOID)address, size, dwOldProtect, &dwNewProtect);
             }
};

#endif
