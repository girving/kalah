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

struct endgame {
  int n;
  int bits;
  size_t size;
  int nm[50];   
  unsigned char* d;

  size_t ai[50][50];
  size_t bi[50];
  size_t ci[LPIT][50];
  };

struct endgame_header {
  char sig[2];
  char n[5];
  char bits[5];
  char size[20];
  };

extern struct endgame eg;

void read_endgame(FILE *f, int n);
void write_endgame(FILE *f);
void free_endgame(void);

void eg_init_tables(void);
size_t eg_index(int t, position *p);

extern inline int eg_known(int t) { return t <= eg.n; }
extern inline int eg_lookup(int t, position *p, int *q);

/* internal stuff */
extern void eg_create(char *file, int bits);
void eg_init_long_tables(long long a[][100], long long b[], long long c[][100]);

extern inline int eg_maxval(void);
extern inline int eg_getd(size_t i);
extern inline void eg_setd(size_t i, int v);

extern inline int eg_getd_explicit(struct endgame *eg, size_t i);
extern inline void eg_setd_explicit(struct endgame *eg, size_t i, int v);

/* inline code */

extern inline int eg_maxval() {
  return (1<<eg.bits) - 1; 
  }

extern inline int eg_getd_4_explicit(struct endgame *e, size_t i) {
  return i&1 ? e->d[i>>1]>>4 : e->d[i>>1] & 15;
  }

extern inline int eg_getd_5_explicit(struct endgame *e, size_t i) {
  unsigned char *q;
  i *= 5;
  q = e->d + (i>>3);
  return (*q | *(q+1)<<8) >> (i&7) & 31;
  }

extern inline int eg_getd_explicit(struct endgame *e, size_t i) {
  return e->bits < 5 ? eg_getd_4_explicit(e,i) : eg_getd_5_explicit(e,i); 
  }

extern inline int eg_getd(size_t i) {
  return eg_getd_explicit(&eg,i);
  }

extern inline int eg_lookup(int t, position *p, int *q) { 
  int r = eg_getd(eg_index(t,p));
  *q = (r == 15 && eg.bits < 5) ? EG_LO : EG_EXACT; 
  return (++r << 1) - t + rate(*p);
  }

extern inline void eg_setd_4_explicit(struct endgame *e, size_t i, int v) {
  if (v > 15) v = 15;
  e->d[i>>1] = i&1 ? (e->d[i>>1] & 15) | (v << 4) : (e->d[i>>1] & 240) | v;
  }

extern inline void eg_setd_5_explicit(struct endgame *e, size_t i, int v) {
  unsigned char *q;
  int r;
  i *= 5;
  q = e->d + (i>>3);
  r = ((*q | (*(q+1)<<8)) & ~(31 << (i&7))) | (v << (i&7));
  *q = r;
  *(q+1) = r>>8;
  }

extern inline void eg_setd_explicit(struct endgame *e, size_t i, int v) {
  if (e->bits < 5)
    eg_setd_4_explicit(e,i,v);
  else
    eg_setd_5_explicit(e,i,v);
  }

extern inline void eg_setd(size_t i, int v) {
  eg_setd_explicit(&eg,i,v);
  }

#endif
    

