//---------------------------------------------------------------------------
//
// Name:        file.cpp
// Author:      ex0ticprgmr 2020 & beziak 2009-2011
// Description: BynaCam is free Tibia movie recording & playing software.
//              BynaCam Reloaded v2 is using Tibia GUI engine
//              Designed exclusively for Tibia 8.6
// BynaCam Reloaded v2 is placed in public domain.
// Anyone is free to copy, modify, publish, use, compile, sell, or distribute the original BynaCam code, 
// either in source code form or as a compiled binary, for any purpose, commercial or non-commercial, and by any means.
//---------------------------------------------------------------------------

#include "file.h"

FileHandler::FileHandler()
{
    isopen = false;
    opening = false;
    saving = false;
    savingpacket = false;
}

void FileHandler::Open()
{        
    opening = true;
    fData.clear();
    
    Write(0x06, 1); //unknown header
    Write(0x02, 1); //unknown header
    Write(0, 4); //packet num
        
    totalTime = 0;
    delayTime = 0;
    numOfPackets = 0;
    startClock = clock();
    savingpacket = false;
    opening = false;
    isopen = true;
    
    unsigned long crc, poly;
    poly = 0xEDB88320L;
    for (int i = 0; i < 256; i++)
    {
       crc = i;
       for (int j = 8; j > 0; j--)
       {
	       if (crc & 1)
	       {
	          crc = (crc >> 1) ^ poly;
	       }
	       else
	       {
	          crc >>= 1;
	       }
        }
        crc_tab[i] = crc;
     }    
}

int FileHandler::Close()
{     
    isopen = false;       
    saving = true;
    
    char szFile[MAX_PATH] = {0};  
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    
    if (exiting)
        ofn.hwndOwner = NULL;
    else
        ofn.hwndOwner = (*(HWND*)(TIBIA_HWND_ADDR));
    
    ofn.hInstance = NULL;
    ofn.lpstrFilter = "Cam Recordings (*.cam)\0*.cam\0"; 
    ofn.lpstrCustomFilter = NULL;
    ofn.nMaxCustFilter = 0;
    ofn.nFilterIndex = 0;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrTitle = "Save Movie As";
    ofn.Flags = OFN_HIDEREADONLY;
    ofn.nFileOffset = 0;
    ofn.nFileExtension = 0;
    ofn.lpstrDefExt = "cam";
    if (!GetSaveFileNameA(&ofn))
        {
           saving = false;
           isopen = true;
           return 2;
        }   
    strcpy(file, szFile);
    pFile = fopen(file, "wb");
    if (pFile == NULL)       
    {
        saving = false;
        isopen = false;
        MessageBox(GetForegroundWindow(), "Unable to save file in specified location!\r\nIf you are using Windows Vista or later turn off UAC\r\nor try to save file in different (non-protected) location.", PROGRAM_NAME, MB_OK | MB_ICONINFORMATION);
        return 0;
    }
    numOfPackets += 0x39;
    const unsigned char* containerData = (const unsigned char*)fData.c_str();
    unsigned int containerOffset = fData.length();
    *(unsigned int*)&containerData[2] = numOfPackets;
    size_t outLen = containerOffset + 5 + 8;
    unsigned char* outData = new unsigned char[outLen]; // + props + size
    size_t outPropsLen = 5;
    if (outData == NULL)
    {
        saving = false;
        return 0;
    }
    memset(outData, 0, outLen); //mem setting to 0
    *(unsigned long*)&outData[5] = containerOffset;    
    int ret = LzmaCompress(&outData[13], &outLen, (const unsigned char*)containerData, containerOffset, &outData[0], &outPropsLen, 5, (1 << 23), 3, 0, 2, 32, 2);
    if (ret != SZ_OK)
    {
        saving = false;
        delete[] outData;
        return 0;
    }
    const char hash[32] = {0};
    const int zero = 0;
    fwrite(hash, 1, 32, pFile); //write hash
    fwrite(TIBIA_VERSION, 1, 4, pFile); //writing tibia version
    fwrite(&zero, 1, 4, pFile); //write metadata length (0)
        
    outLen += 5 + 8;
    fwrite((const char*)&outLen, 1, 4, pFile); // write compressed len
    fwrite(outData, 1, outLen, pFile); //write main data
    fclose(pFile);
    delete[] outData;
    saving = false;
    return 1;
}

void FileHandler::Write(unsigned char *cBuffer, int nSize)
{  
    int capacity = fData.capacity();
    if ((fData.size() + nSize) > capacity) 
       fData.reserve(capacity + 2097152);
    fData.append((const char*)cBuffer, nSize);
}

void FileHandler::Write(int value, int nSize)
{
    Write ((unsigned char*)&value, nSize);
}

unsigned int FileHandler::chksum_crc32(unsigned char *block, unsigned int length)
{   
    uint32_t crc = 0xFFFFFFFF;
    for (int i = 0; i < length; i++)
    {
       crc = ((crc >> 8) & 0x00FFFFFF) ^ crc_tab[(crc ^ *block++) & 0xFF];
    }
    return (crc ^ 0xFFFFFFFF);
}

void FileHandler::WritePacket(unsigned char *cBuffer, unsigned int nSize, bool nodelay/* = false*/, clock_t delayt/* = 0*/)
{                      
    if (savingpacket)
       Sleep(0);
       
    savingpacket = true;
    numOfPackets++; 
    clock_t oldTimestamp = totalTime;
    
    Write(nSize + 2, 2);
    WriteTime(nodelay);
    Write(nSize, 2);
    Write(cBuffer,nSize);
    Write(chksum_crc32(cBuffer,nSize), 4); //crc
    
    savingpacket = false;
}

void FileHandler::WriteTime(bool nodelay)
{
    if (nodelay)
        delayTime = 0;
    else
        delayTime = clock() - startClock;
       
    startClock = clock();
    totalTime += delayTime;

    Write(totalTime, 4);
}

bool FileHandler::isOpen()
{
    return isopen;
}

bool FileHandler::isOpening()
{
    return opening;
}

bool FileHandler::isSaving()
{
    return saving;
}

void FileHandler::ResetDelay()
{
    startClock = clock();
}
