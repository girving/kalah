// Kalah toplevel parallism code
// Geoffrey Irving
// 3nov0

#ifndef __PARALLEL_H
#define __PARALLEL_H

#ifndef PVM        
#define init_parallel()
#define P_INIT()
#define P_PUSH()
#define P_POP()
#define P_CHECK()
#else

#include <time.h>
#include <unistd.h>
#include <pvm3.h>
#include "log.h"
#include "crunch.cilkh"

#define JOB_FREE 0
#define JOB_DONE 1
#define JOB_TAKEN 2
#define JOB_BLOCKED 3
#define JOB_CANCELLED 4

#define TAG_HELLO      1
#define TAG_WORKERS    2
#define TAG_KILL       3
#define TAG_REQUEST    4
#define TAG_JOB        5
#define TAG_NO_JOB     6
#define TAG_CANCEL     7
#define TAG_DONE       8
#define TAG_STAT       9

/* stolen job information */
typedef struct __jobinfo {
  int jid;        // job id 
  int tid,pjid;   // parent process id and job id
  int d,a,n;      // depth / alpha
  c_res s;        // job information
  int status;     // job status
  int o[PITS];    // move ordering
  int js[PITS];   // child status
  int ctid[PITS]; // child tids
  int cjid[PITS]; // child jids
  } jobinfo;

/* info for nonstolen jobs */
typedef struct __stackinfo {
  int d,a,n,i;
  c_res *s,*c;
  c_res **o;
  } stackinfo;

/* message handling routine */
extern void p_handle(int b);

/* stuff used within crunch() */
#define P_STACKDEPTH 200

extern jobinfo *cj;
extern stackinfo p_stack[P_STACKDEPTH];
extern int p_head, p_tail;

#define P_INIT() \
  do { \
    p_stack[p_tail].d = d; \
    p_stack[p_tail].a = a; \
    p_stack[p_tail].n = n; \
    p_stack[p_tail].s = s; \
    p_stack[p_tail].c = c; \
    p_stack[p_tail].o = o; \
    } while (0)

#define P_PUSH()           \
  do {                     \
    p_stack[p_tail].i = i; \
    p_tail++;              \
    } while (0)

#define P_POP()                     \
  do {                              \
    if (p_head > --p_tail) {        \
      p_tail++;                     \
      if (cj->status = JOB_TAKEN) { \
        cj->s = *p_stack[p_tail].s; \
        cj->status = JOB_DONE;      \
        }                           \
      return 0;                     \
      }                             \
    } while (0)

inline void P_CHECK() {
  int b = pvm_nrecv(-1,-1);
  if (b > 0) 
    p_handle(b);
  }

/* initialization and destruction */
extern void init_parallel(int m);   // m = 1 for master, 0 otherwise
extern void close_parallel();

/* main functions */
extern void parallel(c_res* s, int d, int a);   // master
extern void worker();                           // worker 

#endif
#endif
