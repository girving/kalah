// Automatic serial C elision header file for cilk 
// Geoffrey Irving
// 8aug0

#ifndef __ELISION_H
#define __ELISION_H

#include <stdlib.h>
#include <unistd.h>

#ifndef NOCILK
#define CILK
#else

/* turn cilk keywords to whitespace */
#define cilk
#define spawn
#define sync
#define inlet
#define abort

/* handle global variables */
#define Cilk_active_size 1
#define Self 0

/* handle timers */
#include <sys/time.h>
typedef double Cilk_time;
#define Cilk_wall_time_to_sec(a) (a)
static inline Cilk_time Cilk_get_wall_time() {
  struct timeval t;
  gettimeofday(&t,NULL);
  return t.tv_sec + t.tv_usec / 1000000.0;
  }

#endif
#endif
