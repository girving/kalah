// Endgame database class header file
// Geoffrey Irving
// 05sept99

#ifndef __ENDGAME_H
#define __ENDGAME_H

#include <stdio.h>
#include <limits.h>
#include "params.h"
#include "rules.h"

/* Optional parameters */
//#define LONG_INDEXES

#define EG_LO 1
#define EG_EXACT 3

#if defined(LONG_INDEXES) && ULONG_MAX == 0xffffffffL
typedef long long index_t;
#else
typedef unsigned long index_t;
#endif

typedef struct __endgame {
  int n;
  index_t size;
  int nm[50];   
  unsigned char* d;

  index_t ai[50][50];
  index_t bi[50];
  index_t ci[LPIT][50];
  index_t di[50][50];
  index_t si[50];
  } endgame;

extern void read_endgame(endgame *e, FILE *f, int n);
extern void write_endgame(endgame *e, FILE *f);
extern void free_endgame(endgame *e);

extern void eg_init_tables(endgame *e);  
extern index_t eg_index(endgame *e, int t, position *p);

static inline int eg_known(endgame *e, int t) { return t <= e->n; }
static inline int eg_lookup(endgame *e, int t, position *p, int *q);

/* inline code */

static inline int eg_getd(endgame *e, index_t i) {
  return i&1 ? e->d[i>>1]>>4 : e->d[i>>1] & 15;
  }

static inline void eg_setd(endgame *e, index_t i, int v) {
  if (--v > 15) v = 15;
  e->d[i>>1] = i&1 ? e->d[i>>1] & 15 | (v << 4) : e->d[i>>1] & 240 | v;
  }

static inline int eg_lookup(endgame *e, int t, position *p, int *q) { 
  int r = eg_getd(e,eg_index(e,t,p));
  *q = r < 15 ? EG_EXACT : EG_LO;
  return (++r << 1) - t + rate(p);
  }

#endif
    

