//---------------------------------------------------------------------------
//
// Name:        packetenumerator.cpp
// Author:      ex0ticprgmr 2020 & beziak 2009-2011
// Description: BynaCam is free Tibia movie recording & playing software.
//              BynaCam Reloaded v2 is using Tibia GUI engine
//              Designed exclusively for Tibia 8.6
// BynaCam Player is placed in public domain.
// Anyone is free to copy, modify, publish, use, compile, sell, or distribute the original BynaCam code, 
// either in source code form or as a compiled binary, for any purpose, commercial or non-commercial, and by any means.
//---------------------------------------------------------------------------


#include "packetenumerator.h"

PacketEnumerator::PacketEnumerator(uint8_t* d)
{
    offset = 0;                                 
    data = d;   
    unknown = Get16();
    packets = Get32();
    packets -= 0x39;     
    for (int i = 0; i < packets; i++)
    {
        Get16(); //int packetSizeTotal = 
        time = Get32();
        int packetSize = Get16();
        GetData(packetSize); //uint8_t* data = 
        Get32(); //padding
    }    
    offset = 6;  
    curtime = 0;    
    curpacket = 0;                             
}

uint32_t PacketEnumerator::Get32()
{
    uint32_t ret = *(uint32_t*)&data[offset];
    offset += 4;
    return ret;
}

uint16_t PacketEnumerator::Get16()
{
    uint16_t ret = *(uint16_t*)&data[offset];
    offset += 2;
    return ret;
}

uint8_t* PacketEnumerator::GetData(uint32_t size)
{
    uint8_t* ret = &data[offset];
    offset += size;  
    return ret;
}

uint8_t* PacketEnumerator::GetNextPacket(uint32_t* outsize)
{
    curpacket++;  
    if (curpacket >= packets)   
    {
       curtime = time;         
       return NULL;
    }
       
    Get16(); //int packetSizeTotal = 
    curtime = Get32();
    int packetSize = Get16(); //
    uint8_t* ret = GetData(packetSize); //uint8_t* data = 
    *outsize = packetSize;
    Get32(); //padding
    return ret;
}

uint32_t PacketEnumerator::GetCurrentTime()
{
    if (curpacket >= packets) return time;    
    return curtime;     
}

void PacketEnumerator::GotoTime(uint32_t stamp)
{
    bool ret = false; 
    if(stamp > curtime)
    {
       for (int i = curpacket; i < packets; i++)
       {
           Get16(); //int packetSizeTotal = 
           curtime = Get32();
           if (curtime >= stamp)
               ret = true;
           int packetSize = Get16();
           GetData(packetSize); //uint8_t* data = 
           Get32(); //padding
           if (ret)
               break;
       }    
    } 
    else if (stamp < curtime)
    {
       for (int i = 0; i < packets; i++)
       {
           Get16(); //int packetSizeTotal = 
           curtime = Get32();
           if (curtime >= stamp)
               ret = true;
           int packetSize = Get16();
           GetData(packetSize); //uint8_t* data = 
           Get32(); //padding
           if (ret)
               break;
       }    
    }
}

uint32_t PacketEnumerator::GetTotalTime()
{
    return time;     
}
