#include "MoMysql.h"

const char *g_host_name = "127.0.0.1";
const char *g_user_name = "metoak";
const char *g_password = "m123";
const char *g_db_name = "tcpserver";
const unsigned int g_db_port = 3306;

SystemDefault* pSystemDefault = new SystemDefault;

void destory_mysql(MOMySQL *pMysql){
  mysql_close(pMysql->g_conn);
    free(pMysql);
}

int executesql(const char * sql, MOMySQL *pMysql) {
    //query the database according the sql
    if (mysql_real_query(pMysql->g_conn, sql, strlen(sql))) 
    {
      printf("Write to SQL Failed!\n");
      return -1;
    }
    printf("Write to SQL Success!\n");
    return 0; 
}

int selectSqlCount(const char *sql , MOMySQL *pMysql){
 
  MYSQL_RES *pMysqlRes;
  if (mysql_real_query(pMysql->g_conn, sql, strlen(sql))) 
        return -1;
  pMysqlRes = mysql_store_result(pMysql->g_conn);

  int num = mysql_num_rows(pMysqlRes);
  
  return num;
}



void print_mysql_error(const char *msg, MOMySQL *pMysql) { 
    if (msg)
        printf("%s: %s\n", msg, mysql_error(pMysql->g_conn));
    else
        puts(mysql_error(pMysql->g_conn));
}


MOMySQL * init_mysql() 
{ 
  MOMySQL *pMysql =  (MOMySQL*)malloc(sizeof(MOMySQL));
  
  pMysql->g_conn = mysql_init(NULL);
    
    //connect the database 
    if(!mysql_real_connect(pMysql->g_conn, g_host_name, g_user_name, g_password, g_db_name, g_db_port, NULL, 0)) 
    {
      printf("Connect SQL Failed\n");
      return NULL;
    }
    printf("Connect SQL Success\n");
    return pMysql; 
}

void saveClientDeviceVer(MOMySQL *pMysql, TVer *pTVer, char* pTime)
{
    const char* SQL_CREATE_TABLE = "CREATE TABLE IF NOT EXISTS `mo_client_device_ver`(\
      time_stamp VARCHAR(100) NOT NULL PRIMARY KEY,\
      hw_version VARCHAR(100) NOT NULL,\
      sw_version VARCHAR(100) NOT NULL,\
      manufacturer VARCHAR(100) NOT NULL\
      )ENGINE=InnoDB DEFAULT CHARSET=utf8;";
    executesql(SQL_CREATE_TABLE,pMysql);

    const char* SQL_TMPL = "INSERT INTO `mo_client_device_ver`(`time_stamp`,`hw_version`,`sw_version`,`manufacturer`) VALUES ('%s','%s','%s','%s')";
    char arrSqlBuf[MAX_SQL_BUF_SIZE];
    sprintf(arrSqlBuf, SQL_TMPL, pTime, pTVer->strDeviceVer, pTVer->strHardwareVer, pTVer->strManufacturer);
    printf("Will Write DeviceVer To SQL\n:%s\n", arrSqlBuf);
    executesql(arrSqlBuf,pMysql);
}

void saveClientAlarmMsg(MOMySQL *pMysql, TAlarm *pTAlarm, char* pTime)
{
    const char* SQL_CREATE_TABLE = "CREATE TABLE IF NOT EXISTS `mo_client_alarm_msg`(\
      time_stamp VARCHAR(100) NOT NULL PRIMARY KEY,\
      dev_status1 BIGINT NOT NULL,\
      dev_status2 BIGINT NOT NULL,\
      war_status BIGINT NOT NULL,\
      media_type BIGINT NOT NULL,\
      media_id BIGINT NOT NULL,\
      media_len BIGINT NOT NULL\
      )ENGINE=InnoDB DEFAULT CHARSET=utf8;";
    executesql(SQL_CREATE_TABLE,pMysql);

    int i4State1Temp = pTAlarm->i4State1;
    int i4State2Temp = pTAlarm->i4State2;
    unsigned int u4MediaIDTemp = pTAlarm->u4MediaID;
    unsigned int u4MediaLengthTemp = pTAlarm->u4MediaLength;
    pSystemDefault->SwitchByte(i4State1Temp);
    pSystemDefault->SwitchByte(i4State2Temp);
    pSystemDefault->SwitchByte(u4MediaIDTemp);
    pSystemDefault->SwitchByte(u4MediaLengthTemp);

    const char* SQL_TMPL = "INSERT INTO `mo_client_alarm_msg` \
    (`time_stamp`,`dev_status1`,`dev_status2`,`war_status`,`media_type`,`media_id`,`media_len`)  VALUES  ('%s',%d,%d,%d,%d,%d,%d)";
    char arrSqlBuf[MAX_SQL_BUF_SIZE];    
    sprintf(arrSqlBuf, SQL_TMPL, pTime, i4State1Temp, i4State2Temp, pTAlarm->i1AlarmState, pTAlarm->i1MediaType, u4MediaIDTemp, u4MediaLengthTemp);
    printf("Will Write AlarmMsg To SQL\n:%s\n", arrSqlBuf);
    executesql(arrSqlBuf,pMysql);
}


void saveClientMediaMsg(MOMySQL *pMysql,  unsigned int u4Len, unsigned char* pMedia, char* pTime)
{
    const char* SQL_CREATE_TABLE = "CREATE TABLE IF NOT EXISTS `mo_client_media_buf`(\
      time_stamp VARCHAR(100) NOT NULL PRIMARY KEY,\
      media_len BIGINT NOT NULL,\
      media_Buf TEXT(102400) NOT NULL\
      )ENGINE=InnoDB DEFAULT CHARSET=utf8;";
    executesql(SQL_CREATE_TABLE,pMysql);

    unsigned int u4LenTemp = u4Len;
    pSystemDefault->SwitchByte(u4LenTemp);
    const char* SQL_TMPL = "INSERT INTO `mo_client_media_buf`(`time_stamp`,`media_len`,`media_Buf`) VALUES ('%s','%d','%s')";
    char arrSqlBuf[MAX_SQL_BUF_SIZE];
    sprintf(arrSqlBuf, SQL_TMPL, pTime, &u4LenTemp, pMedia);
    //printf("Will Write MediaBuf To SQL\n:%s\n", arrSqlBuf);
    executesql(arrSqlBuf,pMysql);
    printf("-----------------1-------------------1-----\n");
    usleep(1000);
}