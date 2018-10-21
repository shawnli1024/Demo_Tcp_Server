#include "systemdefault.h"

SystemDefault::SystemDefault()
{

}

void SystemDefault::SwitchByte(unsigned short& u2Data)
{
    u2Data = ((u2Data & 0xff00) >> 8) | ((u2Data & 0x00ff) << 8);
}
void SystemDefault::SwitchByte(short& u2Data)
{
    u2Data = ((u2Data & 0xff00) >> 8) | ((u2Data & 0x00ff) << 8);
}
void SystemDefault::SwitchByte(unsigned int& u4Data)
{
    u4Data = ((u4Data & 0xff000000) >> 24) | ((u4Data & 0xff0000) >> 8) | ((u4Data & 0xff00) << 8) | ((u4Data & 0xff) << 24);
}
void SystemDefault::SwitchByte(int& u4Data)
{
    u4Data = ((u4Data & 0xff000000) >> 24) | ((u4Data & 0xff0000) >> 8) | ((u4Data & 0xff00) << 8) | ((u4Data & 0xff) << 24);
}
/*
void SystemDefault::SwitchByte(unsigned float& u4Data)
{
    u4Data = ((u4Data & 0xff000000) >> 24) | ((u4Data & 0xff0000) >> 8) | ((u4Data & 0xff00) << 8) | ((u4Data & 0xff) << 24);
}
void SystemDefault::SwitchByte(float& u4Data)
{
    u4Data = ((u4Data & 0xff000000) >> 24) | ((u4Data & 0xff0000) >> 8) | ((u4Data & 0xff00) << 8) | ((u4Data & 0xff) << 24);
}
*/

void SystemDefault::IntToHex(unsigned short u2data, char &p)
{
    p = (u2data/10 <<4) | u2data%10;
}
void SystemDefault::SwitchByte(char &data1, char &data2)
{
    char data = data1;
    data1 = data2;
    data2 = data;
}

void SystemDefault::CRC16(unsigned char *data, int length, unsigned short* pCRCOut)
{
    uint16_t quotient,i,j;
    uint16_t remainder,divisor=0x1021;
    uint16_t data1;
    uint16_t extra_crc_len = 2;
    remainder=0;
    for(j=0;j<length;j++)
    {
        data1=data[j];
        for(i=8;i>0;i--)
        {
            quotient=remainder&0x8000;
            remainder<<=1;
            if((data1<<=1)&0x0100)
                remainder|=1;
            if(quotient)
                remainder^=divisor;
        }
    }
    while(extra_crc_len--)
    {
        data1=0x00;
        for(i=8;i>0;i--)
        {
            quotient=remainder&0x8000;
            remainder<<=1;
            if((data1<<=1)&0x0100)
                remainder|=1;
            if(quotient)
                remainder^=divisor;
        }
    }
    *pCRCOut = remainder;
}
