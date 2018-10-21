#ifndef BINPARSER_H
#define BINPARSER_H

#include "systemdefault.h"
#include <string>
#include <string.h>

#define MAX_LENTH     5*1024
#pragma pack(1)
enum ENUM_UART_STREAM_TYPE
{
    T_FLAG,
    T_HEADER,
    T_MSG,
    T_CHECK,
    T_DEFAULT
};

typedef struct _tMsgHeader
{
    unsigned short u2Length;
    unsigned short u2ID;
    unsigned short u2SerialNo;
    unsigned char u1Type;
}TMsgHeader;

typedef struct _tVerH
{
    unsigned char i1len[3];
    char strDeviceVer[256];//固件版本号
    char strHardwareVer[256];//硬件版本号
    char strManufacturer[256];//制造商名字
}TVer;

typedef struct _tUpdateFile
{
    char i1IsSuccess;
    unsigned int u4Start;
    unsigned short u2Length;
}TUpdateFile;

typedef struct _tAlarmH
{
    int i4State1;
    int i4State2;
    char i1AlarmState;
    char i1MediaType;
    unsigned int u4MediaID;
    unsigned int u4MediaLength;
}TAlarm;



typedef struct _tGetImage//应答数据的前10个自己对应的内容
{
    unsigned int u4ID;
    unsigned int u4Start;
    unsigned short u2Len;
}TGetImage;
typedef struct _tGetImageEx//图像头部数据的内容，12个字节
{
    unsigned char u1Len;
    unsigned short u2Flag;
    unsigned char u1Type;
    unsigned char u1Time[6];
    unsigned short u2Speed;
}TGetImageEx;

#pragma pack()


class BinParser
{
public:
    BinParser();
    int BIN_ParserFrame(char Buf);
    double power(double base, int exponent);
    SystemDefault *m_SD;
    TVer g_TVer;
    TAlarm g_TAlarm;
    TUpdateFile g_TUpdateFile;      //中间数据，如果确认接收成功则将其传递给本地稳定的数据
    TUpdateFile g_TUpdateFileLocal; //本地数据，保存每次接收结果的状态值
    TGetImage g_TGetImage;
    TGetImageEx g_TGetImageEx;
    TMsgHeader g_TMsgHeader;
    unsigned char m_BuffRecv[MAX_LENTH];
    static long g_nRevCnt;
    static char Flag;
};

#endif // BINPARSER_H
