//---------------------------------------------------------------------------
//
// Name:        var.h
// Author:      ex0ticprgmr 2020 & beziak 2009-2011
// Description: BynaCam is free Tibia movie recording & playing software.
//              BynaCam Reloaded v2 is using Tibia GUI engine
//              Designed exclusively for Tibia 8.6
// BynaCam Reloaded v2 is placed in public domain.
// Anyone is free to copy, modify, publish, use, compile, sell, or distribute the original BynaCam code, 
// either in source code form or as a compiled binary, for any purpose, commercial or non-commercial, and by any means.
//---------------------------------------------------------------------------

#ifndef _VAR_H_
#define _VAR_H_

#include <windows.h>

typedef int __stdcall TF_POSTPRINTFPS();
typedef void __stdcall TF_EXITTIBIA(DWORD exitvalue);
typedef void __cdecl TF_DRAWSKIN(int nSurface, int X, int Y, int W, int H, int SkinId, int dX, int dY);
typedef int TF_GETNEXTPACKET();
typedef int __cdecl TF_SAYTOPLAYER (DWORD type, CHAR* receiver, CHAR* message);
typedef int __cdecl TF_SAYTONPC (DWORD type, CHAR* message);
typedef void __cdecl TF_PRINTFPS(DWORD surface, DWORD x, DWORD y, DWORD fontnumber, DWORD red, DWORD green, DWORD blue, CHAR* text, DWORD align);
typedef bool (__stdcall _GetItemProps)(int id, int flag);
typedef BOOL (WINAPI *_GetCurrentHwProfileA)(LPHW_PROFILE_INFOA);

const char PROGRAM_NAME[] = "BynaCam Reloaded for Tibia 8.6";
const char TIBIA_VERSION[4] = {8, 6, 0, 0};

static DWORD CALL_SAY_TO_PLAYER_ADDR = 0x004292ED; //8.6
static DWORD CALL_SAY_TO_NPC_ADDR = 0x00429116; //8.6
static DWORD PRINTFPS_FUNC_ADDR = 0x4B4DD0;//8.6
static DWORD CALL_POSTPRINT_FPS_ADDR = 0x45A34A;//8.6
static DWORD DRAWSKIN_FUNC_ADDR = 0x004B96E0;//8.6
static DWORD GUI_POINTER_ADDR = 0x64F5C8;//8.6
static DWORD TIBIA_HWND_ADDR = 0x63DB24;//8.6?
static DWORD TIBIA_EXIT_ADDR = 0x5613D2;//8.6


////////////////////////
static DWORD CALL_GET_NEXT_PACKET_ADDR = 0x45C3A5; //8.6
static DWORD ADDR_RECV_STREAM = 0x7998AC; //8.6
static DWORD PLAYER_CONNECTION_ADDR = 0x79CF28;//8.6

static DWORD GET_ITEM_PROPERTY_FUNC = 0x500EC0; //8.6
static _GetItemProps* GetItemProperty = (_GetItemProps*) GET_ITEM_PROPERTY_FUNC;

enum RecState_t
{
     RECSTATE_RECORDING,
     RECSTATE_SAVING,
     RECSTATE_SAVED,
     RECSTATE_PAUSE,
     RECSTATE_READY,
     RECSTATE_FILEWRITEERROR
};

enum ButtonType_t
{
     BUTTONTYPE_STARTPAUSE,
     BUTTONTYPE_STOP,
     BUTTONTYPE_NEWFILE,
     BUTTONTYPE_CENSOR,
     BUTTONTYPE_CENSOR_HIDEDEFAULT,
     BUTTONTYPE_CENSOR_HIDEINCOMINGPRIVATE,
     BUTTONTYPE_CENSOR_HIDEALLCHANNELS,
     BUTTONTYPE_CENSOR_HIDEVIP,
     BUTTONTYPE_CENSOR_HIDEBACKPACKS,
     BUTTONTYPE_CENSOR_HIDESKILLS,
     BUTTONTYPE_CENSOR_HIDEEQ,
     BUTTONTYPE_CENSOR_HIDESENTMESSAGES,
     BUTTONTYPE_CENSOR_HIDECHANNELLIST,
     BUTTONTYPE_CENSOR_HIDEOWNNAME,
     BUTTONTYPE_CENSOR_HIDEPLAYERSOUTFITS,
     BUTTONTYPE_CENSOR_HIDEPLAYERNAMES
};

struct TPacketStream
{
    LPVOID pBuffer; //pointer to the memory buffer
    DWORD dwSize; //size of the buffer
    DWORD dwPos; //current position to work with the buffer
};

struct TMemItem
{
    DWORD id;
    DWORD data1;
	DWORD data2;
};

struct TMemTile
{
	DWORD count;
    TMemItem items[10];
    DWORD order[10];
    DWORD padding;
};
    
//misc
static DWORD MAP_POINTER = 0x654118; // 8.6
        
static int VIP_BEGIN = 0x63DBB8; // 8.6
static int VipPlayerDist = 0x2C;
static int VipNameOffset = 4;
static int VipStatusOffset = 34;
static int VIP_END = VIP_BEGIN + (VipPlayerDist * 200);
        
static int CONTAINER_BEGIN = 0x64CD10; //8.6
static int CONTAINER_STEP = 492;
static int CONTAINER_MAX = 16;
static int CONTAINER_END = (CONTAINER_BEGIN + (CONTAINER_MAX * CONTAINER_STEP));
static int CONTAINER_ID = 4; 
static int CONTAINER_VOL = 48;
static int CONTAINER_NUM_OF_ITEMS = 56;
static int CONTAINER_ITEM_DISTANCE = 60;
static int CONTAINER_ITEM_STEP = 12;
static int CONTAINER_NAME = 16;

static DWORD EXP_MARK                            = 0x63FE8C;
static DWORD HEAD_MARK                           = 0x64CC98;

static DWORD PLAYER_HP                           = (EXP_MARK + 8);
static DWORD PLAYER_HP_MAX                       = (EXP_MARK + 4);
static DWORD PLAYER_CAP                          = (EXP_MARK - 36);
static DWORD PLAYER_MANA                         = (EXP_MARK - 20);
static DWORD PLAYER_MANA_MAX                     = (EXP_MARK - 24);

static DWORD PLAYER_SOUL                         = EXP_MARK - 28;
static DWORD PLAYER_STAMINA                      = EXP_MARK - 32;
static DWORD PLAYER_EXP                          = EXP_MARK;
static DWORD PLAYER_LEVEL                        = EXP_MARK - 4;
static DWORD PLAYER_MAGIC_LEVEL                  = EXP_MARK - 8;
static DWORD PLAYER_LEVEL_PER                    = EXP_MARK - 12;
static DWORD PLAYER_MAGIC_LEVEL_PER              = EXP_MARK - 16;

static DWORD PLAYER_ID                           = EXP_MARK + 12;
static DWORD PLAYER_FIST                         = EXP_MARK - 76;
static DWORD PLAYER_CLUB                         = EXP_MARK - 72;
static DWORD PLAYER_SWORD                        = EXP_MARK - 68; 
static DWORD PLAYER_AXE                          = EXP_MARK - 64; 
static DWORD PLAYER_DIST                         = EXP_MARK - 60; 
static DWORD PLAYER_SHIELD                       = EXP_MARK - 56; 
static DWORD PLAYER_FISH                         = EXP_MARK - 52; 

static DWORD PLAYER_FIST_PER                     = EXP_MARK - 104;
static DWORD PLAYER_CLUB_PER                     = EXP_MARK - 100; 
static DWORD PLAYER_SWORD_PER                    = EXP_MARK - 96; 
static DWORD PLAYER_AXE_PER                      = EXP_MARK - 92; 
static DWORD PLAYER_DIST_PER                     = EXP_MARK - 88; 
static DWORD PLAYER_SHIELD_PER                   = EXP_MARK - 84; 
static DWORD PLAYER_FISH_PER                     = EXP_MARK - 80; 

static DWORD PLAYER_SLOT_HEAD                    = HEAD_MARK; 
static DWORD PLAYER_SLOT_NECKLACE                = HEAD_MARK + 12; 
static DWORD PLAYER_SLOT_CONTAINER               = HEAD_MARK + 24; 
static DWORD PLAYER_SLOT_ARMOR                   = HEAD_MARK + 36; 
static DWORD PLAYER_SLOT_RIGHT                   = HEAD_MARK + 48; 
static DWORD PLAYER_SLOT_RIGHT_COUNT             = HEAD_MARK + 52; 
static DWORD PLAYER_SLOT_LEFT                    = HEAD_MARK + 60; 
static DWORD PLAYER_SLOT_LEFT_COUNT              = HEAD_MARK + 64; 
static DWORD PLAYER_SLOT_LEGS                    = HEAD_MARK + 72; 
static DWORD PLAYER_SLOT_FEET                    = HEAD_MARK + 84; 
static DWORD PLAYER_SLOT_RING                    = HEAD_MARK + 96; 
static DWORD PLAYER_SLOT_AMMO                    = HEAD_MARK + 108; 
static DWORD PLAYER_SLOT_AMMO_COUNT              = HEAD_MARK + 112; 
        
static DWORD BlistStepCreatures = 0xA8;
static DWORD BlistStart = 0x63FEF8;//8.6
static DWORD BlistMaxCreatures = 250;
static DWORD BlistEnd = BlistStart + (BlistStepCreatures * BlistMaxCreatures);
        
//distances
static DWORD BATTLELIST_ID = 0;
static DWORD BATTLELIST_NAME = 4;
static DWORD BATTLELIST_X = 36;
static DWORD BATTLELIST_Y = 40;
static DWORD BATTLELIST_Z = 44;
static DWORD BATTLELIST_WALKING = 76; 
static DWORD BATTLELIST_DIRECTION = 80;
static DWORD BATTLELIST_OUTFIT = 96;
static DWORD BATTLELIST_HEAD_COLOR = 100;
static DWORD BATTLELIST_BODY_COLOR = 104;
static DWORD BATTLELIST_LEGS_COLOR = 108;
static DWORD BATTLELIST_FEET_COLOR = 112;
static DWORD BATTLELIST_ADDON = 116; 
static DWORD BATTLELIST_LIGHT = 120;
static DWORD BATTLELIST_LIGHT_COLOR = 124;
static DWORD BATTLELIST_LIGHT_PATTEN = 127;
static DWORD BATTLELIST_HPBAR = 136;
static DWORD BATTLELIST_WALK_SPEED = 140;
static DWORD BATTLELIST_ISVISIBLE = 144;
static DWORD BATTLELIST_SKULL = 148; 
static DWORD BATTLELIST_PARTY = 152;   
static DWORD BATTLELIST_WARICON = 160;
static DWORD BATTLELIST_BLOCKING = 164;

#endif
