//---------------------------------------------------------------------------
//
// Name:        networkmessage.cpp
// Author:      ex0ticprgmr 2020 & beziak 2009-2011
// Description: BynaCam is free Tibia movie recording & playing software.
//              BynaCam Reloaded v2 is using Tibia GUI engine
//              Designed exclusively for Tibia 8.6
// BynaCam Reloaded v2 is placed in public domain.
// Anyone is free to copy, modify, publish, use, compile, sell, or distribute the original BynaCam code, 
// either in source code form or as a compiled binary, for any purpose, commercial or non-commercial, and by any means.
//---------------------------------------------------------------------------

#include "networkmessage.h"

NetworkMessage::NetworkMessage()
{
    Allocate(DEFAULT_DATA_SIZE);
    return;
}

NetworkMessage::NetworkMessage(int size)
{
    Allocate(size);
}

NetworkMessage::NetworkMessage(unsigned char* p, int size)
{
    Allocate(size);    
    memcpy(data, p, size); 
    offset = 0;
}

NetworkMessage::~NetworkMessage()
{
    offset = 0;
    delete[] data;
}


bool NetworkMessage::Allocate(int size)
{
    data = new unsigned char[size];
    offset = 0;

    if(data != NULL)
        {
                ZeroMemory(data, size);
                return true;
        } else
        return false;
}

void NetworkMessage::Clear()
{
        offset = 0;
}

void NetworkMessage::WriteBytes(char *bytes, int len)
{
        memcpy(&data[offset],bytes,len);
        offset += len;
}

void NetworkMessage::WriteUInt32(int value)
{
        memcpy(&data[offset],&value,4);
        offset += 4;
}
void NetworkMessage::WriteUInt64(__int64 value)
{
        memcpy(&data[offset],&value,8);
        offset += 8;
}
void NetworkMessage::WriteUInt16(short value)
{
        memcpy(&data[offset],&value,2);
        offset += 2;
}
void NetworkMessage::WriteByte(char byte)
{
        memcpy(&data[offset],&byte,1);
        offset += 1;
}
void NetworkMessage::WriteString(string str)
{
    int pLen = strlen(str.c_str());
    WriteUInt16(pLen);
    WriteBytes((char*)str.c_str(), pLen);
}

char NetworkMessage::ReadByte()
{
    char ret = data[offset];
    offset++;
    return ret;
}

int NetworkMessage::ReadUInt32()
{
    int ret = *(int*)&data[offset];
    offset += 4;
    return ret;
}

short NetworkMessage::ReadUInt16()
{
    short ret = *(int*)&data[offset];
    offset += 2;
    return ret;
}

void NetworkMessage::ReadBytes(LPBYTE ptr, int len)
{
    for (int i = 0; i < len; i++)
    {
        BYTE b = (BYTE)ReadByte();
        if (ptr != NULL)
           ptr[i] = b;
    } 
}
