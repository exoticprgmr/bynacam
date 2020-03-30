//---------------------------------------------------------------------------
//
// Name:        file.h
// Author:      ex0ticprgmr 2020 & beziak 2009-2011
// Description: BynaCam is free Tibia movie recording & playing software.
//              BynaCam Reloaded v2 is using Tibia GUI engine
//              Designed exclusively for Tibia 8.6
// BynaCam Reloaded v2 is placed in public domain.
// Anyone is free to copy, modify, publish, use, compile, sell, or distribute the original BynaCam code, 
// either in source code form or as a compiled binary, for any purpose, commercial or non-commercial, and by any means.
//---------------------------------------------------------------------------

#ifndef FILE_H
#define FILE_H

#include <time.h>
#include <windows.h>
#include <stdio.h>
#include <sstream>
#include <fstream>
#include "LZMA\LzmaLib.h"
#include "var.h"

using namespace std;

extern "C" bool exiting;

class FileHandler
{
public:
        FileHandler();
        
        void Open();
        int Close();
        
        void WritePacket(unsigned char *cBuffer, unsigned int nSize, bool nodelay = false, clock_t delayt = 0);
        void ResetDelay(); 
        
        bool isOpen();
        bool isOpening();
        bool isSaving();
        unsigned int totalTime;
        unsigned int delayTime;
        clock_t startClock;
        unsigned int numOfPackets;
        bool isopen;
private:
        std::string fData;
        void WriteTime(bool nodelay);
        void Write(unsigned char *cBuffer, int nSize);
        void Write(int value, int nSize);
        
        FILE* pFile;  // File stream
        std::string OpenFileDialog();
        bool opening;
        bool saving;
        bool savingpacket;
        char file[MAX_PATH];
        
        unsigned int chksum_crc32(unsigned char *block, unsigned int length);
        unsigned int crc_tab[256];
};

#endif
