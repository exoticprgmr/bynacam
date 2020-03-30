//---------------------------------------------------------------------------
//
// Name:        core.h
// Author:      ex0ticprgmr 2020 & beziak 2009-2011
// Description: BynaCam is free Tibia movie recording & playing software.
//              BynaCam Reloaded v2 is using Tibia GUI engine
//              Designed exclusively for Tibia 8.6
// BynaCam Reloaded v2 is placed in public domain.
// Anyone is free to copy, modify, publish, use, compile, sell, or distribute the original BynaCam code, 
// either in source code form or as a compiled binary, for any purpose, commercial or non-commercial, and by any means.
//---------------------------------------------------------------------------

#ifndef _CORE_H_
#define _CORE_H_

#define _WIN32_WINNT 0x0500

#include <windows.h>
#include <windowsx.h>
#include <wincrypt.h>
#include <process.h>
#include <stdint.h>
#include <sstream>
#include <shlwapi.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <tlhelp32.h>
#include "var.h"
#include "hooks.h"
#include "networkmessage.h"
#include "file.h"
#include "mappacket.h"
#include "md5.h"

#ifndef MAKEULONGLONG
#define MAKEULONGLONG(ldw, hdw) ((ULONGLONG(hdw) << 32) | ((ldw) & 0xFFFFFFFF))
#endif
  
#ifndef MAXULONGLONG
#define MAXULONGLONG ((ULONGLONG)~((ULONGLONG)0))
#endif

WNDPROC wndProc;

TF_DRAWSKIN *TfDrawSkin;
TF_PRINTFPS *TfPrintFps;
TF_EXITTIBIA *TfExitTibia;
TF_POSTPRINTFPS *TfPostPrintFps;
TF_SAYTOPLAYER *TfSayToPlayer;
TF_SAYTONPC *TfSayToNpc;

DWORD printTime = 0;
DWORD recState = RECSTATE_READY;
BOOL mouseDown = false;
DWORD relativeX = 500;
std::string hash;
NetworkMessage *msg = new NetworkMessage(65535UL); //main msg buffer
FileHandler* file = new FileHandler();
bool bGenMap = false;
bool bRecording = false;
HINSTANCE hInstance;
HMENU hMenu;
int mouseX;
int mouseY;
bool showCensor = false;
bool showGui = true;
time_t lastGuiCtrlTab = 0;
bool exiting = false;

const DWORD normalBtnId = 60;
const DWORD clickedId = 157; //158
const DWORD checkedId = 78;//159
const DWORD unCheckedId = 74;//160 todo
DWORD currentNewFileBtnId = 60; 

bool hideDefChannel = false; //
bool hideIncPrivMsg = false; //
bool hideAllChannels = false; //
bool hideVip = false; //
bool hideBackpacks = false; //
bool hideSkills = false; // 
bool hideEq = false; //
bool hideSentMsg = false; //
bool hideChannelList = true; //

void SetAddresses();
void Unload();
void DisableHooks();
int __cdecl OnSayToPlayer(DWORD type, CHAR* receiver, CHAR* message);
int __cdecl OnSayToNpc(DWORD type, CHAR* message);
bool CheckPacket(LPBYTE data, DWORD len);
void NewFileButtonProc(void *dummy);
DWORD GetMainThreadId();
void StopButtonProc(void *dummy);
void GenerateMapPacket();
int OnGetNextPacket();
void OnButtonClick(ButtonType_t newtype);
LRESULT APIENTRY TibiaMessageHander(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void EnableHooks();
uint32_t GetMainYPos();
int _stdcall MyPostPrintFps();
void __stdcall MyExitTibia(DWORD exitvalue);
TF_GETNEXTPACKET *TfGetNextPacket;
void StartApp();

#endif
