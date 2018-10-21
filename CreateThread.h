#ifndef CREATETHREAD_H
#define CREATETHREAD_H
#include <iostream>
#include <pthread.h> 
#include <string>
#include <cstring>
#include <iostream>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include "CJRingBuffer.h"
#include "AboutData.h"
#include "MoMysql.h"

using namespace std;

#define SIZE_OF_MEDIA 102400
#define BUFFER_SIZE 1024
#define NUM_THREADS 3

static int  sockfdTimer;
pthread_mutex_t mut;
bool bIsConnected = 0;
CJRing g_ringBuf;
unsigned short u2ReadSize;  //每次获取文件的大小
unsigned int   u4TempTolLen;
unsigned short u2tempLen;              //本次获取的多媒体文件的长度
unsigned int u4tempId;
unsigned int u4tempStart;              //本次获取的多媒体文件的起始地址
unsigned int u4nextStart;                //下一次读取文件的起始地址
unsigned char u1BufferToSaveMedia[SIZE_OF_MEDIA]; 
unsigned char u1BufferToSaveMediaTemp[SIZE_OF_MEDIA]; 

BinParser* pBinParser = new BinParser;
MOMySQL *pMySql;

bool IsSocketClosed(int clientSocket)
{
     char buff[32];
     int recvBytes = recv(clientSocket, buff, sizeof(buff), MSG_PEEK);
     int sockErr = errno;
     if( recvBytes > 0) //Get data
        return false; 
     if( (recvBytes == -1) && (sockErr == EWOULDBLOCK) ) //No receive data
        return false;   
     return true;
}

void AskGetImage(void *threadid, unsigned int u4Start)
{
    unsigned short u2CRCOut;
    unsigned int   u4Temp = u4Start;                  //获取文件的起始地址,第一次默认为0，以后依次加上已获取文件的长度
    unsigned int   u4TempId;    
    if(u4Start==0)
    {
          u4TempId = pBinParser->g_TAlarm.u4MediaID;         //在报警状态检测中检测到多媒ID，没有转序
    }
    else
    {
         u4TempId = pBinParser->g_TGetImage.u4ID;
    }
    unsigned char  mulType = pBinParser->g_TAlarm.i1MediaType;         //在报警中检测到的对媒体类型
    unsigned int u4TolLong = pBinParser->g_TAlarm.u4MediaLength;       //获取的多媒体文件的总长度
    pSystemDefault->SwitchByte(u4TolLong);

    //判断每次读取的多媒体文件的长度
    if((u4Temp + FILESIZE) > u4TolLong)
    {
        u2ReadSize = (u4TolLong - u4Temp);
    }
    else
    {
        u2ReadSize = FILESIZE;
    }

    //组装发送请求信息
    

    dataAskMedia[8] = (g_u2CountNum >> 8) & 0xFF;
    dataAskMedia[9] = (g_u2CountNum) & 0xFF;
    g_u2CountNum++;

    //将设备状态和报警状态读到的信息填充到请求数据中
    memcpy(&dataAskMedia[11], &u4TempId, sizeof(u4TempId));
    memcpy(&dataAskMedia[15],&mulType,sizeof(mulType));
    pSystemDefault->SwitchByte(u4Temp);
    memcpy(&dataAskMedia[16], &u4Temp, sizeof(u4Temp));

    unsigned short u2eachReadImageSize = u2ReadSize;
    pSystemDefault->SwitchByte(u2eachReadImageSize);
    memcpy(&dataAskMedia[20], &u2eachReadImageSize, sizeof(u2eachReadImageSize));

    pSystemDefault->CRC16(dataAskMedia, sizeof(dataAskMedia)-2, &u2CRCOut);
    pSystemDefault->SwitchByte(u2CRCOut);
    memcpy(&dataAskMedia[22], &u2CRCOut, sizeof(u2CRCOut));
    send(*((int*)threadid), dataAskMedia,sizeof(dataAskMedia),0);
}

void *thread_RecvData(void *threadid)
{
     sockfdTimer = *((int*)threadid);
      int i;
      printf("thread_RecvData : I'm thread 1\n");  
       char buffer[BUFFER_SIZE];
       int len;
       while(1)
       {
            if(IsSocketClosed(sockfdTimer))
            {
              printf("The connect is Disconnect!\n");
              bIsConnected = 0;
              alarm(0); //Stop signal
              break;
            }
            len = recv(sockfdTimer, buffer, sizeof(buffer),0);
            //printf("RecvDate-len:%d   data:%s\n", len, buffer);            
            pthread_mutex_lock(&mut);
            for (int i = 0; i < len; i++)
            {
                g_ringBuf.PushBack(buffer[i]);        
            }
            pthread_mutex_unlock(&mut);
            usleep(1000);
      }
      printf("thread_RecvData has been freeed\n");
      pthread_exit(NULL);
}


void *thread_ReadData(void *threadid)
{
       printf("thread_ReadData : I'm thread 2\n");
       unsigned char buf;
       string strTemp;
        while(bIsConnected)
        {            
              //printf("\nRead Data:");
               pthread_mutex_lock(&mut);
               if (g_ringBuf.Get(buf))
               {  
                     switch(pBinParser->BIN_ParserFrame((char)buf))
                     {
                            case 0x8080:
                            {
                               printf("-----------------8080H\n");
                               break;
                            }
                            case 0xACC6:
                            {
                               printf("-----------------ACC6H\n");

                               //固件版本号
                                memcpy(&pBinParser->g_TVer.i1len[0], &pBinParser->m_BuffRecv[11], sizeof(pBinParser->g_TVer.i1len[0]));
                                memcpy(&pBinParser->g_TVer.strDeviceVer, &pBinParser->m_BuffRecv[12], pBinParser->g_TVer.i1len[0]);
                                //pBinParser->g_TVer.strDeviceVer = temp;

                                //硬件版本号
                                memcpy(&pBinParser->g_TVer.i1len[1], &pBinParser->m_BuffRecv[12+pBinParser->g_TVer.i1len[0]], sizeof(pBinParser->g_TVer.i1len[1]));
                                memcpy(pBinParser->g_TVer.strHardwareVer,&pBinParser->m_BuffRecv[13+pBinParser->g_TVer.i1len[0]], pBinParser->g_TVer.i1len[1]);

                                 //制造商名字
                                memcpy(&pBinParser->g_TVer.i1len[2], &pBinParser->m_BuffRecv[13+pBinParser->g_TVer.i1len[0]+pBinParser->g_TVer.i1len[1]], sizeof(pBinParser->g_TVer.i1len[2]));
                                memcpy(pBinParser->g_TVer.strManufacturer,&pBinParser->m_BuffRecv[14+pBinParser->g_TVer.i1len[0]+pBinParser->g_TVer.i1len[1]], pBinParser->g_TVer.i1len[2]);

                                printf("%s\n", pBinParser->g_TVer.strDeviceVer);
                                printf("%s\n", pBinParser->g_TVer.strHardwareVer);
                                printf("%s\n", pBinParser->g_TVer.strManufacturer);


                                strTemp = pBinParser->g_TVer.strDeviceVer;
                                if(strstr(strTemp.c_str(), "ADAS") != NULL)
                                {
                                    g_u1DevideType = DEVICE_ADAS;
                                    //printf("The Device is ADAS\n");
                                }
                                else if(strstr(strTemp.c_str(), "ADAS") != NULL)
                                {
                                    g_u1DevideType = DEVICE_DMS;
                                    //printf("The Device is DMS\n");
                                }
                                g_u1DataSendType = MODEL_ASKALARM; 

                                time_t t = time(0); 
                                char tmp[12]; 
                                strftime( tmp, sizeof(tmp), "%y%m%d%H%M%S",localtime(&t) ); 
                                saveClientDeviceVer(pMySql, &pBinParser->g_TVer, tmp);
                                break;
                            }
                            case 0xACC8:
                            {
                               printf("-----------------ACC8H\n");

                               memset(&pBinParser->g_TAlarm, 0, sizeof(pBinParser->g_TAlarm));
                               memcpy(&pBinParser->g_TAlarm, &pBinParser->m_BuffRecv[11], sizeof(pBinParser->g_TAlarm));
                               memcpy(&pBinParser->g_TMsgHeader,&pBinParser->m_BuffRecv[4],sizeof(pBinParser->g_TMsgHeader));
                               if ((DEVICE_ADAS == g_u1DevideType) && (pBinParser->g_TAlarm.i1AlarmState))
                               {
                                   printf("Ready to Ask Media\n");
                                   time_t t = time(0); 
                                  char tmp[12]; 
                                  strftime( tmp, sizeof(tmp), "%y%m%d%H%M%S",localtime(&t) ); 
                                   saveClientAlarmMsg(pMySql, &pBinParser->g_TAlarm, tmp);
                                   AskGetImage(threadid, 0);
                               }
                               break;
                            }
                            case 0xACC9:
                            {
                               printf("-----------------ACC9H\n");

                               memset(&pBinParser->g_TGetImage, 0, sizeof(pBinParser->g_TGetImage));
                               memcpy(&pBinParser->g_TGetImage, &pBinParser->m_BuffRecv[11], sizeof(pBinParser->g_TGetImage));

                               u4TempTolLen=pBinParser->g_TAlarm.u4MediaLength;
                               pSystemDefault->SwitchByte(u4TempTolLen);

                               u2tempLen=pBinParser->g_TGetImage.u2Len;
                               pSystemDefault->SwitchByte(u2tempLen);

                               u4tempId=pBinParser->g_TGetImage.u4ID;
                               pSystemDefault->SwitchByte(u4tempId);

                               u4tempStart=pBinParser->g_TGetImage.u4Start;
                               pSystemDefault->SwitchByte(u4tempStart);

                               if (u2tempLen<=0)
                               {
                                   printf("Warning: No Media:the Media Length is %d!\n", u2tempLen);
                               }

                               if(0 == u4tempStart)
                               {
                                       memcpy(&pBinParser->g_TGetImageEx, &pBinParser->m_BuffRecv[21], sizeof(pBinParser->g_TGetImageEx));
                                       unsigned short headImageValidFlag = pBinParser->g_TGetImageEx.u2Flag;
                                       unsigned short headImageSpeed = pBinParser->g_TGetImageEx.u2Speed;
                                       pSystemDefault->SwitchByte(headImageSpeed);
                                       pSystemDefault->SwitchByte(headImageValidFlag);
                                       if(headImageValidFlag == 0xFF00)
                                       {
                                                memset(u1BufferToSaveMedia, 0, sizeof(u1BufferToSaveMedia));
                                                memcpy(u1BufferToSaveMedia, &pBinParser->m_BuffRecv[33], u2tempLen);

                                               //判断是否读取完毕
                                               if(u4TempTolLen <= (u4tempStart + u2tempLen))
                                               {
                                                    printf("Error:The media is too small, the size is %d\n", u2tempLen);
                                                    break;
                                               }
                                               else
                                               {
                                                   u4nextStart = u4tempStart + u2tempLen;
                                                   printf("Have recvive %d Media data!\n", u4nextStart);
                                                   AskGetImage(threadid, u4nextStart);
                                               }
                                       }
                                       else
                                       {
                                               printf("Warning:The media data is invalid, the flag is not 0xFF00\n");
                                               break;
                                       }
                               }
                               else
                               {
                                       memcpy(&u1BufferToSaveMedia[u4tempStart], &pBinParser->m_BuffRecv[21], u2tempLen);
                                       
                                       if(u4TempTolLen <= (u4tempStart + u2tempLen))
                                       {
                                            time_t t = time(0); 
                                            char strtemp[128];
                                            strftime( strtemp, sizeof(strtemp), "%y%m%d_%H%M%S.jpeg",localtime(&t) );  
                                            cout<<"---------------The file is saved at :"<<strtemp<<endl;      
                                            int fd=creat(strtemp,00777);
                                            write(fd,u1BufferToSaveMedia,u4TempTolLen);
                                            close(fd);

                                            memset(u1BufferToSaveMediaTemp, 0, sizeof(u1BufferToSaveMediaTemp));
                                            base64_encode((char*)u1BufferToSaveMedia, u4TempTolLen, (char*)u1BufferToSaveMediaTemp) ;

                                            time_t tt = time(0); 
                                            char tmp[12]; 
                                            strftime( tmp, sizeof(tmp), "%y%m%d%H%M%S",localtime(&tt) ); 
                                            saveClientMediaMsg(pMySql, u4TempTolLen, u1BufferToSaveMediaTemp, tmp);

                                            printf("Have recvive %d Media data   and the total size is %d!\n", u4tempStart + u2tempLen, u4TempTolLen);
                                            printf("The media data has been received!\n");
                                            break;
                                       }
                                       else
                                       {
                                           u4nextStart = u4tempStart + u2tempLen;
                                           printf("Have recvive %d Media data!\n", u4nextStart);
                                           AskGetImage(threadid, u4nextStart);
                                       }
                               }

                               break;
                            }

                            default:
                                break;
                     }
               }     
               pthread_mutex_unlock(&mut);
               usleep(500);
         } 
          pthread_exit(NULL);
}

void *thread_AskVer(void *threadid)
{
       unsigned short u2Count;
       unsigned short u2CRC;
       printf("thread_AskVer : I'm thread 3\n");
       while(1)
       {
              u2Count = g_u2CountNum;
              if (!bIsConnected)
              {
                pthread_exit(NULL);
                break;
              }
            switch(g_u1DataSendType)
            {
                case MODEL_ASKVER:
                    {
                        pSystemDefault->SwitchByte(u2Count);
                        memcpy(&dataAskVer[8], &u2Count, sizeof(u2Count));
                        pSystemDefault->CRC16(dataAskVer, sizeof(dataAskVer)-2, &u2CRC);
                        pSystemDefault->SwitchByte(u2CRC);
                        memcpy(&dataAskVer[sizeof(dataAskVer)-2], &u2CRC, sizeof(u2CRC));
                        send(*((int*)threadid), dataAskVer,sizeof(dataAskVer),0);
                        printf("thread_AskVer : -------------%d\n", g_u2CountNum); 
                    }
                    break;
                case MODEL_ASKALARM:
                    {
                            time_t t = time(0); 
                            char tmp[12]; 
                            strftime( tmp, sizeof(tmp), "%y%m%d%H%M%S",localtime(&t) ); 
                            for (int i = 0; i < 6; ++i)
                            {
                                dataAskAlarm[12+i] = (tmp[2*i] - '0' )*10 + (tmp[2*i+1] - '0');
                            }

                            pSystemDefault->SwitchByte(u2Count);
                            memcpy(&dataAskAlarm[8], &u2Count, sizeof(u2Count));
                            dataAskAlarm[10] = g_u1DevideType;
                            dataAskAlarm[11] = 0x06;
                            dataAskAlarm[18] = 0x02;

                            pSystemDefault->CRC16(dataAskAlarm, sizeof(dataAskAlarm)-2, &u2CRC);
                            pSystemDefault->SwitchByte(u2CRC);
                            memcpy(&dataAskAlarm[sizeof(dataAskAlarm)-2], &u2CRC, sizeof(u2CRC));
                            send(*((int*)threadid), dataAskAlarm,sizeof(dataAskAlarm),0);


                            printf("thread_AskAlarm : -------------%d\n", g_u2CountNum);
                    }
                    break;
                default:
                    break;
            }
            g_u2CountNum++;            
            
            sleep(1);
       }
       
}

void createThread(int conn)
{
  //m_stTest.num2=conn;
    //开辟一个多线程读取socket Duff 
   pMySql = init_mysql();
   pthread_t threads[NUM_THREADS];
   int indexes[NUM_THREADS];// 用数组来保存i的值
   int rc[NUM_THREADS];
   memset(&threads, 0, sizeof(threads)); 
   for(int i=0; i < NUM_THREADS; i++ )
   {      
          cout << "Create Thread to read Socket Buff!   Serial No:" << i << endl;
          indexes[i] = i; //先保存i的值
          // 传入的时候必须强制转换为void* 类型，即无类型指针        
          //rc = pthread_create(&threads[i], NULL,PrintHello, (void *)&(indexes[i]));
          //m_stTest.num1=indexes[i];          
          switch(i)
          {
                case 0:
                  rc[i] = pthread_create(&threads[i], NULL,thread_RecvData, (void *)&(conn));
                  usleep(1000);
                  break;
                case 1:
                  rc[i] = pthread_create(&threads[i], NULL,thread_ReadData, (void *)&(conn));
                  usleep(1000);
                  break;
                case 2:
                  rc[i] = pthread_create(&threads[i], NULL,thread_AskVer, (void *)&(conn));
                  usleep(1000);
                  break;
                default:
                  break;
          }
          if (rc[i])
          {
                  cout << "Error:Cannot create Thread!" << rc << endl;
                  exit(-1);
          }
      
    }
   
}

#endif