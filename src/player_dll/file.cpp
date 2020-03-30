//---------------------------------------------------------------------------
//
// Name:        file.cpp
// Author:      ex0ticprgmr 2020 & beziak 2009-2011
// Description: BynaCam is free Tibia movie recording & playing software.
//              BynaCam Reloaded v2 is using Tibia GUI engine
//              Designed exclusively for Tibia 8.6
// BynaCam Player is placed in public domain.
// Anyone is free to copy, modify, publish, use, compile, sell, or distribute the original BynaCam code, 
// either in source code form or as a compiled binary, for any purpose, commercial or non-commercial, and by any means.
//---------------------------------------------------------------------------


#include "file.h"

File::File(const string name)
{
    memAllocated = false;  
    size = 0;          
    pFile = fopen(name.c_str(), "rb");
    if(pFile!=NULL)
    {
        fseek (pFile, 0, SEEK_END);
        DWORD fsize = ftell (pFile);
        MemAlloc(fsize);
        rewind (pFile);
        if (memAllocated)
        {
           fread(data,1,fsize,pFile);
           size = fsize;
        }
        fclose(pFile);
    }  
}

File::~File()
{
    if (memAllocated)   
        MemDealloc();            
}

VOID File::MemAlloc(DWORD size)
{
    if (memAllocated)   
        MemDealloc();    
    data = new uint8_t[size]; 
    if (data != NULL)
       memAllocated = true;
}

VOID File::MemDealloc()
{
    delete[] data; 
    memAllocated = false;
    size = 0;
    data = NULL;
}


uint8_t* File::GetBuffer()
{
    return data;     
}

uint32_t File::GetSize()
{
    return size;     
}

bool File::IsGood()
{
    return size != 0 && data != NULL; 
}
