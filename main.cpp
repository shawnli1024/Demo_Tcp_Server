#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <signal.h>
#include <string>
#include <unistd.h>
#include <assert.h>
#include "mo_log/mo_log.h"
#include "CreateThread.h"


#define MEDIA_BUFFER_SIZE (10240)
static int g_DataCount=0;
#define PORT  8234
#define QUEUE_SIZE   10


int main(int argc, char *argv[])
{
    //解析功能测试
    mo_init_log(0,"");
    log_trace("System Started");
    int server_sockfd = socket(AF_INET,SOCK_STREAM, 0);
    int on;
    if((setsockopt(server_sockfd, SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)))<0)  
    {  
        perror("set sockopt failed");  
        return -1;  
    }  

/*------------------------------------------------*/

    struct sockaddr_in server_sockaddr;
    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_sockaddr.sin_port = htons(PORT);
 
    if(bind(server_sockfd,(struct sockaddr *)&server_sockaddr,sizeof(server_sockaddr))==-1)
    {
        perror("bind");
        exit(1);
    }
    
    log_trace("bind success");
    
    if(listen(server_sockfd,QUEUE_SIZE) == -1)
    {
        perror("listen");
        exit(1);
    }
    log_trace("listen success");

/*------------------------------------------------*/

    for(;;)
    {        
          struct sockaddr_in client_addr;
          socklen_t length = sizeof(client_addr);
    
          int conn = accept(server_sockfd, (struct sockaddr*)&client_addr,&length);        
          if(conn<0)
          {
              perror("connect");
              exit(1);
          }
           log_trace("new client accepted.");
           bIsConnected = 1;
           pthread_mutex_init(&mut,NULL);
           createThread(conn);
           while(1)
           {
                 if(!bIsConnected)
                {
                  g_ringBuf.Clear();
                  destory_mysql(pMySql);
                  printf("The connect is Disconnect! You can Connect a New Socket!\n");
                  break;
                }
                 usleep(1000000);
           }
    }
    pthread_exit(NULL);
    return 0;
}

