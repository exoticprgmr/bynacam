//---------------------------------------------------------------------------
//
// Name:        lzmadecoder.h
// Author:      ex0ticprgmr 2020 & beziak 2009-2011
// Description: BynaCam is free Tibia movie recording & playing software.
//              BynaCam Reloaded v2 is using Tibia GUI engine
//              Designed exclusively for Tibia 8.6
// BynaCam Player is placed in public domain.
// Anyone is free to copy, modify, publish, use, compile, sell, or distribute the original BynaCam code, 
// either in source code form or as a compiled binary, for any purpose, commercial or non-commercial, and by any means.
//---------------------------------------------------------------------------


#ifndef _LZMADECODER_H_
#define _LZMADECODER_H_

#include <stdint.h>
#include "LZMA/LzmaLib.h"

class LzmaDecoder
{
      public:
             LzmaDecoder(const uint8_t* data, const uint8_t* props, uint32_t inLen, uint32_t outlen);
             ~LzmaDecoder();
             
             uint8_t* GetBuffer();
      private:
             uint8_t* out; 
};

#endif
