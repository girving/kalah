// Logging functions
// Geoffrey Irving
// 13aug00

#ifndef __MISC_H
#define __MISC_H

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <pvm3.h>

/* variables needed for log entries */
extern FILE *log;
extern int tid;
extern char task[]; 
extern char host[];

void logopen(char *file);
void logblank();
void logentry(char *fmt, ...);
void die(char *fmt, ...);

#endif
