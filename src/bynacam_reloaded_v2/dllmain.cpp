//---------------------------------------------------------------------------
//
// Name:        dllmain.cpp
// Author:      ex0ticprgmr 2020 & beziak 2009-2011
// Description: BynaCam is free Tibia movie recording & playing software.
//              BynaCam Reloaded v2 is using Tibia GUI engine
//              Designed exclusively for Tibia 8.6
// BynaCam Reloaded v2 is placed in public domain.
// Anyone is free to copy, modify, publish, use, compile, sell, or distribute the original BynaCam code, 
// either in source code form or as a compiled binary, for any purpose, commercial or non-commercial, and by any means.
//---------------------------------------------------------------------------

#include <windows.h>
#include <process.h>

extern void StartApp();//core.cpp
extern HINSTANCE hInstance;

__cdecl void MainThreadProc(void* unknown)
{
     StartApp(); //core.cpp
}

extern "C" BOOL APIENTRY DllMain (HINSTANCE hInst     /* Library instance handle. */ ,
                       DWORD reason        /* Reason this function is being called. */ ,
                       LPVOID reserved     /* Not used. */ )
{
    switch (reason)
    {
      case DLL_PROCESS_ATTACH:
        {           
           hInstance = hInst;             
           _beginthread(MainThreadProc, 0, NULL);
            break;
        }

      case DLL_PROCESS_DETACH:
        break;
    }

    /* Returns TRUE on success, FALSE on failure */
    return TRUE;
}
