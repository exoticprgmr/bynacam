//---------------------------------------------------------------------------
//
// Name:        file.h
// Author:      ex0ticprgmr 2020 & beziak 2009-2011
// Description: BynaCam is free Tibia movie recording & playing software.
//              BynaCam Reloaded v2 is using Tibia GUI engine
//              Designed exclusively for Tibia 8.6
// BynaCam Player is placed in public domain.
// Anyone is free to copy, modify, publish, use, compile, sell, or distribute the original BynaCam code, 
// either in source code form or as a compiled binary, for any purpose, commercial or non-commercial, and by any means.
//---------------------------------------------------------------------------


#ifndef _FILE_H_
#define _FILE_H_

#include <string>
#include <windows.h>

using namespace std;

class File
{
      public:
             File(const string name);
             ~File();
             
             uint8_t* GetBuffer();
             uint32_t GetSize();
             bool IsGood();
      private:
              FILE* pFile;
              bool memAllocated;
              
              uint8_t* data;
              uint32_t size;
              VOID MemAlloc(DWORD size);
              VOID MemDealloc();
};

#endif
