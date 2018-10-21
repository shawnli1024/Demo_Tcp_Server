#ifndef SYSTEMDEFAULT_H
#define SYSTEMDEFAULT_H
typedef unsigned short int uint16_t;

class SystemDefault
{
public:
    SystemDefault();
    void SwitchByte(unsigned short& u2Data);
    void SwitchByte(unsigned int& u4Data);
    void SwitchByte(short& u2Data);
    void SwitchByte(int& u4Data);
    //void SwitchByte(float& u4Data);
    //void SwitchByte(unsigned float& u4Data);

    void IntToHex(unsigned short u2data, char &p);
    void SwitchByte(char &data1, char &char2);
    void CRC16(unsigned char *data, int length, unsigned short* pCRCOut);

};

#endif // SYSTEMDEFAULT_H
