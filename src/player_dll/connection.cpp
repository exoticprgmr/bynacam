//---------------------------------------------------------------------------
//
// Name:        connection.cpp
// Author:      ex0ticprgmr 2020 & beziak 2009-2011
// Description: BynaCam is free Tibia movie recording & playing software.
//              BynaCam Reloaded v2 is using Tibia GUI engine
//              Designed exclusively for Tibia 8.6
// BynaCam Player is placed in public domain.
// Anyone is free to copy, modify, publish, use, compile, sell, or distribute the original BynaCam code, 
// either in source code form or as a compiled binary, for any purpose, commercial or non-commercial, and by any means.
//---------------------------------------------------------------------------


#include "connection.h"

NetworkMessage* Connection::GetLoginPacket(short gamePort, long* xtea)
{
    NetworkMessage* msg = new NetworkMessage();
    msg->WriteInt16(0); //packet len
    msg->WriteInt32(0); //adler
    msg->WriteInt16(25); //logical packet len
    msg->WriteByte(0x64);
    msg->WriteByte(0x01);
    msg->WriteString("Byna");
    msg->WriteString("BynaCam");
    
    //ip
    msg->WriteByte(127);
    msg->WriteByte(0);
    msg->WriteByte(0);
    msg->WriteByte(1);
    
    msg->WriteInt16(gamePort);
    msg->WriteInt16(0xFFFF); //premmy days
    
    msg->PrepareToSend(xtea);
    
    return msg;
}

void Connection::SendLoginPacket(Socket* s, short gamePort, long* xtea)
{
    string temp;
    NetworkMessage* msg = GetLoginPacket(gamePort, xtea);
    temp.assign ((const char*)msg->data, 38);
    s->SendBytes(temp);
    delete msg;
}

void Connection::SetLoginServers(short port)
{
    char server[] = "127.0.0.1";
    
    for (int i = 0; i < MaxLoginServers; i++)
    {
        char* s = (char*)(LoginServerStart+(i*StepLoginServer));
        strcpy(s, server);
        uint32_t* p = (uint32_t*)((uint32_t)s + DistancePort);
        *p = port;
    }
}

void Connection::AutoLogin()
{
    HWND hWnd = *TIBIA_HWND;
    RECT WindowSize;
    GetClientRect(hWnd,&WindowSize); // Get size of the window

    int posX = 120; // Distance to enter game button from left of window
    int posY = WindowSize.bottom - 220; // Distance from the bottom
    
    
    SendMessage(hWnd, WM_KEYDOWN, VK_ESCAPE, 0); // Hit esc button
    SendMessage(hWnd,WM_CHAR,VK_ESCAPE,0);
    SendMessage(hWnd,WM_KEYUP,VK_ESCAPE,0);
    Sleep(200);
    SendMessage(hWnd,WM_KEYDOWN,VK_ESCAPE,0); // Hit esc button
    SendMessage(hWnd,WM_CHAR,VK_ESCAPE,0);
    SendMessage(hWnd,WM_KEYUP,VK_ESCAPE,0);
    Sleep(200);
    SendMessage(hWnd,WM_LBUTTONDOWN,MAKEWPARAM(posX,posY),MAKELPARAM(posX,posY)); // Click  the "Enter Game" button
    SendMessage(hWnd,WM_LBUTTONUP,MAKEWPARAM(posX,posY),MAKELPARAM(posX,posY));
    
    Sleep(1000);
    
    SendMessage(hWnd,WM_KEYDOWN,VK_RETURN,0); // Hit Enter button
    SendMessage(hWnd,WM_CHAR,VK_RETURN,0);
    SendMessage(hWnd,WM_KEYUP,VK_RETURN,0);
    
    Sleep(1000);
    
    SendMessage(hWnd,WM_KEYDOWN,VK_RETURN,0); // Hit Enter button
    SendMessage(hWnd,WM_CHAR,VK_RETURN,0);
    SendMessage(hWnd,WM_KEYUP,VK_RETURN,0);
}


