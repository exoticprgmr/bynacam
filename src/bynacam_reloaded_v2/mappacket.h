//---------------------------------------------------------------------------
//
// Name:        mappacket.h
// Author:      ex0ticprgmr 2020 & beziak 2009-2011
// Description: BynaCam is free Tibia movie recording & playing software.
//              BynaCam Reloaded v2 is using Tibia GUI engine
//              Designed exclusively for Tibia 8.6
// BynaCam Reloaded v2 is placed in public domain.
// Anyone is free to copy, modify, publish, use, compile, sell, or distribute the original BynaCam code, 
// either in source code form or as a compiled binary, for any purpose, commercial or non-commercial, and by any means.
//---------------------------------------------------------------------------

#ifndef _MAPPACKET_H_
#define _MAPPACKET_H_

#include "networkmessage.h"
#include "var.h"
#include <sstream>
#include <list>

class MapPacket : public NetworkMessage
{
    public:
        MapPacket();
        ~MapPacket();
        
        void BuildMapPacket();
        void BuildPlayerProps();
        void WriteCreature(int id);
        void ContainerPacket();
        static std::string FindPlayerName(int cid);
        static int BattleListGet(int cid,int offset);
        void RestoreRandomTileItems(); 
        
    private:       
        bool MapPacket::HasExtraByte(DWORD id);
        void InsertCreature(int id);
        TMemTile * XYZ(int x, int y, int realZ);
        void SkillsPacket();    
        void StatusPacket();        
        void WorldLightPacket(); 
        void VipPacket();
        void EqPacket();
        void SetRandomTile();
        
        int myX;
        int myY;
        char myZ;
        
        TMemTile* randomTile;
        int randomTileX;
        int randomTileY;
        char randomTileZ;
        
        std::list<int> knownCreatureIds;
        bool KnownListContains(int id);
};

#endif
