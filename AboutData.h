#ifndef ABOUTDATA_H
#define ABOUTDATA_H

enum 
{ 
    MODEL_ASKVER, 
    MODEL_ASKALARM
};

enum 
{ 
    DEVICE_ADAS=1, 
    DEVICE_DMS = 2
};

#define FILESIZE 1024

unsigned char g_u1DevideType;
unsigned char g_u1DataSendType = MODEL_ASKVER;

unsigned short g_u2CountNum = 0;
unsigned char dataAskVer[13]    = {0xAA, 0x55, 0xCC, 0x33, 0x00, 0x0D, 0xAC, 0xC6};
unsigned char dataAskAlarm[23] = {0xAA, 0x55, 0xCC, 0x33, 0x00, 0x17, 0xAC, 0xC8};
unsigned char dataAskMedia[24] = {0xAA, 0x55, 0xCC, 0x33, 0x00, 0x18, 0xAC, 0xC9};



#endif
