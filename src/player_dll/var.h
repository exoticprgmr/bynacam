//---------------------------------------------------------------------------
//
// Name:        var.h
// Author:      ex0ticprgmr 2020 & beziak 2009-2011
// Description: BynaCam is free Tibia movie recording & playing software.
//              BynaCam Reloaded v2 is using Tibia GUI engine
//              Designed exclusively for Tibia 8.6
// BynaCam Player is placed in public domain.
// Anyone is free to copy, modify, publish, use, compile, sell, or distribute the original BynaCam code, 
// either in source code form or as a compiled binary, for any purpose, commercial or non-commercial, and by any means.
//---------------------------------------------------------------------------


#ifndef _VAR_H_
#define _VAR_H_

#include <windows.h>

static DWORD CALL_GET_NEXT_PACKET = 0x0045C3A5; //8.6
static HWND* TIBIA_HWND = (HWND*) 0x0063DB24; //8.6
static DWORD CALL_PRINT_FPS = 0x45A258; //8.6
static DWORD NOP_PRINT_FPS = 0x45A194; //8.6
static DWORD SCROLLBAR_HOOK = 0x4D054D; //8.6
static DWORD STATUSMSG_NOP = 0x413B1E; //5 nops here
static DWORD WMCLOSE_HOOK = 0x45A9C6;

static int LoginServerStart = 0x7947F8; 
static int StepLoginServer = 112;
static int DistancePort = 100;
static int MaxLoginServers = 10;
        
static int XTeaKey = 0x7998BC;
        
#endif

