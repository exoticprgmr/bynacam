//---------------------------------------------------------------------------
//
// Name:        mappacket.cpp
// Author:      ex0ticprgmr 2020 & beziak 2009-2011
// Description: BynaCam is free Tibia movie recording & playing software.
//              BynaCam Reloaded v2 is using Tibia GUI engine
//              Designed exclusively for Tibia 8.6
// BynaCam Reloaded v2 is placed in public domain.
// Anyone is free to copy, modify, publish, use, compile, sell, or distribute the original BynaCam code, 
// either in source code form or as a compiled binary, for any purpose, commercial or non-commercial, and by any means.
//---------------------------------------------------------------------------

#include "mappacket.h"

//extern "C" bool _cdecl GetItemProperty(DWORD id, DWORD flags);
extern bool hideVip; //
extern bool hideBackpacks; //
extern bool hideSkills; // 
extern bool hideEq; //

MapPacket::MapPacket()
{
     knownCreatureIds.clear();  
     TMemTile* randomTile = NULL;               
}

MapPacket::~MapPacket()
{                 
}

bool MapPacket::HasExtraByte(DWORD id)
{
     return GetItemProperty(id, 0x0a) || GetItemProperty(id, 0x0b) || GetItemProperty(id, 0x05);                   
}

std::string MapPacket::FindPlayerName(int cid) {

        for(unsigned int i = (BlistStart); i < (BlistEnd); i = i + BlistStepCreatures)  {
                int *creatureId = (int *)i;
                if(*creatureId == cid) {
                        char temp[32];
                        memcpy(&temp[0],(void *)(i + BATTLELIST_NAME),32);
                        string bname(temp);
                        return bname;

                }
        }
        return "";
}

int MapPacket::BattleListGet(int cid,int offset)
{
        for(unsigned int i = (BlistStart); i < (BlistEnd); i = i + BlistStepCreatures)  {
                int *temp = (int *)(i);
                if(*temp == cid) {
                        int *ret = (int *)(i + offset);
                        return *ret;
                }
        }
        return 0;
}

void MapPacket::SetRandomTile()
{
     randomTile = XYZ(0, 0, myZ);			
     randomTileX = myX - 8;
     randomTileY = myY - 6;
     randomTileZ = myZ;
}

// Custom tile access
TMemTile * MapPacket::XYZ(int x, int y, int realZ)
{
	TMemTile * pCurrentTile = (TMemTile*)(*(DWORD*)(MAP_POINTER));

    //find map offsets
	for(int pz=0; pz<8; pz++)
	{
        for(int py=0; py<14; py++)
		{
			for(int px=0; px<18; px++)
			{
				//look for player tile
				for(int i=0; i < (int)pCurrentTile->count; i++)
				{                                
					if(pCurrentTile->items[i].id == 0x63)
					{                   
						if(pCurrentTile->items[i].data1 == (uint32_t)*(DWORD*)(PLAYER_ID))
						{    
	                        //center 8,6: calculate offsets
	                        int m_OffsetX = px - 8; if(m_OffsetX < 0) m_OffsetX += 18;
	                        int m_OffsetY = py - 6; if(m_OffsetY < 0) m_OffsetY += 14;
	                        int m_OffsetZ = pz - (15 - myZ);
	
	                        int PosX = x + m_OffsetX; if(PosX > 17) PosX-=18;
	                        int PosY = y + m_OffsetY; if(PosY > 13) PosY-=14;

	                        int PosZ = (15 - realZ) + m_OffsetZ;
	                        if(PosZ < 0) PosZ+=8;
	                        if(PosZ > 7) PosZ-=8;
	                        return (TMemTile*)*(DWORD*)(MAP_POINTER) + PosX + PosY*18 + PosZ*14*18;
						}
					}
				}
				pCurrentTile++;
			}//x
		}//y
	}//z

	return NULL;
}       

void MapPacket::InsertCreature(int id)
{
    WriteUInt16(0x61);
    WriteUInt32(0);
    WriteUInt32(id);
    WriteString(FindPlayerName(id));
    WriteByte(BattleListGet(id,BATTLELIST_HPBAR));
    WriteByte(BattleListGet(id,BATTLELIST_DIRECTION));
    uint16_t bOutfit = (uint16_t)BattleListGet(id,BATTLELIST_OUTFIT);
    WriteUInt16(bOutfit);
    
    if(bOutfit != 0)
    {
        WriteByte(BattleListGet(id,BATTLELIST_HEAD_COLOR));
        WriteByte(BattleListGet(id,BATTLELIST_BODY_COLOR));
        WriteByte(BattleListGet(id,BATTLELIST_LEGS_COLOR));
        WriteByte(BattleListGet(id,BATTLELIST_FEET_COLOR));
        WriteByte(BattleListGet(id,BATTLELIST_ADDON));
    } 
    else
    {
        WriteUInt16(BattleListGet(id,BATTLELIST_HEAD_COLOR));
    }

    WriteByte(BattleListGet(id,BATTLELIST_LIGHT));
    WriteByte(BattleListGet(id,BATTLELIST_LIGHT_COLOR));
    WriteUInt16(BattleListGet(id,BATTLELIST_WALK_SPEED));
    WriteByte(BattleListGet(id,BATTLELIST_SKULL));
    WriteByte(BattleListGet(id,BATTLELIST_PARTY));
    WriteByte(BattleListGet(id,BATTLELIST_WARICON));                                                 
    WriteByte(BattleListGet(id,BATTLELIST_BLOCKING));
}  

void MapPacket::RestoreRandomTileItems()
{
    for(int n=0; n < randomTile->count; n++)
	{
        WriteByte(0x6A); 
        WriteUInt16(randomTileX);//posx
        WriteUInt16(randomTileY);//posy
        WriteByte(randomTileZ);//posz     
        WriteByte(n);//stack     
        
		DWORD id = randomTile->items[n].id;

		if(id == 0x63)	   
			InsertCreature(randomTile->items[n].data1);
        else if (id > 0x63)
		{
			WriteUInt16(id);
            if(HasExtraByte(id))
                WriteByte(randomTile->items[n].data1); 
		}
    }
}

bool MapPacket::KnownListContains(int id)
{
    for (list<int>::iterator it=knownCreatureIds.begin(); it != knownCreatureIds.end(); it++)
        if (*it == id)
            return true;
    return false;
}  

void MapPacket::WriteCreature(int id)
{          
    if (id != 0 && !KnownListContains(id))
    {
        WriteByte(0x6A); //addcreature
        WriteUInt16(randomTileX);//posx
        WriteUInt16(randomTileY);//posy
        WriteByte(randomTileZ);//posz
        WriteByte(0x00);//stack
        InsertCreature(id);
        
        WriteByte(0x6C); //removecreature
        WriteUInt16(randomTileX);//posx
        WriteUInt16(randomTileY);//posy
        WriteByte(randomTileZ);//posz
        WriteByte(0x00);//stack
    } 
}

void MapPacket::BuildMapPacket()
{
    randomTile = NULL;   

    myX = BattleListGet(*(DWORD*)(PLAYER_ID),BATTLELIST_X);
	myY = BattleListGet(*(DWORD*)(PLAYER_ID),BATTLELIST_Y);
	myZ = BattleListGet(*(DWORD*)(PLAYER_ID),BATTLELIST_Z);
    
    /// Self appear
    WriteByte(0x0A);
    WriteUInt32(*(DWORD*)(PLAYER_ID));
    WriteByte(0x32);
    WriteByte(0x00);
    WriteByte(0x00);
    
    WriteByte(0x64);
    WriteUInt16(myX);
    WriteUInt16(myY);
    WriteByte(myZ);
    
    int startz, endz, zstep, skip = -1;
    
	if(myZ > 7)
	{
		startz = myZ - 2;
		endz = min(15, myZ + 2);
		zstep = 1;

	} else
	{
		startz = 7;
		endz = 0;
		zstep = -1;
	}

	for(int z = startz; z != endz + zstep; z += zstep)
	{
		for(int x=0; x<18; x++)
		{
			for(int y=0; y<14; y++)
			{    
			  	TMemTile * pCurrent = XYZ(x,y,z);

			  	if (randomTile == NULL)
                    SetRandomTile();
                
			  	if (pCurrent->count == 0 || pCurrent == randomTile)
			  	{         
                    skip++; 
                	if(skip == 0xFF) 
	                {
                         WriteByte(0xFF);
                         WriteByte(0xFF); 
                         skip = -1;
                    }

                   if (pCurrent == randomTile)
                       continue;
                }
                else if (pCurrent->count > 0)
                {
                    if(skip >= 0) 
                    {
                         WriteByte(skip);
                         WriteByte(0xFF); 
                    }
                    skip = 0; 
                }

				for(uint32_t n=0; n < pCurrent->count; n++)
				{
					DWORD id = pCurrent->items[n].id;

					if(id == 0x63)
                    {		
						InsertCreature(pCurrent->items[n].data1);
						knownCreatureIds.push_back(pCurrent->items[n].data1);
					} 
                    else if (id > 0x63)
					{

						WriteUInt16(id);
                        if(HasExtraByte(id))
                            WriteByte(pCurrent->items[n].data1);
				    }
                }   
			}
		}
	}	
	if(skip >= 0) 
	{
         WriteByte(skip);
         WriteByte(0xFF); 
    }
}

void MapPacket::BuildPlayerProps()
{      
    WorldLightPacket();
    
    if (!hideEq)     
       EqPacket(); 
    
    if (!hideSkills)
    {   
        StatusPacket();
        SkillsPacket();
    }
    
    if (!hideVip)
       VipPacket();
    
    if (!hideBackpacks)
       ContainerPacket();
}

void MapPacket::SkillsPacket()
{
    //skills packet
    WriteByte(0xA1);  
    WriteByte(*(DWORD*)(PLAYER_FIST));
    WriteByte(*(DWORD*)(PLAYER_FIST_PER));
    WriteByte(*(DWORD*)(PLAYER_CLUB));
    WriteByte(*(DWORD*)(PLAYER_CLUB_PER));
    WriteByte(*(DWORD*)(PLAYER_SWORD));
    WriteByte(*(DWORD*)(PLAYER_SWORD_PER));
    WriteByte(*(DWORD*)(PLAYER_AXE));
    WriteByte(*(DWORD*)(PLAYER_AXE_PER));
    WriteByte(*(DWORD*)(PLAYER_DIST));
    WriteByte(*(DWORD*)(PLAYER_DIST_PER));
    WriteByte(*(DWORD*)(PLAYER_SHIELD));
    WriteByte(*(DWORD*)(PLAYER_SHIELD_PER));
    WriteByte(*(DWORD*)(PLAYER_FISH));
    WriteByte(*(DWORD*)(PLAYER_FISH_PER));
}

void MapPacket::StatusPacket()
{
    //status packet
    WriteByte(0xA0);
    WriteUInt16(*(DWORD*)(PLAYER_HP));
    WriteUInt16(*(DWORD*)(PLAYER_HP_MAX));
    WriteUInt32(*(DWORD*)(PLAYER_CAP));
    WriteUInt32(*(DWORD*)(PLAYER_EXP));
    WriteUInt16(*(DWORD*)(PLAYER_LEVEL));
    WriteByte(*(DWORD*)(PLAYER_LEVEL_PER));
    WriteUInt16(*(DWORD*)(PLAYER_MANA));
    WriteUInt16(*(DWORD*)(PLAYER_MANA_MAX));
    WriteByte(*(DWORD*)(PLAYER_MAGIC_LEVEL));
    WriteByte(*(DWORD*)(PLAYER_MAGIC_LEVEL_PER));
    WriteByte(*(DWORD*)(PLAYER_SOUL));
    WriteUInt16(*(DWORD*)(PLAYER_STAMINA));
}

void MapPacket::WorldLightPacket()
{
    //world light
    WriteByte(0x82);
    WriteByte(0xFF);
    WriteByte(0xD7);
}

void MapPacket::VipPacket()
{
        for(int i = (VIP_BEGIN); i < (VIP_END); i = i + VipPlayerDist) {
                int *playerId = (int *)i;
                if(*playerId != 0) {
                        std::string vname ((char *)(i + VipNameOffset));
                        int *online = (int *)(i + VipStatusOffset);
                        WriteByte(0xD2);
                        WriteUInt32(*playerId);
                        WriteString(vname.c_str());
                        WriteByte(*online);
                }
        }
}

void MapPacket::EqPacket()
{
        if(*(DWORD*)(PLAYER_SLOT_HEAD) != 0) 
        {
                WriteByte(0x78);
                WriteByte(0x01);
                WriteUInt16(*(DWORD*)(PLAYER_SLOT_HEAD));
        }

        if(*(DWORD*)(PLAYER_SLOT_NECKLACE) != 0) 
        {
                WriteByte(0x78);
                WriteByte(0x02);
                WriteUInt16(*(DWORD*)(PLAYER_SLOT_NECKLACE));
        }

        if(*(DWORD*)(PLAYER_SLOT_CONTAINER) != 0) 
        {
                WriteByte(0x78);
                WriteByte(0x03);
                WriteUInt16(*(DWORD*)(PLAYER_SLOT_CONTAINER));
        }

        if(*(DWORD*)(PLAYER_SLOT_ARMOR) != 0) 
        {
                WriteByte(0x78);
                WriteByte(0x04);
                WriteUInt16(*(DWORD*)(PLAYER_SLOT_ARMOR));
        }

        if(*(DWORD*)(PLAYER_SLOT_RIGHT) != 0) 
        {
                WriteByte(0x78);
                WriteByte(0x05);
                WriteUInt16(*(DWORD*)(PLAYER_SLOT_RIGHT));

                if(HasExtraByte(*(DWORD*)(PLAYER_SLOT_RIGHT)))
                {
                        WriteByte(*(DWORD*)(PLAYER_SLOT_RIGHT_COUNT));
                }
        }

        if(*(DWORD*)(PLAYER_SLOT_LEFT) != 0) 
        {
                WriteByte(0x78);
                WriteByte(0x06);
                WriteUInt16(*(DWORD*)(PLAYER_SLOT_LEFT));

                if(HasExtraByte(*(DWORD*)(PLAYER_SLOT_LEFT)))
                {
                        WriteByte(*(DWORD*)(PLAYER_SLOT_LEFT_COUNT));
                }
        }

        if(*(DWORD*)(PLAYER_SLOT_LEGS) != 0) 
        {
                WriteByte(0x78);
                WriteByte(0x07);
                WriteUInt16(*(DWORD*)(PLAYER_SLOT_LEGS));
        }

        if(*(DWORD*)(PLAYER_SLOT_FEET) != 0) 
        {
                WriteByte(0x78);
                WriteByte(0x08);
                WriteUInt16(*(DWORD*)(PLAYER_SLOT_FEET)); 
        }

        if(*(DWORD*)(PLAYER_SLOT_RING) != 0) 
        {
                WriteByte(0x78);
                WriteByte(0x09);
                WriteUInt16(*(DWORD*)(PLAYER_SLOT_RING));
        }

        if(*(DWORD*)(PLAYER_SLOT_AMMO) != 0) 
        {
                WriteByte(0x78);
                WriteByte(0x0A);
                WriteUInt16(*(DWORD*)(PLAYER_SLOT_AMMO));

                if(HasExtraByte(*(DWORD*)(PLAYER_SLOT_AMMO)))
                {
                        WriteByte(*(DWORD*)(PLAYER_SLOT_AMMO_COUNT));
                }
        }
}

void MapPacket::ContainerPacket()
{
    for(int i = (CONTAINER_BEGIN); i < (CONTAINER_END); i += CONTAINER_STEP)
    {
        int *isOpen = (int *)i;
        if(*isOpen == 1)
        {
            int *contID = (int *)(i + CONTAINER_ID);
            int *contVol = (int *)(i + CONTAINER_VOL);
            int *contAmm = (int *)(i + CONTAINER_NUM_OF_ITEMS);
            char *contName = (char *)(i + CONTAINER_NAME);
                        
            WriteByte(0x6E);
            WriteByte(((i - (CONTAINER_BEGIN)) / CONTAINER_STEP));
            WriteUInt16(*contID);
            WriteString(contName);
            WriteUInt16(*contVol);
            WriteByte(0);
            for (int j = (*contAmm)-1; j >= 0; j--)
            {
                WriteByte(0x70);
                WriteByte(((i - (CONTAINER_BEGIN)) / CONTAINER_STEP));
                int *id = (int *)(i + CONTAINER_ITEM_DISTANCE + (j * CONTAINER_ITEM_STEP));
                int *idData = (int *)(i + CONTAINER_ITEM_DISTANCE + (j * CONTAINER_ITEM_STEP) + 4);
                WriteUInt16(*id);
                if(HasExtraByte(*id))
                   WriteByte(*idData);            
            }            
        }
    }
}
