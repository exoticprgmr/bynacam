//---------------------------------------------------------------------------
//
// Name:        connection.h
// Author:      ex0ticprgmr 2020 & beziak 2009-2011
// Description: BynaCam is free Tibia movie recording & playing software.
//              BynaCam Reloaded v2 is using Tibia GUI engine
//              Designed exclusively for Tibia 8.6
// BynaCam Player is placed in public domain.
// Anyone is free to copy, modify, publish, use, compile, sell, or distribute the original BynaCam code, 
// either in source code form or as a compiled binary, for any purpose, commercial or non-commercial, and by any means.
//---------------------------------------------------------------------------


#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include <windows.h>
#include <string>
#include "socket.h"
#include "networkmessage.h"
#include "var.h"

class Connection
{
      public:
             static NetworkMessage* GetLoginPacket(short gamePort, long* xtea);
             static void SendLoginPacket(Socket* s, short gamePort, long* xtea);
             static void SetLoginServers(short port);
             static void AutoLogin();
};

#endif
