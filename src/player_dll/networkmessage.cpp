//---------------------------------------------------------------------------
//
// Name:        networkmessage.cpp
// Author:      ex0ticprgmr 2020 & beziak 2009-2011
// Description: BynaCam is free Tibia movie recording & playing software.
//              BynaCam Reloaded v2 is using Tibia GUI engine
//              Designed exclusively for Tibia 8.6
// BynaCam Player is placed in public domain.
// Anyone is free to copy, modify, publish, use, compile, sell, or distribute the original BynaCam code, 
// either in source code form or as a compiled binary, for any purpose, commercial or non-commercial, and by any means.
//---------------------------------------------------------------------------


#include "networkmessage.h"

NetworkMessage::NetworkMessage()
{
    if (!Allocate(DEFAULT_DATA_SIZE))
    throw -1;
}

NetworkMessage::NetworkMessage(int size)
{
    if (!Allocate(size))
    throw -1;
}

NetworkMessage::NetworkMessage(unsigned char* p, int size)
{
    if (!Allocate(size))
    throw -1;
    
    memcpy(&data[offset], p, size); 
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
        
    offset = 0;
}

void NetworkMessage::WriteBytes(char *bytes, int len)
{
        memcpy(&data[offset],bytes,len);
        offset += len;
}

void NetworkMessage::WriteInt32(int value)
{
        memcpy(&data[offset],&value,4);
        offset += 4;
}
void NetworkMessage::WriteInt16(short value)
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
    WriteInt16(pLen);
    WriteBytes((char*)str.c_str(), pLen);
}

char NetworkMessage::ReadByte()
{
    char ret = data[offset];
    offset++;
    return ret;
}

std::string NetworkMessage::ReadString()
{
    static char string[0xFFFF];
    ZeroMemory(string, 0xFFFF);
    short len = *(short*)&data[offset];
    offset += 2;
    memcpy(string, &data[offset], len);
    offset += len;
    return string;
}

void NetworkMessage::tean(long *v, long *k, long N)      /* replaces XTEA's code and decode */
{
    	unsigned long y = v[0],
                      z = v[1],
                      DELTA = 0x9E3779B9,
                      limit,
                      sum,
    				  i;

        if (N > 0)				/* encoding */
        {
            limit = DELTA * N;
            sum = 0;

            //while (sum != limit)
    		for(i=0; i<32; i++)
            {   y   += (((z << 4) ^ (z >> 5)) + z) ^ (sum + k[sum & 3]);
                sum += DELTA;
                z   += (((y << 4) ^ (y >> 5)) + y) ^ (sum + k[(sum >> 11) & 3]);
            }
    	}
        else				/* decoding */
        {   sum = 0xC6EF3720;

            //while (sum)
    		for(i=0; i<32; i++)
            {   z   -= (((y << 4) ^ (y >> 5)) + y) ^ (sum + k[(sum >> 11) & 3]);
                sum -= DELTA;
                y   -= (((z << 4) ^ (z >> 5)) + z) ^ (sum + k[sum & 3]);
            }
        }

    	v[0] = y;
    	v[1] = z;
        return;
}


void NetworkMessage::XTEAEncode(unsigned char *lpBuffer, DWORD dwSize, long* lpXteaKey)
{
        unsigned char *data = lpBuffer;
        while (dwSize%8 != 0)
        {
            offset++;
            dwSize++;
        }
            
    	for( unsigned int i=0; i < (dwSize/8); i++ )
    	{
    		tean( (long*)data, lpXteaKey, 32 );
    		data += 8;
    	}
}

int NetworkMessage::Adler32(unsigned char *data, size_t len)
{
        uint32_t a = 1, b = 0;
 
        while (len) {
                unsigned tlen = len > 5550 ? 5550 : len;
                len -= tlen;
                do {
                        a += *data++;
                        b += a;
                } while (--tlen);
                a = (a & 0xffff) + (a >> 16) * (65536-MOD_ADLER);
                b = (b & 0xffff) + (b >> 16) * (65536-MOD_ADLER);
        }
        /* It can be shown that a <= 0x1013a here, so a single subtract will do. */
        if (a >= MOD_ADLER)
                a -= MOD_ADLER;
        /* It can be shown that b can reach 0xffef1 here. */
        b = (b & 0xffff) + (b >> 16) * (65536-MOD_ADLER);
        if (b >= MOD_ADLER)
                b -= MOD_ADLER;
        return (b << 16) | a;
}

void NetworkMessage::PrepareToSend(long* xtea)
{
    XTEAEncode(&data[6], offset-6, xtea);
    *(int*)&data[2] = Adler32(&data[6], offset-6);
    *(short*)&data[0] = offset - 2;
}
