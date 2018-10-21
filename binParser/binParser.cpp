#include "binParser.h"

long BinParser::g_nRevCnt = 0;
char BinParser::Flag = T_FLAG;
BinParser::BinParser()
{
    m_SD = new SystemDefault();
}
int BinParser::BIN_ParserFrame(char Buf)
{
    m_BuffRecv[g_nRevCnt++] = Buf;
        switch (Flag)
        {
        case T_FLAG:
            if (g_nRevCnt == 4)
            {
                if ((0xAA == m_BuffRecv[0]) && (0x55 == m_BuffRecv[1]) && (0xCC == m_BuffRecv[2]) && (0x33 == m_BuffRecv[3]))
                {
                    Flag = T_HEADER;
                }
                else
                {
                    m_BuffRecv[0] = m_BuffRecv[1] ;
                    m_BuffRecv[1] = m_BuffRecv[2] ;
                    m_BuffRecv[2] = m_BuffRecv[3] ;
                    g_nRevCnt = 3;
                }
            }
            break;
        case T_HEADER:
            //g_nRevCnt++;  //4-11
            if(g_nRevCnt>=11)
            {
                Flag = T_MSG;
                memcpy(&g_TMsgHeader, &m_BuffRecv[4], sizeof(g_TMsgHeader));
                m_SD->SwitchByte(g_TMsgHeader.u2Length);
                m_SD->SwitchByte(g_TMsgHeader.u2ID);
                m_SD->SwitchByte(g_TMsgHeader.u2SerialNo);
                break;
            }
            break;
        case T_MSG:
            //g_nRevCnt++;  //12-end
            if(g_nRevCnt>=g_TMsgHeader.u2Length)
            {
                g_nRevCnt = 0;
                unsigned short u2_CRCOut =0;
                unsigned short u2_Test = 0;
                m_SD->CRC16(m_BuffRecv, g_TMsgHeader.u2Length-2, &u2_CRCOut);
                m_SD->SwitchByte(u2_CRCOut);
                memcpy(&u2_Test, &m_BuffRecv[g_TMsgHeader.u2Length-2], sizeof(u2_Test));
                if(u2_Test == u2_CRCOut)
                {

                    Flag = T_FLAG;
                    return g_TMsgHeader.u2ID;
                }
            }
            break;
        default:
            g_nRevCnt = 0;
            Flag = T_FLAG;
            break;
        }
        return 0;  //必须return 0。不然的话当switch语句break时会返回上一次的结果，解帧显示错误
}

double BinParser::power(double base, int exponent)
{
    int result = base;
    int i;
    
    if (exponent == 0) {
        return 1;
    }
    
    for(i = 1; i < exponent; ++i){
        result = result * base;
    }
    return result;
}