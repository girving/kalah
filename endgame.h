// Endgame database class header file
// Geoffrey Irving
// 05sept99

#ifndef __ENDGAME_H
#define __ENDGAME_H

#include <stdio.h>
#include "params.h"
#include "rules.h"

#define EG_LO 1
#define EG_EXACT 3

typedef struct __endgame {
  int n;
  unsigned long size;
  int nm[50];   
  unsigned char* d;

  unsigned long ai[50][50];
  unsigned long bi[50];
  unsigned long ci[LPIT][50];
  unsigned long di[50][50];
  unsigned long si[50];
  } endgame;

extern void read_endgame(endgame *e, FILE *f, int n);
extern void write_endgame(endgame *e, FILE *f);
extern void free_endgame(endgame *e);

extern void eg_init_tables(endgame *e);  
extern unsigned long eg_index(endgame *e, int t, position *p);

static inline int eg_known(endgame *e, int t) { return t <= e->n; }
static inline int eg_lookup(endgame *e, int t, position *p, int *q);

/* inline code */

static inline int eg_getd(endgame *e, unsigned long i) {
  return i&1 ? e->d[i>>1]>>4 : e->d[i>>1] & 15;
  }

static inline void eg_setd(endgame *e, unsigned long i, int v) {
  if (--v > 15) v = 15;
  e->d[i>>1] = i&1 ? e->d[i>>1] & 15 | (v << 4) : e->d[i>>1] & 240 | v;
  }

static inline int eg_lookup(endgame *e, int t, position *p, int *q) { 
  int r = eg_getd(e,eg_index(e,t,p));
  *q = r < 15 ? EG_EXACT : EG_LO;
  return (++r << 1) - t + rate(p);
  }

#endif
    

