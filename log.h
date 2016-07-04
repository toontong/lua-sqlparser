#ifndef _LOG_H_HREAD_
#define _LOG_H_HREAD_ 1

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <stdarg.h>

#define LOG_ERR     0
#define LOG_NORMAL  1
#define LOG_DEBUG   2
#define LOG_LEVEL   LOG_DEBUG

#define LOG_OUTPUT stdout

void _log(int level, const char *format, ...);
// get current date time with format yyyy-MM-dd hh:mm:ss
// int currenttime(char * currTime);

#define Debug(...) _log(LOG_DEBUG, __VA_ARGS__);


#endif
