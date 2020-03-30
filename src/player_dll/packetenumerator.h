//---------------------------------------------------------------------------
//
// Name:        packetenumerator.h
// Author:      ex0ticprgmr 2020 & beziak 2009-2011
// Description: BynaCam is free Tibia movie recording & playing software.
//              BynaCam Reloaded v2 is using Tibia GUI engine
//              Designed exclusively for Tibia 8.6
// BynaCam Player is placed in public domain.
// Anyone is free to copy, modify, publish, use, compile, sell, or distribute the original BynaCam code, 
// either in source code form or as a compiled binary, for any purpose, commercial or non-commercial, and by any means.
//---------------------------------------------------------------------------


#ifndef _PACKETENUMERATOR_H_
#define _PACKETENUMERATOR_H_

#include <stdint.h>
#include <windows.h>

class PacketEnumerator
{
      public:
             PacketEnumerator(uint8_t* d);
             uint8_t* GetNextPacket(uint32_t* outsize);
             uint32_t GetCurrentTime();
             void GotoTime(uint32_t stamp);
             uint32_t GetTotalTime();
             uint32_t offset;
             uint32_t curpacket;
             uint32_t curtime;
             uint32_t time;
      private:
              uint8_t* data;              
              uint32_t packets;
              uint16_t unknown;
              
              uint32_t Get32();
              uint16_t Get16();
              uint8_t* GetData(uint32_t size);
};

#endif
