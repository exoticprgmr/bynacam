//---------------------------------------------------------------------------
//
// Name:        networkmessage.h
// Author:      ex0ticprgmr 2020 & beziak 2009-2011
// Description: BynaCam is free Tibia movie recording & playing software.
//              BynaCam Reloaded v2 is using Tibia GUI engine
//              Designed exclusively for Tibia 8.6
// BynaCam Reloaded v2 is placed in public domain.
// Anyone is free to copy, modify, publish, use, compile, sell, or distribute the original BynaCam code, 
// either in source code form or as a compiled binary, for any purpose, commercial or non-commercial, and by any means.
//---------------------------------------------------------------------------

#ifndef _NETWORKMESSAGE_H_
#define _NETWORKMESSAGE_H_

#define DEFAULT_DATA_SIZE 131072UL
#include <windows.h>
#include <string>

using namespace std;

class NetworkMessage
{
    public:
        NetworkMessage();
        NetworkMessage(int size);
        NetworkMessage(unsigned char* p, int size);
        ~NetworkMessage();
        void Clear();
        
        void WriteBytes(char *bytes, int len);
        void WriteUInt32(int value);
        void WriteUInt16(short value);
        void WriteByte(char byte);
        void WriteUInt64(__int64 value);
        void WriteString(string str);
        
        char ReadByte();
        int ReadUInt32();
        short ReadUInt16();
        void ReadBytes(LPBYTE ptr, int len);
        
        unsigned char* data;
        unsigned int offset;
    
    private:
        bool Allocate(int bytes);

};

#endif
