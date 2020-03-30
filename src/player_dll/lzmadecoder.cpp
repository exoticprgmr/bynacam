//---------------------------------------------------------------------------
//
// Name:        lzmadecoder.cpp
// Author:      ex0ticprgmr 2020 & beziak 2009-2011
// Description: BynaCam is free Tibia movie recording & playing software.
//              BynaCam Reloaded v2 is using Tibia GUI engine
//              Designed exclusively for Tibia 8.6
// BynaCam Player is placed in public domain.
// Anyone is free to copy, modify, publish, use, compile, sell, or distribute the original BynaCam code, 
// either in source code form or as a compiled binary, for any purpose, commercial or non-commercial, and by any means.
//---------------------------------------------------------------------------

#include "lzmadecoder.h"

MY_STDAPI LzmaUncompress(unsigned char *dest, size_t *destLen, const unsigned char *src, SizeT *srcLen,
  const unsigned char *props, size_t propsSize);

LzmaDecoder::LzmaDecoder(const uint8_t* data, const uint8_t* props, uint32_t inLen, uint32_t outlen)
{                            
    out = new uint8_t[outlen];     
    if (LzmaUncompress(out, &outlen, data, &inLen, props, 5) != SZ_OK)
       {
           delete[] out;    
           out = NULL;                   
       }
}    

LzmaDecoder::~LzmaDecoder()
{
    delete[] out;    
    out = NULL;                  
}                

uint8_t* LzmaDecoder::GetBuffer()
{
    return out;     
}                                  
