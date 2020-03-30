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

#include "core.h"

void StartApp()
{
     EnableHooks();
     //SetWindowText(*(HWND*)(TIBIA_HWND_ADDR), PROGRAM_NAME);
}

uint32_t GetMainYPos()
{
    DWORD yPos = GUI_POINTER_ADDR;
    yPos = *(DWORD*)yPos;
    yPos = *(DWORD*)(yPos + 0x30);
    if (yPos == 0)
       return 0;
    
    relativeX = (*(DWORD*)(yPos + 0x1C)) - 355;
    yPos = *(DWORD*)(yPos + 0x20);
    
    return yPos - 38;
}

void Unload() //never called : todo- wndproc SetWindowLongPtr back
{
    DisableHooks();
    FreeLibraryAndExitThread(hInstance, 0);
}

void DisableHooks()
{
    DWORD mainThreadId = GetMainThreadId();    
    HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME,
                                    TRUE, mainThreadId);
    if (hThread != NULL)
       SuspendThread(hThread);
       
     Hooks::HookCall(TIBIA_EXIT_ADDR, (DWORD)TfExitTibia, NULL);        
     Hooks::HookCall(CALL_POSTPRINT_FPS_ADDR, (DWORD)TfPostPrintFps, NULL); 
     Hooks::HookCall(CALL_SAY_TO_PLAYER_ADDR, (DWORD)TfSayToPlayer, NULL);
     Hooks::HookCall(CALL_SAY_TO_NPC_ADDR, (DWORD)TfSayToNpc, NULL);
     Hooks::HookCall(CALL_GET_NEXT_PACKET_ADDR, (DWORD)TfGetNextPacket, NULL);
     
    if (hThread != NULL)
    {
       ResumeThread(hThread);
       CloseHandle(hThread);
    }
}

void EnableHooks()
{
    DWORD mainThreadId = GetMainThreadId();    
    HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME,
                                    TRUE, mainThreadId);
    if (hThread != NULL)
       SuspendThread(hThread);
       
    wndProc = (WNDPROC)SetWindowLongPtr(*(HWND*)(TIBIA_HWND_ADDR), GWLP_WNDPROC, (LONG)&TibiaMessageHander); // Set message pointer
     
     TfPrintFps = (TF_PRINTFPS*) PRINTFPS_FUNC_ADDR;
     TfDrawSkin = (TF_DRAWSKIN *) DRAWSKIN_FUNC_ADDR;
     Hooks::HookCall(TIBIA_EXIT_ADDR, (DWORD)&MyExitTibia, (LPDWORD) &TfExitTibia); 
     Hooks::HookCall(CALL_POSTPRINT_FPS_ADDR, (DWORD)&MyPostPrintFps, (LPDWORD) &TfPostPrintFps); 
     Hooks::HookCall(CALL_SAY_TO_PLAYER_ADDR, (DWORD)&OnSayToPlayer, (LPDWORD)&TfSayToPlayer);
     Hooks::HookCall(CALL_SAY_TO_NPC_ADDR, (DWORD)&OnSayToNpc, (LPDWORD)&TfSayToNpc);
     Hooks::HookCall(CALL_GET_NEXT_PACKET_ADDR, (DWORD)&OnGetNextPacket, (LPDWORD)&TfGetNextPacket);

    if (hThread != NULL)
    {
       ResumeThread(hThread);
       CloseHandle(hThread);
    }
}

LRESULT APIENTRY TibiaMessageHander(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (lastGuiCtrlTab + 300 > clock() || GetMainYPos() == 0)
        return CallWindowProc(wndProc, hWnd, uMsg, wParam, lParam ); // return the message
    
    if(wParam == VK_TAB)
    { 
        if (GetKeyState(VK_CONTROL) & 0x80) // -- ctrl
        {
            if (lastGuiCtrlTab + 300 < clock())
                showGui = !showGui; //time control????
            
            lastGuiCtrlTab = clock();
                
            return CallWindowProc(wndProc, hWnd, uMsg, wParam, lParam ); // return the message
        }
    }
    
    if (!showGui)
        return CallWindowProc(wndProc, hWnd, uMsg, wParam, lParam ); // return the message
            
    if (uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONDBLCLK)
    {     
          DWORD mYPos = GetMainYPos();      
          if (mYPos == 0) return CallWindowProc(wndProc, hWnd, uMsg, wParam, lParam ); // return the message
                        
          DWORD xPos = GET_X_LPARAM(lParam); 
          DWORD yPos = GET_Y_LPARAM(lParam); 
          mouseX = xPos;
          mouseY = yPos;
                     
          if (xPos >= 193 + relativeX && xPos <= 193 + 90 + relativeX && yPos >= mYPos + 5 && yPos <= mYPos + 5 + 12)
          {
             OnButtonClick(BUTTONTYPE_STARTPAUSE);
             return 0;    
          }
          if (xPos >= 193 + relativeX && xPos <= 193 + 90 + relativeX && yPos >= mYPos + 5 + 12 + 4 && yPos <= mYPos + 5 + 12 + 12 + 4)
          {
             OnButtonClick(BUTTONTYPE_STOP);
             return 0;   
          }    
          if (xPos >= 286 + relativeX && xPos <= 286 + 65 + relativeX && yPos >= mYPos + 5 && yPos <= mYPos + 5 + 12)
          {
             OnButtonClick(BUTTONTYPE_NEWFILE);
             return 0;   
          }
          if (xPos >= 286 + relativeX && xPos <= 286 + 65 + relativeX && yPos >= mYPos + 5 + 12 + 4 && yPos <= mYPos + 5 + 12 + 12 + 4)
          {
             OnButtonClick(BUTTONTYPE_CENSOR);
             return 0;   
          }
                        
          /////////////////////censor buttons here
          if (showCensor && recState != RECSTATE_RECORDING)
          {
             if (xPos >= 286 + relativeX && xPos <= 286 + 200 + relativeX && yPos >= mYPos - 12 && yPos <= mYPos)
             {
                OnButtonClick(BUTTONTYPE_CENSOR_HIDEDEFAULT);
                return 0;  
             }   
             if (xPos >= 286 + relativeX && xPos <= 286 + 200 + relativeX && yPos >= mYPos - 12 - 12 - 1 && yPos <= mYPos - 12 - 12 - 1 + 12)
             {
                OnButtonClick(BUTTONTYPE_CENSOR_HIDEINCOMINGPRIVATE);
                return 0;   
             }   
             if (xPos >= 286 + relativeX && xPos <= 286 + 200 + relativeX && yPos >= mYPos - 12 - 12 - 12 - 2 && yPos <= mYPos - 12 - 12 - 12 - 2 + 12)
             {
                OnButtonClick(BUTTONTYPE_CENSOR_HIDEALLCHANNELS);
                return 0;   
             } 
             if (xPos >= 286 + relativeX && xPos <= 286 + 200 + relativeX && yPos >= mYPos - 12 - 12 - 12 - 12 - 3 && yPos <= mYPos - 12 - 12 - 12 - 12 - 3 + 12)
             {
                OnButtonClick(BUTTONTYPE_CENSOR_HIDEVIP);
                return 0;   
             } 
             if (xPos >= 286 + relativeX && xPos <= 286 + 200 + relativeX && yPos >= mYPos - 12 - 12 - 12 - 12 - 12 - 4 && yPos <= mYPos - 12 - 12 - 12 - 12 - 12 - 4 + 12)
             {
                OnButtonClick(BUTTONTYPE_CENSOR_HIDEBACKPACKS);
                return 0;   
             } 
             if (xPos >= 286 + relativeX && xPos <= 286 + 200 + relativeX && yPos >= mYPos - 12 - 12 - 12 - 12 - 12 - 12 - 5 && yPos <= mYPos - 12 - 12 - 12 - 12 - 12 - 12 - 5 + 12)
             {
                OnButtonClick(BUTTONTYPE_CENSOR_HIDESKILLS);
                return 0;   
             } 
             if (xPos >= 286 + relativeX && xPos <= 286 + 200 + relativeX && yPos >= mYPos - 12 - 12 - 12 - 12 - 12 - 12 - 12 - 6 && yPos <= mYPos - 12 - 12 - 12 - 12 - 12 - 12 - 12 - 6 + 12)
             {
                OnButtonClick(BUTTONTYPE_CENSOR_HIDEEQ);
                return 0;   
             } 
             if (xPos >= 286 + relativeX && xPos <= 286 + 200 + relativeX && yPos >= mYPos - 12 - 12 - 12 - 12 - 12 - 12 - 12 - 12 - 7 && yPos <= mYPos - 12 - 12 - 12 - 12 - 12 - 12 - 12 - 12 - 7 + 12)
             {
                OnButtonClick(BUTTONTYPE_CENSOR_HIDESENTMESSAGES);
                return 0;   
             } 
             if (xPos >= 286 + relativeX && xPos <= 286 + 200 + relativeX && yPos >= mYPos - 12 - 12 - 12 - 12 - 12 - 12 - 12 - 12 - 12 - 8 && yPos <= mYPos - 12 - 12 - 12 - 12 - 12 - 12 - 12 - 12 - 12 - 8 + 12)
             {
                OnButtonClick(BUTTONTYPE_CENSOR_HIDECHANNELLIST);
                return 0;   
             }                      
          }
    }

    return CallWindowProc(wndProc, hWnd, uMsg, wParam, lParam ); // return the message
} 

void __stdcall MyExitTibia(DWORD exitvalue)
{
    exiting = true;
    StopButtonProc(NULL);
    return TfExitTibia(exitvalue); //never be called
}

int __stdcall MyPostPrintFps()
{            
     int surface = 1, red = 0xc8, green = 0xc8, blue = 0xc8, align = 0, fontnumber = 2;
     
    DWORD yPos = GetMainYPos();
    if (yPos == 0)
    {      
      TfDrawSkin(surface, 15, 19, 170, 15, 60, 0,0);
      TfPrintFps(surface, 17, 21, fontnumber, 0, 255, 150, "BYNACAM \xbb PLEASE LOG IN !!!", align);
      return TfPostPrintFps();
    }
    
    TfPrintFps(1, 210 + relativeX, yPos + 34 , 3, red, green, blue, (CHAR*)PROGRAM_NAME, align);
    TfPrintFps(1, 50 + relativeX, yPos + 34 , 3, red, green, blue, "CTRL + TAB - Show/Hide", align);
    
    if (!showGui)
        return TfPostPrintFps();
        
    if (showCensor && recState != RECSTATE_RECORDING)
    {
         //censor draw here
         TfDrawSkin(surface, 286 + relativeX, yPos - 12 - 12 - 12 - 12 - 12 - 12 - 12 - 12 - 12 - 8, 200, 12 * 9 + 9, normalBtnId, 0,0);

         TfDrawSkin(1, 288 + relativeX, yPos - 12, 12, 12, (hideDefChannel) ? checkedId : unCheckedId, 0, 0);
         TfPrintFps(1, 288 + 14 + relativeX, yPos - 12, 2, red, green, blue, "Default Channel", align);
         
         TfDrawSkin(1, 288 + relativeX, yPos - 12 - 12 - 1, 12, 12, (hideIncPrivMsg) ? checkedId : unCheckedId, 0, 0);
         TfPrintFps(1, 288 + 14 + relativeX, yPos - 12 - 12 - 1, 2, red, green, blue, "Incoming Private Messages", align);
         
         TfDrawSkin(1, 288 + relativeX, yPos - 12 - 12 - 12 - 2, 12, 12, (hideAllChannels) ? checkedId : unCheckedId, 0, 0);
         TfPrintFps(1, 288 + 14 + relativeX, yPos - 11 - 12 - 12 - 2, 2, red, green, blue, "All Channels", align);
         
         TfDrawSkin(1, 288 + relativeX, yPos - 12 - 12 - 12 - 12 - 3, 12, 12, (hideVip) ? checkedId : unCheckedId, 0, 0);
         TfPrintFps(1, 288 + 14 + relativeX, yPos - 11 - 12 - 12 - 12 - 3, 2, red, green, blue, "VIP List", align);
         
         TfDrawSkin(1, 288 + relativeX, yPos - 12 - 12 - 12 - 12 - 12 - 4, 12, 12, (hideBackpacks) ? checkedId : unCheckedId, 0, 0);
         TfPrintFps(1, 288 + 14 + relativeX, yPos - 11 - 12 - 12 - 12 - 12 - 4, 2, red, green, blue, "Backpacks", align);
         
         TfDrawSkin(1, 288 + relativeX, yPos - 12 - 12 - 12 - 12 - 12 - 12 - 5, 12, 12, (hideSkills) ? checkedId : unCheckedId, 0, 0);
         TfPrintFps(1, 288 + 14 + relativeX, yPos - 11 - 12 - 12 - 12 - 12 - 12 - 5, 2, red, green, blue, "Skills", align);
         
         TfDrawSkin(1, 288 + relativeX, yPos - 12 - 12 - 12 - 12 - 12 - 12 - 12 - 6, 12, 12, (hideEq) ? checkedId : unCheckedId, 0, 0);
         TfPrintFps(1, 288 + 14 + relativeX, yPos - 11 - 12 - 12 - 12 - 12 - 12 - 12 - 6, 2, red, green, blue, "Equipment", align);
         
         TfDrawSkin(1, 288 + relativeX, yPos - 12 - 12 - 12 - 12 - 12 - 12 - 12 - 12 - 7, 12, 12, (hideSentMsg) ? checkedId : unCheckedId, 0, 0);
         TfPrintFps(1, 288 + 14 + relativeX, yPos - 11 - 12 - 12 - 12 - 12 - 12 - 12 - 12 - 7, 2, red, green, blue, "Sent Messages", align);
         
         TfDrawSkin(1, 288 + relativeX, yPos - 12 - 12 - 12 - 12 - 12 - 12 - 12 - 12 - 12 - 8, 12, 12, (hideChannelList) ? checkedId : unCheckedId, 0, 0);
         TfPrintFps(1, 288 + 14 + relativeX, yPos - 11 - 12 - 12 - 12 - 12 - 12 - 12 - 12 - 12 - 8, 2, red, green, blue, "Channel List", align);
         
    }
    
    TfDrawSkin(surface, 286 + relativeX, yPos + 5, 65, 12, currentNewFileBtnId, 0,0);
    TfPrintFps(surface, 288 + relativeX, yPos + 6, fontnumber, red, green, blue, "\xbb New File", align);
    
    TfDrawSkin(surface, 286 + relativeX, yPos + 5 + 12 + 4, 65, 12, (showCensor && recState != RECSTATE_RECORDING) ? clickedId : normalBtnId, 0,0);
    
    if(recState == RECSTATE_RECORDING)
                TfPrintFps(surface, 288 + relativeX, yPos + 5 + 12 + 5, fontnumber, 90, 90, 90, "\xbb Censor", align);
    else
                TfPrintFps(surface, 288 + relativeX, yPos + 5 + 12 + 5, fontnumber, red, green, blue, "\xbb Censor", align);                    
    
    TfDrawSkin(surface, 193 + relativeX, yPos + 5, 90, 12, (recState == RECSTATE_RECORDING) ? clickedId : normalBtnId, 0,0);    
    TfPrintFps(surface, 195 + relativeX, yPos + 6, fontnumber, red, green, blue, "\xbb Start/Pause", align);
    
    TfDrawSkin(surface, 193 + relativeX, yPos + 5 + 12 + 4, 90, 12, normalBtnId, 0,0);
    TfPrintFps(surface, 195 + relativeX, yPos + 5 + 12 + 5, fontnumber, red, green, blue, "\xbb Stop/Save", align);      

    TfDrawSkin(surface, 110 + relativeX, yPos + 5, 80, 28, 60, 0,0);
    switch(recState)
    {
        case RECSTATE_READY:
        {
             TfPrintFps(surface, 115 + relativeX, yPos + 7, fontnumber, 0, 255, 10, "READY", align);
             break;
        }
        case RECSTATE_RECORDING:
        { 
             TfPrintFps(surface, 115 + relativeX, yPos + 7, fontnumber, 255, 0, 80, "RECORDING", align);
             break;
        }
        case RECSTATE_PAUSE:
        {
             TfPrintFps(surface, 115 + relativeX, yPos + 7, fontnumber, 0, 145, 152, "PAUSED", align);
             break;
        }
        case RECSTATE_SAVING:
        {
             TfPrintFps(surface, 115 + relativeX, yPos + 7, fontnumber, 255, 255, 255, "SAVING...", align);
             break;
        }
        case RECSTATE_SAVED:
        {
             TfPrintFps(surface, 115 + relativeX, yPos + 7, fontnumber, 0, 255, 10, "SAVED!", align);
             break;
        }
        case RECSTATE_FILEWRITEERROR:
        {
             TfPrintFps(surface, 115 + relativeX, yPos + 7, fontnumber, 255, 0, 80, "ERROR", align);
             break;
        }
    }
    
    if (recState == RECSTATE_RECORDING)
        printTime = (file->totalTime + (clock() - file->startClock)) / 1000;
    else
        file->ResetDelay();

    uint32_t tSec = printTime % 60;// calculate running time
    uint32_t tMin = (printTime / 60) % 60;
    uint32_t tHour = (printTime / 60) / 60;
    static char timeStr[] = "00:00:00";
    sprintf(timeStr, "%02d:%02d:%02d", tHour, tMin, tSec);
    TfPrintFps(surface, 115 + relativeX, yPos + 21, fontnumber, red, green, blue, timeStr, align);
    return TfPostPrintFps();
}

void OnButtonClick(ButtonType_t newtype)
{    
    switch(newtype)
    {   
          case BUTTONTYPE_STARTPAUSE:
               {
                    if (file->isOpening() || file->isSaving())
                       return;
        
                    if (!bRecording)
                    { //start  
                          if (!file->isOpen())
                             file->Open();  
                          bGenMap = true;
                          recState = RECSTATE_RECORDING; 
                    }
                    else
                    { //pause    
                          recState = RECSTATE_PAUSE;
                          bRecording = false;    
                    }
                    break;                
               }
          case BUTTONTYPE_STOP:
               {
                    _beginthread(StopButtonProc, 0, NULL); 
                    break;
               }    
          case BUTTONTYPE_NEWFILE:
               {
                    _beginthread(NewFileButtonProc, 0, NULL);              
                    break;
               }    
          case BUTTONTYPE_CENSOR:
               {    
                    if (recState != RECSTATE_RECORDING)
                       showCensor = !showCensor;
                    break;
               }   
          //////censor buttons events here
          case BUTTONTYPE_CENSOR_HIDEDEFAULT:
               {
                    hideDefChannel = !hideDefChannel;                         
                    break;                         
               }  
          case BUTTONTYPE_CENSOR_HIDEINCOMINGPRIVATE:
               {
                    hideIncPrivMsg = !hideIncPrivMsg;                         
                    break;                         
               }  
          case BUTTONTYPE_CENSOR_HIDEALLCHANNELS:
               {
                    hideAllChannels = !hideAllChannels;                         
                    break;                         
               }  
          case BUTTONTYPE_CENSOR_HIDEVIP:
               {
                    hideVip = !hideVip;                         
                    break;                         
               }  
          case BUTTONTYPE_CENSOR_HIDEBACKPACKS:
               {
                    hideBackpacks = !hideBackpacks;                         
                    break;                         
               }  
          case BUTTONTYPE_CENSOR_HIDESKILLS:
               {
                    hideSkills = !hideSkills;                         
                    break;                         
               }  
          case BUTTONTYPE_CENSOR_HIDEEQ:
               {
                    hideEq = !hideEq;                         
                    break;                         
               }  
          case BUTTONTYPE_CENSOR_HIDESENTMESSAGES:
               {
                    hideSentMsg = !hideSentMsg;                         
                    break;                         
               }  
          case BUTTONTYPE_CENSOR_HIDECHANNELLIST:
               {
                    hideChannelList = !hideChannelList;                         
                    break;                         
               }  
    } 
}

void NewFileButtonProc(void *dummy)
{
    if (file->isopen)
    {
       currentNewFileBtnId = clickedId;             
       CHAR msg[MAX_PATH];
       sprintf(msg, "Do you really want to drop your recording?!");
       if (MessageBox(*(HWND*)(TIBIA_HWND_ADDR), msg, PROGRAM_NAME, MB_ICONWARNING | MB_YESNO) == IDYES)
       {
          printTime = 0;                                   
          recState = RECSTATE_READY;    //thanks to DarkstaR!                               
          bRecording = false;
          file->isopen = false;
       }
       currentNewFileBtnId = normalBtnId;     
    }
}

void StopButtonProc(void *dummy)
{
    if (file->isOpen())
    {
        bRecording = false;
        bGenMap = false;               
        recState = RECSTATE_SAVING; 
        int closeVal = file->Close();
        if (closeVal == 0)
        {
            printTime = 0;
            recState = RECSTATE_FILEWRITEERROR;
        }
        else if (closeVal == 1) 
        {
            printTime = 0;
            recState = RECSTATE_SAVED;
        }
        else if (closeVal == 2) 
            recState = RECSTATE_PAUSE;
    }
}

int OnGetNextPacket()
{
    TPacketStream* pRecvStream = (TPacketStream*)(ADDR_RECV_STREAM);
    static DWORD pPos;
    static LPBYTE pCmdBuffer;
    
    if (bGenMap)
    {   
        bGenMap = false;
        GenerateMapPacket();
    }
    
    DWORD lSize = pRecvStream->dwPos - pPos;
    
    if (lSize > 0 && pCmdBuffer != NULL && lSize != pRecvStream->dwSize)
    {
        pPos = 0;
        
        while ((bRecording) && (file->isOpen()) && ((*(BYTE*)(PLAYER_CONNECTION_ADDR)) >= 6))
        {
            if (pCmdBuffer[0] == 0x1F //first game server packet
            || pCmdBuffer[0] == 0x14
            || pCmdBuffer[0] == 0x15
            || pCmdBuffer[0] == 0x16)
            {
                file->ResetDelay();
                break;
            }

            if (CheckPacket(pCmdBuffer, lSize))
            {
               msg->WriteBytes((char*)pCmdBuffer, lSize); //WRITE COMPLETE COMMAND HERE
            }
            
    		break;
        }
    }
    
	int iCmd = TfGetNextPacket();
	if(iCmd != -1)
	{
		pPos = pRecvStream->dwPos - 1;
		pCmdBuffer = (LPBYTE)pRecvStream->pBuffer + pPos;
	}else
	{       
        if (msg->offset > 0)
        {
            file->WritePacket(msg->data, msg->offset);
            msg->Clear();
        }
    }
	
	return iCmd;
}

void GenerateMapPacket()
{
    MapPacket *map = new MapPacket();

    map->BuildMapPacket(); //build map
    file->WritePacket(map->data, map->offset, true);
    map->offset = 0;

    map->BuildPlayerProps();
    file->WritePacket(map->data, map->offset, true);
    map->offset = 0;
        
     //unknown creatures
    for(unsigned int i = (BlistStart); i < (BlistEnd); i = i + BlistStepCreatures) 
    {
        int *id = (int *)(i);
        map->WriteCreature(*id);
        if (map->offset > 14000)
        {
           file->WritePacket(map->data, map->offset, true);
           map->offset = 0;
        }
    }
    if (map->offset > 0)
    {
        file->WritePacket(map->data, map->offset, true);
        map->offset = 0;        
    } 
    map->RestoreRandomTileItems();
    file->WritePacket(map->data, map->offset, true);
    map->offset = 0; 
    
    delete map;
    bRecording = true;
}

DWORD GetMainThreadId()
{
  DWORD dwMainThreadID = 0;
  ULONGLONG ullMinCreateTime = MAXULONGLONG;

  HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
  if (hThreadSnap != INVALID_HANDLE_VALUE) {
    THREADENTRY32 th32;
    th32.dwSize = sizeof(THREADENTRY32);
    DWORD procId = GetCurrentProcessId();
    BOOL bOK = TRUE;
    for (bOK = Thread32First(hThreadSnap, &th32); bOK;
         bOK = Thread32Next(hThreadSnap, &th32)) {
      if (th32.th32OwnerProcessID == procId) {
        HANDLE hThread = OpenThread(THREAD_QUERY_INFORMATION,
                                    TRUE, th32.th32ThreadID);
        if (hThread) {
          FILETIME afTimes[4] = {0};
          if (GetThreadTimes(hThread,
                             &afTimes[0], &afTimes[1], &afTimes[2], &afTimes[3])) {
            ULONGLONG ullTest = MAKEULONGLONG(afTimes[0].dwLowDateTime,
                                              afTimes[0].dwHighDateTime);
            if (ullTest && ullTest < ullMinCreateTime) {
              ullMinCreateTime = ullTest;
              dwMainThreadID = th32.th32ThreadID; // let it be main... :)
              
            }
          }
          CloseHandle(hThread);
        }
      }
    }
    CloseHandle(hThreadSnap);
  }
  else
    return 0;
  
  return dwMainThreadID;
}

bool CheckPacket(LPBYTE data, DWORD len)
{
     BYTE pType = data[0]; //type
     
     if (pType == 0x7D || pType == 0x7E || pType == 0x7F)
           return false; //trade
     if (pType == 0x96 || pType == 0x97)
           return false; //itemtext & housetext
     if (pType == 0xC8)
           return false; //outfit window
     if (hideChannelList && pType == 0xAB)
           return false; //channellist
           
     if (pType == 0xD2 || pType == 0xD3 || pType == 0xD4)
        if (hideVip)
            return false; // vips
     if (pType == 0x6E || pType == 0x6F || pType == 0x70 || pType == 0x71 || pType == 0x72)
        if (hideBackpacks)
            return false; //bp
     if (pType == 0xA0 || pType == 0xA1) 
        if (hideSkills) //skills and status
            return false;
     if (pType == 0x78 || pType == 0x79)
        if (hideEq)  //set and reset invenstory
            return false;
     if (pType == 0xAC || pType == 0xAD) 
        if (hideAllChannels) //channels
            return false;
            
            
     if (pType == 0xAA) //creaturespeech
     {
        if (hideDefChannel || hideIncPrivMsg)
        {
            NetworkMessage* msg = new NetworkMessage(data, len);  
            if (msg == NULL || msg->data == NULL)
               return false;
                        
            msg->ReadByte(); //type    
            msg->ReadUInt32(); //unknownspeech
            short textLength = msg->ReadUInt16();
            msg->ReadBytes(NULL, textLength);
            msg->ReadUInt16();
            BYTE type = msg->ReadByte();
            delete msg; //clear

            if (type == 0x01 //say
             || type == 0x02 //whisper
             || type == 0x03 //yell
             || type == 0x06) //private 
                return false;  
         }
         else if (hideAllChannels)
            return false;
     }
     return true;
}
     
int __cdecl OnSayToPlayer(DWORD type, CHAR* receiver, CHAR* message)
{
    if (type == 0x06 && !hideSentMsg && bRecording && (file->isOpen()) && ((*(BYTE*)(PLAYER_CONNECTION_ADDR)) >= 6) && !bGenMap)
    {
        msg->WriteByte(0xAA);
        msg->WriteUInt32(0); //unknown speak
        msg->WriteString(receiver);
        msg->WriteUInt16(0);
        msg->WriteByte(0x06);
        msg->WriteString("» " + string(message));
    }
    
    return TfSayToPlayer(type, receiver, message);
}
     
int __cdecl OnSayToNpc(DWORD type, CHAR* message)
{
    if (type == 0x04 && !hideSentMsg && bRecording && (file->isOpen()) && ((*(BYTE*)(PLAYER_CONNECTION_ADDR)) >= 6) && !bGenMap)
    {
        msg->WriteByte(0xAA);
        msg->WriteUInt32(0); //unknown speak
        msg->WriteString("NPC »");
        msg->WriteUInt16(0);
        msg->WriteByte(0x06);//speechtype private
        msg->WriteString(string(message));
    }
    
    return TfSayToNpc(type, message);
}
