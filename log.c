#include "log.h"
int currenttime(char * currTime)
{
    struct tm *ptm = NULL;
    time_t tme;
    tme = time(NULL);
    ptm = localtime(&tme);
    char szTime[256];
    memset(szTime, 0, 256);
    sprintf(szTime, "[%d-%02d-%02d %02d:%02d:%02d] ", (ptm->tm_year + 1900),
        ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
    strcpy(currTime, szTime);
    return 0;
}

void _log(int level, const char *format, ...){
    if (level < LOG_LEVEL) 
        return ;

    char temp[1024];
    memset(temp, 0, 1024);

    va_list args;
    va_start(args, format );
    vsprintf(temp, format, args );
    va_end( args );
    
    char currTime[256];
    memset(currTime, 0, 256);
    currenttime(currTime);

    fputs(currTime, LOG_OUTPUT);
    fputs(temp, LOG_OUTPUT);
    fputs("\n", LOG_OUTPUT);
} 
