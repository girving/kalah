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

typedef struct __endgame {
  int n;
  int bits;
  size_t size;
  int nm[50];   
  unsigned char* d;

  size_t ai[50][50];
  size_t bi[50];
  size_t ci[LPIT][50];
  } endgame;

typedef struct __endgame_header {
  char sig[2];
  char n[5];
  char bits[5];
  char size[20];
  } endgame_header;

extern void read_endgame(endgame *e, FILE *f, int n);
extern void write_endgame(endgame *e, FILE *f);
extern void free_endgame(endgame *e);

extern void eg_init_tables(endgame *e);  
extern size_t eg_index(endgame *e, int t, position *p);

static inline int eg_known(endgame *e, int t) { return t <= e->n; }
static inline int eg_lookup(endgame *e, int t, position *p, int *q);

/* internal stuff */
extern void eg_create(char *file, int bits);
void eg_init_long_tables(long long a[][100], long long b[], long long c[][100]);

static inline int eg_maxval(endgame *e);
static inline int eg_getd(endgame *e, size_t i);
static inline void eg_setd(endgame *e, size_t i, int v);

/* inline code */

static inline int eg_maxval(endgame *e) {
  return (1<<e->bits) - 1; 
  }

static inline int eg_getd_4(endgame *e, size_t i) {
  return i&1 ? e->d[i>>1]>>4 : e->d[i>>1] & 15;
  }

static inline int eg_getd_5(endgame *e, size_t i) {
  unsigned char *q;
  i *= 5;
  q = e->d + (i>>3);
  return (*q | *(q+1)<<8) >> (i&7) & 31;
  }

static inline int eg_getd(endgame *e, size_t i) {
  return e->bits < 5 ? eg_getd_4(e,i) : eg_getd_5(e,i); 
  }

static inline int eg_lookup(endgame *e, int t, position *p, int *q) { 
  int r = eg_getd(e,eg_index(e,t,p));
  *q = (r == 15 && e->bits < 5) ? EG_LO : EG_EXACT; 
  return (++r << 1) - t + rate(*p);
  }

static inline void eg_setd_4(endgame *e, size_t i, int v) {
  if (v > 15) v = 15;
  e->d[i>>1] = i&1 ? e->d[i>>1] & 15 | (v << 4) : e->d[i>>1] & 240 | v;
  }

static inline void eg_setd_5(endgame *e, size_t i, int v) {
  unsigned char *q;
  int r;
  i *= 5;
  q = e->d + (i>>3);
  r = (*q | *(q+1)<<8) & ~(31 << (i&7)) | v << (i&7);
  *q = r;
  *(q+1) = r>>8;
  }

static inline void eg_setd(endgame *e, size_t i, int v) {
  if (e->bits < 5)
    eg_setd_4(e,i,v);
  else
    eg_setd_5(e,i,v);
  }

#endif
    

