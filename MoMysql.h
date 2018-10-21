#ifndef MO_MYSQL_H
#define MO_MYSQL_H
#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MoBase64.h"
#include "binParser/binParser.h"
#include "unistd.h"
#include "dataSwitchByte/systemdefault.h"


#define MAX_SQL_BUF_SIZE 102400

typedef struct MO_MYSQL_ {
  MYSQL *g_conn;
}MOMySQL;

extern SystemDefault* pSystemDefault;
MOMySQL * init_mysql();

void destory_mysql(MOMySQL *pMysql);

int executesql(const char * sql, MOMySQL *pMysql) ;
void print_mysql_error(const char *msg, MOMySQL *pMysql) ;

void saveClientDeviceVer(MOMySQL *pMysql, TVer *pTVer, char* pTime);
void saveClientAlarmMsg(MOMySQL *pMysql, TAlarm *pTAlarm, char* pTime);
void saveClientMediaMsg(MOMySQL *pMysql,  unsigned int u4Len, unsigned char* pMedia, char* pTime);

#endif