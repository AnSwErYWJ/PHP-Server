/*************************************************************************
> File Name: Moon.c
> Author: AnSwEr
> Mail: 1045837697@qq.com
> Created Time: 2015年09月17日 星期四 19时11分05秒
************************************************************************/

/*
* moon服务器主程序
*/

#include <stdio.h>
#include "function.h"
#include <sys/socket.h>
#include <strings.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/types.h>
#include <signal.h>

static int ser_sockfd,cli_sockfd;

/*
 * listen()的包裹函数，可以自定义backlog
 */
static void Listen(int fd,int backlog)
{
    char *ptr;

    if((ptr = getenv("LISTENQ")) != NULL)
        backlog = atoi(ptr);

    ret = listen(fd,backlog);
    if(ret == -1)
        Debug("Error:listen()\n");
}

/*退出函数*/
void quit(int signo)
{
    close(ser_sockfd);
    exit(EXIT_SUCCESS);
}

int main(int argc,const char *argv[])
{
    struct sockaddr_in ser_addr,cli_addr;
    char buf[MAXSIZE];
    socklen_t cli_addr_size;
    int thread_count = 0;
    pthread_t threads[MAX_THREAD_NUM];
    char ipaddress[20];
    FILE *fp_log;
    time_t t_log;   

    /*get server sockfd*/
    ser_sockfd = socket(PF_INET,SOCK_STREAM,0);
    if(ser_sockfd == -1)
        Debug("Error:socket()\n");

    /*quit signal*/
    signal(SIGINT,quit);

    /*configure ser_addr*/
    bzero(&ser_addr,sizeof(ser_addr));
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    ser_addr.sin_port = htons(PORT);

    /*bind ser_sockfd*/
    ret = bind(ser_sockfd,(struct sockaddr*)&ser_addr,sizeof(ser_addr));
    if(ret == -1)
        Debug("Error:bind()\n");

    /*listen*/
    Listen(ser_sockfd,5);

    while(1)
    {
        /*accept*/
        cli_addr_size = sizeof(cli_addr);
        bzero(&cli_addr,cli_addr_size);
       
        cli_sockfd = accept(ser_sockfd,(struct sockaddr *)&cli_addr,&cli_addr_size);
        if(cli_sockfd == -1)
        {
            Debug("Error:accept()\n");
        }
        
        /*client log*/
        fp_log = fopen("log/cli_log.txt","a+");
        if(fp_log == NULL)
        {
            Debug("Error:fopen()\n");
        }
        
        time(&t_log);
        fputs(ctime(&t_log),fp_log);
        inet_ntop(AF_INET,(void *)&cli_addr.sin_addr,ipaddress,16);
        fprintf(fp_log,"ipaddress:%s port:%d\n",ipaddress,cli_addr.sin_port);
        fclose(fp_log);
       
        /*多线程*/
        ret = pthread_create(threads+(thread_count++),NULL,(void *)handleRequest,&cli_sockfd);
        if(ret != 0)
        {
            Debug("Error:pthread_create\n");
        }
    }

    /*close fd*/
    close(ser_sockfd);

    return EXIT_SUCCESS;
}
