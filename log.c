// Logging functions
// Geoffrey Irving
// 13aug00

#include "log.h"

FILE *log = stderr;
int tid;
char host[50];

void logopen(char *file) {
  log = fopen(file,"a");
  if (!log) {
    log = stderr;
    die("Cannot open log file\n");
    }
  }

void logheader() {
  char c[100];
  time_t t = time(NULL);
  strftime(c,40,"%b %d %T",gmtime(&t));
  fprintf(log,"%s %s %s[%d]: ",c,host,task,tid);
  }

void logblank() {
  putc('\n',log);
  }

void logentry(char *fmt, ...) {
  va_list ap;
  logheader();  
  va_start(ap,fmt);
  vfprintf(log,fmt,ap);
  va_end(ap);
  fflush(log);
  }

void die(char *fmt, ...) {
  if (fmt) {
    va_list ap;
    logheader();
    fputs("Fatal: ",log);
    vfprintf(log,fmt,ap);
    va_end(ap);
    }
  fclose(log);
  pvm_exit();
  exit(-1);
  }
