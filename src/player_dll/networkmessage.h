//---------------------------------------------------------------------------
//
// Name:        networkmessage.h
// Author:      ex0ticprgmr 2020 & beziak 2009-2011
// Description: BynaCam is free Tibia movie recording & playing software.
//              BynaCam Reloaded v2 is using Tibia GUI engine
//              Designed exclusively for Tibia 8.6
// BynaCam Player is placed in public domain.
// Anyone is free to copy, modify, publish, use, compile, sell, or distribute the original BynaCam code, 
// either in source code form or as a compiled binary, for any purpose, commercial or non-commercial, and by any means.
//---------------------------------------------------------------------------

#ifndef _NETWORKMESSAGE_H_
#define _NETWORKMESSAGE_H_

#define DEFAULT_DATA_SIZE 0xFFFF
#define MOD_ADLER 65521
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
        
        void WriteBytes(char *bytes, int len);
        void WriteInt32(int value);
        void WriteInt16(short value);
        void WriteByte(char byte);
        void WriteString(string str);
        
        std::string ReadString();
        char ReadByte();
        
        unsigned char* data;
        unsigned int offset;
        
        void PrepareToSend(long* xtea);
    
    private:
        bool Allocate(int bytes);
        void tean(long *v, long *k, long N);
        void XTEAEncode(unsigned char *lpBuffer, DWORD dwSize, long* lpXteaKey);
        int Adler32(unsigned char *data, size_t len);

};

#endif
